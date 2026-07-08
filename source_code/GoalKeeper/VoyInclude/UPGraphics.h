// UPGraphics.h: interface for the CUPGraphics class.
//
//////////////////////////////////////////////////////////////////////
/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.1      Date: 2008/9/16
  Email:  zwj_uptech@126.com
  Description:    // 图像统一接口类
  Others:         //
*************************************************/

#if !defined(AFX_UPGRAPHICS_H__268FC593_7D1B_4972_BAC6_3F34C33DEBB2__INCLUDED_)
#define AFX_UPGRAPHICS_H__268FC593_7D1B_4972_BAC6_3F34C33DEBB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUPGraphics
{
  public:
    void DrawLine(BYTE *rgbBuf, int beginx, int beginy, int endx, int endy, RGBTRIPLE *inpen);
    RGBTRIPLE color;
    void DrawFocus(BYTE *rgbBuf, long x, long y, RGBTRIPLE *prgb);
    BOOL ShowMap(BYTE *rgbBuf, int inWidth, int inHeight);
    BOOL Refrash();
    // 作为输出结果
    int nWidth;
    int nHeight;
    UCHAR *map;

    void InitMap(UCHAR *inMap, int inWidth, int inHeight, UCHAR IDVal);

    CUPGraphics();
    virtual ~CUPGraphics();

  protected:
    void SetColor(BYTE *rgbBuf, int x, int y, RGBTRIPLE *prgb);
    void SetPoint(UCHAR *Targ, int x, int y, UCHAR Val);
    UCHAR GetPoint(int x, int y, UCHAR *inbuf);
    UCHAR *srcmap;
    UCHAR _IDVal;
};

#endif // !defined(AFX_UPGRAPHICS_H__268FC593_7D1B_4972_BAC6_3F34C33DEBB2__INCLUDED_)
