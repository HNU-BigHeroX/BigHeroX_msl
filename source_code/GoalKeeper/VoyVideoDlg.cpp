// VoyVideoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VoyVideo.h"
#include "VoyVideoDlg.h"
#include <stdio.h>
#include <stdlib.h>
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
// 直角坐标到极坐标转换函数 角度

static CString strCFGFile = ".\\Config\\RobotCfg.ini";

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
// CVoyVideoDlg dialog

CVoyVideoDlg::CVoyVideoDlg(CWnd *pParent /*=NULL*/) : CDialog(CVoyVideoDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CVoyVideoDlg)
    m_nPort = 1;
    m_strCoachIP = _T(" ");
    m_nAgentID = 0;
    m_nHoldBall = 55;
    m_setword = _T("");
    m_nSubSpeed = 100;
    m_nSubSpeedR = -100;
    m_nKickStrength = 100;
    m_nCamOffsetX = 0;
    m_nCamOffsetY = 0;
    m_nLiftPos = 0;
    m_nMpuPort = 9;
    m_nGKSaveVal = 0;
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    zhuand = 0;
    jiao = 0;
    chang = 0;
    motol = 0;
    motor = 0;
    motob = 0;
    mos.x = 413;
    mos.y = 502;
    //	cdbl=2.4;
    // cfg
    m_cfg.AgentID = 0;
    m_cfg.Port = 4;
    m_cfg.nOverlookIndex = 0;
    m_cfg.nAheadIndex = 1;

    m_dlgView = NULL;
    m_dlgMInfo = NULL;
    m_pdlgDebug = NULL;
    m_pdlgDView = NULL;

    CString strValue;
    // AgentID
    m_nAgentID = (int)GetPrivateProfileInt("AGENT", "AGENT_ID", 1, strCFGFile);

    // RobotPort
    m_nPort = (int)GetPrivateProfileInt("PORT", "ROBOT", 1, strCFGFile);

    // MPU_Port
    m_nMpuPort = (int)GetPrivateProfileInt("PORT", "MPU", 9, strCFGFile);

    // IP
    char buffer[255];
    GetPrivateProfileString("AGENT", "MON_IP", "", buffer, 255, strCFGFile);
    m_strCoachIP.Format("%s", buffer);
    //	WritePrivateProfileString("AGENT", "MON_IP", m_strCoachIP, strCFGFile);

    // m_nHoldBall
    m_nHoldBall = (int)GetPrivateProfileInt("AGENT", "HOLDBALL", 0, strCFGFile);
}

void CVoyVideoDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CVoyVideoDlg)
    DDX_Control(pDX, IDC_GK_RC_DIS, m_cGKRecvDis);
    DDX_Control(pDX, IDC_OMNIMOVE, m_cOmniMove);
    DDX_Control(pDX, IDC_OVERLOOKCOMBO, m_cOverlookCombo);
    DDX_Control(pDX, IDC_STATUS, m_cStatusList);
    DDX_Control(pDX, IDC_FIELD, m_cField);
    DDX_Control(pDX, IDC_POS, m_cPosShow);
    DDX_Text(pDX, IDC_PORT, m_nPort);
    DDX_Text(pDX, IDC_COACHIP, m_strCoachIP);
    DDX_Text(pDX, IDC_AGENTID, m_nAgentID);
    DDX_Text(pDX, IDC_HOLDBALL, m_nHoldBall);
    DDX_Text(pDX, IDC_SET, m_setword);
    DDX_Text(pDX, IDC_SUBSPEED, m_nSubSpeed);
    DDX_Text(pDX, IDC_SUBSPEED2, m_nSubSpeedR);
    DDX_Text(pDX, IDC_KICK_STRENGTH, m_nKickStrength);
    DDX_Text(pDX, IDC_CAM_OFFSET_X, m_nCamOffsetX);
    DDX_Text(pDX, IDC_CAM_OFFSET_Y, m_nCamOffsetY);
    DDX_Text(pDX, IDC_DIS_LIFTPOS, m_nLiftPos);
    DDX_Text(pDX, IDC_MPU_PORT, m_nMpuPort);
    DDX_Text(pDX, IDC_GK_VAL, m_nGKSaveVal);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVoyVideoDlg, CDialog)
//{{AFX_MSG_MAP(CVoyVideoDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_CAPSTART, OnCapstart)
ON_BN_CLICKED(IDC_STOPCAP, OnStopcap)
ON_BN_CLICKED(IDC_REMOVE, OnRemove)
ON_BN_CLICKED(IDC_HSI, OnHsi)
ON_BN_CLICKED(IDC_Yuzhi_1, OnYuzhi1)
ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
ON_WM_KEYDOWN()
ON_WM_KEYUP()
ON_BN_CLICKED(IDC_BUTTON6, OnButton6)
ON_BN_CLICKED(IDC_CNTCOACH, OnCntcoach)
ON_BN_CLICKED(IDC_CHANGEID, OnChangeid)
ON_WM_TIMER()
ON_BN_CLICKED(IDC_AUTOSTART, OnAutostart)
ON_BN_CLICKED(IDC_PARAM, OnParam)
ON_BN_CLICKED(IDC_UPDATEPARAM, OnUpdateparam)
ON_WM_CLOSE()
ON_WM_RBUTTONDOWN()
ON_WM_RBUTTONUP()
ON_MESSAGE(WM_VIEWCLOSED, OnViewClosing)
ON_MESSAGE(WM_VIEWMOVEING, OnViewMoving)
ON_MESSAGE(WM_VIEWKEYDOWN, OnViewKeyDown)
ON_BN_CLICKED(IDC_SUCBALL, OnSucball)
ON_BN_CLICKED(IDC_INFODIS, OnInfodis)
ON_MESSAGE(WM_MINFOCLOSED, OnMInfoClosing)
ON_BN_CLICKED(IDC_NOWATCHDOG, OnNowatchdog)
ON_BN_CLICKED(IDC_SHOWVIDEO, OnShowvideo)
ON_BN_CLICKED(IDC_DEBUG_INFO, OnDebugInfo)
ON_BN_CLICKED(IDC_SHOWDBGVIEW, OnShowdbgview)
ON_BN_CLICKED(IDC_NETSEND, OnNetsend)
ON_BN_CLICKED(IDC_AMC_EN, OnAmcEn)
ON_BN_CLICKED(IDC_AMC_BRAKE, OnAmcBrake)
ON_BN_CLICKED(IDC_AMC_DISEN, OnAmcDisen)
ON_BN_CLICKED(IDC_CAM_SET_OFFSET, OnCamSetOffset)
ON_BN_CLICKED(IDC_SET_LIFTPOS, OnSetLiftpos)
ON_BN_CLICKED(IDC_OPEN_MPU_PORT, OnOpenMpuPort)
ON_BN_CLICKED(IDC_FIELD, OnField)
ON_BN_CLICKED(IDC_GK_SAVE, OnGkSave)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVoyVideoDlg message handlers

BOOL CVoyVideoDlg::OnInitDialog()
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
    m_cOmniMove.pRobot = &m_robot;
    //	m_vdemo.m_pShow = &m_cPosShow;
    m_find.m_pShow = &m_cPosShow;

    m_find.m_find.pField = &m_cField;
    m_find.m_find.pList = &m_cStatusList;
    m_find.pRobot = &m_robot;

    m_mpu.pCmd = &m_robot;

// 	CFileFind finder;
// 	if(finder.FindFile(".\\Config\\default.cfg") == TRUE)
// 	{
// 		LoadCfg();
// 	}
// 	else
// 	{
// 		m_cOverlookCombo.SetCurSel(0);
// 	}
#ifdef RUN_WITH_EMU
    m_nAgentID_g_set = m_nAgentID;
#endif

    // test
    // m_cmd.CmdShow_Rec = &m_cStatusList;
    m_find.net.m_pShowList = &m_cStatusList;
    for (int i = 0; i < 5; i++)
    {
        speed[i] = 0;
    }
    //	m_cField.m_curID = 2;
    m_dlgView = NULL;
    m_dlgMInfo = NULL;

    m_cField.m_pNet = &(m_find.net);
    EmuCamera();

#ifdef RUN_WITH_EMU // 如果使用仿真环境就 将程序设置自启动
    OnAutostart();
#endif

    // 组播
    m_find.m_zubo.plist = &m_cStatusList;
    m_find.m_zubo.JoinGroup("224.16.32.67", 20000, 1, 0);

    // 接收kinect数据
    m_find.m_GKRecv.pDis = &m_cGKRecvDis;
    m_find.m_GKRecv.m_pShowList = &m_cStatusList;
    m_find.m_GKRecv.Listen(201501);

    return TRUE; // return TRUE  unless you set the focus to a control
}

void CVoyVideoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CVoyVideoDlg::OnPaint()
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
HCURSOR CVoyVideoDlg::OnQueryDragIcon()
{
    return (HCURSOR)m_hIcon;
}

#ifdef RUN_WITH_EMU
unsigned char ImgBuf[640 * 480 * 3];
UINT CVoyVideoDlg::EmuOverlook(LPVOID lParam)
{
    CVoyVideoDlg *Cam = (CVoyVideoDlg *)lParam;
    Sleep(1000);
    int i;
    while (1)
    {
        i = 0;
        while (i < 921600)
        {
            ImgBuf[i] = 10;
            i++;
        }
#ifndef RUN_WITH_LOCAL_EMU
        Cam->m_find.AfterUpdateOverlook(ImgBuf, 640, 480, 1, 0);
#endif
#ifdef RUN_WITH_LOCAL_EMU
        Cam->m_find.AfterUpdateOverlookEMU(ImgBuf, 640, 480, 1, 0);
#endif
        Sleep(15);
    }
    return (1);
}
#endif

UINT CVoyVideoDlg::JoyReadThread(LPVOID lParam)
{
    // 	CVoyVideoDlg* Cam = (CVoyVideoDlg*)lParam;
    // 	int i;
    // 	JOYINFO joy;
    // 	while (1)
    // 	{
    // 		joyGetPos(0,&joy);
    // 		JOY_Xpos	=32767;
    // 		JOY_Ypos	=32767;
    // 		JOY_Zpos	=32767;
    // 		JOY_Buttons =0;
    // 	if ((joy.wXpos>=0)&&(joy.wXpos<=65537))
    // 		JOY_Xpos=joy.wXpos;
    // 	if ((joy.wYpos>=0)&&(joy.wYpos<=65537))
    // 		JOY_Ypos=joy.wYpos;
    // 	if ((joy.wZpos>=0)&&(joy.wZpos<=65537))
    // 		JOY_Zpos=joy.wZpos;
    // 	JOY_XposF=-(float)(32767-JOY_Xpos)/32767.0;
    // 	JOY_YposF=(float)(32767-JOY_Ypos)/32767.0;
    // 	JOY_ZposF=(float)(32767-JOY_Zpos)/32767.0;
    //
    // 	if ((joy.wButtons>=0)&&(joy.wButtons<=65537))
    // 	{	JOY_Buttons=joy.wButtons;
    // 		for (i=0;i<16;i++)
    // 			if (joy.wButtons&(0x01<<i))
    // 				JOY_Button[i]=1;
    // 			else
    // 				JOY_Button[i]=0;
    // 	}
    //
    // 		Sleep(15);
    // 	}
    return (1);
}

void CVoyVideoDlg::OnCapstart()
{
    // TODO: Add your control notification handler code here

#ifndef RUN_WITH_EMU
    HRESULT result = m_Camera.Open(FALSE);
    if (result == E_FAIL)
    {
        m_printf("全景视频打开失败！");
        return;
    }
    else
    {
        m_printf("全景视频开启...");
        (CWnd *)GetDlgItem(IDC_CAPSTART)->EnableWindow(FALSE);
    }
#endif
#ifdef RUN_WITH_EMU
    AfxBeginThread(EmuOverlook, this); //		AfxBeginThread( threadProcess, this ,THREAD_PRIORITY_TIME_CRITICAL);
#endif
#ifdef RUN_WITH_JOY
    AfxBeginThread(JoyReadThread, this); //		AfxBeginThread( threadProcess, this ,THREAD_PRIORITY_TIME_CRITICAL);
#endif

    m_printf("正在进行数据的初始化...");
    // m_printf("稍等，先不要点击按钮！");
    m_DisableAll();
    // 场线位图查找表生成
    CDC dcMem;
    CPaintDC dc(this);
    dcMem.CreateCompatibleDC(&dc);
    CBitmap bm;
    bm.LoadBitmap(IDB_BITMAP2);
    dcMem.SelectObject(&bm);

    BOOL baidianx[50000];
    BOOL baidiany[50000];
    int dianshu111;
    //******************
    dc.BitBlt(0, 0, 313, 220, &dcMem, 0, 0, SRCCOPY); // 将图像复制到dcmen
    BOOL weitu1[320][240];
    for (int i = 0; i < 320; i++)
        for (int j = 0; j < 240; j++)
            weitu1[i][j] = 0;
    for (i = 0; i < 324; i++)
        for (int j = 0; j < 222; j++)
        {
            //	weitu1[i/2][j/2]=0;
            if (dcMem.GetPixel(i, j) != 0)
            {
                weitu1[i][j] = 1; // 将dcmen复制到weitu[][]这个数组中
            }
        }
    for (i = 0; i <= 313; i++)
        for (int j = 0; j <= 220; j++)
        {
            weitu[i + 158][j + 130] = weitu1[i][j]; // 将这个weitu[][]平移到图像中间
        }
    dianshu111 = 0;
    for (i = 158; i <= 471; i++)
        for (int j = 130; j <= 350; j++)
        {
            if (weitu[i][j] == 1)
            {
                baidianx[dianshu111] = i;
                baidiany[dianshu111] = j;
                dianshu111++;
            }
        }
    // dianshu=dianshu;

    m_mapfile.Load(".\\Config\\fieldmap", (int *)shendubiao, 640 * 480 * 4);
    // 深度图生成完毕
    // 场线位图查找表生成完毕
    m_printf("数据初始化完毕...");
    m_EnableAll();
    // m_pCap->SetCmd(&m_cmd);			//视频类对象和指令类对象连接
    SetTimer(4, 40, NULL); // 刷新地图
}

void CVoyVideoDlg::OnStopcap()
{
    // TODO: Add your control notification handler code here
    m_Camera.Close();
}

void CVoyVideoDlg::OnRemove()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    m_robot.Kick(m_nKickStrength);
}

void CVoyVideoDlg::OnHsi()
{
#ifndef RUN_WITH_EMU
    CreateView();
#endif
    // 	int zhi11;
    // 	CButton* xx1 = (CButton*)GetDlgItem(IDC_HUANG);
    // 	if (xx1->GetCheck())
    // 	{
    // 		zhi11=1;
    // 	}
    // 	else
    // 	{
    // 		zhi11=0;
    //
    // 	}
    //
    // 	m_find.zhuanhuan(zhi11);
    //////////////////////////////////////////////////////////////////////////
    CFileFind finder;
    if (finder.FindFile(".\\Config\\Default.th2") == TRUE)
    {
        m_find.wenjian.LoadHSIFromINI(".\\Config\\Default.th2", &(m_find.m_hsiset), &m_cStatusList);
        m_Camera.LoadParamFromINI(".\\Config\\Default.th2");
    }
    else
    {
        CFileDialog fdlg(TRUE, "th2", "Default.th2", NULL, "UP颜色阈值文件(*.th2)|*.th2", NULL);
        if (fdlg.DoModal() != IDOK)
        {
            return;
        }

        m_find.wenjian.LoadHSIFromINI(fdlg.GetPathName(), &(m_find.m_hsiset), &m_cStatusList);
        m_Camera.LoadParamFromINI("Config\\Default.th2");
    }
    // 	if(finder.FindFile("Default.hsi") == TRUE)
    // 	{
    // 		m_find.wenjian.Load("Default.hsi",&(m_find.m_hsiset),&m_cStatusList);
    // 	}
    // 	else
    // 	{
    // 		CFileDialog fdlg(TRUE,"hsi","Default.hsi",NULL,"UP颜色阈值文件(*.hsi)|*.hsi",NULL);
    // 		if (fdlg.DoModal() != IDOK)
    // 		{
    // 			return;
    // 		}
    //
    // 		m_find.wenjian.Load(fdlg.GetFileName(),&(m_find.m_hsiset),&m_cStatusList);
    // 	}
    //////////////////////////////////////////////////////////////////////////
    // TODO: Add your control notification handler code here
    // 传递底盘返回信息的数组指针
    m_find.m_pMPU6050Data = m_robot.Arduino_MPU6050Data;
    m_find.m_pMotorPos = m_robot.nMotorPos;
    m_find.m_pMotorCurrent = m_robot.wMotorCurrent;
    m_find.m_pIO = m_robot.m_valIO;
    m_find.m_pAD = m_robot.m_valAD;

    // m_cmd.SetBehavior(&m_find);		//加载行为

    // m_Camera.SetCmd(&m_cmd);
    m_Camera.pGetImage = &m_find;

    bianliang1 = 1;

    /*/camera
    if (m_pCap != NULL)
        {
            m_pCap->SetCameraProperty(&(m_find.m_hsiset.cameraproc));
        }
    if (m_pCap2 != NULL)
        {
            m_pCap2->SetCameraProperty(&(m_find.m_hsiset.cameraproc));
        }
    SetTimer(2,5000,NULL);*/
}

