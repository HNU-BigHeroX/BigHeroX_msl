// StillView.cpp : implementation file
//

#include "stdafx.h"
#include "Thresdhold.h"
#include "StillView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStillView

CStillView::CStillView()
{
    /*	m_nWidth = 640;
        m_nHeight = 504;
        m_pBuf = new UCHAR[m_nWidth*m_nHeight*3];*/
    m_nWidth = m_nHeight = 0;
    m_pBuf = NULL;

    memset(&m_bih, 0, sizeof(m_bih));
    m_bih.biSize = sizeof(m_bih);
    m_bih.biWidth = 640;
    m_bih.biHeight = 504;
    m_bih.biPlanes = 1;
    m_bih.biBitCount = 24;

    m_bPress = false;

    m_nPartbufsize = 640 * 504 * 3;
    m_pPartbuf = new UCHAR[m_nPartbufsize];
    m_nPartWidth = 0;
    m_nPartHeight = 0;
    mode = 0;
    pPart = pPartH = pPartS = pPartI = NULL;
    pColor = NULL;
    m_SelRect.left = m_SelRect.top = 0;
    m_SelRect.right = m_SelRect.bottom = 80;

    m_lastpoint.x = 40;
    m_lastpoint.y = 40;
    m_range = 40;
    m_fpsDis = 0;
    m_nCount = 0;
    m_bEnable = true;
    m_bImgUpdate = false;
}

CStillView::~CStillView()
{
    if (m_pBuf != NULL)
    {
        delete[] m_pBuf;
        m_pBuf = NULL;
    }
    if (m_pPartbuf != NULL)
    {
        delete[] m_pPartbuf;
        m_pPartbuf = NULL;
    }
}

BEGIN_MESSAGE_MAP(CStillView, CStatic)
//{{AFX_MSG_MAP(CStillView)
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_WM_MOUSEWHEEL()
ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStillView message handlers

void CStillView::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: Add your message handler code here
    CDC dcmem;
    dcmem.CreateCompatibleDC(&dc);

    // 计算本体的尺寸
    GetClientRect(&rc);
    m_nStillWidth = rc.right - rc.left;
    m_nStillHeight = rc.bottom - rc.top;

    // 将dcmem撑开
    Bitmap.CreateCompatibleBitmap(&dc, rc.right, rc.bottom);
    CBitmap *oldbmp = dcmem.SelectObject(&Bitmap);

    CBrush *pBrush = new CBrush(RGB(150, 150, 150));
    dcmem.FillRect(&rc, pBrush);
    delete pBrush;

    if (m_pBuf != NULL)
    {
        PAINTSTRUCT ps;
        BeginPaint(&ps);

        SetStretchBltMode(dcmem.m_hDC, COLORONCOLOR);
        StretchDIBits(
            // 目标
            dcmem.m_hDC, 0, 0, m_nStillWidth, m_nStillHeight,
            // 源
            0, 0, m_nWidth, m_nHeight, m_pBuf, (BITMAPINFO *)&m_bih, DIB_RGB_COLORS, SRCCOPY);

        EndPaint(&ps);

        // 绘制边框
        if (/*m_bPress == false*/ m_fpsDis == 0)
        {
            CPen red(PS_SOLID, 1, RGB(255, 0, 0));
            CPen *oldpen = dcmem.SelectObject(&red);
            dcmem.MoveTo(m_SelRect.left, m_SelRect.top);
            dcmem.LineTo(m_SelRect.left, m_SelRect.bottom);
            dcmem.LineTo(m_SelRect.right, m_SelRect.bottom);
            dcmem.LineTo(m_SelRect.right, m_SelRect.top);
            dcmem.LineTo(m_SelRect.left, m_SelRect.top);
        }

        if (m_fpsDis > 0)
        {
            CString strfps;
            strfps.Format("%.2d", m_fpsDis);
            dcmem.TextOut(5, 5, strfps);
        }
    }

    // dc拷贝
    dc.BitBlt(0, 0, m_nStillWidth, m_nStillHeight, &dcmem, 0, 0, SRCCOPY);
    dcmem.DeleteDC();
    Bitmap.DeleteObject();

    // Do not call CStatic::OnPaint() for painting messages
}

