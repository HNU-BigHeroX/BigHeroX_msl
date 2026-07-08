#include "stdafx.h"
#include "Tac_FreeKick.h"

CTac_FreeKick::CTac_FreeKick()
{
    pTacPlaying = NULL;
    pCatchPosition = NULL;
    Reset();
}

CTac_FreeKick::~CTac_FreeKick()
{
}

void CTac_FreeKick::Reset()
{
    m_nTacStep = TAC_STEP_READY;
    m_nGoStep = GO_STEP_READY;

    nPassRoleIndex = 1;  // 任意球接球跑位目标角色index
    nCatchRoleIndex = 2; // 任意球传球跑位目标角色index
    bCornerKick = false;
    bLongPass = false;

    m_cPassSwitch.Reset();
}

void CTac_FreeKick::PlanTac(stTactics *outTactics)
{
    pTactics = outTactics;
    switch (m_nTacStep)
    {
    case TAC_STEP_READY:
        Step_Ready();
        break;
    case TAC_STEP_GO:
        Step_Go();
        break;
    }
}

/************************************************************************/
/* 任意球准备跑位                                                         */
/************************************************************************/
void CTac_FreeKick::Step_Ready()
{
    // 任意球跑位
    uRole *pRole = pTactics->role;
    if (pDataSummary->bNoOneSeeBall == false)
    {
        strTacticInfo = "我方任意球跑位 - 正常";
        // 需要接球位置
        pCatchPosition->CalCatchPosition();

        // 队员[1]:守门员
        pRole[0].ctrl = CTRL_GOALKEEP;
        // 其他队员跑动到指定点
        for (int i = 1; i < 5; i++)
        {
            pRole[i].ctrl = CTRL_MOVETO;
            pRole[i].nDest_angle = 0;
        }

        uBall tmpball = pDataSummary->cGlobalBall;

        CString strData, strTmp;
        // 先计算接球点位置
        CPoint posCatchNear =
            m_Loca_Ray(tmpball.x, tmpball.y, 1800, 600, (int)(DIST_TAC_START_CATCH * pDataSummary->kDistance));
        posCatchNear = m_Loca_Rotate(posCatchNear.x, posCatchNear.y, tmpball.x, tmpball.y, 180);
        /*strTmp.Format("(%d,%d)-", posCatchNear.x, posCatchNear.y);
        strData += strTmp;*/
        posCatchNear = m_Fix_KeepInTouchLine(posCatchNear.x, posCatchNear.y, tmpball.x, tmpball.y, 250, 100);
        /*strTmp.Format("(%d,%d)-", posCatchNear.x, posCatchNear.y);
        strData += strTmp;*/
        posCatchNear = m_Fix_KeepInOppoField(posCatchNear.x, posCatchNear.y, tmpball.x, tmpball.y, 250, 100);
        /*strTmp.Format("(%d,%d)-", posCatchNear.x, posCatchNear.y);
        strData += strTmp;*/
        // PrintInfo(strData);

        // 【！】根据接球点位置来确定往前传还是往后传
        CPoint tmp;
        // if (tmpball.x < 800)	//会抖动
        if (m_cPassSwitch.isInOwnField(tmpball.x) == true)
        {
            // PrintInfo("isInOwnField == true");
            // 1、球在后场，传远点（对方半场），接球后射门
            // 队员[2]:传球队员-球-对方球门 为一直线(从球射向对方球门，然后转180度)
            tmp = m_Loca_Ray(tmpball.x, tmpball.y, 1700, 600,
                             (int)(DIST_TAC_START_PASS * pDataSummary->kDistance)); // 最后一个参数是距离
            tmp = m_Loca_Rotate(tmp.x, tmp.y, tmpball.x, tmpball.y, 180);
            pRole[1].nDest_x = tmp.x;
            pRole[1].nDest_y = tmp.y;
            pRole[1].nDest_angle = m_Loca_CalAngle(tmp.x, tmp.y, tmpball.x, tmpball.y);
            pDataSummary->posPass.x = tmp.x;
            pDataSummary->posPass.y = tmp.y;
            nPassRoleIndex = 1;

            // 队员[2]接球跑位
            tmp.x = pCatchPosition->posCatch[0].x;
            tmp.y = pCatchPosition->posCatch[0].y;
            pRole[2].nDest_x = tmp.x;
            pRole[2].nDest_y = tmp.y;
            pRole[2].nDest_angle = m_Loca_CalAngle(tmp.x, tmp.y, tmpball.x, tmpball.y);
            pDataSummary->posCatch.x = tmp.x;
            pDataSummary->posCatch.y = tmp.y;
            nCatchRoleIndex = 2;

            bLongPass = true;
        }
        else
        {
            // PrintInfo("isInOwnField == false");
            // 2、球在前场，传近点直接射门
            // 队员[2]:传球队员-球-接球点 为一直线
            tmp = m_Loca_Ray(tmpball.x, tmpball.y, posCatchNear.x, posCatchNear.y,
                             (int)(DIST_TAC_START_PASS * pDataSummary->kDistance)); // 最后一个参数是距离
            tmp = m_Loca_Rotate(tmp.x, tmp.y, tmpball.x, tmpball.y, 180);
            pRole[1].nDest_x = tmp.x;
            pRole[1].nDest_y = tmp.y;
            pRole[1].nDest_angle = m_Loca_CalAngle(tmp.x, tmp.y, tmpball.x, tmpball.y);
            pDataSummary->posPass.x = tmp.x;
            pDataSummary->posPass.y = tmp.y;
            nPassRoleIndex = 1;

            // 队员[3]:近距接球队员，就在近接球点
            tmp = posCatchNear;
            pRole[2].nDest_x = tmp.x;
            pRole[2].nDest_y = tmp.y;
            pRole[2].nDest_angle = m_Loca_CalAngle(tmp.x, tmp.y, tmpball.x, tmpball.y);
            pDataSummary->posCatch.x = tmp.x;
            pDataSummary->posCatch.y = tmp.y;
            nCatchRoleIndex = 2;

            bLongPass = false;
        }

        // 队员[4]左后卫
        tmp = m_Fix_KeepAwayFrom(300, 800, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, 300, 0, 100);
        pRole[3].nDest_x = tmp.x;
        pRole[3].nDest_y = tmp.y;
        pRole[3].nDest_angle = m_Loca_CalAngle(tmp.x, tmp.y, tmpball.x, tmpball.y);

        // 队员[5]右后卫
        tmp = m_Fix_KeepAwayFrom(300, 400, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, 300, 0, 100);
        pRole[4].nDest_x = tmp.x;
        pRole[4].nDest_y = tmp.y;
        pRole[4].nDest_angle = m_Loca_CalAngle(tmp.x, tmp.y, tmpball.x, tmpball.y);
    }
    else
    {
        strTacticInfo = "我方任意球跑位 - 未检测到球";
        pRole[0].ctrl = CTRL_GOALKEEP;

        if (bCornerKick == false)
        {
            pRole[1].ctrl = CTRL_MOVETO; // 后卫
            pRole[1].nDest_x = 300;
            pRole[1].nDest_y = 600;
            pRole[1].nDest_angle = 0;

            pRole[2].ctrl = CTRL_MOVETO; // 去往上一次球出现的位置
            pRole[2].nDest_x = pDataSummary->cGlobalBall.x;
            pRole[2].nDest_y = pDataSummary->cGlobalBall.x;
            pRole[2].nDest_angle = 0;
        }
        else
        {
            strTacticInfo = "我方角球跑位 - [未检测到球]";
            // 这是个角球，去往对方半场角球位置
            pRole[1].ctrl = CTRL_MOVETO;
            pRole[1].nDest_x = 1600;
            pRole[1].nDest_y = 200;
            pRole[1].nDest_angle = 0;

            pRole[2].ctrl = CTRL_MOVETO; // 去往上一次球出现的位置
            pRole[2].nDest_x = 1600;
            pRole[2].nDest_y = 1000;
            pRole[2].nDest_angle = 0;
        }
        pRole[3].ctrl = CTRL_MOVETO;
        pRole[3].nDest_x = 800;
        pRole[3].nDest_y = 600;
        pRole[3].nDest_angle = 0;

        pRole[4].ctrl = CTRL_MOVETO;
        pRole[4].nDest_x = 800;
        pRole[4].nDest_y = 300;
        pRole[4].nDest_angle = 0;
    }
}

