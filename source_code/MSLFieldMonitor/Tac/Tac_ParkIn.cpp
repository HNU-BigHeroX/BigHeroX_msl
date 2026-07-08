#include "stdafx.h"
#include "Tac_ParkIn.h"

CTac_ParkIn::CTac_ParkIn()
{
}

CTac_ParkIn::~CTac_ParkIn()
{
}

void CTac_ParkIn::Reset()
{
}

void CTac_ParkIn::PlanTac(stTactics *outTactics)
{
    // 机器人入场的站位
    uRole *pRole = outTactics->role;
    pRole[0].ctrl = CTRL_MOVETO; // 11
    pRole[0].nDest_x = 50;
    pRole[0].nDest_y = 600;
    pRole[0].nDest_angle = 0;

    pRole[1].ctrl = CTRL_MOVETO;
    pRole[1].nDest_x = 800;
    pRole[1].nDest_y = 700;
    pRole[1].nDest_angle = 0;

    pRole[2].ctrl = CTRL_MOVETO;
    pRole[2].nDest_x = 800;
    pRole[2].nDest_y = 500;
    pRole[2].nDest_angle = 0;

    pRole[3].ctrl = CTRL_MOVETO;
    pRole[3].nDest_x = 300;
    pRole[3].nDest_y = 900;
    pRole[3].nDest_angle = 0;

    pRole[4].ctrl = CTRL_MOVETO;
    pRole[4].nDest_x = 300;
    pRole[4].nDest_y = 300;
    pRole[4].nDest_angle = 0;
}