void CStillView::BufImg(BYTE *inBuf, int inWidth, int inHeight)
{
    if (m_bEnable == false)
    {
        return;
    }
    if (m_bImgUpdate == false)
    {
        SetTimer(VT_DRAW, 30, NULL);
    }
    m_bImgUpdate = true;
    m_nCount++;

    // 如果发送来图像尺寸大于缓冲区，重新分配内存
    if (inWidth * inHeight > m_nWidth * m_nHeight)
    {
        if (m_pBuf != NULL)
        {
            delete m_pBuf;
            m_pBuf = NULL;
        }
        m_pBuf = new BYTE[inWidth * inHeight * 3];
    }
    if (m_pBuf != NULL)
    {
        m_nWidth = inWidth;
        m_nHeight = inHeight;
        m_bih.biWidth = inWidth;
        m_bih.biHeight = inHeight;
    }
    else
    {
        m_nWidth = m_nHeight = 0;
        return;
    }

    if (m_pBuf != NULL)
    {
        memcpy(m_pBuf, inBuf, inWidth * inHeight * 3);
        _ASSERTE(_CrtCheckMemory());
        // Invalidate();
    }

    if (mode == 0)
    {
        return;
    }

    // 空间转换
    m_rgb2hsi.InitBuf(m_pBuf, inWidth, inHeight);
    m_rgb2hsi.Convert();
    m_Stat();
}

void CStillView::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    if (m_pBuf == NULL || mode != 0)
        return;

    /*m_SelRect.left = m_SelRect.right = point.x;
    m_SelRect.top = m_SelRect.bottom = point.y;*/
    m_bPress = true;
    Invalidate();

    CStatic::OnLButtonDown(nFlags, point);
}

void CStillView::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    if (m_bPress == true && mode == 0)
    {
        m_SelRect.left = point.x - m_range;
        m_SelRect.right = point.x + m_range;
        m_SelRect.top = point.y - m_range;
        m_SelRect.bottom = point.y + m_range;
        // 纠正矩形信息
        int temp;
        if (m_SelRect.left > m_SelRect.right)
        {
            temp = m_SelRect.left;
            m_SelRect.left = m_SelRect.right;
            m_SelRect.right = temp;
        }
        if (m_SelRect.top > m_SelRect.bottom)
        {
            temp = m_SelRect.top;
            m_SelRect.top = m_SelRect.bottom;
            m_SelRect.bottom = temp;
        }
        if (m_SelRect.left < 0)
        {
            m_SelRect.right += -m_SelRect.left;
            m_SelRect.left = 0;
        }
        if (m_SelRect.right > rc.right)
        {
            m_SelRect.left = rc.right - 2 * m_range;
            m_SelRect.right = rc.right;
        }
        if (m_SelRect.top < 0)
        {
            m_SelRect.bottom += -m_SelRect.top;
            m_SelRect.top = 0;
        }
        if (m_SelRect.bottom > rc.bottom)
        {
            m_SelRect.top = rc.bottom - 2 * m_range;
            m_SelRect.bottom = rc.bottom;
        }

        // 计算截选的源图像尺寸
        m_srcx1 = m_SelRect.left * m_nWidth / m_nStillWidth;
        m_srcy1 = (m_nStillHeight - m_SelRect.bottom) * m_nHeight / m_nStillHeight;

        m_srcx2 = m_SelRect.right * m_nWidth / m_nStillWidth;
        m_srcy2 = (m_nStillHeight - m_SelRect.top) * m_nHeight / m_nStillHeight;

        // 截取此部分区域
        int width = m_srcx2 - m_srcx1;
        m_srcx2 += 4 - (width % 4);
        width = m_srcx2 - m_srcx1;
        int height = m_srcy2 - m_srcy1;
        if (width * height > m_nPartbufsize)
        {
            if (m_pPartbuf != NULL)
            {
                delete m_pPartbuf;
                m_pPartbuf = NULL;
            }
            m_nPartbufsize = width * height * 3;
            m_pPartbuf = new BYTE[m_nPartbufsize];
        }
        if (m_pPartbuf != NULL)
        {
            m_nPartWidth = width;
            m_nPartHeight = height;
        }
        else
        {
            m_nPartWidth = m_nPartHeight = 0;
            m_nPartbufsize = 0;
            return;
        }

        // 复制截取图像信息
        BYTE *p = m_pPartbuf;
        for (int i = m_srcy1; i < m_srcy2; i++)
        {
            for (int j = m_srcx1; j < m_srcx2; j++)
            {
                *p = m_pBuf[i * m_nWidth * 3 + j * 3];
                p++;
                *p = m_pBuf[i * m_nWidth * 3 + j * 3 + 1];
                p++;
                *p = m_pBuf[i * m_nWidth * 3 + j * 3 + 2];
                p++;
            }
        }
        m_ShowPart();
        Invalidate();
        m_lastpoint = point;
    }

    CStatic::OnMouseMove(nFlags, point);
}

