// ThresdholdDlg.h : header file
//

#if !defined(AFX_THRESDHOLDDLG_H__50A472A1_1CB9_4950_A9F1_991D64436751__INCLUDED_)
#define AFX_THRESDHOLDDLG_H__50A472A1_1CB9_4950_A9F1_991D64436751__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Color.h"
#include "VoyInclude/VoyWith2Cap.h"
#include "ColorTest.h"
#include "ItemList.h"
#include "VoyInclude/VoyHSI.h"
#include "StillView.h"
#include "CameraProcDlg.h"
#include "DHCameraProc.h"
#include "Dlg_FramePreview.h"

/////////////////////////////////////////////////////////////////////////////
// CThresdholdDlg dialog

class CThresdholdDlg : public CDialog
{
    // Construction
  public:
    void EmuDis();
    void EmuAct();
    void UpdateAll(int inIndex); // ¸üÐÂitemlistºÍcolor
    void ShowParam(HSISet inHsi);
    CThresdholdDlg(CWnd *pParent = NULL); // standard constructor

    // Dialog Data
    //{{AFX_DATA(CThresdholdDlg)
    enum
    {
        IDD = IDD_THRESDHOLD_DIALOG
    };
    CListBox m_cInfoList;
    CStillView m_cVideo;
    CItemList m_cList;
    CComboBox m_cCamList;
    CStillView m_cView;
    CColor m_cColor;
    BOOL m_bOverCenter;
    BOOL m_bFront;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CThresdholdDlg)
  public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);

  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    void m_printf(CString instr);
    IBehavior m_idle;
    VoyHSI m_hsiset;
    CColorTest m_test;
    HICON m_hIcon;
    CVoyWith2Cap m_voy;
    int m_nLastCamIndex;

    // CCameraProcDlg m_camdlg;
    CDHCameraProc m_DHCamDlg;

    CDlg_FramePreview m_dlgFramePreview;

    // Generated message map functions
    //{{AFX_MSG(CThresdholdDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnCreatcap();
    afx_msg void OnClosecap();
    afx_msg void OnSavehsi();
    afx_msg void OnLoadhsi();
    afx_msg void OnClose();
    afx_msg void OnPreview();
    afx_msg void OnDspprop();
    afx_msg void OnOvercenter();
    afx_msg void OnFrontang();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnClickItemlist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnCapturestill();
    afx_msg void OnLoadimg();
    afx_msg void OnVideoemu();
    afx_msg void OnFramePreview();
    afx_msg void OnAutoCol();
    afx_msg void On1KeyRead();
    afx_msg void On1Keysave();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THRESDHOLDDLG_H__50A472A1_1CB9_4950_A9F1_991D64436751__INCLUDED_)
