// ThresdholdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Thresdhold.h"
#include "ThresdholdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
  public:
    CAboutDlg();

    // Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum
    {
        IDD = IDD_ABOUTBOX
    };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    //{{AFX_MSG(CAboutDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT nIDEvent);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CThresdholdDlg dialog

CThresdholdDlg::CThresdholdDlg(CWnd *pParent /*=NULL*/) : CDialog(CThresdholdDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CThresdholdDlg)
    m_bOverCenter = FALSE;
    m_bFront = FALSE;
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_nLastCamIndex = 999;
}

void CThresdholdDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CThresdholdDlg)
    DDX_Control(pDX, IDC_INFOLIST, m_cInfoList);
    DDX_Control(pDX, IDC_VIDEO, m_cVideo);
    DDX_Control(pDX, IDC_ITEMLIST, m_cList);
    DDX_Control(pDX, IDC_COMBO1, m_cCamList);
    DDX_Control(pDX, IDC_VIEW, m_cView);
    DDX_Control(pDX, IDC_COLOR, m_cColor);
    DDX_Check(pDX, IDC_OVERCENTER, m_bOverCenter);
    DDX_Check(pDX, IDC_FRONTANG, m_bFront);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CThresdholdDlg, CDialog)
//{{AFX_MSG_MAP(CThresdholdDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_CREATCAP, OnCreatcap)
ON_BN_CLICKED(IDC_CLOSECAP, OnClosecap)
ON_BN_CLICKED(IDC_SAVEHSI, OnSavehsi)
ON_BN_CLICKED(IDC_LOADHSI, OnLoadhsi)
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
ON_BN_CLICKED(IDC_DSPPROP, OnDspprop)
ON_BN_CLICKED(IDC_OVERCENTER, OnOvercenter)
ON_BN_CLICKED(IDC_FRONTANG, OnFrontang)
ON_WM_TIMER()
ON_NOTIFY(NM_CLICK, IDC_ITEMLIST, OnClickItemlist)
ON_WM_MOUSEWHEEL()
ON_BN_CLICKED(IDC_CAPTURESTILL, OnCapturestill)
ON_BN_CLICKED(IDC_LOADIMG, OnLoadimg)
ON_BN_CLICKED(IDC_VIDEOEMU, OnVideoemu)
ON_BN_CLICKED(IDC_FRAME_PREVIEW, OnFramePreview)
ON_BN_CLICKED(IDC_BUTTON1, OnAutoCol)
ON_BN_CLICKED(IDC_BUTTON2, On1KeyRead)
ON_BN_CLICKED(IDC_BUTTON3, On1Keysave)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CThresdholdDlg message handlers

BOOL CThresdholdDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu *pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);  // Set big icon
    SetIcon(m_hIcon, FALSE); // Set small icon

    // TODO: Add extra initialization here

    // 列表控件初始化
    m_cList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    CString strname[4];

    strname[0] = "Name";
    strname[1] = "H";
    strname[2] = "S";
    strname[3] = "I";

    for (int i = 0; i < 4; i++)
    {
        m_cList.InsertColumn(i, strname[i], LVCFMT_LEFT, 70);
    }
    CHeaderCtrl *pHeaderCtrl = (CHeaderCtrl *)m_cList.GetHeaderCtrl();
    pHeaderCtrl->EnableWindow(FALSE);

    m_cList.pset = &(m_test.m_vhsi);
    m_cColor.pList = &m_cList;
    m_voy.SetBehavior(&m_test);

    m_cColor.pParam = &(m_test.m_vhsi.overball);
    m_test.bOverBall = true;
    m_cColor.HSIToShow(m_test.m_vhsi.overball);

    // 显示视频设备到列表框
    /*CCaptureVideo cap;
    cap.EnumDevices(m_cCamList);
    m_cCamList.SetCurSel(0);*/

    m_cCamList.SetCurSel(0);

    // 静态图显示
    m_test.pStill = &m_cView;
    m_test.pLive = &m_cVideo;
    m_cVideo.m_fpsDis = 60;
    m_cView.mode = 0;
    m_cView.pColor = &m_cColor;

    // List
    m_test.pList = &m_cInfoList;

    m_cList.pParent = this;

    // camera proc
    // m_camdlg.phsi = &(m_test.m_vhsi);
    // m_DHCamDlg.m_pCam = &(m_voy.m_cOverCap);

    // 读取当前文件夹下的阈值文件
    CFileFind finder;
    if (finder.FindFile("Default.th2") == TRUE)
    {
        m_hsiset.LoadHSIFromINI(".\\Default.th2", &(m_test.m_vhsi), &m_cInfoList);
    }
    ShowParam(m_test.m_vhsi);

    m_dlgFramePreview.pTest = &m_test;

    SetTimer(10086, 1000, NULL);

    return TRUE; // return TRUE  unless you set the focus to a control
}

void CThresdholdDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CThresdholdDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CThresdholdDlg::OnQueryDragIcon()
{
    return (HCURSOR)m_hIcon;
}

void CThresdholdDlg::OnCreatcap()
{
    // TODO: Add your control notification handler code here
    BOOL res;
    //////////////////////////////////////////////////////////////////////////
    /*if (m_cCamList.GetCurSel() != m_nLastCamIndex)
    {
        m_voy.CloseOverCap();
        res = m_voy.CreatOverCap(m_cCamList.GetCurSel(),&m_cVideo);
        m_nLastCamIndex = m_cCamList.GetCurSel();
        //SetTimer(1,10,NULL);
    }
    else
    {
        res = m_voy.m_OverCap->Restart();
    }
    /////////////////////////////////////////////////////////////////////////*/

    m_voy.CloseOverCap();
    res = m_voy.CreatOverCap(0, &m_cVideo);
    Sleep(500);
    CFileFind finder;
    if (finder.FindFile("Default.th2") == TRUE)
    {
        m_voy.m_cOverCap.LoadParamFromINI(".\\Default.th2");
    }

    if (res == FALSE)
    {
        m_printf("视频开启失败！");
        return;
    }
    else
    {
        m_printf("视频打开成功...");
    }

    //////////////////////////////////////////////////////////////////////////
    /*m_voy.m_OverCap->SetCmd(&m_voy.m_cmd);
    m_voy.SetBehavior(&m_test);
    m_camdlg.pCap = m_voy.m_OverCap;*/
    //////////////////////////////////////////////////////////////////////////
    m_voy.SetBehavior(&m_test);

    EmuDis();

    ((CWnd *)GetDlgItem(IDC_CREATCAP))->EnableWindow(FALSE);
    ((CWnd *)GetDlgItem(IDC_CLOSECAP))->EnableWindow(TRUE);
    ((CWnd *)GetDlgItem(IDC_LOADIMG))->EnableWindow(FALSE);
    ((CWnd *)GetDlgItem(IDC_CAPTURESTILL))->EnableWindow(TRUE);
}

void CThresdholdDlg::OnClosecap()
{
    // TODO: Add your control notification handler code here
    // m_voy.SetBehavior(&m_idle);
    // SetTimer(1,100,NULL);
    /*
    if (m_voy.m_OverCap != NULL)
    {
        m_voy.m_OverCap->Stop();
    }*/
    m_voy.CloseOverCap();

    ((CWnd *)GetDlgItem(IDC_CREATCAP))->EnableWindow(TRUE);
    ((CWnd *)GetDlgItem(IDC_CLOSECAP))->EnableWindow(FALSE);
    ((CWnd *)GetDlgItem(IDC_LOADIMG))->EnableWindow(TRUE);
    ((CWnd *)GetDlgItem(IDC_CAPTURESTILL))->EnableWindow(FALSE);
}

