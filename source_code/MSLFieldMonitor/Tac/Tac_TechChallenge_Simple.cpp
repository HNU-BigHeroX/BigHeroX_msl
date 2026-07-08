#include "stdafx.h"
#include "Tac_TechChallenge_Simple.h"

CTac_TechChallenge_Simple::CTac_TechChallenge_Simple()
{
}

CTac_TechChallenge_Simple::~CTac_TechChallenge_Simple()
{
}

void CTac_TechChallenge_Simple::Reset()
{
    m_nTacStep = TAC_STEP_READY;
}

void CTac_TechChallenge_Simple::PlanTac(stTactics *outTactics)
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

void CTac_TechChallenge_Simple::Start()
{
    m_nTacStep = TAC_STEP_GO;
}

void CTac_TechChallenge_Simple::Step_Ready()
{
    strTacticInfo = "[技术挑战赛] 预备跑位";
    uRole *pRole = pTactics->role;

    pRole[0].ctrl = CTRL_MOVETO;
    pRole[0].nDest_x = 100;
    pRole[0].nDest_y = 600;
    pRole[0].nDest_angle = 0;
    pRole[1].ctrl = CTRL_STOP;
    pRole[2].ctrl = CTRL_STOP;
    pRole[3].ctrl = CTRL_STOP;
    pRole[4].ctrl = CTRL_STOP;
}
void CTac_TechChallenge_Simple::Step_Go()
{
    if (pDataSummary->bNoOneSeeBall == true)
    {

        strTacticInfo = "[技术挑战赛] 开始 - 看不到球";
        uRole *pRole = pTactics->role;
        pRole[0].ctrl = CTRL_SEARCHBALL;
        pRole[1].ctrl = CTRL_STOP;
        pRole[2].ctrl = CTRL_STOP;
        pRole[3].ctrl = CTRL_STOP;
        pRole[4].ctrl = CTRL_STOP;
    }
    else
    {
        strTacticInfo = "[技术挑战赛] 开始";
        uRole *pRole = pTactics->role;
        pRole[0].ctrl = CTRL_ATTACK;
        pRole[1].ctrl = CTRL_STOP;
        pRole[2].ctrl = CTRL_STOP;
        pRole[3].ctrl = CTRL_STOP;
        pRole[4].ctrl = CTRL_STOP;
    }
}
