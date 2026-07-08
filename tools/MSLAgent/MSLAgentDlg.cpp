// MSLAgentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MSLAgent.h"
#include "MSLAgentDlg.h"
#include "MSL_Protocol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
static int nRemotePort = 20090;
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
// CMSLAgentDlg dialog

CMSLAgentDlg::CMSLAgentDlg(CWnd *pParent /*=NULL*/) : CDialog(CMSLAgentDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CMSLAgentDlg)
    m_nBallx = 900;
    m_nBally = 500;
    m_nID = 1;
    m_nSelfy = -50;
    m_strIP = _T("127.0.0.1");
    m_nPort = 20090;
    m_nSelfx = 100;
    m_nMballx = 0;
    m_nMbally = 0;
    m_bAddAg1 = TRUE;
    m_bAddAg2 = TRUE;
    m_bAddAg3 = TRUE;
    m_bAddAg4 = FALSE;
    m_bAddAg5 = FALSE;
    m_nVirtualID1 = 2;
    m_nVirtualID2 = 3;
    m_nVirtualID3 = 2;
    m_nVirtualID4 = 3;
    m_nIDWhoGetBall = 0;
    m_bAtkIm = TRUE;
    m_nObstID = 1;
    m_nCycle = 15;
    m_nObstIDToRemove = 0;
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMSLAgentDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CMSLAgentDlg)
    DDX_Control(pDX, IDC_NUMDIS, m_cNumDis);
    DDX_Control(pDX, IDC_BKFIELD, m_nFullField);
    DDX_Control(pDX, IDC_LIST2, m_cAlist);
    DDX_Control(pDX, IDC_LIST1, m_cList);
    DDX_Text(pDX, IDC_BALLX, m_nBallx);
    DDX_Text(pDX, IDC_BALLY, m_nBally);
    DDX_Text(pDX, IDC_ID, m_nID);
    DDX_Text(pDX, IDC_SELFY, m_nSelfy);
    DDX_Text(pDX, IDC_IP, m_strIP);
    DDX_Text(pDX, IDC_PORT, m_nPort);
    DDX_Text(pDX, IDC_SELFX, m_nSelfx);
    DDX_Text(pDX, IDC_MBALLX, m_nMballx);
    DDX_Text(pDX, IDC_MBALLY, m_nMbally);
    DDX_Check(pDX, IDC_ADDAG1, m_bAddAg1);
    DDX_Check(pDX, IDC_ADDAG2, m_bAddAg2);
    DDX_Check(pDX, IDC_ADDAG3, m_bAddAg3);
    DDX_Check(pDX, IDC_ADDAG4, m_bAddAg4);
    DDX_Check(pDX, IDC_ADDAG5, m_bAddAg5);
    DDX_Text(pDX, IDC_VIRTUALID1, m_nVirtualID1);
    DDX_Text(pDX, IDC_VIRTUALID2, m_nVirtualID2);
    DDX_Text(pDX, IDC_VIRTUALID3, m_nVirtualID3);
    DDX_Text(pDX, IDC_VIRTUALID4, m_nVirtualID4);
    DDX_Text(pDX, IDC_CBALLID, m_nIDWhoGetBall);
    DDX_Check(pDX, IDC_ATKIM, m_bAtkIm);
    DDX_Text(pDX, IDC_OBSTOWN, m_nObstID);
    DDX_Text(pDX, IDC_CYCLE, m_nCycle);
    DDX_Text(pDX, IDC_OBST_ID_TOREMOVE, m_nObstIDToRemove);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMSLAgentDlg, CDialog)
