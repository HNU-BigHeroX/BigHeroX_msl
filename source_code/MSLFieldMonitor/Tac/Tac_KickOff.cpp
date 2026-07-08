#include "stdafx.h"
#include "Tac_KickOff.h"

CTac_KickOff::CTac_KickOff()
{
    pTacPlaying = NULL;          // 暂不清楚这样设置的意义何在
    m_nTacStep = TAC_STEP_READY; // 1
    m_nGoStep = GO_STEP_READY;   // 11

    nPassRoleIndex = 1;  // 开球接球跑位目标角色index
    nCatchRoleIndex = 2; // 开球传球跑位目标角色index
}

CTac_KickOff::~CTac_KickOff()
{
}

void CTac_KickOff::Reset()
{
    m_nTacStep = TAC_STEP_READY; // 1
    m_nGoStep = GO_STEP_READY;   // 11

    nPassRoleIndex = 1;  // 开球接球跑位目标角色index
    nCatchRoleIndex = 2; // 开球传球跑位目标角色index
}

void CTac_KickOff::PlanTac(stTactics *outTactics) // 入口函数
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
/* 开球准备跑位                                                         */
/************************************************************************/
void CTac_KickOff::Step_Ready()
{
    // 开球跑位
    uRole *pRole = pTactics->role;
    if (pDataSummary->bNoOneSeeBall == false) // 有发现球
    {
        strTacticInfo = "己方开球跑位 - 正常";
        pRole[0].ctrl = CTRL_GOALKEEP; // 指定守门员
        // 其他队员跑动到指定点
        for (int i = 1; i < 5; i++)
        {
            pRole[i].ctrl = CTRL_MOVETO; // 状态设置
            pRole[i].nDest_angle = 0;    // 角度设置
        }

        uBall tmpball = pDataSummary->cGlobalBall;

        // 传球队员-球-对方球门
        CPoint tmp = m_Loca_Ray(tmpball.x, tmpball.y, 1600, 600,
                                (int)(DIST_TAC_START_PASS * pDataSummary->kDistance)); // 最后一个参数是距离
        tmp = m_Loca_Rotate(tmp.x, tmp.y, tmpball.x, tmpball.y, -105);
        pRole[1].nDest_x = tmp.x;
        pRole[1].nDest_y = tmp.y;
        pRole[1].nDest_angle = -80; // m_Loca_CalAngle(tmp.x,tmp.y,tmpball.x,tmpball.y);
        pDataSummary->posPass.x = tmp.x;
        pDataSummary->posPass.y = tmp.y;
        nPassRoleIndex = 1;

        // 接球进攻队员-球-对方球门
        tmp = m_Loca_Ray(tmpball.x, tmpball.y, 1600, 600,
                         (int)(DIST_TAC_START_CATCH * pDataSummary->kDistance)); // 最后一个参数是距离
        tmp = m_Loca_Rotate(tmp.x, tmp.y, tmpball.x, tmpball.y, 100);
        pRole[2].nDest_x = tmp.x;
        pRole[2].nDest_y = tmp.y;
        pRole[2].nDest_angle = m_Loca_CalAngle(tmp.x, tmp.y, tmpball.x, tmpball.y);
        pDataSummary->posCatch.x = tmp.x;
        pDataSummary->posCatch.y = tmp.y;
        nCatchRoleIndex = 2;

        pRole[3].nDest_x = 300;
        pRole[3].nDest_y = 900;
        pRole[4].nDest_x = 300;
        pRole[4].nDest_y = 300;
    }
    else
    {
        strTacticInfo = "己方开球跑位 - 未检测到球";
        pRole[0].ctrl = CTRL_GOALKEEP;

        // 其他队员跑动到指定点
        for (int i = 1; i < 5; i++)
        {
            pRole[i].ctrl = CTRL_MOVETO;
            pRole[i].nDest_angle = 0;
        }

        // 计算跑动位置
        pRole[1].nDest_x = 600;
        pRole[1].nDest_y = 600;

        pRole[2].nDest_x = 600;
        pRole[2].nDest_y = 900;

        pRole[3].nDest_x = 300;
        pRole[3].nDest_y = 300;

        pRole[4].nDest_x = 300;
        pRole[4].nDest_y = 900;
    }
}

