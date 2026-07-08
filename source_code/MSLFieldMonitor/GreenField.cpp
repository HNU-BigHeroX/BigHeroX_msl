// GreenField.cpp : implementation file
//

#include "stdafx.h"
#include "MSLFieldMonitor.h"
#include "GreenField.h"
#include "MSL_Structs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static WCHAR wcInfo[1024];

// 从资源文件加载图片
BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR, Image *&pImg)
{
    HINSTANCE hInst = AfxGetResourceHandle();
    HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(nID), sTR); // type
    if (!hRsrc)
        return FALSE;
    // load resource into memory
    DWORD len = SizeofResource(hInst, hRsrc);
    BYTE *lpRsrc = (BYTE *)LoadResource(hInst, hRsrc);
    if (!lpRsrc)
        return FALSE;
    // Allocate global memory on which to create stream
    HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
    BYTE *pmem = (BYTE *)GlobalLock(m_hMem);
    memcpy(pmem, lpRsrc, len);
    IStream *pstm;
    CreateStreamOnHGlobal(m_hMem, FALSE, &pstm);
    // load from stream
    pImg = Gdiplus::Image::FromStream(pstm);
    // free/release stuff
    GlobalUnlock(m_hMem);
    pstm->Release();
    FreeResource(lpRsrc);
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGreenField

CGreenField::CGreenField()
{
    BOOL res;
    res = ImageFromIDResource(IDB_FIELD_LR, "PNG", m_imgFieldLR);
    res = ImageFromIDResource(IDB_FIELD_RL, "PNG", m_imgFieldRL);
    res = ImageFromIDResource(IDB_BTN_FLIP, "PNG", m_imgFlip);
    res = ImageFromIDResource(IDB_BTN_SETPLAYERIN, "PNG", m_imgSetPlayerIn);
    res = ImageFromIDResource(IDB_BTN_SHOWOBST, "PNG", m_imgShowOrgObst);
    res = ImageFromIDResource(IDB_BTN_SHOWOBSTDEF, "PNG", m_imgObstNumDef);
    res = ImageFromIDResource(IDB_BTN_SHOWENEMYTRACK, "PNG", m_imgEnemyTrack);

    pDataSummary = NULL;
    pCoachUDP = NULL;
    pDataCenter = NULL;

    m_nSelAgentID = 0;

    // 地图翻转
    m_bLeftToRight = true;
    m_rcFlip.X = m_imgFieldLR->GetWidth() - 16;
    m_rcFlip.Y = 0;
    m_rcFlip.Width = 16;
    m_rcFlip.Height = 16;

    // 设置入场点
    m_bSetPlayerIn = false;
    m_rcSetPlayerIn.X = m_imgFieldLR->GetWidth() - 24;
    m_rcSetPlayerIn.Y = m_imgFieldLR->GetHeight() - 24;
    m_rcSetPlayerIn.Width = 24;
    m_rcSetPlayerIn.Height = 24;

    // 显示原始障碍物点
    m_bShowOrgObst = false;
    m_rcShowOrgObst.X = 0; // m_imgFieldLR->GetWidth() - 32;
    m_rcShowOrgObst.Y = 0;
    m_rcShowOrgObst.Width = 16;
    m_rcShowOrgObst.Height = 16;

    // 显示障碍物追踪状态
    m_bShowEnemyTrack = true;
    m_rcShowEnemyTrack.X = 0;
    m_rcShowEnemyTrack.Y = 32;
    m_rcShowEnemyTrack.Width = 16;
    m_rcShowEnemyTrack.Height = 16;

    // 障碍物显示数字模式
    m_nObstNumMode = OBST_NUM_BACK_DEF;
    // m_nObstNumMode = OBST_NUM_CLOSETO_OWNGOAL;

    // 设置显示防守顺序
    m_rcObstNumDef.X = 0;
    m_rcObstNumDef.Y = 16;
    m_rcObstNumDef.Width = 16;
    m_rcObstNumDef.Height = 16;

    m_nAgentTestMode = AGENT_TEST_NONE;
    m_nAgentTestDefDist = 200;
}

CGreenField::~CGreenField()
{
    delete m_imgFieldLR;
    delete m_imgFieldRL;

    delete m_imgFlip;
    delete m_imgSetPlayerIn;
}

BEGIN_MESSAGE_MAP(CGreenField, CStatic)
//{{AFX_MSG_MAP(CGreenField)
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_RBUTTONDOWN()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGreenField message handlers

