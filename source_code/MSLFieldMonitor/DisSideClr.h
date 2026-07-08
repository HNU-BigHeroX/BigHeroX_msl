// 左上角裁判盒模块的变化以及展示
#if !defined(AFX_DISSIDECLR_H__5058967A_0F30_4799_8EAF_794612E2A2D4__INCLUDED_)
#define AFX_DISSIDECLR_H__5058967A_0F30_4799_8EAF_794612E2A2D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DisSideClr.h : header file

/////////////////////////////////////////////////////////////////////////////
// CDisSideClr window

class CDisSideClr : public CStatic
{
    // Construction
  public:
    CDisSideClr();

    // Attributes
  public:
    CDC dcMemory;
    CBitmap Bitmap;
    int nDividLine;
    int nCurPosition;
    int nStep;

    CString strTxt;
    COLORREF clrbk;
    COLORREF clrtext;
    CFont font;

    int nFlag;

    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDisSideClr)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    virtual ~CDisSideClr();

    // Generated message map functions
  protected:
    //{{AFX_MSG(CDisSideClr)
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISSIDECLR_H__5058967A_0F30_4799_8EAF_794612E2A2D4__INCLUDED_)
