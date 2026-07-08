#include "stdafx.h"
#include "Role_Stop.h"

CRole_Stop::CRole_Stop()
{
}

CRole_Stop::~CRole_Stop()
{
}

void CRole_Stop::Reset()
{
}

void CRole_Stop::Play(stDataSummary *inDataSummary, uRole *inRole)
{
    uAgent *pAgent = inDataSummary->agent;
    //[1]挑选离得最近未分配队员
    int nMinDist = 50000;
    int nBestID = 0;
    int nTmpDist = 0;
    // 找距离最近的未分配队员
    for (int i = 0; i < AG_NUM; i++)
    {
        if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR && pAgent[i].allocated == false)
        {
            //[2]执行环节
            if (pCoachUDP != NULL)
            {
                pCoachUDP->m_ToAgent[i].CtrlCmd(CTRL_STOP, inRole->re_ballx, inRole->re_bally, inRole->nDest_angle, 0,
                                                true);

                // 标记该队员已经分配行为
                pAgent[i].allocated = true;
                inRole->allocated_ID = nBestID;
            }
            break;
        }
    }
}
