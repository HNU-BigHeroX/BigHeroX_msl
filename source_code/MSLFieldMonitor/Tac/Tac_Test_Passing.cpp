#include "stdafx.h"
#include "Tac_Test_Passing.h"

#define TESTTAC_PASSING_GETBALL 0
#define TESTTAC_PASSING_TOPASS 1
#define TESTTAC_PASSING_PASSING 2

CTac_Test_Passing::CTac_Test_Passing()
{
    Reset();
}

CTac_Test_Passing::~CTac_Test_Passing()
{
}

void CTac_Test_Passing::Reset()
{
    m_nStep = TESTTAC_PASSING_GETBALL;
}

void CTac_Test_Passing::PlanTac(stTactics *outTactics)
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
        m_nStep = TESTTAC_PASSING_GETBALL;
        pDataSummary->nCatchID = 0;
        pDataSummary->nPassID = 0;
        return;
    }

    pRole[0].ctrl = CTRL_GOALKEEP;
    pRole[3].ctrl = CTRL_STOP;
    pRole[4].ctrl = CTRL_STOP;
    pDataSummary->posCatch.y = 600;
    switch (m_nStep)
    {
    case TESTTAC_PASSING_GETBALL:
        // 根据球所在半场来确定接球位置
        if (pDataSummary->cGlobalBall.x < 900)
        {
            // 球在己方半场,则接球点在对方半场
            pDataSummary->posCatch.x = 1500;
            strTacticInfo = "[测试_传接球] 传球队员找球（对方半场）";
        }
        else
        {
            // 球在对方半场,则接球点在记方半场
            pDataSummary->posCatch.x = 300;
            strTacticInfo = "[测试_传接球] 传球队员找球（己方半场）";
        }

        pRole[1].ctrl = CTRL_PASS_MOVE;
        pRole[1].nDest_x = pDataSummary->posCatch.x;
        pRole[1].nDest_y = pDataSummary->posCatch.y;

        pRole[2].ctrl = CTRL_CATCH_MOVE;
        pRole[2].nDest_x = pDataSummary->posCatch.x;
        pRole[2].nDest_y = pDataSummary->posCatch.y;

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
                m_nStep = TESTTAC_PASSING_TOPASS;
            }

            // 在这个阶段传球队员紧急起脚（或者是决策周期太长，错过了起脚信号），紧急切换
            if (pDataSummary->bPassKick == true)
            {
                pDataSummary->bPassing = true;
                // 球已经传出，切换状态
                m_nStep = TESTTAC_PASSING_PASSING;
            }
        }
        break;

    case TESTTAC_PASSING_TOPASS:
        strTacticInfo = "[测试_传接球] 传球队员准备传球";

        pRole[1].ctrl = CTRL_PASS_MOVE;
        pRole[1].nDest_x = pDataSummary->posCatch.x;
        pRole[1].nDest_y = pDataSummary->posCatch.y;

        pRole[2].ctrl = CTRL_CATCH_MOVE;
        pRole[2].nDest_x = pDataSummary->posCatch.x;
        pRole[2].nDest_y = pDataSummary->posCatch.y;

        // 计算状态切换条件
        if (pDataSummary->nPassID > 0 && pDataSummary->nPassID <= AG_NUM)
        {
            // 判断传球队员是否起脚
            if (pDataSummary->bPassKick == true)
            {
                pDataSummary->bPassing = true;
                // 球已经传出，切换状态
                m_nStep = TESTTAC_PASSING_PASSING;
            }
        }
        break;

    case TESTTAC_PASSING_PASSING:
        strTacticInfo = "[测试_传接球] 球已经传出！";
        // 这里对pDataSummary->posCatch的赋值是从传球队友发回来的数据
        int indexPass = GetAgentIndex(CTRL_PASS_MOVE);
        if (indexPass > 0 && indexPass < AG_NUM)
        {
            pDataSummary->posCatch.x = pDataSummary->agent[indexPass].dest_x;
            pDataSummary->posCatch.y = pDataSummary->agent[indexPass].dest_y;
        }

        pRole[1].ctrl = CTRL_CATCH_FOCUS;
        pRole[1].nDest_x = pDataSummary->posCatch.x;
        pRole[1].nDest_y = pDataSummary->posCatch.y;

        // 传球队员回到下一轮接球点位置
        pRole[2].ctrl = CTRL_STOP;
        if (pDataSummary->posCatch.x > 900)
        {
            pRole[2].nDest_x = 300;
        }
        else
        {
            pRole[2].nDest_x = 1500;
        }
        pRole[2].nDest_y = 600;

        /*pRole[2].ctrl = CTRL_PASS_MOVE;
        pRole[2].nDest_x = pDataSummary->posCatch.x;
        pRole[2].nDest_y = pDataSummary->posCatch.y;*/
        // 使机器人朝向球
        pRole[2].nDest_angle = m_Loca_CalAngle(pRole[2].nDest_x, pRole[2].nDest_y, pDataSummary->cGlobalBall.x,
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
                (pDataSummary->nPassKickTimeCountSec > 2))
            {
                // 接球队员已经接到球，开始下一轮传接球
                m_nStep = TESTTAC_PASSING_GETBALL;
                pDataSummary->nCatchID = 0;
                pDataSummary->nPassID = 0;

                pDataSummary->bPassing = false;
            }
        }

        break;
    }
}
