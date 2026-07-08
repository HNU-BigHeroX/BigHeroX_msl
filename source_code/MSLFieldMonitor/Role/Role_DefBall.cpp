#include "stdafx.h"
#include "Role_DefBall.h"

CRole_DefBall::CRole_DefBall()
{
}

CRole_DefBall::~CRole_DefBall()
{
}

void CRole_DefBall::Reset()
{
}

void CRole_DefBall::Play(stDataSummary *inDataSummary, uRole *inRole)
{
    pDataSummary = inDataSummary;
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
        ///////////////////
        // 传球后moveto的修正
        POINT pathPoint =
            FixMovingPathPoint(pAgent[nBestID - 1].x, pAgent[nBestID - 1].y, inRole->nDest_x, inRole->nDest_y);
        if (inRole->nDest_x != pathPoint.x || inRole->nDest_y != pathPoint.y)
        {
            pCoachUDP->AgentCmdMoveTo(nBestID, pathPoint.x, pathPoint.y, 180, 0, true, MOVETO_DEFAULT_SPEED);
        }
        else
            ////////////////////
            pCoachUDP->m_ToAgent[nBestID - 1].DEF_BALL(inRole->nDest_x, inRole->nDest_y);
        // 标记该队员已经分配行为
        pAgent[nBestID - 1].allocated = true;
        inRole->allocated_ID = nBestID;
    }
}
