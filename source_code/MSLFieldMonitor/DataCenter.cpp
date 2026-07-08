#include "stdafx.h"
#include "DataCenter.h"

CDataCenter::CDataCenter()
{
    pInfoList = NULL;
    ////////////////////////////////////////////
    /*CString str;
    str.Format("obstacle = %d  uagent = %d enemy = %d", sizeof(obstacle), sizeof(uAgent), sizeof(enemy));
    AfxMessageBox(str);*/
    ////////////////////////////////////////////

    m_dataSummary = new stDataSummary;
    m_dataSummary->agent = new uAgent[AG_NUM];
    m_dataSummary->pre_obstacle = new obstacle[FULL_OST_NUM];
    m_dataSummary->t_enemy = new enemy[20];
    m_stTactics = new stTactics;
    m_stTactics->role = new uRole[AG_NUM * 2];
    // 数据概况的初始化
    for (size_t i = 0; i < 5; i++)
    {
        memset(&(m_dataSummary->agent[i]), 0, sizeof(uAgent));
        // agents
        m_dataSummary->agent[i].ID = i + 1;
        m_dataSummary->agent[i].x = 100 + i * 200;
        m_dataSummary->agent[i].y = 500;
        m_dataSummary->agent[i].dest_x = m_dataSummary->agent[i].x + 500;
        m_dataSummary->agent[i].dest_y = m_dataSummary->agent[i].y + 100;
        m_dataSummary->agent[i].status = CTRL_MOVETO;
        m_dataSummary->agent[i].angle = 0;
        m_dataSummary->agent[i].foundball = false;
        m_dataSummary->agent[i].dist_ball = 10000;
        m_dataSummary->agent[i].online = false;
        m_dataSummary->agent[i].allocated = false;

        // balls
        m_dataSummary->agent[i].ball.ID = i + 1;
        m_dataSummary->agent[i].ball.updated = false;
        m_dataSummary->agent[i].ball.x = m_dataSummary->agent[i].x;
        m_dataSummary->agent[i].ball.y = m_dataSummary->agent[i].y + 50;
    }
    m_dataSummary->bNoOneSeeBall = true;
    m_dataSummary->nNoOneSeenBallCount = 0;
    m_dataSummary->kDistance = 1.0f;
    m_dataSummary->bAction = false;
    // m_dataSummary->nStartTimeCountSec = 20;	//初始化一个大于10的数，地图上就不会显示
    m_dataSummary->nLastTacReady = MATCH_DROPPEDBALL_READY;
    m_dataSummary->bPassKick = false;
    m_dataSummary->bPassing = false;
    m_dataSummary->nPassKickTime = 0;
    m_dataSummary->nPassKickTimeCountSec = 0;
    m_dataSummary->nCatchID = 0;
    m_dataSummary->posCatch.x = 0;
    m_dataSummary->posCatch.y = 0;
    m_dataSummary->nPassID = 0;
    m_dataSummary->posPass.x = 0;
    m_dataSummary->posPass.y = 0;
    m_dataSummary->posPlayerIn.x = 900;
    m_dataSummary->posPlayerIn.y = -50;
    m_dataSummary->nStartTimeCountSec = 0;
    m_dataSummary->nTimeLastStartSec = 0;
    m_dataSummary->posTouchBall.x = 900;
    m_dataSummary->posTouchBall.y = 600;
    m_dataSummary->bHoldBall = false;
    m_dataSummary->bCtrlBallBackHalf = false;
    m_dataSummary->posOppoTouchBall.x = 0; // 对方触球点（其实最后一次触球就很有参考意义）
    m_dataSummary->posOppoTouchBall.y = 0;
    m_dataSummary->bOppoTouchedBall = false; // 对方是否碰过球

    m_dataSummary->posPassToEnemy[0].x = 0;
    m_dataSummary->posPassToEnemy[0].y = 0;
    m_dataSummary->posPassToEnemy[1].x = 0;
    m_dataSummary->posPassToEnemy[1].y = 0;
    m_dataSummary->nNumOfPassToEnemy = 0;

    m_dataSummary->posPassToTeammate.x = 0;
    m_dataSummary->posPassToTeammate.y = 0;
    m_dataSummary->nNumOfPassToTeammate = 0;

    m_dataSummary->arObstIndexBackDef[0] = 0;
    m_dataSummary->arObstIndexBackDef[1] = 0;
    m_dataSummary->arObstIndexBackDef[2] = 0;
    m_dataSummary->nNumOfObstBackDef = 0;

    m_dataSummary->cGlobalBall.x = 0;
    m_dataSummary->cGlobalBall.y = 0;
    m_dataSummary->bNoOneSeeBall = true;

    // 障碍物信息融合
    memset(m_dataSummary->t_enemy, 0, 20 * sizeof(enemy));
    m_dataSummary->NumOfemy = 0;

    // 策略数据的初始化
    for (i = 0; i < AG_NUM; i++)
    {
        m_stTactics->role[i].ctrl = 0;
        m_stTactics->role[i].allocated_ID = 0;
        m_stTactics->role[i].def_angle = 0;
        m_stTactics->role[i].def_dist = 0;
        m_stTactics->role[i].nDest_angle = 0;
        m_stTactics->role[i].nDest_x = 0;
        m_stTactics->role[i].nDest_y = 0;
        m_stTactics->role[i].nPassTo_x = 0;
        m_stTactics->role[i].nPassTo_y = 0;
        m_stTactics->role[i].re_ballx = 0;
        m_stTactics->role[i].re_bally = 0;
    }

    m_nMatchMode = MATCH_OFF;
    pCurTactic = &m_tac_Off;

    m_catchPosition.pDataSummary = m_dataSummary;
    m_ballMoveEstimation.pDataSummary = m_dataSummary;

    // 初始化
    m_tac_Off.pDataSummary = m_dataSummary;
    m_tac_Stop.pDataSummary = m_dataSummary;
    m_tac_ParkIn.pDataSummary = m_dataSummary;
    m_tac_KickOff.pDataSummary = m_dataSummary;
    m_tac_FreeKick.pDataSummary = m_dataSummary;
    m_tac_Playing.pDataSummary = m_dataSummary;
    m_tac_Counter_KickOff.pDataSummary = m_dataSummary;
    m_tac_Counter_FreeKick.pDataSummary = m_dataSummary;
    m_tac_DroppedBall.pDataSummary = m_dataSummary;
    m_tac_Penalty.pDataSummary = m_dataSummary;
    m_tac_Counter_Penalty.pDataSummary = m_dataSummary;
    m_tac_Test_Passing.pDataSummary = m_dataSummary;
    m_tac_TechChallenge.pDataSummary = m_dataSummary;
    m_tac_Test_4Pass.pDataSummary = m_dataSummary;
    m_tac_Test_AllDef.pDataSummary = m_dataSummary;
    m_tac_Test_MoveARound.pDataSummary = m_dataSummary;
    m_tac_Test_CatchMove.pDataSummary = m_dataSummary;
    m_tac_Playing_AreaDef.pDataSummary = m_dataSummary;
    m_tac_TechChallenge_Simple.pDataSummary = m_dataSummary;

    m_tac_KickOff.pTacPlaying = &CUR_TAC_PLAYING_OBJ;
    m_tac_FreeKick.pTacPlaying = &CUR_TAC_PLAYING_OBJ;
    m_tac_DroppedBall.pTacPlaying = &CUR_TAC_PLAYING_OBJ;
    m_tac_Test_AllDef.pTacPlaying = &CUR_TAC_PLAYING_OBJ;

    m_tac_Stop.Reset();
    m_tac_ParkIn.Reset();
    m_tac_Off.Reset();
    m_tac_KickOff.Reset();
    m_tac_FreeKick.Reset();
    m_tac_Playing.Reset();
    m_tac_Counter_KickOff.Reset();
    m_tac_Counter_FreeKick.Reset();
    m_tac_DroppedBall.Reset();
    m_tac_Penalty.Reset();
    m_tac_Counter_Penalty.Reset();
    m_tac_Test_Passing.Reset();
    m_tac_TechChallenge.Reset();
    m_tac_Test_4Pass.Reset();
    m_tac_Test_AllDef.Reset();
    m_tac_Test_MoveARound.Reset();
    m_tac_Test_CatchMove.Reset();
    m_tac_Playing_AreaDef.Reset();
    m_tac_TechChallenge_Simple.Reset();

    m_role_Stop.Reset();
    m_role_MoveTo.Reset();
    m_role_Manual.Reset();
    m_role_Pass.Reset();
    m_role_Catch.Reset();
    m_role_GoalKeeper.Reset();
    m_role_Attack.Reset();
    m_role_FocusOnBall.Reset();
    m_role_PassMove.Reset();
    m_role_CatchMove.Reset();
    m_role_CatchFocus.Reset();
    m_role_DefBall.Reset();
    m_role_DefGoal.Reset();
    m_role_Defence.Reset();
    m_role_Penalty.Reset();
    m_role_SearchBall.Reset();

    m_tac_Playing.pCatchPosition = &m_catchPosition;
    m_tac_Counter_FreeKick.pCatchPosition = &m_catchPosition;
    m_tac_Test_CatchMove.pCatchPosition = &m_catchPosition;
    m_tac_Playing_AreaDef.pCatchPosition = &m_catchPosition;
    m_tac_FreeKick.pCatchPosition = &m_catchPosition;
    m_tac_TechChallenge.pCatchPosition = &m_catchPosition;

    // m_ballMoveEstimation.Init(50);
}

