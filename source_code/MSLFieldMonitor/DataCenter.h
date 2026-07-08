#pragma once
#include "MSL_Structs.h"
#include "CoachUDP.h"
#include "CatchPosition.h"
#include "BallMoveEstimation.h"
// 策略
#include "./Tac/Tac_Off.h"
#include "./Tac/Tac_Stop.h"
#include "./Tac/Tac_ParkIn.h"
#include "./Tac/Tac_KickOff.h"
#include "./Tac/Tac_FreeKick.h"
#include "./Tac/Tac_Playing.h"
#include "./Tac/Tac_Counter_KickOff.h"
#include "./Tac/Tac_Counter_FreeKick.h"
#include "./Tac/Tac_DroppedBall.h"
#include "./Tac/Tac_Penalty.h"
#include "./Tac/Tac_Counter_Penalty.h"
#include "./Tac/Tac_TechChallenge.h"
#include "./Tac/Tac_Test_Passing.h"
#include "./Tac/Tac_Test_4Pass.h"
#include "./Tac/Tac_Test_AllDef.h"
#include "./Tac/Tac_Test_MoveARound.h"
#include "./Tac/Tac_Test_CatchMove.h"
#include "./Tac/Tac_Playing_AreaDef.h"
#include "./tac/Tac_TechChallenge_Simple.h"
// 角色
#include "./Role/Role_Manual.h"
#include "./Role/Role_Stop.h"
#include "./Role/Role_MoveTo.h"
#include "./Role/Role_Pass.h"
#include "./Role/Role_Catch.h"
#include "./Role/Role_GoalKeeper.h"
#include "./Role/Role_Attack.h"
#include "./Role/Role_FocusOnBall.h"
#include "./Role/Role_PassMove.h"
#include "./Role/Role_CatchMove.h"
#include "./Role/Role_CatchFocus.h"
#include "./Role/Role_DefBall.h"
#include "./Role/Role_DefGoal.h"
#include "./Role/Role_Defence.h"
#include "./Role/Role_Penalty.h"
#include "./Role/Role_SearchBall.h"

// 障碍物
#include "ObstFusion.h"
#include "ObstFusion_ZWJ.h"

class CDataCenter
{
  public:
    CDataCenter();
    ~CDataCenter();

    // 数据汇总
    stDataSummary *m_dataSummary;
    stTactics *m_stTactics;
    int m_nMatchMode;

    // 策略
    ITactic *pCurTactic;
    CTac_Off m_tac_Off;
    CTac_Stop m_tac_Stop;
    CTac_ParkIn m_tac_ParkIn;
    CTac_KickOff m_tac_KickOff;
    CTac_FreeKick m_tac_FreeKick;
    CTac_Playing m_tac_Playing;
    CTac_Counter_KickOff m_tac_Counter_KickOff;
    CTac_Counter_FreeKick m_tac_Counter_FreeKick;
    CTac_DroppedBall m_tac_DroppedBall;
    CTac_Penalty m_tac_Penalty;
    CTac_Counter_Penalty m_tac_Counter_Penalty;
    CTac_TechChallenge m_tac_TechChallenge;
    CTac_Test_Passing m_tac_Test_Passing;
    CTac_Test_4Pass m_tac_Test_4Pass;
    CTac_Test_AllDef m_tac_Test_AllDef;
    CTac_Test_MoveARound m_tac_Test_MoveARound;
    CTac_Test_CatchMove m_tac_Test_CatchMove;
    CTac_Playing_AreaDef m_tac_Playing_AreaDef;
    CTac_TechChallenge_Simple m_tac_TechChallenge_Simple;

    // 角色
    CRole_Manual m_role_Manual;
    CRole_MoveTo m_role_MoveTo;
    CRole_Stop m_role_Stop;
    CRole_Pass m_role_Pass;
    CRole_Catch m_role_Catch;
    CRole_GoalKeeper m_role_GoalKeeper;
    CRole_Attack m_role_Attack;
    CRole_FocusOnBall m_role_FocusOnBall;
    CRole_PassMove m_role_PassMove;
    CRole_CatchMove m_role_CatchMove;
    CRole_CatchFocus m_role_CatchFocus;
    CRole_DefBall m_role_DefBall;
    CRole_DefGoal m_role_DefGoal;
    CRole_Defence m_role_Defence;
    CRole_Penalty m_role_Penalty;
    CRole_SearchBall m_role_SearchBall;

    // 障碍物融合
    CObstFusion m_ObstFusion;
    CObstFusion_ZWJ m_ObstF_Zwj;
    void m_Obst_Fussion();
    CString m_GetObstInfo();

    // 接球位置计算
    CCatchPosition m_catchPosition;

    // 球的数据融合
    void m_Ball_Fussion();
    CBallMoveEstimation m_ballMoveEstimation;

    // 主循环
    void MainLoop();

    void SetCoachUDP(CCoachUDP *inCoachUDP);
    void ExecuteTactic(stTactics *inTactics);
    void ChangeMatchModeTo(int inNewMode);
    int m_CalDist(int inx1, int iny1, int inx2, int iny2);

    // 显示控件
    CListBox *pInfoList;
    // 输出信息到List控件
    void PrintInfoToList(CString inInfo);
};
