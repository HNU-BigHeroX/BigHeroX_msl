#if !defined(AFX_DLGDEBUGVIEW_H__EFCC7477_E940_4E41_9F90_477779D64B9F__INCLUDED_)
#define AFX_DLGDEBUGVIEW_H__EFCC7477_E940_4E41_9F90_477779D64B9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDebugView.h : header file
//
#include "DebugView.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgDebugView dialog

class CDlgDebugView : public CDialog
{
    // Construction
  public:
    CDlgDebugView(CWnd *pParent = NULL); // standard constructor

    // Dialog Data
    //{{AFX_DATA(CDlgDebugView)
    enum
    {
        IDD = IDD_DEBUG_VIEW
    };
    CDebugView m_cDbgView;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDlgDebugView)
  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    // Generated message map functions
    //{{AFX_MSG(CDlgDebugView)
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg void OnClean();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDEBUGVIEW_H__EFCC7477_E940_4E41_9F90_477779D64B9F__INCLUDED_)
