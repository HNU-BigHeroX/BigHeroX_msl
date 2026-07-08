// UPGraphics.cpp: implementation of the CUPGraphics class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UPGraphics.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUPGraphics::CUPGraphics()
{
    nWidth = nHeight = 0;
    map = NULL;
    _IDVal = 0;
    srcmap = NULL;
}

CUPGraphics::~CUPGraphics()
{
    delete[] map;
}

void CUPGraphics::InitMap(UCHAR *inMap, int inWidth, int inHeight, UCHAR IDVal)
{
    // 检查参数合法性
    if (inMap == NULL || inWidth < 0 || inHeight < 0)
    {
        return;
    }

    srcmap = inMap;

    if (inWidth != nWidth || inHeight != nHeight)
    {
        delete map;
        map = new UCHAR[inWidth * inHeight];
        nWidth = inWidth;
        nHeight = inHeight;
    }

    /*for (int i=0;i<nWidth * nHeight;i++)
    {
        map[i] = srcmap[i];
    }*/
    memcpy(map, srcmap, nWidth * nHeight);

    _IDVal = IDVal;
}

UCHAR CUPGraphics::GetPoint(int x, int y, UCHAR *inbuf)
{
    if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
    {
        return 0;
    }

    if (inbuf == NULL)
    {
        return 0;
    }

    return inbuf[y * nWidth + x];
}

void CUPGraphics::SetPoint(UCHAR *Targ, int x, int y, UCHAR Val)
{
    if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
    {
        return;
    }
    Targ[y * nWidth + x] = Val;
}

BOOL CUPGraphics::Refrash()
{
    if (nWidth <= 0 || nHeight <= 0 || map == NULL || srcmap == NULL)
    {
        return FALSE;
    }

    for (int i = 0; i < nWidth * nHeight; i++)
    {
        if (srcmap[i] == _IDVal)
        {
            map[i] = _IDVal;
        }
        else
        {
            map[i] = _IDVal - 1;
        }
    }
    return TRUE;
}

BOOL CUPGraphics::ShowMap(BYTE *rgbBuf, int inWidth, int inHeight)
{
    if (rgbBuf == NULL || inWidth != nWidth || inHeight != nHeight)
    {
        return FALSE;
    }

    UCHAR *lCurmapp = map;
    UCHAR *lCursrcp = rgbBuf;
    for (int i = 0; i < nWidth * nHeight; i++)
    {
        if (*lCurmapp == _IDVal)
        {
            *lCursrcp = color.rgbtBlue;
            *(lCursrcp + 1) = color.rgbtGreen;
            *(lCursrcp + 2) = color.rgbtRed;
        }
        lCurmapp++;
        lCursrcp += 3;
    }
    return TRUE;
}

void CUPGraphics::SetColor(BYTE *rgbBuf, int x, int y, RGBTRIPLE *prgb)
{
    if (x > nWidth - 1 || y > nHeight - 1 || x < 0 || y < 0)
        return;

    rgbBuf[y * nWidth * 3 + x * 3] = prgb->rgbtBlue;
    rgbBuf[y * nWidth * 3 + x * 3 + 1] = prgb->rgbtGreen;
    rgbBuf[y * nWidth * 3 + x * 3 + 2] = prgb->rgbtRed;
}

void CUPGraphics::DrawFocus(BYTE *rgbBuf, long x, long y, RGBTRIPLE *prgb)
{
    // 绘制标记
    for (int i = -19; i < 20; i++)
    {
        for (int j = -19; j < 20; j++)
        {
            if (abs(i) < 1 || abs(j) < 1)
            {
                SetColor(rgbBuf, x + i, y + j, prgb);
            }
        }
    }
}

void CUPGraphics::DrawLine(BYTE *rgbBuf, int beginx, int beginy, int endx, int endy, RGBTRIPLE *inpen)
{
    int dx = abs(beginx - endx);
    int dy = abs(beginy - endy);
    if (dx > dy)
    {
        float slope = (float)(endy - beginy) / (endx - beginx);
        if (endx < beginx)
        {
            int t = beginx;
            beginx = endx;
            endx = t;
            t = beginy;
            beginy = endy;
            endy = t;
        }
        int j;
        for (int i = beginx; i <= endx; i++)
        {
            j = beginy + (i - beginx) * slope;
            SetColor(rgbBuf, i, j, inpen);
        }
    }
    else
    {
        float slope = (float)(endx - beginx) / (endy - beginy);
        if (endy < beginy)
        {
            int t = beginx;
            beginx = endx;
            endx = t;
            t = beginy;
            beginy = endy;
            endy = t;
        }
        int i;
        for (int j = beginy; j <= endy; j++)
        {
            i = beginx + (j - beginy) * slope;
            SetColor(rgbBuf, i, j, inpen);
        }
    }
}
