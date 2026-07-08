// HSI_Vision.h: interface for the HSI_Vision class.
//
//////////////////////////////////////////////////////////////////////
/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.0       Date: 2007/10/19
  Email:  zwj_uptech@126.com
  Description:    // 基于HSI的图像像素统计类
  Others:         //
*************************************************/

#if !defined(AFX_HSI_VISION_H__DBF49C6A_D09A_45C1_8520_D20ADD0CF40D__INCLUDED_)
#define AFX_HSI_VISION_H__DBF49C6A_D09A_45C1_8520_D20ADD0CF40D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include "HSIParam.h"
#include "RGB2HSI.h"
#include "UPGraphics.h"

class HSI_Vision
{
  public:
    CRGB2HSI _rgb2hsi;
    BOOL FindSplash(BOOL bBina);
    CListBox *list;
    void oup(CString instr);
    void SetParam(HSIParam *inParam);
    void ImageBuf(int Width, int Height, BYTE *Buf);
    HSI_Vision();
    virtual ~HSI_Vision();

  protected:
    CUPGraphics _toshow;
    BOOL GetRGB(int x, int y, RGBTRIPLE *prgb);
    void DrawFocus(long x, long y, RGBTRIPLE *prgb);
    BOOL SetColor(int x, int y, RGBTRIPLE rgb);
    int m_nCY;
    int m_nCX;
    int m_nWidth;
    int m_nHeight;
    HSIParam *m_pParam;
    BYTE *m_pImageBuf;
};

#endif // !defined(AFX_HSI_VISION_H__DBF49C6A_D09A_45C1_8520_D20ADD0CF40D__INCLUDED_)
