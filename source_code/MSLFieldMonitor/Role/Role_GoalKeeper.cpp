#include "stdafx.h"
#include "Role_GoalKeeper.h"

CRole_GoalKeeper::CRole_GoalKeeper()
{
}

CRole_GoalKeeper::~CRole_GoalKeeper()
{
}

void CRole_GoalKeeper::Reset()
{
}

void CRole_GoalKeeper::Play(stDataSummary *inDataSummary, uRole *inRole)
{
    uAgent *pAgent = inDataSummary->agent;
    //[1]挑选最佳执行队员
    int nBestID = 0;
    int nTmpDist = 0;
    // 找已经存在的Pass队员，维持角色稳定性
    nBestID = m_GetIDInStatus(CTRL_GOALKEEP, inDataSummary->agent, false);
    if (nBestID < 0)
    {
        // 如果没有pass队员，则找距离传球点最近的未分配队员
        for (int i = 0; i < AG_NUM; i++)
        {
            if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR && pAgent[i].allocated == false)
            {
                if (pAgent[i].ID == 1)
                {
                    nBestID = pAgent[i].ID;
                    break;
                }
            }
        }
    }

    //[2]执行环节
    if (nBestID > 0 && nBestID <= AG_NUM && pCoachUDP != NULL)
    {
        pCoachUDP->m_ToAgent[nBestID - 1].CtrlCmd(CTRL_GOALKEEP, inDataSummary->cGlobalBall.x,
                                                  inDataSummary->cGlobalBall.y, 0, 0, true);
        // 标记该队员已经分配行为
        pAgent[nBestID - 1].allocated = true;
        inRole->allocated_ID = nBestID;
    }
}