// Vision.cpp: implementation of the CVision class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vision.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVision::CVision()
{
    m_pParam = NULL;
    m_pImageBuf = NULL;
}

CVision::~CVision()
{
}

// 主要的入口函数，色块统计
BOOL CVision::FindSplash(BOOL bBina)
{
    // 若上一次处理未完，则返回
    if (m_pParam->bBusy == TRUE)
    {
        return FALSE;
    }
    m_pParam->bBusy = TRUE;

    m_pParam->Clear();

    if (m_pImageBuf == NULL)
    {
        m_pParam->bBusy = FALSE;
        return FALSE;
    }

    RGBTRIPLE *prgb = new RGBTRIPLE;
    float tU, tV, tY; // U和V的中间变量

    if (bBina)
    { // 需要进行二值化显示的情形

        RGBTRIPLE black;
        black.rgbtBlue = black.rgbtGreen = black.rgbtRed = 0x0;
        RGBTRIPLE white;
        white.rgbtBlue = white.rgbtGreen = white.rgbtRed = 0xFF;
        BOOL flag;

        for (int x = 0; x < m_nWidth; x++)
        {
            for (int y = 0; y < m_nHeight; y++)
            {
                GetColor(x, y, prgb);
                flag = FALSE; // 判断结果标记

                // 根据使能变量决定是否进行颜色空间转换
                if (m_pParam->Uen == TRUE)
                {
                    tU = (float)(-2 * prgb->rgbtRed - 3 * prgb->rgbtGreen + 5 * prgb->rgbtBlue);
                    if (tU > m_pParam->Umin && tU < m_pParam->Umax)
                    {
                        m_pParam->Ux += x;
                        m_pParam->Uy += y;
                        m_pParam->Usum++;
                        flag = TRUE;
                    }
                }

                if (m_pParam->Ven == TRUE)
                {
                    tV = 5 * prgb->rgbtRed - 4.5 * prgb->rgbtGreen - 0.5 * prgb->rgbtBlue;
                    if (m_pParam->Vmin < tV && tV < m_pParam->Vmax)
                    {
                        m_pParam->Vx += x;
                        m_pParam->Vy += y;
                        m_pParam->Vsum++;
                        flag = TRUE;
                    }
                }

                if (m_pParam->Yen == TRUE)
                {
                    tY = 0;
                    if (m_pParam->Ymin < tY && tY < m_pParam->Ymax)
                    {
                        m_pParam->Yx += x;
                        m_pParam->Yy += y;
                        m_pParam->Ysum++;
                        flag = TRUE;
                    }
                }

                // 根据标记描绘点
                if (flag == TRUE)
                {
                    SetColor(x, y, black);
                }
                else
                {
                    SetColor(x, y, white);
                }
            }
        }
    }
    else
    { // 只进行统计
        for (int x = 0; x < m_nWidth - 1; x++)
        {
            for (int y = 0; y < m_nHeight - 1; y++)
            {
                GetColor(x, y, prgb);
                // 根据使能标记进行颜色空间变换
                if (m_pParam->Uen == TRUE)
                {
                    tU = -0.2 * prgb->rgbtRed - 0.3 * prgb->rgbtGreen + 0.5 * prgb->rgbtBlue;
                    if (m_pParam->Umin < tU && tU < m_pParam->Umax)
                    {
                        m_pParam->Ux += x;
                        m_pParam->Uy += y;
                        m_pParam->Usum++;
                    }
                }

                if (m_pParam->Ven == TRUE)
                {
                    tV = 0.5 * prgb->rgbtRed - 0.45 * prgb->rgbtGreen - 0.05 * prgb->rgbtBlue;
                    ;
                    if (m_pParam->Vmin < tV && tV < m_pParam->Vmax)
                    {
                        m_pParam->Vx += x;
                        m_pParam->Vy += y;
                        m_pParam->Vsum++;
                    }
                }

                if (m_pParam->Yen == TRUE)
                {
                    tY = 0;
                    if (m_pParam->Ymin < tY && tY < m_pParam->Ymax)
                    {
                        m_pParam->Yx += x;
                        m_pParam->Yy += y;
                        m_pParam->Ysum++;
                    }
                }
            }
        }
    }

    // 开始绘制颜色重心点
    // 如果没有找到任何点则返回
    if (m_pParam->Ysum < 1 && m_pParam->Usum < 1 && m_pParam->Vsum < 1)
    {
        delete prgb;
        m_pParam->bBusy = FALSE;
        return FALSE;
    }

    // 计算被标记的点数量(不绘制总标记重心则可以去掉)
    m_nCX = (m_pParam->Ux + m_pParam->Vx + m_pParam->Yx) / (m_pParam->Usum + m_pParam->Vsum + m_pParam->Ysum);
    m_nCY = (m_pParam->Uy + m_pParam->Vy + m_pParam->Yy) / (m_pParam->Usum + m_pParam->Vsum + m_pParam->Ysum);

    prgb->rgbtBlue = 0;
    prgb->rgbtGreen = 0;
    prgb->rgbtRed = 0xff;
    DrawFocus(m_nCX, m_nCY, prgb);

    // 计算各重心坐标
    if (m_pParam->Uen == TRUE)
    {
        m_pParam->Ux = m_pParam->Ux / m_pParam->Usum;
        m_pParam->Uy = m_pParam->Uy / m_pParam->Usum;
    }

    if (m_pParam->Ven == TRUE)
    {
        m_pParam->Vx = m_pParam->Vx / m_pParam->Vsum;
        m_pParam->Vy = m_pParam->Vy / m_pParam->Vsum;
    }

    if (m_pParam->Yen == TRUE)
    {
        m_pParam->Yx = m_pParam->Yx / m_pParam->Ysum;
        m_pParam->Yy = m_pParam->Yy / m_pParam->Ysum;
    }

    // 工作完毕，可以进行下一次运算
    m_pParam->bBusy = FALSE;
    delete prgb;
    return TRUE;
}

