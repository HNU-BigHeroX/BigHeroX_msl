// MSLFieldMonitorView.cpp : implementation of the CMSLFieldMonitorView class
//

#include "stdafx.h"
#include "MSLFieldMonitor.h"

#include "MSLFieldMonitorDoc.h"
#include "MSLFieldMonitorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMSLFieldMonitorView

IMPLEMENT_DYNCREATE(CMSLFieldMonitorView, CFormView)

BEGIN_MESSAGE_MAP(CMSLFieldMonitorView, CFormView)
//{{AFX_MSG_MAP(CMSLFieldMonitorView)
ON_WM_TIMER()
ON_WM_SIZE()
//}}AFX_MSG_MAP
// Standard printing commands
ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMSLFieldMonitorView construction/destruction

CMSLFieldMonitorView::CMSLFieldMonitorView() : CFormView(CMSLFieldMonitorView::IDD)
{
    //{{AFX_DATA_INIT(CMSLFieldMonitorView)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    // TODO: add construction code here
}

CMSLFieldMonitorView::~CMSLFieldMonitorView()
{
}

void CMSLFieldMonitorView::DoDataExchange(CDataExchange *pDX)
{
    CFormView::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CMSLFieldMonitorView)
    // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}

BOOL CMSLFieldMonitorView::PreCreateWindow(CREATESTRUCT &cs)
{
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return CFormView::PreCreateWindow(cs);
}

