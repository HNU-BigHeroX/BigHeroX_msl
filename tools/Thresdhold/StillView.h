/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.1       Date: 2008/10/17
  Email:  zwj_uptech@126.com
  Description:    // 静态显示控件
  Others:         // 1.1为缩小版阈值程序做了优化
*************************************************/
#if !defined(AFX_STILLVIEW_H__CA0706A0_2C77_4A84_9CA8_C3F7F592F4FD__INCLUDED_)
#define AFX_STILLVIEW_H__CA0706A0_2C77_4A84_9CA8_C3F7F592F4FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StillView.h : header file
//

#include "VoyInclude/RGB2HSI.h"
#include "Color.h"

#define VT_DRAW 0
/////////////////////////////////////////////////////////////////////////////
// CStillView window

class CStillView : public CStatic
{
    // Construction
  public:
    CStillView();

    // Attributes
  public:
    // 显示句柄
    int mode;
    CStillView *pPart;
    CStillView *pPartH;
    CStillView *pPartS;
    CStillView *pPartI;
    CColor *pColor;
    RECT rc;

    // 缩放
    int m_range;
    CPoint m_lastpoint;
    int m_nCount;
    int m_fpsDis;

    // 显示与否
    bool m_bEnable;
    bool m_bImgUpdate;

    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CStillView)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    void UpdateSelRect(CPoint inpoint);
    void BufImg(BYTE *inBuf, int inWidth, int inHeight);
    virtual ~CStillView();

    // Generated message map functions
  protected:
    void m_Stat();
    void m_ShowPart();
    BYTE *m_pBuf;
    int m_nWidth;
    int m_nHeight;
    CRGB2HSI m_rgb2hsi;

    // 全图显示
    BITMAPINFOHEADER m_bih;
    CBitmap Bitmap;
    int m_nStillWidth;
    int m_nStillHeight;

    // 鼠标点选
    CRect m_SelRect;
    bool m_bPress;

    // 选区对应的源像素信息
    int m_srcx1;
    int m_srcy1;
    int m_srcx2;
    int m_srcy2;
    int m_nPartWidth;
    int m_nPartHeight;
    BYTE *m_pPartbuf;
    int m_nPartbufsize;

    //{{AFX_MSG(CStillView)
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnTimer(UINT nIDEvent);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STILLVIEW_H__CA0706A0_2C77_4A84_9CA8_C3F7F592F4FD__INCLUDED_)