CDataCenter::~CDataCenter()
{
    delete[] m_dataSummary->agent;
    delete[] m_dataSummary->pre_obstacle;
    delete[] m_dataSummary->t_enemy;
    delete m_dataSummary;

    delete[] m_stTactics->role;
    delete m_stTactics;
}

void CDataCenter::SetCoachUDP(CCoachUDP *inCoachUDP)
{
    m_role_Stop.pCoachUDP = inCoachUDP;
    m_role_MoveTo.pCoachUDP = inCoachUDP;
    m_role_Manual.pCoachUDP = inCoachUDP;
    m_role_Pass.pCoachUDP = inCoachUDP;
    m_role_Catch.pCoachUDP = inCoachUDP;
    m_role_GoalKeeper.pCoachUDP = inCoachUDP;
    m_role_Attack.pCoachUDP = inCoachUDP;
    m_role_FocusOnBall.pCoachUDP = inCoachUDP;
    m_role_PassMove.pCoachUDP = inCoachUDP;
    m_role_CatchMove.pCoachUDP = inCoachUDP;
    m_role_CatchFocus.pCoachUDP = inCoachUDP;
    m_role_DefBall.pCoachUDP = inCoachUDP;
    m_role_DefGoal.pCoachUDP = inCoachUDP;
    m_role_Defence.pCoachUDP = inCoachUDP;
    m_role_Penalty.pCoachUDP = inCoachUDP;
    m_role_SearchBall.pCoachUDP = inCoachUDP;

    // 技术挑战赛策略需要直接控制队员
    m_tac_TechChallenge.pToAgent = inCoachUDP->m_ToAgent;
}

