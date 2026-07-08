#pragma once
#include "ITactic.h"
#include "../CatchPosition.h"
class CTac_Test_CatchMove : public ITactic
{
  public:
    CTac_Test_CatchMove();
    ~CTac_Test_CatchMove();

    CCatchPosition *pCatchPosition;

    void Reset();
    void PlanTac(stTactics *outTactics);
};