void CStillView::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    if (m_bPress == true && mode == 0)
    {
        m_bPress = false;
        m_lastpoint = point;
        ////////////////////////////////////////////////////////////////
        m_SelRect.left = point.x - m_range;
        m_SelRect.right = point.x + m_range;
        m_SelRect.top = point.y - m_range;
        m_SelRect.bottom = point.y + m_range;
        // 纠正矩形信息
        int temp;
        if (m_SelRect.left > m_SelRect.right)
        {
            temp = m_SelRect.left;
            m_SelRect.left = m_SelRect.right;
            m_SelRect.right = temp;
        }
        if (m_SelRect.top > m_SelRect.bottom)
        {
            temp = m_SelRect.top;
            m_SelRect.top = m_SelRect.bottom;
            m_SelRect.bottom = temp;
        }
        if (m_SelRect.left < 0)
        {
            m_SelRect.right += -m_SelRect.left;
            m_SelRect.left = 0;
        }
        if (m_SelRect.right > rc.right)
        {
            m_SelRect.left = rc.right - 2 * m_range;
            m_SelRect.right = rc.right;
        }
        if (m_SelRect.top < 0)
        {
            m_SelRect.bottom += -m_SelRect.top;
            m_SelRect.top = 0;
        }
        if (m_SelRect.bottom > rc.bottom)
        {
            m_SelRect.top = rc.bottom - 2 * m_range;
            m_SelRect.bottom = rc.bottom;
        }

        // 计算截选的源图像尺寸
        m_srcx1 = m_SelRect.left * m_nWidth / m_nStillWidth;
        m_srcy1 = (m_nStillHeight - m_SelRect.bottom) * m_nHeight / m_nStillHeight;

        m_srcx2 = m_SelRect.right * m_nWidth / m_nStillWidth;
        m_srcy2 = (m_nStillHeight - m_SelRect.top) * m_nHeight / m_nStillHeight;

        // 截取此部分区域
        int width = m_srcx2 - m_srcx1;
        m_srcx2 += 4 - (width % 4);
        width = m_srcx2 - m_srcx1;
        int height = m_srcy2 - m_srcy1;
        if (width * height > m_nPartbufsize)
        {
            if (m_pPartbuf != NULL)
            {
                delete m_pPartbuf;
                m_pPartbuf = NULL;
            }
            m_nPartbufsize = width * height * 3;
            m_pPartbuf = new BYTE[m_nPartbufsize];
        }
        if (m_pPartbuf != NULL)
        {
            m_nPartWidth = width;
            m_nPartHeight = height;
        }
        else
        {
            m_nPartWidth = m_nPartHeight = 0;
            m_nPartbufsize = 0;
            return;
        }

        // 复制截取图像信息
        BYTE *p = m_pPartbuf;
        for (int i = m_srcy1; i < m_srcy2; i++)
        {
            for (int j = m_srcx1; j < m_srcx2; j++)
            {
                *p = m_pBuf[i * m_nWidth * 3 + j * 3];
                p++;
                *p = m_pBuf[i * m_nWidth * 3 + j * 3 + 1];
                p++;
                *p = m_pBuf[i * m_nWidth * 3 + j * 3 + 2];
                p++;
            }
        }
        m_ShowPart();
    }

    /*if (pColor != NULL)
    {
        pColor->pZoombuf = NULL;
    }*/

    CStatic::OnLButtonUp(nFlags, point);
}

void CStillView::m_ShowPart()
{
    /*if (pPart != NULL)
    {
        pPart->BufImg(m_pPartbuf,m_nPartWidth,m_nPartHeight);
        pPart->Invalidate();
    }*/
    // 空间转换
    m_rgb2hsi.InitBuf(m_pPartbuf, m_nPartWidth, m_nPartHeight);
    m_rgb2hsi.Convert();
    m_Stat();

    if (pColor != NULL)
    {
        pColor->pZoombuf = m_pPartbuf;
        pColor->nZoomWidth = m_nPartWidth;
        pColor->nZoomHeight = m_nPartHeight;
        pColor->m_bih.biWidth = m_nPartWidth;
        pColor->m_bih.biHeight = m_nPartHeight;
        pColor->GetFocus();
        pColor->Invalidate();
    }
}