void CVoyVideoDlg::OnYuzhi1()
{
    // TODO: Add your control notification handler code here
    //	Yuzhi *y=new Yuzhi;
    //	m_y.GetDefault(y);
    //	CString zhi;
    //	zhi.Format("H=%d,S=%d",y->H,y->S );
}
char m_Com_2_ok_flag = 0;
void CVoyVideoDlg::OnButton1()
{
    // TODO: Add your control notification handler code here
    UpdateData();

#ifndef RUN_WITH_EMU
#ifdef USE_GYRO_MPU6050
/*	m_Com_2.SetCmd(&m_cmd);
    if (m_Com_2_ok_flag==0)
    {
    //	for(int retry1=0;retry1<5;retry1++)
    //	{
    //			BOOL res2 = m_Com_2.InitPort(NULL,m_nPort/10,115200);
    //			Sleep(100);
    //			m_Com_2.ClosePort();
    //	}
        for(int retry=0;retry<6;retry++)
        {	BOOL res2 = m_Com_2.InitPort(NULL,m_nPort/10,115200);
            if (res2 == TRUE)
            {
                m_Com_2.StartMonitoring();
                Sleep(3000);
                char sendchar='A';
            //		Sleep(500);
                m_Com_2.Send(&sendchar, 1);//WriteToPort("A");
                if (m_Com_2.last_received_char!=0x0A)
                {		Sleep(100);
                if (m_Com_2.last_received_char!=0x0A)
                {		Sleep(100);
                if (m_Com_2.last_received_char!=0x0A)
                {		Sleep(100);
                if (m_Com_2.last_received_char!=0x0A)
                {		Sleep(100);
                if (m_Com_2.last_received_char!=0x0A)
                {		Sleep(100);
                    m_printf("串口2打开成功...mpu6050 OK");
                    m_Com_2_ok_flag=1;
                    break;
                }}}}}
                m_Com_2.ClosePort();
                m_printf("串口2打开重试");
            }
            else
            {
                CString strInfo;
                strInfo.Format("串口2打开失败！com %d",m_nPort/10);
                AfxMessageBox(strInfo);
            }
        }
    }
*/
#endif
#endif

    // m_Com.SetCmd(&m_cmd);
    BOOL res = m_robot.Create(m_nPort % 10);

    if (res == TRUE)
    {
        (CWnd *)GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
        m_cStatusList.AddString("成功打开机器人端口！");
        Sleep(100);
        // 设定发送周期
        m_robot.InitSensor();
    }
    else
    {
        m_cStatusList.AddString("机器人端口打开失败...");
#ifndef RUN_WITH_EMU
        CString strInfo;
        strInfo.Format("串口1打开失败！com%d", m_nPort % 10);
        AfxMessageBox(strInfo);
#endif
    }
    m_cStatusList.SetCurSel(m_cStatusList.GetCount() - 1);

    //	CString str;
    //	str.Format("串口号%d",m_nPort);
}

void CVoyVideoDlg::OnButton2()
{
    // TODO: Add your control notification handler code here
    m_cOmniMove.speed[0] = -100;
    m_cOmniMove.speed[1] = -100;
    m_cOmniMove.speed[2] = -100;
    m_robot.SetFiveMotorsSpeed(m_cOmniMove.speed);
}

void CVoyVideoDlg::OnButton4()
{
    // TODO: Add your control notification handler code here

    m_cOmniMove.speed[0] = 100;
    m_cOmniMove.speed[1] = 100;
    m_cOmniMove.speed[2] = 100;
    m_robot.SetFiveMotorsSpeed(m_cOmniMove.speed);
}

void CVoyVideoDlg::Controlmoto(int jiao, int sudu, int zhuan, int *motol, int *motor, int *motob)
{
    *motol = -int(sin((float(jiajiao(jiao, -60))) * 3.14 / 180) * sudu * sudu / 10) + zhuan;
    *motor = -int(sin((float(jiajiao(jiao, 60))) * 3.14 / 180) * sudu * sudu / 10) + zhuan;
    *motob = -int(sin((float(jiajiao(jiao, +180))) * 3.14 / 180) * sudu * sudu / 10) + zhuan;
}

int CVoyVideoDlg::jiajiao(int jizhun, int jiao)
{
    int jiajiao;
    jiajiao = jiao - jizhun;
    if (jiajiao > 180)
        jiajiao = -360 + jiajiao;
    if (jiajiao < -180)
        jiajiao = 360 + jiajiao;
    return jiajiao;
}

void CVoyVideoDlg::jizuobiao1(int x, int y, double *jiao, double *chang)
{
    x = x - 413;
    y = 502 - y;
    if (x != 0 || y != 0)
    {
        *chang = sqrt(x * x + y * y);
        if (x > 0)
            *jiao = (100 - asin(y / *chang) * 63.66) * 0.9;
        else
            *jiao = -(100 - asin(y / *chang) * 63.66) * 0.9;
    }
    else
    {
        *chang = 0;
        *jiao = 0;
    }
}

void CVoyVideoDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    CDialog::OnLButtonDown(nFlags, point);

    if (point.x > 522 && point.x < 1010 && point.y > 18 && point.y < 358)
    {
        dianjipanding = 10;
        button1 = 1;
        dianjix = (point.x - 550) * 4.5112781954887218045112781954887;
        dianjiy = (-point.y + 321) * 4.5454545454545454545454545454545;
        //	CString str;
        // str.Format("%d,%d",point.x,point.y);
        //	AfxMessageBox(str);
        //		m_cField->SetAgent(1,0,0,0);
    }
}

void CVoyVideoDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    CDialog::OnLButtonUp(nFlags, point);
    button = 0;
    button1 = 0;
}

void CVoyVideoDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    double jiao;
    double chang;
    CDialog::OnMouseMove(nFlags, point);

    if (button1 == 1)
    {
        if (point.x > 522 && point.x < 1010 && point.y > 18 && point.y < 358)
        {
            dianjipanding = 10;
            dianjix = (point.x - 569) * 4.5112781954887218045112781954887;
            dianjiy = (-point.y + 321) * 4.5454545454545454545454545454545;
        }
    }
    if (button1 == 3)
        if (point.x > 522 && point.x < 1010 && point.y > 0 && point.y < 370)
        {
            m_find.m_find.jizuobiao((point.x - 569) * 4.5112781954887218045112781954887,
                                    (-point.y + 321) * 4.5454545454545454545454545454545, &jiao, &chang, dianjix,
                                    dianjiy);
            dianjipanding = 10;
            dianjijiao = -jiao + 90;
        }
}

void CVoyVideoDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: Add your message handler code here and/or call default

    CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CVoyVideoDlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: Add your message handler code here and/or call default

    CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
    //	AfxMessageBox("11");
}

void CVoyVideoDlg::OnButton6()
{
    // TODO: Add your control notification handler code here
    /*
    CDC dcMem;
    CPaintDC dc(this);
    dcMem.CreateCompatibleDC(&dc);
    CBitmap bm;
    bm.LoadBitmap(IDB_BITMAP2);
    dcMem.SelectObject(&bm);
    dc.BitBlt(0, 0, 161, 110, &dcMem, 0, 0, SRCCOPY);
    int a;
    a=dcMem.GetPix""el(5,3) ;
    CString xx;
    xx.Format("%d",a);
    AfxMessageBox(xx);
    */
    //	m_cmd.SetBehavior(&m_idle);
    //	m_Camera.pGetImage = NULL;

    m_robot.Brake(0);

    //	m_Camera.End();
}

void CVoyVideoDlg::m_printf(CString info)
{
    m_cStatusList.AddString(info);
    m_cStatusList.SetCurSel(m_cStatusList.GetCount() - 1);
}

void CVoyVideoDlg::m_DisableAll()
{
    (CWnd *)GetDlgItem(IDC_CAPSTART)->EnableWindow(FALSE);
    (CWnd *)GetDlgItem(IDC_STOPCAP)->EnableWindow(FALSE);
    (CWnd *)GetDlgItem(IDC_HSI)->EnableWindow(FALSE);
}

void CVoyVideoDlg::m_EnableAll()
{
    (CWnd *)GetDlgItem(IDC_CAPSTART)->EnableWindow(TRUE);
    (CWnd *)GetDlgItem(IDC_STOPCAP)->EnableWindow(TRUE);
    (CWnd *)GetDlgItem(IDC_HSI)->EnableWindow(TRUE);
}

void CVoyVideoDlg::OnCntcoach()
{
    // TODO: Add your control notification handler code here

    UpdateData();
    // m_find.net.pCntBtn = (CWnd*) GetDlgItem(IDC_CNTCOACH);
    m_find.net.m_pShowList = &m_cStatusList;
    // m_find.net.ConnectTo(m_strCoachIP,2008);
#ifndef RUN_WITH_EMU_LINK_MSLCoach2 // 设置使用本机上的原教练机
    m_find.net.SetTarget(m_strCoachIP, 20090);
#endif
#ifdef RUN_WITH_EMU_LINK_MSLCoach2 // 设置使用本机上的2号教练机 用于测试双队对抗
    m_find.net.SetTarget(m_strCoachIP, 30090);
#endif
    m_find.net.SetCoachIP(m_strCoachIP);
    m_printf("UDP通讯建立...");
    // m_find.net.pCntBtn->EnableWindow(FALSE);
    m_find.net.Report();
    // SetTimer(3,3000,NULL);
    m_cpu_battery.AutoQuery(5000);
    SetTimer(5, 5000, NULL);
}

void CVoyVideoDlg::OnChangeid()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    // m_find.net.m_status.ID = m_nAgentID;
    m_find.net.SetID(m_nAgentID);
}

void CVoyVideoDlg::SaveCfg()
{
    UpdateData();

    CString strValue;
    // AgentID
    strValue.Format("%d", m_nAgentID);
    BOOL res = WritePrivateProfileString("AGENT", "AGENT_ID", strValue, strCFGFile);

    // RobotPort
    strValue.Format("%d", m_nPort);
    WritePrivateProfileString("PORT", "ROBOT", strValue, strCFGFile);

    // MPU_Port
    strValue.Format("%d", m_nMpuPort);
    WritePrivateProfileString("PORT", "MPU", strValue, strCFGFile);

    // IP
    WritePrivateProfileString("AGENT", "MON_IP", m_strCoachIP, strCFGFile);

    // m_nHoldBall
    strValue.Format("%f", m_nHoldBall);
    WritePrivateProfileString("AGENT", "HOLDBALL", strValue, strCFGFile);

    ////////////////////////////////////////////////
    // 	m_cfg.AgentID = m_nAgentID;
    // 	m_cfg.Port = m_nPort;
    //
    // 	m_cfg.nOverlookIndex = m_cOverlookCombo.GetCurSel();
    //
    // 	strcpy(m_cfg.CoachIP,m_strCoachIP.GetBuffer(m_strCoachIP.GetLength()));
    //
    // 	m_cfg.HoldBall = m_nHoldBall;
    //
    // 	//m_cfg.nCompPort = m_nCmpsPort;
    //
    // 	s.Save(".\\Config\\default.cfg",m_cfg);
    ////////////////////////////////////////////////////////
}

void CVoyVideoDlg::LoadCfg()
{
    BOOL load = s.Load(".\\Config\\default.cfg", &m_cfg);
    if (load == TRUE)
    {
        CString str;
        m_nCamOffsetX = m_cfg.nCameraOffsetX;
        m_nCamOffsetY = m_cfg.nCameraOffsetY;
        m_Camera.m_nOffSetX = m_nCamOffsetX;
        m_Camera.m_nOffSetY = m_nCamOffsetY;

        str.Format("%d", m_cfg.nCameraOffsetX);
        ((CWnd *)GetDlgItem(IDC_CAM_OFFSET_X))->SetWindowText(str);
        str.Format("%d", m_cfg.nCameraOffsetY);
        ((CWnd *)GetDlgItem(IDC_CAM_OFFSET_Y))->SetWindowText(str);

        // port
        str.Format("%d", m_cfg.Port);
        ((CWnd *)GetDlgItem(IDC_PORT))->SetWindowText(str);
        m_nPort = m_cfg.Port;

        // agentid
        str.Format("%d", m_cfg.AgentID);
        ((CWnd *)GetDlgItem(IDC_AGENTID))->SetWindowText(str);
        m_nAgentID = m_cfg.AgentID;

        // camera index
        m_cOverlookCombo.SetCurSel(m_cfg.nOverlookIndex);

        // to coach
        str.Format("%s", m_cfg.CoachIP);
        m_strCoachIP = str;
        ((CWnd *)GetDlgItem(IDC_COACHIP))->SetWindowText(str);

        // param
        str.Format("%.3f", m_cfg.HoldBall);
        ((CWnd *)GetDlgItem(IDC_HOLDBALL))->SetWindowText(str);
        m_nHoldBall = m_cfg.HoldBall;

        // compass port
        // 		str.Format("%d",m_cfg.nCompPort);
        // 		((CWnd*)GetDlgItem(IDC_COMPASPORT))->SetWindowText(str);
        // 		m_nCmpsPort = m_cfg.HoldBall;

        // auto
        // SetTimer(0,500,NULL);
        m_printf("default.cfg加载成功！");
        m_printf("请点击“快速启动”按钮");
    }
    else
    {
        m_printf("default.cfg文件不存在，请手动配置设置信息");
    }
}

static CFileFind finder;
void CVoyVideoDlg::OnTimer(UINT nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    switch (nIDEvent)
    {
    case 0:
        // OnCapstart();
        // OnCap2();
        OnHsi();
        OnButton1();
        OnCntcoach();
        KillTimer(0);
        break;

    case 2:
        if (finder.FindFile(".\\Config\\Default.th2") == TRUE)
        {
            m_Camera.LoadParamFromINI(".\\Config\\Default.th2");
        }
        KillTimer(2);
        m_printf("摄像头参数设置完成！");
        break;

    case 3:
        OnCntcoach();
        break;

    case 4: // 刷新地图
        m_cField.Invalidate(FALSE);
        break;

    case 5: // 更新cpu、电池和帧率信息
        // 电池
        m_find.net.SetBatteryStatus(m_cpu_battery.lpPwrStatus.ACLineStatus, m_cpu_battery.lpPwrStatus.BatteryFlag,
                                    m_cpu_battery.lpPwrStatus.BatteryLifePercent,
                                    m_cpu_battery.lpPwrStatus.BatteryLifeTime);

        // CPU
        m_find.net.SetCPUStatus(m_cpu_battery.nCpuUsage, m_cpu_battery.fCpuFrequency);

        // 机体信息
        m_find.net.SetRobotStatus(m_robot.m_valAD[4] / 17, m_robot.m_valIO[2]);

        // 姿态模块统计
        m_find.net.SetAttitudeFPS(m_mpu.m_nRecvCntLog);
        m_mpu.m_nRecvCntLog = 0;

        // 视频帧返送统计
        m_find.net.SetVisionFPS(m_find.nVisionFps);

        // 串口帧率返送
        m_find.net.SetSerialPortFPS(m_robot.m_nRecvCntLog); // 串口自动清零

        // 组播
        m_find.net.SetMulticastFPS(m_find.m_zubo.m_nRecvCntLog);
        m_find.m_zubo.m_nRecvCntLog = 0;

        // 软件版本
        m_find.net.SetSoftwareVer(m_find.fSoftwareVer);
        break;

    case 255:
        short speed[5];
        for (int j = 0; j < 10000; j++)
        {
            for (int i = 0; i < 5; i++)
            {
                speed[i] = 0;
            }
            speed[0] = rand();
            speed[1] = rand();
            speed[2] = rand();
            m_robot.SetFiveMotorsSpeed(speed);
            // m_cmd.SynGetMotorPos(2000);
            // Sleep(500);
            // m_cmd.SynGetIO(3000);
            // Sleep(500);
            // m_cmd.SynGetAD(2000);
            // m_cmd.Kick(1000);
        }

        break;
    }
    CDialog::OnTimer(nIDEvent);
}

