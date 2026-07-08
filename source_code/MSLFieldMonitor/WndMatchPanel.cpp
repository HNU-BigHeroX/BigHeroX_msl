// WndMatchPanel.cpp : implementation file
//

#include "stdafx.h"
#include "MSLFieldMonitor.h"
#include "WndMatchPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int nMoveTestDir[AG_NUM];

/////////////////////////////////////////////////////////////////////////////
// CWndMatchPanel dialog

CWndMatchPanel::CWndMatchPanel(CWnd *pParent /*=NULL*/) : CDialog(CWndMatchPanel::IDD, pParent)
{
    //{{AFX_DATA_INIT(CWndMatchPanel)
    m_nAgentTestCtrlAngle = 0;
    m_nAgentTestCtrlDist = 200;
    m_nAgentTestCtrlNum = 0;
    m_nDefDist = 200;
    //}}AFX_DATA_INIT

    pRefClient = NULL;
    m_bPause = false;

    for (int i = 0; i < AG_NUM; i++)
    {
        nMoveTestDir[i] = 0;
    }
}

void CWndMatchPanel::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CWndMatchPanel)
    DDX_Control(pDX, IDC_PANEL_PAUSE, m_cBtnPause);
    DDX_Control(pDX, IDC_TACINFOLIST, m_cTacInfoList);
    DDX_Text(pDX, IDC_AGENTTEST_CTRLANGLE, m_nAgentTestCtrlAngle);
    DDX_Text(pDX, IDC_AGENTTEST_CTRLDIST, m_nAgentTestCtrlDist);
    DDX_Text(pDX, IDC_AGENTTEST_CTRLNUM, m_nAgentTestCtrlNum);
    DDX_Text(pDX, IDC_DEF_DIST, m_nDefDist);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWndMatchPanel, CDialog)
//{{AFX_MSG_MAP(CWndMatchPanel)
ON_BN_CLICKED(IDC_REF_CYN_KICKOFF, OnRefCynKickoff)
ON_BN_CLICKED(IDC_REF_CYN_FREEKICK, OnRefCynFreekick)
ON_BN_CLICKED(IDC_REF_CYN_GOALKICK, OnRefCynGoalkick)
ON_BN_CLICKED(IDC_REF_CYN_THROWIN, OnRefCynThrowin)
ON_BN_CLICKED(IDC_REF_CYN_CORNERKICK, OnRefCynCornerkick)
ON_BN_CLICKED(IDC_REF_CYN_PENALTY, OnRefCynPenalty)
ON_BN_CLICKED(IDC_REF_START, OnRefStart)
ON_BN_CLICKED(IDC_REF_STOP, OnRefStop)
ON_BN_CLICKED(IDC_PANEL_PAUSE, OnPanelPause)
ON_BN_CLICKED(IDC_REF_PARKING, OnRefParking)
ON_BN_CLICKED(IDC_REF_DROPPEDBALL, OnRefDroppedball)
ON_BN_CLICKED(IDC_REF_MAGENTA_KICKOFF, OnRefMagentaKickoff)
ON_BN_CLICKED(IDC_REF_MAGENTA_FREEKICK, OnRefMagentaFreekick)
ON_BN_CLICKED(IDC_REF_MAGENTA_GOALKICK, OnRefMagentaGoalkick)
ON_BN_CLICKED(IDC_REF_MAGENTA_THROWIN, OnRefMagentaThrowin)
ON_BN_CLICKED(IDC_REF_MAGENTA_CORNERKICK, OnRefMagentaCornerkick)
ON_BN_CLICKED(IDC_REF_MAGENTA_PENALTY, OnRefMagentaPenalty)
ON_BN_CLICKED(IDC_AGTEST_ATTACK, OnAgtestAttack)
ON_BN_CLICKED(IDC_AGTEST_GOALKEEPER, OnAgtestGoalkeeper)
ON_BN_CLICKED(IDC_AGTEST_CATCH_MOVE, OnAgtestCatchMove)
ON_BN_CLICKED(IDC_AGTEST_CATCH, OnAgtestCatch)
ON_BN_CLICKED(IDC_AGTEST_PASS, OnAgtestPass)
ON_BN_CLICKED(IDC_AGTEST_CTRLSET, OnAgtestCtrlset)
ON_BN_CLICKED(IDC_AGTEST_DEFBALL, OnAgtestDefball)
ON_BN_CLICKED(IDC_AGTEST_DEFPASS, OnAgtestDefpass)
ON_BN_CLICKED(IDC_ENEMY_TRACK, OnEnemyTrack)
ON_BN_CLICKED(IDC_TAC_START_AUTO, OnTacStartAuto)
ON_BN_CLICKED(IDC_TAC_TECHCHALLENGE, OnTacTechchallenge)
ON_BN_CLICKED(IDC_TAC_TEST_PASSING, OnTacTestPassing)
ON_BN_CLICKED(IDC_TAC_TEST_MOVEAROUND, OnTacTestMovearound)
ON_BN_CLICKED(IDC_TAC_TEST_CATCHMOVE, OnTacTestCatchmove)
ON_BN_CLICKED(IDC_TAC_TEST_4PASS, OnTacTest4pass)
ON_BN_CLICKED(IDC_TAC_TEST_ALLDEF, OnTacTestAlldef)
ON_BN_CLICKED(IDC_CLEAN_TACINFO, OnCleanTacinfo)
ON_BN_CLICKED(IDC_MOVETEST, OnMovetest)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWndMatchPanel message handlers