//{{AFX_MSG_MAP(CMSLAgentDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_CONNECT, OnConnect)
ON_BN_CLICKED(IDC_SEND, OnSend)
ON_BN_CLICKED(IDC_COMBO, OnCombo)
ON_WM_TIMER()
ON_BN_CLICKED(IDC_STOP, OnStop)
ON_BN_CLICKED(IDC_MULTIAGEBT, OnMultiagebt)
ON_BN_CLICKED(IDC_UPDATEBALL, OnUpdateball)
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_CNTNSND, OnCntnsnd)
ON_BN_CLICKED(IDC_CTRLALLBALL, OnCtrlallball)
ON_BN_CLICKED(IDC_ADDAG1, OnAddag1)
ON_BN_CLICKED(IDC_ADDAG2, OnAddag2)
ON_BN_CLICKED(IDC_ADDAG3, OnAddag3)
ON_BN_CLICKED(IDC_ADDAG4, OnAddag4)
ON_BN_CLICKED(IDC_ADDAG5, OnAddag5)
ON_BN_CLICKED(IDC_VIRTUALATK1, OnVirtualatk1)
ON_BN_CLICKED(IDC_VIRTUALATK2, OnVirtualatk2)
ON_BN_CLICKED(IDC_VIRTUALKOP, OnVirtualkop)
ON_BN_CLICKED(IDC_VIRTUALKOS, OnVirtualkos)
ON_BN_CLICKED(IDC_CANCELVIR1, OnCancelvir1)
ON_BN_CLICKED(IDC_CANCELVIR2, OnCancelvir2)
ON_BN_CLICKED(IDC_CANCELVIR3, OnCancelvir3)
ON_BN_CLICKED(IDC_CANCELVIR4, OnCancelvir4)
ON_BN_CLICKED(IDC_VIRTUALGK, OnVirtualgk)
ON_BN_CLICKED(IDC_BALLMISSING, OnBallmissing)
ON_BN_CLICKED(IDC_AGGETBALL, OnAggetball)
ON_BN_CLICKED(IDC_ATKIM, OnAtkim)
ON_WM_MOUSEWHEEL()
ON_BN_CLICKED(IDC_ADDOBST, OnAddobst)
ON_BN_CLICKED(IDC_NOADDOBST, OnNoaddobst)
ON_BN_CLICKED(IDC_SETTRUEOBST, OnSettrueobst)
ON_BN_CLICKED(IDC_ERR, OnErr)
ON_BN_CLICKED(IDC_REMOVEOBST, OnRemoveobst)
ON_BN_CLICKED(IDC_OBST_BUST, OnObstBust)
ON_BN_CLICKED(IDC_CLEAR_LIST, OnClearList)
ON_BN_CLICKED(IDC_TOCAPTAIN, OnTocaptain)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMSLAgentDlg message handlers

#define LVS_EX_DOUBLEBUFFER 0x00010000
BOOL CMSLAgentDlg::OnInitDialog()
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
    m_nFullField.pnumdis = &m_cNumDis;

    // agents
    m_bmulti = false;
    m_cAlist.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    CString strname[8];
    strname[0] = "ID";
    strname[1] = "status";
    strname[2] = "targetx";
    strname[3] = "targety";
    strname[4] = "t_angel";
    strname[5] = "rb_x";
    strname[6] = "rb_y";
    strname[7] = "bField";

    for (int i = 1; i < 7; i++)
    {
        m_nFullField.m_AgUDPs[i].SetID(i);
        m_nFullField.m_AgToCap[i].SetID(i);
    }

    for (i = 0; i < 8; i++)
    {
        m_cAlist.InsertColumn(i, strname[i], LVCFMT_LEFT, 60);
    }

    CString str;
    for (i = 1; i < 8; i++)
    {
        str.Format("%d", i);
        m_cAlist.InsertItem(i - 1, str);
    }

    srand(GetTickCount());

    m_bAllBalls = true;

    for (i = 0; i < 7; i++)
    {
        m_nFullField.m_AgUDPs[i].pInfoList = &m_cList;
        m_nFullField.m_AgToCap[i].pInfoList = &m_cList;
    }

    return TRUE; // return TRUE  unless you set the focus to a control
}

void CMSLAgentDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMSLAgentDlg::OnPaint()
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
HCURSOR CMSLAgentDlg::OnQueryDragIcon()
{
    return (HCURSOR)m_hIcon;
}

void CMSLAgentDlg::OnConnect()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    // m_agentnet.ConnectTo(m_strIP,m_nPort);
    m_agUDP.SetTarget(m_strIP, m_nPort);
    m_agUDP.SetCoachIP(m_strIP);
}

void CMSLAgentDlg::OnSend()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    //////////////////////////////////////////////////////////////////////////
    // 	m_status.ID = m_nID;
    // 	m_status.status = m_agentnet.m_ctrl.ctrl;
    // 	m_status.foundball = true;
    // 	m_status.ball_x = m_nBallx;
    // 	m_status.ball_y = m_nBally;
    // 	m_status.selfx = m_nSelfx;
    // 	//m_status.selfx = m_nID*100;
    // 	m_status.selfy = m_nSelfy;
    // 	m_status.angle = 0;

    //	m_agentnet.Send(((char*)&m_status),sizeof(m_status));
    //////////////////////////////////////////////////////////////////////////

    m_agUDP.SetID(m_nID);
    m_agUDP.Report(m_nSelfx, m_nSelfy, 0, CTRL_STOP, true, m_nBallx, m_nBally);
}

