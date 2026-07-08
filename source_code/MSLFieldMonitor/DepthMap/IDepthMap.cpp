#include "stdafx.h"
#include "IDepthMap.h"

IDepthMap::IDepthMap()
{
    m_nWidth = 0;
    m_nHeight = 0;
    m_pRGBData = NULL;
    m_pMapData = NULL;
}

IDepthMap::~IDepthMap()
{
    m_Release();
}

void IDepthMap::m_Release()
{
    if (m_pMapData != NULL)
    {
        delete[] m_pMapData;
        m_pMapData = NULL;
    }

    if (m_pRGBData != NULL)
    {
        delete[] m_pRGBData;
        m_pRGBData = NULL;
    }
}

void IDepthMap::InitData(int inWidth, int inHeight)
{
    if (inWidth != m_nWidth || inHeight != m_nHeight)
    {
        m_Release();

        m_nWidth = inWidth;
        m_nHeight = inHeight;
        m_pMapData = new int[m_nWidth * m_nHeight];
        m_pRGBData = new unsigned char[m_nWidth * m_nHeight * 3];

        // bmpinfo 显示用
        m_bmi.bmiHeader.biBitCount = 24;
        m_bmi.bmiHeader.biClrImportant = 0;
        m_bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
        m_bmi.bmiHeader.biSizeImage = 0;
        m_bmi.bmiHeader.biWidth = m_nWidth;
        m_bmi.bmiHeader.biHeight = m_nHeight;
        m_bmi.bmiHeader.biXPelsPerMeter = 0;
        m_bmi.bmiHeader.biYPelsPerMeter = 0;
        m_bmi.bmiHeader.biClrUsed = 0;
        m_bmi.bmiHeader.biPlanes = 1;
        m_bmi.bmiHeader.biCompression = BI_RGB;
    }

    memset(m_pMapData, 0, m_nWidth * m_nHeight * sizeof(int));
    memset(m_pRGBData, 0, m_nWidth * m_nHeight * 3);
}

void IDepthMap::DisplayToWnd(CWnd *inpWnd, int inX, int inY, int inDestWidth, int inDestHeight)
{
    // map -> rgb
    int len = m_nWidth * m_nHeight;
    // 获取深度最大值和最小值
    int nMax = 0;
    int nMin = 255;
    for (int i = 0; i < len; i++)
    {
        if (m_pMapData[i] > nMax)
        {
            nMax = m_pMapData[i];
        }
        if (m_pMapData[i] < nMin)
        {
            nMin = m_pMapData[i];
        }
    }
    // 检测结果，如果结果异常，则退出
    if (nMax <= nMin)
    {
        return;
    }

    double k = (double)0xff / (nMax - nMin);
    for (i = 0; i < len; i++)
    {
        m_pMapData[i] = (m_pMapData[i] - nMin) * k;
    }
    for (i = 0; i < len; i++)
    {
        m_pRGBData[i * 3] = m_pMapData[i];
        m_pRGBData[i * 3 + 1] = m_pMapData[i];
        m_pRGBData[i * 3 + 2] = m_pMapData[i];
    }

    HDC hdc;
    CDC *pDC = inpWnd->GetDC();
    hdc = pDC->GetSafeHdc();
    // 贴图显示
    ::StretchDIBits(hdc, inX, inY,
                    m_nWidth,  // rc.right - rc.left,
                    m_nHeight, // rc.top,
                    0, 0, m_nWidth, m_nHeight, m_pRGBData, &m_bmi, DIB_RGB_COLORS, SRCCOPY);
    inpWnd->ReleaseDC(pDC);
}

int IDepthMap::m_CalDist(int inx1, int iny1, int inx2, int iny2)
{
    double sq = (inx1 - inx2) * (inx1 - inx2) + (iny1 - iny2) * (iny1 - iny2);
    int ret = (int)sqrt(sq);
    return ret;
}

// 将矩阵数值重置到指定值
void IDepthMap::ResetMat(int inValue)
{
    int len = m_nWidth * m_nHeight;
    for (int i = 0; i < len; i++)
    {
        m_pMapData[i] = inValue;
    }
}

void IDepthMap::AddPeak(int inX, int inY, int inPeakVal, int inStep)
{
    int nIndex = 0;
    for (int x = 0; x < m_nWidth; x++)
    {
        for (int y = 0; y < m_nHeight; y++)
        {
            nIndex = y * m_nWidth + x;
            m_pMapData[nIndex] += (inPeakVal - m_CalDist(inX, inY, x, y) * inStep);
        }
    }
}

// 文字输出
void IDepthMap::DrawText(CWnd *inpWnd, int inX, int inY, CString inText, int inCharNum)
{
    HDC hdc;
    CDC *pDC = inpWnd->GetDC();
    hdc = pDC->GetSafeHdc();
    // 文字输出
    TextOut(hdc, inX, inY, inText, inCharNum);
    inpWnd->ReleaseDC(pDC);
}

void IDepthMap::AddSunken(int inEdgeValue, int inSlopeLength)
{
    if (inSlopeLength <= 0)
    {
        return;
    }
    // 计算每一像素下降的值
    int nStep = inEdgeValue / inSlopeLength;
    int nVal = 0;
    // 左侧
    for (int y = 0; y < 120; y++)
    {
        for (int x = 0; x < inSlopeLength; x++)
        {
            // 计算斜度值
            nVal = inEdgeValue - x * nStep;
            if (nVal > m_pMapData[y * 180 + x])
            {
                m_pMapData[y * 180 + x] = nVal;
            }
        }
    }

    // 右侧
    for (y = 0; y < 120; y++)
    {
        for (int x = 179; x > 179 - inSlopeLength; x--)
        {
            nVal = inEdgeValue - (179 - x) * nStep;
            if (nVal > m_pMapData[y * 180 + x])
            {
                m_pMapData[y * 180 + x] = nVal;
            }
        }
    }

    // 下侧
    for (y = 0; y < inSlopeLength; y++)
    {
        for (int x = 0; x < 180; x++)
        {
            nVal = inEdgeValue - y * nStep;
            if (nVal > m_pMapData[y * 180 + x])
            {
                m_pMapData[y * 180 + x] = nVal;
            }
        }
    }

    // 上侧
    for (y = 119; y > 119 - inSlopeLength; y--)
    {
        for (int x = 0; x < 180; x++)
        {
            nVal = inEdgeValue - (119 - y) * nStep;
            if (nVal > m_pMapData[y * 180 + x])
            {
                m_pMapData[y * 180 + x] = nVal;
            }
        }
    }
}
