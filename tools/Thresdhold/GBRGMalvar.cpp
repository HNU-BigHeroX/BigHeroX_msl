// GBRGMalvar.cpp: implementation of the CGBRGMalvar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Thresdhold.h"
#include "GBRGMalvar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

static int Neigh[5][5];

void SetNeigh(int inx, int iny, BYTE val)
{
    Neigh[inx + 2][iny + 2] = (int)val;
}

int GetNeigh(int inx, int iny)
{
    return Neigh[inx + 2][iny + 2];
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGBRGMalvar::CGBRGMalvar()
{
    m_nWidth = 640;
    m_nHeight = 480;
    m_nWidthBytes = m_nWidth * 3;
}

CGBRGMalvar::~CGBRGMalvar()
{
}

void CGBRGMalvar::MalvarDemosaic(BYTE *inBuf, int inWidth, int inHeight)
{
    BYTE *pCurPixel = inBuf;
    bool bRedRow = true;
    bool bGreen = false;
    for (int h = 2; h < inHeight - 2; h++)
    {
        if (h % 2 == 0)
        {
            bRedRow = true;
            bGreen = false;
        }
        else
        {
            bRedRow = false;
            bGreen = true;
        }
        // pCurPixel = inBuf +(h*inWidth)*3 + 6;
        for (int w = 2; w < inWidth - 2; w++)
        {
            pCurPixel = inBuf + (h * inWidth + w) * 3;
            if (bRedRow == true)
            {
                // red row
                if (bGreen == true)
                {
                    GreenCenter_RedRow(pCurPixel);
                }
                else
                {
                    RedCenter(pCurPixel);
                }
            }
            else
            {
                // blue row
                if (bGreen == true)
                {
                    GreenCenter_BlueRow(pCurPixel);
                }
                else
                {
                    BlueCenter(pCurPixel);
                }
            }
            bGreen = !bGreen;

            // pCurPixel += 3;
        }
    }
}

int tmp[5];
int res;
/**红色像素**/
void CGBRGMalvar::RedCenter(BYTE *pBuf)
{
    // 绿色分量
    tmp[0] = *(pBuf - 3 + 1);                   //(-1,0,g)
    tmp[0] = tmp[0] + *(pBuf + 3 + 1)           //(1,0,g)
             + *(pBuf - m_nWidthBytes + 1)      //(0,-1,g)
             + *(pBuf + m_nWidthBytes + 1);     //(0,1,g)
    tmp[1] = *(pBuf - 6 + 2);                   //(-2,0,r)
    tmp[1] = tmp[1] + *(pBuf + 6 + 2)           //(2,0,r)
             + *(pBuf - m_nWidthBytes * 2 + 2)  //(0,-2,r)
             + *(pBuf + m_nWidthBytes * 2 + 2); //(0,2,r)
    tmp[2] = *(pBuf + 2);                       //(0,0,r)
    res = (tmp[0] * 2 - tmp[1] + tmp[2] * 4) / 8;
    if (res < 0)
    {
        res = 0;
    }
    if (res > 0xff)
    {
        res = 0xff;
    }
    pBuf[1] = res;

    // 蓝色分量
    tmp[0] = *(pBuf - m_nWidthBytes - 3);         //(-1,-1,b)
    tmp[0] = tmp[0] + *(pBuf + m_nWidthBytes - 3) //(-1,1,b)
             + *(pBuf - m_nWidthBytes + 3)        //(1,-1,b)
             + *(pBuf + m_nWidthBytes + 3);       //(1,1,b)
    tmp[1] = *(pBuf - 6 + 2);                     //(-2,0,r)
    tmp[1] = tmp[1] + *(pBuf + 6 + 2)             //(2,0,r)
             + *(pBuf - m_nWidthBytes * 2 + 2)    //(0,-2,r)
             + *(pBuf + m_nWidthBytes * 2 + 2);   //(0,2,r)
    tmp[2] = *(pBuf + 2);                         //(0,0,r)
    res = (tmp[0] * 4 - tmp[1] * 3 + tmp[2] * 12) / 16;
    if (res < 0)
    {
        res = 0;
    }
    if (res > 0xff)
    {
        res = 0xff;
    }
    pBuf[0] = res;
}

/**蓝色像素**/
void CGBRGMalvar::BlueCenter(BYTE *pBuf)
{
    // 红色分量
    tmp[0] = *(pBuf - m_nWidthBytes - 3 + 2);         //(-1,-1,r)
    tmp[0] = tmp[0] + *(pBuf + m_nWidthBytes - 3 + 2) //(-1,1,r)
             + *(pBuf - m_nWidthBytes + 3 + 2)        //(1,-1,r)
             + *(pBuf + m_nWidthBytes + 3 + 2);       //(1,1,r)

    tmp[1] = *(pBuf - 6);                   //(-2,0,b)
    tmp[1] = tmp[1] + *(pBuf + 6)           //(2,0,b)
             + *(pBuf - m_nWidthBytes * 2)  //(0,-2,b)
             + *(pBuf + m_nWidthBytes * 2); //(0,2,r)

    tmp[2] = (*pBuf); //(0,0,b)

    res = (tmp[0] * 4 - tmp[1] * 3 + tmp[2] * 12) / 16;
    if (res < 0)
    {
        res = 0;
    }
    if (res > 0xff)
    {
        res = 0xff;
    }
    pBuf[2] = res;

    // 绿色分量
    tmp[0] = *(pBuf - 3 + 1);               //(-1,0,g)
    tmp[0] = tmp[0] + *(pBuf + 3 + 1)       //(1,0,g)
             + *(pBuf - m_nWidthBytes + 1)  //(0,-1,g)
             + *(pBuf + m_nWidthBytes + 1); //(0,1,g)

    tmp[1] = *(pBuf - 6);                   //(-2,0,b)
    tmp[1] = tmp[1] + *(pBuf + 6)           //(2,0,b)
             + *(pBuf - m_nWidthBytes * 2)  //(0,-2,b)
             + *(pBuf + m_nWidthBytes * 2); //(0,2,b)

    tmp[2] = (*pBuf); //(0,0,b)

    res = ((tmp[0]) * 2 - (tmp[1]) + (tmp[2] * 4)) / 8;
    if (res < 0)
    {
        res = 0;
    }
    if (res > 0xff)
    {
        res = 0xff;
    }
    pBuf[1] = res;
}

/**绿色像素（在红色行）**/
void CGBRGMalvar::GreenCenter_RedRow(BYTE *pBuf)
{
    // 红色分量
    tmp[0] = *(pBuf - 3 + 2);          //(-1,0,r)
    tmp[0] = tmp[0] + *(pBuf + 3 + 2); //(1,0,r)

    tmp[1] = *(pBuf - m_nWidthBytes - 3 + 1);         //(-1,-1,g)
    tmp[1] = tmp[1] + *(pBuf + m_nWidthBytes - 3 + 1) //(-1,1,g)
             + *(pBuf - m_nWidthBytes + 3 + 1)        //(1,-1,g)
             + *(pBuf + m_nWidthBytes + 3 + 1)        //(1,1,g)
             + *(pBuf - 6 + 1)                        //(-2,0,g)
             + *(pBuf + 6 + 1);                       //(2,0,g)

    tmp[2] = *(pBuf - m_nWidthBytes * 2 + 1);          //(0,-2,g)
    tmp[2] = tmp[2] + *(pBuf + m_nWidthBytes * 2 + 1); //(0,2,g)

    tmp[3] = *(pBuf + 1); //(0,0,g)

    res = (tmp[0] * 8 - tmp[1] * 2 + (tmp[2]) + (tmp[3] * 10)) / 16;
    if (res < 0)
    {
        res = 0;
    }
    if (res > 0xff)
    {
        res = 0xff;
    }
    pBuf[2] = res;

    // 蓝色分量
    tmp[0] = *(pBuf - m_nWidthBytes);          //(0,-1,b)
    tmp[0] = tmp[0] + *(pBuf + m_nWidthBytes); //(0,1,b)

    tmp[1] = *(pBuf - m_nWidthBytes - 3 + 1);         //(-1,-1,g)
    tmp[1] = tmp[1] + *(pBuf + m_nWidthBytes - 3 + 1) //(-1,1,g)
             + *(pBuf - m_nWidthBytes + 3 + 1)        //(1,-1,g)
             + *(pBuf + m_nWidthBytes + 3 + 1)        //(1,1,g)
             + *(pBuf - m_nWidthBytes * 2 + 1)        //(0,-2,g)
             + *(pBuf + m_nWidthBytes * 2 + 1);       //(0,2,g)

    tmp[2] = *(pBuf - 6 + 1);          //(-2,0,g)
    tmp[2] = tmp[2] + *(pBuf + 6 + 1); //(2,0,g)

    tmp[3] = *(pBuf + 1); //(0,0,g)

    res = (tmp[0] * 8 - tmp[1] * 2 + tmp[2] + tmp[3] * 10) / 16;
    if (res < 0)
    {
        res = 0;
    }
    if (res > 0xff)
    {
        res = 0xff;
    }
    pBuf[0] = res;
}

/**绿色像素（在蓝色行）**/
void CGBRGMalvar::GreenCenter_BlueRow(BYTE *pBuf)
{
    // 红色分量
    tmp[0] = *(pBuf - m_nWidthBytes + 2);          //(0,-1,r)
    tmp[0] = tmp[0] + *(pBuf + m_nWidthBytes + 2); //(0,1,r)

    tmp[1] = *(pBuf - m_nWidthBytes - 3 + 1);         //(-1,-1,g)
    tmp[1] = tmp[1] + *(pBuf + m_nWidthBytes - 3 + 1) //(-1,1,g)
             + *(pBuf - m_nWidthBytes + 3 + 1)        //(1,-1,g)
             + *(pBuf + m_nWidthBytes + 3 + 1)        //(1,1,g)
             + *(pBuf - m_nWidthBytes * 2 + 1)        //(0,-2,g)
             + *(pBuf + m_nWidthBytes * 2 + 1);       //(0,2,g)

    tmp[2] = *(pBuf - 6 + 1);          //(-2,0,g)
    tmp[2] = tmp[2] + *(pBuf + 6 + 1); //(2,0,g)

    tmp[3] = *(pBuf + 1); //(0,0,g)*10

    res = (tmp[0] * 8 - tmp[1] * 2 + tmp[2] + tmp[3] * 10) / 16;
    if (res < 0)
    {
        res = 0;
    }
    if (res > 0xff)
    {
        res = 0xff;
    }
    pBuf[2] = res;

    // 蓝色分量
    tmp[0] = *(pBuf - 3);          //(-1,0,b)
    tmp[0] = tmp[0] + *(pBuf + 3); //(1,0,b)

    tmp[1] = *(pBuf - m_nWidthBytes - 3 + 1);         //(-1,-1,g)
    tmp[1] = tmp[1] + *(pBuf + m_nWidthBytes - 3 + 1) //(-1,1,g)
             + *(pBuf - m_nWidthBytes + 3 + 1)        //(1,-1,g)
             + *(pBuf + m_nWidthBytes + 3 + 1)        //(1,1,g)
             + *(pBuf - 6 + 1)                        //(-2,0,g)
             + *(pBuf + 6 + 1);                       //(2,0,g)

    tmp[2] = *(pBuf - m_nWidthBytes * 2 + 1);          //(0,-2,g)
    tmp[2] = tmp[2] + *(pBuf + m_nWidthBytes * 2 + 1); //(0,2,g)

    tmp[3] = *(pBuf + 1); //(0,0,g)

    res = (tmp[0] * 8 - tmp[1] * 2 + tmp[2] + tmp[3] * 10) / 16;
    if (res < 0)
    {
        res = 0;
    }
    if (res > 0xff)
    {
        res = 0xff;
    }
    pBuf[0] = res;
}
