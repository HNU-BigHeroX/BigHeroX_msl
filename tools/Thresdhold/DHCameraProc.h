#if !defined(AFX_DHCAMERAPROC_H__7DC1EA4B_A03C_4B72_9E23_00FE94E38078__INCLUDED_)
#define AFX_DHCAMERAPROC_H__7DC1EA4B_A03C_4B72_9E23_00FE94E38078__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DHCameraProc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDHCameraProc dialog

class CDHCameraProc : public CDialog
{
    // Construction
  public:
    CDHCameraProc(CWnd *pParent = NULL); // standard constructor
    CString strVal;

    // Dialog Data
    //{{AFX_DATA(CDHCameraProc)
    enum
    {
        IDD = IDD_DHCAMERA_DIALOG
    };
    CComboBox m_ctlCMADC;
    CComboBox m_ctlCMGain;
    CSliderCtrl m_cLutRedSlider;
    CSliderCtrl m_cLutGreenSlider;
    CSliderCtrl m_cLutBlueSlider;
    CSliderCtrl m_cLutAllSlider;
    CSliderCtrl m_cGainSlider;
    CSliderCtrl m_cExpUpperSlider;
    CSliderCtrl m_cExpLowerSlider;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDHCameraProc)
  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    void m_DefaultVal();
    int m_nGainChannel;

    // Generated message map functions
    //{{AFX_MSG(CDHCameraProc)
    virtual BOOL OnInitDialog();
    afx_msg void OnCustomdrawElSlider(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCustomdrawEuSlider(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCustomdrawGain2Slider(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCustomdrawLutallSlider(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCustomdrawLutblueSlider(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCustomdrawLutgreenSlider(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCustomdrawLutredSlider(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSelchangeCmAdc();
    afx_msg void OnSelchangeCmGain();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DHCAMERAPROC_H__7DC1EA4B_A03C_4B72_9E23_00FE94E38078__INCLUDED_)
