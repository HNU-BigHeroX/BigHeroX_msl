/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.1       Date: 2008/10/21
  Email:  zwj_uptech@126.com
  Description:    // 颜色空间转换
  Others:         // 1.1 修正阈值边界问题
*************************************************/
// RGB2HSI.h: interface for the CRGB2HSI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RGB2HSI_H__67F1D90E_687F_4733_8170_4B9D048ECD98__INCLUDED_)
#define AFX_RGB2HSI_H__67F1D90E_687F_4733_8170_4B9D048ECD98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include "HSIParam.h"

class CRGB2HSI
{
  public:
    void m_FixOverlookOnLogiDeluxe(UCHAR *inbuf, int inwidth, int inheight);
    void Convert(BYTE *rgbBuf, int inWidth, int inHeight);
    BOOL ShowMap(BYTE *rgbBuf, int inWidth, int inHeight, UCHAR inVal, RGBTRIPLE inColor);
    void PreviewMark(BYTE *rgbBuf, int inWidth, int inHeight);
    RGBTRIPLE color;
    BOOL ShowMap(BYTE *rgbBuf, int inWidth, int inHeight);
    UCHAR lastVal;
    BOOL Mapping(HSIParam *inparam, UCHAR inVal);
    void SetMark(int x, int y, UCHAR inflag);
    UCHAR GetMark(int x, int y);
    UCHAR *Map;
    int *HSI;
    UCHAR *Mark;
    void InitBuf(BYTE *rgbBuf, int inWidth, int inHeight);
    BOOL Convert();
    CRGB2HSI();
    virtual ~CRGB2HSI();

  protected:
    void SetColor(BYTE *rgbBuf, int x, int y, RGBTRIPLE *prgb);
    UCHAR GetPoint(int x, int y, UCHAR *inbuf);
    void SetMap(int x, int y, UCHAR inVal);
    void _DelMap();
    int _Min(int x, int y, int z);
    int _Max(int x, int y, int z);
    void _ConvPixel(int x, int y);
    void _DelHSI();
    void _DelMark();
    BYTE *_RGBBuf; // RGB原始图像指针
    int _nWidth;
    int _nHeight;

    BYTE R;
    BYTE G;
    BYTE B;
    int H;
    int S;
    int I;
    int max;
    int min;

    int index;

    // logi
    int *logifix;
};

#endif // !defined(AFX_RGB2HSI_H__67F1D90E_687F_4733_8170_4B9D048ECD98__INCLUDED_)
