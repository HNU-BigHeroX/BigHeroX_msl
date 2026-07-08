#include "stdafx.h"
#include "Tac_TechChallenge.h"

#define TC_GO_A_PASSTO_B 0
#define TC_GO_PASSING_A_B 1
#define TC_GO_B_PASSTO_C 2
#define TC_GO_PASSING_B_C 3
#define TC_GO_C_GOAL 4

static POINT PosReady_A;
static POINT PosReady_B;
static POINT PosReady_C;

CTac_TechChallenge::CTac_TechChallenge()
{
    PosReady_A.x = 1500;
    PosReady_A.y = 300;
    PosReady_B.x = 300;
    PosReady_B.y = 600;
    PosReady_C.x = 1500;
    PosReady_C.y = 900;
    Reset();
}

CTac_TechChallenge::~CTac_TechChallenge()
{
}

void CTac_TechChallenge::Reset()
{
    m_nTacStep = TAC_STEP_READY;
    m_nGoStep = TC_GO_A_PASSTO_B;

    nPlayerA_ID = 0;
    bPlayerA_Done = false;
    nPlayerB_ID = 0;
    bPlayerB_Done = false;
    nPlayerC_ID = 0;
    bPlayerC_Done = false;
}

void CTac_TechChallenge::PlanTac(stTactics *outTactics)
{
    pTactics = outTactics;
    switch (m_nTacStep)
    {
    case TAC_STEP_READY:
        Step_Ready();
        break;
    case TAC_STEP_GO:
        // 技术挑战赛因为对队员的使用不能重复，所以需要绕开角色分配系统，直接指定队员去执行。
        // 所以通过CTRL_MANUAL来让自动分配系统放弃对队员的控制
        uRole *pRole = outTactics->role;
        pRole[0].ctrl = CTRL_MANUAL;
        pRole[1].ctrl = CTRL_MANUAL;
        pRole[2].ctrl = CTRL_MANUAL;
        pRole[3].ctrl = CTRL_MANUAL;
        pRole[4].ctrl = CTRL_MANUAL;
        Step_Go();
        break;
    }
}

void CTac_TechChallenge::RoleABCAllocate()
{
    nPlayerA_ID = 0;
    nPlayerB_ID = 0;
    nPlayerC_ID = 0;

    // 根据球所处的上下半场来确定A和C点
    if (pDataSummary->cGlobalBall.y > 600)
    {
        PosReady_A.y = 900;
        PosReady_C.y = 300;
    }
    else
    {
        PosReady_A.y = 300;
        PosReady_C.y = 900;
    }

    uAgent *pAgent = pDataSummary->agent;
    int nTmpDist = 0;
    int nMinDist = 1800;
    // 1）挑选A
    nMinDist = 1800;
    for (int i = 0; i < AG_NUM; i++)
    {
        if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR && pAgent[i].ID != nPlayerB_ID &&
            pAgent[i].ID != nPlayerC_ID)
        {
            nTmpDist = m_CalDist(PosReady_A.x, PosReady_A.y, pAgent[i].x, pAgent[i].y);
            if (nTmpDist < nMinDist)
            {
                nPlayerA_ID = pAgent[i].ID;
                nMinDist = nTmpDist;
            }
        }
    }
    if (nPlayerA_ID <= 0)
    {
        PrintInfo("[技术挑战赛]A角色选择失败");
    }

    // 2）挑选B
    nMinDist = 1800;
    for (i = 0; i < AG_NUM; i++)
    {
        if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR && pAgent[i].ID != nPlayerA_ID &&
            pAgent[i].ID != nPlayerC_ID)
        {
            nTmpDist = m_CalDist(PosReady_B.x, PosReady_B.y, pAgent[i].x, pAgent[i].y);
            if (nTmpDist < nMinDist)
            {
                nPlayerB_ID = pAgent[i].ID;
                nMinDist = nTmpDist;
            }
        }
    }
    if (nPlayerB_ID <= 0)
    {
        PrintInfo("[技术挑战赛]B角色选择失败");
    }

    // 3）挑选C
    nMinDist = 1800;
    for (i = 0; i < AG_NUM; i++)
    {
        if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR && pAgent[i].ID != nPlayerA_ID &&
            pAgent[i].ID != nPlayerB_ID)
        {
            nTmpDist = m_CalDist(PosReady_C.x, PosReady_C.y, pAgent[i].x, pAgent[i].y);
            if (nTmpDist < nMinDist)
            {
                nPlayerC_ID = pAgent[i].ID;
                nMinDist = nTmpDist;
            }
        }
    }
    if (nPlayerC_ID <= 0)
    {
        PrintInfo("[技术挑战赛]C角色选择失败");
    }
}

void CTac_TechChallenge::Start()
{
    m_nTacStep = TAC_STEP_GO;
    m_nGoStep = TC_GO_A_PASSTO_B;
}

