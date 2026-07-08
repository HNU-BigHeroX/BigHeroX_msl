#include "stdafx.h"
#include "Tac_Test_4Pass.h"

#define TESTTAC_4PASS_GETBALL 0
#define TESTTAC_4PASS_TOPASS 1
#define TESTTAC_4PASS_PASSING 2

static POINT posCatch[4];
static POINT posWaitting[3];

CTac_Test_4Pass::CTac_Test_4Pass()
{
    posCatch[0].x = 300; // 左下
    posCatch[0].y = 300;

    posCatch[1].x = 1500; // 右下
    posCatch[1].y = 300;

    posCatch[2].x = 1500; // 右上
    posCatch[2].y = 900;

    posCatch[3].x = 300; // 左上
    posCatch[3].y = 900;

    m_nIndexCatch = 0;
}

CTac_Test_4Pass::~CTac_Test_4Pass()
{
}

void CTac_Test_4Pass::Reset()
{
    m_nIndexCatch = 0;
    PassTo(m_nIndexCatch);
    m_nStep = TESTTAC_4PASS_GETBALL;
}

// 切换接球点
void CTac_Test_4Pass::PassTo(int inCatchIndex)
{
    if (inCatchIndex < 0 || inCatchIndex > 3 || pDataSummary == NULL)
    {
        return;
    }

    switch (inCatchIndex)
    {
    case 0:
        pDataSummary->posCatch.x = posCatch[0].x;
        pDataSummary->posCatch.y = posCatch[0].y;

        posWaitting[0] = posCatch[1];
        posWaitting[1] = posCatch[2];
        posWaitting[2] = posCatch[3];
        break;

    case 1:
        pDataSummary->posCatch.x = posCatch[1].x;
        pDataSummary->posCatch.y = posCatch[1].y;

        posWaitting[0] = posCatch[2];
        posWaitting[1] = posCatch[3];
        posWaitting[2] = posCatch[0];
        break;

    case 2:
        pDataSummary->posCatch.x = posCatch[2].x;
        pDataSummary->posCatch.y = posCatch[2].y;

        posWaitting[0] = posCatch[3];
        posWaitting[1] = posCatch[0];
        posWaitting[2] = posCatch[1];
        break;

    case 3:
        pDataSummary->posCatch.x = posCatch[3].x;
        pDataSummary->posCatch.y = posCatch[3].y;

        posWaitting[0] = posCatch[0];
        posWaitting[1] = posCatch[1];
        posWaitting[2] = posCatch[2];
        break;
    default:
        break;
    }
}