void CThresdholdDlg::ShowParam(HSISet inHsi)
{
    CString str;
    m_cList.DeleteAllItems();
    // 先插入的排在显示列表最下面

    m_cList.InsertItem(0, "对手队标");
    str.Format("%d ~ %d", inHsi.opponent.Hue_min, inHsi.opponent.Hue_max);
    m_cList.SetItemText(0, 1, str);
    str.Format("%d ~ %d", inHsi.opponent.Satu_min, inHsi.opponent.Satu_max);
    m_cList.SetItemText(0, 2, str);
    str.Format("%d ~ %d", inHsi.opponent.I_min, inHsi.opponent.I_max);
    m_cList.SetItemText(0, 3, str);

    m_cList.InsertItem(0, "我方色标");
    str.Format("%d ~ %d", inHsi.teammate.Hue_min, inHsi.teammate.Hue_max);
    m_cList.SetItemText(0, 1, str);
    str.Format("%d ~ %d", inHsi.teammate.Satu_min, inHsi.teammate.Satu_max);
    m_cList.SetItemText(0, 2, str);
    str.Format("%d ~ %d", inHsi.teammate.I_min, inHsi.teammate.I_max);
    m_cList.SetItemText(0, 3, str);

    m_cList.InsertItem(0, "球（全景）");
    str.Format("%d ~ %d", inHsi.overball.Hue_min, inHsi.overball.Hue_max);
    m_cList.SetItemText(0, 1, str);
    str.Format("%d ~ %d", inHsi.overball.Satu_min, inHsi.overball.Satu_max);
    m_cList.SetItemText(0, 2, str);
    str.Format("%d ~ %d", inHsi.overball.I_min, inHsi.overball.I_max);
    m_cList.SetItemText(0, 3, str);

    m_cList.InsertItem(0, "白色场线");
    str.Format("%d ~ %d", inHsi.whitelines.Hue_min, inHsi.whitelines.Hue_max);
    m_cList.SetItemText(0, 1, str);
    str.Format("%d ~ %d", inHsi.whitelines.Satu_min, inHsi.whitelines.Satu_max);
    m_cList.SetItemText(0, 2, str);
    str.Format("%d ~ %d", inHsi.whitelines.I_min, inHsi.whitelines.I_max);
    m_cList.SetItemText(0, 3, str);

    m_cList.InsertItem(0, "绿色场地");
    str.Format("%d ~ %d", inHsi.green.Hue_min, inHsi.green.Hue_max);
    m_cList.SetItemText(0, 1, str);
    str.Format("%d ~ %d", inHsi.green.Satu_min, inHsi.green.Satu_max);
    m_cList.SetItemText(0, 2, str);
    str.Format("%d ~ %d", inHsi.green.I_min, inHsi.green.I_max);
    m_cList.SetItemText(0, 3, str);

    m_cList.InsertItem(0, "黑色障碍");
    str.Format("%d ~ %d", inHsi.black.Hue_min, inHsi.black.Hue_max);
    m_cList.SetItemText(0, 1, str);
    str.Format("%d ~ %d", inHsi.black.Satu_min, inHsi.black.Satu_max);
    m_cList.SetItemText(0, 2, str);
    str.Format("%d ~ %d", inHsi.black.I_min, inHsi.black.I_max);
    m_cList.SetItemText(0, 3, str);

    // Color的显示
    UpdateAll(m_cList.lastsel);

    /*m_cList.ResetContent();
    CString str;
    str.Format("蓝色球门： Hmax:%d Hmin:%d Smin:%d",inHsi.blue.Hue_min,inHsi.blue.Hue_max,inHsi.blue.Satu_min);
    m_cList.AddString(str);
    str.Format("黄色球门： Hmax:%d Hmin:%d S:%d",inHsi.yellow.Hue_min,inHsi.yellow.Hue_max,inHsi.yellow.Satu_min);
    m_cList.AddString(str);
    str.Format("球(全景)： Hmax:%d Hmin:%d S:%d",inHsi.overball.Hue_min,inHsi.overball.Hue_max,inHsi.overball.Satu_min);
    m_cList.AddString(str);
    str.Format("对方颜色： Hmax:%d Hmin:%d S:%d",inHsi.oppo.Hue_min,inHsi.oppo.Hue_max,inHsi.oppo.Satu_min);
    m_cList.AddString(str);
    str.Format("绿色地毯： Hmax:%d Hmin:%d S:%d",inHsi.green.Hue_min,inHsi.green.Hue_max,inHsi.green.Satu_min);
    m_cList.AddString(str);
    str.Format("球(前视)： Hmax:%d Hmin:%d
    S:%d",inHsi.frontball.Hue_min,inHsi.frontball.Hue_max,inHsi.frontball.Satu_min); m_cList.AddString(str);
    m_cList.ReSelect();*/
}

