/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.1       Date: 2008/9/26
  Email:  zwj_uptech@126.com
  Description:    // 标定列表
  Others:         // 1.1加入白线标定
*************************************************/
#if !defined(AFX_ITEMLIST_H__28F9E868_8BA0_4EAB_8EBA_6C04E5E90542__INCLUDED_)
#define AFX_ITEMLIST_H__28F9E868_8BA0_4EAB_8EBA_6C04E5E90542__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ItemList.h : header file
//

// #include "ColorTest.h"
// #include "Color.h"
#include "VoyInclude/VoyHSI.h"

/////////////////////////////////////////////////////////////////////////////
// CItemList window

class CItemList : public CListCtrl
{
    // Construction
  public:
    CListBox *plist;
    CWnd *pParent;
    HSISet *pset;
    CItemList();

    // Attributes
  public:
    // CColorTest* pTest;
    // CColor* pclr;
    int lastsel;

    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CItemList)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    void UpdateDisplay();
    void ReSelect();
    void ShowParam(HSISet inHsi);
    virtual ~CItemList();

    // Generated message map functions
  protected:
    //{{AFX_MSG(CItemList)
    afx_msg BOOL OnEraseBkgnd(CDC *pDC);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITEMLIST_H__28F9E868_8BA0_4EAB_8EBA_6C04E5E90542__INCLUDED_)
