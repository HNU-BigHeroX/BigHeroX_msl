// UPFullField.cpp : implementation file
//

#include "stdafx.h"
#include "MSLAgent.h"
#include "UPFullField.h"
#include "MSL_Protocol.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUPFullField

CUPFullField::CUPFullField()
{
    pAglist = NULL;
    m_nAgentNum = 0;

    m_eSel = none;

    m_bAllBall = true;
    m_bKeyHold = false;
    m_bSetObst = false; // 设置障碍物
    m_curObst = 0;
    m_nCycle = 15;

    for (int i = 0; i < 7; i++)
    {
        // agents
        m_agents[i].ID = i;
        m_agents[i].foundball = false;
        m_agents[i].dist_ball = 10000;
        m_agents[i].online = false;
        m_agents[i].next = NULL;
        m_agents[i].x = (m_CalPoint(200 + 300 * i, 0)).x;
        m_agents[i].y = -80; //(m_CalPoint(200+300*i,0)).y;
        m_agents[i].angle = 12;
        m_agents[i].moveto_x = m_agents[i].x + 500;
        m_agents[i].moveto_y = m_agents[i].y + 400;
        m_agents[i].status = CTRL_MOVETO;
        m_agents[i].V = 0;
        m_agents[i].V_Angle = 0;
        // balls
        m_balls[i].ID = i;
        m_balls[i].updated = false;
        m_balls[i].x = 900;
        m_balls[i].y = 600;

        // m_nVirtualSt
        m_nVirtualSt[i] = 0;
    }
    m_recball.x = 900;
    m_recball.y = 600;

    for (i = 0; i < OBST_NUM; i++)
    {
        m_obst[i].valid = false;
    }
    // 	m_obst[10].ID = 1;
    // 	m_obst[10].x = 800;
    // 	m_obst[10].y = 600;
    // 	m_obst[10].size = 5;
    // 	m_obst[10].valid = true;
    // 	m_nObstID = 1;

    for (i = 0; i < OBST_NUM / 5; i++)
    {
        m_trueobst[i].valid = false;
    }
    m_bSetTrueObst = false;
    m_MoveTrueObstIndex = -1;

    m_agents[6].online = false;

    // sel
    SelID = 0;
    m_SelAng = 0;

    // status
    VERIFY(font.CreateFont(16,                       // nHeight
                           0,                        // nWidth
                           0,                        // nEscapement
                           0,                        // nOrientation
                           FW_BLACK,                 // nWeight
                           FALSE,                    // bItalic
                           FALSE,                    // bUnderline
                           0,                        // cStrikeOut
                           ANSI_CHARSET,             // nCharSet
                           OUT_DEFAULT_PRECIS,       // nOutPrecision
                           CLIP_DEFAULT_PRECIS,      // nClipPrecision
                           DEFAULT_QUALITY,          // nQuality
                           DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
                           "Arial"));                // lpszFacename

    // status
    VERIFY(txtfont.CreateFont(14,                       // nHeight
                              0,                        // nWidth
                              0,                        // nEscapement
                              0,                        // nOrientation
                              FW_NORMAL,                // nWeight
                              FALSE,                    // bItalic
                              FALSE,                    // bUnderline
                              0,                        // cStrikeOut
                              ANSI_CHARSET,             // nCharSet
                              OUT_DEFAULT_PRECIS,       // nOutPrecision
                              CLIP_DEFAULT_PRECIS,      // nClipPrecision
                              DEFAULT_QUALITY,          // nQuality
                              DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
                              "Arial"));                // lpszFacename

    m_strCurSt.Format("虚拟赛场");

    GetCurTime();

    m_bAtkIm = true;

    m_bBallMove = false;
    m_nBallTarX = 0;
    m_nBallTarY = 0;
    m_nPassPlayer = 0;

    bObstBust = false;
}

CUPFullField::~CUPFullField()
{
    m_Clearpa();
    m_Clearpb();
    m_asc.Unlock();
    m_bsc.Unlock();
}

BEGIN_MESSAGE_MAP(CUPFullField, CStatic)
//{{AFX_MSG_MAP(CUPFullField)
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_RBUTTONDOWN()
ON_WM_TIMER()
ON_WM_CLOSE()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_WM_MOUSEWHEEL()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUPFullField message handlers