// 为某一像素赋值
BOOL CVision::SetColor(int x, int y, RGBTRIPLE rgb)
{
    if (x > m_nWidth - 1 || y > m_nHeight - 1 || x < 0 || y < 0)
        return FALSE;

    m_pImageBuf[y * m_nWidth * 3 + x * 3] = rgb.rgbtBlue;
    m_pImageBuf[y * m_nWidth * 3 + x * 3 + 1] = rgb.rgbtGreen;
    m_pImageBuf[y * m_nWidth * 3 + x * 3 + 2] = rgb.rgbtRed;

    return TRUE;
}

// 查询某一像素的颜色
BOOL CVision::GetColor(int x, int y, RGBTRIPLE *prgb)
{
    if (x > m_nWidth - 1 || y > m_nHeight - 1 || x < 0 || y < 0)
        return FALSE;

    prgb->rgbtBlue = m_pImageBuf[y * m_nWidth * 3 + x * 3];
    prgb->rgbtGreen = m_pImageBuf[y * m_nWidth * 3 + x * 3 + 1];
    prgb->rgbtRed = m_pImageBuf[y * m_nWidth * 3 + x * 3 + 2];

    return TRUE;
}

// 引入图像缓冲区
void CVision::ImageBuf(int Width, int Height, BYTE *Buf)
{
    m_nHeight = Height;
    m_nWidth = Width;
    m_pImageBuf = Buf;
}

// 移动到某一像素点
BOOL CVision::MoveTo(int x, int y)
{
    if (x > m_nWidth - 1 || y > m_nHeight - 1 || x < 0 || y < 0)
        return FALSE;

    m_nCX = x;
    m_nCY = y;
    m_nPosition = y * m_nWidth * 3 + x * 3;

    return TRUE;
}

// 画直线（未实现）
BOOL CVision::LineTo(int x, int y)
{
    if (x > m_nWidth - 1 || y > m_nHeight - 1 || x < 0 || y < 0)
        return FALSE;

    return TRUE;
}

void CVision::SetParam(YUVParam *inParam)
{
    m_pParam = inParam;
}

void CVision::DrawFocus(long x, long y, RGBTRIPLE *prgb)
{
    // 绘制标记
    for (int i = -8; i < 9; i++)
    {
        for (int j = -8; j < 9; j++)
        {
            if (abs(i) < 2 || abs(j) < 2)
            {
                SetColor(x + i, y + j, *prgb);
            }
        }
    }
}