void CMSLAgentDlg::OnCombo()
{
    // TODO: Add your control notification handler code here
    SetTimer(0, 50, NULL);
}

void CMSLAgentDlg::OnTimer(UINT nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    UINT time;
    int i;
    switch (nIDEvent)
    {
    case 0:
        // 单体
        OnSend();

        time = (UINT)((double)rand() / RAND_MAX) * 30 + 20;
        SetTimer(0, time, NULL);
        break;

    case 1:
        // 多机
        // OnMultiagebt();//显示接收到的教练控制指令
        if (m_bmulti == true)
        {
            CString str;

            static int nDisplayCnt = 10;

            if (nDisplayCnt <= 0)
            {
                for (i = 1; i < 7; i++)
                {
                    // 			str.Format("%d",i);
                    // 			m_cAlist.SetItemText(i-1,1,str);
                    str = GetCtrl(m_nFullField.m_AgUDPs[i].rec_ctrl);
                    m_cAlist.SetItemText(i - 1, 1, str);
                    str.Format("%d", m_nFullField.m_AgUDPs[i].rec_targetx);
                    m_cAlist.SetItemText(i - 1, 2, str);
                    str.Format("%d", m_nFullField.m_AgUDPs[i].rec_targety);
                    m_cAlist.SetItemText(i - 1, 3, str);
                    str.Format("%d", m_nFullField.m_AgUDPs[i].rec_targetang);
                    m_cAlist.SetItemText(i - 1, 4, str);
                    str.Format("%d", m_nFullField.m_AgUDPs[i].re_ballx);
                    m_cAlist.SetItemText(i - 1, 5, str);
                    str.Format("%d", m_nFullField.m_AgUDPs[i].re_bally);
                    m_cAlist.SetItemText(i - 1, 6, str);
                    if (m_nFullField.m_AgUDPs[i].rec_bField == true)
                    {
                        str.Format("true");
                    }
                    else
                    {
                        str.Format("false");
                    }
                    m_cAlist.SetItemText(i - 1, 7, str);
                    nDisplayCnt = 10;
                }
            }
            nDisplayCnt--;
        }

        m_nFullField.Exec();

        break;

    case 2: // 电池 cpu 机体信息
        int nCPULoad = m_cpu_battery.nCpuUsage;
        float fCPUFrequency = m_cpu_battery.fCpuFrequency;
        BYTE nRobPow = 70;
        BYTE nRobCharg = 10;
        int mcFPS = 60;
        float fSoftwareVer = 1.12;
        for (int i = 1; i < 6; i++)
        {
            // 电池
            m_nFullField.m_AgUDPs[i].SetBatteryStatus(
                m_cpu_battery.lpPwrStatus.ACLineStatus, m_cpu_battery.lpPwrStatus.BatteryFlag,
                m_cpu_battery.lpPwrStatus.BatteryLifePercent, m_cpu_battery.lpPwrStatus.BatteryLifeTime);
            m_cpu_battery.lpPwrStatus.BatteryLifePercent++;
            m_cpu_battery.lpPwrStatus.BatteryLifeTime++;

            // CPU
            m_nFullField.m_AgUDPs[i].SetCPUStatus(nCPULoad, fCPUFrequency);
            nCPULoad++;
            fCPUFrequency++;

            // 机体信息
            m_nFullField.m_AgUDPs[i].SetRobotStatus(nRobPow, nRobCharg);
            nRobPow++;
            nRobCharg++;

            // 串口帧率
            m_nFullField.m_AgUDPs[i].SetSerialPortFPS(50 + i);

            // 视觉帧率
            m_nFullField.m_AgUDPs[i].SetVisionFPS(80 + i);

            // 组播帧率
            m_nFullField.m_AgUDPs[i].SetMulticastFPS(mcFPS);
            mcFPS++;

            // 姿态模块帧率
            m_nFullField.m_AgUDPs[i].SetAttitudeFPS(100 + i);

            // 软件版本
            m_nFullField.m_AgUDPs[i].SetSoftwareVer(fSoftwareVer);
            fSoftwareVer += 0.1;
        }
        break;
    }

    CDialog::OnTimer(nIDEvent);
}

