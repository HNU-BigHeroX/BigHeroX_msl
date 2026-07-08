/*
敌方自由球应对策略
*/
#pragma once
#include "ITactic.h"
#include "../CatchPosition.h"
class CTac_Counter_FreeKick : public ITactic
{
  public:
    bool chkPosRight(int inX, int inY);
    CPoint FixSingleMove(int inID, int inX, int inY);
    CTac_Counter_FreeKick();
    ~CTac_Counter_FreeKick();
    void Reset();                        // 重置
    void PlanTac(stTactics *outTactics); // 入口

    void Step_Ready(); // 开球准备跑位
    void Step_Go();    // 开球执行

    int nGoStep; // 开球后的更细步骤

    CPoint posFocusOnBall; // 记录警戒队员应该跑到的位置

    bool bCounterCornerKick;

    obstacle *arObstDef; // 需要防守的障碍物列表
    int m_nObstDef;      // 需要防守的障碍物数量
    CCatchPosition *pCatchPosition;

    CPoint m_Loc_DefInOurPenaltyArea();

    // 判断球是否在我方半场的回滞死区
    bool isInOwnField(int inX);
    int nInOwnX;
    int nInOppoX;
    bool resInOwnField;

    // 判断球在左右半场的回滞死区
    bool isInLeftField(int inY);
    int nInLeft;
    int nInRight;
    bool resInLeftField;
};