void CUPFullField::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // TODO: Add your message handler code here
    CDC dcMemory;
    dcMemory.CreateCompatibleDC(&dc);

    m_BkBmp.DeleteObject();
    if (OwnGoalColor == YELLOWGOAL)
    {
        m_BkBmp.LoadBitmap(IDB_YELFIELD);
    }
    else
    {
        m_BkBmp.LoadBitmap(IDB_BLUEFIELD);
    }

    dcMemory.SelectObject(&m_BkBmp);
    dcMemory.SetBkMode(TRANSPARENT);
    CPen *oldpen;
    CPen path(PS_DOT, 1, RGB(255, 255, 0));

    // 绘制Agnet
    CBrush blue(RGB(0, 0, 255));
    CPoint coodinate;
    CPoint direct;
    CString aNum;
    CPen dpen(PS_SOLID, 3, RGB(255, 0, 100));
    // uagent* t = m_pa;

    CPen outline(PS_SOLID, 1, RGB(0, 0, 0));
    CPen yellow(PS_SOLID, 2, RGB(255, 255, 0));
    // m_asc.Lock();
    for (int i = 1; i < 7; i++)
    {
        if (m_agents[i].online == false)
        {
            continue;
        }
        coodinate = m_CalPoint(m_agents[i].x, m_agents[i].y);
        dcMemory.SelectObject(&blue);

        // 绘制路径
        if ((m_agents[i].x != m_agents[i].moveto_x || m_agents[i].y != m_agents[i].moveto_y) &&
            m_agents[i].status == CTRL_MOVETO)
        {
            oldpen = dcMemory.SelectObject(&path);
            CPoint moveto = m_CalPoint(m_agents[i].moveto_x, m_agents[i].moveto_y);
            dcMemory.MoveTo(coodinate);
            dcMemory.LineTo(moveto);
            dcMemory.SelectObject(oldpen);
        }

        // 如果是点选目标，则将外围线设为黄色
        if (m_agents[i].ID == (UINT)SelID)
        {
            oldpen = dcMemory.SelectObject(&yellow);
        }
        else
        {
            oldpen = dcMemory.SelectObject(&outline);
        }
        dcMemory.Ellipse(coodinate.x - 8, coodinate.y - 8, coodinate.x + 8, coodinate.y + 8);
        dcMemory.SelectObject(oldpen);

        direct.x = (long)(12 * cos(m_agents[i].angle * 3.14 / 180));
        direct.y = (long)(-12 * sin(m_agents[i].angle * 3.14 / 180));
        direct += coodinate;
        oldpen = dcMemory.SelectObject(&dpen);
        dcMemory.MoveTo(coodinate);
        dcMemory.LineTo(direct);
        dcMemory.SelectObject(oldpen);
        aNum.Format("%d", m_agents[i].ID);
        dcMemory.SetTextColor(RGB(0, 255, 255));
        dcMemory.TextOut(coodinate.x - 4, coodinate.y - 8, aNum);

        // 显示状态
        aNum = GetCTRLString(m_agents[i].status);
        CFont *oldfont = dcMemory.SelectObject(&txtfont);
        dcMemory.SetTextColor(RGB(255, 255, 255));
        dcMemory.TextOut(coodinate.x - 10, coodinate.y - 20, aNum);
        dcMemory.SelectObject(oldfont);

        //////////////////////////////////////////////////////////////////////////
        // 		aNum.Format("v=%d,a=%d",m_agents[i].V,m_agents[i].V_Angle);
        // 		oldfont = dcMemory.SelectObject(&txtfont);
        // 		dcMemory.SetTextColor(RGB(255,255,255));
        // 		dcMemory.TextOut(coodinate.x-10,coodinate.y+20,aNum);
        // 		dcMemory.SelectObject(oldfont);
        //////////////////////////////////////////////////////////////////////////
    }

    // 绘制Ball
    CBrush orange(RGB(255, 155, 0));
    CBrush *oldbrush = dcMemory.SelectObject(&orange);
    dcMemory.SetTextColor(RGB(0, 0, 0));
    for (i = 1; i < 7; i++)
    {
        if (m_agents[i].foundball == true)
        {
            // m_bsc.Lock();
            coodinate = m_CalPoint(m_balls[i].x, m_balls[i].y);
            dcMemory.Ellipse(coodinate.x - 7, coodinate.y - 7, coodinate.x + 7, coodinate.y + 7);
            aNum.Format("%d", m_balls[i].ID);
            dcMemory.TextOut(coodinate.x - 4, coodinate.y - 8, aNum);
            // m_bsc.Unlock();
        }
    }
    // 参考球
    CBrush red(RGB(255, 0, 0));
    oldbrush = dcMemory.SelectObject(&red);
    coodinate = m_CalPoint(m_recball.x, m_recball.y);
    dcMemory.Ellipse(coodinate.x - 7, coodinate.y - 7, coodinate.x + 7, coodinate.y + 7);
    dcMemory.TextOut(coodinate.x - 4, coodinate.y - 8, "b");

    // 障碍物
    CBrush black(RGB(0, 0, 0));
    oldbrush = dcMemory.SelectObject(&black);
    dcMemory.SetTextColor(RGB(255, 255, 255));
    for (i = 0; i < OBST_NUM; i++)
    {
        if (m_obst[i].valid == true)
        {
            coodinate = m_CalPoint(m_obst[i].x, m_obst[i].y);
            dcMemory.Ellipse(coodinate.x - 7, coodinate.y - 7, coodinate.x + 7, coodinate.y + 7);
            aNum.Format("%d", m_obst[i].ID);
            dcMemory.TextOut(coodinate.x - 4, coodinate.y - 8, aNum);
        }
    }
    dcMemory.SelectObject(oldbrush);

    // status
    dcMemory.SelectObject(&font);
    if (m_strCurSt.FindOneOf("对") == 0)
    {
        dcMemory.SetTextColor(RGB(200, 0, 0));
    }
    else
    {
        dcMemory.SetTextColor(RGB(0, 0, 200));
    }
    dcMemory.TextOut(200, 10, m_strCurSt);

    //////////////////////////////////////////////////////////////////////////
    CString info;
    info.Format("%d,%d", m_nBallTarX, m_nBallTarY);
    dcMemory.TextOut(400, 10, info);
    //////////////////////////////////////////////////////////////////////////

    dc.BitBlt(0, 0, 493, 346, &dcMemory, 0, 0, SRCCOPY);
    dcMemory.DeleteDC();

    // Do not call CStatic::OnPaint() for painting messages
}

void CUPFullField::m_Clearpa()
{
}

CPoint CUPFullField::m_CalPoint(int inx, int iny)
{
    CPoint p;
    p.x = inx * 400 / 1800 + 48;
    p.y = 305 - iny * 266 / 1200;
    // 限一下值
    if (p.x > 490)
    {
        p.x = 490;
    }
    if (p.x < 0)
    {
        p.x = 0;
    }
    if (p.y < 0)
    {
        p.y = 0;
    }
    if (p.y > 340)
    {
        p.y = 340;
    }
    return p;
}

void CUPFullField::SetBall(UINT inID, int inx, int iny)
{
    m_bsc.Lock();
    m_balls[inID].x = inx;
    m_balls[inID].y = iny;
    m_balls[inID].updated = true;
    m_bsc.Unlock();
}

void CUPFullField::m_Clearpb()
{
}

