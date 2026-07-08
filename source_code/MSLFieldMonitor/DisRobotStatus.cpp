// DisRobotStatus.cpp : implementation file
//

#include "stdafx.h"
#include "MSLFieldMonitor.h"
#include "DisRobotStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//					序号，状态，坐标，球坐标，网络 串口 视觉 组播 姿态	CPU  程序版本 电脑电量  机器电量
static int gridx[13] = {20, 80, 150, 250, 290, 320, 350, 380, 410, 500, 560, 640, 720};
static CString strTitle[13];
/////////////////////////////////////////////////////////////////////////////
// CDisRobotStatus

CDisRobotStatus::CDisRobotStatus()
{
    // 图标
    bmpACPlug.LoadBitmap(IDB_BMP_AC_PLUG);
    bmpBatteryCharging.LoadBitmap(IDB_BMP_BATTERY_CHARGING);

    for (int i = 0; i < 5; i++)
    {
        m_rcRobot[i].left = 2;
        m_rcRobot[i].right = 700;
        m_rcRobot[i].top = 15 + i * 17;
        m_rcRobot[i].bottom = m_rcRobot[i].top + 16;
    }

    strTitle[0] = "序号";
    strTitle[1] = "状态";
    strTitle[2] = "坐标";
    strTitle[3] = "球坐标";
    strTitle[4] = "网络";
    strTitle[5] = "串口";
    strTitle[6] = "视觉";
    strTitle[7] = "组播";
    strTitle[8] = "姿态";
    strTitle[9] = "CPU";
    strTitle[10] = "程序版本";
    strTitle[11] = "电脑电量";
    strTitle[12] = "机器电量";

    pField = NULL;
    pCoachUDP = NULL;

    // title
    VERIFY(fontTitle.CreateFont(14,                       // nHeight
                                0,                        // nWidth
                                0,                        // nEscapement
                                0,                        // nOrientation
                                FW_ULTRABOLD,             // nWeight
                                FALSE,                    // bItalic
                                FALSE,                    // bUnderline
                                0,                        // cStrikeOut
                                ANSI_CHARSET,             // nCharSet
                                OUT_DEFAULT_PRECIS,       // nOutPrecision
                                CLIP_DEFAULT_PRECIS,      // nClipPrecision
                                DEFAULT_QUALITY,          // nQuality
                                DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
                                "Aria"));                 // lpszFacename

    VERIFY(fontText.CreateFont(14,                       // nHeight
                               0,                        // nWidth
                               0,                        // nEscapement
                               0,                        // nOrientation
                               FW_REGULAR,               // nWeight
                               FALSE,                    // bItalic
                               FALSE,                    // bUnderline
                               0,                        // cStrikeOut
                               ANSI_CHARSET,             // nCharSet
                               OUT_DEFAULT_PRECIS,       // nOutPrecision
                               CLIP_DEFAULT_PRECIS,      // nClipPrecision
                               DEFAULT_QUALITY,          // nQuality
                               DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
                               "Aria"));                 // lpszFacename
}

CDisRobotStatus::~CDisRobotStatus()
{
    // 图标
    bmpACPlug.DeleteObject();
    bmpBatteryCharging.DeleteObject();
}

BEGIN_MESSAGE_MAP(CDisRobotStatus, CStatic)
//{{AFX_MSG_MAP(CDisRobotStatus)
ON_WM_PAINT()
ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisRobotStatus message handlers