void CGreenField::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // TODO: Add your message handler code here
    // 【1】创建内存dc并撑开******************************************
    dcMemory.CreateCompatibleDC(&dc);

    // GetClientRect(&m_FrameRect);
    m_FrameRect.top = 0;
    m_FrameRect.left = 0;
    m_FrameRect.bottom = m_FrameRect.top + m_imgFieldLR->GetHeight();
    m_FrameRect.right = m_FrameRect.left + m_imgFieldLR->GetWidth();

    m_BkBmp.CreateCompatibleBitmap(&dc, m_FrameRect.right, m_FrameRect.bottom);
    CBitmap *oldbmp = dcMemory.SelectObject(&m_BkBmp);

    // 【2】贴场地背景图******************************************
    Graphics m_graphScreen(dcMemory.m_hDC);
    m_graphScreen.SetSmoothingMode(SmoothingModeHighQuality);

    m_scrImg.X = 0;
    m_scrImg.Y = 0;
    m_scrImg.Width = (REAL)m_imgFieldLR->GetWidth();
    m_scrImg.Height = (REAL)m_imgFieldLR->GetHeight();
    if (m_bLeftToRight == true)
    {
        m_graphScreen.DrawImage(m_imgFieldLR, m_scrImg);
    }
    else
    {
        m_graphScreen.DrawImage(m_imgFieldRL, m_scrImg);
    }
    Point coordinate;
    Point posObst;

    RectF rcTxt;
    CString strTmp;
    Gdiplus::Font fontTacInfo(L"宋体", 12, FontStyleBold);
    Point pntBegin;
    Point pntEnd;
    Pen penYellow(Color(255, 255, 255, 0), 2);
    SolidBrush brRed(Color(155, 255, 0, 0));
    SolidBrush brYellow(Color(155, 255, 255, 0));
    SolidBrush brBlue(Color(50, 0, 100, 255));

    // 有队员带球时，填充带球范围（以触球点为圆心）
    if (pDataSummary->bHoldBall == true)
    {
        int range = 100;
        Point pntTouchBall = m_Point_Field2Img(pDataSummary->posTouchBall.x, pDataSummary->posTouchBall.y);
        m_graphScreen.FillEllipse(&brBlue, pntTouchBall.X - range, pntTouchBall.Y - range, range * 2, range * 2);
    }

    // 绘制入场点(匹配起始点)
    if (pDataSummary->posPlayerIn.x > 0)
    {
        pntBegin = m_Point_Field2Img(pDataSummary->posPlayerIn.x - 20, pDataSummary->posPlayerIn.y);
        pntEnd = m_Point_Field2Img(pDataSummary->posPlayerIn.x + 22, pDataSummary->posPlayerIn.y);
    }
    else
    {
        pntBegin = m_Point_Field2Img(pDataSummary->posPlayerIn.x - 22, pDataSummary->posPlayerIn.y);
        pntEnd = m_Point_Field2Img(pDataSummary->posPlayerIn.x + 20, pDataSummary->posPlayerIn.y);
    }
    m_graphScreen.DrawLine(&penYellow, pntBegin, pntEnd);
    if (pDataSummary->posPlayerIn.y > 0)
    {
        pntBegin = m_Point_Field2Img(pDataSummary->posPlayerIn.x, pDataSummary->posPlayerIn.y - 20);
        pntEnd = m_Point_Field2Img(pDataSummary->posPlayerIn.x, pDataSummary->posPlayerIn.y + 22);
    }
    else
    {
        pntBegin = m_Point_Field2Img(pDataSummary->posPlayerIn.x, pDataSummary->posPlayerIn.y - 22);
        pntEnd = m_Point_Field2Img(pDataSummary->posPlayerIn.x, pDataSummary->posPlayerIn.y + 20);
    }
    m_graphScreen.DrawLine(&penYellow, pntBegin, pntEnd);

    if (m_bSetPlayerIn == true)
    {
        strTmp = "请设置入场点";
        rcTxt.X = 550;
        rcTxt.Y = 480;
        MBToWC(strTmp, wcInfo);
        m_graphScreen.DrawString(wcInfo,
                                 -1, // wcslen(strTmp),
                                 &fontTacInfo, rcTxt, NULL, &brRed);
    }

    // 【3】绘制势场图******************************************

    // 【4】绘制中间层信息******************************************

    // 【5】绘制机器人******************************************
    int nRofRobot = 10;
    SolidBrush sbrRobot(Color(255, 0, 0, 255));
    SolidBrush sbrErr(Color(255, 100, 100, 100));
    int nRofBall = 8;
    SolidBrush sbrGlobalBall(Color(255, 230, 0, 0));

    Pen penRobotDir(Color(255, 255, 0, 100), 3);
    Pen penMovePath(Color(155, 255, 255, 0), 2);
    penMovePath.SetDashStyle(DashStyleDash);
    Pen penZPPath(Color(155, 255, 255, 255), 1);
    penZPPath.SetDashStyle(DashStyleDot);
    Pen penPassPath(Color(155, 0, 0, 255), 1);
    penPassPath.SetDashStyle(DashStyleDash);
    Pen penKickPath(Color(255, 255, 0, 0), 1);
    penKickPath.SetDashStyle(DashStyleDash);
    Pen penCatchPath(Color(155, 255, 255, 255), 1);
    penCatchPath.SetDashStyle(DashStyleDash);
    Pen penOutline(Color(255, 255, 255, 0), 3);

    Pen penEnemy(Color(200, 255, 0, 0), 1);
    REAL dashVals[2] = {5.0f, 10.0f};
    penEnemy.SetDashPattern(dashVals, 2);
    // penEnemy.SetDashOffset(70);

    Pen penTeammate(Color(200, 0, 0, 255), 1);
    penTeammate.SetDashPattern(dashVals, 2);
    // penTeammate.SetDashOffset(70);

    Point direct;
    double dAngle = 0;
    uAgent *pAgent = pDataSummary->agent;
    for (size_t i = 0; i < 5; i++)
    {
        if (pAgent[i].online == true)
        {
            // 绘制在线机器人
            coordinate = m_Point_Field2Img(pAgent[i].x, pAgent[i].y);

            // 为动态传球队员绘制传球路径
            if (pAgent[i].status == CTRL_PASS_MOVE)
            {
                if (pAgent[i].dest_x != 0 && pAgent[i].dest_y != 0)
                {
                    coordinate = m_Point_Field2Img(pAgent[i].x, pAgent[i].y);
                    direct = m_Point_Field2Img(pAgent[i].dest_x, pAgent[i].dest_y);
                    m_graphScreen.DrawLine(&penPassPath, direct, coordinate);
                    posKickInImage = coordinate;
                }
            }

            if (pAgent[i].status == CTRL_PASS)
            {
                if (pAgent[i].dest_x != 0 && pAgent[i].dest_y != 0)
                {
                    coordinate = m_Point_Field2Img(pAgent[i].x, pAgent[i].y);
                    posKickInImage = coordinate;
                }
            }

            // 绘制MOVETO路径
            if ((pAgent[i].x != pAgent[i].dest_x || pAgent[i].y != pAgent[i].dest_y) && pAgent[i].status == CTRL_MOVETO)
            {
                Point moveto = m_Point_Field2Img(pAgent[i].dest_x, pAgent[i].dest_y);
                m_graphScreen.DrawLine(&penMovePath, moveto, coordinate);
            }

            // 绘制防传球的目标
            if ((pAgent[i].x != pAgent[i].dest_x || pAgent[i].y != pAgent[i].dest_y) &&
                pAgent[i].status == CTRL_DEF_BALL)
            {
                Point moveto = m_Point_Field2Img(pAgent[i].dest_x, pAgent[i].dest_y);
                m_graphScreen.DrawLine(&penZPPath, coordinate, moveto);
            }

            // 绘制防射门的目标
            if ((pAgent[i].x != pAgent[i].dest_x || pAgent[i].y != pAgent[i].dest_y) &&
                pAgent[i].status == CTRL_DEF_GOAL)
            {
                Point moveto = m_Point_Field2Img(pAgent[i].dest_x, pAgent[i].dest_y);
                m_graphScreen.DrawLine(&penZPPath, coordinate, moveto);
            }

            // 为动态接球队员绘制接球点
            if ((pAgent[i].x != pAgent[i].dest_x || pAgent[i].y != pAgent[i].dest_y) &&
                (pAgent[i].status == CTRL_CATCH_MOVE || pAgent[i].status == CTRL_CATCH_FOCUS))
            {
                Point moveto = m_Point_Field2Img(pAgent[i].dest_x, pAgent[i].dest_y);
                m_graphScreen.DrawLine(&penCatchPath, coordinate, moveto);
            }

            // 填充机器人图标本体
            if (pAgent[i].status != ROBST_ERR)
            {
                m_graphScreen.FillEllipse(&sbrRobot, coordinate.X - nRofRobot, coordinate.Y - nRofRobot, nRofRobot * 2,
                                          nRofRobot * 2);
            }
            else
            {
                m_graphScreen.FillEllipse(&sbrErr, coordinate.X - nRofRobot, coordinate.Y - nRofRobot, nRofRobot * 2,
                                          nRofRobot * 2);
            }

            // 如果是点选目标，则将外围线设为黄色
            if (pAgent[i].ID == m_nSelAgentID)
            {
                m_graphScreen.DrawEllipse(&penOutline, coordinate.X - nRofRobot, coordinate.Y - nRofRobot,
                                          nRofRobot * 2, nRofRobot * 2);
            }

            // 机器人朝向角度
            dAngle = pAgent[i].angle;
            if (m_bLeftToRight == false)
            {
                dAngle = 180 - dAngle;
                dAngle = -dAngle;
            }
            direct.X = (long)(15 * cos(dAngle * 3.14 / 180));
            direct.Y = (long)(-15 * sin(dAngle * 3.14 / 180));
            direct = direct + coordinate;
            m_graphScreen.DrawLine(&penRobotDir, coordinate, direct);

            // 机器人编号
            Gdiplus::Font fontID(L"宋体", 13, FontStyleBold);
            rcTxt.X = (REAL)coordinate.X - 7;
            rcTxt.Y = (REAL)coordinate.Y - 9;
            rcTxt.Width = 100;
            rcTxt.Height = 16;
            SolidBrush CyanBrush(Color(255, 0, 255, 255));
            strTmp.Format("%d", pAgent[i].ID);
            MBToWC(strTmp, wcInfo);
            m_graphScreen.DrawString(wcInfo,
                                     -1, // wcslen(strTmp),
                                     &fontID, rcTxt, NULL, &CyanBrush);

            // 绘制机器人行为
            Gdiplus::Font fontName(L"宋体", 10);
            rcTxt.X = (REAL)coordinate.X - nRofRobot - 5;
            rcTxt.Y = (REAL)coordinate.Y - nRofRobot - 14;
            rcTxt.Width = 100;
            rcTxt.Height = 16;
            SolidBrush whiteBrush(Color(255, 255, 255, 255));
            strTmp = GetCTRLString(pAgent[i].status);
            MBToWC(strTmp, wcInfo);
            m_graphScreen.DrawString(wcInfo,
                                     -1, // wcslen(strTmp),
                                     &fontName, rcTxt, NULL, &whiteBrush);
        }
    }

    // 【6】绘制障碍物******************************************
    SolidBrush sbrObstacle(Color(40, 0, 0, 0));
    int nSizeOfObst = 0;
    Pen penObstSeen(Color(30, 0, 0, 0), 2);
    penObstSeen.SetDashStyle(DashStyleDash);

    // 队员各自的障碍物
    if (m_bShowOrgObst == true)
    {
        for (size_t i = 0; i < 5; i++)
        {
            if (pAgent[i].online == true)
            {
                for (size_t j = 0; j < 10; j++)
                {
                    if (pAgent[i].obstacle[j].valid == true)
                    {
                        posObst = m_Point_Field2Img(pAgent[i].obstacle[j].x, pAgent[i].obstacle[j].y); // 障碍物坐标
                        coordinate = m_Point_Field2Img(pAgent[i].x, pAgent[i].y);                      // 机器人坐标
                        nSizeOfObst = pAgent[i].obstacle[j].size;                                      // 障碍物尺寸

                        m_graphScreen.FillEllipse( // 填充障碍物
                            &sbrObstacle, posObst.X - nSizeOfObst, posObst.Y - nSizeOfObst, nSizeOfObst * 2,
                            nSizeOfObst * 2);
                        m_graphScreen.DrawLine(&penObstSeen, coordinate, posObst); // 绘制障碍物到目击者的连线
                    }
                }
            }
        }
    }

    // 预处理后的障碍物
    for (i = 0; i < FULL_OST_NUM; i++)
        if (pDataSummary->pre_obstacle[i].valid == true)
        {
            posObst = m_Point_Field2Img(pDataSummary->pre_obstacle[i].x, pDataSummary->pre_obstacle[i].y); // 障碍物坐标
            coordinate = m_Point_Field2Img(pAgent[pDataSummary->pre_obstacle[i].ID - 1].x,
                                           pAgent[pDataSummary->pre_obstacle[i].ID - 1].y); // 机器人坐标
            nSizeOfObst = 3;                                                                // 障碍物尺寸
            m_graphScreen.FillEllipse(                                                      // 填充障碍物
                &sbrObstacle, posObst.X - nSizeOfObst, posObst.Y - nSizeOfObst, nSizeOfObst * 2, nSizeOfObst * 2);
            // m_graphScreen.DrawLine(&penObstSeen, coordinate, posObst); //绘制障碍物到目击者的连线
        }

    // 融合后的障碍物
    SolidBrush sbrObstFussion(Color(150, 155, 0, 0));
    for (i = 0; i < pDataSummary->NumOfemy; i++)
    {
        posObst = m_Point_Field2Img(pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y); // 障碍物坐标
        nSizeOfObst = pDataSummary->t_enemy[i].size;                                         // 障碍物尺寸
        m_graphScreen.FillEllipse(                                                           // 填充障碍物
            &sbrObstFussion, posObst.X - nSizeOfObst, posObst.Y - nSizeOfObst, nSizeOfObst * 2, nSizeOfObst * 2);

        if (m_nObstNumMode == OBST_NUM_CLOSETO_OWNGOAL)
        {
            // 显示障碍物的index（便于判断障碍物的排序）
            Gdiplus::Font fontName(L"宋体", 12);
            rcTxt.X = (REAL)posObst.X - 6;
            rcTxt.Y = (REAL)posObst.Y - 8;
            rcTxt.Width = 20;
            rcTxt.Height = 20;
            SolidBrush obstBrush(Color(255, 255, 255, 255));
            strTmp.Format("%d", i);
            MBToWC(strTmp, wcInfo);
            m_graphScreen.DrawString(wcInfo,
                                     -1, // wcslen(strTmp),
                                     &fontName, rcTxt, NULL, &obstBrush);
        }
        if (m_nObstNumMode == OBST_NUM_BACK_DEF)
        {
            if (pDataSummary->nNumOfObstBackDef > 0)
            {
                for (int j = 0; j < pDataSummary->nNumOfObstBackDef; j++)
                {
                    if (pDataSummary->arObstIndexBackDef[j] == i)
                    {
                        Gdiplus::Font fontName(L"宋体", 12);
                        rcTxt.X = (REAL)posObst.X - 6;
                        rcTxt.Y = (REAL)posObst.Y - 8;
                        rcTxt.Width = 20;
                        rcTxt.Height = 20;
                        SolidBrush obstBrush(Color(255, 0, 255, 0));
                        strTmp.Format("%d", j);
                        MBToWC(strTmp, wcInfo);
                        m_graphScreen.DrawString(wcInfo, wcslen(wcInfo), &fontName, rcTxt, NULL, &obstBrush);
                    }
                }
            }
        }
    }

    // 看不到球时，绘制疑似带球障碍物坐标
    if (pDataSummary->bNoOneSeeBall == true)
    {
        Point pntBall = m_Point_Field2Img(pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);
        coordinate = m_Point_Field2Img(pDataSummary->nObstHoldBallInNoSeen_x, pDataSummary->nObstHoldBallInNoSeen_y);
        m_graphScreen.DrawLine(&penEnemy, pntBall, coordinate);
        int tmpR = 12;
        m_graphScreen.DrawEllipse(&penEnemy, coordinate.X - tmpR, coordinate.Y - tmpR, tmpR * 2, tmpR * 2);
    }

    // 绘制球滚向的障碍物
    if (pDataSummary->nNumOfPassToEnemy > 0)
    {
        Point pntBall = m_Point_Field2Img(pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);
        coordinate = m_Point_Field2Img(pDataSummary->posPassToEnemy[0].x, pDataSummary->posPassToEnemy[0].y);
        m_graphScreen.DrawLine(&penEnemy, pntBall, coordinate);
        int tmpR = 20;
        m_graphScreen.DrawEllipse(&penEnemy, coordinate.X - tmpR, coordinate.Y - tmpR, tmpR * 2, tmpR * 2);
    }

    // 绘制球滚向的队友
    if (pDataSummary->nNumOfPassToTeammate > 0)
    {
        Point pntBall = m_Point_Field2Img(pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);
        coordinate = m_Point_Field2Img(pDataSummary->posPassToTeammate.x, pDataSummary->posPassToTeammate.y);
        m_graphScreen.DrawLine(&penTeammate, pntBall, coordinate);
        int tmpR = 20;
        m_graphScreen.DrawEllipse(&penTeammate, coordinate.X - tmpR, coordinate.Y - tmpR, tmpR * 2, tmpR * 2);
    }

    // 绘制障碍物跟踪效果
    if (pObstFusion != NULL && m_bShowEnemyTrack == true)
    {
        if (pObstFusion->nNumTrackEnemy > 0)
        {
            for (int i = 0; i < pObstFusion->nNumTrackEnemy; i++)
            {
                coordinate = m_Point_Field2Img(pObstFusion->arTrackEnemy[i].x, pObstFusion->arTrackEnemy[i].y);
                // 绘制锁定圈
                int tmpR = 12;
                m_graphScreen.DrawEllipse(&penTeammate, coordinate.X - tmpR, coordinate.Y - tmpR, tmpR * 2, tmpR * 2);
                // 绘制计数信息
                Gdiplus::Font fontName(L"宋体", 8);
                rcTxt.X = (REAL)coordinate.X - 10; // 6;
                rcTxt.Y = (REAL)coordinate.Y - 20; // 8;
                rcTxt.Width = 40;
                rcTxt.Height = 20;
                SolidBrush obstBrush(Color(255, 255, 255, 0));
                strTmp.Format("%d", pObstFusion->arTrackCount[i]);
                MBToWC(strTmp, wcInfo);
                m_graphScreen.DrawString(wcInfo,
                                         -1, // wcslen(strTmp),
                                         &fontName, rcTxt, NULL, &obstBrush);
            }
        }
    }

    // 【7】绘制球******************************************
    // 绘制全局球
    if (pDataSummary->bNoOneSeeBall == false)
    {
        coordinate = m_Point_Field2Img(pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);
        m_graphScreen.FillEllipse(&sbrGlobalBall, coordinate.X - nRofBall, coordinate.Y - nRofBall, nRofBall * 2,
                                  nRofBall * 2);
    }

    // 【8】文字标示******************************************
    //  策略状态信息
    rcTxt.X = 15;
    rcTxt.Y = 5;
    rcTxt.Width = 500;
    rcTxt.Height = 20;
    SolidBrush InfoBrush(Color(255, 255, 255, 0));
    MBToWC(pDataCenter->pCurTactic->strTacticInfo, wcInfo);
    m_graphScreen.DrawString(wcInfo,
                             -1, // wcslen(pDataCenter->pCurTactic->strTacticInfo),
                             &fontTacInfo, rcTxt, NULL, &InfoBrush);
    // 开始后读秒
    if (pDataSummary->nStartTimeCountSec < 11)
    {
        rcTxt.X = 550;
        SolidBrush StartSecBrush(Color(255, 255, 0, 0));
        CString strStartSec;
        strStartSec.Format("开始计时 %d 秒", pDataSummary->nStartTimeCountSec);
        MBToWC(strStartSec, wcInfo);
        m_graphScreen.DrawString(wcInfo,
                                 -1, // wcslen(strStartSec),
                                 &fontTacInfo, rcTxt, NULL, &StartSecBrush);
    }
    // 传球过程中读秒（传球超时）
    if (pDataSummary->bPassing == true)
    {
        rcTxt.X = 550;
        rcTxt.Y += 20;
        SolidBrush KickSecBrush(Color(255, 255, 100, 0));
        CString strStartSec;
        strStartSec.Format("传球计时 %d 秒", pDataSummary->nPassKickTimeCountSec);
        MBToWC(strStartSec, wcInfo);
        m_graphScreen.DrawString(wcInfo,
                                 -1, // wcslen(strStartSec),
                                 &fontTacInfo, rcTxt, NULL, &KickSecBrush);

        // 传球路径1（教练机指派的）
        coordinate = m_Point_Field2Img(pDataSummary->nCatchBeforKick_x, pDataSummary->nCatchBeforKick_y);
        m_graphScreen.DrawLine(&penPassPath, posKickInImage, coordinate);
        // 传球路径2（传球队员返回的）
        coordinate = m_Point_Field2Img(pDataSummary->posCatch.x, pDataSummary->posCatch.y);
        m_graphScreen.DrawLine(&penKickPath, posKickInImage, coordinate);
    }
    // 文字输出入场点坐标
    /*rcTxt.X = 400;
    CString strPlayerIn;
    strPlayerIn.Format("入场点(%d,%d)", pDataSummary->posPlayerIn.x, pDataSummary->posPlayerIn.y);
    MBToWC(strPlayerIn,wcInfo);
    m_graphScreen.DrawString(
            wcInfo,
    wcslen(strPlayerIn),
    &fontTacInfo,
    rcTxt,
    NULL,
    &InfoBrush
    );*/

    // 【9】画切换图标
    // m_graphScreen.FillRectangle(&brRed, m_rcFlip);
    // m_graphScreen.FillRectangle(&brRed, m_rcSetPlayerIn);

    ImageAttributes imAtt;
    imAtt.SetWrapMode(WrapModeTile); // 拉伸图片

    m_graphScreen.DrawImage(m_imgFlip, m_rcFlip, 0, 0, m_rcFlip.Width, m_rcFlip.Height, UnitPixel, &imAtt);

    m_graphScreen.DrawImage(m_imgSetPlayerIn, m_rcSetPlayerIn, 0, 0, m_rcSetPlayerIn.Width, m_rcSetPlayerIn.Height,
                            UnitPixel, &imAtt);

    m_graphScreen.DrawImage(m_imgShowOrgObst, m_rcShowOrgObst, 0, 0, m_rcShowOrgObst.Width, m_rcShowOrgObst.Height,
                            UnitPixel, &imAtt);

    m_graphScreen.DrawImage(m_imgObstNumDef, m_rcObstNumDef, 0, 0, m_rcObstNumDef.Width, m_rcObstNumDef.Height,
                            UnitPixel, &imAtt);

    m_graphScreen.DrawImage(m_imgEnemyTrack, m_rcShowEnemyTrack, 0, 0, m_rcShowEnemyTrack.Width,
                            m_rcShowEnemyTrack.Height, UnitPixel, &imAtt);

    /////////////
    // 测试字符串显示
    /*CString strInfo = "你好";
    SolidBrush txtBrush(Color(255, 255, 100, 0));
    rcTxt.X = 100;
    rcTxt.Y = 50;
    int a = 10;
    //swprintf(wcInfo,L"你好%d", a);

    //多字节CString 转换到 WCHAR进行显示
    int len = MultiByteToWideChar(CP_ACP, 0, strInfo, -1, NULL, 0);  // 先取得转换后的UNICODE字符串所需的长度
    MultiByteToWideChar(CP_ACP, 0, strInfo, -1, wcInfo, len);    // 开始转换

    m_graphScreen.DrawString(
            wcInfo,//(const WCHAR*)(LPCTSTR)strInfo,
            -1,//wcslen(strStartSec),
            &fontTacInfo,
            rcTxt,
            NULL,
            &txtBrush
            );*/
    /////////////

    // 【10】dc拷贝******************************************
    dc.BitBlt(0, 0, m_FrameRect.right, m_FrameRect.bottom, &dcMemory, 0, 0, SRCCOPY);

    // delete m_graphScreen;
    dcMemory.DeleteDC();
    m_BkBmp.DeleteObject();

    // Do not call CStatic::OnPaint() for painting messages
}