void CUPFullField::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    int rag = 10; // 点选范围
    CPoint tpoint;

    {
        // 遍历agents和balls列表
        for (int i = 1; i < 7; i++)
        {
            // 查看ball
            tpoint = m_CalPoint(m_balls[i].x, m_balls[i].y);
            if (point.x > tpoint.x - rag && point.x < tpoint.x + rag && point.y > tpoint.y - rag &&
                point.y < tpoint.y + rag)
            {
                m_bKeyHold = true;
                m_eSel = BallSel;
                SelID = m_balls[i].ID;

                if (pnumdis != NULL)
                {
                    pnumdis->bisBall = true;
                    pnumdis->Number = SelID;
                    pnumdis->Invalidate();
                }
                m_bBallMove = false;
                Invalidate();
                return;
            }

            // 查看agent
            m_asc.Lock();
            tpoint = m_CalPoint(m_agents[i].x, m_agents[i].y);
            if (point.x > tpoint.x - rag && point.x < tpoint.x + rag && point.y > tpoint.y - rag &&
                point.y < tpoint.y + rag)
            {
                if (m_agents[i].online == true)
                {
                    m_bKeyHold = true;
                    m_eSel = AgentSel;
                    SelID = m_agents[i].ID;

                    if (pnumdis != NULL)
                    {
                        pnumdis->bisBall = false;
                        if (m_bKeyHold == true)
                        {
                            pnumdis->Number = 0;
                        }
                        else
                        {
                            pnumdis->Number = SelID;
                        }
                        pnumdis->Invalidate();
                    }
                    m_asc.Unlock();
                    Invalidate();
                    return;
                }
            }
            m_asc.Unlock();
        }
        // 		if (SelID != 0)
        // 		{
        // 			//移动
        // 			int tx,ty;
        // 			tx = (point.x-48)*18/4;
        // 			ty = (305-point.y)*1200/266;
        // 			//AMoveTo(SelID,tx,ty,m_SelAng);
        // 			//Invalidate();
        // 		}
        for (i = 0; i < OBST_NUM; i++)
        {
            if (m_obst[i].valid == true)
            {
                tpoint = m_CalPoint(m_obst[i].x, m_obst[i].y);
                if (point.x > tpoint.x - rag && point.x < tpoint.x + rag && point.y > tpoint.y - rag &&
                    point.y < tpoint.y + rag)
                {
                    m_curObst = i;
                    SelID = m_obst[i].ID;
                    m_eSel = ObstSel;
                    m_bKeyHold = true;
                    break;
                }
            }
        }

        for (i = 0; i < OBST_NUM / 5; i++)
        {
            if (m_trueobst[i].valid == true)
            {
                tpoint = m_CalPoint(m_trueobst[i].x, m_trueobst[i].y);
                if (point.x > tpoint.x - rag && point.x < tpoint.x + rag && point.y > tpoint.y - rag &&
                    point.y < tpoint.y + rag)
                {
                    m_MoveTrueObstIndex = i;
                    m_eSel = TrueobstSel;
                    m_bKeyHold = true;
                    break;
                }
            }
        }
        Invalidate();
    }

    if (m_bSetObst == true)
    {
        for (int i = 0; i < OBST_NUM; i++)
        {
            // 先查看已经存在的障碍物有没有被选中的
            if (m_obst[i].valid == true)
            {
                tpoint = m_CalPoint(m_obst[i].x, m_obst[i].y);
                if (point.x > tpoint.x - rag && point.x < tpoint.x + rag && point.y > tpoint.y - rag &&
                    point.y < tpoint.y + rag)
                {
                    m_curObst = i;
                    break;
                }
            }
        }

        // 已存在的障碍物都不符合条件，新建一个
        for (i = 0; i < OBST_NUM; i++)
        {
            // 先查看已经存在的障碍物有没有被选中的
            if (m_obst[i].valid == false)
            {
                // 				static int nid;
                // 				nid = i/10+1;
                // 				if (nid > 5)
                // 				{
                // 					nid = 1;
                // 				}
                // 				m_obst[i].ID = nid;
                m_obst[i].ID = m_nObstID;

                m_obst[i].x = (point.x - 48) * 18 / 4;
                m_obst[i].y = (305 - point.y) * 1200 / 266;
                m_obst[i].size = 10;
                m_obst[i].valid = true;
                m_curObst = i;
                break;
            }
        }
        Invalidate();
    }

    if (m_bSetTrueObst == true)
    {
        int i;
        // 先在现有全局障碍物数组里找有没有和现有障碍物相近的障碍物
        // 		for (i=0;i<OBST_NUM/5;i++)
        // 		{
        // 			if (m_trueobst[i].valid == true)
        // 			{
        // 				tpoint = m_CalPoint(m_trueobst[i].x,m_trueobst[i].y);
        // 				if (point.x>tpoint.x-rag && point.x<tpoint.x+rag && point.y>tpoint.y-rag &&
        // point.y<tpoint.y+rag)
        // 				{
        // 					break;
        // 				}
        // 			}
        // 		}

        // 遍历了现有全局障碍物，未找到与其位置相近的障碍物,增加障碍物
        for (i = 0; i < OBST_NUM / 5; i++)
        {
            if (m_trueobst[i].valid == false)
            {
                break;
            }
        }

        // 根据i来判断是否扫描到数组尾部
        if (i >= OBST_NUM / 5)
        {
            i = 0;
        }

        // 设置新的全局障碍物
        m_trueobst[i].ID = 0;
        m_trueobst[i].x = (point.x - 48) * 18 / 4;
        m_trueobst[i].y = (305 - point.y) * 1200 / 266;
        m_trueobst[i].size = 10;
        m_trueobst[i].valid = true;

        m_SetObstOnTrueobsts();

        Invalidate();
    }
    CStatic::OnLButtonDown(nFlags, point);
}

void CUPFullField::OnRButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    SelID = 0;

    Invalidate();
    CStatic::OnRButtonDown(nFlags, point);
}

void CUPFullField::RemoveAgent(int inID)
{
    if (m_agents[inID].online == true)
    {
        m_asc.Lock();
        m_nAgentNum--;
        m_agents[inID].online = false;
        m_agents[inID].foundball = false;
        m_asc.Unlock();
    }
}

void CUPFullField::RemoveBall(UINT inID)
{
}

uagent *CUPFullField::GetAgent(UINT inID)
{
    if (inID < 1 || inID > 6)
    {
        return NULL;
    }

    if (m_agents[inID].online == false)
    {
        return NULL;
    }

    return &m_agents[inID];
}

