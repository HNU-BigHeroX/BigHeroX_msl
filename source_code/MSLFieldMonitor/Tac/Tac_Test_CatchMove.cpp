#include "stdafx.h"
#include "Tac_Test_CatchMove.h"

CTac_Test_CatchMove::CTac_Test_CatchMove()
{
    pCatchPosition = NULL;
}

CTac_Test_CatchMove::~CTac_Test_CatchMove()
{
}

void CTac_Test_CatchMove::Reset()
{
}

void CTac_Test_CatchMove::PlanTac(stTactics *outTactics)
{
    uRole *pRole = outTactics->role;

    strTacticInfo = "[测试]接球位置跑位";

    // 接球位置计算
    pCatchPosition->CalCatchPosition();

    for (int i = 0; i < 3; i++)
    {
        pRole[i].ctrl = CTRL_CATCH_MOVE;
        pRole[i].nDest_x = pCatchPosition->posCatch[i].x;
        pRole[i].nDest_y = pCatchPosition->posCatch[i].y;
    }

    pRole[3].ctrl = CTRL_STOP;
    pRole[4].ctrl = CTRL_STOP;
}
