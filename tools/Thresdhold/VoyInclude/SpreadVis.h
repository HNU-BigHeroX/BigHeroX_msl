// SpreadVis.h: interface for the CSpreadVis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPREADVIS_H__580191A5_52A2_4BF6_ADBE_E32C2F6DEB5B__INCLUDED_)
#define AFX_SPREADVIS_H__580191A5_52A2_4BF6_ADBE_E32C2F6DEB5B__INCLUDED_

#include <WINGDI.H>
#include "YUVParam.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpreadVis
{
  public:
    void SetCenter(int x, int y);
    BOOL Process(UINT inMode);
    BOOL GetColor(int x, int y, RGBTRIPLE *prgb);
    BOOL SetColor(int x, int y, RGBTRIPLE rgb);
    void SetParam(YUVParam *inParam);
    void ImageBuf(int Width, int Height, BYTE *Buf);
    void DrawFocus(long x, long y, int inRange, RGBTRIPLE *prgb);
    CSpreadVis();
    virtual ~CSpreadVis();

  protected:
    bool m_Treat(int x, int y, RGBTRIPLE *prgb, UINT inMode);
    int m_nCntrY;    // 机体中心点坐标
    int m_nCntrX;    // 机体中心点坐标
    int m_nMaxRange; // 生长区域边缘
    int m_nWidth;
    int m_nHeight;
    RGBTRIPLE m_rgb;
    RGBTRIPLE m_FocusClr;
    RGBTRIPLE m_TargClr;
    RGBTRIPLE m_BgClr;
    BYTE *m_pImageBuf;
    YUVParam *m_pParam;

    // 临时变量
    bool found;
    bool flag; // 像素查找标记
    int tY;
    int tU;
    int tV;
    int Range;
    int Step;
};

#endif // !defined(AFX_SPREADVIS_H__580191A5_52A2_4BF6_ADBE_E32C2F6DEB5B__INCLUDED_)
