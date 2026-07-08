// SCPlatformDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SCPlatform.h"
#include "SCPlatformDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT g_sliderID[] = {IDC_SLIDER_MOTOR1, IDC_SLIDER_MOTOR2, IDC_SLIDER_MOTOR3,
                            IDC_SLIDER_MOTOR4, IDC_SLIDER_MOTOR5, IDC_SLIDER_MOTOR6};
static UINT g_editDis[] = {IDC_DISPALY_MOTOR1, IDC_DISPALY_MOTOR2, IDC_DISPALY_MOTOR3,
                           IDC_DISPALY_MOTOR4, IDC_DISPALY_MOTOR5, IDC_DISPALY_MOTOR6};
static CString strCFGFile = "./config.ini";

static bool bMotorIncrease[6];

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
// No message handlers
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCPlatformDlg dialog

CSCPlatformDlg::CSCPlatformDlg(CWnd *pParent /*=NULL*/) : CDialog(CSCPlatformDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSCPlatformDlg)
    m_nRobotPort = 1;
    m_nSendLoopTime = 100;
    m_nInpManMotor1 = 0;
    m_nInpManMotor2 = 0;
    m_nInpManMotor3 = 0;
    m_nInpManMotor4 = 0;
    m_nInpManMotor5 = 0;
    m_nInpManMotor6 = 0;
    m_nKickPower = 200;
    m_nRobotRate = 115200;
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    for (int i = 0; i < 6; i++)
    {
        m_nMotorVal[i] = 0;
        bMotorIncrease[i] = true;
    }
    m_nRobotPort = (int)GetPrivateProfileInt("PORT", "ROBOT", 1, strCFGFile);
}

void CSCPlatformDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSCPlatformDlg)
    DDX_Control(pDX, IDC_DIS_IO, m_cDisIO);
    DDX_Control(pDX, IDC_DIS_AD, m_cDisAD);
    DDX_Control(pDX, IDC_DIS_MOTOR, m_cDisMotor);
    DDX_Control(pDX, IDC_RECVLIST, m_cRecvList);
    DDX_Control(pDX, IDC_INFOLIST, m_cInfoList);
    DDX_Control(pDX, IDC_SENDLIST, m_cSendList);
    DDX_Text(pDX, IDC_INP_ROBOTPORT, m_nRobotPort);
    DDX_Text(pDX, IDC_SENDLOOP_TIME, m_nSendLoopTime);
    DDX_Text(pDX, IDC_MAN_MOTOR_1, m_nInpManMotor1);
    DDX_Text(pDX, IDC_MAN_MOTOR_2, m_nInpManMotor2);
    DDX_Text(pDX, IDC_MAN_MOTOR_3, m_nInpManMotor3);
    DDX_Text(pDX, IDC_MAN_MOTOR_4, m_nInpManMotor4);
    DDX_Text(pDX, IDC_MAN_MOTOR_5, m_nInpManMotor5);
    DDX_Text(pDX, IDC_MAN_MOTOR_6, m_nInpManMotor6);
    DDX_Text(pDX, IDC_INP_KICK_POWER, m_nKickPower);
    DDX_Text(pDX, IDC_INP_ROBOTBAUD, m_nRobotRate);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSCPlatformDlg, CDialog)
//{{AFX_MSG_MAP(CSCPlatformDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_WM_HSCROLL()
ON_BN_CLICKED(IDC_BTN_OPENROBOT_PORT, OnBtnOpenrobotPort)
ON_WM_TIMER()
ON_BN_CLICKED(IDC_BTN_AUTOSTART, OnBtnAutostart)
ON_BN_CLICKED(IDC_BTN_AUTOSTOP, OnBtnAutostop)
ON_BN_CLICKED(IDC_BTN_BRAKEALL, OnBtnBrakeall)
ON_BN_CLICKED(IDC_BTN_MANU_SEND, OnBtnManuSend)
ON_BN_CLICKED(IDC_BTN_CLOSEROBOT_PORT, OnBtnCloserobotPort)
ON_BN_CLICKED(IDC_BTN_KICK, OnBtnKick)
ON_BN_CLICKED(IDC_BTN_TEST_START, OnBtnTestStart)
ON_BN_CLICKED(IDC_BTN_TEST_STOP, OnBtnTestStop)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCPlatformDlg message handlers