void CTac_TechChallenge::Step_Ready()
{
    strTacticInfo = "[技术挑战赛] 预备跑位";
    uRole *pRole = pTactics->role;

    pRole[0].ctrl = CTRL_MOVETO;
    pRole[0].nDest_x = PosReady_A.x;
    pRole[0].nDest_y = PosReady_A.y;
    pRole[0].nDest_angle = 180;

    pRole[1].ctrl = CTRL_MOVETO;
    pRole[1].nDest_x = PosReady_B.x;
    pRole[1].nDest_y = PosReady_B.y;
    pRole[1].nDest_angle = 0;

    pRole[2].ctrl = CTRL_MOVETO;
    pRole[2].nDest_x = PosReady_C.x;
    pRole[2].nDest_y = PosReady_C.y;
    pRole[2].nDest_angle = 180;

    pRole[3].ctrl = CTRL_STOP;
    pRole[4].ctrl = CTRL_STOP;

    // READY的阶段就一直在指派A、B、C三名队员
    RoleABCAllocate();
}
void CTac_TechChallenge::Step_Go()
{
    uAgent *pAgent = pDataSummary->agent;
    switch (m_nGoStep)
    {
    case TC_GO_A_PASSTO_B:
        strTacticInfo = "[技术挑战赛] 队员A找球（前场）";
        pDataSummary->posCatch.x = 500;
        pDataSummary->posCatch.y = 600;
        pToAgent[nPlayerA_ID - 1].CtrlCmd(CTRL_PASS_MOVE, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,
                                          pDataSummary->posCatch.x, pDataSummary->posCatch.y, true);
        pToAgent[nPlayerB_ID - 1].CtrlCmd(CTRL_CATCH_MOVE, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,
                                          pDataSummary->posCatch.x, pDataSummary->posCatch.y, true);
        pToAgent[nPlayerC_ID - 1].MoveTo(PosReady_C.x, PosReady_C.y, 180, 0, true, 100);
        if (pDataSummary->bPassKick == true)
        {
            // A起脚传球信号
            m_nGoStep = TC_GO_PASSING_A_B;
        }
        break;

    case TC_GO_PASSING_A_B:
        strTacticInfo = "[技术挑战赛] 队员A传球给队员B(前场传后场)";
        // 接球位置以传球队员提供的为准
        pDataSummary->posCatch.x = pAgent[nPlayerA_ID - 1].dest_x;
        pDataSummary->posCatch.y = pAgent[nPlayerA_ID - 1].dest_y;
        pToAgent[nPlayerA_ID - 1].CtrlCmd(CTRL_PASS_MOVE, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,
                                          pDataSummary->posCatch.x, pDataSummary->posCatch.y, true);
        pToAgent[nPlayerB_ID - 1].CtrlCmd(CTRL_CATCH_FOCUS, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,
                                          pDataSummary->posCatch.x, pDataSummary->posCatch.y, true);
        // C队员应该已经在跑接球点了
        pCatchPosition->CalCatchPosition();
        pToAgent[nPlayerC_ID - 1].MoveTo(pCatchPosition->posCatch[0].x, pCatchPosition->posCatch[0].y, 180, 0, true,
                                         100);

        // 队员B接到球的条件
        if (pAgent[nPlayerB_ID - 1].status == CTRL_ATTACK || pAgent[nPlayerB_ID - 1].status == CTRL_PASS_MOVE

        )
        {
            // B队员已经接到球，切换到下个状态
            m_nGoStep = TC_GO_B_PASSTO_C;
        }
        break;

    case TC_GO_B_PASSTO_C:
        strTacticInfo = "[技术挑战赛] 队员B找球（后场）";
        pCatchPosition->CalCatchPosition();
        pDataSummary->posCatch = pCatchPosition->posCatch[0];
        pToAgent[nPlayerB_ID - 1].CtrlCmd(CTRL_PASS_MOVE, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,
                                          pDataSummary->posCatch.x, pDataSummary->posCatch.y, true);
        pToAgent[nPlayerC_ID - 1].CtrlCmd(CTRL_CATCH_MOVE, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,
                                          pDataSummary->posCatch.x, pDataSummary->posCatch.y, true);
        // A队员完成任务，尽快移动到场边
        if (pAgent[nPlayerA_ID - 1].y > 600)
        {
            PosReady_A.y = 1100;
        }
        else
        {
            PosReady_A.y = 100;
        }
        pToAgent[nPlayerA_ID - 1].MoveTo(PosReady_A.x, PosReady_A.y, 180, 0, true, 100);
        if (pDataSummary->bPassKick == true)
        {
            // B起脚传球信号
            m_nGoStep = TC_GO_PASSING_B_C;
        }
        break;

    case TC_GO_PASSING_B_C:
        strTacticInfo = "[技术挑战赛] 队员B传球给队员C(后场传前场)";
        // 接球位置以传球队员提供的为准
        pDataSummary->posCatch.x = pAgent[nPlayerB_ID - 1].dest_x;
        pDataSummary->posCatch.y = pAgent[nPlayerB_ID - 1].dest_y;
        pToAgent[nPlayerB_ID - 1].CtrlCmd(CTRL_PASS_MOVE, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,
                                          pDataSummary->posCatch.x, pDataSummary->posCatch.y, true);
        pToAgent[nPlayerC_ID - 1].CtrlCmd(CTRL_CATCH_FOCUS, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,
                                          pDataSummary->posCatch.x, pDataSummary->posCatch.y, true);
        if (pAgent[nPlayerC_ID - 1].status == CTRL_ATTACK || pAgent[nPlayerC_ID - 1].status == CTRL_PASS_MOVE)
        {
            // B队员已经接到球，切换到下个状态
            m_nGoStep = TC_GO_C_GOAL;
        }
        break;

    case TC_GO_C_GOAL:
        strTacticInfo = "[技术挑战赛] 队员C射门";
        pToAgent[nPlayerA_ID - 1].MoveTo(PosReady_A.x, PosReady_A.y, 180, 0, true, 100);
        pToAgent[nPlayerB_ID - 1].MoveTo(PosReady_B.x, PosReady_B.y, 0, 0, true, 100);
        pToAgent[nPlayerC_ID - 1].SimpleCtrl(CTRL_ATTACK);
        break;
    default:
        break;
    }
}
