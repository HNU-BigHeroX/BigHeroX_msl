// HSI_Vision.cpp: implementation of the HSI_Vision class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HSI_Vision.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HSI_Vision::HSI_Vision()
{
    m_pParam = NULL;
    m_pImageBuf = NULL;
}

HSI_Vision::~HSI_Vision()
{
}

void HSI_Vision::ImageBuf(int Width, int Height, BYTE *Buf)
{

    m_nHeight = Height;
    m_nWidth = Width;
    m_pImageBuf = Buf;

    _rgb2hsi.InitBuf(Buf, Width, Height);
}

void HSI_Vision::DrawFocus(long x, long y, RGBTRIPLE *prgb)
{
    // 绘制标记
    for (int i = -8; i < 9; i++)
    {
        for (int j = -8; j < 9; j++)
        {
            if (abs(i) < 1 || abs(j) < 1)
            {
                SetColor(x + i, y + j, *prgb);
            }
        }
    }
}

BOOL HSI_Vision::SetColor(int x, int y, RGBTRIPLE rgb)
{
    if (x > m_nWidth - 1 || y > m_nHeight - 1 || x < 0 || y < 0)
        return FALSE;

    m_pImageBuf[y * m_nWidth * 3 + x * 3] = rgb.rgbtBlue;
    m_pImageBuf[y * m_nWidth * 3 + x * 3 + 1] = rgb.rgbtGreen;
    m_pImageBuf[y * m_nWidth * 3 + x * 3 + 2] = rgb.rgbtRed;

    return TRUE;
}

BOOL HSI_Vision::GetRGB(int x, int y, RGBTRIPLE *prgb)
{
    if (x > m_nWidth - 1 || y > m_nHeight - 1 || x < 0 || y < 0)
        return FALSE;

    prgb->rgbtBlue = m_pImageBuf[y * m_nWidth * 3 + x * 3];
    prgb->rgbtGreen = m_pImageBuf[y * m_nWidth * 3 + x * 3 + 1];
    prgb->rgbtRed = m_pImageBuf[y * m_nWidth * 3 + x * 3 + 2];

    return TRUE;
}

void HSI_Vision::SetParam(HSIParam *inParam)
{
    m_pParam = inParam;
}

void HSI_Vision::oup(CString instr)
{
    list->AddString(instr);
    list->SetCurSel(list->GetCount() - 1);
}

BOOL HSI_Vision::FindSplash(BOOL bBina)
{
    // 判断图像指针和参数指针是否为空
    if (m_pImageBuf == NULL || m_pParam == NULL)
    {
        return FALSE;
    }

    m_pParam->Clear();

    RGBTRIPLE red;
    red.rgbtRed = 0xff;
    red.rgbtGreen = 0;
    red.rgbtBlue = 0;

    // rgb -> hsi
    _rgb2hsi.Convert();
    _rgb2hsi.Mapping(m_pParam, 1);

    if (bBina != 0)
    {
        _toshow.InitMap(_rgb2hsi.Map, m_nWidth, m_nHeight, 1);
        _toshow.color = red;
        _toshow.ShowMap(m_pImageBuf, m_nWidth, m_nHeight);
    }

    red.rgbtBlue = 0xff;
    red.rgbtRed = 0;
    DrawFocus(m_pParam->x, m_pParam->y, &red);

    /*/连通域检测
    m_cm.SetThreshold(m_pParam->Median,m_pParam->Range,m_pParam->Satu_Low);
    m_cm.SetHSIBuf(pHSIBuf,m_nWidth,m_nHeight);
    m_cm.Process();

    red.rgbtRed = 0;
    red.rgbtGreen = 0;
    red.rgbtBlue = 0xff;
    CString str;
    if (m_cm.GetCount() > 2)
    {
        DrawFocus(m_cm.GetNode(0)->x,m_cm.GetNode(0)->y,&red);
        DrawFocus(m_cm.GetNode(1)->x,m_cm.GetNode(1)->y,&red);
        str.Format("%d first(%d,%d)",m_cm.GetCount(),m_cm.GetNode(0)->x,m_cm.GetNode(0)->y);
    }
    else
    {
        str.Format("%d",m_cm.GetCount());
    }
    oup(str);*/
    return TRUE;
}