/************************************************************************/
/* 开球开始                                                             */
/************************************************************************/
void CTac_KickOff::Step_Go()
{
    // 开球进行中...
    // 一旦有人进入进攻状态（接到球了！），则激活自主决策
    int i;
    for (i = 0; i < AG_NUM; i++)
    {
        if (pDataSummary->agent[i].online == true && pDataSummary->agent[i].status != ROBST_ERR)
        {
            if (pDataSummary->agent[i].status == CTRL_ATTACK) // 每个机器人都有一个状态参数，.status
            {
                pDataSummary->bAction = true; // 设置开始比赛
                return;
            }
        }
    }
    // 时间超过开球时间（这种情况一般是己方开球不顺利），强行激活自主决策
    if (pDataSummary->nStartTimeCountSec > 9)
    {
        pDataSummary->bAction = true;
    }

    uBall tmpball = pDataSummary->cGlobalBall;
    uRole *pRole = pTactics->role;
    CPoint tmp;
    POINT posTmpCatch;

    // 除了GO_STEP_READY不考虑看不到球的情况，否则会发球失误
    strTacticInfo = "我方开球开始！ - 正常";
    switch (m_nGoStep)
    {
    case GO_STEP_READY:
        // 继续计算传球接球队员目标位置，以免因为（球自然滚动）和（READY阶段机器人离球太远，对球定位不准）而出现定位偏差

        Step_Ready();
        strTacticInfo = "我方开球开始！ - 继续跑位准备中..";

        // 只有一名队员的情况（未实现，可以考虑将球大力射向对方球门）

        // 传球队员和接球队员都就位后才开始执行传球动作
        if (isSomeoneNear(pDataSummary->agent, pDataSummary->posPass.x, pDataSummary->posPass.y,
                          MOVETO_DIST_DEADZONE) &&
            isSomeoneNear(pDataSummary->agent, pDataSummary->posCatch.x, pDataSummary->posCatch.y,
                          MOVETO_DIST_DEADZONE))
        {
            m_nGoStep = GO_STEP_PASS; // 12 传球程序开始
        }
        break;

    case GO_STEP_PASS:
        strTacticInfo = "我方开球开始！ - 进行传球..";
        // 接球位置要往对方半场突出一点，以便接球后可以直接射门
        posTmpCatch = pDataSummary->posCatch;
        // posTmpCatch.x = 950;	//接球突出前场的一点点距离
        posTmpCatch.x = 920;

        // 传球跑位角色变成传球角色，传球的目标点是接球队员的跑位目标点
        pRole[nPassRoleIndex].ctrl = CTRL_PASS;
        pRole[nPassRoleIndex].nDest_x = posTmpCatch.x;
        pRole[nPassRoleIndex].nDest_y = posTmpCatch.y;

        // 接球跑位角色变成接球角色
        pRole[nCatchRoleIndex].ctrl = CTRL_CATCH;
        pRole[nCatchRoleIndex].nDest_x = posTmpCatch.x;
        pRole[nCatchRoleIndex].nDest_y = posTmpCatch.y;

        // 将传球队员的kick信号作为切换条件（bPassKick在从CoachUDP获取数据时才会复制，所以不会漏掉）
        if (pDataSummary->bPassKick == true)
        {
            pDataSummary->posCatch = posTmpCatch;
            m_nGoStep = GO_STEP_CATCH;
        }
        break;

    case GO_STEP_CATCH: // 13 接球状态的意义在于：球传出去之后，让传球队员和其他能做其他动作，不必死等接球动作完成
        strTacticInfo = "我方开球开始！ - 接球状态..";
        if (m_GetAgentIDInStatus(CTRL_ATTACK) > 0 || m_GetAgentIDInStatus(CTRL_PASS_MOVE) > 0)
        {
            // 开始自主决策
            pTacPlaying->PlanTac(pTactics);
            pDataSummary->bAction = true;
            return;
        }
        else
        {
            // 开始中...(接球队员等着接球，其他队员可以开始按照比赛开始模式进行各种行动)
            pRole = pTactics->role;
            for (int i = 0; i < AG_NUM; i++)
            {
                if (pRole[i].ctrl == CTRL_CATCH) // CTRL_CATCH指的应该是接球状态
                {
                    pRole[i].ctrl = CTRL_CATCH;
                    pRole[i].nDest_x = pDataSummary->posCatch.x;
                    pRole[i].nDest_y = pDataSummary->posCatch.y;
                }
            }
        }
        break;
    }
}
