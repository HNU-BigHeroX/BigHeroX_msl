#if !defined(AFX_DLGVIEW_H__2064EB30_68BC_43F5_A2D5_1D8676F849A6__INCLUDED_)
#define AFX_DLGVIEW_H__2064EB30_68BC_43F5_A2D5_1D8676F849A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgView.h : header file
//
#define WM_VIEWKEYDOWN WM_USER + 1997
#define WM_VIEWMOVEING WM_USER + 1998
#define WM_VIEWCLOSED WM_USER + 1999
/////////////////////////////////////////////////////////////////////////////
// CDlgView dialog

class CDlgView : public CDialog
{
    // Construction
  public:
    CDlgView(CWnd *pParent = NULL); // standard constructor

    HWND m_hParent;

    // Dialog Data
    //{{AFX_DATA(CDlgView)
    enum
    {
        IDD = IDD_DLG_VIEW
    };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDlgView)
  public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);

  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    // Generated message map functions
    //{{AFX_MSG(CDlgView)
    afx_msg void OnClose();
    afx_msg void OnMove(int x, int y);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGVIEW_H__2064EB30_68BC_43F5_A2D5_1D8676F849A6__INCLUDED_)