static nHoldingBall = 100;
void CUPFullField::Exec()
{
    int i;
    // 解析各个agent收到的信息
    for (i = 1; i < 6; i++)
    {
        ParseRecCtrl(i, &m_AgUDPs[i]);
    }

    //////////////////////////////////////////////////////////////////////////
    /*/根据m_agents[]数组向coach反馈状态
    ReplyAgentsStatus();
    return;
    /////////////////////////////////////////////////////////////////////////*/

    CPoint pntTmp;
    int tmpAngel;
    // 对agent位置进行调整
    for (i = 1; i < 7; i++)
    {
        if (m_agents[i].online == true)
        {
            switch (m_agents[i].status)
            {
            case CTRL_ZONEDEF:
                int a;
                a = 0;
            case CTRL_MOVETO:
                m_BehMoveto(&m_agents[i]);
                break;

            case CTRL_GOALKEEP:
                m_BehGoalKeeper(&m_agents[i]);
                break;

            case CTRL_ATTACK:
                m_BehAttack(&m_agents[i]);
                break;

            case CTRL_DEFENCE:
                m_BehDef(&m_agents[i]);
                break;

            case CTRL_PASS:
                m_nBallTarX = m_agents[i].target_x;
                m_nBallTarY = m_agents[i].target_y;

                m_nPassPlayer = i;
                if (m_bAtkIm == true)
                {
                    nHoldingBall--;
                    if (nHoldingBall <= 0)
                    {
                        // 							m_agents[i].x = m_recball.x;
                        // 							m_agents[i].y = m_recball.y;
                        nHoldingBall = 20;
                        m_bBallMove = true;
                        if (m_nPassPlayer != i)
                        {
                            m_AgUDPs[i].AttentionKick(m_agents[i].x, m_agents[i].y, m_agents[i].target_x,
                                                      m_agents[i].target_y);
                            m_AgToCap[i].AttentionKick(m_agents[i].x, m_agents[i].y, m_agents[i].target_x,
                                                       m_agents[i].target_y);
                        }
                    }
                }
                break;

            case CTRL_CATCH:
                m_BehCatch(&m_agents[i]);
                // 接球后转为进攻
                if (m_bAtkIm == true)
                {
                    //	if (m_recball.x == m_agents[i].x && m_recball.y == m_agents[i].y)
                    if (m_CalDist(m_recball.x, m_recball.y, m_agents[i].x, m_agents[i].y) <= 50)
                    {
                        m_AgUDPs[i].toS_status = CTRL_ATTACK;
                        m_AgToCap[i].toS_status = CTRL_ATTACK;
                        m_agents[i].status = CTRL_ATTACK;
                        m_BehAttack(&m_agents[i]);
                        m_nBallTarX = m_agents[i].x;
                        m_nBallTarY = m_agents[i].y;
                    }
                }
                break;

            case CTRL_PASS_MOVE:
                m_BehPassMove(&m_agents[i]);
                break;

            case CTRL_CATCH_MOVE:
            case CTRL_CATCH_FOCUS:
                m_BehCatch(&m_agents[i]);
                // 接球后转为进攻
                if (m_bAtkIm == true)
                {
                    //	if (m_recball.x == m_agents[i].x && m_recball.y == m_agents[i].y)
                    if (m_CalDist(m_recball.x, m_recball.y, m_agents[i].x, m_agents[i].y) <= 50)
                    {
                        m_AgUDPs[i].toS_status = CTRL_ATTACK;
                        m_AgToCap[i].toS_status = CTRL_ATTACK;
                        m_agents[i].status = CTRL_ATTACK;
                        m_BehAttack(&m_agents[i]);
                        m_nBallTarX = m_agents[i].x;
                        m_nBallTarY = m_agents[i].y;
                    }
                }
                break;

            case CTRL_KICKOFF_PRIM_READY:
                m_BehKOPReady(&m_agents[i]);
                break;

            case CTRL_KICKOFF_SLAVE_READY:
                m_BehKOSReady(&m_agents[i]);
                break;

            case CTRL_FOLLOW:

                break;

            case CTRL_DEF_BALL:
                pntTmp = m_Loca_Ray(m_agents[i].moveto_x, m_agents[i].moveto_y, m_recball.x, m_recball.y, 100);
                tmpAngel = m_Loca_CalAngle(m_agents[i].moveto_x, m_agents[i].moveto_y, m_recball.x, m_recball.y);
                m_BehMoveto(&m_agents[i], pntTmp.x, pntTmp.y, tmpAngel);
                break;

            case CTRL_DEF_GOAL:
                pntTmp = m_Loca_Ray(m_agents[i].moveto_x, m_agents[i].moveto_y, 0, 600, 100);
                tmpAngel = m_Loca_CalAngle(m_agents[i].moveto_x, m_agents[i].moveto_y, m_recball.x, m_recball.y);
                m_BehMoveto(&m_agents[i], pntTmp.x, pntTmp.y, tmpAngel);
                break;

            case CTRL_ATTENTIONBALL:
                // break;
            case CTRL_KICKOFF_PRIM:
                if (m_bAtkIm == true)
                {
                    //					m_agents[i].status = CTRL_ATTACK;
                }
                break;
            }
        }
    }

    // 虚拟角色
    for (i = 0; i < 7; i++)
    {
        if (m_nVirtualSt[i] != 0)
        {
            m_VirtualRole(i, m_nVirtualSt[i]);
        }
    }

    m_SetObstOnTrueobsts();

    // 障碍物
    for (i = 0; i < 7; i++)
    {
        m_AgUDPs[i].Clear_Obst();
        m_AgToCap[i].Clear_Obst();
    }
    for (i = 0; i < OBST_NUM; i++)
    {
        if (m_obst[i].valid == true)
        {
            m_AgUDPs[m_obst[i].ID].AddObst(m_obst[i].x, m_obst[i].y, m_obst[i].size);
        }
    }

    // 球移动
    int tmp = 0;
    int scale = 100;
    if (m_bBallMove == true)
    {
        if (m_recball.x != m_nBallTarX)
        {
            if (m_recball.x > m_nBallTarX)
            {
                tmp = (m_recball.x - m_nBallTarX) / scale;
                if (tmp <= 0)
                {
                    tmp = 1;
                }
                m_recball.x -= tmp;
            }
            else
            {
                tmp = (m_nBallTarX - m_recball.x) / scale;
                if (tmp <= 0)
                {
                    tmp = 1;
                }
                m_recball.x += tmp;
            }
        }

        if (m_recball.y != m_nBallTarY)
        {
            if (m_recball.y > m_nBallTarY)
            {
                tmp = (m_recball.y - m_nBallTarY) / scale;
                if (tmp <= 0)
                {
                    tmp = 1;
                }
                m_recball.y -= tmp;
            }
            else
            {
                tmp = (m_nBallTarY - m_recball.y) / scale;
                if (tmp <= 0)
                {
                    tmp = 1;
                }
                m_recball.y += tmp;
            }
        }

        // 所有球都跟着动
        if (m_bAllBall == true)
        {
            for (int i = 1; i < 7; i++)
            {
                m_balls[i].x = m_recball.x;
                m_balls[i].y = m_recball.y;
            }
        }
    }

    // 显示
    Invalidate();

    //////////////////////////////////////////////////////////////////////////

    // 根据m_agents[]数组向coach反馈状态
    ReplyAgentsStatus();
}

void CUPFullField::printf(CString instr)
{
}

int CUPFullField::m_CalDist(int inx1, int iny1, int inx2, int iny2)
{
    int ret = (inx1 - inx2) * (inx1 - inx2) + (iny1 - iny2) * (iny1 - iny2);
    ret = sqrt(ret);
    return ret;
}

CPoint CUPFullField::m_CalMoveTo(int x, int y)
{
    CPoint moveto(x, y);

    return moveto;
}

void CUPFullField::m_SortAgent()
{
}

void CUPFullField::GetCurTime()
{
    m_critiTime.Lock();
    SYSTEMTIME st;
    GetLocalTime(&st);
    m_strTime.Format("%2d:%2d:%2d", st.wHour, st.wMinute, st.wSecond);
    m_critiTime.Unlock();
}

void CUPFullField::OnTimer(UINT nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    switch (nIDEvent)
    {
    case 0:
        break;
    default:
        break;
    }

    CStatic::OnTimer(nIDEvent);
}

void CUPFullField::OnClose()
{
    // TODO: Add your message handler code here and/or call default

    CStatic::OnClose();
}

void CUPFullField::m_ReleaseAgentList(uagent *inList)
{
    uagent *delt = inList;
    uagent *next = inList;
    while (delt != NULL)
    {
        next = delt->next;
        delete[] delt;
        delt = next;
    }
}

CString CUPFullField::GetCTRLString(int inCtrlCode)
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
        strC.Format("积极防守");
        break;

    case CTRL_ATTENTIONBALL:
        strC.Format("球观察员");
        break;

    case CTRL_DEFENCE:
        strC.Format("防守");
        break;

    case CTRL_PASS:
        strC.Format("传球");
        break;

    case CTRL_CATCH:
        strC.Format("接球");
        break;

    case CTRL_PASS_MOVE:
        strC.Format("动态传球");
        break;

    case CTRL_CATCH_MOVE:
        strC.Format("动态接球");
        break;

    case CTRL_CATCH_FOCUS:
        strC.Format("接球ing");
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
        strC.Format("FOLLOW");
        break;

    case CTRL_DEF_BALL:
        strC.Format("DEF_BALL");
        break;

    case CTRL_DEF_GOAL:
        strC.Format("DEF_GOAL");
        break;

    default:
        strC.Format("未知(%d)", inCtrlCode);
        break;
    }
    return strC;
}