void CThresdholdDlg::OnSavehsi()
{
    // TODO: Add your control notification handler code here
    CFileDialog fdlg(FALSE, "th2", "Default.th2", NULL, "相机参数及阈值文件(*.th2)|*.th2", this);
    if (fdlg.DoModal() != IDOK)
    {
        return;
    }

    CString strFn = fdlg.GetPathName();
    m_hsiset.SaveHSIToINI(strFn, &(m_test.m_vhsi), &m_cInfoList);
    m_voy.m_cOverCap.SaveParamToINI(strFn);
}

void CThresdholdDlg::OnLoadhsi()
{
    // TODO: Add your control notification handler code here
    CFileDialog fdlg(TRUE, "th2", "Default.th2", NULL, "相机参数及阈值文件(*.th2)|*.th2", this);
    if (fdlg.DoModal() != IDOK)
    {
        return;
    }

    m_voy.m_cOverCap.LoadParamFromINI(fdlg.GetPathName());
    m_hsiset.LoadHSIFromINI(fdlg.GetPathName(), &(m_test.m_vhsi), &m_cInfoList);
    ShowParam(m_test.m_vhsi);
    m_cColor.Invalidate();
    /*//////////////////////////////////////////////////////////////////////////
    m_hsiset.Load(fdlg.GetFileName(),&(m_test.m_vhsi),&m_cInfoList);
    ShowParam(m_test.m_vhsi);
    m_cColor.Invalidate();

    //camera
    if (m_voy.m_OverCap != NULL)
    {
        //m_voy.m_OverCap->DefaultSet();
        m_voy.m_OverCap->SetCameraProperty(&(m_test.m_vhsi.cameraproc));
// 		m_voy.m_OverCap->DefaultSet();
// 		m_voy.m_OverCap->SetCameraProperty(&(m_test.m_vhsi.cameraproc));
// 		m_voy.m_OverCap->DefaultSet();
// 		m_voy.m_OverCap->SetCameraProperty(&(m_test.m_vhsi.cameraproc));

 //		SetTimer(2,5000,NULL);

//		m_printf("正在摄像头参数，请稍候...");
    }*/
}

void CThresdholdDlg::OnClose()
{
    // TODO: Add your message handler code here and/or call default
    for (int i = 0; i < 5; i++)
    {
        m_test.en[i] = false;
    }
    m_test.bMidPoint = false;
    OnClosecap();
    Sleep(500);
    m_voy.SetBehavior(&m_idle);
    CDialog::OnClose();
}

void CThresdholdDlg::OnPreview()
{
    // TODO: Add your control notification handler code here
    m_test.ClearAllFlag();
    m_test.bBlack = true;
    m_test.bGreen = true;
    m_test.bLines = true;
    m_test.bOverBall = true;
    m_test.bTeammate = true;
    m_test.bOpponent = true;
}

void CThresdholdDlg::OnDspprop()
{
    // TODO: Add your control notification handler code here
    /*if (m_voy.m_OverCap != NULL)
    {
        m_voy.m_OverCap->DisplayPropPage(m_cColor.GetSafeHwnd());
    }*/
    /*CameraProperty cp;
    m_voy.m_OverCap->GetCameraProperty(&cp);
    cp.nExposure = 0;
    m_voy.m_OverCap->SetCameraProperty(&cp);*/
    // m_camdlg.InitBar();
    // m_camdlg.DoModal();
    // m_DHCamDlg.DoModal();
    // m_voy.m_cOverCap.Cameracontrol(this->GetSafeHwnd());
    m_voy.m_cOverCap.ShowPropertyDialog();
}

