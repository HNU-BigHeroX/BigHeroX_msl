#include "stdafx.h"
#include "Tac_Off.h"

CTac_Off::CTac_Off()
{
}

CTac_Off::~CTac_Off()
{
}

void CTac_Off::Reset()
{
}

void CTac_Off::PlanTac(stTactics *outTactics)
{
    strTacticInfo = "ÊÖ¶¯¿ØÖÆ";
    uRole *pRole = outTactics->role;
    pRole[0].ctrl = CTRL_MANUAL; // 9
    pRole[1].ctrl = CTRL_MANUAL;
    pRole[2].ctrl = CTRL_MANUAL;
    pRole[3].ctrl = CTRL_MANUAL;
    pRole[4].ctrl = CTRL_MANUAL;
}