void CMSLAgentDlg::OnStop()
{
    // TODO: Add your control notification handler code here
    KillTimer(0);
    Sleep(100);
}

void CMSLAgentDlg::OnMultiagebt()
{
    // TODO: Add your control notification handler code here
    //////////////////////////////////////////////////////////////////////////
    // 	UpdateData();
    // 	for (int i=1;i<6;i++)
    // 	{
    // 		m_nFullField.m_AgUDPs[i].SetTarget(m_strIP,m_nPort);
    // 	}
    //////////////////////////////////////////////////////////////////////////

    for (int i = 1; i < 6; i++)
    {
        m_nFullField.m_AgUDPs[i].Close();
        m_nFullField.m_AgUDPs[i].SetID(i);
        m_nFullField.m_AgUDPs[i].SetTarget(m_strIP, 20090);
        m_nFullField.m_AgUDPs[i].SetCoachIP(m_strIP);
    }
}

CString CMSLAgentDlg::GetCtrl(UINT inctrl)
{
    CString strC;
    switch (inctrl)
    {
    case CTRL_ATTACK:
        strC.Format("进攻");
        break;
    case CTRL_STOP:
        strC.Format("停止");
        break;
    case CTRL_MOVETO:
        strC.Format("移动");
        break;

    case CTRL_GOALKEEP:
        strC.Format("守门员");
        break;

    case CTRL_PRODEF:
        strC.Format("积极防守");
        break;

    case CTRL_ATTENTIONBALL:
        strC.Format("球观察员");
        break;

    case CTRL_PASS:
        strC.Format("传球");
        break;

    case CTRL_CATCH:
        strC.Format("接球");
        break;

    // 跑位
    case CTRL_KICKOFF_PRIM_READY:
        strC.Format("主导开球跑位");
        break;

    case CTRL_KICKOFF_SLAVE_READY:
        strC.Format("辅助开球跑位");
        break;

    case CTRL_KICKOFF_PRIM:
        strC.Format("主导开球");
        break;

    case CTRL_KICKOFF_SLAVE:
        strC.Format("辅助开球");
        break;

    case CTRL_PASS_MOVE:
        strC.Format("动态传球");
        break;
    case CTRL_CATCH_MOVE:
        strC.Format("接球跑位");
        break;
    case CTRL_CATCH_FOCUS:
        strC.Format("接球ing");
        break;

    case CTRL_DEF_BALL:
        strC.Format("防传球");
        break;
    case CTRL_DEF_GOAL:
        strC.Format("防射门");
        break;
    default:
        strC.Format("未知");
        break;
    }
    return strC;
}

void CMSLAgentDlg::OnUpdateball()
{
    // TODO: Add your control notification handler code here
    UpdateData();
}

void CMSLAgentDlg::OnClose()
{
    // TODO: Add your message handler code here and/or call default
    OnStop();
    CDialog::OnClose();
}

void CMSLAgentDlg::OnCntnsnd()
{
    // TODO: Add your control notification handler code here
}

void CMSLAgentDlg::OnCtrlallball()
{
    // TODO: Add your control notification handler code here
    if (m_nFullField.m_bAllBall == true)
    {
        m_nFullField.m_bAllBall = false;
    }
    else
    {
        m_nFullField.m_bAllBall = true;
    }
}

