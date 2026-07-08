// DHCameraProc.cpp : implementation file
//

#include "stdafx.h"
#include "Thresdhold.h"
#include "DHCameraProc.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDHCameraProc dialog

CDHCameraProc::CDHCameraProc(CWnd *pParent /*=NULL*/) : CDialog(CDHCameraProc::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDHCameraProc)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

void CDHCameraProc::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDHCameraProc)
    DDX_Control(pDX, IDC_CM_ADC, m_ctlCMADC);
    DDX_Control(pDX, IDC_CM_GAIN, m_ctlCMGain);
    DDX_Control(pDX, IDC_LUTRED_SLIDER, m_cLutRedSlider);
    DDX_Control(pDX, IDC_LUTGREEN_SLIDER, m_cLutGreenSlider);
    DDX_Control(pDX, IDC_LUTBLUE_SLIDER, m_cLutBlueSlider);
    DDX_Control(pDX, IDC_LUTALL_SLIDER, m_cLutAllSlider);
    DDX_Control(pDX, IDC_GAIN2_SLIDER, m_cGainSlider);
    DDX_Control(pDX, IDC_EU_SLIDER, m_cExpUpperSlider);
    DDX_Control(pDX, IDC_EL_SLIDER, m_cExpLowerSlider);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDHCameraProc, CDialog)
//{{AFX_MSG_MAP(CDHCameraProc)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_EL_SLIDER, OnCustomdrawElSlider)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_EU_SLIDER, OnCustomdrawEuSlider)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_GAIN2_SLIDER, OnCustomdrawGain2Slider)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_LUTALL_SLIDER, OnCustomdrawLutallSlider)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_LUTBLUE_SLIDER, OnCustomdrawLutblueSlider)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_LUTGREEN_SLIDER, OnCustomdrawLutgreenSlider)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_LUTRED_SLIDER, OnCustomdrawLutredSlider)
ON_CBN_SELCHANGE(IDC_CM_ADC, OnSelchangeCmAdc)
ON_CBN_SELCHANGE(IDC_CM_GAIN, OnSelchangeCmGain)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDHCameraProc message handlers

BOOL CDHCameraProc::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here

    // 增益初始化
    m_ctlCMGain.AddString("Red  channel");
    m_ctlCMGain.AddString("Red  channel green");
    m_ctlCMGain.AddString("Blue channel green");
    m_ctlCMGain.AddString("Blue channel");
    m_ctlCMGain.AddString("All  channel");
    m_ctlCMGain.SetCurSel(4); // 初始化为“all”
    m_cGainSlider.SetRange(0, 127);
    OnSelchangeCmGain();

    // ADC初始化
    m_ctlCMADC.AddString("Level 0");
    m_ctlCMADC.AddString("Level 1");
    m_ctlCMADC.AddString("Level 2");
    m_ctlCMADC.AddString("Level 3");

    // 白平衡初始化(未完成)
    m_cLutAllSlider.SetRange(0, 200);
    // 	double m_dR,m_dG,m_dB;

    // 	m_cLutRedSlider.SetRange(0,200);
    // 	m_cLutRedSlider.SetLineSize(1);
    //
    // 	m_cLutGreenSlider.SetRange(0,200);
    //
    // 	m_cLutBlueSlider.SetRange(0,200);

    // 曝光时间初始化

    return TRUE; // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CDHCameraProc::m_DefaultVal()
{
}

void CDHCameraProc::OnCustomdrawElSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    int nLower = m_cExpLowerSlider.GetPos();
    nLower = pow(10, nLower);

    int nUpper = m_cExpUpperSlider.GetPos();

    strVal.Format("%d", nLower);
    ((CWnd *)GetDlgItem(IDC_EL_VAL))->SetWindowText(strVal);

    *pResult = 0;
}

void CDHCameraProc::OnCustomdrawEuSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    int nLower = m_cExpLowerSlider.GetPos();
    nLower = pow(10, nLower);

    int nUpper = m_cExpUpperSlider.GetPos();

    strVal.Format("%d", nUpper);
    ((CWnd *)GetDlgItem(IDC_EU_VAL))->SetWindowText(strVal);

    *pResult = 0;
}

void CDHCameraProc::OnCustomdrawGain2Slider(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    int nGain = m_cGainSlider.GetPos();

    strVal.Format("%d", nGain);

    ((CWnd *)GetDlgItem(IDC_GAIN2_VAL))->SetWindowText(strVal);

    *pResult = 0;
}

void CDHCameraProc::OnCustomdrawLutallSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    double fLutAll = (double)m_cLutAllSlider.GetPos();
    fLutAll /= 100;

    strVal.Format("%.2f", fLutAll);

    ((CWnd *)GetDlgItem(IDC_LUTALL_VAL))->SetWindowText(strVal);

    *pResult = 0;
}

void CDHCameraProc::OnCustomdrawLutblueSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here

    *pResult = 0;
}

void CDHCameraProc::OnCustomdrawLutgreenSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here

    *pResult = 0;
}

void CDHCameraProc::OnCustomdrawLutredSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here

    *pResult = 0;
}

void CDHCameraProc::OnSelchangeCmAdc()
{
    // TODO: Add your control notification handler code here
}

void CDHCameraProc::OnSelchangeCmGain()
{
    // TODO: Add your control notification handler code here
}
