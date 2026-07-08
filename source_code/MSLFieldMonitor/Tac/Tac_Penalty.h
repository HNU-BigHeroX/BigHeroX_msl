/*
罚球（点球）策略，基本不会使用到
*/
#pragma once
#include "ITactic.h"
class CTac_Penalty : public ITactic
{
  public:
    CTac_Penalty();
    ~CTac_Penalty();

    void Reset();                        // 重置
    void PlanTac(stTactics *outTactics); // 入口

    void Step_Ready(); // 己方罚球准备跑位
    void Step_Go();    // 己方罚球执行

    int nGoStep; // 己方罚球后的更细步骤
};