void CGreenField::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    CPoint tpoint;
    uAgent *pAgent = pDataSummary->agent;
    if (m_bSetPlayerIn == true)
    {
        // 更新匹配起始点
        tpoint = m_Point_Img2Field(point.x, point.y);
        // pDataSummary->posPlayerIn = tpoint;
        pDataSummary->posPlayerIn.x = tpoint.x;
        pDataSummary->posPlayerIn.y = tpoint.y;
        for (int i = 0; i < AG_NUM; i++)
        {
            pCoachUDP->m_ToAgent[i].SetInitPos(pDataSummary->posPlayerIn.x, pDataSummary->posPlayerIn.y, 0);
        }
        m_bSetPlayerIn = false;
        Invalidate(FALSE);
    }
    else
    {
        // 判断是否点击了翻转开关
        if (PointInRect(point, m_rcFlip) == true)
        {
            if (m_bLeftToRight == true)
            {
                m_bLeftToRight = false;
            }
            else
            {
                m_bLeftToRight = true;
            }
            Invalidate(FALSE);
            return;
        }

        // 判断是否点击了设置入场点开关
        if (PointInRect(point, m_rcSetPlayerIn) == true)
        {
            if (m_bSetPlayerIn == true)
            {
                m_bSetPlayerIn = false;
            }
            else
            {
                m_bSetPlayerIn = true;
            }
            Invalidate(FALSE);
            return;
        }

        // 判断是否点击了显示原始障碍物点开关
        if (PointInRect(point, m_rcShowOrgObst) == true)
        {
            if (m_bShowOrgObst == true)
            {
                m_bShowOrgObst = false;
            }
            else
            {
                m_bShowOrgObst = true;
            }
            Invalidate(FALSE);
            return;
        }

        // 判断是否点击了显示障碍物追踪开关
        if (PointInRect(point, m_rcShowEnemyTrack) == true)
        {
            if (m_bShowEnemyTrack == true)
            {
                m_bShowEnemyTrack = false;
            }
            else
            {
                m_bShowEnemyTrack = true;
            }
            Invalidate(FALSE);
            return;
        }

        // 判断是否点击了障碍物index显示模式
        if (PointInRect(point, m_rcObstNumDef) == true)
        {
            if (m_nObstNumMode == OBST_NUM_CLOSETO_OWNGOAL)
            {
                m_nObstNumMode = OBST_NUM_BACK_DEF;
            }
            else
            {
                m_nObstNumMode = OBST_NUM_CLOSETO_OWNGOAL;
            }
            Invalidate(FALSE);
            return;
        }

        // 判断点选范围
        int rag = 12 * 3;
        tpoint = m_Point_Img2Field(point.x, point.y);
        for (int i = 0; i < 5; i++)
        {
            if (tpoint.x > pAgent[i].x - rag && tpoint.x < pAgent[i].x + rag && tpoint.y > pAgent[i].y - rag &&
                tpoint.y < pAgent[i].y + rag)
            {
                m_nSelAgentID = pAgent[i].ID;
                /*if (pNum != NULL)
                {
                    pNum->Number = SelID;
                    pNum->Invalidate();
                }*/
                Invalidate(FALSE);
                return;
            }
        }
        CPoint targ;
        // 计算移动目标点
        if (m_nSelAgentID != 0 && pDataSummary->agent[m_nSelAgentID - 1].online == true &&
            pDataSummary->agent[m_nSelAgentID - 1].status != ROBST_ERR)
        {
            switch (m_nAgentTestMode)
            {
            case AGENT_TEST_NONE: // 默认模式下是左键点击辅助MoveTo
                // 移动
                if (pDataCenter != NULL)
                {
                    if (pDataCenter->m_nMatchMode == MATCH_STOP)
                    {
                        pDataCenter->ChangeMatchModeTo(MATCH_OFF);
                    }
                }

                targ = m_Point_Img2Field(point.x, point.y);

                // 发送指令并修改显示(MoveTo)
                pCoachUDP->AgentCmdMoveTo(m_nSelAgentID, targ.x, targ.y, 0, 0, true, 100);
                break;

            case AGENT_TEST_DEF_BALL: // 鼠标左键点击辅助训练防传球
                if (pDataCenter != NULL)
                {
                    if (pDataCenter->m_nMatchMode == MATCH_STOP)
                    {
                        pDataCenter->ChangeMatchModeTo(MATCH_OFF);
                    }
                }

                targ = m_Point_Img2Field(point.x, point.y);

                // 发送指令并修改显示(MoveTo)
                pCoachUDP->m_ToAgent[m_nSelAgentID - 1].DEF_BALL(targ.x, targ.y, m_nAgentTestDefDist);
                break;

            case AGENT_TEST_DEF_GOAL: // 鼠标左键点击辅助训练防射门
                if (pDataCenter != NULL)
                {
                    if (pDataCenter->m_nMatchMode == MATCH_STOP)
                    {
                        pDataCenter->ChangeMatchModeTo(MATCH_OFF);
                    }
                }

                targ = m_Point_Img2Field(point.x, point.y);

                // 发送指令并修改显示(MoveTo)
                pCoachUDP->m_ToAgent[m_nSelAgentID - 1].DEF_GOAL(targ.x, targ.y, m_nAgentTestDefDist);
                break;

            case AGENT_TEST_PASS:
                if (pDataCenter != NULL)
                {
                    if (pDataCenter->m_nMatchMode == MATCH_STOP)
                    {
                        pDataCenter->ChangeMatchModeTo(MATCH_OFF);
                    }
                }

                targ = m_Point_Img2Field(point.x, point.y);

                // 发送指令并修改显示(MoveTo)
                pCoachUDP->m_ToAgent[m_nSelAgentID - 1].SetRefBall(pDataSummary->cGlobalBall.x,
                                                                   pDataSummary->cGlobalBall.y);
                pCoachUDP->m_ToAgent[m_nSelAgentID - 1].PassCmd(targ.x, targ.y);
                break;

            case AGENT_TEST_CATCH:
                if (pDataCenter != NULL)
                {
                    if (pDataCenter->m_nMatchMode == MATCH_STOP)
                    {
                        pDataCenter->ChangeMatchModeTo(MATCH_OFF);
                    }
                }

                targ = m_Point_Img2Field(point.x, point.y);

                // 发送指令并修改显示(MoveTo)
                pCoachUDP->m_ToAgent[m_nSelAgentID - 1].SetRefBall(pDataSummary->cGlobalBall.x,
                                                                   pDataSummary->cGlobalBall.y);
                pCoachUDP->m_ToAgent[m_nSelAgentID - 1].CatchCmd(targ.x, targ.y);
                break;
            }

            Invalidate(FALSE);
        }

        // 设置目标防守区域
        /*if (m_bZoneDefMode == true && SelID != 0)
        {
        tpoint = m_Point_Img2Field(point.x, point.y);
        int rectID = m_fieldWhichRect(tpoint.x, tpoint.y);
        AZoneDefTo(SelID, m_fieldMidPnt[rectID].x, m_fieldMidPnt[rectID].y, 0);
        //直接更新显示出移动目标点
        m_agents[SelID].dest_x = m_fieldMidPnt[rectID].x;
        m_agents[SelID].dest_y = m_fieldMidPnt[rectID].y;
        }*/
    }

    // CStatic::OnLButtonDown(nFlags, point);
}

