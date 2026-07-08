#include "stdafx.h"
#include "Role_MoveTo.h"

CRole_MoveTo::CRole_MoveTo()
{
}

CRole_MoveTo::~CRole_MoveTo()
{
}

void CRole_MoveTo::Reset()
{
}

void CRole_MoveTo::Play(stDataSummary *inDataSummary, uRole *inRole)
{
    pDataSummary = inDataSummary;
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
        ///////////////////
        // 传球后moveto的修正
        POINT pathPoint =
            FixMovingPathPoint(pAgent[nBestID - 1].x, pAgent[nBestID - 1].y, inRole->nDest_x, inRole->nDest_y);
        inRole->nDest_x = pathPoint.x;
        inRole->nDest_y = pathPoint.y;
        ////////////////////
        int nDeffAngle = (int)abs((int)(pAgent[nBestID - 1].angle) - inRole->nDest_angle);
        if (nMinDist < MOVETO_DIST_DEADZONE && nDeffAngle < MOVETO_ANGLE_DEADZONE)
        {
            // 进入移动死区，让其Stop以避免抖动
            pCoachUDP->m_ToAgent[nBestID - 1].CtrlCmd(CTRL_STOP, inRole->re_ballx, inRole->re_bally,
                                                      inRole->nDest_angle, 0, true);
        }
        else
        {
            // 正常的movoto
            // pCoachUDP->m_ToAgent[nBestID - 1].MoveTo(inRole->target_x, inRole->target_y, inRole->target_angle, 0,
            // true, MOVETO_DEFAULT_SPEED);
            pCoachUDP->AgentCmdMoveTo(nBestID, inRole->nDest_x, inRole->nDest_y, inRole->nDest_angle, 0, true,
                                      MOVETO_DEFAULT_SPEED);
        }

        // 标记该队员已经分配行为
        pAgent[nBestID - 1].allocated = true;
        inRole->allocated_ID = nBestID;
    }
}

bool CRole_MoveTo::isCross(int Line1_x1, int Line1_y1, int Line1_x2, int Line1_y2, int Line2_x1, int Line2_y1,
                           int Line2_x2, int Line2_y2)
{
    bool retVal = false;

    return retVal;
}
