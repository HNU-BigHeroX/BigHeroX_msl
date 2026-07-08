#include "stdafx.h"
#include "Tac_Test_AllDef.h"

CTac_Test_AllDef::CTac_Test_AllDef()
{
    pTacPlaying = NULL;
}

CTac_Test_AllDef::~CTac_Test_AllDef()
{
}

void CTac_Test_AllDef::Reset()
{
    if (pTacPlaying != NULL)
    {
        pTacPlaying->Reset();
    }
}

void CTac_Test_AllDef::PlanTac(stTactics *outTactics)
{

    strTacticInfo = "[测试_无进攻策略] 没有进攻球员的比赛决策";

    if (pTacPlaying != NULL)
    {
        pTacPlaying->PlanTac(outTactics);
    }

    uRole *pRole = outTactics->role;
    // 将进攻和动态传球队员设为STOP
    for (int i = 0; i < AG_NUM; i++)
    {
        if (pRole[i].ctrl == CTRL_ATTACK || pRole[i].ctrl == CTRL_PASS_MOVE)
        {
            // pRole[i].ctrl = CTRL_STOP;
            // 将后边的role前移，最后一个用stop补齐
            for (int j = i; j < AG_NUM - 1; j++)
            {
                memcpy(&pRole[j], &pRole[j + 1], sizeof(uRole));
            }
            pRole[AG_NUM - 1].ctrl = CTRL_STOP;
        }
    }
}

void CTac_Test_AllDef::Start()
{
}
