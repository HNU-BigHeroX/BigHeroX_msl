#include "stdafx.h"
#include "Role_FocusOnBall.h"
#include <math.h>

CRole_FocusOnBall::CRole_FocusOnBall()
{
}

CRole_FocusOnBall::~CRole_FocusOnBall()
{
}

void CRole_FocusOnBall::Reset()
{
}

void CRole_FocusOnBall::Play(stDataSummary *inDataSummary, uRole *inRole)
{
    uAgent *pAgent = inDataSummary->agent;
    pDataSummary = inDataSummary;
    //[1]挑选最佳执行队员
    int nMinDist = 50000;
    int nBestID = 0;
    int nTmpDist = 0;
    // 找已经存在的Focus队员，维持角色稳定性
    nBestID = m_GetIDInStatus(CTRL_FOCUSONBALL, inDataSummary->agent, false);
    if (nBestID < 0)
    {
        // 如果没有Focus队员，则找距离球最近的未分配队员
        for (int i = 0; i < AG_NUM; i++)
        {
            if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR && pAgent[i].allocated == false)
            {
                nTmpDist =
                    m_CalDist(pAgent[i].x, pAgent[i].y, inDataSummary->cGlobalBall.x, inDataSummary->cGlobalBall.y);

                // 将被遮挡考虑进去
                if (isBlock(i) == true)
                {
                    nTmpDist += 200;
                }
                if (nTmpDist < nMinDist)
                {
                    nBestID = pAgent[i].ID;
                    nMinDist = nTmpDist;
                }
            }
        }
    }

    //[2]执行环节
    if (nBestID > 0 && nBestID <= AG_NUM && pCoachUDP != NULL)
    {
        pCoachUDP->m_ToAgent[nBestID - 1].CtrlCmd(CTRL_FOCUSONBALL, inDataSummary->cGlobalBall.x,
                                                  inDataSummary->cGlobalBall.y, 0, 0, true);
        // 标记该队员已经分配行为
        pAgent[nBestID - 1].allocated = true;
        inRole->allocated_ID = nBestID;
    }
}

bool CRole_FocusOnBall::isBlock(int inAgentIndex)
{
    if (inAgentIndex < 0 || inAgentIndex >= AG_NUM)
    {
        return false;
    }

    int nAgentX = pDataSummary->agent[inAgentIndex].x;
    int nAgentY = pDataSummary->agent[inAgentIndex].y;

    int nAngleToBall = m_Loca_CalAngle(nAgentX, nAgentY, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);
    int nDistToBall = m_CalDist(nAgentX, nAgentY, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);

    for (size_t i = 0; i < pDataSummary->NumOfemy; i++)
    {
        if (m_CalDist(nAgentX, nAgentY, pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y) < nDistToBall)
        {
            int nAngleToEnemy =
                m_Loca_CalAngle(nAgentX, nAgentY, pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y);
            if (abs(nAngleToBall - nAngleToEnemy) < 30)
            {
                return true;
            }
        }
    }
    return false;
}