void CStillView::m_Stat()
{
    if (pColor == NULL)
    {
        return;
    }
    // 初始化color的统计值
    pColor->CleanStat();

    // 从HSI矩阵里取值
    int H, S, I;
    for (int x = 0; x < m_nPartWidth; x++)
    {
        for (int y = 0; y < m_nPartHeight; y++)
        {
            H = m_rgb2hsi.HSI[y * m_nPartWidth * 3 + x * 3];
            S = m_rgb2hsi.HSI[y * m_nPartWidth * 3 + x * 3 + 1];
            I = m_rgb2hsi.HSI[y * m_nPartWidth * 3 + x * 3 + 2];
            pColor->Hdistrib[H]++;
            if (pColor->Hdistrib[H] > pColor->Hcmax)
            {
                pColor->Hcmax = pColor->Hdistrib[H];
            }
            pColor->Sdistrib[S]++;
            if (pColor->Sdistrib[S] > pColor->Scmax)
            {
                pColor->Scmax = pColor->Sdistrib[S];
            }
            pColor->Idistrib[I]++;
            if (pColor->Idistrib[I] > pColor->Icmax)
            {
                pColor->Icmax = pColor->Idistrib[I];
            }
        }
    }
    pColor->Invalidate();
}

BOOL CStillView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: Add your message handler code here and/or call default
    // AfxMessageBox("!!");
    return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}

void CStillView::UpdateSelRect(CPoint inpoint)
{
    if (m_bImgUpdate == false)
    {
        return;
    }
    if (mode == 0)
    {
        if (m_range < 1)
        {
            m_range = 1;
        }
        if (m_range > 100)
        {
            m_range = 100;
        }
        m_SelRect.left = inpoint.x - m_range;
        m_SelRect.right = inpoint.x + m_range;
        m_SelRect.top = inpoint.y - m_range;
        m_SelRect.bottom = inpoint.y + m_range;
        // 纠正矩形信息
        int temp;
        if (m_SelRect.left > m_SelRect.right)
        {
            temp = m_SelRect.left;
            m_SelRect.left = m_SelRect.right;
            m_SelRect.right = temp;
        }
        if (m_SelRect.top > m_SelRect.bottom)
        {
            temp = m_SelRect.top;
            m_SelRect.top = m_SelRect.bottom;
            m_SelRect.bottom = temp;
        }
        if (m_SelRect.left < 0)
        {
            m_SelRect.right += -m_SelRect.left;
            m_SelRect.left = 0;
        }
        if (m_SelRect.right > rc.right)
        {
            m_SelRect.left = rc.right - 2 * m_range;
            m_SelRect.right = rc.right;
        }
        if (m_SelRect.top < 0)
        {
            m_SelRect.bottom += -m_SelRect.top;
            m_SelRect.top = 0;
        }
        if (m_SelRect.bottom > rc.bottom)
        {
            m_SelRect.top = rc.bottom - 2 * m_range;
            m_SelRect.bottom = rc.bottom;
        }

        // 计算截选的源图像尺寸
        m_srcx1 = m_SelRect.left * m_nWidth / m_nStillWidth;
        m_srcy1 = (m_nStillHeight - m_SelRect.bottom) * m_nHeight / m_nStillHeight;

        m_srcx2 = m_SelRect.right * m_nWidth / m_nStillWidth;
        m_srcy2 = (m_nStillHeight - m_SelRect.top) * m_nHeight / m_nStillHeight;

        // 截取此部分区域
        int width = m_srcx2 - m_srcx1;
        m_srcx2 += 4 - (width % 4);
        width = m_srcx2 - m_srcx1;
        int height = m_srcy2 - m_srcy1;
        if (width * height > m_nPartbufsize)
        {
            if (m_pPartbuf != NULL)
            {
                delete m_pPartbuf;
                m_pPartbuf = NULL;
            }
            m_nPartbufsize = width * height * 3;
            m_pPartbuf = new BYTE[m_nPartbufsize];
        }
        if (m_pPartbuf != NULL)
        {
            m_nPartWidth = width;
            m_nPartHeight = height;
        }
        else
        {
            m_nPartWidth = m_nPartHeight = 0;
            m_nPartbufsize = 0;
            return;
        }

        // 复制截取图像信息
        BYTE *p = m_pPartbuf;
        for (int i = m_srcy1; i < m_srcy2; i++)
        {
            for (int j = m_srcx1; j < m_srcx2; j++)
            {
                *p = m_pBuf[i * m_nWidth * 3 + j * 3];
                p++;
                *p = m_pBuf[i * m_nWidth * 3 + j * 3 + 1];
                p++;
                *p = m_pBuf[i * m_nWidth * 3 + j * 3 + 2];
                p++;
            }
        }
        m_ShowPart();
        Invalidate();
    }
}

void CStillView::OnTimer(UINT nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    switch (nIDEvent)
    {
    case VT_DRAW:
        if (m_bImgUpdate == true)
        {
            Invalidate();
        }
        m_bImgUpdate = false;
        break;
    }

    CStatic::OnTimer(nIDEvent);
}
