#if !defined(AFX_DLGDEBUGINFO_H__F40AAAF2_2457_44A9_8030_09F54940C4F8__INCLUDED_)
#define AFX_DLGDEBUGINFO_H__F40AAAF2_2457_44A9_8030_09F54940C4F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDebugInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgDebugInfo dialog

class CDlgDebugInfo : public CDialog
{
    // Construction
  public:
    CDlgDebugInfo(CWnd *pParent = NULL); // standard constructor

    // Dialog Data
    //{{AFX_DATA(CDlgDebugInfo)
    enum
    {
        IDD = IDD_DEBUG_INFO
    };
    CEdit m_cSigInfo;
    CListBox m_cInfoList;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDlgDebugInfo)
  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    // Generated message map functions
    //{{AFX_MSG(CDlgDebugInfo)
    // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDEBUGINFO_H__F40AAAF2_2457_44A9_8030_09F54940C4F8__INCLUDED_)
