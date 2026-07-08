// WndStatus.cpp : implementation file
//

#include "stdafx.h"
#include "MSLFieldMonitor.h"
#include "WndStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWndStatus dialog

CWndStatus::CWndStatus(CWnd *pParent /*=NULL*/) : CDialog(CWndStatus::IDD, pParent)
{
    //{{AFX_DATA_INIT(CWndStatus)
    //}}AFX_DATA_INIT
}

void CWndStatus::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CWndStatus)
    DDX_Control(pDX, IDC_ROLE_STATUS, m_disRoleStatus);
    DDX_Control(pDX, IDC_ROBOT_STATUS, m_disRobotStatus);
    DDX_Control(pDX, IDC_DIS_INFO, m_cDisInfo);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWndStatus, CDialog)
//{{AFX_MSG_MAP(CWndStatus)
ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWndStatus message handlers

BOOL CWndStatus::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    // 角色列表
    /*m_cRoleList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    CString strname[12];
    strname[0] = "ID";
    strname[1] = "Ctr";
    strname[2] = "DF_Ang";
    strname[3] = "DF_Dit";
    strname[4] = "MT_Ang";
    strname[5] = "MT_X";
    strname[6] = "MT_Y";
    strname[7] = "PT_X";
    strname[8] = "PT_Y";
    strname[9] = "RB_X";
    strname[10] = "RB_Y";
    strname[11] = "A_ID";

    m_cRoleList.InsertColumn(0, strname[0], LVCFMT_LEFT, 30);
    m_cRoleList.InsertColumn(1, strname[1], LVCFMT_LEFT, 70);
    m_cRoleList.InsertColumn(2, strname[2], LVCFMT_LEFT, 60);
    m_cRoleList.InsertColumn(3, strname[3], LVCFMT_LEFT, 60);
    m_cRoleList.InsertColumn(4, strname[4], LVCFMT_LEFT, 60);
    m_cRoleList.InsertColumn(5, strname[5], LVCFMT_LEFT, 60);
    m_cRoleList.InsertColumn(6, strname[6], LVCFMT_LEFT, 60);
    m_cRoleList.InsertColumn(7, strname[7], LVCFMT_LEFT, 60);
    m_cRoleList.InsertColumn(8, strname[8], LVCFMT_LEFT, 60);
    m_cRoleList.InsertColumn(9, strname[9], LVCFMT_LEFT, 60);
    m_cRoleList.InsertColumn(10, strname[10], LVCFMT_LEFT, 60);
    m_cRoleList.InsertColumn(11, strname[11], LVCFMT_LEFT, 40);

    CString strIndex;
    for (int i = 0; i < AG_NUM; i++)
    {
    strIndex.Format("%d", i);
    m_cRoleList.InsertItem(i, strIndex);
    }*/
    return TRUE; // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CWndStatus::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    CRect rcWnd;
    rcWnd.left = 0;
    rcWnd.top = 0;
    rcWnd.right = 300;
    rcWnd.bottom = cy;
    if (m_cDisInfo.m_hWnd) // 信息显示
    {
        m_cDisInfo.MoveWindow(&rcWnd);
    }
    rcWnd.left = rcWnd.right;
    rcWnd.right = cx;
    if (m_disRobotStatus.m_hWnd) // 机器人状态
    {
        rcWnd.bottom = 100;
        m_disRobotStatus.MoveWindow(&rcWnd);
    }
    if (m_disRoleStatus.m_hWnd) // 角色列表
    {
        rcWnd.top = rcWnd.bottom + 1;
        rcWnd.bottom = cy;

        m_disRoleStatus.MoveWindow(&rcWnd);
    }
}

void CWndStatus::UpdateDisInfo()
{
    stDataSummary *pDataSummary = m_disRobotStatus.pField->pDataSummary;
    CString strInfo;
    strInfo.Format("【传球】%d-(%d,%d) ,【接球】%d-(%d,%d)  \r\n\
【起脚】%d 【传球中】%d 【传球时间】%d\r\n\
【球】%d-(%d,%d) 失踪计数= %d【触球】%d(%d,%d)\r\n\
【上策】%s \r\n【开始】%d 【开始时间】%d\r\n\
【入场点】( %d , %d )\r\n\
【障碍物个数】%d【最近障碍物】( %d , %d )\r\n\
【球向障碍物个数】%d【球向障碍物】( %d , %d )\r\n\
【球向队友个数】%d【球向队友】( %d , %d )\r\n\
【技术挑战赛】A= %d, B=%d, C=%d )\r\n\
【障碍物跟踪】跟踪数 = %d 计数0=%d)",
                   pDataSummary->nPassID, pDataSummary->posPass.x, pDataSummary->posPass.y,      // 传球
                   pDataSummary->nCatchID, pDataSummary->posCatch.x, pDataSummary->posCatch.y,   // 接球
                   pDataSummary->bPassKick, pDataSummary->bPassing, pDataSummary->nPassKickTime, // 球传出标记，传球时间
                   !pDataSummary->bNoOneSeeBall, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,
                   pDataSummary->nNoOneSeenBallCount,                                                   // 球
                   pDataSummary->bHoldBall, pDataSummary->posTouchBall.x, pDataSummary->posTouchBall.y, // 触球
                   GetTacString(pDataSummary->nLastTacReady), pDataSummary->bAction,
                   pDataSummary->nTimeLastStartSec, // 上一条策略，开始信号，开始时间
                   pDataSummary->posPlayerIn.x, pDataSummary->posPlayerIn.y, pDataSummary->NumOfemy,
                   pDataSummary->t_enemy[0].x, pDataSummary->t_enemy[0].y, pDataSummary->nNumOfPassToEnemy,
                   pDataSummary->posPassToEnemy[0].x, pDataSummary->posPassToEnemy[0].y,
                   pDataSummary->nNumOfPassToTeammate, pDataSummary->posPassToTeammate.x,
                   pDataSummary->posPassToTeammate.y, pTac_TC->nPlayerA_ID, pTac_TC->nPlayerB_ID, pTac_TC->nPlayerC_ID,
                   pObstFusion->nNumTrackEnemy, pObstFusion->arTrackCount[0]);
    m_cDisInfo.SetWindowText(strInfo);
}

