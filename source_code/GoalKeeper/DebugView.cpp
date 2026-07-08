// DebugView.cpp : implementation file
//

#include "stdafx.h"
#include "VoyVideo.h"
#include "DebugView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDebugView

CDebugView::CDebugView()
{
    m_pBuf = NULL;
    m_nWidth = 0;
    m_nHeight = 0;

    m_dcMem.CreateCompatibleDC(NULL);
    Bitmap.CreateCompatibleBitmap(&m_dcMem, 640, 480);
    m_dcMem.SelectObject(&Bitmap);

    memset(&m_bih, 0, sizeof(m_bih));
    m_bih.biSize = sizeof(m_bih);
    m_bih.biWidth = 640;
    m_bih.biHeight = 480;
    m_bih.biPlanes = 1;
    m_bih.biBitCount = 24;

    m_nDisWidth = 640;
    m_nDisHeight = 480;

    m_bInitDC = false;
}

CDebugView::~CDebugView()
{
    m_bInitDC = false;
    m_dcMem.DeleteDC();
    Bitmap.DeleteObject();
    ReleaseBuf();
}

BEGIN_MESSAGE_MAP(CDebugView, CStatic)
//{{AFX_MSG_MAP(CDebugView)
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDebugView message handlers

void CDebugView::InitSize(int inWidth, int inHeight, CDC *inpDC)
{
    if (inWidth != m_nWidth || inHeight != m_nHeight)
    {
        ReleaseBuf();
        m_pBuf = new UCHAR[inWidth * inHeight * 3];
    }
    m_nWidth = inWidth;
    m_nHeight = inHeight;

    m_nDisWidth = inWidth;
    m_nDisHeight = inHeight;

    memset(m_pBuf, 255, m_nWidth * m_nHeight * 3);

    m_dcMem.DeleteDC();
    Bitmap.DeleteObject();
    m_dcMem.CreateCompatibleDC(inpDC);
    Bitmap.CreateCompatibleBitmap(inpDC, inWidth, inHeight);
    m_dcMem.SelectObject(&Bitmap);
}

void CDebugView::ReleaseBuf()
{
    if (m_pBuf != NULL)
    {
        delete[] m_pBuf;
        m_pBuf = NULL;
    }
}

void CDebugView::Display()
{
    if (m_pBuf != NULL)
    {
        PAINTSTRUCT ps;
        BeginPaint(&ps);

        SetStretchBltMode(m_dcMem.m_hDC, COLORONCOLOR);
        StretchDIBits(
            // 目标
            m_dcMem.m_hDC, 0, 0, m_nDisWidth, m_nDisHeight,
            // 源
            0, 0, m_nWidth, m_nHeight, m_pBuf, (BITMAPINFO *)&m_bih, DIB_RGB_COLORS, SRCCOPY);

        EndPaint(&ps);
        Invalidate(FALSE);
    }
}

void CDebugView::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // TODO: Add your message handler code here
    if (m_bInitDC == false)
    {
        InitSize(640, 480, &dc);
        m_bInitDC = true;
    }
    //////////////////////////////////////////////////////////////////////////
    // 	PAINTSTRUCT ps;
    // 	BeginPaint(&ps);
    //
    // 	SetStretchBltMode(dc.m_hDC, COLORONCOLOR);
    // 	StretchDIBits(
    // 		//目标
    // 		dc.m_hDC, 0, 0,
    // 		m_nDisWidth, m_nDisHeight,
    // 		//源
    // 		0, 0, m_nWidth, m_nHeight,
    // 		m_pBuf,
    // 		(BITMAPINFO*) &m_bih,
    // 		DIB_RGB_COLORS,
    // 		SRCCOPY );
    //
    // 		EndPaint(&ps);
    //////////////////////////////////////////////////////////////////////////
    dc.BitBlt(0, 0, m_nDisWidth, m_nDisHeight, &m_dcMem, 0, 0, SRCCOPY);
    // Do not call CStatic::OnPaint() for painting messages
}

void CDebugView::PointColor(int inX, int inY, UCHAR inR, UCHAR inG, UCHAR inB)
{
    if (inX < 0 || inX >= m_nWidth)
    {
        return;
    }

    if (inY < 0 || inY >= m_nHeight)
    {
        return;
    }

    if (m_pBuf != NULL)
    {
        int tmp = inY * m_nWidth * 3 + inX * 3;
        m_pBuf[tmp] = inB;
        m_pBuf[tmp + 1] = inG;
        m_pBuf[tmp + 2] = inR;
    }
    // Display();
}

void CDebugView::DrawFocus(int inX, int inY, UCHAR inR, UCHAR inG, UCHAR inB)
{
    for (int i = -5; i < 6; i++)
    {
        PointColor(inX + i, inY, inR, inG, inB);
        PointColor(inX, inY + i, inR, inG, inB);
    }
}
