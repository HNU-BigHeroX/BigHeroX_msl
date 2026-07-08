#if !defined(AFX_WNDMATCHPANEL_H__C5B4CC5D_754E_4BEF_820B_3D1BF1348868__INCLUDED_)
#define AFX_WNDMATCHPANEL_H__C5B4CC5D_754E_4BEF_820B_3D1BF1348868__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndMatchPanel.h : header file
//

#include "DataCenter.h"
#include "RefClient.h"
#include "MSL_Structs.h"
#include "GreenField.h"
/////////////////////////////////////////////////////////////////////////////
// CWndMatchPanel dialog

class CWndMatchPanel : public CDialog
{
    // Construction
  public:
    void MoveTest(int inID);
    CWndMatchPanel(CWnd *pParent = NULL); // standard constructor
    CDataCenter *pDataCenter;             // 指向数据中心
    CRefClient *pRefClient;               // 指向裁判盒解析
    stKeyPress *pKey;                     // 按键消息
    CGreenField *pFiled;                  // 指向地图控件

    bool m_bPause; // 暂停信号
    // Dialog Data
    //{{AFX_DATA(CWndMatchPanel)
    enum
    {
        IDD = IDD_CHILD_MATCH_PANEL
    };
    CButton m_cBtnPause;
    CListBox m_cTacInfoList;
    int m_nAgentTestCtrlAngle;
    int m_nAgentTestCtrlDist;
    int m_nAgentTestCtrlNum;
    int m_nDefDist;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CWndMatchPanel)
  public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);

  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  public:
    // Generated message map functions
    //{{AFX_MSG(CWndMatchPanel)
    afx_msg void OnRefCynKickoff();
    afx_msg void OnRefCynFreekick();
    afx_msg void OnRefCynGoalkick();
    afx_msg void OnRefCynThrowin();
    afx_msg void OnRefCynCornerkick();
    afx_msg void OnRefCynPenalty();
    afx_msg void OnRefStart();
    afx_msg void OnRefStop();
    afx_msg void OnPanelPause();
    afx_msg void OnRefParking();
    afx_msg void OnRefDroppedball();
    afx_msg void OnRefMagentaKickoff();
    afx_msg void OnRefMagentaFreekick();
    afx_msg void OnRefMagentaGoalkick();
    afx_msg void OnRefMagentaThrowin();
    afx_msg void OnRefMagentaCornerkick();
    afx_msg void OnRefMagentaPenalty();
    afx_msg void OnAgtestAttack();
    afx_msg void OnAgtestGoalkeeper();
    afx_msg void OnAgtestCatchMove();
    afx_msg void OnAgtestCatch();
    afx_msg void OnAgtestPass();
    afx_msg void OnAgtestCtrlset();
    afx_msg void OnAgtestDefball();
    afx_msg void OnAgtestDefpass();
    afx_msg void OnEnemyTrack();
    afx_msg void OnTacStartAuto();
    afx_msg void OnTacTechchallenge();
    afx_msg void OnTacTestPassing();
    afx_msg void OnTacTestMovearound();
    afx_msg void OnTacTestCatchmove();
    afx_msg void OnTacTest4pass();
    afx_msg void OnTacTestAlldef();
    afx_msg void OnCleanTacinfo();
    afx_msg void OnMovetest();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDMATCHPANEL_H__C5B4CC5D_754E_4BEF_820B_3D1BF1348868__INCLUDED_)
