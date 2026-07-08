#pragma once
#include "ITactic.h"
#include "../ToAgent.h"
#include "../CatchPosition.h"
class CTac_TechChallenge : public ITactic
{
  public:
    CTac_TechChallenge();
    ~CTac_TechChallenge();

    void Reset();
    void PlanTac(stTactics *outTactics);
    void Start();

    void Step_Ready(); // 技术挑战准备跑位
    void Step_Go();    // 技术挑战开始执行

    void RoleABCAllocate(); // 分配ABC队员ID号

    int nGoStep; // 开球后的更细步骤

    CToAgent *pToAgent;
    CCatchPosition *pCatchPosition;

    int nPlayerA_ID;    // 队员A：从对方半场往己方半场传球
    bool bPlayerA_Done; // 队员A的任务完成
    int nPlayerB_ID;    // 队员B：从己方半场往对方半场传球
    bool bPlayerB_Done; // 队员B的任务完成
    int nPlayerC_ID;    // 队员C：在对方半场接球后射门
    bool bPlayerC_Done; // 队员C的任务完成
};