void CDisRobotStatus::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // TODO: Add your message handler code here
    dcMemory.CreateCompatibleDC(&dc);

    // 获取当前尺寸，将内存dc撑开
    GetClientRect(&m_FrameRect);
    m_bk.CreateCompatibleBitmap(&dc, m_FrameRect.right, m_FrameRect.bottom);
    CBitmap *oldbmp = dcMemory.SelectObject(&m_bk);

    CPoint pntBegin, pntEnd;

    // 填充背景
    CBrush BrBlack(RGB(0, 0, 0));
    dcMemory.FillRect(&m_FrameRect, &BrBlack);
    dcMemory.SetBkMode(TRANSPARENT);

    // 填充机器人信息背景
    CBrush brGreen(RGB(0, 150, 0));
    CBrush brGrey(RGB(100, 100, 100));
    CBrush brAttack(RGB(200, 50, 50));
    CBrush brPassMove(RGB(200, 50, 50));
    CBrush brCatchMove(RGB(50, 50, 200));
    CBrush brCatchFocus(RGB(0, 0, 100));
    for (int i = 0; i < 5; i++)
    {
        m_rcRobot[i].right = m_FrameRect.right - 2;
        if (pField->pDataSummary->agent[i].online == true)
        {
            // dcMemory.FillRect(m_rcRobot[i], &brGreen);
            switch (pField->pDataSummary->agent[i].status)
            {
            case CTRL_ATTACK:
                dcMemory.FillRect(m_rcRobot[i], &brAttack);
                break;
            case CTRL_PASS_MOVE:
                dcMemory.FillRect(m_rcRobot[i], &brPassMove);
                break;
            case CTRL_CATCH_MOVE:
                dcMemory.FillRect(m_rcRobot[i], &brCatchMove);
                break;
            case CTRL_CATCH_FOCUS:
                dcMemory.FillRect(m_rcRobot[i], &brCatchFocus);
                break;
            case ROBST_ERR:
                dcMemory.FillRect(m_rcRobot[i], &brGrey);
                break;
            default:
                dcMemory.FillRect(m_rcRobot[i], &brGreen);
                break;
            }
        }
        else
        {
            dcMemory.FillRect(m_rcRobot[i], &brGrey);
        }
    }

    // 标题
    dcMemory.SelectObject(&fontTitle);
    dcMemory.SetTextColor(RGB(255, 255, 0));

    // 黑色分隔框
    CPen penBlack(PS_SOLID, 1, COLORREF(RGB(0, 0, 0)));
    dcMemory.SelectObject(&penBlack);
    for (i = 0; i < 13; i++)
    {
        if (i == 0)
        {
            dcMemory.TextOut(2, 1, "ID");
        }
        else
        {
            dcMemory.TextOut(gridx[i - 1] + 2, 1, strTitle[i]);
        }
        pntBegin.x = gridx[i];
        pntBegin.y = 0;
        pntEnd.x = pntBegin.x;
        pntEnd.y = 132;
        dcMemory.MoveTo(pntBegin);
        dcMemory.LineTo(pntEnd);
    }

    if (pField != NULL)
    {
        CString strTxt;
        dcMemory.SelectObject(&fontText);
        // 加载图标
        dcACPlug.CreateCompatibleDC(&dc);
        dcACPlug.SelectObject(bmpACPlug);
        dcBatteryCharging.CreateCompatibleDC(&dc);
        dcBatteryCharging.SelectObject(bmpBatteryCharging);

        // 填写内容
        for (int i = 0; i < 5; i++)
        {
            if (pField->pDataSummary->agent[i].online == true)
            {
                dcMemory.SetTextColor(RGB(255, 255, 255));
                if (pField->pDataSummary->agent[i].status == ROBST_ERR)
                {
                    dcMemory.SetTextColor(RGB(150, 150, 150));
                }
            }
            else
            {
                dcMemory.SetTextColor(RGB(150, 150, 150));
            }
            // 1）ID
            strTxt.Format("%d", i + 1);
            dcMemory.TextOut(m_rcRobot[i].left + 2, m_rcRobot[i].top + 1, strTxt);

            // 2）状态
            if (pField->pDataSummary->agent[i].online == true)
            {
                strTxt = pField->GetCTRLString(pField->pDataSummary->agent[i].status);
                dcMemory.TextOut(gridx[0] + 2, m_rcRobot[i].top + 1, strTxt);
            }
            else
            {
                dcMemory.TextOut(gridx[0] + 2, m_rcRobot[i].top + 1, "OFFLINE");
                continue;
            }

            // 3）坐标
            strTxt.Format("(%d,%d)", pField->pDataSummary->agent[i].x, pField->pDataSummary->agent[i].y);
            dcMemory.TextOut(gridx[1] + 2, m_rcRobot[i].top + 1, strTxt);

            // 4）球坐标
            if (pField->pDataSummary->agent[i].foundball == true)
            {
                strTxt.Format("(%d,%d) %d", pField->pDataSummary->agent[i].ball.x,
                              pField->pDataSummary->agent[i].ball.y, pField->pDataSummary->agent[i].dist_ball);
            }
            else
            {
                strTxt = "--";
            }

            dcMemory.TextOut(gridx[2] + 2, m_rcRobot[i].top + 1, strTxt);

            // 5）数据包率
            strTxt.Format("%d /%d", pCoachUDP->m_recvLast[i], pCoachUDP->m_recvCnt[i]);
            dcMemory.TextOut(gridx[3] + 2, m_rcRobot[i].top + 1, strTxt);

            // 6）串口
            strTxt.Format("%d", pField->pDataSummary->agent[i].nSerialPort_FPS);
            dcMemory.TextOut(gridx[4] + 2, m_rcRobot[i].top + 1, strTxt);
            // 7）视觉";
            strTxt.Format("%d", pField->pDataSummary->agent[i].nVision_FPS);
            dcMemory.TextOut(gridx[5] + 2, m_rcRobot[i].top + 1, strTxt);
            // 8）组播";
            strTxt.Format("%d", pField->pDataSummary->agent[i].nMulticast_FPS);
            dcMemory.TextOut(gridx[6] + 2, m_rcRobot[i].top + 1, strTxt);
            // 9）姿态";
            strTxt.Format("%d", pField->pDataSummary->agent[i].nAttitude_FPS);
            dcMemory.TextOut(gridx[7] + 2, m_rcRobot[i].top + 1, strTxt);

            // 10）CPU
            //  10-1 占用率
            strTxt.Format("%d%%", pField->pDataSummary->agent[i].nComputer_CPULoadPercent);
            dcMemory.TextOut(gridx[8] + 2, m_rcRobot[i].top + 1, strTxt);
            // 10-2 主频
            strTxt.Format("[%.2f GHz]", pField->pDataSummary->agent[i].fComputer_CPUFrequency / 1000);
            dcMemory.TextOut(gridx[8] + 30, m_rcRobot[i].top + 1, strTxt);

            // 11）软件版本号
            strTxt.Format("%f", pField->pDataSummary->agent[i].fComputer_SoftwareVer);
            dcMemory.TextOut(gridx[9] + 2, m_rcRobot[i].top + 1, strTxt);

            // 12）电脑电源
            strTxt.Format("%d%%", pField->pDataSummary->agent[i].nComputer_BatteryPercent);
            dcMemory.TextOut(gridx[10] + 2, m_rcRobot[i].top + 1, strTxt);
            // 12-1 充电图标 (1-高 2-低 4-极低 8-充电中 128(0x80)-没有电池)
            if ((pField->pDataSummary->agent[i].nComputer_BatteryFlag & 0x08) > 0)
            {
                dcMemory.BitBlt(gridx[10] + 40, m_rcRobot[i].top, 16, 16, &dcBatteryCharging, 0, 0, SRCCOPY);
            }
            // 12-2 AC电源插入图标 (0-offline 1-online)
            if (pField->pDataSummary->agent[i].nComputer_ACPlug > 0)
            {
                dcMemory.BitBlt(gridx[10] + 60, m_rcRobot[i].top, 16, 16, &dcACPlug, 0, 0, SRCCOPY);
            }

            // 13) 机器电源
            float fRobPow = pField->pDataSummary->agent[i].nRobot_Power;
            fRobPow /= 10;
            strTxt.Format("%.1f", fRobPow);

            dcMemory.TextOut(gridx[11] + 2, m_rcRobot[i].top + 1, strTxt);
            // 13-1 充电图标
            if (pField->pDataSummary->agent[i].nRobot_Charging > 0)
            {
                dcMemory.BitBlt(gridx[11] + 40, m_rcRobot[i].top, 16, 16, &dcBatteryCharging, 0, 0, SRCCOPY);
            }
        }

        dcBatteryCharging.DeleteDC();
        dcACPlug.DeleteDC();
    }

    dc.BitBlt(0, 0, m_FrameRect.right, m_FrameRect.bottom, &dcMemory, 0, 0, SRCCOPY);
    m_bk.DeleteObject();
    dcMemory.DeleteDC();

    // Do not call CStatic::OnPaint() for painting messages
}

void CDisRobotStatus::OnSize(UINT nType, int cx, int cy)
{
    CStatic::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
}
