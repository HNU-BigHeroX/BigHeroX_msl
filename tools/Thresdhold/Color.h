/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.4       Date: 2008/10/17
  Email:  zwj_uptech@126.com
  Description:    // 可视化颜色选择
  Others:         // 1.3 配合StillView1.1做了优化
                  // 1.4 加入标定目标提示，仿真调试信息 2008/11/9
*************************************************/
#if !defined(AFX_COLOR_H__0622103A_E2F4_4F24_B088_5EC33D15EEF6__INCLUDED_)
#define AFX_COLOR_H__0622103A_E2F4_4F24_B088_5EC33D15EEF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Color.h : header file
//

#include "VoyInclude/VoyHSI.h"
#include "ItemList.h"

/////////////////////////////////////////////////////////////////////////////
// CColor window

class CColor : public CStatic
{
    // Construction
  public:
    CColor();

    // Attributes
  public:
    bool bColor;
    HSIParam *pParam;
    CItemList *pList;
    int Hdistrib[360];
    int Sdistrib[255];
    int Idistrib[255];
    int Hcmax;
    int Scmax;
    int Icmax;

    // zoomimg
    int nZoomWidth;
    int nZoomHeight;
    UCHAR *pZoombuf;
    BITMAPINFOHEADER m_bih;

    // display
    CString m_strItem;

    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CColor)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    void CleanStat();
    void HSIToShow(HSIParam inHsi);
    virtual ~CColor();

    // Generated message map functions
  protected:
    void m_RegSelBox();
    void ShowToHSI();

    // 绘图用
    CDC dcMemory;
    CBitmap Bitmap;
    HSIParam hsi;
    int left;
    int right;
    int top;
    int bottom;
    int Dx;
    int Dy;
    CPoint tp;
    bool leftmoving;
    bool rightmoving;
    bool bottommoving;
    bool topmoving;
    int m_status;
    CRect HRect;
    CRect HRect_1;
    CRect SRect;

    // 右侧I视图
    int itop;
    int ibottom;
    bool itopmoving;
    bool ibottommoving;
    int iDy;
    CRect IRect;

    // item font
    CFont m_itfont;

    //{{AFX_MSG(CColor)
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLOR_H__0622103A_E2F4_4F24_B088_5EC33D15EEF6__INCLUDED_)