/************************************************************************/
/* 逻辑主循环                                                           */
/************************************************************************/
void CDataCenter::MainLoop()
{
    // 更新Start时间计数
    if (m_dataSummary->nStartTimeCountSec < 50)
    {
        UINT nCurTimeSec = GetTickCount() / 1000;
        m_dataSummary->nStartTimeCountSec = nCurTimeSec - m_dataSummary->nTimeLastStartSec;
    }
    /*CString strInfo;
    strInfo.Format("St=%d Cur=%d Stc=%d", m_dataSummary->nTimeLastStartSec, nCurTimeSec,
    m_dataSummary->nStartTimeCountSec); PrintInfoToList(strInfo);*/

    // 球信息融合
    m_Ball_Fussion();

    // 球的运动方向预测（因为进攻球员倾覆剧烈，会造成球的定位抖动，会对防守策略造成干扰，暂时不用）
    /*if (m_dataSummary->bNoOneSeeBall == false)
    {
        m_ballMoveEstimation.Add(m_dataSummary->cGlobalBall.x, m_dataSummary->cGlobalBall.y);
        m_ballMoveEstimation.Estimation();
    }*/

    // 障碍物信息融合
    m_Obst_Fussion();

    // 激活进攻行为
    if (m_dataSummary->bAction == true)
    {
        ChangeMatchModeTo(MATCH_PLAYING);
        m_dataSummary->bAction = false;
    }

    // 决策
    if (pCurTactic != NULL)
    {
        // pCurTactic是关键，指向当前策略对象（详见ChangeMatchModeTo函数）
        pCurTactic->PlanTac(m_stTactics);
        ExecuteTactic(m_stTactics);
    }
}