BOOL CMSLAgentDlg::PreTranslateMessage(MSG *pMsg)
{
    // TODO: Add your specialized code here and/or call the base class

    if (pMsg->message == WM_KEYDOWN) // 判断是否为按键消息
    {

        switch (pMsg->wParam) // 根据虚拟键表值来判断按下的是什么键
        {
        case VK_UP: // 按下向上键
            break;

        case VK_DOWN: // 按下向下键
            break;

        case VK_LEFT: // 按下向左键
            break;

        case VK_RIGHT: // 按下向右键
            break;

        case VK_SHIFT:
            break;

        case VK_CONTROL:
            // AfxMessageBox("ctrl down!");
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
            break;

        case 88: // x		放大
            break;

        default:
            break;
        }
    }

    if (pMsg->message == WM_KEYUP) // 判断是否为按键弹起消息
    {
        switch (pMsg->wParam) // 根据虚拟键表值来判断按下的是什么键
        {
        case VK_UP: // 上键
            break;

        case VK_DOWN: // 下键
            break;

        case VK_LEFT: // 左键
            break;

        case VK_RIGHT: // 右键
            break;

        case VK_SHIFT:
            break;

        case VK_CONTROL:
            AfxMessageBox("ctrl UP!");
            break;
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CMSLAgentDlg::OnAddag1()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    if (m_bAddAg1 == TRUE)
    {
        // AfxMessageBox("ag1 is on");
        m_nFullField.AddAgent(1, true, m_strIP, nRemotePort);
    }
    else
    {
        // AfxMessageBox("ag1 is off");
        m_nFullField.KickAgent(1);
    }
    m_nFullField.Invalidate();
}

void CMSLAgentDlg::OnAddag2()
{
    // TODO: Add your control notification handler code here

    UpdateData();
    if (m_bAddAg2 == TRUE)
    {
        // AfxMessageBox("ag2 is on");
        m_nFullField.AddAgent(2, true, m_strIP, nRemotePort);
    }
    else
    {
        // AfxMessageBox("ag2 is off");
        m_nFullField.KickAgent(2);
    }
    m_nFullField.Invalidate();
}

void CMSLAgentDlg::OnAddag3()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    if (m_bAddAg3 == TRUE)
    {
        // AfxMessageBox("ag3 is on");
        m_nFullField.AddAgent(3, true, m_strIP, nRemotePort);
    }
    else
    {
        // AfxMessageBox("ag3 is off");
        m_nFullField.KickAgent(3);
    }
    m_nFullField.Invalidate();
}

void CMSLAgentDlg::OnAddag4()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    if (m_bAddAg4 == TRUE)
    {
        // AfxMessageBox("ag4 is on");
        m_nFullField.AddAgent(4, true, m_strIP, nRemotePort);
    }
    else
    {
        // AfxMessageBox("ag4 is off");
        m_nFullField.KickAgent(4);
    }
    m_nFullField.Invalidate();
}

void CMSLAgentDlg::OnAddag5()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    if (m_bAddAg5 == TRUE)
    {
        // AfxMessageBox("ag5 is on");
        m_nFullField.AddAgent(5, true, m_strIP, nRemotePort);
    }
    else
    {
        // AfxMessageBox("ag5 is off");
        m_nFullField.KickAgent(5);
    }
    m_nFullField.Invalidate();
}

void CMSLAgentDlg::OnVirtualatk1()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    if (m_nVirtualID1 < 0 || m_nVirtualID1 > 6)
    {
        return;
    }
    m_nFullField.m_nVirtualSt[m_nVirtualID1] = CTRL_ATTACK;
    m_nFullField.AddAgent(m_nVirtualID1, false, m_strIP, nRemotePort);

    m_bmulti = true;
    SetTimer(1, 50, NULL);
}

void CMSLAgentDlg::OnVirtualatk2()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    if (m_nVirtualID2 < 0 || m_nVirtualID2 > 6)
    {
        return;
    }
    m_nFullField.m_nVirtualSt[m_nVirtualID2] = CTRL_ATTACK;
    m_nFullField.AddAgent(m_nVirtualID2, false, m_strIP, nRemotePort);
    m_bmulti = true;
    SetTimer(1, 50, NULL);
}

void CMSLAgentDlg::OnVirtualkop()
{
    // TODO: Add your control notification handler code here

    UpdateData();
    if (m_nVirtualID3 < 0 || m_nVirtualID3 > 6)
    {
        return;
    }
    m_nFullField.m_nVirtualSt[m_nVirtualID3] = CTRL_KICKOFF_PRIM_READY;
    m_nFullField.AddAgent(m_nVirtualID3, false, m_strIP, nRemotePort);
    m_bmulti = true;
    SetTimer(1, 50, NULL);
}

void CMSLAgentDlg::OnVirtualkos()
{
    // TODO: Add your control notification handler code here

    UpdateData();
    if (m_nVirtualID4 < 0 || m_nVirtualID4 > 6)
    {
        return;
    }
    m_nFullField.m_nVirtualSt[m_nVirtualID4] = CTRL_KICKOFF_SLAVE_READY;
    m_nFullField.AddAgent(m_nVirtualID4, false, m_strIP, nRemotePort);
    m_bmulti = true;
    SetTimer(1, 50, NULL);
}

void CMSLAgentDlg::OnCancelvir1()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    if (m_nVirtualID1 < 0 || m_nVirtualID1 > 6)
    {
        return;
    }
    m_nFullField.KickAgent(m_nVirtualID1);
    m_nFullField.m_nVirtualSt[m_nVirtualID1] = 0;
}

