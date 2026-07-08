// ConctDomain.h: interface for the CConctDomain class.
//
//////////////////////////////////////////////////////////////////////
/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.1       Date: 2008/9/16
  Email:  zwj_uptech@126.com
  Description:    // 图像色块连通域检测
  Others:         //
*************************************************/

#if !defined(AFX_CONCTDOMAIN_H__F42E661E_D5F3_4CB7_810A_FB9A95FA728A__INCLUDED_)
#define AFX_CONCTDOMAIN_H__F42E661E_D5F3_4CB7_810A_FB9A95FA728A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "UPGraphics.h"

struct DomainNode
{
    int x;
    int y;
    int sum;
    int top;
    int bottom;
    int left;
    int right;
    DomainNode *next;
};

class CConctDomain //: public CUPGraphics
{
  public:
    void Combo(UCHAR *inMap, int inWidth, int inHeight, UCHAR IDVal);
    void DrawFocus(BYTE *rgbBuf, long x, long y, RGBTRIPLE *prgb);
    void Erosion(int inMode);
    void Dilation(int inTimes);
    BOOL ShowMap(BYTE *rgbBuf, int inWidth, int inHeight);
    void InitMap(UCHAR *inMap, int inWidth, int inHeight, UCHAR IDVal);
    bool RadScan(int orgx, int orgy, int drcx, int drcy, CPoint *inpnt);
    DomainNode GetDomainNear(int x, int y);
    DomainNode GetDomainAt(int index);
    void SetRange(int inRge);
    void Mark(BYTE *rgbBuf);
    int GetCount();
    BOOL Process(int inMode);
    CConctDomain();
    virtual ~CConctDomain();
    int debug_cnt;
    DomainNode *result;
    int m_nNumofResult;

  protected:
    bool m_checkpoint(UCHAR *inpData);
    bool m_checkpoint(int inx, int iny);
    // 临时变量
    int _Range;
    DomainNode _CurDomain;
    int m_Nextx;
    int m_Nexty;
    int tempx;
    int tempy;
    int temp;
    int i;
    bool flag;
    UCHAR *m_cursrcp;
    UCHAR *m_curmapp;

    // check临时变量
    UCHAR *ch_cursrcp;
    UCHAR *ch_curmapp;
    int ch_temp;
    int ch_i;

    // 临时变量
    UCHAR *lCurmapp;
    UCHAR *lCursrcp;

    BOOL _Expend(int x, int y);

    // 作为输出结果
    int m_nWidth;
    int m_nHeight;
    UCHAR *map;
    UCHAR *srcmap;
    UCHAR _IDVal;

    // 连通域检测
    DomainNode *toscan;
};

#endif // !defined(AFX_CONCTDOMAIN_H__F42E661E_D5F3_4CB7_810A_FB9A95FA728A__INCLUDED_)