void CVoyVideoDlg::OnAutostart()
{
    // TODO: Add your control notification handler code here
    OnButton1(); // 打开串口
    Sleep(300);
    OnOpenMpuPort();
    Sleep(300);
    OnChangeid();
    OnCapstart();
    OnHsi();
    OnCntcoach();

    (CWnd *)GetDlgItem(IDC_AUTOSTART)->EnableWindow(FALSE);
}

void CVoyVideoDlg::OnParam()
{
    // TODO: Add your control notification handler code here
    SaveCfg();
}

void CVoyVideoDlg::OnUpdateparam()
{
    // TODO: Add your control notification handler code here

    UpdateData();
    m_find.holdball = m_nHoldBall;
}

void CVoyVideoDlg::OnClose()
{
    // TODO: Add your message handler code here and/or call default
    KillTimer(3);
    KillTimer(5);
    OnButton6();
    Sleep(100);

    if (m_dlgMInfo != NULL)
    {
        delete m_dlgMInfo;
        m_dlgMInfo = NULL;
    }

    if (m_dlgView != NULL)
    {
        delete m_dlgView;
        m_dlgView = NULL;
    }

    if (m_pdlgDebug != NULL)
    {
        delete m_pdlgDebug;
        m_pdlgDebug = NULL;
    }

    if (m_pdlgDView != NULL)
    {
        delete m_pdlgDView;
        m_pdlgDView = NULL;
    }
    Sleep(125);
    OnStopcap();
    Sleep(375);
    CDialog::OnClose();
}

BOOL CVoyVideoDlg::PreTranslateMessage(MSG *pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    // TODO: Add your specialized code here and/or call the base class
    int i;
    // 判断窗体消息
    int nVirtKey;
    if (pMsg->message == WM_KEYDOWN)
    {
        // 判断按下的键
        nVirtKey = int(pMsg->wParam);

        if (nVirtKey == VK_RETURN)
        {
            // 判断当前的焦点控件
            CWnd *EditWnd = this->GetFocus();
            if (EditWnd->GetDlgCtrlID() == IDC_SET)
            {
                // 获取控件里的输入字符串
                UpdateData();
                AfxMessageBox(m_setword);
                kongzhitai = m_setword;
                return TRUE;
            }
        }

        switch (nVirtKey) // 根据虚拟键表值来判断按下的是什么键
        {
        case VK_UP: // 按下前键
            break;

        case VK_DOWN: // 按下后键
            break;

        case VK_LEFT: // 按下左键
            break;

        case VK_RIGHT: // 按下右键
            break;

        case VK_SHIFT: // Shift
            for (i = 0; i < MOTOR_NUM; i++)
            {
                m_cOmniMove.speed[i] = 0;
            }
            m_robot.SetFiveMotorsSpeed(m_cOmniMove.speed);
            break;

        case 87:
            // AfxMessageBox(_T("w按下"));
            m_cOmniMove.PointToVelocity(CPoint(110, 80));
            m_cOmniMove.Invalidate(FALSE);
            break;
        case 83:
            // AfxMessageBox(_T("s按下"));
            m_cOmniMove.PointToVelocity(CPoint(110, 110));
            m_cOmniMove.Invalidate(FALSE);
            break;
        case 65:
            // AfxMessageBox(_T("a按下"));
            m_cOmniMove.PointToVelocity(CPoint(80, 110));
            m_cOmniMove.Invalidate(FALSE);
            break;
        case 68:
            // AfxMessageBox(_T("d按下"));
            m_cOmniMove.PointToVelocity(CPoint(140, 110));
            m_cOmniMove.Invalidate(FALSE);
            break;
        case 81:

            break;
        default:
            break;
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}

void CVoyVideoDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
    double jiao;
    double chang;

    if (point.x > 522 && point.x < 1010 && point.y > 0 && point.y < 370)
    {
        m_find.m_find.jizuobiao((point.x - 569) * 4.5112781954887218045112781954887,
                                (-point.y + 321) * 4.5454545454545454545454545454545, &jiao, &chang, dianjix, dianjiy);

        dianjipanding = 10;
        button1 = 3;
        dianjijiao = -jiao + 90;
    }
}

void CVoyVideoDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    button1 = 0;
    CDialog::OnRButtonUp(nFlags, point);
}

double temp_juli = 0;
// 预测 实验梯度 应该与当前旋转平移方向 一致  实验步长应与上次梯度大小一致

// temp_min 存储的是掐头去尾过的 误差和	[11100]表示每幅图片可以有11100个点
int wp_dis_num[11100] = {0};              // 每个点距离 排序
float wp_dis_w[11100] = {0};              // 每个点距离 存储值
double temp_min_histary[1110] = {0};      //[MAX_Gdown2_Loop_time];	[1110]表示可以存储  循环1110次的数据
float temp_min_mis_a_histary[1110] = {0}; //[MAX_Gdown2_Loop_time];
float temp_min_mis_x_histary[1110] = {0}; //[MAX_Gdown2_Loop_time];
float temp_min_mis_y_histary[1110] = {0}; //[MAX_Gdown2_Loop_time];

// void field_line_matching_Gdown2(void);//最速下降法2型下降控制参数设置
// 头占的百分数
float up_line_Gdown2_set = 0.0;   // 0.15;      //0.1;//0.5;
float down_line_Gdown2_set = 0.0; // 0.4;//0.35;
// int MAX_Gdown2_Loop_time=10;//80;//设置下降迭代最大次数
// float angle_change_Gdown2_quotiety    =5;//6;//3;//6;//角度量变化速度系数
// float     x_change_Gdown2_quotiety    =1.5;//2;//1;//2;//x方向平移变化速度系数
// float     y_change_Gdown2_quotiety    =1.5;//2;//1;//2;//y方向平移变化速度系数
// int MAX_err_Gdown2_direct = 7;//3;//6;//设置最多震荡次数   解释：如果已经下降到底 因为掩膜步距精度有限所以只能
// 在最好点和非最好点间震荡 振一次 累加一

int MAX_Gdown2_Loop_time = 60;          // 20;     //140;//10;//80;//设置下降迭代最大次数
float angle_change_Gdown2_quotiety = 5; // 5;//  5;//2.5;//6;//3;//6;//角度量变化速度系数
float x_change_Gdown2_quotiety = 1.5;   // 3;//  1.5;//0.8;//2;//1;//2;//x方向平移变化速度系数
float y_change_Gdown2_quotiety = 1.5;   // 3;//  1.5;//0.8;//2;//1;//2;//y方向平移变化速度系数
float max_speed_abs = 0.6;              // 1.0;//设置可变下降速率的最大速率
int MAX_err_Gdown2_direct = 20;         // 10;     //15;//3;//6;//设置最多震荡次数   解释：如果已经下降到底
                                        // 因为掩膜步距精度有限所以只能 在最好点和非最好点间震荡 振一次 累加一

float white_point_a[10000] = {0};
float white_point_tr[10000] = {0};
float white_point_tr_org[10000] = {0};
float white_point_w[10000] = {0};

double l_x = 320; // 170;//120;
double l_y = 240; // 143;//110;
double l_a = 90;  //+-90;

int save_start_loop = 0; // 保护开始 运行周期
const line_num = 144;
const line_radius_max = 240;
double rectify_tab2[line_radius_max] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; // 畸变二次矫正表
double rectify_tab2p[line_radius_max];
// 靠单个 点 进行 调整函数
inline void Adjust_rectify_tab(
    float angle, float radiusM,
    float radiusI) // angle 白线点所在角度 radiusM (radius mask) 匹配图上的角度 radiusI (radius In) 采集图上的角度
{
    if (radiusI == 0)
        return; // 保险程序 防止分母为零
    double rectify_tab2_radiusI_c = radiusM / radiusI;
    double rectify_tab2_radiusI_p = rectify_tab2[int(radiusI)];
    double temp_c_p = (rectify_tab2_radiusI_c - rectify_tab2_radiusI_p) / 50.0;
    if ((temp_c_p) > 0.002)
        temp_c_p = 0.002;
    if ((temp_c_p) < -0.002)
        temp_c_p = -0.002;
    rectify_tab2_radiusI_c = rectify_tab2_radiusI_p + temp_c_p;
    if (rectify_tab2_radiusI_p == 0)
        return; // 保险程序 防止分母为零
    double times = rectify_tab2_radiusI_c / rectify_tab2_radiusI_p;
    //		Oi'=Oi * Oa' /Oa
    //		INa*tab1[i]*tab2[i]=OUTa				:tab1[i] 第一次矫正表  tab2[i]第二次矫正表
    //		tab2[i] =OUTa    / INa*tab1[i]			:OUT矫正后输出 [即模板 大小]			INa矫正前输入[视觉输入]
    //		tab2[i] =radiusM / radiusI
    int i;
    for (i = 0; i < line_radius_max; i++)
    {
        rectify_tab2p[i] = rectify_tab2[i];
    }

    for (i = 0; i <= radiusI; i++)
    {
        rectify_tab2[i] = rectify_tab2[i] * times;
    }

    for (i = i; i <= line_radius_max; i++)
    {
        //	rectify_tab2[i] =  rectify_tab2[int(radiusI)] +sqrt(0.01)*sqrt((double)(j));//rectify_tab2_radiusI_c -
        // rectify_tab2_radiusI_p;
        rectify_tab2[i] = (rectify_tab2[i - 1] * (i - 1) + (i * rectify_tab2p[i] - (i - 1) * rectify_tab2p[i - 1])) /
                          i; // rectify_tab2_radiusI_c - rectify_tab2_radiusI_p;
    }
    //(i*rectify_tab2[i+1]-i*rectify_tab2[i])/
    /*	rectify_tab2[(int)radiusI] = rectify_tab2[(int)radiusI] * times;
     */
    //	for ( i=0;i<line_radius_max;i++)
    //	{
    //		rectify_tab2[i]=0.7;//
    //	}
}
// 双线性插值函数
/*
inline float Bilinear02(float fx,float fy)
{
        int x=(int)fx; if (x>fx) --x; //x=floor(fx);
        int y=(int)fy; if (y>fy) --y; //y=floor(fy);

        float u=fx-x;
        float v=fy-y;
        float pm3=u*v;
        float pm2=u*(1-v);
        float pm1=v*(1-u);
        float pm0=(1-u)*(1-v);
        //(x,y) 左下(+0,+0)           左上(+0,+1)             右下(+1,+0)             右上(+1,+1)
        if((x<=0)||((x+1)>=640)||(y<=0)||((y+1)>=480))
            return (100);
        return (pm0*shendubiao[x][y] +pm1*shendubiao[x][y+1] +pm2*shendubiao[x+1][y] +pm3*shendubiao[x+1][y+1]     );
}*/
// 双线性插值函数
float Bilinear02_u, Bilinear02_v, Bilinear02_pm3, Bilinear02_pm2, Bilinear02_pm1, Bilinear02_pm0, Bilinear02_t;
int Bilinear02_x, Bilinear02_y;
char Bilinear02_mode = 1; // 设置Bilinear02工作在差值方式(1) 还是直接返回模板模式(0)
double max_return = 36;   // 150;//15;
float Bilinear02_tx = 0;
float Bilinear02_ty = 0;

inline float Bilinear02(float fx, float fy)
{
    if ((fx <= 150) || ((fx + 1) >= 640 - 150) || (fy <= 117) || ((fy + 1) >= 480 - 117))
    {
        //	/*20110615
        if ((fx <= 0) || ((fx + 1) >= 640) || (fy <= 0) || ((fy + 1) >= 480))
        {
            if (fx <= 0)
                Bilinear02_tx = 0 - fx;
            if ((fx + 1) >= 640)
                Bilinear02_tx = fx + 1 - 640;
            if (fy <= 0)
                Bilinear02_ty = 0 - fy;
            if ((fy + 1) >= 480)
                Bilinear02_ty = fy + 1 - 480;
            return (max_return + Bilinear02_tx + Bilinear02_ty);
        }
        else
        //	20110615*/
        {
            return (max_return);
        }
    }
    if (Bilinear02_mode == 0)
        return (double(shendubiao[int(fx)][int(fy)]));

    Bilinear02_x = (int)fx;
    if (Bilinear02_x > fx)
        --Bilinear02_x; // x=floor(fx);
    Bilinear02_y = (int)fy;
    if (Bilinear02_y > fy)
        --Bilinear02_y; // y=floor(fy);

    Bilinear02_u = fx - Bilinear02_x;
    Bilinear02_v = fy - Bilinear02_y;
    Bilinear02_pm3 = Bilinear02_u * Bilinear02_v;
    Bilinear02_pm2 = Bilinear02_u * (1 - Bilinear02_v);
    Bilinear02_pm1 = Bilinear02_v * (1 - Bilinear02_u);
    Bilinear02_pm0 = (1 - Bilinear02_u) * (1 - Bilinear02_v);
    //(x,y) 左下(+0,+0)           左上(+0,+1)             右下(+1,+0)             右上(+1,+1)
    //		if((Bilinear02_x<=0)||((Bilinear02_x+1)>=640)||(Bilinear02_y<=0)||((Bilinear02_y+1)>=480))
    //			return (100);

    Bilinear02_t = Bilinear02_pm0 * shendubiao[Bilinear02_x][Bilinear02_y] +
                   Bilinear02_pm1 * shendubiao[Bilinear02_x][Bilinear02_y + 1] +
                   Bilinear02_pm2 * shendubiao[Bilinear02_x + 1][Bilinear02_y] +
                   Bilinear02_pm3 * shendubiao[Bilinear02_x + 1][Bilinear02_y + 1]; //
    if (Bilinear02_t > max_return)
        Bilinear02_t = max_return;
    return (Bilinear02_t);

    //		return (Bilinear02_pm0*shendubiao[Bilinear02_x][Bilinear02_y]
    //+Bilinear02_pm1*shendubiao[Bilinear02_x][Bilinear02_y+1]
    //+Bilinear02_pm2*shendubiao[Bilinear02_x+1][Bilinear02_y]
    //+Bilinear02_pm3*shendubiao[Bilinear02_x+1][Bilinear02_y+1]     );

    //		测试代码
    //		return (pm0*map_2d[y][x] +pm1*map_2d[y+1][x] +pm2*map_2d[y][x+1] +pm3*map_2d[y+1][x+1]     );
}