/************************************************************************/
/* 执行策略                                                                     */
/************************************************************************/
void CDataCenter::ExecuteTactic(stTactics *inTactics)
{
    // 先清空标记
    for (int i = 0; i < AG_NUM; i++)
    {
        m_dataSummary->agent[i].allocated = false;

        // 出故障的机器发停止指令
        if (m_dataSummary->agent[i].online == true && m_dataSummary->agent[i].status == ROBST_ERR)
        {
            m_role_Manual.pCoachUDP->m_ToAgent[i].SimpleCtrl(CTRL_STOP);
        }
    }
    // 按角色号分配队员
    for (i = 0; i < AG_NUM; i++)
    {
        inTactics->role[i].allocated_ID = 0;
        switch (inTactics->role[i].ctrl)
        {
        case CTRL_MANUAL:
            m_role_Manual.Play(m_dataSummary, &(inTactics->role[i]));
            break;
        case CTRL_STOP:
            m_role_Stop.Play(m_dataSummary, &(inTactics->role[i]));
            break;
        case CTRL_MOVETO:
            m_role_MoveTo.Play(m_dataSummary, &(inTactics->role[i]));
            break;
        case CTRL_PASS:
            m_role_Pass.Play(m_dataSummary, &(inTactics->role[i]));
            break;
        case CTRL_CATCH:
            m_role_Catch.Play(m_dataSummary, &(inTactics->role[i]));
            break;
        case CTRL_GOALKEEP:
            m_role_GoalKeeper.Play(m_dataSummary, &(inTactics->role[i]));
            break;
        case CTRL_ATTACK:
            m_role_Attack.Play(m_dataSummary, &(inTactics->role[i]));
            break;
        case CTRL_FOCUSONBALL:
            m_role_FocusOnBall.Play(m_dataSummary, &(inTactics->role[i]));
            break;
        case CTRL_PASS_MOVE:
            m_role_PassMove.Play(m_dataSummary, &(inTactics->role[i]));
            break;
        case CTRL_CATCH_MOVE:
            m_role_CatchMove.Play(m_dataSummary, &(inTactics->role[i]));
            break;
        case CTRL_CATCH_FOCUS:
            m_role_CatchFocus.Play(m_dataSummary, &(inTactics->role[i]));
            break;
        case CTRL_DEF_BALL:
            m_role_DefBall.Play(m_dataSummary, &(inTactics->role[i]));
            break;
        case CTRL_DEF_GOAL:
            m_role_DefGoal.Play(m_dataSummary, &(inTactics->role[i]));
            break;
        case CTRL_DEFENCE:
            m_role_Defence.Play(m_dataSummary, &(inTactics->role[i]));
            break;
        case CTRL_PENALTY:
            m_role_Penalty.Play(m_dataSummary, &(inTactics->role[i]));
            break;
        case CTRL_SEARCHBALL:
            m_role_SearchBall.Play(m_dataSummary, &(inTactics->role[i]));
            break;
        default:
            PrintInfoToList("CDataCenter::ExecuteTactic 出现未定义的Role");
            break;
        }
    }
}

