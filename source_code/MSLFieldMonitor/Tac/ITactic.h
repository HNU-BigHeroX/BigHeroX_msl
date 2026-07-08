#pragma once
#include "../MSL_Structs.h"
#include <math.h>
#include "../PassSwitch.h"

// m_nTacStep接受下面两个参数
#define TAC_STEP_READY 1 // 开球准备
#define TAC_STEP_GO 2    // 开始！

// Go执行后的子步骤
//  m_nGoStep接受下面的五个参数
#define GO_STEP_READY 11 // 仍未跑位到位，继续跑位
#define GO_STEP_PASS 12  // 各方面已经到位，正在执行传球步骤
#define GO_STEP_CATCH 13 // 球已经传出一定距离，进入接球步骤

#define GO_STEP_ALERT 14 // 对方开球警戒(一旦球动就开始)

#define GO_STEP_ACTION 15 // 行动（己方罚球和对方罚球用到）

class ITactic
{
  public:
    CString strClassName;
    CString strTacticInfo;
    stDataSummary *pDataSummary; // stDataSummary是接受的数据汇总的一个类
    stTactics *pTactics;
    int m_nTacStep;
    int m_nGoStep;

    CPassSwitch m_cPassSwitch;

    CListBox *pInfoList;

  public:
    ITactic();
    ~ITactic();

    int nNumToOurPenaltyArea;
    int nNumToOppoPenaltyArea;

    virtual void Reset() = 0;
    virtual void PlanTac(stTactics *outTactics) = 0;
    virtual void Start();

    void PrintInfo(CString inStr);
    double m_CalDist(int inx1, int iny1, int inx2, int iny2);
    int m_Loca_CalAngle(int inXbase, int inYbase, int inXtarg, int inYtarg);
    CPoint m_Loca_Ray(int inXbase, int inYbase, int inXtarg, int inYtarg, int inDist);
    CPoint m_Loca_Rotate(int inx, int iny, int inCx, int inCy, int inAng);
    bool isSomeoneNear(uAgent *inAgent, int inX, int inY, int inNearDist);
    int GetAgentIndex(int inStatus);
    CPoint m_Fix_KeepInOppoField(int inX, int inY, int inAwayX, int inAwayY, int inAwayDist, int inOppoDist);
    CPoint m_Fix_KeepInTouchLine(int inX, int inY, int inAwayX, int inAwayY, int inAwayDist, int inOppoDist);
    CPoint m_Fix_KeepAwayFrom(int inX, int inY, int inAwayX, int inAwayY, int inAwayDist, int inTeammateDist,
                              int inOppoDist);
    bool m_Chk_InOurPenalty(int inX, int inY);
    bool m_Chk_InOppoPenalty(int inX, int inY);
    bool m_Chk_OutOfField(int inX, int inY);
    bool m_Chk_AwayFromTeammate(int inX, int inY, int inDist);
    bool m_Chk_AwayFromOppo(int inX, int inY, int inDist);
    bool m_Chk_InLeftBack(int inX, int inY);
    bool m_Chk_InRightBack(int inX, int inY);
    bool m_Chk_InMidField(int inX, int inY);
    void m_GetPassCatchID();
    int m_GetAgentIDNear(int inx, int iny);
    int m_GetAgentIDInStatus(int inCtrl);
    void Get3BackObstToDef();
    void Get3ObstCloseToDef();
    void m_BackEnemyFix();
    void m_Fix_MoveToAway();
    int m_MinDistToMoveToRoleBefore(int inX, int inY, int inIndex);
    void SortEnemyDistToOurGoal(stDataSummary *inDataSummary);
};
