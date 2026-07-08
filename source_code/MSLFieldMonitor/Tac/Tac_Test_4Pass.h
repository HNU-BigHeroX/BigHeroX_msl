#pragma once
#include "ITactic.h"
class CTac_Test_4Pass : public ITactic
{
  public:
    CTac_Test_4Pass();
    ~CTac_Test_4Pass();

    void Reset();
    void PlanTac(stTactics *outTactics);
    void Start();

    int m_nStep;       // 有限状态机变量
    int m_nIndexCatch; // 当前接球点下标
    void PassTo(int inCatchIndex);
};