/************************************************************************/
/* 切换比赛模式                                                                     */
/************************************************************************/
void CDataCenter::ChangeMatchModeTo(int inNewMode)
{
    switch (inNewMode)
    {
    case MATCH_TECHCHALLENGE: // 技术挑战
        PrintInfoToList("[技术挑战] 指定动作");
        // 2014国际比赛
        // m_tac_TechChallenge_Simple.Reset();
        // pCurTactic = &m_tac_TechChallenge_Simple;
        // 下面的一项是2014国内赛的时序
        m_tac_TechChallenge.Reset();
        pCurTactic = &m_tac_TechChallenge;
        break;
    case MATCH_TEST_CATCHMOVE: // 绕圈跑
        PrintInfoToList("[比赛模式切换] 接球跑位测试");
        m_tac_Test_CatchMove.Reset();
        pCurTactic = &m_tac_Test_CatchMove;
        break;
    case MATCH_TEST_MOVEAROUND: // 绕圈跑
        PrintInfoToList("[比赛模式切换] 绕圈跑测试");
        m_tac_Test_MoveARound.Reset();
        pCurTactic = &m_tac_Test_MoveARound;
        break;
    case MATCH_TEST_ALLDEF: // 群体防守测试
        PrintInfoToList("[比赛模式切换] 群体防守测试");
        m_tac_Test_AllDef.Reset();
        pCurTactic = &m_tac_Test_AllDef;
        break;
    case MATCH_TEST_4PASS: // 4队友传接球测试
        PrintInfoToList("[比赛模式切换] 4队友传接球测试");
        m_tac_Test_4Pass.Reset();
        pCurTactic = &m_tac_Test_4Pass;
        break;
    case MATCH_TEST_PASSING: // 传接球测试
        PrintInfoToList("[比赛模式切换] 传接球测试");
        m_tac_Test_Passing.Reset();
        pCurTactic = &m_tac_Test_Passing;
        break;
    case MATCH_OFF: // 非自主模式
        PrintInfoToList("[比赛模式切换] 非自主模式");
        pCurTactic = &m_tac_Off;
        break;
    case MATCH_STOP: // 比赛暂停
        PrintInfoToList("[比赛模式切换] -比赛暂停-");
        m_dataSummary->nStartTimeCountSec = 100;
        pCurTactic = &m_tac_Stop;
        break;
    case MATCH_PLAYING:
        PrintInfoToList("[比赛模式切换] -自主模式-");
        CUR_TAC_PLAYING_OBJ.Reset();
        pCurTactic = &CUR_TAC_PLAYING_OBJ;
        break;
    case MATCH_PARK_IN: // 机器人入场
        PrintInfoToList("[比赛模式切换] 机器人入场");
        pCurTactic = &m_tac_ParkIn;
        break;
    case MATCH_KICKOFF_READY: // 我方开球
        PrintInfoToList("[比赛模式切换] 我方开球");
        m_dataSummary->nLastTacReady = MATCH_KICKOFF_READY;
        m_tac_KickOff.Reset();
        pCurTactic = &m_tac_KickOff;
        break;
    case MATCH_COUNTER_KICKOFF_READY: // 对方开球
        PrintInfoToList("[比赛模式切换] 对方开球");
        m_dataSummary->nLastTacReady = MATCH_COUNTER_KICKOFF_READY;
        m_tac_Counter_KickOff.Reset();
        pCurTactic = &m_tac_Counter_KickOff;
        break;
    case MATCH_FREEKICK_READY: // 我方任意球
        PrintInfoToList("[比赛模式切换] 我方任意球");
        m_dataSummary->nLastTacReady = MATCH_FREEKICK_READY;
        m_tac_FreeKick.Reset();
        pCurTactic = &m_tac_FreeKick;
        break;
    case MATCH_CORNERKICK_READY: // 我方角球
        PrintInfoToList("[比赛模式切换] 我方任意球");
        m_dataSummary->nLastTacReady = MATCH_FREEKICK_READY;
        m_tac_FreeKick.Reset();
        m_tac_FreeKick.bCornerKick = true;
        pCurTactic = &m_tac_FreeKick;
        break;
    case MATCH_COUNTER_FREEKICK_READY: // 对方任意球
        PrintInfoToList("[比赛模式切换] 对方任意球");
        m_dataSummary->nLastTacReady = MATCH_COUNTER_FREEKICK_READY;
        m_tac_Counter_FreeKick.Reset();
        pCurTactic = &m_tac_Counter_FreeKick;
        break;
    case MATCH_COUNTER_CORNERKICK_READY: // 对方角球
        PrintInfoToList("[比赛模式切换] 对方任意球");
        m_dataSummary->nLastTacReady = MATCH_COUNTER_FREEKICK_READY;
        m_tac_Counter_FreeKick.Reset();
        m_tac_Counter_FreeKick.bCounterCornerKick = true;
        pCurTactic = &m_tac_Counter_FreeKick;
        break;
    case MATCH_DROPPEDBALL_READY:
        PrintInfoToList("[比赛模式切换] 争球");
        m_dataSummary->nLastTacReady = MATCH_DROPPEDBALL_READY;
        m_tac_DroppedBall.Reset();
        pCurTactic = &m_tac_DroppedBall;
        break;
    case MATCH_PENALTY_READY:
        PrintInfoToList("[比赛模式切换] 我方罚球");
        m_dataSummary->nLastTacReady = MATCH_PENALTY_READY;
        m_tac_Penalty.Reset();
        pCurTactic = &m_tac_Penalty;
        break;
    case MATCH_COUNTER_PENALTY_READY:
        PrintInfoToList("[比赛模式切换] 对方罚球");
        m_dataSummary->nLastTacReady = MATCH_COUNTER_PENALTY_READY;
        m_tac_Counter_Penalty.Reset();
        pCurTactic = &m_tac_Counter_Penalty;
        break;
    case MATCH_START: // Start！
        PrintInfoToList("[比赛模式切换] START！");
        m_dataSummary->nTimeLastStartSec = GetTickCount() / 1000;
        m_dataSummary->nStartTimeCountSec = 0;
        if (pCurTactic == &m_tac_Stop)
        {
            PrintInfoToList("[操作失误]错误点击了Stop和Start");
            ChangeMatchModeTo(m_dataSummary->nLastTacReady);
            // 先执行一次Ready策略，完成跑位运算
            if (pCurTactic != NULL)
            {
                // pCurTactic是关键，指向当前策略对象（详见ChangeMatchModeTo函数）
                pCurTactic->PlanTac(m_stTactics);
                ExecuteTactic(m_stTactics);
            }
        }
        if (pCurTactic != NULL)
        {
            pCurTactic->Start();
        }
        break;
    default:
        PrintInfoToList("CDataCenter::ChangeMatchModeTo 出现未定义的Tac");
        break;
    }
    // 比赛模式变化，并且是Ready状态，则调用其初始化函数
    if (inNewMode != m_nMatchMode && pCurTactic != NULL)
    {
        m_nMatchMode = inNewMode;
    }
}