void CTac_Test_4Pass::PlanTac(stTactics *outTactics)
{
    uRole *pRole = outTactics->role;

    //[最高优先级]若球出界，则全部停下来（最高优先级）
    if (pDataSummary->cGlobalBall.x < 0 || pDataSummary->cGlobalBall.x > 1800 || pDataSummary->cGlobalBall.y < 0 ||
        pDataSummary->cGlobalBall.y > 1200)
    {
        strTacticInfo = "[测试_传接球] 球出界，停止！";
        for (int i = 0; i < AG_NUM; i++)
        {
            pRole[i].ctrl = CTRL_STOP;
        }
        m_nStep = TESTTAC_4PASS_GETBALL;
        pDataSummary->nCatchID = 0;
        pDataSummary->nPassID = 0;
        return;
    }

    pRole[0].ctrl = CTRL_GOALKEEP;

    switch (m_nStep)
    {
    case TESTTAC_4PASS_GETBALL:
        strTacticInfo.Format("[测试_4传接] 第%d个点_传球队员找球", m_nIndexCatch);

        pRole[1].ctrl = CTRL_PASS_MOVE;
        pRole[1].nDest_x = pDataSummary->posCatch.x;
        pRole[1].nDest_x = pDataSummary->posCatch.y;

        pRole[2].ctrl = CTRL_CATCH_MOVE;
        pRole[2].nDest_x = pDataSummary->posCatch.x;
        pRole[2].nDest_y = pDataSummary->posCatch.y;

        pRole[3].ctrl = CTRL_MOVETO;
        pRole[3].nDest_x = posWaitting[0].x;
        pRole[3].nDest_y = posWaitting[0].y;
        pRole[3].nDest_angle = m_Loca_CalAngle(pRole[3].nDest_x, pRole[3].nDest_y, pDataSummary->cGlobalBall.x,
                                               pDataSummary->cGlobalBall.y);

        pRole[4].ctrl = CTRL_MOVETO;
        pRole[4].nDest_x = posWaitting[1].x;
        pRole[4].nDest_y = posWaitting[1].y;
        pRole[4].nDest_angle = m_Loca_CalAngle(pRole[4].nDest_x, pRole[4].nDest_y, pDataSummary->cGlobalBall.x,
                                               pDataSummary->cGlobalBall.y);

        // 计算状态切换条件
        if (pDataSummary->nPassID > 0 && pDataSummary->nPassID <= AG_NUM)
        {
            // 判断传球队员是否持到球
            if (pDataSummary->agent[pDataSummary->nPassID - 1].x ==
                    pDataSummary->agent[pDataSummary->nPassID - 1].ball.x &&
                pDataSummary->agent[pDataSummary->nPassID - 1].y ==
                    pDataSummary->agent[pDataSummary->nPassID - 1].ball.y)
            {
                // 传球队员已经持球，开始传球
                m_nStep = TESTTAC_4PASS_TOPASS;
            }

            // 在这个阶段传球队员紧急起脚（或者是决策周期太长，错过了起脚信号），紧急切换
            if (pDataSummary->bPassKick == true)
            {
                // 球已经传出，切换状态
                m_nStep = TESTTAC_4PASS_PASSING;
                pDataSummary->bPassing = true;
            }
        }
        break;

    case TESTTAC_4PASS_TOPASS:
        strTacticInfo.Format("[测试_4传接] 第%d个点_传球队员准备传球", m_nIndexCatch);

        pRole[1].ctrl = CTRL_PASS_MOVE;
        pRole[1].nDest_x = pDataSummary->posCatch.x;
        pRole[1].nDest_y = pDataSummary->posCatch.y;

        pRole[2].ctrl = CTRL_CATCH_MOVE;
        pRole[2].nDest_x = pDataSummary->posCatch.x;
        pRole[2].nDest_y = pDataSummary->posCatch.y;

        pRole[3].ctrl = CTRL_MOVETO;
        pRole[3].nDest_x = posWaitting[0].x;
        pRole[3].nDest_y = posWaitting[0].y;
        pRole[3].nDest_angle = m_Loca_CalAngle(pRole[3].nDest_x, pRole[3].nDest_y, pDataSummary->cGlobalBall.x,
                                               pDataSummary->cGlobalBall.y);

        pRole[4].ctrl = CTRL_MOVETO;
        pRole[4].nDest_x = posWaitting[1].x;
        pRole[4].nDest_y = posWaitting[1].y;
        pRole[4].nDest_angle = m_Loca_CalAngle(pRole[4].nDest_x, pRole[4].nDest_y, pDataSummary->cGlobalBall.x,
                                               pDataSummary->cGlobalBall.y);

        // 计算状态切换条件
        if (pDataSummary->nPassID > 0 && pDataSummary->nPassID <= AG_NUM)
        {
            // 判断传球队员是否起脚
            if (pDataSummary->bPassKick == true)
            {
                // 球已经传出，切换状态
                m_nStep = TESTTAC_4PASS_PASSING;
                pDataSummary->bPassing = true;
            }
        }
        break;

    case TESTTAC_4PASS_PASSING:
        strTacticInfo.Format("[测试_4传接] 第%d个点_球已传出", m_nIndexCatch);

        // 这里对pDataSummary->posCatch的赋值是从传球队友发回来的数据
        int indexPass = GetAgentIndex(CTRL_PASS_MOVE);
        if (indexPass > 0 && indexPass < AG_NUM)
        {
            pDataSummary->posCatch.x = pDataSummary->agent[indexPass].dest_x;
            pDataSummary->posCatch.y = pDataSummary->agent[indexPass].dest_y;
        }

        // 传球队员回到下一轮接球点位置

        pRole[1].ctrl = CTRL_CATCH_FOCUS;
        pRole[1].nDest_x = pDataSummary->posCatch.x;
        pRole[1].nDest_y = pDataSummary->posCatch.y;

        pRole[2].ctrl = CTRL_MOVETO;
        pRole[2].nDest_x = posWaitting[0].x;
        pRole[2].nDest_y = posWaitting[0].y;
        pRole[2].nDest_angle = m_Loca_CalAngle(pRole[3].nDest_x, pRole[3].nDest_y, pDataSummary->cGlobalBall.x,
                                               pDataSummary->cGlobalBall.y);

        pRole[3].ctrl = CTRL_MOVETO;
        pRole[3].nDest_x = posWaitting[1].x;
        pRole[3].nDest_y = posWaitting[1].y;
        pRole[3].nDest_angle = m_Loca_CalAngle(pRole[4].nDest_x, pRole[4].nDest_y, pDataSummary->cGlobalBall.x,
                                               pDataSummary->cGlobalBall.y);

        pRole[4].ctrl = CTRL_MOVETO;
        pRole[4].nDest_x = posWaitting[2].x;
        pRole[4].nDest_y = posWaitting[2].y;
        pRole[4].nDest_angle = m_Loca_CalAngle(pRole[4].nDest_x, pRole[4].nDest_y, pDataSummary->cGlobalBall.x,
                                               pDataSummary->cGlobalBall.y);

        // 计算状态切换条件
        if (pDataSummary->nCatchID > 0 && pDataSummary->nCatchID <= AG_NUM)
        {
            UINT nCurTimeSec = GetTickCount() / 1000;
            pDataSummary->nPassKickTimeCountSec = nCurTimeSec - pDataSummary->nPassKickTime;
            // 判断接球队员是否接到球 或者 传球之后时间是否超时
            if ((pDataSummary->agent[pDataSummary->nCatchID - 1].x ==
                     pDataSummary->agent[pDataSummary->nCatchID - 1].ball.x &&
                 pDataSummary->agent[pDataSummary->nCatchID - 1].y ==
                     pDataSummary->agent[pDataSummary->nCatchID - 1].ball.y) ||
                (pDataSummary->nPassKickTimeCountSec > 5))
            {
                // 接球队员已经接到球，开始下一轮传接球
                m_nIndexCatch++;
                if (m_nIndexCatch > 3)
                {
                    m_nIndexCatch = 0;
                }
                PassTo(m_nIndexCatch);
                m_nStep = TESTTAC_4PASS_GETBALL;
                pDataSummary->nCatchID = 0;
                pDataSummary->nPassID = 0;
            }
        }
        // 传球状态终止条件
        if (pDataSummary->bPassing == true)
        {
            // 传球状态终止条件一：传球超时
            UINT nCurTimeSec = GetTickCount() / 1000;
            pDataSummary->nPassKickTimeCountSec = nCurTimeSec - pDataSummary->nPassKickTime;
            if (pDataSummary->nPassKickTimeCountSec > TIMEOUT_PASSING)
            {
                pDataSummary->bPassing = false;
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
                }
            }
            // 传球状态终止条件三：接球队员接球后切换成进攻
            if (pDataSummary->nCatchID > 0 && pDataSummary->nCatchID <= 5)
            {
                if (pDataSummary->agent[pDataSummary->nCatchID - 1].status == CTRL_ATTACK ||
                    pDataSummary->agent[pDataSummary->nCatchID - 1].status == CTRL_PASS_MOVE)
                {
                    pDataSummary->bPassing = false;
                }
            }
        }

        break;
    }
}

void CTac_Test_4Pass::Start()
{
}