#define WND_REF_WIDTH 300
#define WND_REF_HEIGHT 500
#define WND_FIELD_WIDTH 700
#define WND_FIELD_HEIGHT 500
#define WND_MATCH_PANEL_WIDTH 400
#define WND_MATCH_PANEL_HEIGHT 750
#define WND_MAP_WIDTH 570
#define WND_MAP_HEIGHT 400
#define WND_STATUS_HEIGHT 201
void CMSLFieldMonitorView::OnInitialUpdate() // 初始化
{
    CFormView::OnInitialUpdate();
    GetParentFrame()->RecalcLayout();
    ResizeParentToFit();

    //[1]构造窗体
    m_wndField.Create(IDD_CHILD_FIELD, this);
    m_wndReferee.Create(IDD_CHILD_REFEREE, this);
    m_wndMatchPanel.Create(IDD_CHILD_MATCH_PANEL, this); // 猜测为右边的各种按钮在这里生成
    //	m_wndMap.Create(IDD_CHILD_MAP, this);
    m_wndStatus.Create(IDD_CHILD_STATUS, this);

    //[2]窗体尺寸和位置
    CRect rs;
    this->GetClientRect(&rs);
    CRect subrs;
    subrs = rs;

    // 裁判盒模块
    subrs.right = subrs.left + WND_REF_WIDTH;
    subrs.bottom = subrs.top + WND_REF_HEIGHT;
    m_wndReferee.MoveWindow(subrs);
    // 场地图模块
    subrs.left = WND_REF_WIDTH;
    subrs.right = subrs.left + WND_FIELD_WIDTH;
    subrs.bottom = subrs.top + WND_FIELD_HEIGHT;
    m_wndField.MoveWindow(subrs);
    // 比赛控制面板模块
    subrs.left = WND_REF_WIDTH + WND_FIELD_WIDTH;
    subrs.right = subrs.left + WND_MATCH_PANEL_WIDTH;
    subrs.bottom = subrs.top + WND_MATCH_PANEL_HEIGHT;
    m_wndMatchPanel.MoveWindow(subrs);
    // map显示模块
    subrs.top = subrs.bottom;
    subrs.right = subrs.left + WND_MAP_HEIGHT;
    subrs.bottom = subrs.top + WND_MAP_WIDTH;
    //	m_wndMap.MoveWindow(subrs);
    // Status模块
    subrs.left = rs.left;
    subrs.right = subrs.left + WND_REF_WIDTH + WND_FIELD_WIDTH;
    subrs.top = rs.top + WND_FIELD_HEIGHT + 1;
    subrs.bottom = subrs.top + WND_STATUS_HEIGHT;
    m_wndStatus.MoveWindow(subrs);

    //[3]传递指针
    m_wndField.m_cGreenField.pDataSummary = (m_cDataCenter.m_dataSummary);
    m_wndField.m_cGreenField.pCoachUDP = &m_cCoachUDP;
    m_wndField.m_cGreenField.pDataCenter = &m_cDataCenter;
    m_cDataCenter.SetCoachUDP(&m_cCoachUDP);
    m_cDataCenter.pInfoList = &(m_wndReferee.m_cDataInfoList);
    m_wndMatchPanel.pDataCenter = &m_cDataCenter;
    m_wndMatchPanel.pRefClient = &m_cRefClient;
    m_wndMatchPanel.pFiled = &(m_wndField.m_cGreenField);
    m_cRefClient.pDataCenter = &m_cDataCenter;
    m_wndStatus.m_disRobotStatus.pField = &(m_wndField.m_cGreenField);
    m_wndStatus.m_disRobotStatus.pCoachUDP = &m_cCoachUDP;
    m_wndStatus.m_disRoleStatus.pField = &(m_wndField.m_cGreenField);
    m_wndStatus.m_disRoleStatus.pTac = m_cDataCenter.m_stTactics;
    m_wndStatus.pTac_TC = &(m_cDataCenter.m_tac_TechChallenge);
    m_wndMatchPanel.pKey = &(m_cDataCenter.m_dataSummary->key);
    m_wndReferee.pKey = &(m_cDataCenter.m_dataSummary->key);
    m_wndReferee.pRefClient = &m_cRefClient;
    m_cRefClient.m_pShowList = &(m_wndReferee.m_cRefInfoList);
    m_wndField.m_cGreenField.pObstFusion = &(m_cDataCenter.m_ObstF_Zwj);
    m_wndStatus.pObstFusion = &(m_cDataCenter.m_ObstF_Zwj);
    m_wndReferee.m_dlgAgentIPList.pCoachUDP = &(m_cCoachUDP);
    m_cCoachUDP.pInfoList = &m_wndMatchPanel.m_cTacInfoList;

    // 深度图显示
    //	m_cDataCenter.m_tac_FreeKick.pWndMap = &(m_wndMap);
    //	m_cDataCenter.m_tac_Playing.pWndMap = &(m_wndMap);

    // 策略信息
    m_cDataCenter.m_tac_FreeKick.pInfoList = &(m_wndMatchPanel.m_cTacInfoList);
    m_cDataCenter.m_tac_Playing.pInfoList = &(m_wndMatchPanel.m_cTacInfoList);
    m_cDataCenter.m_tac_Playing_AreaDef.pInfoList = &(m_wndMatchPanel.m_cTacInfoList);
    m_cDataCenter.m_tac_TechChallenge.pInfoList = &(m_wndMatchPanel.m_cTacInfoList);
    m_cDataCenter.m_tac_Counter_FreeKick.pInfoList = &(m_wndMatchPanel.m_cTacInfoList);

    //[4]显示
    m_wndReferee.ShowWindow(true);
    m_wndField.ShowWindow(true);
    m_wndMatchPanel.ShowWindow(true);
    //	m_wndMap.ShowWindow(true);
    m_wndStatus.ShowWindow(true);

    //[端口与链接]
    m_cCoachUDP.Listen(20090);

    // 记录模式：打开发送记录信息的端口(应该从配置文件读取记录标记，这里未实现)
    if (false)
    {
        CString strLogServerIP = L"127.0.0.1";
        for (int i = 0; i < AG_NUM; i++)
        {
            m_cCoachUDP.m_ToAgent[i].m_LogUdpSender.SetRemoteStr(strLogServerIP, 201400 + i + 1);
            m_cCoachUDP.m_ToAgent[i].bLogMode = true;
            m_cCoachUDP.m_LogUdpSender[i].SetRemoteStr(strLogServerIP, 201410 + i + 1);
        }
    }

    m_wndMatchPanel.SetFocus();
    SetTimer(TMR_EXEC, 30, NULL); // 设置回调，时间30ms一次
    SetTimer(TMR_ONE_SEC, 1000, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CMSLFieldMonitorView printing

BOOL CMSLFieldMonitorView::OnPreparePrinting(CPrintInfo *pInfo)
{
    // default preparation
    return DoPreparePrinting(pInfo);
}

void CMSLFieldMonitorView::OnBeginPrinting(CDC * /*pDC*/, CPrintInfo * /*pInfo*/)
{
    // TODO: add extra initialization before printing
}

void CMSLFieldMonitorView::OnEndPrinting(CDC * /*pDC*/, CPrintInfo * /*pInfo*/)
{
    // TODO: add cleanup after printing
}

void CMSLFieldMonitorView::OnPrint(CDC *pDC, CPrintInfo * /*pInfo*/)
{
    // TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CMSLFieldMonitorView diagnostics

#ifdef _DEBUG
void CMSLFieldMonitorView::AssertValid() const
{
    CFormView::AssertValid();
}

void CMSLFieldMonitorView::Dump(CDumpContext &dc) const
{
    CFormView::Dump(dc);
}

CMSLFieldMonitorDoc *CMSLFieldMonitorView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMSLFieldMonitorDoc)));
    return (CMSLFieldMonitorDoc *)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMSLFieldMonitorView message handlers

