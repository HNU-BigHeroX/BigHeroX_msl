#pragma once
#include "ITactic.h"
class CTac_Off : public ITactic
{
  public:
    CTac_Off();
    ~CTac_Off();

    void Reset();
    void PlanTac(stTactics *outTactics);
};