// int setp=0;
double UPRColor::pipei2_5(Yuzhi *changdi)
{
    //	if (setp>=27)
    //	{	setp=setp;
    //	}
    //	setp++;
    if (changdi->benjix > (1800 + 200))
    {
        changdi->benjix = (1800 + 200);
    }
    if (changdi->benjix < (-200))
    {
        changdi->benjix = (-200);
    }

    if (changdi->benjiy > (1200 + 200))
    {
        changdi->benjiy = (1200 + 200);
    }
    if (changdi->benjiy < (-200))
    {
        changdi->benjiy = (-200);
    }
    // y.benjijiao=-181-360;
    //		if((changdi->benjijiao>180)||(changdi->benjijiao<-180))//角度取整
    //		{	int temp_benjijiao    = changdi->benjijiao/360;
    //			changdi->benjijiao = changdi->benjijiao-temp_benjijiao*360;
    //			angle_add_transfer2(changdi->benjijiao,0);
    //		}
    float temp_benjijiao = angle_add_transfer2(changdi->benjijiao, 0);
    changdi->benjijiao = temp_benjijiao;

    l_x = (changdi->benjix - 900) / 6 + 320;
    l_y = -(changdi->benjiy - 600) / 6 + 240;
    l_a = angle_add_transfer2(changdi->benjijiao, 90);
    //		if (l_a> 180)
    //			l_a-=360;
    //		if (l_a<-180)
    //			l_a+=360;

    int white_point_l = xiandianshu;
    if (white_point_l <= 3) // 当点少于3点定位pipei2直接跳过return(1000000)
    {
        return (1000000);
    }

    float try_angle; // 试验的角度
    float try_x;     // 试验的x偏移量
    float try_y;     // 试验的y偏移量

    //	changdi->benjix=900+zuixiaox1*6;
    //	changdi->benjiy=600-zuixiaoy1*6;
    //	changdi->benjijiao=zuixiaojiao-90;
    //	qishix=min_mis_x;
    //	qishiy=min_mis_y;
    //	qishijiao=min_mis_a;
    double min_mis_x =
        l_x; //=StartLocaliSationX;//[10];register	//误差 最小点的序列  可以从中判断大概真正位置 用于下降法的起点
    double min_mis_y = l_y;  //=StartLocaliSationY;//[10];register
    double min_mis_a = l_a;  //=StartLocaliSationA;//[10];register
    double temp_min = 0;     // 存有 普通全屏匹配后的 最小误差
    double temp_min_add = 0; // 存有 普通全屏匹配后的 [最小误差]与[期望误差]的差的[累加和][注:可以是平方]

    int up_line = white_point_l * (1 - up_line_Gdown2_set); // 掐头 最大值部分 //掐头  去掉最大误差的几个点
                                                            // 用于在匹配比较好的时候 把查找错误的噪点去掉
    int down_line =
        white_point_l *
        down_line_Gdown2_set; // 去位 最小值部分//去尾  去掉最小误差的几个点	对于匹配更有效果 容易走出局部极小点

    int err_direct = 0; // 如果已经下降到底 因为掩膜步距精度有限所以只能 在最好点和非最好点间震荡 振一次 累加一
    int i, j, k, l, x, y;
    double temp = 0;
    double temp_ = 0;
    double tempx = 0;
    double tempy = 0;
    double tempr = 0;
    double tempa = 0;
    const double x_fanwei_min = 320 - 200;
    const double x_fanwei_max = 320 + 200;
    const double y_fanwei_min = 240 - 135;
    const double y_fanwei_max = 240 + 135;
    // for(i=1;i<=xiandianshu;i++)
    j = 0;
    for (i = 0; i < white_point_l; i++)
    {
        tempr = coordinate_transfer_radius(changxianx[i] - 320, changxiany[i] - 240);
        if ((tempr < 1200) && (tempr > 10))
        {
            tempa = coordinate_transfer_angle(changxianx[i] - 320, changxiany[i] - 240);
            tempx = coordinate_transfer_x(tempa + min_mis_a, tempr) + min_mis_x;
            tempy = coordinate_transfer_y(tempa + min_mis_a, tempr) + min_mis_y;
            //		if((tempx>173-36)&&(tempx<320)&&(tempy>160-36)&&(tempy<240))//匹配 屏幕 左下角1/4部分
            if ((tempx > x_fanwei_min) && (tempx < x_fanwei_max) && (tempy > y_fanwei_min) &&
                (tempy < y_fanwei_max)) // 匹配 屏幕 左下角1/4部分
            {
                if (shendubiao[int(tempx)][int(tempy)] < 15)
                {
                    white_point_a[j] = tempa;
                    white_point_tr[j] = tempr;
                    j++;
                }
            }
        }

        //	white_point_a[i] =coordinate_transfer_angle (white_point_x[i],white_point_y[i]);
        //	white_point_tr[i]=coordinate_transfer_radius(white_point_x[i],white_point_y[i]);
        //	white_point_tr_org[i]=white_point_tr[i];
        //	white_point_tr[i]=white_point_tr[i]*1;//暂时使用原值 不启用矫正表rectify_tab2[int(white_point_tr[i])];
        //	white_point_tr[i]=white_point_tr[i];//*rectify_tab2[int(white_point_tr[i])];
        //		white_point_w[i]=1;
    }
    white_point_l = j;
    if (white_point_l <= 3) // 当点少于3点定位pipei2直接跳过return(1000000)
    {
        return (1000000);
    }
    // for (i=0;i<white_point_l;i++)
    //{    wp_dis_num[i]=i;
    //      wp_dis_w[i]=0;
    // }
    // 2最速下降法
    // temp_min//现在存的是最小的误差 大小
    float delta_temp_a; // 德耳塔 △ temp的变量
    float delta_temp_x; // 德耳塔 △ temp的变量
    float delta_temp_y; // 德耳塔 △ temp的变量
    // float delta_temp_pa;//上个周期的 德耳塔 △ temp的变量	暂时没有使用
    // float delta_temp_px;//上个周期的 德耳塔 △ temp的变量	暂时没有使用
    // float delta_temp_py;//上个周期的 德耳塔 △ temp的变量	暂时没有使用
    float step_a = 0.01; // 为测试偏导数的试验步长变量 角度
    float step_x = 0.01; // 为测试偏导数的试验步长变量 x
    float step_y = 0.01; // 为测试偏导数的试验步长变量 y

    if (Bilinear02_mode == 1) // 设置Bilinear02工作在双线性差值方式(1) 还是直接返回模板模式(0)
    {
        step_a = 0.12; // 0.21;//为测试偏导数的试验步长变量 角度
        step_x = 0.06; // 0.51;//为测试偏导数的试验步长变量 x
        step_y = 0.06; // 0.51;//为测试偏导数的试验步长变量 y
    }
    else
    {
        step_a = 0.5; // 为测试偏导数的试验步长变量 角度
        step_x = 1;   // 为测试偏导数的试验步长变量 x
        step_y = 1;   // 为测试偏导数的试验步长变量 y
    }

    double speed_abs; // 梯度的倒数 用来控制 下降步长   梯度越大越陡 步长越小

    // 初始化temp_min 和点误差排序
    try_angle = min_mis_a;
    try_x = min_mis_x;
    try_y = min_mis_y;
    temp = 0;
    for (i = 0; i < white_point_l; i++)
    {
        temp += Bilinear02((coordinate_transfer_x(white_point_a[i] + try_angle, white_point_tr[i]) + try_x),
                           (coordinate_transfer_y(white_point_a[i] + try_angle, white_point_tr[i]) + try_y));
    }
    temp_min = temp;

    for (l = 0; l < MAX_Gdown2_Loop_time; l++)
    {
        temp = 0;
        try_angle = min_mis_a + step_a;
        try_x = min_mis_x + 0;
        try_y = min_mis_y + 0;
        for (i = 0; i < white_point_l; i++)
        {
            temp += Bilinear02((coordinate_transfer_x(white_point_a[i] + try_angle, white_point_tr[i]) + try_x),
                               (coordinate_transfer_y(white_point_a[i] + try_angle, white_point_tr[i]) + try_y));
        }
        delta_temp_a = temp_min - temp;

        temp = 0;
        temp_ = 0;
        // try_angle=min_mis_a+0;	try_x=min_mis_x+1;	try_y=min_mis_y+0;
        //	double abc[1000]={0};
        //	int txx[1000]={0};
        //	int tyy[1000]={0};
        for (i = 0; i < white_point_l; i++)
        {
            tempx = (coordinate_transfer_x(white_point_a[i] + min_mis_a, white_point_tr[i]) + min_mis_x);
            tempy = (coordinate_transfer_y(white_point_a[i] + min_mis_a, white_point_tr[i]) + min_mis_y);
            temp += Bilinear02(tempx + step_x, tempy);
            temp_ += Bilinear02(tempx, tempy + step_y);
        }
        delta_temp_x = temp_min - temp;
        delta_temp_y = temp_min - temp_;

        temp = 0;
        // double delta_abs=
        if ((delta_temp_a <= 0.001) && (delta_temp_x <= 0.001) &&
            (delta_temp_y <=
             0.001)) // 注意这个判定错误三项不为0时仍然和可为0 ((delta_temp_a + delta_temp_x + delta_temp_y)!=0)
            speed_abs = max_speed_abs; // 如果有最极端的现象出现 各偏导数都为零 就用一个比较大的增长速率0.1
        else
            speed_abs =
                1 / sqrt(delta_temp_a * delta_temp_a + delta_temp_x * delta_temp_x + delta_temp_y * delta_temp_y);
        if (speed_abs > max_speed_abs)
            speed_abs = max_speed_abs;
        // if (speed_abs<0.0001)
        //  speed_abs=0;//
        // else
        // double speed=(1.0-(float)(l)/(float)(MAX_Gdown2_Loop_time))+0.0001;//
        double speed2 =
            (1.01 - float(err_direct) / float(MAX_err_Gdown2_direct + 1)); // 有可能err_direct>MAX_err_Gdown2_direct+1
        speed2 = speed2 * speed2 + 0.001;                                  //*speed2
        if (speed2 > 1.0)
            speed2 = 1.0;
        if (speed2 < 0.0001)
            speed2 = 0.0001;

        // debug用代码  找出下降调变点
        //	if (delta_temp_a*speed_abs*6>5)
        //		min_mis_a=min_mis_a;
        //	if (delta_temp_x*speed_abs*2>5)
        //		delta_temp_x=delta_temp_x;
        //	if (delta_temp_y*speed_abs*2>5)
        //		delta_temp_y=delta_temp_y;
        // debug用代码结束
        double tempAngle = delta_temp_a * speed_abs * angle_change_Gdown2_quotiety * speed2; // 6;//3;//*10;
        double tempX = delta_temp_x * speed_abs * x_change_Gdown2_quotiety * speed2;         // 2;
        double tempY = delta_temp_y * speed_abs * y_change_Gdown2_quotiety * speed2;         // 2;
        if (tempAngle > 2.0)
            tempAngle = 2.0;
        if (tempAngle < -2.0)
            tempAngle = -2.0;
        if (fabs(tempX) > 2.0 || fabs(tempY) > 2.0)
        {
            if (fabs(tempX) > fabs(tempY))
            {
                tempY = tempY * 2.0 / tempX;
                tempX = 2.0;
            }
            else
            {
                tempX = tempX * 2.0 / tempY;
                tempY = 2.0;
            }
        }
        min_mis_a = angle_add_transfer2(min_mis_a, tempAngle);
        min_mis_x += tempX;
        min_mis_y += tempY;

        if (min_mis_x > 640 - 1)
            min_mis_x = 640 - 1;
        if (min_mis_x < 0)
            min_mis_x = 0;
        if (min_mis_y > 480 - 1)
            min_mis_y = 480 - 1;
        if (min_mis_y < 0)
            min_mis_y = 0;

        //	speed_abs=speed_abs;
        // 保险程序 防止使用到场地距离样板以外的地方
        //	if (min_mis_x<mark_x/2)
        //		min_mis_x=mark_x/2;
        //	if (min_mis_x>mark_x/2+former_x)
        //		min_mis_x=mark_x/2+former_x;
        //	if (min_mis_y<mark_y/2)
        //		min_mis_y=mark_y/2;
        //	if (min_mis_y>mark_y/2+former_y)
        //		min_mis_y=mark_y/2+former_y;

        try_angle = min_mis_a;
        try_x = min_mis_x;
        try_y = min_mis_y;
        temp = 0;
        for (i = 0; i < white_point_l; i++)
        {
            temp += Bilinear02((coordinate_transfer_x(white_point_a[i] + try_angle, white_point_tr[i]) + try_x),
                               (coordinate_transfer_y(white_point_a[i] + try_angle, white_point_tr[i]) + try_y));
        }
        temp_min = temp;

        // 在20081006 1次大删减中注释掉 不明代码		猜测写错了应该是用来计算所有点距离[误差]和的,但原代码temp没清零.
        // 现在注释掉后temp变成只是掐头去尾后的中间段距离[误差]求和了. 	for (i=0;i<white_point_l;i++)//这里想干什么
        // 有待考证?????????????????????????????????????????!!!!!!!!!!!!!!!!!!!??????????!!!!!!!!!!!!!!???????????? {
        // temp += wp_dis_w[wp_dis_num[i]];
        //   }
        // 在20081006 1次大删减中注释掉 不明代码
        temp_min_histary[l] = temp;
        temp_min_mis_a_histary[l] = min_mis_a;
        temp_min_mis_x_histary[l] = min_mis_x;
        temp_min_mis_y_histary[l] = min_mis_y;
        if (l > 0)
        {
            if (temp_min_histary[l] > temp_min_histary[l - 1])
            {
                err_direct++;
            }
        }
// 显示每次匹配下降路线
#ifdef SELF_LOCATION_02_DIS_DOWN_PATH // 设置使用 OPEN_CV提供的功能 显示每次下降路线
        DrawPoint1(min_mis_x, min_mis_y, 255, 0, 0);
//	DrawPoint1( min_mis_x+coordinate_transfer_x(min_mis_a,5),  min_mis_y+coordinate_transfer_y(min_mis_a,5),
// 0,0,255);
#endif
#ifdef SELF_LOCATION_02_DIS_EVERY_DOWN // 设置使用 OPEN_CV提供的功能 显示每次下降过程
        clean1();
        for (y = 0; y < 480; y++)     // for (y=240;y<241;y++)//
            for (x = 0; x < 640; x++) // for (x=100;x<500;x++)//
            {
                DrawPoint1(x, y, 0, depth_map[y][x] * 10, 0);
            } // 画点
        for (j = 0; j < white_point_l; j++)
        {
            DrawPoint1((int)(coordinate_transfer_x(white_point_a[j] + min_mis_a, white_point_tr[j]) + min_mis_x),
                       (int)(coordinate_transfer_y(white_point_a[j] + min_mis_a, white_point_tr[j]) + min_mis_y), 255,
                       0, 255);
        }
        cvShowImage1(); // 显示
        cvWaitKey(90);
#endif
        // 20110429	if ((err_direct>MAX_err_Gdown2_direct)&&(temp_min_histary[l]<temp_min_histary[l-1])) //大于震荡次数
        // 且下降
        if (err_direct > MAX_err_Gdown2_direct) ////大于震荡次数
        {                                       //	int min_err_num=l;
                                                //	for (i=l;i>=0;i--)
                                                //	{if(temp_min_histary[i]<temp_min_histary[min_err_num])
                                                //		min_err_num=i;
                                                //	}
                                                //	min_mis_a=temp_min_mis_a_histary[min_err_num];
                                                //	min_mis_x=temp_min_mis_x_histary[min_err_num];
                                                //	min_mis_y=temp_min_mis_y_histary[min_err_num];
                                                //	temp_min =temp_min_histary[min_err_num];
            break;
        }

        //	delta_temp_pa = delta_temp_a;
        //	delta_temp_px = delta_temp_x;
        //	delta_temp_py = delta_temp_y;
        /*
            for (i=0;i<white_point_l;i++)
                data[((int)point_ty[i])*320*3 +
           (int)point_tx[i]*3]=255;//dlg->C1_Bool_Data[((int)point_ty[i]+radial_cy)*320 + (int)point_tx[i]+radial_cx]=1;
            dlg->m_VideoOCX.Show(dlg->m_Image);				// display capture frame
            for (i=0;i<white_point_l;i++)
                data[((int)point_ty[i])*320*3 +
           (int)point_tx[i]*3]=0;//dlg->C1_Bool_Data[((int)point_ty[i]+radial_cy)*320 + (int)point_tx[i]+radial_cx]=1;
        */
        //	Sleep(200);
    }
    l--; // 因为出了for循环l已经加了1了
    int min_err_num = l;
    for (i = l; i >= 0; i--)
    {
        if (temp_min_histary[i] < temp_min_histary[min_err_num])
            min_err_num = i;
    }
    min_mis_a = temp_min_mis_a_histary[min_err_num];
    min_mis_x = temp_min_mis_x_histary[min_err_num];
    min_mis_y = temp_min_mis_y_histary[min_err_num];
// temp_min =temp_min_histary[min_err_num];

// outa=min_mis_a;
// outx=min_mis_x;
// outy=min_mis_y;

// 注功能代码转到 上一层类内部调用函数 field_line_matching_Gdown2_speed[n]里 便于使用灵活
//     temp_min_add+=temp_min-temp_min_set;//?????????是否使用线性累加 因为误差增的快减的慢//函数末尾
//     对temp_min_add累加[定位误差累加] if (temp_min_add<0)
//		temp_min_add=0;

// field_line_matching_Gdown_loop_time=l;//记录此次下降用迭代次数
#ifdef SELF_LOCATION_02_DIS_EVERY_DOWN
    cvWaitKey(0);
#endif
    //
    //
    //
    //
    //
    // 删掉的调整曲率的代码
    //
    //
    //
    //

    l_x = min_mis_x;
    l_y = min_mis_y;
    l_a = min_mis_a;

    RGBTRIPLE point1;
    point1.rgbtBlue = 0x00;
    point1.rgbtGreen = 0x00;
    point1.rgbtRed = 0x00;
    // for (x=0;x<640;x++)
    // for (y=0;y<480;y++)
    //{
    //	point1.rgbtGreen=	Bilinear02 (x, y)*10;
    //	SetColor( x,y,point1);
    // }

    RGBTRIPLE ball1;
    ball1.rgbtBlue = 0xFF;
    ball1.rgbtGreen = 0xFF;
    ball1.rgbtRed = 0x00;
    if (pipei2Display == 1)
    { //	for (j=1;j<=xiandianshu;j++)
        for (j = 0; j < white_point_l; j++)
        {
            ball1.rgbtBlue = 0x00;
            ball1.rgbtGreen = 0x00;
            ball1.rgbtRed = 0xFF;
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x - 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y - 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x + 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y + 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x + 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y - 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x - 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y + 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y + 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y - 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x - 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x + 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y), ball1);
        }
        //		up_line  =white_point_l*(1-0);//掐头 最大值部分 //掐头  去掉最大误差的几个点    用于在匹配比较好的时候
        // 把查找错误的噪点去掉 		down_line=white_point_l*0;//去位 最小值部分//去尾  去掉最小误差的几个点
        // 对于匹配更有效果 容易走出局部极小点