void CThresdholdDlg::OnOvercenter()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    m_test.bMidPoint = m_bOverCenter;
    m_voy.SetBehavior(&m_test);
}

void CThresdholdDlg::OnFrontang()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    if (m_bFront == TRUE)
    {
        m_cColor.bColor = false;
        m_test.bFrontAng = true;
        m_cColor.Invalidate();
    }
    else
    {
        m_cColor.bColor = true;
        m_test.bFrontAng = false;
        m_cColor.Invalidate();
    }
}

void CThresdholdDlg::OnTimer(UINT nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    // m_cColor.Invalidate();
    // m_cView.Invalidate();

    switch (nIDEvent)
    {
    case 1:
        // 		if (m_voy.m_OverCap != NULL)
        // 		{
        // 			m_voy.m_OverCap->Stop();
        // 		}
        KillTimer(1);
        break;

    case 2:
        // m_voy.m_OverCap->DefaultSet();
        //	m_voy.m_OverCap->SetCameraProperty(&(m_test.m_vhsi.cameraproc));
        m_printf("摄像头设置完成！");
        KillTimer(2);
        break;

    case 3:
        m_voy.ReloadOverlook();
        break;

    case 10086:
        SetWindowText("中型组阈值预览 V3.03   http://www.6-robot.com/");
        m_cVideo.m_fpsDis = m_cVideo.m_nCount;
        m_cVideo.m_nCount = 0;
        break;

    default:;
    }
    CDialog::OnTimer(nIDEvent);
}

void CThresdholdDlg::OnClickItemlist(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    m_voy.ReloadOverlook();

    NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
    if (pNMListView->iItem != -1)
    {
        UpdateAll(pNMListView->iItem);

        m_cList.lastsel = pNMListView->iItem;
    }
    *pResult = 0;
}

BOOL CThresdholdDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: Add your message handler code here and/or call default
    float temp = (float)zDelta / 120;
    m_cView.m_range = m_cView.m_range - (int)temp;

    /*CString str;
    str.Format("r:%d ,%f",m_cView.m_range,temp);
    m_cInfoList.AddString(str);*/

    m_cView.UpdateSelRect(m_cView.m_lastpoint);
    return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CThresdholdDlg::m_printf(CString instr)
{
    m_cInfoList.AddString(instr);
    m_cInfoList.SetCurSel(m_cInfoList.GetCount() - 1);
}

void CThresdholdDlg::UpdateAll(int inIndex)
{
    switch (inIndex)
    {
    case (HSI_TARGET_NUM - 1): // opponent
        m_test.ClearAllFlag();
        m_test.bOpponent = true;
        m_cColor.HSIToShow(m_test.m_vhsi.opponent);
        m_cColor.m_strItem = "对手队标";
        m_cColor.Invalidate();
        m_cColor.pParam = &(m_test.m_vhsi.opponent);
        // m_printf("对手队标 标定中...");
        break;
    case (HSI_TARGET_NUM - 2): // teammate
        m_test.ClearAllFlag();
        m_test.bTeammate = true;
        m_cColor.HSIToShow(m_test.m_vhsi.teammate);
        m_cColor.m_strItem = "队友色标";
        // m_cColor.m_strItem = "None";
        m_cColor.Invalidate();
        m_cColor.pParam = &(m_test.m_vhsi.teammate);
        // m_printf("队友色标 标定中...");
        break;
    case (HSI_TARGET_NUM - 3): // overball
        m_test.ClearAllFlag();
        m_test.bOverBall = true;
        m_cColor.HSIToShow(m_test.m_vhsi.overball);
        m_cColor.m_strItem = "球（全景）";
        m_cColor.Invalidate();
        m_cColor.pParam = &(m_test.m_vhsi.overball);
        // m_printf("球（全景） 标定中...");
        break;

    case (HSI_TARGET_NUM - 4): // lines
        m_test.ClearAllFlag();
        m_test.bLines = true;
        m_cColor.HSIToShow(m_test.m_vhsi.whitelines);
        m_cColor.m_strItem = "白色场线";
        m_cColor.Invalidate();
        m_cColor.pParam = &(m_test.m_vhsi.whitelines);
        // m_printf("白色场线 标定中...");
        break;

    case (HSI_TARGET_NUM - 5): // green
        m_test.ClearAllFlag();
        m_test.bGreen = true;
        m_cColor.HSIToShow(m_test.m_vhsi.green);
        m_cColor.m_strItem = "绿色场地";
        m_cColor.Invalidate();
        m_cColor.pParam = &(m_test.m_vhsi.green);
        // m_printf("绿色场地 标定中...");
        break;

    case (HSI_TARGET_NUM - 6): // black
        m_test.ClearAllFlag();
        m_test.bBlack = true;
        m_cColor.HSIToShow(m_test.m_vhsi.black);
        m_cColor.m_strItem = "黑色障碍";
        m_cColor.Invalidate();
        m_cColor.pParam = &(m_test.m_vhsi.black);
        // m_printf("黑色障碍 标定中...");
        break;
    }
}