static int nExeScale = 2;
void CMSLFieldMonitorView::OnTimer(UINT nIDEvent) // 定时执行的一个函数
{
    // TODO: Add your message handler code here and/or call default
    if (TMR_EXEC == nIDEvent)
    {
        if (m_wndMatchPanel.m_bPause == false)
        {
            checkKeyPressed();                                            // 键盘按钮事件
            m_cRefClient.Self = m_wndReferee.m_cSideClr.nFlag;            // 阵营
            m_cCoachUDP.CopyDataSummaryTo((m_cDataCenter.m_dataSummary)); // 复制数据
            m_cDataCenter.MainLoop();                                     // 主逻辑函数

            m_wndField.m_cGreenField.Invalidate(FALSE);     // 场地图更新
            m_wndStatus.UpdateDisInfo();                    // 状态信息更新
            m_wndStatus.m_disRobotStatus.Invalidate(FALSE); // 机器人状态更新
            m_wndStatus.UpdateRoleInfo();                   // 角色状态更新
        }
        else
        {
            // 按下了暂停键
            for (int i = 0; i < AG_NUM; i++)
            {
                m_cCoachUDP.m_ToAgent[i].SimpleCtrl(CTRL_STOP);
            }
        }
    }

    if (TMR_ONE_SEC == nIDEvent)
    {
        m_cCoachUDP.CheckAgentReply(); // 检查回包，判断是否离线
    }

    CFormView::OnTimer(nIDEvent);
}

void CMSLFieldMonitorView::OnSize(UINT nType, int cx, int cy)
{
    CFormView::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
}

BOOL CMSLFieldMonitorView::PreTranslateMessage(MSG *pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    if (pMsg->message == WM_KEYDOWN) // 判断是否为按键消息
    {

        switch (pMsg->wParam) // 根据虚拟键表值来判断按下的是什么键
        {
        case VK_UP: // 按下前键
            break;

        case VK_DOWN: // 按下后键
            break;

        case VK_LEFT: // 按下左键
            break;

        case VK_RIGHT: // 按下右键
            break;

        case VK_SHIFT:
            // 停止
            m_cRefClient.Parse(COMM_STOP);
            break;
        case VK_CONTROL:
            // 开始设置入场点
            m_wndField.m_cGreenField.m_bSetPlayerIn = true;
            m_wndField.m_cGreenField.Invalidate(FALSE); // 更新显示
            break;

        case 87:
            /*/AfxMessageBox(_T("w按下"));
            break;*/
        case 83:
            // AfxMessageBox(_T("s按下"));
            break;
        case 65:
            // AfxMessageBox(_T("a按下"));
            // 进攻
            break;
        case 67:
            // AfxMessageBox(_T("c按下"));
            break;
        case 68:
            /*/AfxMessageBox(_T("d按下"));
            break;*/
        case 81:
            break;

        default:
            break;
        }
    }

    if (pMsg->message == WM_KEYUP) // 判断是否为按键弹起消息
    {
        switch (pMsg->wParam) // 根据虚拟键表值来判断按下的是什么键
        {
        case VK_CONTROL:
            // AfxMessageBox("ctrl UP!");
            // 解除设置入场点

            m_wndField.m_cGreenField.m_bSetPlayerIn = false;
            m_wndField.m_cGreenField.Invalidate(FALSE);
            break;
        }
    }

    return CFormView::PreTranslateMessage(pMsg);
}

void CMSLFieldMonitorView::checkKeyPressed()
{
    // shift 按下
    if (m_cDataCenter.m_dataSummary->key.bShiftPressed == true)
    {
        m_cDataCenter.m_dataSummary->key.bShiftPressed = false;
        m_wndMatchPanel.OnRefStop();
    }

    // ctrl 按下
    if (m_cDataCenter.m_dataSummary->key.bCtrlPressed == true)
    {
        m_cDataCenter.m_dataSummary->key.bCtrlPressed = false;
        m_wndField.m_cGreenField.m_bSetPlayerIn = true;
    }

    // A 按下
    if (m_cDataCenter.m_dataSummary->key.bKeyAPressed == true)
    {
        m_cDataCenter.m_dataSummary->key.bKeyAPressed = false;
        // AfxMessageBox(L"A Pressed");
        //[1]获取所选队员的ID号
        int nSelID = m_wndField.m_cGreenField.m_nSelAgentID;

        if (nSelID > 0 || nSelID <= AG_NUM)
        {
            // 选择队员ID存在，继续进行后面操作
            //[2]切换到手动控制模式
            m_cDataCenter.ChangeMatchModeTo(MATCH_OFF);

            //[3]将所选的队员切换到进攻
            m_cCoachUDP.m_ToAgent[nSelID - 1].CtrlCmd(CTRL_ATTACK, m_cDataCenter.m_dataSummary->cGlobalBall.x,
                                                      m_cDataCenter.m_dataSummary->cGlobalBall.y, 0, 0, true);
        }
    }

    // S 按下
    if (m_cDataCenter.m_dataSummary->key.bKeySPressed == true)
    {
        m_cDataCenter.m_dataSummary->key.bKeySPressed = false;
        m_wndMatchPanel.OnRefStop();
    }
}