/*		for (i=down_line;i<up_line;i++)
		{   j=wp_dis_num[i];
			ball1.rgbtBlue = 0;	ball1.rgbtGreen = 0; ball1.rgbtRed = 255*j/white_point_l;

			SetColor( (int)(coordinate_transfer_x(white_point_a[j] + l_a,white_point_tr[j])+l_x-1),
					  (int)(coordinate_transfer_y(white_point_a[j] + l_a,white_point_tr[j])+l_y-1),ball1);
			SetColor( (int)(coordinate_transfer_x(white_point_a[j] + l_a,white_point_tr[j])+l_x+1),
					  (int)(coordinate_transfer_y(white_point_a[j] + l_a,white_point_tr[j])+l_y+1),ball1);
			SetColor( (int)(coordinate_transfer_x(white_point_a[j] + l_a,white_point_tr[j])+l_x+1),
					  (int)(coordinate_transfer_y(white_point_a[j] + l_a,white_point_tr[j])+l_y-1),ball1);
			SetColor( (int)(coordinate_transfer_x(white_point_a[j] + l_a,white_point_tr[j])+l_x-1),
					  (int)(coordinate_transfer_y(white_point_a[j] + l_a,white_point_tr[j])+l_y+1),ball1);
			SetColor( (int)(coordinate_transfer_x(white_point_a[j] + l_a,white_point_tr[j])+l_x),
					  (int)(coordinate_transfer_y(white_point_a[j] + l_a,white_point_tr[j])+l_y),ball1);
			SetColor( (int)(coordinate_transfer_x(white_point_a[j] + l_a,white_point_tr[j])+l_x),
					  (int)(coordinate_transfer_y(white_point_a[j] + l_a,white_point_tr[j])+l_y+1),ball1);
			SetColor( (int)(coordinate_transfer_x(white_point_a[j] + l_a,white_point_tr[j])+l_x),
					  (int)(coordinate_transfer_y(white_point_a[j] + l_a,white_point_tr[j])+l_y-1),ball1);
			SetColor( (int)(coordinate_transfer_x(white_point_a[j] + l_a,white_point_tr[j])+l_x-1),
					  (int)(coordinate_transfer_y(white_point_a[j] + l_a,white_point_tr[j])+l_y),ball1);
			SetColor( (int)(coordinate_transfer_x(white_point_a[j] + l_a,white_point_tr[j])+l_x+1),
					  (int)(coordinate_transfer_y(white_point_a[j] + l_a,white_point_tr[j])+l_y),ball1);
		}
*/	}
//	for (j=1;j<=xiandianshu;j++)
//	{
//	    SetColor( (int)(coordinate_transfer_x(white_point_a[j] ,white_point_tr[j])+320),
//				  (int)(coordinate_transfer_y(white_point_a[j] ,white_point_tr[j])+240),ball1);
//	}

// ball1.rgbtBlue = 0x00;	ball1.rgbtGreen = 0x00; ball1.rgbtRed = 0x00;
//	for (x=0;x<640;x++)
//	for (y=0;y<480;y++)
//	{	ball1.rgbtGreen = shendubiao[x][y];
//		SetColor(x,y,ball1);
//	}

// 代码接口部分 将坐标转回zuixiaox1  zuixiaoy1  zuixiaojiao
//	pField->SetAgent(2,(l_x-320)*6+900,(-l_y+240)*6+600,l_a-90);	//右边小场地显示标号
// 900+zuixiaox1*6=(l_x-320)*6+900
double zuixiaox1 = (l_x - 320);
// 600-zuixiaoy1*6=(-l_y+240)*6+600
double zuixiaoy1 = (l_y - 240);
// zuixiaojiao-90 =l_a-90
double zuixiaojiao = l_a;
if (zuixiaojiao > 180)
    zuixiaojiao -= 360;
if (zuixiaojiao < -180)
    zuixiaojiao += 360;

double chang;

RGBTRIPLE shizi;
shizi.rgbtBlue = 0x00;
shizi.rgbtGreen = 0x00;
shizi.rgbtRed = 0xFF;
if (pipei2Display == 1)
{
    DrawFocus(zuixiaox1 + 320, zuixiaoy1 + 240, &shizi);
}
jizuobiao(76, 104, &menjiao, &chang, zuixiaox1 + 320, zuixiaoy1 + 240);
//	pField->SetAgent(2,900+zuixiaox1*6,600-zuixiaoy1*6,zuixiaojiao-90);
changdi->benjix = 900 + zuixiaox1 * 6;
changdi->benjiy = 600 - zuixiaoy1 * 6;
changdi->benjijiao = zuixiaojiao - 90;
if (changdi->benjijiao > 360)
    changdi->benjijiao -= 360;
if (changdi->benjijiao < -360)
    changdi->benjijiao += 360;

double balljiao;
double ballchang;
double ballx;
double bally;
balljiao = -changdi->ball_jiao - changdi->benjijiao + 90;
ballchang = quxian[int(changdi->ball_chang)] / cdbl * 2.7; // quxian[int(changdi->ball_chang)];
zhizuobiao(&ballx, &bally, balljiao, ballchang, changdi->benjix, changdi->benjiy);

//		pField->SetBall(2,ballx,bally);
//		pField->SetBall(3,1550,1000);
changdi->quanbx = ballx;
changdi->quanby = bally;

long int juli = 0;
/*
for(i=1;i<=xiandianshu;i++)
{
    juli+=Bilinear02((coordinate_transfer_x(white_point_a[i] + l_a,white_point_tr[i])+l_x),
                     (coordinate_transfer_y(white_point_a[i] + l_a,white_point_tr[i])+l_y));
}
//temp_juli+=(double)juli;//记录累加匹配误差(double)juli/(double)xiandianshu;
//	return (temp_juli);
    return ((float)(juli)/(float)(xiandianshu));
*/
// for (i=down_line;i<up_line;i++)
//{   j=wp_dis_num[i];
//	juli+=Bilinear02((coordinate_transfer_x(white_point_a[j] + l_a,white_point_tr[j])+l_x),
//				     (coordinate_transfer_y(white_point_a[j] + l_a,white_point_tr[j])+l_y));
// }
//	return ((float)(juli)/(float)(up_line-down_line));

temp = 0;
for (i = 0; i < white_point_l; i++)
{
    temp += Bilinear02((coordinate_transfer_x(white_point_a[i] + l_a, white_point_tr[i]) + l_x),
                       (coordinate_transfer_y(white_point_a[i] + l_a, white_point_tr[i]) + l_x));
}
return ((float)(temp) / (float)(white_point_l));
}

