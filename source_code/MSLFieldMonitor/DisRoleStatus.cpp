// DisRoleStatus.cpp : implementation file
//

#include "stdafx.h"
#include "MSLFieldMonitor.h"
#include "DisRoleStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int gridx[12] = {20, 100, 160, 220, 280, 340, 400, 460, 520, 580, 640, 700};
static CString strTitle[12];

/////////////////////////////////////////////////////////////////////////////
// CDisRoleStatus

CDisRoleStatus::CDisRoleStatus()
{
    pTac = NULL;
    pField = NULL;

    for (int i = 0; i < 5; i++)
    {
        m_rcRole[i].left = 2;
        m_rcRole[i].right = 700;
        m_rcRole[i].top = 15 + i * 17;
        m_rcRole[i].bottom = m_rcRole[i].top + 16;
    }

    strTitle[0] = "ID";
    strTitle[1] = "角色";
    strTitle[2] = "DF_角度";
    strTitle[3] = "DF_距离";
    strTitle[4] = "Dest_角度";
    strTitle[5] = "Dest_X";
    strTitle[6] = "Dest_Y";
    strTitle[7] = "PT_X";
    strTitle[8] = "PT_Y";
    strTitle[9] = "RB_X";
    strTitle[10] = "RB_Y";
    strTitle[11] = "A_ID";

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
                                "Arial"));                // lpszFacename

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
                               "Arial"));                // lpszFacename
}

CDisRoleStatus::~CDisRoleStatus()
{
}

BEGIN_MESSAGE_MAP(CDisRoleStatus, CStatic)
//{{AFX_MSG_MAP(CDisRoleStatus)
ON_WM_PAINT()
ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisRoleStatus message handlers

void CDisRoleStatus::OnPaint()
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
    CBrush brBlue(RGB(0, 0, 255));

    CBrush brAttack(RGB(200, 100, 100));
    CBrush brPassMove(RGB(100, 155, 50));
    CBrush brCatchMove(RGB(0, 155, 150));
    CBrush brCatchFocus(RGB(100, 50, 150));
    for (int i = 0; i < 5; i++)
    {
        m_rcRole[i].right = m_FrameRect.right - 2;
        // 根据指令填充背景
        switch (pTac->role[i].ctrl)
        {
        case CTRL_ATTACK:
            dcMemory.FillRect(m_rcRole[i], &brAttack);
            break;
        case CTRL_PASS_MOVE:
            dcMemory.FillRect(m_rcRole[i], &brPassMove);
            break;
        case CTRL_CATCH_MOVE:
            dcMemory.FillRect(m_rcRole[i], &brCatchMove);
            break;
        case CTRL_CATCH_FOCUS:
            dcMemory.FillRect(m_rcRole[i], &brCatchFocus);
            break;
        default:
            dcMemory.FillRect(m_rcRole[i], &brBlue);
            break;
        }
    }

    // 标题
    dcMemory.SelectObject(&fontTitle);
    dcMemory.SetTextColor(RGB(255, 255, 0));

    // 黑色分隔框
    CPen penBlack(PS_SOLID, 1, COLORREF(RGB(0, 0, 0)));
    dcMemory.SelectObject(&penBlack);
    for (i = 0; i < 12; i++)
    {
        if (i == 0)
        {
            dcMemory.TextOut(2, 1, L"ID");
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

    if (pTac != NULL && pField != NULL)
    {
        CString strTxt;
        dcMemory.SelectObject(&fontText);
        dcMemory.SetTextColor(RGB(255, 255, 255));
        // 填写内容
        for (int i = 0; i < 5; i++)
        {
            /*if (pField->pDataSummary->agent[i].online == true)
            {
            dcMemory.SetTextColor(RGB(0, 0, 0));
            }
            else
            {
            dcMemory.SetTextColor(RGB(255, 255, 255));
            }*/
            // 1）ID
            strTxt.Format("%d", i + 1);
            dcMemory.TextOut(m_rcRole[i].left + 2, m_rcRole[i].top + 1, strTxt);

            // 2）Ctrl
            strTxt = pField->GetCTRLString(pTac->role[i].ctrl);
            dcMemory.TextOut(gridx[0] + 2, m_rcRole[i].top + 1, strTxt);

            // 3）DF_Ang
            strTxt.Format("%d", pTac->role[i].def_angle);
            dcMemory.TextOut(gridx[1] + 2, m_rcRole[i].top + 1, strTxt);

            // 4）DF_Dit
            strTxt.Format("%d", pTac->role[i].def_dist);
            dcMemory.TextOut(gridx[2] + 2, m_rcRole[i].top + 1, strTxt);

            // 5）Dest_Ang
            strTxt.Format("%d", pTac->role[i].nDest_angle);
            dcMemory.TextOut(gridx[3] + 2, m_rcRole[i].top + 1, strTxt);

            // 6）Dest_X
            strTxt.Format("%d", pTac->role[i].nDest_x);
            dcMemory.TextOut(gridx[4] + 2, m_rcRole[i].top + 1, strTxt);

            // 7）Dest_Y
            strTxt.Format("%d", pTac->role[i].nDest_y);
            dcMemory.TextOut(gridx[5] + 2, m_rcRole[i].top + 1, strTxt);

            // 8）PT_X
            strTxt.Format("%d", pTac->role[i].nPassTo_x);
            dcMemory.TextOut(gridx[6] + 2, m_rcRole[i].top + 1, strTxt);

            // 9）PT_Y
            strTxt.Format("%d", pTac->role[i].nPassTo_y);
            dcMemory.TextOut(gridx[7] + 2, m_rcRole[i].top + 1, strTxt);

            // 10）RB_X
            strTxt.Format("%d", pTac->role[i].re_ballx);
            dcMemory.TextOut(gridx[8] + 2, m_rcRole[i].top + 1, strTxt);

            // 11）RB_Y
            strTxt.Format("%d", pTac->role[i].re_bally);
            dcMemory.TextOut(gridx[9] + 2, m_rcRole[i].top + 1, strTxt);

            // 12）A_ID
            strTxt.Format("%d", pTac->role[i].allocated_ID);
            dcMemory.TextOut(gridx[10] + 2, m_rcRole[i].top + 1, strTxt);
        }
        // 释放图片
    }

    dc.BitBlt(0, 0, m_FrameRect.right, m_FrameRect.bottom, &dcMemory, 0, 0, SRCCOPY);
    m_bk.DeleteObject();
    dcMemory.DeleteDC();

    // Do not call CStatic::OnPaint() for painting messages
}

void CDisRoleStatus::OnSize(UINT nType, int cx, int cy)
{
    CStatic::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
}
