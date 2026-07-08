/*
±»»¸‘›Õ£
*/
#pragma once
#include "ITactic.h"
class CTac_Stop : public ITactic
{
  public:
    CTac_Stop();
    ~CTac_Stop();
    void Reset();
    void PlanTac(stTactics *outTactics);
};