CString CUPFullField::GetTime()
{
    m_critiTime.Lock();
    CString time = m_strTime;
    m_critiTime.Unlock();
    return time;
}

void CUPFullField::KickAgent(int inID)
{
    RemoveAgent(inID);
    // 关闭连接
    m_AgToCap[inID].Close();
}

void CUPFullField::ParseRecCtrl(int inID, CAgentUDP *inAg)
{
    if (inID < 0 || inID > 5)
    {
        return;
    }

    // 更新数据
    m_agents[inID].status = inAg->rec_ctrl;
    m_agents[inID].moveto_x = inAg->rec_targetx;
    m_agents[inID].moveto_y = inAg->rec_targety;
    m_agents[inID].target_x = inAg->rec_targetx;
    m_agents[inID].target_y = inAg->rec_targety;
    m_agents[inID].moveto_angle = inAg->rec_targetang;
    m_agents[inID].def_dist = inAg->rec_Def_Dist;
    m_agents[inID].def_angle = inAg->rec_Def_Ang;
    m_agents[inID].moveto_spd = inAg->rec_MoveSpd;

    if (m_agents[inID].online == true && inAg->rec_ctrl != CTRL_MOVETO)
    {
        // m_recball.x = inAg.re_ballx;
        // m_recball.y = inAg.re_bally;
    }

    if (m_agents[inID].online == true &&
        (inAg->rec_ctrl == CTRL_PASS || inAg->rec_ctrl == CTRL_PASS_MOVE || inAg->rec_ctrl == CTRL_CATCH ||
         inAg->rec_ctrl == CTRL_CATCH_MOVE || inAg->rec_ctrl == CTRL_CATCH_FOCUS))
    {
        m_agents[inID].target_x = inAg->rec_targetx;
        m_agents[inID].target_y = inAg->rec_targety;
    }
}

void CUPFullField::ReplyAgentsStatus()
{
    for (int i = 1; i < 6; i++)
    {
        // 		if (m_agents[i].status == CTRL_PASS)
        // 		{
        // 			continue;
        // 		}
        if (m_bAllBall == false)
        {
            m_agents[i].foundball = false;
        }
        if (m_agents[i].status == CTRL_PASS || m_agents[i].status == CTRL_PASS_MOVE ||
            m_agents[i].status == CTRL_MOVETO || m_agents[i].status == CTRL_DEF_BALL ||
            m_agents[i].status == CTRL_DEF_GOAL)
        {
            m_AgUDPs[i].toS_TargetX = m_agents[i].target_x;
            m_AgUDPs[i].toS_TargetY = m_agents[i].target_y;
            m_AgToCap[i].toS_TargetX = m_agents[i].target_x;
            m_AgToCap[i].toS_TargetY = m_agents[i].target_y;
        }
        m_AgUDPs[i].toS_V = m_agents[i].V;
        m_AgUDPs[i].toS_V_Angle = m_agents[i].V_Angle;
        m_AgToCap[i].toS_V = m_agents[i].V;
        m_AgToCap[i].toS_V_Angle = m_agents[i].V_Angle;
        //////////////////////////////////////////////////////
        // 障碍物爆发测试
        if (bObstBust == true)
        {
            for (int j = 0; j < AG_OBST_NUM; j++)
            {
                m_AgUDPs[i].m_obst[j].x = m_agents[i].x + j * 100;
                m_AgUDPs[i].m_obst[j].y = m_agents[i].y + j * 100;
                m_AgUDPs[i].m_obst[j].size = 10;
                m_AgUDPs[i].m_obst[j].valid = true;
                m_AgToCap[i].m_obst[j].x = m_agents[i].x + j * 100;
                m_AgToCap[i].m_obst[j].y = m_agents[i].y + j * 100;
                m_AgToCap[i].m_obst[j].size = 10;
                m_AgToCap[i].m_obst[j].valid = true;
            }
        }
        //////////////////////////////////////////////////////
        m_AgUDPs[i].Report(m_agents[i].x, m_agents[i].y, m_agents[i].angle, m_agents[i].status, m_agents[i].foundball,
                           m_balls[i].x, m_balls[i].y);
        m_AgUDPs[i].toS_ballx = m_recball.x;
        m_AgUDPs[i].toS_bally = m_recball.y;

        m_AgToCap[i].Report(m_agents[i].x, m_agents[i].y, m_agents[i].angle, m_agents[i].status, m_agents[i].foundball,
                            m_balls[i].x, m_balls[i].y);
        m_AgToCap[i].toS_ballx = m_recball.x;
        m_AgToCap[i].toS_bally = m_recball.y;
    }
}

void CUPFullField::m_BehMoveto(uagent *inagent)
{
    if (inagent == NULL)
    {
        return;
    }
    int tmp = 0;
    int v = 0;
    // x的调整
    if (inagent->x != inagent->moveto_x)
    {
        if (inagent->x > inagent->moveto_x)
        {
            tmp = inagent->x - inagent->moveto_x;
            tmp = tmp * inagent->moveto_spd / 255;
            tmp /= 40;
            tmp = tmp * m_nCycle / 15;
            if (tmp <= 0)
            {
                tmp = 1;
            }
            inagent->x -= tmp;
        }
        else
        {
            tmp = inagent->moveto_x - inagent->x;
            tmp = tmp * inagent->moveto_spd / 255;
            tmp /= 40;
            tmp = tmp * m_nCycle / 15;
            if (tmp <= 0)
            {
                tmp = 1;
            }
            inagent->x += tmp;
        }
    }
    v += tmp;

    // y的调整
    if (inagent->y != inagent->moveto_y)
    {
        if (inagent->y > inagent->moveto_y)
        {
            tmp = inagent->y - inagent->moveto_y;
            tmp = tmp * inagent->moveto_spd / 255;
            tmp /= 40;
            tmp = tmp * m_nCycle / 15;
            if (tmp <= 0)
            {
                tmp = 1;
            }
            inagent->y -= tmp;
        }
        else
        {
            tmp = inagent->moveto_y - inagent->y;
            tmp = tmp * inagent->moveto_spd / 255;
            tmp /= 40;
            tmp = tmp * m_nCycle / 15;
            if (tmp <= 0)
            {
                tmp = 1;
            }
            inagent->y += tmp;
        }
    }
    v += tmp;

    // 还有angle调整
    if (inagent->angle > inagent->moveto_angle)
    {
        inagent->angle--;
    }
    else
    {
        inagent->angle++;
    }

    // 速度方向
    // direct.x = (long)(12*cos(m_agents[i].angle*3.14/180));
    // direct.y = (long)(-12*sin(m_agents[i].angle*3.14/180));
    inagent->V = abs(v);
    if (inagent->moveto_x != inagent->x)
    {
        double dangle;
        dangle = (double)(inagent->moveto_y - inagent->y) / (inagent->moveto_x - inagent->x);

        inagent->V_Angle = atan(dangle) * 180 / 3.14;
        if (inagent->moveto_x - inagent->x < 0)
        {
            inagent->V_Angle = 180 + inagent->V_Angle;
        }

        while (inagent->V_Angle < 0)
        {
            inagent->V_Angle += 360;
        }

        while (inagent->V_Angle > 360)
        {
            inagent->V_Angle -= 360;
        }
    }
    else
    {
        if (inagent->moveto_y > inagent->y)
        {
            inagent->V_Angle = 90;
        }
        else
        {
            inagent->V_Angle = 270;
        }
    }
}

