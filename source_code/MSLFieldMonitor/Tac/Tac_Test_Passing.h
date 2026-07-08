#pragma once
#include "ITactic.h"

class CTac_Test_Passing : public ITactic
{
  public:
    CTac_Test_Passing();
    ~CTac_Test_Passing();
    void Reset();
    void PlanTac(stTactics *outTactics);

    int m_nStep; // 有限状态机变量
};
