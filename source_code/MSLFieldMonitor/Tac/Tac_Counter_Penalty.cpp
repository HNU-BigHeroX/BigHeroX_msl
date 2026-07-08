#include "stdafx.h"
#include "Tac_Counter_Penalty.h"

CTac_Counter_Penalty::CTac_Counter_Penalty()
{
    m_nTacStep = TAC_STEP_READY;
    m_nGoStep = GO_STEP_READY;
}

CTac_Counter_Penalty::~CTac_Counter_Penalty()
{
}

void CTac_Counter_Penalty::Reset()
{
    m_nTacStep = TAC_STEP_READY;
    m_nGoStep = GO_STEP_READY;
}

void CTac_Counter_Penalty::PlanTac(stTactics *outTactics)
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
/* 对方罚球准备跑位                                                      */
/************************************************************************/
void CTac_Counter_Penalty::Step_Ready()
{
    // 对方罚球跑位
    uRole *pRole = pTactics->role;
    if (pDataSummary->bNoOneSeeBall == false)
    {
        strTacticInfo = "对方罚球跑位 - 正常";
        pRole[0].ctrl = CTRL_GOALKEEP;
        // 其他队员跑动到指定点
        for (int i = 1; i < 5; i++)
        {
            pRole[i].ctrl = CTRL_MOVETO;
            pRole[i].nDest_angle = 0;
        }

        uBall tmpball = pDataSummary->cGlobalBall;

        // 除了守门员所有球员都在场边
        pRole[1].nDest_x = 800;
        pRole[1].nDest_y = 100;
        pRole[1].nDest_angle = 90;
        pRole[2].nDest_x = 650;
        pRole[2].nDest_y = 100;
        pRole[2].nDest_angle = 90;
        pRole[3].nDest_x = 500;
        pRole[3].nDest_y = 100;
        pRole[3].nDest_angle = 90;
        pRole[4].nDest_x = 350;
        pRole[4].nDest_y = 100;
        pRole[4].nDest_angle = 90;
    }
    else
    {
        strTacticInfo = "对方罚球跑位 - 未检测到球";
        pRole[0].ctrl = CTRL_GOALKEEP;
        // 其他队员跑动到指定点
        for (int i = 1; i < 5; i++)
        {
            pRole[i].ctrl = CTRL_MOVETO;
            pRole[i].nDest_angle = 0;
        }

        // 除了守门员所有球员都在场边
        pRole[1].nDest_x = 800;
        pRole[1].nDest_y = 100;
        pRole[1].nDest_angle = 90;
        pRole[2].nDest_x = 650;
        pRole[2].nDest_y = 100;
        pRole[2].nDest_angle = 90;
        pRole[3].nDest_x = 500;
        pRole[3].nDest_y = 100;
        pRole[3].nDest_angle = 90;
        pRole[4].nDest_x = 350;
        pRole[4].nDest_y = 100;
        pRole[4].nDest_angle = 90;
    }
}

/************************************************************************/
/* 对方罚球开始                                                             */
/************************************************************************/
void CTac_Counter_Penalty::Step_Go()
{
    // 对方罚球进行中...
    uBall tmpball = pDataSummary->cGlobalBall;
    uRole *pRole = pTactics->role;

    strTacticInfo = "对方罚球开始 - 开始罚球！";
    pRole[0].ctrl = CTRL_GOALKEEP;
    // 除了守门员所有球员都在场边
    pRole[1].nDest_x = 800;
    pRole[1].nDest_y = 100;
    pRole[1].nDest_angle = 90;
    pRole[2].nDest_x = 650;
    pRole[2].nDest_y = 100;
    pRole[2].nDest_angle = 90;
    pRole[3].nDest_x = 500;
    pRole[3].nDest_y = 100;
    pRole[3].nDest_angle = 90;
    pRole[4].nDest_x = 350;
    pRole[4].nDest_y = 100;
    pRole[4].nDest_angle = 90;
}