void CGreenField::OnRButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    m_nSelAgentID = -1;
    Invalidate(FALSE);

    CStatic::OnRButtonDown(nFlags, point);
}

// 场地坐标-》显示坐标
Point CGreenField::m_Point_Field2Img(int inX, int inY)
{
    Point ImagePoint;
    ImagePoint.X = inX * 600 / 1800 + 50;
    ImagePoint.Y = 400 - inY * 400 / 1200 + 50;

    if (m_bLeftToRight == false)
    {
        ImagePoint.X = 700 - ImagePoint.X;
        ImagePoint.Y = 500 - ImagePoint.Y;
    }

    // 限一下值
    /*if (ImagePoint.x > 490)
    {
        ImagePoint.x = 490;
    }
    if (ImagePoint.x <0)
    {
        ImagePoint.x = 0;
    }
    if (ImagePoint.y < 0)
    {
        ImagePoint.y = 0;
    }
    if (ImagePoint.y > 340)
    {
        ImagePoint.y = 340;
    }*/
    return ImagePoint;
}

// 显示坐标 - 》场地坐标
CPoint CGreenField::m_Point_Img2Field(int inX, int inY)
{
    CPoint Fieldpoint;
    Fieldpoint.x = (inX - 50) * 3;
    Fieldpoint.y = (450 - inY) * 3;

    if (m_bLeftToRight == false)
    {
        // 横向翻转
        Fieldpoint.x = 1800 - Fieldpoint.x;
        // 纵向翻转
        Fieldpoint.y = 1200 - Fieldpoint.y;
    }

    return Fieldpoint;
}