BOOL CSCPlatformDlg::OnInitDialog()
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
    for (int i = 0; i < 5; i++)
    {
        CSliderCtrl *pSliderCtrl = (CSliderCtrl *)GetDlgItem(g_sliderID[i]);
        // pSliderCtrl->SetRange(-32767,32767,TRUE);
        pSliderCtrl->SetRange(-7500, 7500, TRUE);
        pSliderCtrl->SetPos(0);

        CEdit *pEdit = (CEdit *)GetDlgItem(g_editDis[i]);
        pEdit->SetWindowText("0");
    }

    // 电机6 位置
    CSliderCtrl *pSliderCtrl = (CSliderCtrl *)GetDlgItem(g_sliderID[5]);
    pSliderCtrl->SetRange(0, 21000, TRUE);
    // pSliderCtrl->SetRange(-32767,32767,TRUE);
    pSliderCtrl->SetPos(0);

    CEdit *pEdit = (CEdit *)GetDlgItem(g_editDis[5]);
    pEdit->SetWindowText("0");

    m_cmd.pSendList = &m_cSendList;
    m_cmd.CmdShow_Rec = &m_cRecvList;

    m_cmd.pMotorDis = &m_cDisMotor;
    m_cmd.pADDis = &m_cDisAD;
    m_cmd.pIODis = &m_cDisIO;

    return TRUE; // return TRUE  unless you set the focus to a control
}

void CSCPlatformDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSCPlatformDlg::OnPaint()
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
HCURSOR CSCPlatformDlg::OnQueryDragIcon()
{
    return (HCURSOR)m_hIcon;
}

void CSCPlatformDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
    // TODO: Add your message handler code here and/or call default
    static CString strSPDSLD;
    UpdateData();
    int nSpeed = ((CSliderCtrl *)pScrollBar)->GetPos();
    strSPDSLD.Format("%d", nSpeed);

    UINT nSliderID = pScrollBar->GetDlgCtrlID();
    for (int i = 0; i < 6; i++)
    {
        if (nSliderID == g_sliderID[i])
        {
            ((CEdit *)GetDlgItem(g_editDis[i]))->SetWindowText(strSPDSLD);
            m_nMotorVal[i] = nSpeed;
        }
    }
    SendCmd();

    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSCPlatformDlg::OnBtnOpenrobotPort()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    BOOL res = m_cmd.Create(m_nRobotPort, m_nRobotRate);
    if (res == TRUE)
    {
        CString strValue;
        strValue.Format("%d", m_nRobotPort);
        WritePrivateProfileString("PORT", "ROBOT", strValue, strCFGFile);

        m_cInfoList.AddString("成功打开机器人端口！");
    }
    else
    {
        m_cInfoList.AddString("机器人端口打开失败...");
    }
    m_cInfoList.SetCurSel(m_cInfoList.GetCount() - 1);
}

void CSCPlatformDlg::SendCmd()
{
    m_cmd.SetSixMotorsSpeed(m_nMotorVal);

    ///////////////////////////////////////////////////
    // 	CString strData,strTmp;
    // 	for (int i=0;i<6;i++)
    // 	{
    // 		strTmp.Format("【%d】= %d ;",i+1,m_nMotorVal[i]);
    // 		strData += strTmp;
    // 	}
    // 	m_cSendList.AddString(strData);
    // 	m_cSendList.SetCurSel(m_cSendList.GetCount()-1);
    //////////////////////////////////////////////////
}

void CSCPlatformDlg::OnTimer(UINT nIDEvent)
{
    // TODO: Add your message handler code here and/or call default

    switch (nIDEvent)
    {
    case TMR_AUTOSEND:
        SendCmd();
        break;

    case TMR_PRESS_TEST:
        m_PressTestStep();
        break;
    }

    CDialog::OnTimer(nIDEvent);
}

void CSCPlatformDlg::OnBtnAutostart()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    SetTimer(TMR_AUTOSEND, m_nSendLoopTime, NULL);
}

void CSCPlatformDlg::OnBtnAutostop()
{
    // TODO: Add your control notification handler code here
    KillTimer(TMR_AUTOSEND);
}

void CSCPlatformDlg::OnBtnBrakeall()
{
    // TODO: Add your control notification handler code here
    for (int i = 0; i < 5; i++)
    {
        CSliderCtrl *pSliderCtrl = (CSliderCtrl *)GetDlgItem(g_sliderID[i]);
        pSliderCtrl->SetPos(0);

        CEdit *pEdit = (CEdit *)GetDlgItem(g_editDis[i]);
        pEdit->SetWindowText("0");

        m_nMotorVal[i] = 0;
    }
    SendCmd();
}

