#include "stdafx.h"
#include "Role_Defence.h"

CRole_Defence::CRole_Defence()
{
}

CRole_Defence::~CRole_Defence()
{
}

void CRole_Defence::Reset()
{
}

void CRole_Defence::Play(stDataSummary *inDataSummary, uRole *inRole)
{
    pDataSummary = inDataSummary;
    uAgent *pAgent = inDataSummary->agent;
    // 找到未分配的队员，强制赋值其分配
    //[1]挑选最佳执行队员
    int nMinDist = 50000;
    int nBestID = 0;
    int nTmpDist = 0;

    // 计算防守点坐标
    CPoint tmp;
    if (pDataSummary->bNoOneSeeBall == false)
    {
        tmp = m_Loca_Ray(pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, 0, 600, inRole->def_dist);
        if (inRole->def_angle != 0)
        {
            tmp = m_Loca_Rotate(tmp.x, tmp.y, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,
                                inRole->def_angle);
        }
    }
    else
    {
        tmp.x = pDataSummary->nObstHoldBallInNoSeen_x;
        tmp.y = pDataSummary->nObstHoldBallInNoSeen_y;
    }
    // nBestID = m_Ag_GetNearBy(tmp.x, tmp.y, pDataSummary->agent);

    // 找距离球最近的未分配队员
    for (int i = 0; i < AG_NUM; i++)
    {
        if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR && pAgent[i].allocated == false)
        {
            nTmpDist = m_CalDist(pAgent[i].x, pAgent[i].y, tmp.x, tmp.y);
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
        pCoachUDP->m_ToAgent[nBestID - 1].DefCmd(inRole->def_angle, inRole->def_dist);
        // 标记该队员已经分配行为
        pAgent[nBestID - 1].allocated = true;
        inRole->allocated_ID = nBestID;
    }
}