static bool tmpNoOneSeeBall = false;
void CDataCenter::m_Ball_Fussion()
{
    // m_dataSummary->bNoOneSeeBall = true;
    tmpNoOneSeeBall = true;
    // 先计算每个队员的球信息
    uAgent *pAgent = m_dataSummary->agent;
    for (int i = 0; i < AG_NUM; i++)
    {
        if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR && pAgent[i].foundball == true)
        {
            // 该队员看到球了，进行球的数值计算
            tmpNoOneSeeBall = false;
            pAgent[i].dist_ball = m_CalDist(pAgent[i].x, pAgent[i].y, pAgent[i].ball.x, pAgent[i].ball.y);
        }
    }

    if (tmpNoOneSeeBall == false) // 看得见球
    {
        // 根据每个球员的球信息来进行融合
        //[方法一] 以离球最近的队员看到的球坐标作为全局球坐标
        int nNearBallAgIndex = -1;
        int nMinDistToBall = 10000;
        for (int i = 0; i < AG_NUM; i++)
        {
            if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR && pAgent[i].foundball == true)
            {
                // 检测该球员是否比上一个最小值小
                if (pAgent[i].dist_ball < nMinDistToBall)
                {
                    nNearBallAgIndex = i;
                    nMinDistToBall = pAgent[nNearBallAgIndex].dist_ball;
                }
            }
        }
        if (nNearBallAgIndex >= 0 && nNearBallAgIndex < AG_NUM)
        {
            m_dataSummary->cGlobalBall.ID = pAgent[nNearBallAgIndex].ID;
            m_dataSummary->cGlobalBall.x = pAgent[nNearBallAgIndex].ball.x;
            m_dataSummary->cGlobalBall.y = pAgent[nNearBallAgIndex].ball.y;
            m_dataSummary->cGlobalBall.updated = pAgent[nNearBallAgIndex].ball.updated;
        }
    }

    // 将融合后的球数据赋值到Role里发送给机器人做参考
    for (i = 0; i < AG_NUM; i++)
    {
        m_stTactics->role[i].re_ballx = m_dataSummary->cGlobalBall.x;
        m_stTactics->role[i].re_bally = m_dataSummary->cGlobalBall.y;
    }

    // 根据球消失后的计数来判断球是否将球消失标志置位
    if (tmpNoOneSeeBall == false)
    {
        m_dataSummary->bNoOneSeeBall = false; // 看见球了马上置位为看见球
        m_dataSummary->nNoOneSeenBallCount = 0;
    }
    else
    {
        m_dataSummary->nNoOneSeenBallCount++; // 看不见球了，开始计数
        if (m_dataSummary->nNoOneSeenBallCount > TIMEOUT_NOONESEENBALL)
        {
            m_dataSummary->bNoOneSeeBall = true;
            m_dataSummary->nNoOneSeenBallCount = TIMEOUT_NOONESEENBALL;
        }
    }
}

