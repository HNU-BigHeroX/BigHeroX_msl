/*
我方开球时的策略
*/
#pragma once
#include "ITactic.h"
#include "Tac_Playing.h"
class CTac_KickOff : public ITactic
{
  public:
    CTac_KickOff();
    ~CTac_KickOff();
    void Reset();                        // 重置
    void PlanTac(stTactics *outTactics); // 入口

    void Step_Ready(); // 开球准备跑位
    void Step_Go();    // 开球执行

    int nGoStep; // 开球后的更细步骤

    int nCatchRoleIndex; // 开球传球跑位目标角色index
    int nPassRoleIndex;  // 开球接球跑位目标角色index

    ITactic *pTacPlaying;
};
