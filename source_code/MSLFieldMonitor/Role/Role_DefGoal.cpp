#include "stdafx.h"
#include "Role_DefGoal.h"

CRole_DefGoal::CRole_DefGoal()
{
}

CRole_DefGoal::~CRole_DefGoal()
{
}

void CRole_DefGoal::Reset()
{
}

void CRole_DefGoal::Play(stDataSummary *inDataSummary, uRole *inRole)
{
    uAgent *pAgent = inDataSummary->agent;
    // 找到未分配的队员，强制赋值其分配
    //[1]挑选最佳执行队员
    int nMinDist = 50000;
    int nBestID = 0;
    int nTmpDist = 0;

    // 找距离防守点最近的未分配队员
    for (int i = 0; i < AG_NUM; i++)
    {
        if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR && pAgent[i].allocated == false)
        {
            nTmpDist = m_CalDist(pAgent[i].x, pAgent[i].y, inRole->nDest_x, inRole->nDest_y);
            if (nTmpDist < nMinDist)
            {
                nBestID = pAgent[i].ID;
                nMinDist = nTmpDist;
            }
        }
    }

    //[2]执行环节
    if (nBestID > 0 && nBestID <= AG_NUM && pCoachUDP != NULL)
    {
        pCoachUDP->m_ToAgent[nBestID - 1].DEF_GOAL(inRole->nDest_x, inRole->nDest_y);
        // 标记该队员已经分配行为
        pAgent[nBestID - 1].allocated = true;
        inRole->allocated_ID = nBestID;
    }
}
