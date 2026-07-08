#pragma once
#include "ITactic.h"
class CTac_Test_AllDef : public ITactic
{
  public:
    CTac_Test_AllDef();
    ~CTac_Test_AllDef();

    void Reset();
    void PlanTac(stTactics *outTactics);
    void Start();

    ITactic *pTacPlaying;
};
