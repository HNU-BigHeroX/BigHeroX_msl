#include "stdafx.h"
#include "Role_Catch.h"

CRole_Catch::CRole_Catch()
{
}

CRole_Catch::~CRole_Catch()
{
}

void CRole_Catch::Reset()
{
}

void CRole_Catch::Play(stDataSummary *inDataSummary, uRole *inRole)
{
    uAgent *pAgent = inDataSummary->agent;
    //[1]挑选最佳执行队员
    int nMinDist = 50000;
    int nBestID = 0;
    int nTmpDist = 0;
    // 找已经存在的Pass队员，维持角色稳定性
    nBestID = m_GetIDInStatus(CTRL_CATCH, inDataSummary->agent, false);
    if (nBestID < 0)
    {
        // 如果没有pass队员，则找距离传球点最近的未分配队员
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
    }

    //[2]执行环节
    if (nBestID > 0 && nBestID <= AG_NUM && pCoachUDP != NULL)
    {
        pCoachUDP->m_ToAgent[nBestID - 1].CatchCmd(inRole->nDest_x, inRole->nDest_y);
        // 标记该队员已经分配行为
        pAgent[nBestID - 1].allocated = true;
        inRole->allocated_ID = nBestID;

        // 记录ID以供显示和查询
        inDataSummary->nCatchID = nBestID;
    }
}