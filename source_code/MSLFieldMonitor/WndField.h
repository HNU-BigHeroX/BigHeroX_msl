#if !defined(AFX_WNDFIELD_H__5FD1163B_D73B_49D2_B5A8_F6EE21438789__INCLUDED_)
#define AFX_WNDFIELD_H__5FD1163B_D73B_49D2_B5A8_F6EE21438789__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndField.h : header file
//

#include "GreenField.h"
/////////////////////////////////////////////////////////////////////////////
// CWndField dialog

class CWndField : public CDialog
{
    // Construction
  public:
    CWndField(CWnd *pParent = NULL); // standard constructor

    // Dialog Data
    //{{AFX_DATA(CWndField)
    enum
    {
        IDD = IDD_CHILD_FIELD
    };
    CGreenField m_cGreenField;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CWndField)
  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    // Generated message map functions
    //{{AFX_MSG(CWndField)
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDFIELD_H__5FD1163B_D73B_49D2_B5A8_F6EE21438789__INCLUDED_)
