// SCPlatformDlg.h : header file
//

#if !defined(AFX_SCPLATFORMDLG_H__0B0CBA10_2AEE_4145_9E29_8DB61F6AAD89__INCLUDED_)
#define AFX_SCPLATFORMDLG_H__0B0CBA10_2AEE_4145_9E29_8DB61F6AAD89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VoyCmd.h"

#define TMR_AUTOSEND 1
#define TMR_PRESS_TEST 2

/////////////////////////////////////////////////////////////////////////////
// CSCPlatformDlg dialog

class CSCPlatformDlg : public CDialog
{
    // Construction
  public:
    void DealWithPos(int inIndex);
    void DealWithMotor(int inIndex);
    void m_PressTestStep();
    void SendCmd();
    CSCPlatformDlg(CWnd *pParent = NULL); // standard constructor

    // Dialog Data
    //{{AFX_DATA(CSCPlatformDlg)
    enum
    {
        IDD = IDD_SCPLATFORM_DIALOG
    };
    CEdit m_cDisIO;
    CEdit m_cDisAD;
    CEdit m_cDisMotor;
    CListBox m_cRecvList;
    CListBox m_cInfoList;
    CListBox m_cSendList;
    int m_nRobotPort;
    int m_nSendLoopTime;
    int m_nInpManMotor1;
    int m_nInpManMotor2;
    int m_nInpManMotor3;
    int m_nInpManMotor4;
    int m_nInpManMotor5;
    int m_nInpManMotor6;
    int m_nKickPower;
    int m_nRobotRate;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSCPlatformDlg)
  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    HICON m_hIcon;
    CVoyCmd m_cmd;
    int m_nMotorVal[6];

    // Generated message map functions
    //{{AFX_MSG(CSCPlatformDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    afx_msg void OnBtnOpenrobotPort();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnBtnAutostart();
    afx_msg void OnBtnAutostop();
    afx_msg void OnBtnBrakeall();
    afx_msg void OnBtnManuSend();
    afx_msg void OnBtnCloserobotPort();
    afx_msg void OnBtnKick();
    afx_msg void OnBtnTestStart();
    afx_msg void OnBtnTestStop();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCPLATFORMDLG_H__0B0CBA10_2AEE_4145_9E29_8DB61F6AAD89__INCLUDED_)