BOOL CThresdholdDlg::PreTranslateMessage(MSG *pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    if (pMsg->message == WM_KEYDOWN) // 判断是否为按键消息
    {

        switch (pMsg->wParam) // 根据虚拟键表值来判断按下的是什么键
        {
        case VK_UP: // 按下向上键
            m_cView.m_lastpoint.y -= 2;
            if (m_cView.m_lastpoint.y < m_cView.m_range)
            {
                m_cView.m_lastpoint = m_cView.m_range;
            }
            m_cView.UpdateSelRect(m_cView.m_lastpoint);
            return TRUE;
            break;

        case VK_DOWN: // 按下向下键
            m_cView.m_lastpoint.y += 2;
            if (m_cView.m_lastpoint.y > m_cView.rc.bottom - m_cView.m_range)
            {
                m_cView.m_lastpoint.y = m_cView.rc.bottom - m_cView.m_range;
            }
            m_cView.UpdateSelRect(m_cView.m_lastpoint);
            return TRUE;
            break;

        case VK_LEFT: // 按下向左键
            m_cView.m_lastpoint.x -= 2;
            if (m_cView.m_lastpoint.x < m_cView.m_range)
            {
                m_cView.m_lastpoint.x = m_cView.m_range;
            }
            m_cView.UpdateSelRect(m_cView.m_lastpoint);
            return TRUE;
            break;

        case VK_RIGHT: // 按下向右键
            m_cView.m_lastpoint.x += 2;
            if (m_cView.m_lastpoint.x > m_cView.rc.right - m_cView.m_range)
            {
                m_cView.m_lastpoint.x = m_cView.rc.right - m_cView.m_range;
            }
            m_cView.UpdateSelRect(m_cView.m_lastpoint);
            return TRUE;
            break;

        case VK_SHIFT:
            break;

        case 87:
            /*/AfxMessageBox(_T("w按下"));
            break;*/
        case 83:
            /*/AfxMessageBox(_T("s按下"));
            break;*/
        case 65:
            /*/AfxMessageBox(_T("a按下"));
            break;*/
        case 68:
            /*/AfxMessageBox(_T("d按下"));
            break;*/
        case 81:
            break;

        case 90: // z		缩小
            m_cView.m_range -= 1;
            m_cView.UpdateSelRect(m_cView.m_lastpoint);
            return TRUE;
            break;

        case 88: // x		放大
            m_cView.m_range += 1;
            m_cView.UpdateSelRect(m_cView.m_lastpoint);
            return TRUE;
            break;

        default:
            break;
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}

void CThresdholdDlg::OnCapturestill()
{
    // TODO: Add your control notification handler code here
    m_voy.m_bmploader.nWidth = m_test.nWidth;
    m_voy.m_bmploader.nHeight = m_test.nHeight;
    m_voy.m_bmploader.m_pImgBuf = m_test.bufImg;
    m_voy.m_bmploader.SaveBack();
}

void CThresdholdDlg::OnLoadimg()
{
    // TODO: Add your control notification handler code here
    CFileDialog fdlg(TRUE, "bmp", "", NULL, "BMP位图文件(*.bmp)|*.bmp", this);
    if (fdlg.DoModal() != IDOK)
    {
        return;
    }

    m_voy.EmuOverlook(fdlg.GetFileName(), &m_cVideo);
    EmuAct();
}

void CThresdholdDlg::EmuAct()
{
    SetTimer(3, 30, NULL);
    m_printf("进入图片仿真模式...");
}

void CThresdholdDlg::EmuDis()
{
    KillTimer(3);
    m_printf("图片仿真模式中止！");
}

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    SetTimer(1, 1000, NULL);

    return TRUE; // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnTimer(UINT nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    SetWindowText("毫无意义的“关于”对话框");

    ((CWnd *)GetDlgItem(IDC_INFO))->SetWindowText("飞奔的猪蹄 QQ：770528497");

    CDialog::OnTimer(nIDEvent);
}

void CThresdholdDlg::OnVideoemu()
{
    // TODO: Add your control notification handler code here
    CFileDialog fdlg(TRUE, "wmv", "", NULL, "Windows媒体文件(*.wmv)|*.wmv", this);
    if (fdlg.DoModal() != IDOK)
    {
        return;
    }

    m_voy.EmuOverlookVideo(fdlg.GetFileName(), &m_cVideo);
    // m_voy.EmuOverlook(fdlg.GetFileName(),&m_cVideo);
}

void CThresdholdDlg::OnFramePreview()
{
    // TODO: Add your control notification handler code here
    m_test.nFrameCnt = 0;
    Sleep(200);
    m_dlgFramePreview.DoModal();
}

void CThresdholdDlg::OnAutoCol()
{

    m_voy.m_bmploader.SaveBack(); // 截图

    //	m_printf("自动标色程序启动...");

    char *strTemp;
    strTemp = new char[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, strTemp); // strTemp是主程序路径

    CString path = strTemp, name = "\\Default.th2", exeName = "\\autocolor.exe";
    // char strStr[200] = "\Default.th2";

    CString pathTh2 = path + name; // th2文件的路径

    char *pathEXE = strTemp; // 自动标色脚本的路径 这个脚本会自动获取你的截图然后自动标色把结果写入th2
    char *exename;
    exename = new char[10];
    exename = "\\autocolor.exe";

    char *EXE;
    EXE = new char[100];
    EXE = strcat(pathEXE, exename);

    WinExec(EXE, SW_SHOW);

    m_voy.m_cOverCap.LoadParamFromINI(pathTh2);
    m_hsiset.LoadHSIFromINI(pathTh2, &(m_test.m_vhsi), &m_cInfoList);
    // m_hsiset.LoadHSIFromINI(path,&(m_test.m_vhsi),&m_cInfoList);
    ShowParam(m_test.m_vhsi);
    m_cColor.Invalidate();

    m_printf("自动标色程序启动...");

    delete (EXE);
}

void CThresdholdDlg::On1KeyRead()
{
    char *strTemp;
    strTemp = new char[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, strTemp); // strTemp是主程序路径

    CString path = strTemp, name = "\\Default.th2", exeName = "\\autocolor.exe";
    CString pathTh2 = path + name; // th2文件的路径

    m_voy.m_cOverCap.LoadParamFromINI(pathTh2);
    m_hsiset.LoadHSIFromINI(pathTh2, &(m_test.m_vhsi), &m_cInfoList);
    ShowParam(m_test.m_vhsi);
    m_cColor.Invalidate();

    OnPreview(); // preview all
}

void CThresdholdDlg::On1Keysave()
{
    char *strTemp;
    strTemp = new char[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, strTemp); // strTemp是主程序路径

    CString path = strTemp, name = "\\Default.th2", exeName = "\\autocolor.exe";
    CString pathTh2 = path + name; // th2文件的路径

    m_hsiset.SaveHSIToINI(pathTh2, &(m_test.m_vhsi), &m_cInfoList);
    m_voy.m_cOverCap.SaveParamToINI(pathTh2);
}