void CWndMatchPanel::OnRefCynKickoff()
{
    // TODO: Add your control notification handler code here

    pRefClient->Parse(COMM_KICKOFF_CYAN);
}

void CWndMatchPanel::OnRefCynFreekick()
{
    // TODO: Add your control notification handler code here

    pRefClient->Parse(COMM_FREEKICK_CYAN);
}

void CWndMatchPanel::OnRefCynGoalkick()
{
    // TODO: Add your control notification handler code here

    pRefClient->Parse(COMM_GOALKICK_CYAN);
}

void CWndMatchPanel::OnRefCynThrowin()
{
    // TODO: Add your control notification handler code here

    pRefClient->Parse(COMM_THROWIN_CYAN);
}

void CWndMatchPanel::OnRefCynCornerkick()
{
    // TODO: Add your control notification handler code here

    pRefClient->Parse(COMM_CORNER_CYAN);
}

void CWndMatchPanel::OnRefCynPenalty()
{
    // TODO: Add your control notification handler code here

    pRefClient->Parse(COMM_PENALTY_CYAN);
}

void CWndMatchPanel::OnRefStart()
{
    // TODO: Add your control notification handler code here

    pRefClient->Parse(COMM_START);
}

void CWndMatchPanel::OnRefStop()
{
    // TODO: Add your control notification handler code here

    pFiled->m_nAgentTestMode = AGENT_TEST_NONE;
    pRefClient->Parse(COMM_STOP);
}

void CWndMatchPanel::OnPanelPause()
{
    // TODO: Add your control notification handler code here

    if (m_bPause == true)
    {
        m_bPause = false;
        m_cBtnPause.SetWindowText("暂停");
    }
    else
    {
        m_bPause = true;
        m_cBtnPause.SetWindowText("继续");
    }
}

void CWndMatchPanel::OnRefParking()
{
    // TODO: Add your control notification handler code here

    pRefClient->Parse(COMM_PARKING);
}

void CWndMatchPanel::OnRefDroppedball()
{
    // TODO: Add your control notification handler code here

    pRefClient->Parse(COMM_DROPPED_BALL);
}

void CWndMatchPanel::OnRefMagentaKickoff()
{
    // TODO: Add your control notification handler code here

    pRefClient->Parse(COMM_KICKOFF_MAGENTA);
}

void CWndMatchPanel::OnRefMagentaFreekick()
{
    // TODO: Add your control notification handler code here

    pRefClient->Parse(COMM_FREEKICK_MAGENTA);
}

