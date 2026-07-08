// Vision.h: interface for the CVision class.
//
//////////////////////////////////////////////////////////////////////
/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.0       Date: 2007/10/19
  Email:  zwj_uptech@126.com
  Description:    // 图像像素统计类
  Others:         //
*************************************************/

#if !defined(AFX_VISION_H__18037566_8167_4CD6_9BE4_D44117FA7CAE__INCLUDED_)
#define AFX_VISION_H__18037566_8167_4CD6_9BE4_D44117FA7CAE__INCLUDED_

#include <WINGDI.H>
#include "YUVParam.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVision
{
  public:
    void SetParam(YUVParam *inParam);
    void ImageBuf(int Width, int Height, BYTE *Buf);
    BOOL GetColor(int x, int y, RGBTRIPLE *prgb);
    BOOL FindSplash(BOOL bBina);
    CVision();
    virtual ~CVision();

  private:
    void DrawFocus(long x, long y, RGBTRIPLE *prgb);
    int m_nCY;
    int m_nCX;
    int m_nPosition;
    BOOL LineTo(int x, int y);
    BOOL MoveTo(int x, int y);
    int m_nWidth;
    int m_nHeight;
    BYTE *m_pImageBuf;
    YUVParam *m_pParam;
    BOOL SetColor(int x, int y, RGBTRIPLE rgb);
};

#endif // !defined(AFX_VISION_H__18037566_8167_4CD6_9BE4_D44117FA7CAE__INCLUDED_)
