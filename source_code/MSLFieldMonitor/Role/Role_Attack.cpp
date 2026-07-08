#include "stdafx.h"
#include "Role_Attack.h"

CRole_Attack::CRole_Attack()
{
}

CRole_Attack::~CRole_Attack()
{
}

void CRole_Attack::Reset()
{
}

void CRole_Attack::Play(stDataSummary *inDataSummary, uRole *inRole)
{
    uAgent *pAgent = inDataSummary->agent;
    //[1]挑选最佳执行队员
    int nMinDist = 50000;
    int nBestID = 0;
    int nTmpDist = 0;

    // 则找距离球最近的未分配队员(为之前的Attack队员加成)
    for (int i = 0; i < AG_NUM; i++)
    {
        if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR && pAgent[i].allocated == false)
        {
            nTmpDist = m_CalDist(pAgent[i].x, pAgent[i].y, inDataSummary->cGlobalBall.x, inDataSummary->cGlobalBall.y);
            if (nTmpDist < 30) // 离得太近了，直接切进攻
            {
                nBestID = pAgent[i].ID;
                nMinDist = nTmpDist;
                break;
            }
            if (pAgent[i].status == CTRL_ATTACK || pAgent[i].status == CTRL_PASS_MOVE)
            {
                nTmpDist -= 200; // 原进攻队员有额外加成
            }
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
        pCoachUDP->m_ToAgent[nBestID - 1].CtrlCmd(CTRL_ATTACK, inDataSummary->cGlobalBall.x,
                                                  inDataSummary->cGlobalBall.y, 0, 0, true);
        // 标记该队员已经分配行为
        pAgent[nBestID - 1].allocated = true;
        inRole->allocated_ID = nBestID;
    }
}
