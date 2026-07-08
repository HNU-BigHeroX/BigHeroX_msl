#pragma once
#include "ITactic.h"
#include "Tac_Playing.h"
class CTac_DroppedBall : public ITactic
{
  public:
    CTac_DroppedBall();
    ~CTac_DroppedBall();
    void Reset();                        // 重置
    void PlanTac(stTactics *outTactics); // 入口

    void Step_Ready(); // 开球准备跑位
    void Step_Go();    // 开球执行

    int nGoStep; // 开球后的更细步骤

    CPoint posFocusOnBall; // 记录警戒队员应该跑到的位置
    ITactic *pTacPlaying;
};