void CUPFullField::m_BehGoalKeeper(uagent *inagent)
{
    if (inagent == NULL)
    {
        return;
    }
    inagent->moveto_spd = 200;
    m_BehMoveto(inagent, 0, 600, 0);
}

void CUPFullField::m_BehDef(uagent *inagent)
{
    if (inagent == NULL)
    {
        return;
    }
    CPoint tmp;
    tmp = m_Loca_Ray(m_recball.x, m_recball.y, 0, 600, inagent->def_dist);
    if (inagent->def_angle != 0)
    {
        tmp = m_Loca_Rotate(tmp.x, tmp.y, m_recball.x, m_recball.y, inagent->def_angle);
    }
    int TargetAngle = -inagent->def_angle;

    inagent->moveto_spd = 200;
    m_BehMoveto(inagent, tmp.x, tmp.y, TargetAngle);
}

void CUPFullField::m_BehAttack(uagent *inagent)
{
    // 目前进攻队员是向球移动
    if (inagent->x != m_recball.x || inagent->y != m_recball.y)
    {
        // 未带球时趋向球
        m_BehMoveto(inagent, m_recball.x, m_recball.y, 0);
    }
    else
    {
        // 带球后向对方球门移动
        // 		m_BehMoveto(inagent,1800,600,0);
        // 		for (int i=1;i<7;i++)
        // 		{
        // 			m_balls[i].x = inagent->x;
        // 			m_balls[i].y = inagent->y;
        // 		}
        // 		m_recball.x = inagent->x;
        // 		m_recball.y = inagent->y;
    }
}

void CUPFullField::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_bKeyHold == true)
    {
        int tx, ty;
        tx = (point.x - 48) * 18 / 4;
        ty = (305 - point.y) * 1200 / 266;

        switch (m_eSel)
        {
        case BallSel:
            m_balls[SelID].x = tx;
            m_balls[SelID].y = ty;

            // 所有球都跟着动
            if (m_bAllBall == true)
            {
                for (int i = 1; i < 7; i++)
                {
                    m_balls[i].x = tx;
                    m_balls[i].y = ty;
                }
                m_recball.x = tx;
                m_recball.y = ty;
            }
            break;

        case AgentSel:
            m_agents[SelID].x = tx;
            m_agents[SelID].y = ty;
            break;

        case ObstSel:
            m_obst[m_curObst].x = tx;
            m_obst[m_curObst].y = ty;
            break;

        case TrueobstSel:
            m_trueobst[m_MoveTrueObstIndex].x = tx;
            m_trueobst[m_MoveTrueObstIndex].y = ty;
            m_ClearObstsNear(tx, ty, 100);
            m_SetObstOnTrueobsts();
            break;
        }
        Invalidate();
    }

    CStatic::OnMouseMove(nFlags, point);
}

void CUPFullField::OnLButtonUp(UINT nFlags, CPoint point)
{
    SelID = -1;
    if (pnumdis != NULL)
    {
        pnumdis->Number = -1;
        pnumdis->Invalidate();
    }

    m_bKeyHold = false;
    m_eSel = none;
    m_MoveTrueObstIndex = -1;
    Invalidate();
}

void CUPFullField::AddAgent(int inID, bool inFoundball, CString inCoachIP, int inPort)
{
    if (inID < 0 || inID > 7)
    {
        return;
    }

    if (m_agents[inID].online == false)
    {
        m_asc.Lock();
        m_nAgentNum++;
        m_agents[inID].online = true;
        m_agents[inID].foundball = inFoundball;
        m_asc.Unlock();
    }

    m_AgToCap[inID].Close();
    m_AgToCap[inID].SetID(inID);
    m_AgToCap[inID].SetTarget(inCoachIP, inPort);
    m_AgToCap[inID].SetCoachIP(inCoachIP);
}

void CUPFullField::m_BehMoveto(uagent *inagent, int inX, int inY, int inAngle)
{
    if (inagent == NULL)
    {
        return;
    }
    int tmp = 0;
    int v = 0;
    // x的调整
    if (inagent->x != inX)
    {
        if (inagent->x > inX)
        {
            tmp = inagent->x - inX;
            tmp = tmp * inagent->moveto_spd / 255;
            tmp /= 40;
            tmp = tmp * m_nCycle / 15;
            if (tmp <= 0)
            {
                tmp = 1;
            }
            inagent->x -= tmp;
        }
        else
        {
            tmp = inX - inagent->x;
            tmp = tmp * inagent->moveto_spd / 255;
            tmp /= 40;
            tmp = tmp * m_nCycle / 15;
            if (tmp <= 0)
            {
                tmp = 1;
            }
            inagent->x += tmp;
        }
    }
    v += tmp;

    // y的调整
    if (inagent->y != inY)
    {
        if (inagent->y > inY)
        {
            tmp = inagent->y - inY;
            tmp = tmp * inagent->moveto_spd / 255;
            tmp /= 40;
            tmp = tmp * m_nCycle / 15;
            if (tmp <= 0)
            {
                tmp = 1;
            }
            inagent->y -= tmp;
        }
        else
        {
            tmp = inY - inagent->y;
            tmp = tmp * inagent->moveto_spd / 255;
            tmp /= 40;
            tmp = tmp * m_nCycle / 15;
            if (tmp <= 0)
            {
                tmp = 1;
            }
            inagent->y += tmp;
        }
    }
    v += tmp;

    // 还有angle调整
    inagent->moveto_angle = inAngle;
    if (inagent->angle > inagent->moveto_angle)
    {
        inagent->angle--;
    }
    else
    {
        inagent->angle++;
    }

    // 速度方向
    // direct.x = (long)(12*cos(m_agents[i].angle*3.14/180));
    // direct.y = (long)(-12*sin(m_agents[i].angle*3.14/180));
    inagent->V = abs(v);
    if (inX != inagent->x)
    {
        double dangle;
        dangle = (double)(inY - inagent->y) / (inX - inagent->x);

        inagent->V_Angle = atan(dangle) * 180 / 3.14;
        if (inX - inagent->x < 0)
        {
            inagent->V_Angle = 180 + inagent->V_Angle;
        }

        while (inagent->V_Angle < 0)
        {
            inagent->V_Angle += 360;
        }

        while (inagent->V_Angle > 360)
        {
            inagent->V_Angle -= 360;
        }
    }
    else
    {
        if (inY > inagent->y)
        {
            inagent->V_Angle = 90;
        }
        else
        {
            inagent->V_Angle = 270;
        }
    }
}

