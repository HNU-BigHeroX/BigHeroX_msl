#include "stdafx.h"
#include "Role_PassMove.h"

CRole_PassMove::CRole_PassMove()
{
}

CRole_PassMove::~CRole_PassMove()
{
}

void CRole_PassMove::Reset()
{
}

void CRole_PassMove::Play(stDataSummary *inDataSummary, uRole *inRole)
{
    uAgent *pAgent = inDataSummary->agent;
    //[1]挑选最佳执行队员
    int nMinDist = 50000;
    int nBestID = -1;
    int nTmpDist = 0;

    // 若在传球状态过程中，则保持传球队员的稳定性，以便传球队员对球的去向进行观测
    if (inDataSummary->bPassing == true)
    {
        nBestID = m_GetIDInStatus(CTRL_PASS_MOVE, inDataSummary->agent, false);
    }
    if (nBestID < 0)
    {
        // 找距离球最近的未分配队员
        for (int i = 0; i < AG_NUM; i++)
        {
            if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR && pAgent[i].allocated == false)
            {
                nTmpDist =
                    m_CalDist(pAgent[i].x, pAgent[i].y, inDataSummary->cGlobalBall.x, inDataSummary->cGlobalBall.y);
                if (nTmpDist < 30) // 离得太近了，直接切进攻
                {
                    nBestID = pAgent[i].ID;
                    nMinDist = nTmpDist;
                    break;
                }
                if (pAgent[i].status == CTRL_ATTACK || pAgent[i].status == CTRL_PASS_MOVE)
                {
                    nTmpDist -= 100; // 原进攻队员有额外加成
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
        pAgent[nBestID - 1].passto_x = inDataSummary->posCatch.x;
        pAgent[nBestID - 1].passto_y = inDataSummary->posCatch.y;
        pCoachUDP->m_ToAgent[nBestID - 1].CtrlCmd(CTRL_PASS_MOVE, inDataSummary->cGlobalBall.x,
                                                  inDataSummary->cGlobalBall.y, inRole->nDest_x, inRole->nDest_y, true);
        // 标记该队员已经分配行为
        pAgent[nBestID - 1].allocated = true;
        inRole->allocated_ID = nBestID;

        // 记录ID以供显示和查询
        inDataSummary->nPassID = nBestID;
    }
}
