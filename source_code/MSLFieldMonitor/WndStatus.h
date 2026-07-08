#if !defined(AFX_WNDSTATUS_H__EA2CE911_3D5D_4C49_A534_783F513FBF4A__INCLUDED_)
#define AFX_WNDSTATUS_H__EA2CE911_3D5D_4C49_A534_783F513FBF4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndStatus.h : header file
//

#include "DisRobotStatus.h"
#include "DisRoleStatus.h"
#include "ObstFusion_ZWJ.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "Tac/Tac_TechChallenge.h"
/////////////////////////////////////////////////////////////////////////////
// CWndStatus dialog

class CWndStatus : public CDialog
{
    // Construction
  public:
    CString GetTacString(int inTac);
    void UpdateRoleInfo();
    void UpdateDisInfo();
    CWndStatus(CWnd *pParent = NULL); // standard constructor

    CTac_TechChallenge *pTac_TC;
    CObstFusion_ZWJ *pObstFusion;
    // Dialog Data
    //{{AFX_DATA(CWndStatus)
    enum
    {
        IDD = IDD_CHILD_STATUS
    };
    CDisRoleStatus m_disRoleStatus;
    CDisRobotStatus m_disRobotStatus;
    CEdit m_cDisInfo;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CWndStatus)
  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    // Generated message map functions
    //{{AFX_MSG(CWndStatus)
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDSTATUS_H__EA2CE911_3D5D_4C49_A534_783F513FBF4A__INCLUDED_)
