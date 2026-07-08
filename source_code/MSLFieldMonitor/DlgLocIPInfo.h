// ±¾µØipÏÔÊ¾
#if !defined(AFX_DLGLOCIPINFO_H__DF551A1E_DF92_4448_9509_57DBFE36DC12__INCLUDED_)
#define AFX_DLGLOCIPINFO_H__DF551A1E_DF92_4448_9509_57DBFE36DC12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgLocIPInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgLocIPInfo dialog

class CDlgLocIPInfo : public CDialog
{
    // Construction
  public:
    void GetLocIP();
    CDlgLocIPInfo(CWnd *pParent = NULL); // standard constructor

    // Dialog Data
    //{{AFX_DATA(CDlgLocIPInfo)
    enum
    {
        IDD = IDD_DLG_LOC_IP
    };
    CListBox m_cLocIPInfo;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDlgLocIPInfo)
  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    // Generated message map functions
    //{{AFX_MSG(CDlgLocIPInfo)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLOCIPINFO_H__DF551A1E_DF92_4448_9509_57DBFE36DC12__INCLUDED_)
