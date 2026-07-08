#if !defined(AFX_WNDMAP_H__CC95E846_779E_43ED_B132_49BD4200A8F5__INCLUDED_)
#define AFX_WNDMAP_H__CC95E846_779E_43ED_B132_49BD4200A8F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndMap.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWndMap dialog

class CWndMap : public CDialog
{
    // Construction
  public:
    CWndMap(CWnd *pParent = NULL); // standard constructor

    // Dialog Data
    //{{AFX_DATA(CWndMap)
    enum
    {
        IDD = IDD_CHILD_MAP
    };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CWndMap)
  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    // Generated message map functions
    //{{AFX_MSG(CWndMap)
    // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDMAP_H__CC95E846_779E_43ED_B132_49BD4200A8F5__INCLUDED_)
