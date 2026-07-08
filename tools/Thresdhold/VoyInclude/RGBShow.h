#if !defined(AFX_RGBSHOW_H__3023AC35_048A_42E4_B55B_AD89493C1BB0__INCLUDED_)
#define AFX_RGBSHOW_H__3023AC35_048A_42E4_B55B_AD89493C1BB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RGBShow.h : header file
//
#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CRGBShow window

class CRGBShow : public CStatic
{
    // Construction
  public:
    CRGBShow();

    // Attributes
  public:
    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CRGBShow)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    BYTE *GetIMGBuf();
    CRect GetRect(int index);
    int GetCount();
    void CreateImg(BYTE *inBuf, int inWidth, int inHeight);
    CEdit *_pShow;
    virtual ~CRGBShow();

    // Generated message map functions
  protected:
    void RemoveRect();
    void _DelBuf();
    bool updateing; // 图像缓冲更新标记，防止操作过程中有进行了访问
    CRect _CurRect; // 当前点选框的尺寸
    CList<CRect, CRect> _list;
    CRect _FrameRect; // 整个控件的尺寸
    BYTE *_pImgbuf;   // 图像缓冲区
    int _nImgWidth;   // 图像宽度
    int _nImgHeight;  // 图像高度
    //{{AFX_MSG(CRGBShow)
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RGBSHOW_H__3023AC35_048A_42E4_B55B_AD89493C1BB0__INCLUDED_)
