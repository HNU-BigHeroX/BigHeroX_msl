#include "stdafx.h"
#include "Tac_Stop.h"
#include "..\MSL_Structs.h"

CTac_Stop::CTac_Stop()
{
}

CTac_Stop::~CTac_Stop()
{
}

void CTac_Stop::Reset()
{
}

void CTac_Stop::PlanTac(stTactics *outTactics)
{
    strTacticInfo = "±ÈÈüÔÝÍ£";
    uRole *pRole = outTactics->role;
    pRole[0].ctrl = CTRL_STOP;
    pRole[1].ctrl = CTRL_STOP;
    pRole[2].ctrl = CTRL_STOP;
    pRole[3].ctrl = CTRL_STOP;
    pRole[4].ctrl = CTRL_STOP;

    pDataSummary->bPassKick = false;
    pDataSummary->bPassing = false;
}