void CSCPlatformDlg::OnBtnManuSend()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    m_nMotorVal[0] = m_nInpManMotor1;
    m_nMotorVal[1] = m_nInpManMotor2;
    m_nMotorVal[2] = m_nInpManMotor3;
    m_nMotorVal[3] = m_nInpManMotor4;
    m_nMotorVal[4] = m_nInpManMotor5;
    m_nMotorVal[5] = m_nInpManMotor6;
    SendCmd();

    CString strTmp;
    for (int i = 0; i < 6; i++)
    {
        CSliderCtrl *pSliderCtrl = (CSliderCtrl *)GetDlgItem(g_sliderID[i]);
        pSliderCtrl->SetPos(m_nMotorVal[i]);

        strTmp.Format("%d", m_nMotorVal[i]);
        CEdit *pEdit = (CEdit *)GetDlgItem(g_editDis[i]);
        pEdit->SetWindowText(strTmp);
    }
}

void CSCPlatformDlg::OnBtnCloserobotPort()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    m_cmd.ClosePort();
    m_cInfoList.AddString("机器人端口关闭！");
    m_cInfoList.SetCurSel(m_cInfoList.GetCount() - 1);
}

void CSCPlatformDlg::OnBtnKick()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    m_cmd.Kick(m_nKickPower);
}

void CSCPlatformDlg::OnBtnTestStart()
{
    // TODO: Add your control notification handler code here
    // clang-format off
    int nRes = MessageBox(_T("【警告】压力测试中底盘轮子将进行高速转动！确定前请保证如下条件：\n1、底盘已经被垫起，轮子不与地面接触\n2、射门杆可自由活动\r\n确定进行压力测试吗？"),
                          _T("【！】测试有危险，请谨慎！"), MB_OKCANCEL | MB_ICONQUESTION);
    // clang-format on
    // 判断消息对话框返回值。如果为IDCANCEL就return，否则继续向下执行
    if (IDCANCEL == nRes)
    {
        // 		m_cInfoList.AddString("放弃压力测试");
        // 		m_cInfoList.SetCurSel(m_cInfoList.GetCount()-1);
        return;
    }
    m_cInfoList.AddString("压力测试开始...");
    m_cInfoList.SetCurSel(m_cInfoList.GetCount() - 1);

    SetTimer(TMR_PRESS_TEST, 100, NULL);
}

static int test_RPMRange = 7000;
static int test_RPMStep = 100;
void CSCPlatformDlg::DealWithMotor(int inIndex)
{
    if (bMotorIncrease[inIndex] == true)
    {
        m_nMotorVal[inIndex] += test_RPMStep;
    }
    else
    {
        m_nMotorVal[inIndex] -= test_RPMStep;
    }

    if (m_nMotorVal[inIndex] > test_RPMRange)
    {
        m_nMotorVal[inIndex] = test_RPMRange;
        bMotorIncrease[inIndex] = false;
    }

    if (m_nMotorVal[inIndex] < -test_RPMRange)
    {
        m_nMotorVal[inIndex] = -test_RPMRange;
        bMotorIncrease[inIndex] = true;
    }
}

static int test_PosRange = 20000;
static int test_PosStep = 1000;
void CSCPlatformDlg::DealWithPos(int inIndex)
{
    if (bMotorIncrease[inIndex] == true)
    {
        m_nMotorVal[inIndex] += test_PosStep;
    }
    else
    {
        m_nMotorVal[inIndex] -= test_PosStep;
    }

    if (m_nMotorVal[inIndex] > test_PosRange)
    {
        m_nMotorVal[inIndex] = test_PosRange;
        bMotorIncrease[inIndex] = false;
    }

    if (m_nMotorVal[inIndex] < 0)
    {
        m_nMotorVal[inIndex] = 0;
        bMotorIncrease[inIndex] = true;
    }
}

void CSCPlatformDlg::m_PressTestStep()
{
    for (int i = 0; i < 5; i++)
    {
        DealWithMotor(i);
    }
    DealWithPos(5);

    CString strTmp;
    for (i = 0; i < 6; i++)
    {
        CSliderCtrl *pSliderCtrl = (CSliderCtrl *)GetDlgItem(g_sliderID[i]);
        pSliderCtrl->SetPos(m_nMotorVal[i]);

        strTmp.Format("%d", m_nMotorVal[i]);
        CEdit *pEdit = (CEdit *)GetDlgItem(g_editDis[i]);
        pEdit->SetWindowText(strTmp);
    }
}

void CSCPlatformDlg::OnBtnTestStop()
{
    // TODO: Add your control notification handler code here
    KillTimer(TMR_PRESS_TEST);
    Sleep(500);
    OnBtnBrakeall();
}
