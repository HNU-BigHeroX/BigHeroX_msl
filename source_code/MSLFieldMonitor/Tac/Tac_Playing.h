#pragma once
#include "ITactic.h"
#include "../CatchPosition.h"
class CTac_Playing : public ITactic
{
  public:
    void SubTacDef(uRole *pRole);
    void SubTacAttack(uRole *pRole);
    CTac_Playing();
    ~CTac_Playing();

    CCatchPosition *pCatchPosition;

    POINT posSelfCatch;

    void Reset();
    void PlanTac(stTactics *outTactics);

    bool m_bOppoHandlingBall();
    void SubTac_BallNoSeen(uRole *pRole);
    void SubTacToPass(uRole *pRole);
    void SubTacPassing(uRole *pRole);
    void SubTacGoal(uRole *pRole);

    void SubTacOppoCtrlBall(uRole *pRole);
    void SubTacOppoPassing(uRole *pRole);

    void AreaDefRoles(uRole *pRole, int inLeftBack, int inRightBack, int inMidBack);
};
