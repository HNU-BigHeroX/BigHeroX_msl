#if !defined(AFX_WNDREFEREE_H__4B6FD3BD_566D_4C55_AB1A_B0FE97746B98__INCLUDED_)
#define AFX_WNDREFEREE_H__4B6FD3BD_566D_4C55_AB1A_B0FE97746B98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndReferee.h : header file
//

#include "DisSideClr.h"
#include "afxwin.h"
#include "MSL_Structs.h"
#include "DlgLocIPInfo.h"
#include "RefClient.h"
#include "DlgAgentIPList.h"
/////////////////////////////////////////////////////////////////////////////
// CWndReferee dialog

class CWndReferee : public CDialog
{
    // Construction
  public:
    CWndReferee(CWnd *pParent = NULL); // standard constructor

    // Dialog Data
    //{{AFX_DATA(CWndReferee)
    enum
    {
        IDD = IDD_CHILD_REFEREE
    };
    CDisSideClr m_cSideClr;
    CListBox m_cDataInfoList;
    CListBox m_cRefInfoList;
    CComboBox m_cRefProtocol;
    CString m_strRefBoxIP;
    //}}AFX_DATA

    CRefClient *pRefClient;
    CDlgLocIPInfo m_dlgLocIPInfo;
    CDlgAgentIPList m_dlgAgentIPList;

    stKeyPress *pKey; // °´¼ü¼ÇÂ¼
                      // Overrides
                      // ClassWizard generated virtual function overrides
                      //{{AFX_VIRTUAL(CWndReferee)
  public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);

  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    // Generated message map functions
    //{{AFX_MSG(CWndReferee)
    afx_msg void OnRefCnt();
    afx_msg void OnRefDiscnt();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTimer(UINT nIDEvent);
    virtual BOOL OnInitDialog();
    afx_msg void OnBtnLocIp();
    afx_msg void OnBtnAgentIp();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDREFEREE_H__4B6FD3BD_566D_4C55_AB1A_B0FE97746B98__INCLUDED_)