/************************************************************************/
/* 任意球开始                                                             */
/************************************************************************/
void CTac_FreeKick::Step_Go()
{
    // 任意球进行中...
    // 一旦有人进入进攻状态（接到球了！），则激活自主决策
    int i;
    for (i = 0; i < AG_NUM; i++)
    {
        if (pDataSummary->agent[i].online == true && pDataSummary->agent[i].status != ROBST_ERR)
        {
            if (pDataSummary->agent[i].status == CTRL_ATTACK)
            {
                pDataSummary->bAction = true;
                return;
            }
        }
    }
    // 时间超过开球时间（这种情况一般是己方开球不顺利），强行激活自主决策
    if (pDataSummary->nStartTimeCountSec > 12) // 对给两秒对方过来抢球
    {
        pDataSummary->bAction = true;
    }

    uBall tmpball = pDataSummary->cGlobalBall;
    uRole *pRole = pTactics->role;
    CPoint tmp;
    int nCatchID;
    CPoint posCatchNear;

    // 除了GO_STEP_READY不考虑看不到球的情况，否则会发球失误
    strTacticInfo = "我方任意球开始！ - 正常";
    switch (m_nGoStep)
    {
    case GO_STEP_READY:
        strTacticInfo = "我方任意球开始！ - 继续跑位准备中..";
        // 继续计算传球接球队员目标位置，以免因为（球自然滚动）和（READY阶段机器人离球太远，对球定位不准）而出现定位偏差

        Step_Ready();

        // 只有一名队员的情况（未实现，可以考虑将球大力射向对方球门）

        // 传球跑位终止条件1：传球队员和接球队员都就位后才开始执行传球动作
        if (isSomeoneNear(pDataSummary->agent, pDataSummary->posPass.x, pDataSummary->posPass.y,
                          MOVETO_DIST_DEADZONE) &&
            isSomeoneNear(pDataSummary->agent, pDataSummary->posCatch.x, pDataSummary->posCatch.y,
                          MOVETO_DIST_DEADZONE))
        {
            m_nGoStep = GO_STEP_PASS;
            posStartPass.x = pDataSummary->cGlobalBall.x;
            posStartPass.y = pDataSummary->cGlobalBall.y;
        }

        // 传球跑位终止条件2：开始时间超过一定时间，再不开对方就过来抢球了
        if (pDataSummary->nStartTimeCountSec > 5)
        {
            m_nGoStep = GO_STEP_PASS;
            posStartPass.x = pDataSummary->cGlobalBall.x;
            posStartPass.y = pDataSummary->cGlobalBall.y;
        }

        break;

    case GO_STEP_PASS:
        strTacticInfo = "我方任意球开始！ - 进行传球..";
        pCatchPosition->CalCatchPosition();
        if (pDataSummary->bPassKick == true)
        {
            pDataSummary->bPassing = true;

            strTacticInfo.Format("bPassKick(%d,%d)", pDataSummary->posCatch.x, pDataSummary->posCatch.y);
            PrintInfo(strTacticInfo);
        }
        // 计算接球队员位置
        nCatchID = m_GetAgentIDNear(pDataSummary->posCatch.x, pDataSummary->posCatch.y);
        pDataSummary->nCatchID = nCatchID;
        if (nCatchID > 0 && nCatchID <= AG_NUM)
        {
            pDataSummary->posCatch.x = pDataSummary->agent[nCatchID - 1].x;
            pDataSummary->posCatch.y = pDataSummary->agent[nCatchID - 1].y;
        }
        // 传球跑位角色变成传球角色，传球的目标点是接球队员的跑位目标点
        pRole[nPassRoleIndex].ctrl = CTRL_PASS;

        // 接球跑位角色变成接球角色(根据传球位置所处的前后场选择不一样的传球点和接球行为)
        if (bLongPass == false)
        {
            // 近点传接
            pRole[nPassRoleIndex].nDest_x = pDataSummary->posCatch.x;
            pRole[nPassRoleIndex].nDest_y = pDataSummary->posCatch.y;

            pRole[nCatchRoleIndex].ctrl = CTRL_CATCH;
            pRole[nCatchRoleIndex].nDest_x = pDataSummary->posCatch.x;
            pRole[nCatchRoleIndex].nDest_y = pDataSummary->posCatch.y;
        }
        else
        {
            // 远点传接
            pRole[nPassRoleIndex].nDest_x = pCatchPosition->posCatch[0].x;
            pRole[nPassRoleIndex].nDest_y = pCatchPosition->posCatch[0].y;

            // 使用CTRL_CATCH_MOVE，接球队员静止等待传球队员起脚的过程会开始准备动态移动接球
            // 方案一：接球队员静态
            pRole[nCatchRoleIndex].ctrl = CTRL_CATCH_MOVE;
            pRole[nCatchRoleIndex].nDest_x = pDataSummary->posCatch.x;
            pRole[nCatchRoleIndex].nDest_y = pDataSummary->posCatch.y;

            // 方案二：接球队员动态跑位
            // pRole[nCatchRoleIndex].nDest_x = pCatchPosition->posCatch[0].x;
            // pRole[nCatchRoleIndex].nDest_y = pCatchPosition->posCatch[0].y;
        }

        // 将传球队员的kick信号作为切换条件（bPassKick在从CoachUDP获取数据时才会复制，所以不会漏掉）
        if (pDataSummary->bPassKick == true || pDataSummary->bPassing == true)
        {
            m_nGoStep = GO_STEP_CATCH;
            // 传球队员刚出脚，接球位置需要缓存下来，维持接球位置，pDataSummary->posCatch会被冲刷掉
            if (bLongPass == false)
            {
                // 近点传球
                posStartCatch = pDataSummary->posCatch;
            }
            else
            {
                // 远点传球，选择动态传球点
                posStartCatch = pCatchPosition->posCatch[0];
            }
            pDataSummary->bPassing = true;
        }
        break;

    case GO_STEP_CATCH: // 接球状态的意义在于：球传出去之后，让传球队员和其他能做其他动作，不必死等接球动作完成
        strTacticInfo = "我方任意球开始！ - 接球状态..";

        if (pDataSummary->bPassing == true)
        {
            // 传球状态终止条件一：传球超时
            UINT nCurTimeSec = GetTickCount() / 1000;
            pDataSummary->nPassKickTimeCountSec = nCurTimeSec - pDataSummary->nPassKickTime;
            if (pDataSummary->nPassKickTimeCountSec > TIMEOUT_PASSING)
            {
                pDataSummary->bPassing = false;
                pDataSummary->bAction = true;
            }
            // 传球状态终止条件二：接球队员已经拿到球
            if (pDataSummary->nCatchID > 0 && pDataSummary->nCatchID <= 5)
            {
                if (pDataSummary->agent[pDataSummary->nCatchID - 1].x ==
                        pDataSummary->agent[pDataSummary->nCatchID - 1].ball.x &&
                    pDataSummary->agent[pDataSummary->nCatchID - 1].y ==
                        pDataSummary->agent[pDataSummary->nCatchID - 1].ball.y)
                {
                    pDataSummary->bPassing = false;
                    pDataSummary->bAction = true;
                }
            }
            // 传球状态终止条件三：接球队员接球后切换成进攻
            if (pDataSummary->nCatchID > 0 && pDataSummary->nCatchID <= 5)
            {
                if (pDataSummary->agent[pDataSummary->nCatchID - 1].status == CTRL_ATTACK ||
                    pDataSummary->agent[pDataSummary->nCatchID - 1].status == CTRL_PASS_MOVE)
                {
                    pDataSummary->bPassing = false;
                    pDataSummary->bAction = true;
                }
            }
        }

        pDataSummary->posCatch = posStartCatch;
        if (m_GetAgentIDInStatus(CTRL_ATTACK) > 0 || m_GetAgentIDInStatus(CTRL_PASS_MOVE) > 0)
        {
            // 开始自主决策
            pTacPlaying->PlanTac(pTactics);
            pDataSummary->bPassing = false;
            pDataSummary->bAction = true;
            return;
        }
        else
        {
            // 开始中...(球已经传出接球队员等着接球，其他队员可以开始按照比赛开始模式进行各种行动)
            pRole = pTactics->role;
            for (int i = 0; i < AG_NUM; i++)
            {
                if (pRole[i].ctrl == CTRL_CATCH || pRole[i].ctrl == CTRL_CATCH_FOCUS ||
                    pRole[i].ctrl == CTRL_CATCH_MOVE)
                {
                    // 接球跑位角色变成接球角色(根据接球位置所处的前后场选择不一样的传球点和接球行为)
                    int nCatchID = m_GetAgentIDNear(pDataSummary->posCatch.x, pDataSummary->posCatch.y);
                    if (nCatchID < 1 || nCatchID > AG_NUM)
                    {
                        nCatchID = 2;
                        PrintInfo("case GO_STEP_CATCH:nCatchID异常");
                    }
                    // 按传球点计算
                    if (bLongPass == false)
                    {
                        pRole[i].ctrl = CTRL_CATCH;
                        pRole[i].nDest_x = posStartCatch.x;
                        pRole[i].nDest_y = posStartCatch.y;
                    }
                    else
                    {
                        pRole[i].ctrl = CTRL_CATCH_FOCUS;
                        pRole[i].nDest_x = posStartCatch.x;
                        pRole[i].nDest_y = posStartCatch.y;
                    }
                }
            }
        }

        break;
    default:
        PrintInfo("我方任意球开始未知状态");
        m_nGoStep = GO_STEP_PASS;
        break;
    }
}

bool CTac_FreeKick::isALongPass()
{
    int Dist = m_CalDist(posStartPass.x, posStartPass.y, posStartCatch.x, posStartCatch.y);
    if (Dist > 400)
    {
        return true;
    }
    else
    {
        return false;
    }
}
