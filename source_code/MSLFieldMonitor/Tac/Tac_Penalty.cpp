#include "stdafx.h"
#include "Tac_Penalty.h"

CTac_Penalty::CTac_Penalty() // 实例化类时自动执行的函数
{
    m_nTacStep = TAC_STEP_READY; // 1
    m_nGoStep = GO_STEP_READY;   // 11
}

CTac_Penalty::~CTac_Penalty()
{
}

void CTac_Penalty::Reset()
{
    m_nTacStep = TAC_STEP_READY;
    m_nGoStep = GO_STEP_READY;
}

void CTac_Penalty::PlanTac(stTactics *outTactics) // 入口函数
{
    pTactics = outTactics;
    switch (m_nTacStep)
    {
    case TAC_STEP_READY:
        Step_Ready();
        break;
    case TAC_STEP_GO:
        Step_Go();
        break;
    }
}

/************************************************************************/
/* 我方罚球准备跑位                                                      */
/************************************************************************/
void CTac_Penalty::Step_Ready()
{
    // 我方罚球跑位
    uRole *pRole = pTactics->role;
    if (pDataSummary->bNoOneSeeBall == false)
    {
        strTacticInfo = "我方罚球跑位 - 正常";
        pRole[0].ctrl = CTRL_GOALKEEP;
        // 其他队员跑动到指定点
        for (int i = 1; i < 5; i++)
        {
            pRole[i].ctrl = CTRL_MOVETO;
            pRole[i].nDest_angle = 0;
        }

        uBall tmpball = pDataSummary->cGlobalBall;

        // 罚球队员站到中圈
        pRole[1].nDest_x = 900;
        pRole[1].nDest_y = 600;
        pRole[1].nDest_angle = 0;

        // 其他球员在场边
        pRole[2].nDest_x = 800;
        pRole[2].nDest_y = 100;
        pRole[2].nDest_angle = 90;
        pRole[3].nDest_x = 650;
        pRole[3].nDest_y = 100;
        pRole[3].nDest_angle = 90;
        pRole[4].nDest_x = 500;
        pRole[4].nDest_y = 100;
        pRole[4].nDest_angle = 90;
    }
    else
    {
        strTacticInfo = "我方罚球跑位 - 未检测到球";
        pRole[0].ctrl = CTRL_GOALKEEP;
        // 其他队员跑动到指定点
        for (int i = 1; i < 5; i++)
        {
            pRole[i].ctrl = CTRL_MOVETO;
            pRole[i].nDest_angle = 0;
        }

        uBall tmpball = pDataSummary->cGlobalBall;

        // 罚球队员站到中圈
        pRole[1].nDest_x = 900;
        pRole[1].nDest_y = 600;
        pRole[1].nDest_angle = 0;

        // 其他球员在场边
        pRole[2].nDest_x = 800;
        pRole[2].nDest_y = 100;
        pRole[2].nDest_angle = 90;
        pRole[3].nDest_x = 650;
        pRole[3].nDest_y = 100;
        pRole[3].nDest_angle = 90;
        pRole[4].nDest_x = 500;
        pRole[4].nDest_y = 100;
        pRole[4].nDest_angle = 90;
    }
}

/************************************************************************/
/* 我方罚球开始                                                             */
/************************************************************************/
void CTac_Penalty::Step_Go()
{
    // 我方罚球进行中...
    uBall tmpball = pDataSummary->cGlobalBall;
    uRole *pRole = pTactics->role;

    CPoint posPenalty; // 罚球点坐标
    posPenalty.x = 1500;
    posPenalty.y = 600;

    switch (m_nGoStep)
    {
    case GO_STEP_READY: // 罚球队员未接近罚球点
        strTacticInfo = "我方罚球开始 - 接近罚球点";
        if (isSomeoneNear(pDataSummary->agent, posPenalty.x, posPenalty.y, DIST_TAC_PENALTY) == false)
        {
            // 在罚球队员未接近罚球点前，一直让罚球队员靠近罚球点
            pRole[1].ctrl = CTRL_MOVETO;
            pRole[1].nDest_x = posPenalty.x;
            pRole[1].nDest_y = posPenalty.y;
            pRole[1].nDest_angle = 0;
        }
        else
        {
            // 罚球队员接近罚球点，进入罚球状态
            m_nGoStep = GO_STEP_ACTION;
        }
        break;

    case GO_STEP_ACTION:
        // 罚球队员已经靠近过罚球点，执行罚球动作
        strTacticInfo = "我方罚球开始 - 开始罚球！";
        pRole[1].ctrl = CTRL_PENALTY;
        break;
    }
}