double UPRColor::pipei2(Yuzhi *changdi)
{
    /*
    if (save_start_loop<=1000)
    {	save_start_loop++;
        if(save_start_loop>1000)//注释掉 可以变成 1000贞内 有效
            save_start_loop=1000;
        if (save_start_loop>1)
        {
            l_x= (changdi->benjix  -900)/6+320;
            l_y=-(changdi->benjiy  -600)/6+240;
            l_a=changdi->benjijiao+90;
            if (l_a> 180)
                l_a-=360;
            if (l_a<-180)
                l_a+=360;
        }
    }
    */
    if (changdi->benjix > (1800 + 200))
    {
        changdi->benjix = (1800 + 200);
    }
    if (changdi->benjix < (-200))
    {
        changdi->benjix = (-200);
    }

    if (changdi->benjiy > (1200 + 200))
    {
        changdi->benjiy = (1200 + 200);
    }
    if (changdi->benjiy < (-200))
    {
        changdi->benjiy = (-200);
    }
    // y.benjijiao=-181-360;
    if ((changdi->benjijiao > 180) || (changdi->benjijiao < -180)) // 角度取整
    {
        int temp_benjijiao = changdi->benjijiao / 360;
        changdi->benjijiao = changdi->benjijiao - temp_benjijiao * 360;
        angle_add_transfer(changdi->benjijiao, 0);
    }

    l_x = (changdi->benjix - 900) / 6 + 320;
    l_y = -(changdi->benjiy - 600) / 6 + 240;
    l_a = angle_add_transfer(changdi->benjijiao, 90);
    //		if (l_a> 180)
    //			l_a-=360;
    //		if (l_a<-180)
    //			l_a+=360;

    int white_point_l = xiandianshu;
    float try_angle; // 试验的角度
    float try_x;     // 试验的x偏移量
    float try_y;     // 试验的y偏移量

    //	changdi->benjix=900+zuixiaox1*6;
    //	changdi->benjiy=600-zuixiaoy1*6;
    //	changdi->benjijiao=zuixiaojiao-90;
    //	qishix=min_mis_x;
    //	qishiy=min_mis_y;
    //	qishijiao=min_mis_a;
    float min_mis_x =
        l_x; //=StartLocaliSationX;//[10];register	//误差 最小点的序列  可以从中判断大概真正位置 用于下降法的起点
    float min_mis_y = l_y;   //=StartLocaliSationY;//[10];register
    float min_mis_a = l_a;   //=StartLocaliSationA;//[10];register
    double temp_min = 0;     // 存有 普通全屏匹配后的 最小误差
    double temp_min_add = 0; // 存有 普通全屏匹配后的 [最小误差]与[期望误差]的差的[累加和][注:可以是平方]

    int up_line = white_point_l * (1 - up_line_Gdown2_set); // 掐头 最大值部分 //掐头  去掉最大误差的几个点
                                                            // 用于在匹配比较好的时候 把查找错误的噪点去掉
    int down_line =
        white_point_l *
        down_line_Gdown2_set; // 去位 最小值部分//去尾  去掉最小误差的几个点	对于匹配更有效果 容易走出局部极小点

    int err_direct = 0; // 如果已经下降到底 因为掩膜步距精度有限所以只能 在最好点和非最好点间震荡 振一次 累加一
    int i, j, k, l;
    double temp = 0;
    double temp_ = 0;
    double tempx = 0;
    double tempy = 0;

    for (i = 1; i <= xiandianshu; i++)
    {
        white_point_a[i] = coordinate_transfer_angle(changxianx[i] - 320, changxiany[i] - 240);
        white_point_tr[i] = coordinate_transfer_radius(changxianx[i] - 320, changxiany[i] - 240);
        white_point_tr_org[i] = white_point_tr[i];
        //	white_point_tr[i]=white_point_tr[i]*1;//暂时使用原值 不启用矫正表rectify_tab2[int(white_point_tr[i])];
        white_point_tr[i] = white_point_tr[i] * rectify_tab2[int(white_point_tr[i])];
        white_point_w[i] = 1;
    }
    for (i = 0; i < white_point_l; i++)
    {
        wp_dis_num[i] = i;
        wp_dis_w[i] = 0;
    }
    // 2最速下降法
    // temp_min//现在存的是最小的误差 大小
    float delta_temp_a; // 德耳塔 △ temp的变量
    float delta_temp_x; // 德耳塔 △ temp的变量
    float delta_temp_y; // 德耳塔 △ temp的变量
    // float delta_temp_pa;//上个周期的 德耳塔 △ temp的变量	暂时没有使用
    // float delta_temp_px;//上个周期的 德耳塔 △ temp的变量	暂时没有使用
    // float delta_temp_py;//上个周期的 德耳塔 △ temp的变量	暂时没有使用
    float step_a = 0.01f; // 为测试偏导数的试验步长变量 角度
    float step_x = 0.01f; // 为测试偏导数的试验步长变量 x
    float step_y = 0.01f; // 为测试偏导数的试验步长变量 y

    double speed_abs; // 梯度的倒数 用来控制 下降步长   梯度越大越陡 步长越小
    for (l = 0; l < MAX_Gdown2_Loop_time; l++)
    {
        temp = 0;
        try_angle = min_mis_a + step_a;
        try_x = min_mis_x + 0;
        try_y = min_mis_y + 0;
        for (i = down_line; i < up_line; i++)
        {
            j = wp_dis_num[i];
            temp += Bilinear02((coordinate_transfer_x(white_point_a[j] + try_angle, white_point_tr[j]) + try_x),
                               (coordinate_transfer_y(white_point_a[j] + try_angle, white_point_tr[j]) + try_y)) *
                    white_point_w[j];
        }
        delta_temp_a = temp_min - temp;

        temp = 0;
        temp_ = 0;
        // try_angle=min_mis_a+0;	try_x=min_mis_x+1;	try_y=min_mis_y+0;
        double abc[1000] = {0};
        int txx[1000] = {0};
        int tyy[1000] = {0};
        for (i = down_line; i < up_line; i++)
        {
            j = wp_dis_num[i];
            tempx = (coordinate_transfer_x(white_point_a[j] + min_mis_a, white_point_tr[j]) + min_mis_x);
            tempy = (coordinate_transfer_y(white_point_a[j] + min_mis_a, white_point_tr[j]) + min_mis_y);
            temp += Bilinear02(tempx + step_x, tempy) * white_point_w[j];
            temp_ += Bilinear02(tempx, tempy + step_y) * white_point_w[j];
            double avc = Bilinear02(0, 0) * white_point_w[j];
            abc[i] = Bilinear02(tempx + step_x, tempy) * white_point_w[j];
            txx[i] = tempx + step_x;
            tyy[i] = tempy;
        }
        delta_temp_x = temp_min - temp;
        delta_temp_y = temp_min - temp_;

        temp = 0;
        // double delta_abs=
        if ((delta_temp_a == 0) && (delta_temp_x == 0) &&
            (delta_temp_y ==
             0))             // 注意这个判定错误三项不为0时仍然和可为0 ((delta_temp_a + delta_temp_x + delta_temp_y)!=0)
            speed_abs = 0.1; // 如果有最极端的现象出现 各偏导数都为零 就用一个比较大的增长速率0.1
        else
            speed_abs =
                1 / sqrt(delta_temp_a * delta_temp_a + delta_temp_x * delta_temp_x + delta_temp_y * delta_temp_y);
        // if (speed_abs<0.0001)
        //  speed_abs=0;//
        // else
        double speed = 0.6 * (1 - (float)(l) / (float)(MAX_Gdown2_Loop_time)) + 0.0001; //
        // debug用代码  找出下降调变点
        //	if (delta_temp_a*speed_abs*6>5)
        //		min_mis_a=min_mis_a;
        //	if (delta_temp_x*speed_abs*2>5)
        //		delta_temp_x=delta_temp_x;
        //	if (delta_temp_y*speed_abs*2>5)
        //		delta_temp_y=delta_temp_y;
        // debug用代码结束
        double tempAngle = delta_temp_a * speed_abs * angle_change_Gdown2_quotiety * speed; // 6;//3;//*10;
        double tempX = delta_temp_x * speed_abs * x_change_Gdown2_quotiety * speed;         // 2;
        double tempY = delta_temp_y * speed_abs * y_change_Gdown2_quotiety * speed;         // 2;
        min_mis_a += tempAngle;
        if (min_mis_a > 180)
            min_mis_a = min_mis_a - 360;
        if (min_mis_a < -180)
            min_mis_a = min_mis_a + 360;
        min_mis_x += tempX;
        min_mis_y += tempY;
        speed_abs = speed_abs;
        // 保险程序 防止使用到场地距离样板以外的地方
        //	if (min_mis_x<mark_x/2)
        //		min_mis_x=mark_x/2;
        //	if (min_mis_x>mark_x/2+former_x)
        //		min_mis_x=mark_x/2+former_x;
        //	if (min_mis_y<mark_y/2)
        //		min_mis_y=mark_y/2;
        //	if (min_mis_y>mark_y/2+former_y)
        //		min_mis_y=mark_y/2+former_y;

        try_angle = min_mis_a;
        try_x = min_mis_x;
        try_y = min_mis_y;
        for (i = 0; i < white_point_l; i++) // 存储每个点的距离值[既:误差]
        {
            wp_dis_w[i] = Bilinear02((coordinate_transfer_x(white_point_a[i] + try_angle, white_point_tr[i]) + try_x),
                                     (coordinate_transfer_y(white_point_a[i] + try_angle, white_point_tr[i]) + try_y)) *
                          white_point_w[i];
        }
        //	temp_min=temp;
        for (i = 0; i < white_point_l; i++) // 每个点的距离值[既:误差] 进行排序 序号顺序存在wp_dis_num[]中
        {
            for (j = 0; j < (white_point_l - 1 - i); j++) // white_point_l-i
            {
                if (wp_dis_w[wp_dis_num[j]] > wp_dis_w[wp_dis_num[j + 1]])
                {
                    k = wp_dis_num[j];
                    wp_dis_num[j] = wp_dis_num[j + 1];
                    wp_dis_num[j + 1] = k;
                }
            }
        }
        for (i = down_line; i < up_line; i++) // 只取down_line~up_line中间部分的误差求和
        {
            temp += wp_dis_w[wp_dis_num[i]];
        }
        temp_min = temp;

        // 在20081006 1次大删减中注释掉 不明代码		猜测写错了应该是用来计算所有点距离[误差]和的,但原代码temp没清零.
        // 现在注释掉后temp变成只是掐头去尾后的中间段距离[误差]求和了. 	for (i=0;i<white_point_l;i++)//这里想干什么
        // 有待考证?????????????????????????????????????????!!!!!!!!!!!!!!!!!!!??????????!!!!!!!!!!!!!!???????????? {
        // temp += wp_dis_w[wp_dis_num[i]];
        //   }
        // 在20081006 1次大删减中注释掉 不明代码
        temp_min_histary[l] = temp;
        temp_min_mis_a_histary[l] = min_mis_a;
        temp_min_mis_x_histary[l] = min_mis_x;
        temp_min_mis_y_histary[l] = min_mis_y;
        if (l > 0)
        {
            if (temp_min_histary[l] > temp_min_histary[l - 1])
            {
                err_direct++;
            }
        }
        if ((err_direct > MAX_err_Gdown2_direct) &&
            (temp_min_histary[l] <
             temp_min_histary[l - 1])) // 这个退出条件再看看!!!!!!!!!!!!!!!!!!!!!!!!!! 看会不会不经过处理过早退出
        {                              //	int min_err_num=l;
                                       //	for (i=l;i>=0;i--)
                                       //	{if(temp_min_histary[i]<temp_min_histary[min_err_num])
                                       //		min_err_num=i;
                                       //	}
                                       //	min_mis_a=temp_min_mis_a_histary[min_err_num];
                                       //	min_mis_x=temp_min_mis_x_histary[min_err_num];
                                       //	min_mis_y=temp_min_mis_y_histary[min_err_num];
                                       //	temp_min =temp_min_histary[min_err_num];
            break;
        }

        //	delta_temp_pa = delta_temp_a;
        //	delta_temp_px = delta_temp_x;
        //	delta_temp_py = delta_temp_y;
        /*
            for (i=0;i<white_point_l;i++)
                data[((int)point_ty[i])*320*3 +
           (int)point_tx[i]*3]=255;//dlg->C1_Bool_Data[((int)point_ty[i]+radial_cy)*320
           + (int)point_tx[i]+radial_cx]=1; dlg->m_VideoOCX.Show(dlg->m_Image);				// display capture frame for
           (i=0;i<white_point_l;i++) data[((int)point_ty[i])*320*3 +
           (int)point_tx[i]*3]=0;//dlg->C1_Bool_Data[((int)point_ty[i]+radial_cy)*320 + (int)point_tx[i]+radial_cx]=1;
        */
        //	Sleep(200);
    }
    l--; // 因为出了for循环l已经加了1了
    int min_err_num = l;
    for (i = l; i >= 0; i--)
    {
        if (temp_min_histary[i] < temp_min_histary[min_err_num])
            min_err_num = i;
    }
    min_mis_a = temp_min_mis_a_histary[min_err_num];
    min_mis_x = temp_min_mis_x_histary[min_err_num];
    min_mis_y = temp_min_mis_y_histary[min_err_num];
    // temp_min =temp_min_histary[min_err_num];

    // outa=min_mis_a;
    // outx=min_mis_x;
    // outy=min_mis_y;

    // 注功能代码转到 上一层类内部调用函数 field_line_matching_Gdown2_speed[n]里 便于使用灵活
    //     temp_min_add+=temp_min-temp_min_set;//?????????是否使用线性累加 因为误差增的快减的慢//函数末尾
    //     对temp_min_add累加[定位误差累加] if (temp_min_add<0)
    //		temp_min_add=0;

    // field_line_matching_Gdown_loop_time=l;//记录此次下降用迭代次数

    //
    //
    //
    //
    //
    // 删掉的调整曲率的代码
    //
    //
    //
    //

    l_x = min_mis_x;
    l_y = min_mis_y;
    l_a = min_mis_a;

    RGBTRIPLE ball1;
    ball1.rgbtBlue = 0xFF;
    ball1.rgbtGreen = 0xFF;
    ball1.rgbtRed = 0x00;
    if (pipei2Display == 1)
    {
        for (j = 1; j <= xiandianshu; j++)
        {
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x - 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y - 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x + 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y + 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x + 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y - 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x - 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y + 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y + 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y - 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x - 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x + 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y), ball1);
        }
        //		up_line  =white_point_l*(1-0);//掐头 最大值部分 //掐头  去掉最大误差的几个点    用于在匹配比较好的时候
        // 把查找错误的噪点去掉 		down_line=white_point_l*0;//去位 最小值部分//去尾  去掉最小误差的几个点
        // 对于匹配更有效果 容易走出局部极小点
        for (i = down_line; i < up_line; i++)
        {
            j = wp_dis_num[i];
            ball1.rgbtBlue = 0;
            ball1.rgbtGreen = 0;
            ball1.rgbtRed = 255 * j / white_point_l;

            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x - 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y - 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x + 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y + 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x + 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y - 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x - 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y + 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y + 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y - 1), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x - 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y), ball1);
            SetColor((int)(coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x + 1),
                     (int)(coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y), ball1);
        }
    }
    //	for (j=1;j<=xiandianshu;j++)
    //	{
    //	    SetColor( (int)(coordinate_transfer_x(white_point_a[j] ,white_point_tr[j])+320),
    //				  (int)(coordinate_transfer_y(white_point_a[j] ,white_point_tr[j])+240),ball1);
    //	}

    // ball1.rgbtBlue = 0x00;	ball1.rgbtGreen = 0x00; ball1.rgbtRed = 0x00;
    //	for (x=0;x<640;x++)
    //	for (y=0;y<480;y++)
    //	{	ball1.rgbtGreen = shendubiao[x][y];
    //		SetColor(x,y,ball1);
    //	}

    // 代码接口部分 将坐标转回zuixiaox1  zuixiaoy1  zuixiaojiao
    //	pField->SetAgent(2,(l_x-320)*6+900,(-l_y+240)*6+600,l_a-90);	//右边小场地显示标号
    // 900+zuixiaox1*6=(l_x-320)*6+900
    double zuixiaox1 = (l_x - 320);
    // 600-zuixiaoy1*6=(-l_y+240)*6+600
    double zuixiaoy1 = (l_y - 240);
    // zuixiaojiao-90 =l_a-90
    double zuixiaojiao = l_a;
    if (zuixiaojiao > 180)
        zuixiaojiao -= 360;
    if (zuixiaojiao < -180)
        zuixiaojiao += 360;

    double chang;

    RGBTRIPLE shizi;
    shizi.rgbtBlue = 0x00;
    shizi.rgbtGreen = 0x00;
    shizi.rgbtRed = 0xFF;
    if (pipei2Display == 1)
    {
        DrawFocus(zuixiaox1 + 320, zuixiaoy1 + 240, &shizi);
    }
    jizuobiao(76, 104, &menjiao, &chang, zuixiaox1 + 320, zuixiaoy1 + 240);
    //	pField->SetAgent(2,900+zuixiaox1*6,600-zuixiaoy1*6,zuixiaojiao-90);
    changdi->benjix = 900 + zuixiaox1 * 6;
    changdi->benjiy = 600 - zuixiaoy1 * 6;
    changdi->benjijiao = zuixiaojiao - 90;
    if (changdi->benjijiao > 360)
        changdi->benjijiao -= 360;
    if (changdi->benjijiao < -360)
        changdi->benjijiao += 360;

    double balljiao;
    double ballchang;
    double ballx;
    double bally;
    balljiao = -changdi->ball_jiao - changdi->benjijiao + 90;
    ballchang = quxian[int(changdi->ball_chang)] / cdbl * 2.7; // quxian[int(changdi->ball_chang)];
    zhizuobiao(&ballx, &bally, balljiao, ballchang, changdi->benjix, changdi->benjiy);

    //		pField->SetBall(2,ballx,bally);
    //		pField->SetBall(3,1550,1000);
    changdi->quanbx = ballx;
    changdi->quanby = bally;

    long int juli = 0;
    /*
    for(i=1;i<=xiandianshu;i++)
    {
        juli+=Bilinear02((coordinate_transfer_x(white_point_a[i] + l_a,white_point_tr[i])+l_x),
                         (coordinate_transfer_y(white_point_a[i] + l_a,white_point_tr[i])+l_y));
    }
    //temp_juli+=(double)juli;//记录累加匹配误差(double)juli/(double)xiandianshu;
    //	return (temp_juli);
        return ((float)(juli)/(float)(xiandianshu));
    */
    for (i = down_line; i < up_line; i++)
    {
        j = wp_dis_num[i];
        juli += Bilinear02((coordinate_transfer_x(white_point_a[j] + l_a, white_point_tr[j]) + l_x),
                           (coordinate_transfer_y(white_point_a[j] + l_a, white_point_tr[j]) + l_y));
    }
    return ((float)(juli) / (float)(up_line - down_line));
}

void CVoyVideoDlg::CreateView()
{
}

void CVoyVideoDlg::OnViewClosing(WPARAM wParam, LPARAM lParam)
{
    // AfxMessageBox("1");
    //	m_Camera.m_bDisplay = false;
    //	Invalidate();
}

void CVoyVideoDlg::OnViewMoving(WPARAM wParam, LPARAM lParam)
{
    // 	if (m_dlgView != NULL)
    // 	{
    // 		CRect vrect;
    // 		m_dlgView->GetWindowRect(vrect);
    // 		m_Camera.SetDisView(vrect.left + 15,vrect.top + 30);
    // 	}
    //
}

void CVoyVideoDlg::OnViewKeyDown(WPARAM wParam, LPARAM lParam)
{
}

void CVoyVideoDlg::OnSucball()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    m_cOmniMove.speed[3] = m_nSubSpeed;
    m_cOmniMove.speed[4] = m_nSubSpeedR;

    m_robot.SetFiveMotorsSpeed(m_cOmniMove.speed);
}

void CVoyVideoDlg::OnInfodis()
{
    // TODO: Add your control notification handler code here

    //////////
    // 	short sp[5];
    // 	m_cmd.SynGetMotorPos(2000);
    // 	//Sleep(500);
    // 	m_cmd.SynGetIO(3000);
    // 	//Sleep(500);
    // 	m_cmd.SynGetAD(2000);
    // 	m_cmd.SetFiveMotorsSpeed(sp);
    // 	m_cmd.Kick(1000);
    ////

    if (m_dlgMInfo != NULL)
    {
        delete m_dlgMInfo;
        m_dlgMInfo = NULL;
    }

    m_dlgMInfo = new CDlgMotorInfo(this);
    m_dlgMInfo->Create(IDD_DLG_MOTORINFO, this);

    m_dlgMInfo->m_hParent = this->GetSafeHwnd();
    m_robot.pMotorDis = &(m_dlgMInfo->m_cMotorInfo);
    m_robot.pIODis = &(m_dlgMInfo->m_cIOInfo);
    m_robot.pADDis = &(m_dlgMInfo->m_cADInfo);
    m_robot.CmdShow_Rec = &(m_dlgMInfo->m_cMInfo_List);

    // 	m_cmd.pMotorDis = &(m_dlgMInfo->m_cMotorInfo);
    // 	m_cmd.pIODis = &(m_dlgMInfo->m_cIOInfo);
    // 	m_cmd.pADDis = &(m_dlgMInfo->m_cADInfo);
    // 	m_cmd.CmdShow_Rec = &(m_dlgMInfo->m_cMInfo_List);

    m_dlgMInfo->CenterWindow(this);
    m_dlgMInfo->ShowWindow(SW_SHOW);
}

void CVoyVideoDlg::OnMInfoClosing(WPARAM wParam, LPARAM lParam)
{
    m_robot.pMotorDis = NULL;
    m_robot.pIODis = NULL;
    m_robot.pADDis = NULL;
    m_robot.CmdShow_Rec = NULL;
}

void CVoyVideoDlg::OnNowatchdog()
{
    // TODO: Add your control notification handler code here
    m_robot.SetWatchDog(0);
}

void CVoyVideoDlg::OnShowvideo()
{
    // TODO: Add your control notification handler code here
    m_Camera.CvDisplayVideo(true);
    //	m_Camera.ShowVideoLive(TRUE);
}

void CVoyVideoDlg::OnDebugInfo()
{
    // TODO: Add your control notification handler code here
    if (m_pdlgDebug != NULL)
    {
        delete m_pdlgDebug;
        m_pdlgDebug = NULL;
    }

    m_pdlgDebug = new CDlgDebugInfo(this);
    m_pdlgDebug->Create(IDD_DEBUG_INFO, this);

    m_find.m_find.pList = &(m_pdlgDebug->m_cInfoList);
    m_find.m_find.pEdit = &(m_pdlgDebug->m_cSigInfo);

    m_pdlgDebug->CenterWindow(this);
    m_pdlgDebug->ShowWindow(SW_SHOW);
}

void CVoyVideoDlg::OnShowdbgview()
{
    // TODO: Add your control notification handler code here
    if (m_pdlgDView != NULL)
    {
        delete m_pdlgDView;
        m_pdlgDView = NULL;
    }

    m_pdlgDView = new CDlgDebugView(this);
    m_pdlgDView->Create(IDD_DEBUG_VIEW, this);

    m_pdlgDView->CenterWindow(this);
    m_pdlgDView->ShowWindow(SW_SHOW);
}

