// 对方开球策略
#pragma once
#include "ITactic.h"
class CTac_Counter_KickOff : public ITactic
{
  public:
    CTac_Counter_KickOff();
    ~CTac_Counter_KickOff();
    void Reset();                        // 重置
    void PlanTac(stTactics *outTactics); // 入口

    void Step_Ready(); // 开球准备跑位
    void Step_Go();    // 开球执行

    int nGoStep; // 开球后的更细步骤

    bool bWaitLeft;   // 开球站位时偏向左边（接近对方接球队员）
    int nLeftCount;   // 对方接球队员在左侧的计数
    int nRightCount;  // 对方接球队员在右侧的计数
    bool bAdjustDone; // 调整完毕
};
