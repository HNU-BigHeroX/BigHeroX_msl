// DisSideClr.cpp : implementation file
//

#include "stdafx.h"
#include "MSLFieldMonitor.h"
#include "DisSideClr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDisSideClr

CDisSideClr::CDisSideClr()
{
    VERIFY(font.CreateFont(32,                       // nHeight
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

    nFlag = 1; // 0-Magenta 1-Cyan
    nStep = 1;
}

CDisSideClr::~CDisSideClr()
{
}

BEGIN_MESSAGE_MAP(CDisSideClr, CStatic)
//{{AFX_MSG_MAP(CDisSideClr)
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_MOUSEMOVE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisSideClr message handlers

void CDisSideClr::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // TODO: Add your message handler code here
    // 创建缓存DC
    dcMemory.CreateCompatibleDC(&dc);
    // 获取当前尺寸，将内存dc撑开
    CRect m_FrameRect;
    GetClientRect(&m_FrameRect);
    Bitmap.CreateCompatibleBitmap(&dc, m_FrameRect.right, m_FrameRect.bottom);
    dcMemory.SetBkMode(TRANSPARENT);
    CBitmap *oldbmp = dcMemory.SelectObject(&Bitmap);

    // 填充两个矩形(左侧矩形magenta颜色，右侧矩形cyan颜色)
    CBrush bkMagenta(RGB(255, 0, 255));
    CBrush bkCyan(RGB(0, 0, 255));
    CRect rc;
    rc = m_FrameRect;
    rc.right = nCurPosition;
    dcMemory.FillRect(rc, &bkMagenta);
    rc = m_FrameRect;
    rc.left = nCurPosition;
    dcMemory.FillRect(rc, &bkCyan);

    if (nCurPosition == nDividLine)
    {
        // 分界线滑动完毕才显示文字

        CFont *def_font = dcMemory.SelectObject(&font);
        //[0]Magenta
        if (nFlag == 0)
        {
            // 文字
            dcMemory.SetTextColor(RGB(100, 0, 0));
            dcMemory.TextOut(2, 2, L"Magenta");
        }
        else
            //[1]Cyan
            if (nFlag == 1)
            {
                dcMemory.SetTextColor(RGB(200, 200, 255));
                dcMemory.TextOut(m_FrameRect.right - 80, 2, L"Cyan");
            }

        dcMemory.SelectObject(def_font);
    }

    // dc拷贝
    dc.BitBlt(0, 0, m_FrameRect.Width(), m_FrameRect.Height(), &dcMemory, 0, 0, SRCCOPY);

    dcMemory.DeleteDC();
    Bitmap.DeleteObject();

    // Do not call CStatic::OnPaint() for painting messages
}

void CDisSideClr::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    if (nFlag == 0) // magenta
    {
        // 点右侧切换到cyan
        CRect rc;
        GetClientRect(&rc);
        int rcWidth = rc.Width();
        rc.left = nCurPosition;
        if (point.x > rc.left && point.x < rc.right && point.y > rc.top && point.y < rc.bottom)
        {
            nDividLine = rcWidth / 8;
            nFlag = 1;
        }
        SetTimer(1, 30, NULL);
        Invalidate(FALSE);
    }
    else if (nFlag == 1) // cyan
    {
        // 点左侧切换到magenta
        CRect rc;
        GetClientRect(&rc);
        int rcWidth = rc.Width();
        rc.right = nCurPosition;
        if (point.x > rc.left && point.x < rc.right && point.y > rc.top && point.y < rc.bottom)
        {
            nDividLine = rcWidth - rcWidth / 8;
            nFlag = 0;
        }
        SetTimer(1, 30, NULL);
        Invalidate(FALSE);
    }

    CStatic::OnLButtonDown(nFlags, point);
}

void CDisSideClr::OnSize(UINT nType, int cx, int cy)
{
    CStatic::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
}

void CDisSideClr::OnTimer(UINT nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    if (nIDEvent == 1)
    {
        if (nCurPosition == nDividLine)
        {
            KillTimer(1);
        }
        else
        {
            nStep = (nDividLine - nCurPosition) / 4;
            if (nStep == 0)
            {
                if (nCurPosition < nDividLine)
                {
                    nStep = 1;
                }
                else
                {
                    nStep = -1;
                }
            }
            nCurPosition += nStep;
        }
        Invalidate(FALSE);
    }

    CStatic::OnTimer(nIDEvent);
}

void CDisSideClr::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    if (nFlag == 0) // magenta
    {
        CRect rc;
        GetClientRect(&rc);
        rc.left = nDividLine;
        if (point.x > rc.left && point.x < rc.right && point.y > rc.top && point.y < rc.bottom)
        {
            HCURSOR hCur = LoadCursor(NULL, IDC_HAND);
            ::SetCursor(hCur);
        }
    }
    else if (nFlag == 1) // cyan
    {
        // 点左侧切换到magenta
        CRect rc;
        GetClientRect(&rc);
        rc.right = nDividLine;
        if (point.x > rc.left && point.x < rc.right && point.y > rc.top && point.y < rc.bottom)
        {
            HCURSOR hCur = LoadCursor(NULL, IDC_HAND);
            ::SetCursor(hCur);
        }
    }

    CStatic::OnMouseMove(nFlags, point);
}