CPoint CUPFullField::m_Loca_Ray(int inXbase, int inYbase, int inXtarg, int inYtarg, int inDist)
{
    CPoint newpoint;
    int srcDist = m_CalDist(inXbase, inYbase, inXtarg, inYtarg);
    if (srcDist == 0)
    {
        newpoint.x = inXbase;
        newpoint.y = inYbase;
        return newpoint;
    }
    int dx = (double)(inXtarg - inXbase) * inDist / srcDist;
    int dy = ((double)(inYtarg - inYbase) * inDist / srcDist);
    newpoint.x = inXbase + dx;
    newpoint.y = inYbase + dy;

    return newpoint;
}

CPoint CUPFullField::m_Loca_Rotate(int inx, int iny, int inCx, int inCy, int inAng)
{
    CPoint newpoint;
    if (inAng % 360 == 0)
    {
        newpoint.x = inx;
        newpoint.y = iny;
        return newpoint;
    }

    // 将角度换算成数值
    double vAng = (double)inAng * 3.14159 / 180;

    newpoint.x = (inx - inCx) * cos(vAng) + (iny - inCy) * sin(vAng) + inCx;
    newpoint.y = -(inx - inCx) * sin(vAng) + (iny - inCy) * cos(vAng) + inCy;

    return newpoint;
}

void CUPFullField::m_BehKOPReady(uagent *inagent)
{
    if (inagent == NULL)
    {
        return;
    }
    m_BehMoveto(inagent, m_recball.x, m_recball.y + 100, 0);
}

void CUPFullField::m_BehKOSReady(uagent *inagent)
{
    if (inagent == NULL)
    {
        return;
    }
    m_BehMoveto(inagent, m_recball.x - 50, m_recball.y - 100, 0);
}

void CUPFullField::m_VirtualRole(int inID, UINT inStatus)
{
    m_asc.Lock();
    switch (inStatus)
    {
    case CTRL_ATTACK:
    case CTRL_KICKOFF_PRIM_READY:
    case CTRL_KICKOFF_SLAVE_READY:
        m_agents[inID].x = m_AgUDPs[inID].re_ballx - 1;
        m_agents[inID].y = m_AgUDPs[inID].re_bally;
        break;

    case CTRL_GOALKEEP:
        m_agents[inID].x = 0;
        m_agents[inID].y = 600;
        break;
    default:
        break;
    }
    m_asc.Unlock();
}

void CUPFullField::SetAllBall(bool inEnable)
{
    int i;
    if (inEnable == true)
    {
        for (i = 0; i < 7; i++)
        {
            m_agents[i].foundball = true;
        }
    }
    else
    {
        for (i = 0; i < 7; i++)
        {
            m_agents[i].foundball = false;
        }
    }
}

void CUPFullField::AgentGetBall(int inID)
{
    // 所有球都跟着动

    for (int i = 1; i < 7; i++)
    {
        m_balls[i].x = m_agents[inID].x;
        m_balls[i].y = m_agents[inID].y;
    }
}

BOOL CUPFullField::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    AfxMessageBox("1");
    // TODO: Add your message handler code here and/or call default

    return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}

#define DIST_DISC_BALL 600
void CUPFullField::m_SetObstOnTrueobsts()
{
    for (int n = 0; n < OBST_NUM; n++)
    {
        m_obst[n].valid = false;
    }
    // 根据全局障碍物的设置来改变全局障碍物
    for (int i = 0; i < OBST_NUM / 5; i++)
    {
        if (m_trueobst[i].valid == true)
        {
            // 查找有可能看见此障碍物的机器人
            for (int j = 1; j < 7; j++)
            {
                if (m_agents[j].online == true)
                {
                    if (m_CalDist(m_agents[j].x, m_agents[j].y, m_trueobst[i].x, m_trueobst[i].y) < DIST_DISC_BALL)
                    {
                        // 需要为j号机器人增加此障碍物
                        // 1、在j号机器人的障碍物里搜索看是否存在此障碍物
                        int oi;
                        for (oi = 0; oi < OBST_NUM; oi++)
                        {
                            if (m_obst[oi].ID == j && m_obst[oi].valid == true &&
                                m_CalDist(m_obst[oi].x, m_obst[oi].y, m_trueobst[i].x, m_trueobst[i].y) < 20)
                            {
                                break;
                            }
                        }

                        // 2、根据oi判断是否有搜索到与要设置的障碍物（m_trueobst[i]）坐标接近的障碍物
                        if (oi < OBST_NUM)
                        {
                            // 与要设置的障碍物已经存在，放弃吧
                            break;
                        }

                        // 3、能执行到这，说明在已有障碍物里没有找到符合条件的障碍物，增加障碍物
                        for (oi = 0; oi < OBST_NUM; oi++)
                        {
                            if (m_obst[oi].valid == false)
                            {
                                // 找到一个空着的，赶紧跳出循环
                                break;
                            }
                        }

                        // 4、根据oi判断是否已经遍历所有障碍物
                        if (oi >= OBST_NUM)
                        {
                            oi = 0;
                        }

                        // 4、增加障碍物
                        m_obst[oi].ID = j;
                        m_obst[oi].x = m_trueobst[i].x;
                        m_obst[oi].y = m_trueobst[i].y;
                        m_obst[oi].size = m_trueobst[i].size;
                        m_obst[oi].valid = true;
                    }
                }
            }
        }
    }
}

void CUPFullField::m_ClearObstsNear(int inx, int iny, int inrag)
{
    for (int i = 0; i < OBST_NUM; i++)
    {
        if (m_obst[i].valid == true)
        {
            if (m_CalDist(inx, iny, m_obst[i].x, m_obst[i].y) < inrag)
            {
                m_obst[i].valid = false;
            }
        }
    }
}

