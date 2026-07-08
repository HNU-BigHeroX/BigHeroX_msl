#if !defined(AFX_CAMERAPROCDLG_H__6350BE51_DC72_48AA_842B_5A6BB405D18A__INCLUDED_)
#define AFX_CAMERAPROCDLG_H__6350BE51_DC72_48AA_842B_5A6BB405D18A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CameraProcDlg.h : header file
//
// #include "VoyInclude\CaptureVideo.h"
#include "VoyInclude\VoyHSI.h"

/////////////////////////////////////////////////////////////////////////////
// CCameraProcDlg dialog

class CCameraProcDlg : public CDialog
{
    // Construction
  public:
    void InitBar();
    //	CCaptureVideo* pCap;
    HSISet *phsi;
    bool bShowCameraProc;
    CCameraProcDlg(CWnd *pParent = NULL); // standard constructor

    // Dialog Data
    //{{AFX_DATA(CCameraProcDlg)
    enum
    {
        IDD = IDD_CAMERAPROC_DIALOG
    };
    CSliderCtrl m_cSharSlider;
    CSliderCtrl m_cSatuSlider;
    CSliderCtrl m_cGainSlider;
    CSliderCtrl m_cExpoSlider;
    CSliderCtrl m_cContSlider;
    CSliderCtrl m_cBriSlider;
    CSliderCtrl m_cWhiteB_Slider;
    CSliderCtrl m_cBackLSlider;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCameraProcDlg)
  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    bool bApply;
    void m_Apply();

    // Generated message map functions
    //{{AFX_MSG(CCameraProcDlg)
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnCustomdrawWbSlider(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCustomdrawGainSlider(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCustomdrawExpoSlider(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCustomdrawBriSlider(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCustomdrawContSlider(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCustomdrawSatuSlider(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCustomdrawSharSlider(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCustomdrawBacklSlider(NMHDR *pNMHDR, LRESULT *pResult);
    virtual void OnOK();
    afx_msg void OnDefaultval();
    afx_msg void OnTimer(UINT nIDEvent);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAMERAPROCDLG_H__6350BE51_DC72_48AA_842B_5A6BB405D18A__INCLUDED_)
