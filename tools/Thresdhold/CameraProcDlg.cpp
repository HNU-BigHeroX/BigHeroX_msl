// CameraProcDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Thresdhold.h"
#include "CameraProcDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCameraProcDlg dialog

CCameraProcDlg::CCameraProcDlg(CWnd *pParent /*=NULL*/) : CDialog(CCameraProcDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CCameraProcDlg)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT

    //	pCap = NULL;
    bShowCameraProc = false;
}

void CCameraProcDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CCameraProcDlg)
    DDX_Control(pDX, IDC_SHAR_SLIDER, m_cSharSlider);
    DDX_Control(pDX, IDC_SATU_SLIDER, m_cSatuSlider);
    DDX_Control(pDX, IDC_GAIN_SLIDER, m_cGainSlider);
    DDX_Control(pDX, IDC_EXPO_SLIDER, m_cExpoSlider);
    DDX_Control(pDX, IDC_CONT_SLIDER, m_cContSlider);
    DDX_Control(pDX, IDC_BRI_SLIDER, m_cBriSlider);
    DDX_Control(pDX, IDC_WB_SLIDER, m_cWhiteB_Slider);
    DDX_Control(pDX, IDC_BACKL_SLIDER, m_cBackLSlider);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCameraProcDlg, CDialog)
//{{AFX_MSG_MAP(CCameraProcDlg)
ON_WM_SHOWWINDOW()
ON_NOTIFY(NM_CUSTOMDRAW, IDC_WB_SLIDER, OnCustomdrawWbSlider)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_GAIN_SLIDER, OnCustomdrawGainSlider)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_EXPO_SLIDER, OnCustomdrawExpoSlider)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_BRI_SLIDER, OnCustomdrawBriSlider)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_CONT_SLIDER, OnCustomdrawContSlider)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_SATU_SLIDER, OnCustomdrawSatuSlider)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_SHAR_SLIDER, OnCustomdrawSharSlider)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_BACKL_SLIDER, OnCustomdrawBacklSlider)
ON_BN_CLICKED(IDC_DEFAULTVAL, OnDefaultval)
ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCameraProcDlg message handlers

void CCameraProcDlg::InitBar()
{
    // 	if (pCap == NULL)
    // 	{
    // 		return;
    // 	}
    //
    // 	//init camera
    // 	pCap->SetCameraProperty( &(phsi->cameraproc) );
    //
    // 	bApply = false;
    // 	//init bar
    // 	pCap->Bar_Init(VideoProcAmp_WhiteBalance,&m_cWhiteB_Slider);
    // 	pCap->Bar_Init(VideoProcAmp_Brightness,&m_cBriSlider);
    // 	pCap->Bar_Init(VideoProcAmp_Contrast,&m_cContSlider);
    // 	pCap->Bar_Init(VideoProcAmp_Saturation,&m_cSatuSlider);
    // 	pCap->Bar_Init(VideoProcAmp_Sharpness,&m_cSharSlider);
    // 	pCap->Bar_Init(VideoProcAmp_BacklightCompensation,&m_cBackLSlider);
    // 	pCap->Bar_Init(VideoProcAmp_Gain,&m_cGainSlider);
    //
    // 	pCap->Bar_Init(CameraControl_Exposure,&m_cExpoSlider);
    bApply = true;

    // init bar pos
    m_cWhiteB_Slider.SetPos(phsi->cameraproc.nWhiteBalance);
    m_cBriSlider.SetPos(phsi->cameraproc.nBrightness);
    m_cContSlider.SetPos(phsi->cameraproc.nContrast);
    m_cSatuSlider.SetPos(phsi->cameraproc.nSaturation);
    m_cSharSlider.SetPos(phsi->cameraproc.nSharpness);
    m_cBackLSlider.SetPos(phsi->cameraproc.nBacklightCompensation);
    m_cGainSlider.SetPos(phsi->cameraproc.nGain);

    m_cExpoSlider.SetPos(phsi->cameraproc.nExposure);
}

void CCameraProcDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);

    // TODO: Add your message handler code here
    bApply = true;
    InitBar();
}

void CCameraProcDlg::OnCustomdrawWbSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    CString str;
    int val = m_cWhiteB_Slider.GetPos();
    str.Format("%d", val);
    ((CWnd *)GetDlgItem(IDC_WB_VAL))->SetWindowText(str);
    phsi->cameraproc.nWhiteBalance = val;

    m_Apply();

    *pResult = 0;
}

