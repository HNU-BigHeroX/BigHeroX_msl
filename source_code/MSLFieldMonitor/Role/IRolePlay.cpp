#include "stdafx.h"
#include "IRolePlay.h"
#include <MATH.H>

IRolePlay::IRolePlay()
{
    pCoachUDP = NULL;
    pDataSummary = NULL;
}

IRolePlay::~IRolePlay()
{
}

int IRolePlay::m_CalDist(int inx1, int iny1, int inx2, int iny2)
{
    double ret = (inx1 - inx2) * (inx1 - inx2) + (iny1 - iny2) * (iny1 - iny2);
    ret = (int)sqrt(ret);
    return (int)ret;
}

CPoint IRolePlay::m_Loca_Ray(int inXbase, int inYbase, int inXtarg, int inYtarg, int inDist)
{
    CPoint newpoint;
    int srcDist = (int)m_CalDist(inXbase, inYbase, inXtarg, inYtarg);
    if (srcDist == 0)
    {
        newpoint.x = inXbase;
        newpoint.y = inYbase;
        return newpoint;
    }
    int dx = (int)((double)(inXtarg - inXbase) * inDist / srcDist);
    int dy = (int)((double)(inYtarg - inYbase) * inDist / srcDist);
    newpoint.x = inXbase + dx;
    newpoint.y = inYbase + dy;

    return newpoint;
}

CPoint IRolePlay::m_Loca_Rotate(int inx, int iny, int inCx, int inCy, int inAng)
{
    CPoint newpoint;
    if (inAng % 360 == 0)
    {
        newpoint.x = inx;
        newpoint.y = iny;
        return newpoint;
    }

    // 将角度换算成数值
    double vAng = (double)inAng * 3.14159 / 180;

    newpoint.x = (int)((inx - inCx) * cos(vAng) + (iny - inCy) * sin(vAng) + inCx);
    newpoint.y = (int)(-(inx - inCx) * sin(vAng) + (iny - inCy) * cos(vAng) + inCy);

    return newpoint;
}

int IRolePlay::m_Loca_CalAngle(int inXbase, int inYbase, int inXtarg, int inYtarg)
{
    double retAngle;
    if (inXbase != inXtarg)
    {
        // 分母不为零
        int dx = inXtarg - inXbase;
        double dy = inYtarg - inYbase;
        retAngle = atan(dy / dx);

        // 换算成角度
        retAngle = retAngle * 180 / 3.14158;

        // 象限的修正
        if (retAngle < 0)
        {
            if (dy > 0)
            {
                retAngle += 180;
            }
        }
        else
        {
            if (dy < 0)
            {
                retAngle += 180;
            }
        }

        // 修正
        while (retAngle < 0)
        {
            retAngle += 360;
        }
        while (retAngle > 360)
        {
            retAngle -= 360;
        }

        // 修正
        if (dy == 0)
        {
            if (dx > 0)
            {
                retAngle = 0;
            }
            else
            {
                retAngle = 180;
            }
        }
    }
    else
    {
        // inXbase == inXtarg
        if (inYtarg > inYbase)
        {
            retAngle = 90;
        }
        else
        {
            retAngle = 270;
        }
    }

    return (int)retAngle;
}

bool IRolePlay::isSomeoneNear(uAgent *inAgent, int inX, int inY, int inNearDist)
{
    bool res = false;
    for (int i = 0; i < AG_NUM; i++)
    {
        if (inAgent[i].online == true && inAgent[i].status != ROBST_ERR)
        {
            if (m_CalDist(inAgent[i].x, inAgent[i].y, inX, inY) <= inNearDist)
            {
                res = true;
                break;
            }
        }
    }
    return res;
}

int IRolePlay::m_GetIDInStatus(UINT inState, uAgent *inAgent, bool inAllocated)
{
    int ret = -1;
    for (int i = 0; i < AG_NUM; i++)
    {
        if (inAgent[i].online == true && inAgent[i].allocated == inAllocated)
        {
            if (inAgent[i].status == inState)
            {
                ret = inAgent[i].ID;
                break;
            }
        }
    }
    return ret;
}

POINT IRolePlay::FixMovingPathPoint(int inCurX, int inCurY, int inDestX, int inDestY)
{
    POINT retPnt;
    retPnt.x = inDestX;
    retPnt.y = inDestY;

    // 查看是否有进攻队员
    int nAtkIndex = -1;
    for (int i = 0; i < AG_NUM; i++)
    {
        if (pDataSummary->agent[i].status == CTRL_ATTACK)
        {
            nAtkIndex = i;
            break;
        }
    }

    // 查看进攻队员是否在控球
    bool bAtkCtrlBall = false;
    int nAtkX = 0;
    int nAtkY = 0;
    if (nAtkIndex >= 0 && nAtkIndex < AG_NUM)
    {
        nAtkX = pDataSummary->agent[nAtkIndex].x;
        nAtkY = pDataSummary->agent[nAtkIndex].y;
        int DistToBall = m_CalDist(nAtkX, nAtkY, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);
        if (DistToBall < 50)
        {
            bAtkCtrlBall = true;
        }
    }

    // 查看现在的moveto路线是否有可能挡住进攻队员的进攻路线
    if (bAtkCtrlBall == true)
    {
        // 当前队员站位比进攻队员还靠前，回程目标比进攻队员要靠后，想办法从进攻队员身后绕过去
        if (inCurX > nAtkX + 50 && inDestX < nAtkX)
        {
            // 从进攻队员身后（靠外侧）绕过去
            if (nAtkY > 800)
            {
                // 进攻队员在上半侧，从上方绕过去
                retPnt.x = nAtkX - 10;
                retPnt.y = nAtkY + 100;
            }

            if (nAtkY < 400)
            {
                // 进攻队员在下半侧，从下方绕过去
                retPnt.x = nAtkX - 10;
                retPnt.y = nAtkY - 100;
            }
        }
    }

    return retPnt;
}

// 查找距离x，y最近的ag
int IRolePlay::m_Ag_GetNearBy(int x, int y, uAgent *inalist)
{
    int ret = 0;
    int mindist = 0x7fffffff;
    int tmp_dist = 0;

    bool scanerr = true; // 标记是否还有必要对err的ag进行扫描
    // 先扫描健康的ag
    for (int i = 1; i < 5; i++)
    {
        if (inalist[i].online == true && inalist[i].allocated == false && inalist[i].status != ROBST_ERR)
        {
            // 健康的ag里有人，不需要对err的ag进行扫描了
            scanerr = false;

            // 判断该健康的ag距离
            tmp_dist = m_CalDist(inalist[i].x, inalist[i].y, x, y);
            if (tmp_dist < mindist)
            {
                ret = inalist[i].ID;
                mindist = tmp_dist;
            }
        }
    }

    // 如果健康ag已经无人选，则从剩余的ag里挑选
    if (scanerr == true)
    {
        for (int i = 1; i < 7; i++)
        {
            if (inalist[i].online == true && inalist[i].allocated == false)
            {
                // 判断该err的ag距离
                tmp_dist = m_CalDist(inalist[i].x, inalist[i].y, x, y);
                if (tmp_dist < mindist)
                {
                    ret = inalist[i].ID;
                    mindist = tmp_dist;
                }
            }
        }
    }

    return ret;
}
