#include "stdafx.h"
#include "Role_SearchBall.h"

CRole_SearchBall::CRole_SearchBall()
{
}

CRole_SearchBall::~CRole_SearchBall()
{
}

void CRole_SearchBall::Reset()
{
}

void CRole_SearchBall::Play(stDataSummary *inDataSummary, uRole *inRole)
{
    uAgent *pAgent = inDataSummary->agent;
    //[1]挑选最佳执行队员
    int nMinDist = 50000;
    int nBestID = 0;
    int nTmpDist = 0;
    // 找距离最近的未分配队员
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

        pCoachUDP->m_ToAgent[nBestID - 1].CtrlCmd(CTRL_SEARCHBALL, inRole->re_ballx, inRole->re_bally,
                                                  inRole->nDest_angle, 0, true);

        // 标记该队员已经分配行为
        pAgent[nBestID - 1].allocated = true;
        inRole->allocated_ID = nBestID;
    }
}