void CCameraProcDlg::OnCustomdrawExpoSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here

    CString str;
    int val = m_cExpoSlider.GetPos();

    switch (val)
    {
    case -14:
        str.Format("n/a");
        break;

    case -13:

        str.Format("1/819");
        break;

    case -12:
        str.Format("1/410");
        break;

    case -11:
        str.Format("1/205");
        break;

    case -10:
        str.Format("1/102");
        break;

    case -9:
        str.Format("1/51");
        break;

    case -8:
        str.Format("1/40");
        break;

    case -7:
        str.Format("1/30");
        break;

    case -6:
        str.Format("1/25");
        break;

    case -5:
        str.Format("1/20");
        break;
    case -4:
        str.Format("1/15");
        break;
    case -3:
        str.Format("1/10");
        break;
    case -2:
    case -1:
    case 0:
        str.Format("1/5");
        break;
    }

    ((CWnd *)GetDlgItem(IDC_EXPO_VAL))->SetWindowText(str);

    m_Apply();

    *pResult = 0;
}

void CCameraProcDlg::OnCustomdrawGainSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here

    CString str;
    int val = m_cGainSlider.GetPos();
    str.Format("%d", val);
    ((CWnd *)GetDlgItem(IDC_GAIN_VAL))->SetWindowText(str);

    m_Apply();

    *pResult = 0;
}

void CCameraProcDlg::OnCustomdrawBriSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here

    CString str;
    int val = m_cBriSlider.GetPos();
    str.Format("%d", val);
    ((CWnd *)GetDlgItem(IDC_BRI_VAL))->SetWindowText(str);

    m_Apply();

    *pResult = 0;
}

void CCameraProcDlg::OnCustomdrawContSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here

    CString str;
    int val = m_cContSlider.GetPos();
    str.Format("%d", val);
    ((CWnd *)GetDlgItem(IDC_CONT_VAL))->SetWindowText(str);

    m_Apply();

    *pResult = 0;
}

void CCameraProcDlg::OnCustomdrawSatuSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here

    CString str;
    int val = m_cSatuSlider.GetPos();
    str.Format("%d", val);
    ((CWnd *)GetDlgItem(IDC_SATU_VAL))->SetWindowText(str);

    m_Apply();

    *pResult = 0;
}

void CCameraProcDlg::OnCustomdrawSharSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here

    CString str;
    int val = m_cSharSlider.GetPos();
    str.Format("%d", val);
    ((CWnd *)GetDlgItem(IDC_SHAR_VAL))->SetWindowText(str);

    m_Apply();

    *pResult = 0;
}

void CCameraProcDlg::OnCustomdrawBacklSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here

    CString str;
    int val = m_cBackLSlider.GetPos();
    str.Format("%d", val);
    ((CWnd *)GetDlgItem(IDC_BACKL_VAL))->SetWindowText(str);

    m_Apply();

    *pResult = 0;
}

void CCameraProcDlg::m_Apply()
{
    // 	if (pCap == NULL || bApply == false)
    // 	{
    // 		return;
    // 	}

    phsi->cameraproc.nBacklightCompensation = m_cBackLSlider.GetPos();
    phsi->cameraproc.nSharpness = m_cSharSlider.GetPos();
    phsi->cameraproc.nSaturation = m_cSatuSlider.GetPos();
    phsi->cameraproc.nContrast = m_cContSlider.GetPos();
    phsi->cameraproc.nBrightness = m_cBriSlider.GetPos();
    phsi->cameraproc.nGain = m_cGainSlider.GetPos();
    phsi->cameraproc.nExposure = m_cExpoSlider.GetPos();

    // pCap->SetCameraProperty( &(phsi->cameraproc) );
}

void CCameraProcDlg::OnOK()
{
    // TODO: Add extra validation here
    m_Apply();

    CDialog::OnOK();
}

void CCameraProcDlg::OnDefaultval()
{
    // TODO: Add your control notification handler code here
    // 	if (pCap != NULL)
    // 	{
    // 		phsi->cameraproc = pCap->m_DefProc;
    // 		pCap->DefaultSet();
    // 		InitBar();
    // 	}
}

void CCameraProcDlg::OnTimer(UINT nIDEvent)
{
    // TODO: Add your message handler code here and/or call default

    CDialog::OnTimer(nIDEvent);
}
