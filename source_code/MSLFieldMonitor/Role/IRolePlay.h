// 基类，所有的role都会继承这个类
#pragma once
#include "../MSL_Structs.h"
#include "../CoachUDP.h"
class IRolePlay
{
  public:
    CString strClassName;
    CCoachUDP *pCoachUDP;

  public:
    IRolePlay();
    ~IRolePlay();
    int m_CalDist(int inx1, int iny1, int inx2, int iny2);
    int m_Loca_CalAngle(int inXbase, int inYbase, int inXtarg, int inYtarg);
    CPoint m_Loca_Ray(int inXbase, int inYbase, int inXtarg, int inYtarg, int inDist);
    CPoint m_Loca_Rotate(int inx, int iny, int inCx, int inCy, int inAng);
    bool isSomeoneNear(uAgent *inAgent, int inX, int inY, int inNearDist);
    int m_GetIDInStatus(UINT inState, uAgent *inAgent, bool inAllocated);
    POINT FixMovingPathPoint(int inCurX, int inCurY, int inDestX, int inDestY);
    int m_Ag_GetNearBy(int x, int y, uAgent *inalist);
    stDataSummary *pDataSummary;

    virtual void Reset() = 0;                                           // 初始化
    virtual void Play(stDataSummary *inDataSummary, uRole *inRole) = 0; // 执行
};
