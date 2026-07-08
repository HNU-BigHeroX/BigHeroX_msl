#include "stdafx.h"
#include "Tac_Test_MoveARound.h"

static POINT posAllocate[10];
static POINT posTarget[5];

#define DIST_ARRIVED 50

CTac_Test_MoveARound::CTac_Test_MoveARound()
{

    for (int i = 0; i < 5; i++)
    {
        posAllocate[i].x = 300 + i * 300;
        posAllocate[i].y = 300;

        posAllocate[i + 5].x = 1500 - i * 300;
        posAllocate[i + 5].y = 900;
    }

    m_nIndexHead = 0;
    for (i = 0; i < 5; i++)
    {
        posTarget[i] = posAllocate[i];
    }
}

CTac_Test_MoveARound::~CTac_Test_MoveARound()
{
}

void CTac_Test_MoveARound::Reset()
{
    m_nIndexHead = 0;
    HeadTo(m_nIndexHead);
}

// 切换接球点
void CTac_Test_MoveARound::HeadTo(int inIndex)
{
    if (inIndex < 0 || inIndex > 9 || pDataSummary == NULL)
    {
        return;
    }

    for (int i = 0; i < 5; i++)
    {
        int index = inIndex - i;
        if (index < 0)
        {
            index = index + 10;
        }

        posTarget[i] = posAllocate[index];
    }
}

// 判断是否有队员出场地了
bool CTac_Test_MoveARound::bSomeOneOutOfField()
{
    bool res = false;
    for (int i = 0; i < AG_NUM; i++)
    {
        if (pDataSummary->agent[i].online == true && pDataSummary->agent[i].status != ROBST_ERR)
        {
            if (pDataSummary->agent[i].x < 50 || pDataSummary->agent[i].x > 1750 || pDataSummary->agent[i].y < 50 ||
                pDataSummary->agent[i].y > 1150)
            {
                res = true;
            }
        }
    }
    return res;
}

// 判断一个位置上是否有队员
bool CTac_Test_MoveARound::bSomeOneInPosition(int inX, int inY)
{
    bool res = false;
    uAgent *pAgent = pDataSummary->agent;
    for (int i = 0; i < AG_NUM; i++)
    {
        if (m_CalDist(pAgent[i].x, pAgent[i].y, inX, inY) < DIST_ARRIVED)
        {
            res = true;
            break;
        }
    }

    return res;
}

// 是否所有人已经就位
bool CTac_Test_MoveARound::bEveryOneArrived()
{
    bool res = true;
    // 先统计现在又多少队员在跑圈
    int nSum = 0;
    uAgent *pAgent = pDataSummary->agent;
    for (int i = 0; i < AG_NUM; i++)
    {
        if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR)
        {
            nSum++;
        }
    }

    // 对头sum个点进行确认
    for (i = 0; i < nSum; i++)
    {
        if (bSomeOneInPosition(posTarget[i].x, posTarget[i].y) == false)
        {
            res = false;
            break;
        }
    }
    return res;
}

void CTac_Test_MoveARound::PlanTac(stTactics *outTactics)
{
    uRole *pRole = outTactics->role;

    //[最高优先级]若有机器人跑出界，则全部停下来（安全最高优先级）
    if (bSomeOneOutOfField() == true)
    {
        strTacticInfo = "[测试_绕圈跑] 有队员出界，停止！";
        for (int i = 0; i < AG_NUM; i++)
        {
            pRole[i].ctrl = CTRL_STOP;
        }
        return;
    }

    if (bEveryOneArrived() == true)
    {
        m_nIndexHead++;
        if (m_nIndexHead >= 10)
        {
            m_nIndexHead = 0;
        }
        HeadTo(m_nIndexHead);
    }

    strTacticInfo.Format("[测试_绕圈跑] 领队跑向第%d个点", m_nIndexHead);
    for (size_t i = 0; i < AG_NUM; i++)
    {
        pRole[i].ctrl = CTRL_MOVETO;
        pRole[i].nDest_x = posTarget[i].x;
        pRole[i].nDest_y = posTarget[i].y;
        pRole[i].nDest_angle = m_Loca_CalAngle(pRole[i].nDest_x, pRole[i].nDest_y, 900, 600); // 面朝场地中央
    }
}

void CTac_Test_MoveARound::Start()
{
}