void CVoyVideoDlg::OnNetsend()
{
    // TODO: Add your control notification handler code here
    // m_find.m_bNetSend = false;
    CString strTmp;
    strTmp.Format("正在加载端口配置文件：“%s” ...", strCFGFile);
    m_cStatusList.AddString(strTmp);
    // 打开所有端口
    //	int m_nPort_Motor[6];
    // 	m_nPort_Motor[0]	= (int)GetPrivateProfileInt("PORT", "MOTOR_1", 1, strCFGFile );
    // 	m_nPort_Motor[1]	= (int)GetPrivateProfileInt("PORT", "MOTOR_2", 2, strCFGFile );
    // 	m_nPort_Motor[2]	= (int)GetPrivateProfileInt("PORT", "MOTOR_3", 3, strCFGFile );
    // 	m_nPort_Motor[3]	= (int)GetPrivateProfileInt("PORT", "MOTOR_4", 4, strCFGFile );
    // 	m_nPort_Motor[4]	= (int)GetPrivateProfileInt("PORT", "MOTOR_5", 5, strCFGFile );

    // 	int m_nPort_Robot = (int)GetPrivateProfileInt("PORT", "ROBOT", 6, strCFGFile );

    // 	for (int i=0;i<MOTOR_NUM;i++)
    // 	{
    // 		if (m_robot.m_amc[i].Create(m_nPort_Motor[i]) == TRUE)
    // 		{
    // 			strTmp.Format("%d号电机(COM %d)打开成功！",i+1,m_nPort_Motor[i]);
    // 		}
    // 		else
    // 		{
    // 			strTmp.Format("%d号电机(COM %d)打开失败...请检测设置！",i+1,m_nPort_Motor[i]);
    // 			//AfxMessageBox(strTmp);
    // 		}
    // 		m_cStatusList.AddString(strTmp);
    // 		Sleep(200);
    // 	}

    // 	if (m_robot.m_robot.Create(m_nPort_Robot,2000000) == TRUE)
    // 	{
    // 		strTmp.Format("击球器端口(COM %d)打开成功！",m_nPort_Robot);
    // 	}
    // 	else
    // 	{
    // 		strTmp.Format("击球器端口(COM %d)打开失败......请检测设置！",m_nPort_Robot);
    // 		//AfxMessageBox(strTmp);
    // 	}

    m_nPort = (int)GetPrivateProfileInt("PORT", "ROBOT", 6, strCFGFile);
    if (m_robot.Create(m_nPort, 2000000) == TRUE)
    {
        strTmp.Format("机器人端口(COM %d)打开成功！", m_nPort);
    }
    else
    {
        strTmp.Format("机器人端口(COM %d)打开失败......请检测设置！", m_nPort);
        // AfxMessageBox(strTmp);
    }

    m_cStatusList.AddString(strTmp);
    m_cStatusList.SetCurSel(m_cStatusList.GetCount() - 1);

    int m_nMpuPort = (int)GetPrivateProfileInt("PORT", "MPU", 9, strCFGFile);
    if (m_mpu.Create(m_nMpuPort, 1000000) == TRUE)
    {
        strTmp.Format("MPU6050(COM %d)打开成功！", m_nMpuPort);
    }
    else
    {
        strTmp.Format("MPU6050(COM %d)打开失败......请检测设置！", m_nMpuPort);
        AfxMessageBox(strTmp);
    }
    m_cStatusList.AddString(strTmp);
    m_cStatusList.SetCurSel(m_cStatusList.GetCount() - 1);
}

void UPRColor::getchangxian_3(Yuzhi *zz)
{
    //	for()
    xiandianshu = 1;
    RGBTRIPLE ball1;
    ball1.rgbtBlue = 0x00;
    ball1.rgbtGreen = 0x90;
    ball1.rgbtRed = 0xFF;
    double jiao1;
    double chang1;
    double x1;
    double y1;
    /*
    for(int  i=0;i<320;i++)
        for(int j=0;j<240;j++)
        {
            if (hsi_s[i][j]<20  && hsi_i[i][j]>200) //
            {
                changxianx[xiandianshu]=i/4;
                changxiany[xiandianshu]=j/4;
                    xiandianshu++;
            }
        }
    */
    int zdianshu = 500;
    int baiseshu;
    baiseshu = 0;
    int lv1;
    int bai1;
    int xx[640];
    int yy[640];
    int xxh;
    int yyh;
    int nn;
    int xundian;
    int x, y, ra, n;
    int radius_min = 30;
    int radius_max = 220;
    int x_min = 100;
    int x_max = 540;

    int y_min = 0;
    int y_max = 480;
    int x_step = 20;
    int y_step = 20;

    RGBTRIPLE dimian;
    dimian.rgbtBlue = 0x00;
    dimian.rgbtGreen = 0xFF;
    dimian.rgbtRed = 0x00;
    RGBTRIPLE ball;
    ball.rgbtBlue = 0x00;
    ball.rgbtGreen = 0x00;
    ball.rgbtRed = 0xff;
    RGBTRIPLE baise;
    baise.rgbtBlue = 0x00;
    baise.rgbtGreen = 0xff;
    baise.rgbtRed = 0xFF;
    register int volh = 0;
    register int vols = 0;
    register int voli = 0;
    register int i = 0;
    register int j = 0;
    register int k = 0;
    register int l = 0;
    register int s = 0;
    register int t = 0;
    register int r, g, b, H, S, V, Vmin;

    for (n = 0; n < 144; n += 2)
    {
        for (ra = radius_min; ra <= radius_max; ra++)
        {
            x = shexianx[n][ra];
            y = shexiany[n][ra];
            //////////////////////////////////////////////////////////////////////////////////////////////////
            if (zz->bhs > zz->bhx) // 切换红色 跨180度问题
            {
                // 不垮180

                i = x; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][0];//[y编号] [x编号]
                       // [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标]  棋盘格 生成掩膜用掩膜 用来生成
                       // 查找连通域撒种的区域
                j = y; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][1];

                k = (j * OMNI_VISION_CAM_WIDTH + i) * 3;

                b = m_pImageBuf[k];
                g = m_pImageBuf[k + 1];
                r = m_pImageBuf[k + 2];
                //	b = *m_pImageBuf+k;
                //	g = *m_pImageBuf+k + 1;
                //	r = *m_pImageBuf+k + 2;
                if (b > g)
                {
                    if (r > b)
                    {
                        V = r;
                        Vmin = g; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=g//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 360 + (g - b) * 60 / (V - Vmin); // 0*60
                        }
                        else
                            H = 0;
                    }
                    else // V=b   Vmin=?
                    {
                        if (r < g)
                        {
                            V = b;
                            Vmin = r; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=r//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = b;
                            Vmin = g; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=g//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                    }
                }
                else
                {
                    if (r > g)
                    {
                        V = r;
                        Vmin = b; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=b//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 0 + (g - b) * 60 / (V - Vmin); // 0*60
                            if (H < 0)
                            {
                                H = H + 255; // H=H+360;
                            }
                        }
                        else
                            H = 0;
                    }
                    else // V=g   Vmin=?
                    {
                        if (r < b)
                        {
                            V = g;
                            Vmin = r; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=r//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = g;
                            Vmin = b; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=b//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                    }
                }
                S = V - Vmin; //*255/V;    //这里改了 原来是S=(V-Vmin)*255/V
                volh = H;
                vols = S;
                voli = V;
                if (vols <= zz->sw && voli >= zz->iw)
                {
                    erzhi_bai[i][j] = 1;
#ifdef UPRColor_FUNCTION_DSPLAY
                    SetColor(i, j, ball); // baise
#endif
                }

                else if ((volh <= zz->chs && volh >= zz->chx) && vols >= zz->cs && voli <= zz->cis && voli >= zz->cix)
                {
                    erzhi_di[i][j] = 1;
                    erzhi_yuandi[i][j] = 1;
#ifdef UPRColor_FUNCTION_DSPLAY
                    SetColor(i, j, dimian);
#endif
                }
            }
            else
            {

                // int	num1;

                i = x; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][0];//[y编号] [x编号]
                       // [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标]  棋盘格 生成掩膜用掩膜 用来生成
                       // 查找连通域撒种的区域
                j = y; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][1];

                k = (j * OMNI_VISION_CAM_WIDTH + i) * 3;

                b = m_pImageBuf[k];
                g = m_pImageBuf[k + 1];
                r = m_pImageBuf[k + 2];

                if (b > g)
                {
                    if (r > b)
                    {
                        V = r;
                        Vmin = g; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=g//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 360 + (g - b) * 60 / (V - Vmin); // 0*60
                        }
                        else
                            H = 0;
                    }
                    else // V=b   Vmin=?
                    {
                        if (r < g)
                        {
                            V = b;
                            Vmin = r; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=r//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = b;
                            Vmin = g; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=g//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                    }
                }
                else
                {
                    if (r > g)
                    {
                        V = r;
                        Vmin = b; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=b//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 0 + (g - b) * 60 / (V - Vmin); // 0*60
                            if (H < 0)
                            {
                                H = H + 255; // H=H+360;
                            }
                        }
                        else
                            H = 0;
                    }
                    else // V=g   Vmin=?
                    {
                        if (r < b)
                        {
                            V = g;
                            Vmin = r; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=r//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = g;
                            Vmin = b; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=b//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                    }
                }
                S = V - Vmin; //*255/V;    //这里改了 原来是S=(V-Vmin)*255/V
                volh = H;
                vols = S;
                voli = V;
                if (vols <= zz->sw && voli >= zz->iw)
                {
                    erzhi_bai[i][j] = 1;
#ifdef UPRColor_FUNCTION_DSPLAY
                    SetColor(i, j, ball); // baise
#endif
                }

                else if ((volh <= zz->chs && volh >= zz->chx) && vols >= zz->cs && voli <= zz->cis && voli >= zz->cix)
                {
                    erzhi_di[i][j] = 1;
                    erzhi_yuandi[i][j] = 1;
#ifdef UPRColor_FUNCTION_DSPLAY
                    SetColor(i, j, dimian);
#endif
                }
            }
            //////////////////////////////////////////////////////////////////////////////////////////////////
        }
    }

    for (x = x_min; x < x_max; x += x_step)
        for (y = y_min; y < y_max; y++)
        {
            //////////////////////////////////////////////////////////////////////////////////////////////////
            if (zz->bhs > zz->bhx) // 切换红色 跨180度问题
            {
                // 不垮180

                i = x; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][0];//[y编号] [x编号]
                       // [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标]  棋盘格 生成掩膜用掩膜 用来生成
                       // 查找连通域撒种的区域
                j = y; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][1];

                k = (j * OMNI_VISION_CAM_WIDTH + i) * 3;

                b = m_pImageBuf[k];
                g = m_pImageBuf[k + 1];
                r = m_pImageBuf[k + 2];
                //	b = *m_pImageBuf+k;
                //	g = *m_pImageBuf+k + 1;
                //	r = *m_pImageBuf+k + 2;
                if (b > g)
                {
                    if (r > b)
                    {
                        V = r;
                        Vmin = g; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=g//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 360 + (g - b) * 60 / (V - Vmin); // 0*60
                        }
                        else
                            H = 0;
                    }
                    else // V=b   Vmin=?
                    {
                        if (r < g)
                        {
                            V = b;
                            Vmin = r; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=r//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = b;
                            Vmin = g; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=g//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                    }
                }
                else
                {
                    if (r > g)
                    {
                        V = r;
                        Vmin = b; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=b//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 0 + (g - b) * 60 / (V - Vmin); // 0*60
                            if (H < 0)
                            {
                                H = H + 255; // H=H+360;
                            }
                        }
                        else
                            H = 0;
                    }
                    else // V=g   Vmin=?
                    {
                        if (r < b)
                        {
                            V = g;
                            Vmin = r; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=r//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = g;
                            Vmin = b; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=b//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                    }
                }
                S = V - Vmin; //*255/V;    //这里改了 原来是S=(V-Vmin)*255/V
                volh = H;
                vols = S;
                voli = V;
                if (vols <= zz->sw && voli >= zz->iw)
                {
                    erzhi_bai[i][j] = 1;
#ifdef UPRColor_FUNCTION_DSPLAY
                    SetColor(i, j, ball); // baise
#endif
                }

                else if ((volh <= zz->chs && volh >= zz->chx) && vols >= zz->cs && voli <= zz->cis && voli >= zz->cix)
                {
                    erzhi_di[i][j] = 1;
                    erzhi_yuandi[i][j] = 1;
#ifdef UPRColor_FUNCTION_DSPLAY
                    SetColor(i, j, dimian);
#endif
                }
            }
            else
            {

                // int	num1;

                i = x; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][0];//[y编号] [x编号]
                       // [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标]  棋盘格 生成掩膜用掩膜 用来生成
                       // 查找连通域撒种的区域
                j = y; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][1];

                k = (j * OMNI_VISION_CAM_WIDTH + i) * 3;

                b = m_pImageBuf[k];
                g = m_pImageBuf[k + 1];
                r = m_pImageBuf[k + 2];

                if (b > g)
                {
                    if (r > b)
                    {
                        V = r;
                        Vmin = g; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=g//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 360 + (g - b) * 60 / (V - Vmin); // 0*60
                        }
                        else
                            H = 0;
                    }
                    else // V=b   Vmin=?
                    {
                        if (r < g)
                        {
                            V = b;
                            Vmin = r; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=r//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = b;
                            Vmin = g; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=g//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                    }
                }
                else
                {
                    if (r > g)
                    {
                        V = r;
                        Vmin = b; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=b//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 0 + (g - b) * 60 / (V - Vmin); // 0*60
                            if (H < 0)
                            {
                                H = H + 255; // H=H+360;
                            }
                        }
                        else
                            H = 0;
                    }
                    else // V=g   Vmin=?
                    {
                        if (r < b)
                        {
                            V = g;
                            Vmin = r; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=r//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = g;
                            Vmin = b; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=b//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                    }
                }
                S = V - Vmin; //*255/V;    //这里改了 原来是S=(V-Vmin)*255/V
                volh = H;
                vols = S;
                voli = V;
                if (vols <= zz->sw && voli >= zz->iw)
                {
                    erzhi_bai[i][j] = 1;
#ifdef UPRColor_FUNCTION_DSPLAY
                    SetColor(i, j, ball); // baise
#endif
                }

                else if ((volh <= zz->chs && volh >= zz->chx) && vols >= zz->cs && voli <= zz->cis && voli >= zz->cix)
                {
                    erzhi_di[i][j] = 1;
                    erzhi_yuandi[i][j] = 1;
#ifdef UPRColor_FUNCTION_DSPLAY
                    SetColor(i, j, dimian);
#endif
                }
            }
            //////////////////////////////////////////////////////////////////////////////////////////////////
        }

    for (y = y_min; y < y_max; y += y_step)
        for (x = x_min; x < x_max; x++)
        {
            //////////////////////////////////////////////////////////////////////////////////////////////////
            if (zz->bhs > zz->bhx) // 切换红色 跨180度问题
            {
                // 不垮180

                i = x; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][0];//[y编号] [x编号]
                       // [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标]  棋盘格 生成掩膜用掩膜 用来生成
                       // 查找连通域撒种的区域
                j = y; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][1];

                k = (j * OMNI_VISION_CAM_WIDTH + i) * 3;

                b = m_pImageBuf[k];
                g = m_pImageBuf[k + 1];
                r = m_pImageBuf[k + 2];
                //	b = *m_pImageBuf+k;
                //	g = *m_pImageBuf+k + 1;
                //	r = *m_pImageBuf+k + 2;
                if (b > g)
                {
                    if (r > b)
                    {
                        V = r;
                        Vmin = g; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=g//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 360 + (g - b) * 60 / (V - Vmin); // 0*60
                        }
                        else
                            H = 0;
                    }
                    else // V=b   Vmin=?
                    {
                        if (r < g)
                        {
                            V = b;
                            Vmin = r; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=r//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = b;
                            Vmin = g; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=g//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                    }
                }
                else
                {
                    if (r > g)
                    {
                        V = r;
                        Vmin = b; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=b//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 0 + (g - b) * 60 / (V - Vmin); // 0*60
                            if (H < 0)
                            {
                                H = H + 255; // H=H+360;
                            }
                        }
                        else
                            H = 0;
                    }
                    else // V=g   Vmin=?
                    {
                        if (r < b)
                        {
                            V = g;
                            Vmin = r; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=r//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = g;
                            Vmin = b; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=b//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                    }
                }
                S = V - Vmin; //*255/V;    //这里改了 原来是S=(V-Vmin)*255/V
                volh = H;
                vols = S;
                voli = V;
                if (vols <= zz->sw && voli >= zz->iw)
                {
                    erzhi_bai[i][j] = 1;
#ifdef UPRColor_FUNCTION_DSPLAY
                    SetColor(i, j, ball); // baise
#endif
                }

                else if ((volh <= zz->chs && volh >= zz->chx) && vols >= zz->cs && voli <= zz->cis && voli >= zz->cix)
                {
                    erzhi_di[i][j] = 1;
                    erzhi_yuandi[i][j] = 1;
#ifdef UPRColor_FUNCTION_DSPLAY
                    SetColor(i, j, dimian);
#endif
                }
            }
            else
            {

                // int	num1;

                i = x; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][0];//[y编号] [x编号]
                       // [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标]  棋盘格 生成掩膜用掩膜 用来生成
                       // 查找连通域撒种的区域
                j = y; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][1];

                k = (j * OMNI_VISION_CAM_WIDTH + i) * 3;

                b = m_pImageBuf[k];
                g = m_pImageBuf[k + 1];
                r = m_pImageBuf[k + 2];

                if (b > g)
                {
                    if (r > b)
                    {
                        V = r;
                        Vmin = g; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=g//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 360 + (g - b) * 60 / (V - Vmin); // 0*60
                        }
                        else
                            H = 0;
                    }
                    else // V=b   Vmin=?
                    {
                        if (r < g)
                        {
                            V = b;
                            Vmin = r; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=r//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = b;
                            Vmin = g; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=g//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                    }
                }
                else
                {
                    if (r > g)
                    {
                        V = r;
                        Vmin = b; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=b//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 0 + (g - b) * 60 / (V - Vmin); // 0*60
                            if (H < 0)
                            {
                                H = H + 255; // H=H+360;
                            }
                        }
                        else
                            H = 0;
                    }
                    else // V=g   Vmin=?
                    {
                        if (r < b)
                        {
                            V = g;
                            Vmin = r; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=r//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = g;
                            Vmin = b; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=b//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                    }
                }
                S = V - Vmin; //*255/V;    //这里改了 原来是S=(V-Vmin)*255/V
                volh = H;
                vols = S;
                voli = V;
                if (vols <= zz->sw && voli >= zz->iw)
                {
                    erzhi_bai[i][j] = 1;
#ifdef UPRColor_FUNCTION_DSPLAY
                    SetColor(i, j, ball); // baise
#endif
                }

                else if ((volh <= zz->chs && volh >= zz->chx) && vols >= zz->cs && voli <= zz->cis && voli >= zz->cix)
                {
                    erzhi_di[i][j] = 1;
                    erzhi_yuandi[i][j] = 1;
#ifdef UPRColor_FUNCTION_DSPLAY
                    SetColor(i, j, dimian);
#endif
                }
            }
            //////////////////////////////////////////////////////////////////////////////////////////////////
        }

    int max_W_width = 20;
    int tx, ty;
    xiandianshu = 0;

    ball1.rgbtBlue = 0;
    ball1.rgbtGreen = 255;
    ball1.rgbtRed = 255;
    for (s = 0; s < 144; s += 2)
    {
        xxh = 0;
        yyh = 0;
        lv1 = 0;
        bai1 = 0;
        baiseshu = 0;
        nn = 0;
        for (r = radius_min; r <= radius_max; r++)
        {
            //	if(erzhi_di[shexianx[x][y]][shexiany[x][y]]==1)
            //	{
            tx = shexianx[s][r];
            ty = shexiany[s][r];
            if (lv1 == 0)
                if (erzhi_yuandi[tx][ty] == 1)
                    // 如果射线是绿色
                    lv1 = 1;
            if (lv1 == 1)
                if (erzhi_bai[tx][ty] == 1)
                {
                    // 如果射线是绿色后又是白色
                    bai1 = 1;
                    baiseshu++;

                    //	xx[nn]=s;
                    yy[nn] = r;
                    nn++;
                }
            if (bai1 == 1 && baiseshu < max_W_width)
                if (erzhi_yuandi[tx][ty] == 1)
                {
                    //	SetColor(huanyuanx[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,huanyuany[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,ball1);
                    lv1 = 0;
                    bai1 = 0;
                    for (xundian = 0; xundian < nn; xundian++)
                    {
                        //	xxh=xxh+xx[xundian];
                        yyh = yyh + yy[xundian];
                    }
                    //	xxh=xxh/nn;
                    xxh = s;
                    yyh = (float)(yyh) / (float)(nn);
                    baiseshu = 0;
                    //	jizuobiao(shexianx[xxh][yyh],shexiany[xxh][yyh],&jiao1,&chang1,320,240);
                    jiao1 = s * 360.0 / 144.0 + 180.0;
                    if (jiao1 > 180)
                        jiao1 -= 360;
                    if (jiao1 < -180)
                        jiao1 += 360;
                    chang1 = yyh;
                    SetColor(shexianx[s][(int)chang1], shexiany[s][(int)chang1], ball1);
                    SetColor(shexianx[s][(int)chang1] + 2, shexiany[s][(int)chang1], ball1);
                    SetColor(shexianx[s][(int)chang1] + 1, shexiany[s][(int)chang1], ball1);
                    SetColor(shexianx[s][(int)chang1], shexiany[s][(int)chang1] + 1, ball1);
                    SetColor(shexianx[s][(int)chang1], shexiany[s][(int)chang1] + 2, ball1);
                    SetColor(shexianx[s][(int)chang1] - 1, shexiany[s][(int)chang1], ball1);
                    SetColor(shexianx[s][(int)chang1] - 2, shexiany[s][(int)chang1], ball1);
                    SetColor(shexianx[s][(int)chang1], shexiany[s][(int)chang1] - 1, ball1);
                    SetColor(shexianx[s][(int)chang1], shexiany[s][(int)chang1] - 2, ball1);
                    float rin = chang1;
                    int rt1 = rin;
                    int rt2;
                    if ((rin - rt1) >= 0) // 注意 windows取整基本只是吧小数点抹了 缩小取整
                        rt2 = rt1 + 1;
                    else
                        rt2 = rt1 - 1;
                    float rout = (quxian[rt2] - quxian[rt1]) * (rin - rt1) / (rt2 - rt1) + quxian[rt1];
                    chang1 = rout * 0.45 / cdbl; // double(quxian[int(chang1)])*0.49/cdbl;
                    //	chang1=double(quxian[int(chang1)])*0.45/cdbl;
                    zhizuobiao(&x1, &y1, jiao1, chang1, 320, 240);
                    changxianx[xiandianshu] = float(x1);
                    changxiany[xiandianshu] = float(y1);
                    xiandianshu++;
                    lv1 = 0;
                    bai1 = 0;
                    nn = 0;
                    xxh = 0;
                    yyh = 0;
                }
            //}
        }
    }

    //******************************************************************

    //	for( x=x_min;x<x_max;x+=x_step)
    //	int x,y;
    xiandianshuT = 0;
    for (x = x_min; x < x_max; x += x_step)
    {
        xxh = 0;
        yyh = 0;
        lv1 = 0;
        bai1 = 0;
        baiseshu = 0;
        nn = 0;
        for (y = 20; y <= 460; y++)
        {
            if (banjing[x][y] < 40000)
            {
                //				SetColor(x,y,ball1);
                if (lv1 == 0)
                    if (erzhi_yuandi[x][y] == 1)
                        // 如果射线是绿色
                        lv1 = 1;
                if (lv1 == 1)
                    if (erzhi_bai[x][y] == 1)
                    {
                        // 如果射线是绿色后又是白色
                        bai1 = 1;
                        baiseshu++;

                        xx[nn] = x;
                        yy[nn] = y;
                        nn++;
                    }
                if (bai1 == 1 && baiseshu < max_W_width)
                    if (erzhi_yuandi[x][y] == 1)
                    {
                        //	SetColor(huanyuanx[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,huanyuany[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,ball1);
                        lv1 = 0;
                        bai1 = 0;
                        for (xundian = 0; xundian < nn; xundian++)
                        {
                            xxh = xxh + xx[xundian];
                            yyh = yyh + yy[xundian];
                        }
                        xxh = xxh / nn;
                        yyh = yyh / nn;
                        baiseshu = 0;
                        jizuobiao(xxh, yyh, &jiao1, &chang1, 320, 240);
                        SetColor(xxh, yyh, ball1);
                        SetColor(xxh + 2, yyh, ball1);
                        SetColor(xxh + 1, yyh, ball1);
                        SetColor(xxh, yyh + 1, ball1);
                        SetColor(xxh, yyh + 2, ball1);
                        SetColor(xxh - 1, yyh, ball1);
                        SetColor(xxh - 2, yyh, ball1);
                        SetColor(xxh, yyh - 1, ball1);
                        SetColor(xxh, yyh - 2, ball1);
                        float rin = chang1;
                        int rt1 = rin;
                        int rt2;
                        if ((rin - rt1) >= 0) // 注意 windows取整基本只是吧小数点抹了 缩小取整
                            rt2 = rt1 + 1;
                        else
                            rt2 = rt1 - 1;
                        float rout = (quxian[rt2] - quxian[rt1]) * (rin - rt1) / (rt2 - rt1) + quxian[rt1];
                        chang1 = rout * 0.45 / cdbl; // double(quxian[int(chang1)])*0.49/cdbl;
                        //								chang1=double(quxian[int(chang1)])*0.45/cdbl;
                        zhizuobiao(&x1, &y1, jiao1, chang1, 320, 240);
                        changxianx[xiandianshu] = float(x1);
                        //	changxianx[xiandianshuT]=float(x1);
                        changxiany[xiandianshu] = float(y1);
                        //	changxiany[xiandianshuT]=float(y1);
                        xiandianshu++;
                        lv1 = 0;
                        bai1 = 0;
                        nn = 0;
                        xxh = 0;
                        yyh = 0;
                    }
            }
        }
    }

    //******************************************************************

    //	for(y=y_min;y<y_max;y+=y_step)
    //******************************************************************
    for (y = y_min; y < y_max; y += y_step)
    {
        xxh = 0;
        yyh = 0;
        lv1 = 0;
        bai1 = 0;
        baiseshu = 0;
        nn = 0;
        for (x = 80; x <= 560; x++)
        {
            if (banjing[x][y] < 40000)
            {
                //	SetColor(x,y,ball1);
                if (lv1 == 0)
                    if (erzhi_yuandi[x][y] == 1)
                        // 如果射线是绿色
                        lv1 = 1;
                if (lv1 == 1)
                    if (erzhi_bai[x][y] == 1)
                    {
                        // 如果射线是绿色后又是白色
                        bai1 = 1;
                        baiseshu++;

                        xx[nn] = x;
                        yy[nn] = y;
                        nn++;
                    }
                if (bai1 == 1 && baiseshu < max_W_width)
                    if (erzhi_yuandi[x][y] == 1)
                    {
                        //	SetColor(huanyuanx[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,huanyuany[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,ball1);
                        lv1 = 0;
                        bai1 = 0;
                        for (xundian = 0; xundian < nn; xundian++)
                        {
                            xxh = xxh + xx[xundian];
                            yyh = yyh + yy[xundian];
                        }
                        xxh = xxh / nn;
                        yyh = yyh / nn;
                        baiseshu = 0;
                        jizuobiao(xxh, yyh, &jiao1, &chang1, 320, 240);
                        SetColor(xxh, yyh, ball1);
                        SetColor(xxh + 2, yyh, ball1);
                        SetColor(xxh + 1, yyh, ball1);
                        SetColor(xxh, yyh + 1, ball1);
                        SetColor(xxh, yyh + 2, ball1);
                        SetColor(xxh - 1, yyh, ball1);
                        SetColor(xxh - 2, yyh, ball1);
                        SetColor(xxh, yyh - 1, ball1);
                        SetColor(xxh, yyh - 2, ball1);
                        float rin = chang1;
                        int rt1 = rin;
                        int rt2;
                        if ((rin - rt1) >= 0) // 注意 windows取整基本只是吧小数点抹了 缩小取整
                            rt2 = rt1 + 1;
                        else
                            rt2 = rt1 - 1;
                        float rout = (quxian[rt2] - quxian[rt1]) * (rin - rt1) / (rt2 - rt1) + quxian[rt1];
                        chang1 = rout * 0.45 / cdbl; // double(quxian[int(chang1)])*0.49/cdbl;
                        //								chang1=double(quxian[int(chang1)])*0.45/cdbl;
                        zhizuobiao(&x1, &y1, jiao1, chang1, 320, 240);
                        changxianx[xiandianshu] = float(x1);
                        changxiany[xiandianshu] = float(y1);
                        xiandianshu++;
                        lv1 = 0;
                        bai1 = 0;
                        nn = 0;
                        xxh = 0;
                        yyh = 0;
                    }
            }
        }
    }

    /*	float zjl;
        if(xiandianshu>zdianshu)
        {
            zjl=xiandianshu/zdianshu;
            for(x=0;x<=zdianshu;x++)
            {
                changxianx[x]=changxianx[int(x*zjl)];
                changxiany[x]=changxiany[int(x*zjl)];
                SetColor(changxianx[int(x*zjl)],changxiany[int(x*zjl)],ball1);
                SetColor(changxianx[int(x*zjl)]+1,changxiany[int(x*zjl)],ball1);
                SetColor(changxianx[int(x*zjl)],changxiany[int(x*zjl)]+1,ball1);
                SetColor(changxianx[int(x*zjl)]-1,changxiany[int(x*zjl)],ball1);
                SetColor(changxianx[int(x*zjl)],changxiany[int(x*zjl)]-1,ball1);
            }
            xiandianshu=zdianshu;
        }
    */
    //******************************************************************

    // changxian[i/5][j/5]=1;
    //	SetColor(i/4,j/4,ball1);
    //	}
    //	else
    //	changxian[i][j]=0;
    /*
    double jiao;
    double chang;
    double xuanzhuan=100;
        for( i=1;i<=xiandianshu;i++)
    {
    jizuobiao(changxianx[i],changxiany[i],&jiao,&chang,40,30);
    jiao+=xuanzhuan;
    zhizuobiao(&changxianx[i],&changxiany[i],jiao,chang,40,30);
    }
    */
    //	for(int i=1;i<=xiandianshu;i++)
    // SetColor(changxianx[i]+30,changxiany[i]+30,ball1);
    /*
    double l_x_t= (zz->benjix  -900)/6+320;
    double l_y_t=-(zz->benjiy  -600)/6+240;
    double l_a_t=angle_add_transfer2(zz->benjijiao,90);
    double tempr=0;
    for(i=0;i<changxianxT;i++)
    {
        tempr=coordinate_transfer_radius(changxianx[i]-320,changxiany[i]-240);
        if ((tempr<1200)&&(tempr>10))
        {
            white_point_a [j]=coordinate_transfer_angle(changxianx[i]-320,changxiany[i]-240);
            white_point_tr[j]=tempr;
            j++;
        }
        for(i=0;i<changxianxT;i++)
        {
            temp+=Bilinear02((coordinate_transfer_x(white_point_a[i] + try_angle,white_point_tr[i])+try_x),
                (coordinate_transfer_y(white_point_a[i] + try_angle,white_point_tr[i])+try_y));
        }


    }
    */
}