void CMSLAgentDlg::OnCancelvir2()
{
    // TODO: Add your control notification handler code here

    UpdateData();
    if (m_nVirtualID2 < 0 || m_nVirtualID2 > 6)
    {
        return;
    }
    m_nFullField.KickAgent(m_nVirtualID2);
    m_nFullField.m_nVirtualSt[m_nVirtualID2] = 0;
}

void CMSLAgentDlg::OnCancelvir3()
{
    // TODO: Add your control notification handler code here

    UpdateData();
    if (m_nVirtualID3 < 0 || m_nVirtualID3 > 6)
    {
        return;
    }
    m_nFullField.KickAgent(m_nVirtualID3);
    m_nFullField.m_nVirtualSt[m_nVirtualID3] = 0;
}

void CMSLAgentDlg::OnCancelvir4()
{
    // TODO: Add your control notification handler code here

    UpdateData();
    if (m_nVirtualID4 < 0 || m_nVirtualID4 > 6)
    {
        return;
    }
    m_nFullField.KickAgent(m_nVirtualID4);
    m_nFullField.m_nVirtualSt[m_nVirtualID4] = 0;
}

void CMSLAgentDlg::OnVirtualgk()
{
    // TODO: Add your control notification handler code here

    UpdateData();
    m_nFullField.m_nVirtualSt[1] = CTRL_GOALKEEP;
    m_nFullField.AddAgent(1, false, m_strIP, m_nPort);

    m_bmulti = true;
    SetTimer(1, 50, NULL);
}

void CMSLAgentDlg::OnBallmissing()
{
    // TODO: Add your control notification handler code here
    if (m_bAllBalls == true)
    {
        m_bAllBalls = false;
    }
    else
    {
        m_bAllBalls = true;
    }
    m_nFullField.SetAllBall(m_bAllBalls);
}

void CMSLAgentDlg::OnAggetball()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    m_nFullField.AgentGetBall(m_nIDWhoGetBall);
}

void CMSLAgentDlg::OnAtkim()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    if (m_bAtkIm == TRUE)
    {
        m_nFullField.m_bAtkIm = true;
    }
    else
    {

        m_nFullField.m_bAtkIm = false;
    }
}

BOOL CMSLAgentDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: Add your message handler code here and/or call default

    return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CMSLAgentDlg::OnAddobst()
{
    // TODO: Add your control notification handler code here

    m_nFullField.m_bSetObst = true;
    UpdateData();
    m_nFullField.m_nObstID = m_nObstID;
}

void CMSLAgentDlg::OnNoaddobst()
{
    // TODO: Add your control notification handler code here
    if (m_nFullField.m_bSetObst == true)
    {
        m_nFullField.m_bSetObst = false;
    }

    if (m_nFullField.m_bSetTrueObst == true)
    {
        m_nFullField.m_bSetTrueObst = false;
    }
}

void CMSLAgentDlg::OnSettrueobst()
{
    // TODO: Add your control notification handler code here
    m_nFullField.m_bSetTrueObst = true;
}

void CMSLAgentDlg::OnErr()
{
    // TODO: Add your control notification handler code here
}

void CMSLAgentDlg::OnRemoveobst()
{
    // TODO: Add your control notification handler code here
    UpdateData();

    m_nFullField.m_trueobst[m_nObstIDToRemove].size = 0;
    m_nFullField.m_trueobst[m_nObstIDToRemove].valid = false;
}

void CMSLAgentDlg::OnObstBust()
{
    // TODO: Add your control notification handler code here
    if (m_nFullField.bObstBust == false)
    {
        m_nFullField.bObstBust = true;
    }
    else
    {
        m_nFullField.bObstBust = false;
    }
}

void CMSLAgentDlg::OnClearList()
{
    // TODO: Add your control notification handler code here
    m_cList.ResetContent();
}

void CMSLAgentDlg::OnTocaptain()
{
    // TODO: Add your control notification handler code here
    nRemotePort = 20090;

    OnAddag1();
    OnAddag2();
    OnAddag3();
    // 	OnAddag4();
    // 	OnAddag5();
    m_nFullField.ReplyAgentsStatus();

    m_bmulti = true;

    UpdateData(TRUE);
    m_nFullField.m_nCycle = m_nCycle;
    SetTimer(1, m_nCycle, NULL);

    m_cpu_battery.AutoQuery(800);
    SetTimer(2, 1000, NULL);
}
