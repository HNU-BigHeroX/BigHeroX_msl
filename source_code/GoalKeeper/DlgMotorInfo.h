#if !defined(AFX_DLGMOTORINFO_H__8AC288FD_590B_4F9F_BBD0_4AE45E6EB360__INCLUDED_)
#define AFX_DLGMOTORINFO_H__8AC288FD_590B_4F9F_BBD0_4AE45E6EB360__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMotorInfo.h : header file
//

#define WM_MINFOCLOSED WM_USER + 2000
/////////////////////////////////////////////////////////////////////////////
// CDlgMotorInfo dialog

class CDlgMotorInfo : public CDialog
{
    // Construction
  public:
    CDlgMotorInfo(CWnd *pParent = NULL); // standard constructor

    HWND m_hParent;

    // Dialog Data
    //{{AFX_DATA(CDlgMotorInfo)
    enum
    {
        IDD = IDD_DLG_MOTORINFO
    };
    CEdit m_cIOInfo;
    CEdit m_cADInfo;
    CListBox m_cMInfo_List;
    CEdit m_cMotorInfo;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDlgMotorInfo)
  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    // Generated message map functions
    //{{AFX_MSG(CDlgMotorInfo)
    afx_msg void OnClose();
    afx_msg void OnLog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMOTORINFO_H__8AC288FD_590B_4F9F_BBD0_4AE45E6EB360__INCLUDED_)