void CVoyVideoDlg::EmuCamera()
{
    CString strCamName;
    char szDeviceList[20][40];
    int iDeviceCount;

    iDeviceCount = IC_ListDevices((char *)szDeviceList, 40);
    for (int i = 0; i < iDeviceCount; i++)
    {
        // printf("%2d. %s\n",i+1,szDeviceList[i]);
        strCamName.Format("%d-%s", i + 1, szDeviceList[i]);
        m_cOverlookCombo.AddString(strCamName);
        m_cOverlookCombo.SetCurSel(0);
    }
}

void CVoyVideoDlg::OnAmcEn()
{
    // TODO: Add your control notification handler code here
    // m_robot.BridgeOnAll();
}

void CVoyVideoDlg::OnAmcBrake()
{
    // TODO: Add your control notification handler code here
    // 	m_robot.Brake();
    // 	for (int i=0;i<MOTOR_NUM;i++)
    // 	{
    // 		m_cOmniMove.speed[i] = 0;
    // 	}
}

void CVoyVideoDlg::OnAmcDisen()
{
    // TODO: Add your control notification handler code here
    // m_robot.BridgeOffAll();
}

void CVoyVideoDlg::OnCamSetOffset()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    m_Camera.m_nOffSetX = m_nCamOffsetX;
    m_Camera.m_nOffSetY = m_nCamOffsetY;

    m_cfg.nCameraOffsetX = m_nCamOffsetX;
    m_cfg.nCameraOffsetY = m_nCamOffsetY;
}

void CVoyVideoDlg::OnSetLiftpos()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    m_robot.SetLiftPos(m_nLiftPos);
    m_robot.SendMotorValAgain();
}

void CVoyVideoDlg::OnOpenMpuPort()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    CString strTmp;
    if (m_mpu.Create(m_nMpuPort, 1000000) == TRUE)
    {
        strTmp.Format("MPU6050(COM %d)打开成功！", m_nMpuPort);
    }
    else
    {
        strTmp.Format("MPU6050(COM %d)打开失败......请检测设置！", m_nMpuPort);
        AfxMessageBox(strTmp);
    }
    m_cStatusList.AddString(strTmp);
    m_cStatusList.SetCurSel(m_cStatusList.GetCount() - 1);
}

void CVoyVideoDlg::OnField()
{
    // TODO: Add your control notification handler code here
}

void CVoyVideoDlg::OnGkSave()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    m_robot.GKSave(m_nGKSaveVal);
}
