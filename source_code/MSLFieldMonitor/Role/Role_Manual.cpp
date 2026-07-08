#include "stdafx.h"
#include "Role_Manual.h"

CRole_Manual::CRole_Manual()
{
}

CRole_Manual::~CRole_Manual()
{
}

void CRole_Manual::Reset()
{
}

void CRole_Manual::Play(stDataSummary *inDataSummary, uRole *inRole)
{
    uAgent *pAgent = inDataSummary->agent;
    // 找到未分配的队员，强制赋值其分配
    for (int i = 0; i < AG_NUM; i++)
    {
        if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR && pAgent[i].allocated == false)
        {
            pAgent[i].allocated = true;

            inRole->allocated_ID = pAgent[i].ID;
            break;
        }
    }
}
