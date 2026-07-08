#pragma once
#include "ITactic.h"
class CTac_TechChallenge_Simple : public ITactic
{
  public:
    CTac_TechChallenge_Simple();
    ~CTac_TechChallenge_Simple();

    void Reset();
    void PlanTac(stTactics *outTactics);
    void Start();

    void Step_Ready(); // 技术挑战准备跑位
    void Step_Go();    // 技术挑战开始执行
};
