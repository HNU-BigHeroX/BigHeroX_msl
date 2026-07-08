// ¶ÓÔ±ipÏÔÊ¾
#if !defined(AFX_DLGAGENTIPLIST_H__58937C2C_2CA5_417C_9FBC_5262B7773A3F__INCLUDED_)
#define AFX_DLGAGENTIPLIST_H__58937C2C_2CA5_417C_9FBC_5262B7773A3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAgentIPList.h : header file
//

#include "CoachUDP.h"
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgAgentIPList dialog

class CDlgAgentIPList : public CDialog
{
    // Construction
  public:
    CDlgAgentIPList(CWnd *pParent = NULL); // standard constructor

    CCoachUDP *pCoachUDP;
    // Dialog Data
    //{{AFX_DATA(CDlgAgentIPList)
    enum
    {
        IDD = IDD_DLG_AGENT_IP
    };
    CListBox m_cAgentIPList;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDlgAgentIPList)
  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    // Generated message map functions
    //{{AFX_MSG(CDlgAgentIPList)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGAGENTIPLIST_H__58937C2C_2CA5_417C_9FBC_5262B7773A3F__INCLUDED_)
