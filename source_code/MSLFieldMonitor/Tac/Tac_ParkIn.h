#pragma once
#include "ITactic.h"
class CTac_ParkIn : public ITactic
{
  public:
    CTac_ParkIn();
    ~CTac_ParkIn();

    void Reset();
    void PlanTac(stTactics *outTactics);
};
