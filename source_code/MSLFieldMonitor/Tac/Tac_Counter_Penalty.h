#pragma once
#include "ITactic.h"
class CTac_Counter_Penalty : public ITactic
{
  public:
    CTac_Counter_Penalty();
    ~CTac_Counter_Penalty();

    void Reset();                        // 重置
    void PlanTac(stTactics *outTactics); // 入口

    void Step_Ready(); // 己方罚球准备跑位
    void Step_Go();    // 己方罚球执行

    int nGoStep; // 己方罚球后的更细步骤
};