void CWndMatchPanel::OnRefMagentaGoalkick()
{
    // TODO: Add your control notification handler code here

    pRefClient->Parse(COMM_GOALKICK_MAGENTA);
}

void CWndMatchPanel::OnRefMagentaThrowin()
{
    // TODO: Add your control notification handler code here

    pRefClient->Parse(COMM_THROWIN_MAGENTA);
}

void CWndMatchPanel::OnRefMagentaCornerkick()
{
    // TODO: Add your control notification handler code here

    pRefClient->Parse(COMM_CORNER_MAGENTA);
}

void CWndMatchPanel::OnRefMagentaPenalty()
{
    // TODO: Add your control notification handler code here
    pRefClient->Parse(COMM_PENALTY_MAGENTA);
}

void CWndMatchPanel::OnAgtestAttack()
{
    // TODO: Add your control notification handler code here

    if (pFiled->m_nSelAgentID > 0)
    {
        pDataCenter->ChangeMatchModeTo(MATCH_OFF);
        pFiled->pCoachUDP->m_ToAgent[pFiled->m_nSelAgentID - 1].SimpleCtrl(CTRL_ATTACK);
    }
}

void CWndMatchPanel::OnAgtestGoalkeeper()
{
    // TODO: Add your control notification handler code here

    if (pFiled->m_nSelAgentID > 0)
    {
        pDataCenter->ChangeMatchModeTo(MATCH_OFF);
        pFiled->pCoachUDP->m_ToAgent[pFiled->m_nSelAgentID - 1].SimpleCtrl(CTRL_GOALKEEP);
    }
}

void CWndMatchPanel::OnAgtestCatchMove()
{
    // TODO: Add your control notification handler code here

    if (pFiled->m_nSelAgentID > 0)
    {
        pDataCenter->ChangeMatchModeTo(MATCH_OFF);
        pFiled->pCoachUDP->m_ToAgent[pFiled->m_nSelAgentID - 1].CatchCmd(pDataCenter->m_dataSummary->cGlobalBall.x,
                                                                         pDataCenter->m_dataSummary->cGlobalBall.y);
    }
}

void CWndMatchPanel::OnAgtestCatch()
{
    // TODO: Add your control notification handler code here
}

void CWndMatchPanel::OnAgtestPass()
{
    // TODO: Add your control notification handler code here
}

void CWndMatchPanel::OnAgtestCtrlset()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    if (pFiled->m_nSelAgentID > 0)
    {
        pDataCenter->ChangeMatchModeTo(MATCH_OFF);
        // pFiled->pCoachUDP->m_ToAgent[pFiled->m_nSelAgentID - 1].SimpleCtrl(m_nAgentTestCtrlNum);
        pFiled->pCoachUDP->m_ToAgent[pFiled->m_nSelAgentID - 1].CtrlCmd(
            m_nAgentTestCtrlNum, pDataCenter->m_dataSummary->cGlobalBall.x, pDataCenter->m_dataSummary->cGlobalBall.x,
            m_nAgentTestCtrlAngle, m_nAgentTestCtrlDist, true);
    }
}

void CWndMatchPanel::OnAgtestDefball()
{
    // TODO: Add your control notification handler code here
    if (pFiled->m_nSelAgentID > 0)
    {
        UpdateData();
        pDataCenter->ChangeMatchModeTo(MATCH_OFF);
        pFiled->m_nAgentTestDefDist = m_nDefDist;
        pFiled->m_nAgentTestMode = AGENT_TEST_DEF_GOAL;
    }
}

void CWndMatchPanel::OnAgtestDefpass()
{
    // TODO: Add your control notification handler code here
    if (pFiled->m_nSelAgentID > 0)
    {
        UpdateData();
        pDataCenter->ChangeMatchModeTo(MATCH_OFF);
        pFiled->m_nAgentTestDefDist = m_nDefDist;
        pFiled->m_nAgentTestMode = AGENT_TEST_DEF_BALL;
    }
}

