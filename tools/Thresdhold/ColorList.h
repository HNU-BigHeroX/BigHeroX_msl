#if !defined(AFX_COLORLIST_H__D4567810_EF6C_401F_B079_F907ED12767B__INCLUDED_)
#define AFX_COLORLIST_H__D4567810_EF6C_401F_B079_F907ED12767B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorList.h : header file
//

#include "ColorTest.h"
#include "Color.h"

/////////////////////////////////////////////////////////////////////////////
// CColorList window

class CColorList : public CListBox
{
    // Construction
  public:
    CColorList();

    // Attributes
  public:
    CColorTest *pTest;
    CColor *pclr;

  protected:
    int lastSel;

    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CColorList)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    void UpdateDisplay();
    void ShowParam(HSISet inHsi);
    void ReSelect();
    virtual ~CColorList();

    // Generated message map functions
  protected:
    //{{AFX_MSG(CColorList)
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORLIST_H__D4567810_EF6C_401F_B079_F907ED12767B__INCLUDED_)