// 控制代号转换成字符串
CString CGreenField::GetCTRLString(int inCtrlCode)
{
    CString strC;
    switch (inCtrlCode)
    {
    case CTRL_ATTACK:
        strC.Format("进攻");
        break;
    case CTRL_STOP:
        strC.Format("停止");
        break;
    case CTRL_MOVETO:
        strC.Format("移动");
        break;

    case CTRL_GOALKEEP:
        strC.Format("守门员");
        break;

    case CTRL_PRODEF:
        strC.Format("高级防守");
        break;

    case CTRL_FOCUSONBALL:
        strC.Format("注视球");
        break;

    case CTRL_SHIFTATK:
        strC.Format("进攻跑位");
        break;

    case CTRL_DEFENCE:
        strC.Format("防守");
        break;

    case CTRL_PASS:
        strC.Format("传球");
        break;

    case CTRL_PASS_MOVE:
        strC.Format("动态传球");
        break;

    case CTRL_CATCH:
        strC.Format("接球");
        break;

    case CTRL_CATCH_MOVE:
        strC.Format("接球跑位");
        break;

    case CTRL_CATCH_FOCUS:
        strC.Format("接球ing");
        break;

    case CTRL_MANUAL:
        strC.Format("手动控制");
        break;

    case CTRL_IDLE:
        strC.Format("解锁");
        break;

        // 跑位
    case CTRL_KICKOFF_PRIM_READY:
        strC.Format("主导开球跑位");
        break;

    case CTRL_KICKOFF_SLAVE_READY:
        strC.Format("辅助开球跑位");
        break;

    case CTRL_KICKOFF_PRIM:
        strC.Format("主导开球");
        break;

    case CTRL_KICKOFF_SLAVE:
        strC.Format("辅助开球");
        break;

    case CTRL_SEARCHBALL:
        strC.Format("搜球");
        break;

    case ROBST_ERR:
        strC.Format("待机状态");
        break;

    case CTRL_AROUNDBALL:
        strC.Format("跑位");
        break;

    case CTRL_LSATK:
        strC.Format("刘硕进攻");
        break;

    case CTRL_LSATKCOVER:
        strC.Format("刘硕协防");
        break;

    case CTRL_ZONEDEF:
        strC.Format("区域防守");
        break;

    case CTRL_FOLLOW:
        strC.Format("跟踪");
        break;

    case CTRL_DEF_BALL:
        strC.Format("防传球");
        break;

    case CTRL_DEF_GOAL:
        strC.Format("防射门");
        break;

    case CTRL_PENALTY:
        strC.Format("点球");
        break;

    default:
        strC.Format("未知(%d)", inCtrlCode);
        break;
    }
    return strC;
}

bool CGreenField::PointInRect(CPoint inPnt, Rect inRect)
{
    if (inPnt.x > inRect.GetLeft() && inPnt.x < inRect.GetRight() && inPnt.y > inRect.GetTop() &&
        inPnt.y < inRect.GetBottom())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void CGreenField::MBToWC(CString inStr, WCHAR *inWC)
{
    // 多字节CString 转换到 WCHAR进行显示
    int len = MultiByteToWideChar(CP_ACP, 0, inStr, -1, NULL, 0); // 先取得转换后的UNICODE字符串所需的长度
    MultiByteToWideChar(CP_ACP, 0, inStr, -1, inWC, len);         // 开始转换
}