void CWndStatus::UpdateRoleInfo()
{
    m_disRoleStatus.Invalidate(FALSE);
}

CString CWndStatus::GetTacString(int inTac)
{
    CString strTac;
    switch (inTac)
    {
    case MATCH_OFF:
        strTac = "MATCH_OFF";
        break;
    case MATCH_STOP:
        strTac = "MATCH_STOP";
        break;
    case MATCH_PLAYING:
        strTac = "MATCH_PLAYING";
        break;
    case MATCH_PARK_IN:
        strTac = "MATCH_PARK_IN";
        break;
    case MATCH_PARK_OUT:
        strTac = "MATCH_PARK_OUT";
        break;
    case MATCH_START:
        strTac = "MATCH_START";
        break;
    case MATCH_DROPPEDBALL_READY:
        strTac = "MATCH_DROPPEDBALL_READY";
        break;
    case MATCH_DROPPEDBALL_START:
        strTac = "MATCH_DROPPEDBALL_START";
        break;
    case MATCH_KICKOFF_READY:
        strTac = "MATCH_KICKOFF_READY";
        break;
    case MATCH_KICKOFF_START:
        strTac = "MATCH_KICKOFF_START";
        break;
    case MATCH_FREEKICK_READY:
        strTac = "MATCH_FREEKICK_READY";
        break;
    case MATCH_FREEKICK_START:
        strTac = "MATCH_FREEKICK_START";
        break;
    case MATCH_GOALKICK_READY:
        strTac = "MATCH_GOALKICK_READY";
        break;
    case MATCH_GOALKICK_START:
        strTac = "MATCH_GOALKICK_START";
        break;
    case MATCH_THROWIN_READY:
        strTac = "MATCH_THROWIN_READY";
        break;
    case MATCH_THROWIN_START:
        strTac = "MATCH_THROWIN_START";
        break;
    case MATCH_CORNERKICK_READY:
        strTac = "MATCH_CORNERKICK_READY";
        break;
    case MATCH_CORNERKICK_START:
        strTac = "MATCH_CORNERKICK_START";
        break;
    case MATCH_PENALTY_READY:
        strTac = "MATCH_PENALTY_READY";
        break;
    case MATCH_PENALTY_START:
        strTac = "MATCH_PENALTY_START";
        break;
    case MATCH_COUNTER_KICKOFF_READY:
        strTac = "MATCH_COUNTER_KICKOFF_READY";
        break;
    case MATCH_COUNTER_KICKOFF_START:
        strTac = "MATCH_COUNTER_KICKOFF_START";
        break;
    case MATCH_COUNTER_FREEKICK_READY:
        strTac = "MATCH_COUNTER_FREEKICK_READY";
        break;
    case MATCH_COUNTER_FREEKICK_START:
        strTac = "MATCH_COUNTER_FREEKICK_START";
        break;
    case MATCH_COUNTER_GOALKICK_READY:
        strTac = "MATCH_COUNTER_GOALKICK_READY";
        break;
    case MATCH_COUNTER_GOALKICK_START:
        strTac = "MATCH_COUNTER_GOALKICK_START";
        break;
    case MATCH_COUNTER_THROWIN_READY:
        strTac = "MATCH_COUNTER_THROWIN_READY";
        break;
    case MATCH_COUNTER_THROWIN_START:
        strTac = "MATCH_COUNTER_THROWIN_START";
        break;
    case MATCH_COUNTER_CORNERKICK_READY:
        strTac = "MATCH_COUNTER_CORNERKICK_READY";
        break;
    case MATCH_COUNTER_CORNERKICK_START:
        strTac = "MATCH_COUNTER_CORNERKICK_START";
        break;
    case MATCH_COUNTER_PENALTY_READY:
        strTac = "MATCH_COUNTER_PENALTY_READY";
        break;
    case MATCH_COUNTER_PENALTY_START:
        strTac = "MATCH_COUNTER_PENALTY_START";
        break;
    case MATCH_TEST_PASSING:
        strTac = "MATCH_TEST_PASSING";
        break;
    }
    return strTac;
}