static int nPassedWatch = 1;
void CUPFullField::m_BehPassMove(uagent *inagent)
{
    // 目前进攻队员是向球移动
    if (inagent->x != m_recball.x || inagent->y != m_recball.y)
    {
        // 未带球时趋向球
        int target_angle = m_Loca_CalAngle(inagent->x, inagent->y, m_recball.x, m_recball.y);

        if (nPassedWatch <= 0)
        {
            m_BehMoveto(inagent, m_recball.x, m_recball.y, target_angle);
        }
        else
        {
            nPassedWatch--;
        }
    }
    else
    {
        // 接触到球以后
        m_AgUDPs[inagent->ID].AttentionKick(inagent->x, inagent->y, inagent->target_x, inagent->target_y);
        m_nBallTarX = inagent->target_x;
        m_nBallTarY = inagent->target_y;

        // 出脚后停一秒钟
        nPassedWatch = 10;

        if (m_bAtkIm == true)
        {
            nHoldingBall--;
            if (nHoldingBall <= 0)
            {
                nHoldingBall = 100;
                m_bBallMove = true;
            }
        }
    }
}

void CUPFullField::m_BallMoveTo(int inTarX, int inTarY)
{
}

void CUPFullField::m_BehFollow(uagent *inagent)
{
    inagent->moveto_x = m_recball.x - 100;
    inagent->moveto_y = m_recball.y;
    m_BehMoveto(inagent);
}

void CUPFullField::m_BehDEF_Ball(uagent *inagent)
{
    inagent->moveto_x = m_recball.x - 200;
    inagent->moveto_y = m_recball.y;
    m_BehMoveto(inagent);
}

void CUPFullField::m_BehDEF_Goal(uagent *inagent)
{
    m_BehDef(inagent);
}

void CUPFullField::m_BehCatch(uagent *inagent)
{
    if (inagent == NULL)
    {
        return;
    }
    int tmp = 0;
    int v = 0;
    // x的调整
    if (inagent->x != inagent->moveto_x)
    {
        if (inagent->x > inagent->moveto_x)
        {
            tmp = inagent->x - inagent->moveto_x;
            tmp = tmp * inagent->moveto_spd / 255;
            tmp /= 40;
            tmp = tmp * m_nCycle / 15;
            if (tmp <= 0)
            {
                tmp = 1;
            }
            inagent->x -= tmp;
        }
        else
        {
            tmp = inagent->moveto_x - inagent->x;
            tmp = tmp * inagent->moveto_spd / 255;
            tmp /= 40;
            tmp = tmp * m_nCycle / 15;
            if (tmp <= 0)
            {
                tmp = 1;
            }
            inagent->x += tmp;
        }
    }
    v += tmp;

    // y的调整
    if (inagent->y != inagent->moveto_y)
    {
        if (inagent->y > inagent->moveto_y)
        {
            tmp = inagent->y - inagent->moveto_y;
            tmp = tmp * inagent->moveto_spd / 255;
            tmp /= 40;
            tmp = tmp * m_nCycle / 15;
            if (tmp <= 0)
            {
                tmp = 1;
            }
            inagent->y -= tmp;
        }
        else
        {
            tmp = inagent->moveto_y - inagent->y;
            tmp = tmp * inagent->moveto_spd / 255;
            tmp /= 40;
            tmp = tmp * m_nCycle / 15;
            if (tmp <= 0)
            {
                tmp = 1;
            }
            inagent->y += tmp;
        }
    }
    v += tmp;

    // 还有angle调整
    int target_angle = m_Loca_CalAngle(inagent->x, inagent->y, m_recball.x, m_recball.y);
    if (inagent->angle > target_angle)
    {
        inagent->angle--;
    }
    else
    {
        inagent->angle++;
    }

    // 速度方向
    // direct.x = (long)(12*cos(m_agents[i].angle*3.14/180));
    // direct.y = (long)(-12*sin(m_agents[i].angle*3.14/180));
    inagent->V = abs(v);
    if (inagent->moveto_x != inagent->x)
    {
        double dangle;
        dangle = (double)(inagent->moveto_y - inagent->y) / (inagent->moveto_x - inagent->x);

        inagent->V_Angle = atan(dangle) * 180 / 3.14;
        if (inagent->moveto_x - inagent->x < 0)
        {
            inagent->V_Angle = 180 + inagent->V_Angle;
        }

        while (inagent->V_Angle < 0)
        {
            inagent->V_Angle += 360;
        }

        while (inagent->V_Angle > 360)
        {
            inagent->V_Angle -= 360;
        }
    }
    else
    {
        if (inagent->moveto_y > inagent->y)
        {
            inagent->V_Angle = 90;
        }
        else
        {
            inagent->V_Angle = 270;
        }
    }

    // 移动精确到位
    if (abs(inagent->x - inagent->moveto_x) < 2)
    {
        inagent->x = inagent->moveto_x;
    }
    if (abs(inagent->y - inagent->moveto_y) < 2)
    {
        inagent->y = inagent->moveto_y;
    }

    // 球靠近就吸住
    if (abs(inagent->x - m_recball.x) < 5)
    {
        inagent->x = m_recball.x;
    }
    if (abs(inagent->y - m_recball.y) < 5)
    {
        inagent->y = m_recball.y;
    }
}

double CUPFullField::m_CalAngle(int inx1, int iny1, int inVx, int inVy, int inx2, int iny2)
{
    int a2 = (inx1 - inx2) * (inx1 - inx2) + (iny1 - iny2) * (iny1 - iny2);
    int b2 = (inx1 - inVx) * (inx1 - inVx) + (iny1 - inVy) * (iny1 - inVy);
    int c2 = (inx2 - inVx) * (inx2 - inVx) + (iny2 - inVy) * (iny2 - inVy);

    int b = (int)sqrt(b2);
    int c = (int)sqrt(c2);

    if (b == 0 || c == 0)
    {
        return 360;
    }

    double Angle = acos((double)(c2 + b2 - a2) / (2 * b * c));

    if (Angle == 0)
    {
        if (a2 > c2)
        {
            return 180;
        }
        else
        {
            return 0;
        }
    }

    // 换算成角度值
    Angle = Angle * 180 / 3.14159;

    return Angle;
}

int CUPFullField::m_Loca_CalAngle(int inXbase, int inYbase, int inXtarg, int inYtarg)
{
    double retAngle;
    if (inXbase != inXtarg)
    {
        // 分母不为零
        int dx = inXtarg - inXbase;
        double dy = inYtarg - inYbase;
        retAngle = atan(dy / dx);

        // 换算成角度
        retAngle = retAngle * 180 / 3.14158;

        // 象限的修正
        if (retAngle < 0)
        {
            if (dy > 0)
            {
                retAngle += 180;
            }
        }
        else
        {
            if (dy < 0)
            {
                retAngle += 180;
            }
        }

        // 修正
        while (retAngle < 0)
        {
            retAngle += 360;
        }
        while (retAngle > 360)
        {
            retAngle -= 360;
        }

        // 修正
        if (dy == 0)
        {
            if (dx > 0)
            {
                retAngle = 0;
            }
            else
            {
                retAngle = 180;
            }
        }
    }
    else
    {
        // inXbase == inXtarg
        if (inYtarg > inYbase)
        {
            retAngle = 90;
        }
        else
        {
            retAngle = 270;
        }
    }

    return (int)retAngle;
}
