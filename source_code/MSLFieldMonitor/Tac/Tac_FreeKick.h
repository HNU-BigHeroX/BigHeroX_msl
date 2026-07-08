#pragma once
#include "ITactic.h"
#include "Tac_Playing.h"
// #include "../DepthMap/Map_FreeKick_Catch.h"
#include "../CatchPosition.h"
class CTac_FreeKick : public ITactic
{
  public:
    CTac_FreeKick();
    ~CTac_FreeKick();
    void Reset();                        // 重置
    void PlanTac(stTactics *outTactics); // 入口

    void Step_Ready(); // 开球准备跑位
    void Step_Go();    // 开球执行

    int nGoStep; // 开球后的更细步骤

    int nCatchRoleIndex; // 开球传球跑位目标角色index
    int nPassRoleIndex;  // 开球接球跑位目标角色index

    ITactic *pTacPlaying;
    CCatchPosition *pCatchPosition;

    bool bCornerKick;

    POINT posStartCatch;
    POINT posStartPass;

    bool isALongPass();
    bool bLongPass;

    //	CMap_FreeKick_Catch m_map_freekick_catch; //用深度图计算FreeKick_Ready时接球队员的位置
    //	CDialog* pWndMap;
};
