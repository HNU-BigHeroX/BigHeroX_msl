// MSLAgentDlg.h : header file
//

#if !defined(AFX_MSLAGENTDLG_H__F86D54BF_62D9_410E_85D3_050110836263__INCLUDED_)
#define AFX_MSLAGENTDLG_H__F86D54BF_62D9_410E_85D3_050110836263__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UPFullField.h"
#include "NumDisplay.h"
#include "AgentUDP.h"
#include "CPU_Battery.h"
/////////////////////////////////////////////////////////////////////////////
// CMSLAgentDlg dialog

class CMSLAgentDlg : public CDialog
{
    // Construction
  public:
    CString GetCtrl(UINT inctrl);
    CMSLAgentDlg(CWnd *pParent = NULL); // standard constructor

    // Dialog Data
    //{{AFX_DATA(CMSLAgentDlg)
    enum
    {
        IDD = IDD_MSLAGENT_DIALOG
    };
    CNumDisplay m_cNumDis;
    CUPFullField m_nFullField;
    CListCtrl m_cAlist;
    CListBox m_cList;
    int m_nBallx;
    int m_nBally;
    int m_nID;
    int m_nSelfy;
    CString m_strIP;
    int m_nPort;
    int m_nSelfx;
    int m_nMballx;
    int m_nMbally;
    BOOL m_bAddAg1;
    BOOL m_bAddAg2;
    BOOL m_bAddAg3;
    BOOL m_bAddAg4;
    BOOL m_bAddAg5;
    int m_nVirtualID1;
    int m_nVirtualID2;
    int m_nVirtualID3;
    int m_nVirtualID4;
    int m_nIDWhoGetBall;
    BOOL m_bAtkIm;
    int m_nObstID;
    int m_nCycle;
    int m_nObstIDToRemove;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMSLAgentDlg)
  public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);

  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    HICON m_hIcon;
    CAgentUDP m_agUDP;
    bool m_bAllBalls;
    CCPU_Battery m_cpu_battery;

    // CAgentNet m_agentnets[7];
    bool m_bmulti;

    // Generated message map functions
    //{{AFX_MSG(CMSLAgentDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnConnect();
    afx_msg void OnSend();
    afx_msg void OnCombo();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnStop();
    afx_msg void OnMultiagebt();
    afx_msg void OnUpdateball();
    afx_msg void OnClose();
    afx_msg void OnCntnsnd();
    afx_msg void OnCtrlallball();
    afx_msg void OnAddag1();
    afx_msg void OnAddag2();
    afx_msg void OnAddag3();
    afx_msg void OnAddag4();
    afx_msg void OnAddag5();
    afx_msg void OnVirtualatk1();
    afx_msg void OnVirtualatk2();
    afx_msg void OnVirtualkop();
    afx_msg void OnVirtualkos();
    afx_msg void OnCancelvir1();
    afx_msg void OnCancelvir2();
    afx_msg void OnCancelvir3();
    afx_msg void OnCancelvir4();
    afx_msg void OnVirtualgk();
    afx_msg void OnBallmissing();
    afx_msg void OnAggetball();
    afx_msg void OnAtkim();
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnAddobst();
    afx_msg void OnNoaddobst();
    afx_msg void OnSettrueobst();
    afx_msg void OnErr();
    afx_msg void OnRemoveobst();
    afx_msg void OnObstBust();
    afx_msg void OnClearList();
    afx_msg void OnTocaptain();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSLAGENTDLG_H__F86D54BF_62D9_410E_85D3_050110836263__INCLUDED_)