void CDataCenter::m_Obst_Fussion()
{
    m_ObstFusion.Fusion(m_dataSummary);
    // m_ObstF_Zwj.Fusion(m_dataSummary);

    m_ObstF_Zwj.SortEnemyDistToOurGoal(m_dataSummary); // 融合后的障碍物按距己方球门由近到远排序

    // 跟踪障碍物
    m_ObstF_Zwj.TrackEnemy(m_dataSummary);
    // 复制追踪结果到数据中心(注释掉下面这句后，追踪将不会对决策数据造成影响)
    m_ObstF_Zwj.SetResult(m_dataSummary);

    // 看不到球时疑似带球目标的计算
    m_ObstF_Zwj.FindObstHoldBallInNoSeen(m_dataSummary);
}

int CDataCenter::m_CalDist(int inx1, int iny1, int inx2, int iny2)
{
    double ret = (inx1 - inx2) * (inx1 - inx2) + (iny1 - iny2) * (iny1 - iny2);
    ret = (int)sqrt(ret);
    return (int)ret;
}

// 输出信息到List控件
void CDataCenter::PrintInfoToList(CString inInfo)
{
    if (pInfoList != NULL)
    {
        pInfoList->AddString(inInfo);
        pInfoList->SetCurSel(pInfoList->GetCount() - 1);

        if (pInfoList->GetCount() > 1000)
        {
            pInfoList->ResetContent();
        }
    }
}

CString CDataCenter::m_GetObstInfo()
{
    CString strTmp;
    CString strInfo;
    for (int i = 0; i < m_dataSummary->NumOfemy; i++)
    {
        strTmp.Format("%d(%d,%d) ", i, m_dataSummary->t_enemy[i].x, m_dataSummary->t_enemy[i].y);
        strInfo += strTmp;
    }

    return strInfo;
}