void CWndMatchPanel::OnEnemyTrack()
{
    // TODO: Add your control notification handler code here
    if (pDataCenter->m_ObstF_Zwj.bEnable == true)
    {
        pDataCenter->m_ObstF_Zwj.bEnable = false;
    }
    else
    {
        pDataCenter->m_ObstF_Zwj.bEnable = true;
    }
}

void CWndMatchPanel::OnTacStartAuto()
{
    // TODO: Add your control notification handler code here

    pRefClient->pDataCenter->ChangeMatchModeTo(MATCH_PLAYING);
}

void CWndMatchPanel::OnTacTechchallenge()
{
    // TODO: Add your control notification handler code here
    pRefClient->pDataCenter->ChangeMatchModeTo(MATCH_TECHCHALLENGE);
}

void CWndMatchPanel::OnTacTestPassing()
{
    // TODO: Add your control notification handler code here

    pRefClient->pDataCenter->ChangeMatchModeTo(MATCH_TEST_PASSING);
}

void CWndMatchPanel::OnTacTestMovearound()
{
    // TODO: Add your control notification handler code here

    pRefClient->pDataCenter->ChangeMatchModeTo(MATCH_TEST_MOVEAROUND);
}

void CWndMatchPanel::OnTacTestCatchmove()
{
    // TODO: Add your control notification handler code here

    pRefClient->pDataCenter->ChangeMatchModeTo(MATCH_TEST_CATCHMOVE);
}

void CWndMatchPanel::OnTacTest4pass()
{
    // TODO: Add your control notification handler code here

    pRefClient->pDataCenter->ChangeMatchModeTo(MATCH_TEST_4PASS);
}

void CWndMatchPanel::OnTacTestAlldef()
{
    // TODO: Add your control notification handler code here

    pRefClient->pDataCenter->ChangeMatchModeTo(MATCH_TEST_ALLDEF);
}

void CWndMatchPanel::OnCleanTacinfo()
{
    // TODO: Add your control notification handler code here

    m_cTacInfoList.ResetContent();
}

void CWndMatchPanel::OnMovetest()
{
    // TODO: Add your control notification handler code here

    for (int i = 2; i <= 5; i++)
    {
        MoveTest(i);
    }
}

void CWndMatchPanel::MoveTest(int inID)
{
    if (inID < 1 || inID > 5)
    {
        return;
    }
    uAgent *pCurAgent = &(pDataCenter->m_dataSummary->agent[inID - 1]);
    CToAgent *pCurToAgent = &(pFiled->pCoachUDP->m_ToAgent[inID - 1]);
    if (nMoveTestDir[inID - 1] == 0)
    {
        // 判断队员在上下那个半场
        int AgentY = pCurAgent->y;
        if (AgentY > 600)
        {
            // 在上半场，往下移一点
            nMoveTestDir[inID - 1] = -1;
        }
        else
        {
            // 在下半场，往上移一点
            nMoveTestDir[inID - 1] = 1;
        }
    }
    else
    {
        if (nMoveTestDir[inID - 1] > 0)
        {
            nMoveTestDir[inID - 1] = -1;
        }
        else
        {
            nMoveTestDir[inID - 1] = 1;
        }
    }
    // 执行
    pCurToAgent->MoveTo(pCurAgent->x, pCurAgent->y + 50 * nMoveTestDir[inID - 1], pCurAgent->angle, 0, true, 100);
}

BOOL CWndMatchPanel::PreTranslateMessage(MSG *pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    if (pMsg->message == WM_KEYDOWN) // 判断是否为按键消息
    {

        switch (pMsg->wParam) // 根据虚拟键表值来判断按下的是什么键
        {
        case VK_SHIFT:
            // AfxMessageBox(L"CWndReferee::PreTranslateMessage");
            pKey->bShiftPressed = true;
            break;
        case VK_CONTROL:
            pKey->bCtrlPressed = true;
            break;
        case 65:
            // AfxMessageBox(_T("a按下"));
            pKey->bKeyAPressed = true;
            break;
        case 83:
            // AfxMessageBox(_T("s按下"));
            pKey->bKeySPressed = true;
            break;
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}
