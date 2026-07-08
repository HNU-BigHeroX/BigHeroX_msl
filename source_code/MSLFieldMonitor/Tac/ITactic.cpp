/*
ITactic.cpp包含了策略中需要用到的计算等方法的封装
tactic是策略的意思
*/
#include "stdafx.h"
#include "ITactic.h"

ITactic::ITactic() // 创建新实例时执行该函数，初始化一些数值
{
    pDataSummary = NULL;
    pTactics = NULL;
    pInfoList = NULL;

    nNumToOurPenaltyArea = 1;
    nNumToOppoPenaltyArea = 1;

    m_nTacStep = TAC_STEP_READY; // TAC_STEP_READY=1 开球准备
}

ITactic::~ITactic()
{
}

void ITactic::Start()
{
    m_nTacStep = TAC_STEP_GO;  // TAC_STEP_GO=2  开始指令
    m_nGoStep = GO_STEP_READY; // GO_STEP_READY=11 仍未跑位到位，继续跑位
}

/*
计算两点之间的距离
*/
double ITactic::m_CalDist(int inx1, int iny1, int inx2, int iny2)
{
    double ret = (inx1 - inx2) * (inx1 - inx2) + (iny1 - iny2) * (iny1 - iny2);
    ret = (int)sqrt(ret);
    return ret;
}

/*
猜测为计算两点之间某个点的坐标的函数
*/
CPoint ITactic::m_Loca_Ray(int inXbase, int inYbase, int inXtarg, int inYtarg, int inDist)
{
    CPoint newpoint;
    int srcDist = (int)m_CalDist(inXbase, inYbase, inXtarg, inYtarg); // 算距离
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

/*
猜测和上面的函数作用类似，但是这个算的是角度有一定偏差的点
*/
CPoint ITactic::m_Loca_Rotate(int inx, int iny, int inCx, int inCy, int inAng)
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

/*
传入两个坐标，计算一个坐标相对于另一个坐标的朝向
这里这个函数前面传机器人坐标，后面传球的坐标，可得出机器人应朝向哪边才能看向球
*/
int ITactic::m_Loca_CalAngle(int inXbase, int inYbase, int inXtarg, int inYtarg)
{
    double retAngle;
    if (inXbase != inXtarg)
    {
        // 分母不为零
        int dx = inXtarg - inXbase;
        double dy = inYtarg - inYbase;
        retAngle = atan(dy / dx);

        // 换算成角度
        retAngle = retAngle * 180 / 3.14159;

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

/*
判断离球最近的机器人有没有小于inNearDist这个值，如果有返回true，没有返回false
*/
bool ITactic::isSomeoneNear(uAgent *inAgent, int inX, int inY, int inNearDist)
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

/*

*/
int ITactic::GetAgentIndex(int inStatus)
{
    int retIndex = -1;
    for (int i = 0; i < AG_NUM; i++)
    {
        if (pDataSummary->agent[i].online == true)
        {
            if (pDataSummary->agent[i].status == inStatus)
            {
                retIndex = i;
                break;
            }
        }
    }

    return retIndex;
}

/*

*/
CPoint ITactic::m_Fix_KeepInOppoField(int inX, int inY, int inAwayX, int inAwayY, int inAwayDist, int inOppoDist)
{
    CPoint newpoint;
    newpoint.x = inX;
    newpoint.y = inY;

    if (newpoint.x > 1000 && newpoint.x < 1600 && newpoint.y > 100 && newpoint.y < 1100)
    {
        return newpoint;
    }

    CPoint tmpPointClockWise;
    CPoint tmpPointAntiClock;

    // 该点在上半场，应该逆时针，（角度递减）
    // if (inAwayY > 600) //在左右半场分界线上下移动会震荡
    if (m_cPassSwitch.isInLeftField(inAwayY) == true)
    {
        for (int i = 1; i < 360; i++)
        {
            tmpPointAntiClock = m_Loca_Rotate(inX, inY, inAwayX, inAwayY, 360 - i);
            if (tmpPointAntiClock.x > 1000 && tmpPointAntiClock.x < 1600 && tmpPointAntiClock.y > 100 &&
                tmpPointAntiClock.y < 1100)
            {
                newpoint = tmpPointAntiClock;
                /*CString str;
                str.Format("逆时针%d°", i);
                PrintInfo(str);*/
                break;
            }
        }
    }
    else
    {
        for (int i = 1; i < 360; i++)
        {
            tmpPointClockWise = m_Loca_Rotate(inX, inY, inAwayX, inAwayY, i);
            if (tmpPointClockWise.x > 1000 && tmpPointClockWise.x < 1600 && tmpPointClockWise.y > 100 &&
                tmpPointClockWise.y < 1100)
            {
                newpoint = tmpPointClockWise;
                /*CString str;
                str.Format("顺时针%d°", i);
                PrintInfo(str);*/
                break;
            }
        }
    }

    return newpoint;
}

/*

*/
CPoint ITactic::m_Fix_KeepInTouchLine(int inX, int inY, int inAwayX, int inAwayY, int inAwayDist, int inOppoDist)
{
    CPoint newpoint;
    newpoint.x = inX;
    newpoint.y = inY;

    if (newpoint.x > 100 && newpoint.x < 1600 && newpoint.y > 100 && newpoint.y < 1100)
    {
        return newpoint;
    }

    CPoint tmpPoint;
    if (inY > 600)
    {
        // 该点在上半场，应该逆时针，（角度递减）
        for (int i = 1; i < 180; i++)
        {
            tmpPoint = m_Loca_Rotate(inX, inY, inAwayX, inAwayY, 360 - i);
            if (tmpPoint.x > 100 && tmpPoint.x < 1600 && tmpPoint.y > 100 && tmpPoint.y < 1100)
            {
                newpoint = tmpPoint;
                break;
            }
        }
    }
    else
    {
        // 该点在下半场，应该顺时针，（角度递增）
        for (int i = 1; i < 180; i++)
        {
            tmpPoint = m_Loca_Rotate(inX, inY, inAwayX, inAwayY, i);
            if (tmpPoint.x > 100 && tmpPoint.x < 1600 && tmpPoint.y > 100 && tmpPoint.y < 1100)
            {
                newpoint = tmpPoint;
                break;
            }
        }
    }

    return newpoint;
}

/*

*/
CPoint ITactic::m_Fix_KeepAwayFrom(int inX, int inY, int inAwayX, int inAwayY, int inAwayDist, int inTeammateDist,
                                   int inOppoDist)
{
    // 需要对输入的点坐标进行参数检查，如果
    CPoint newpoint;
    newpoint.x = inX;
    newpoint.y = inY;

    int nCurDist = m_CalDist(inX, inY, inAwayX, inAwayY);
    if (nCurDist < inAwayDist || m_Chk_InOurPenalty(inX, inY) || m_Chk_InOppoPenalty(inX, inY) ||
        m_Chk_OutOfField(inX, inY) || !m_Chk_AwayFromOppo(inX, inY, inOppoDist))
    {
        // 位置点不符合要求，开始修正
        if (nCurDist < inAwayDist)
        {
            // 1、将物体向背朝dist点的方向推开
            int nPushDist = inAwayDist - nCurDist;
            newpoint = m_Loca_Ray(inX, inY, inAwayX, inAwayY, nPushDist);
            newpoint = m_Loca_Rotate(newpoint.x, newpoint.y, inX, inY, 180);
        }

        int nAngle = 0;
        CPoint tmpPoint = newpoint;
        // 2、查看物体是否在合理的区域内
        // while (nAngle < 361)
        for (nAngle = 1; nAngle < 361; nAngle++)
        {
            // nAngle++;
            if (
				m_Chk_InOurPenalty(tmpPoint.x, tmpPoint.y) ||
				m_Chk_InOppoPenalty(tmpPoint.x, tmpPoint.y) ||
				m_Chk_OutOfField(tmpPoint.x, tmpPoint.y) ||
				!m_Chk_AwayFromOppo(tmpPoint.x, tmpPoint.y, inOppoDist) /*||
				!m_Chk_AwayFromTeammate(tmpPoint.x, tmpPoint.y, inTeammateDist)*/ //会避开正在执行这个角色的队员，去除
				)
            {
                // 计算出的位置非法，继续循环进行修正（后面是一系列根据非法原因进行调整的代码）
                // 1）因为在己方禁区造成的非法，将其移除己方禁区(循环执行，nAngle会累加到360)
                if (m_Chk_InOurPenalty(tmpPoint.x, tmpPoint.y) == true)
                {
                    if (newpoint.y < inAwayY)
                    {
                        // 当前点在远离点下方，逆时针旋转
                        tmpPoint = m_Loca_Rotate(newpoint.x, newpoint.y, inAwayX, inAwayY, 360 - nAngle);
                    }
                    else
                    {
                        // 当前点在远离点上方，顺时针旋转
                        tmpPoint = m_Loca_Rotate(newpoint.x, newpoint.y, inAwayX, inAwayY, nAngle);
                    }
                }

                // 2）因为在对方方禁区造成的非法，将其移除己出对方禁区(循环执行，nAngle会累加到360)
                if (m_Chk_InOppoPenalty(tmpPoint.x, tmpPoint.y) == true)
                {
                    if (newpoint.y < inAwayY)
                    {
                        // 当前点在远离点下方，顺时针旋转
                        tmpPoint = m_Loca_Rotate(newpoint.x, newpoint.y, inAwayX, inAwayY, nAngle);
                    }
                    else
                    {
                        // 当前点在远离点上方，逆时针旋转
                        tmpPoint = m_Loca_Rotate(newpoint.x, newpoint.y, inAwayX, inAwayY, 360 - nAngle);
                    }
                }

                // 3）因为出界造成的非法，将其移回场内(循环执行，nAngle会累加到360)
                if (m_Chk_OutOfField(tmpPoint.x, tmpPoint.y) == true)
                {
                    if (tmpPoint.x > inAwayX)
                    {
                        // 当前位置相对于远离点靠右（靠近对方半场）
                        if (tmpPoint.y > 600)
                        {
                            // 当前位置靠近对方半场，从上面出界，顺时针旋转
                            tmpPoint = m_Loca_Rotate(newpoint.x, newpoint.y, inAwayX, inAwayY, nAngle);
                        }
                        else
                        {
                            // 当前位置靠近对方半场，从下面出界，逆时针旋转
                            tmpPoint = m_Loca_Rotate(newpoint.x, newpoint.y, inAwayX, inAwayY, 360 - nAngle);
                        }
                    }
                    else
                    {
                        // 当前位置相对于远离点靠右左（靠近己方半场）
                        if (tmpPoint.y > 600)
                        {
                            // 当前位置靠近己方半场，从上面出界，逆时针旋转
                            tmpPoint = m_Loca_Rotate(newpoint.x, newpoint.y, inAwayX, inAwayY, 360 - nAngle);
                        }
                        else
                        {
                            // 当前位置靠近己方半场，从下面出界，顺时针旋转
                            tmpPoint = m_Loca_Rotate(newpoint.x, newpoint.y, inAwayX, inAwayY, nAngle);
                        }
                    }
                }

                // 4）因为与对手距离过近造成的非法，将其远离对手(循环执行，nAngle会累加到360)
                if (m_Chk_AwayFromOppo(tmpPoint.x, tmpPoint.y, inOppoDist) == false)
                {
                    if (newpoint.y < inAwayY)
                    {
                        // 当前点在远离点下方，顺时针旋转
                        tmpPoint = m_Loca_Rotate(newpoint.x, newpoint.y, inAwayX, inAwayY, nAngle);
                    }
                    else
                    {
                        // 当前点在远离点上方，逆时针旋转
                        tmpPoint = m_Loca_Rotate(newpoint.x, newpoint.y, inAwayX, inAwayY, 360 - nAngle);
                    }
                }
            }
            else
            {
                newpoint = tmpPoint;
                break;
            }

            // 角度超过360，转了一圈了，该退出了
            if (nAngle > 360)
            {
                break;
            }
        }
    }

    return newpoint;
}

/*

*/
bool ITactic::m_Chk_InOurPenalty(int inX, int inY)
{
    // 我方禁区
    bool res = false;
    if (inX >= 0 && inX < 230 && inY > 270 && inY < 930)
    {
        res = true;
    }
    return res;
}

/*

*/
bool ITactic::m_Chk_InOppoPenalty(int inX, int inY)
{
    // 对方禁区
    bool res = false;
    if (inX > 1570 && inX <= 1800 && inY > 270 && inY < 930)
    {
        res = true;
    }
    return res;
}

/*

*/
bool ITactic::m_Chk_OutOfField(int inX, int inY)
{
    // 场地外
    bool res = false;
    if (inX < 0 || inX > 1800 || inY < 0 || inY > 1200)
    {
        res = true;
    }
    return res;
}

/*

*/
bool ITactic::m_Chk_AwayFromTeammate(int inX, int inY, int inDist)
{
    // 和队友的距离
    bool res = true;
    uAgent *pAgent = pDataSummary->agent;
    for (int i = 0; i < AG_NUM; i++)
    {
        if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR)
        {
            if (m_CalDist(inX, inY, pAgent[i].x, pAgent[i].y) < inDist)
            {
                res = false;
                break;
            }
        }
    }
    return res;
}

/*

*/
bool ITactic::m_Chk_AwayFromOppo(int inX, int inY, int inDist)
{
    // 和对方队员的距离
    bool res = true;
    enemy *pEnamy = pDataSummary->t_enemy;
    for (int i = 0; i < pDataSummary->NumOfemy; i++)
    {
        if (m_CalDist(inX, inY, pEnamy[i].x, pEnamy[i].y) < inDist)
        {
            res = false;
            break;
        }
    }
    return res;
}

/*

*/
bool ITactic::m_Chk_InLeftBack(int inX, int inY)
{
    if (inX > 0 && inX < 500 && inY > 600 && inY < 1200)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*

*/
bool ITactic::m_Chk_InRightBack(int inX, int inY)
{
    if (inX > 0 && inX < 500 && inY > 0 && inY < 600)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*

*/
bool ITactic::m_Chk_InMidField(int inX, int inY)
{
    if (inX > 500 && inX < 1000 && inY > 0 && inY < 1200)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*

*/
void ITactic::m_GetPassCatchID()
{
    // 这个函数要能用，需要策略里设置好pDataSummary->posCatch和pDataSummary->posPass
    uAgent *pAgent = pDataSummary->agent;
    pDataSummary->nPassID = 0;
    pDataSummary->nCatchID = 0;

    // 寻找pass: 处于pass和moveto的队员
    for (int i = 1; i < AG_NUM; i++)
    {
        if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR)
        {
            if (pAgent[i].status == CTRL_PASS)
            {
                pDataSummary->nPassID = pAgent[i].ID;
                break;
            }

            if (pAgent[i].status == CTRL_MOVETO && pAgent[i].dest_x == pDataSummary->posPass.x &&
                pAgent[i].dest_y == pDataSummary->posPass.y)
            {
                pDataSummary->nPassID = pAgent[i].ID;
                break;
            }
        }
    }

    // 寻找catch: 处于catch和moveto的队员
    for (i = 1; i < AG_NUM; i++)
    {
        if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR)
        {
            if (pAgent[i].status == CTRL_CATCH)
            {
                pDataSummary->nCatchID = pAgent[i].ID;
                break;
            }

            if (pAgent[i].status == CTRL_MOVETO && pAgent[i].dest_x == pDataSummary->posCatch.x &&
                pAgent[i].dest_y == pDataSummary->posCatch.y)
            {
                pDataSummary->nCatchID = pAgent[i].ID;
                break;
            }
        }
    }
}

/*

*/
int ITactic::m_GetAgentIDNear(int inx, int iny)
{
    int nRetID = -1;
    int nMinDist = 10000;
    int nTempDeit = 0;
    uAgent *pAgent = pDataSummary->agent;
    for (int i = 1; i < AG_NUM; i++)
    {
        if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR)
        {
            nTempDeit = m_CalDist(pAgent[i].x, pAgent[i].y, inx, iny);
            if (nTempDeit < nMinDist)
            {
                nMinDist = nTempDeit;
                nRetID = pAgent[i].ID;
            }
        }
    }

    return nRetID;
}

/*

*/
int ITactic::m_GetAgentIDInStatus(int inCtrl)
{
    int nRetID = -1;
    int nMinDist = 10000;
    int nTempDeit = 0;
    uAgent *pAgent = pDataSummary->agent;
    for (int i = 1; i < AG_NUM; i++)
    {
        if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR)
        {

            if (pAgent[i].status == inCtrl)
            {
                nRetID = pAgent[i].ID;
                break;
            }
        }
    }

    return nRetID;
}

/*

*/
void ITactic::Get3BackObstToDef()
{
    int nTempDist = 0;
    // 在左后卫区域(左上)，找出离球门最近的障碍物并对其进行防传球
    int nLeftMinDistToGoal = 1800;
    int nLeftMinDistIndex = -1;
    for (int i = 0; i < pDataSummary->NumOfemy; i++)
    {
        // 先判断是否在该区域内
        if (m_Chk_InLeftBack(pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y) == true)
        {
            // 计算该点与球门距离，如果是最短则记录下来
            nTempDist = m_CalDist(pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y, 0, 600);
            if (nTempDist < nLeftMinDistToGoal)
            {
                // 记录最小值
                nLeftMinDistToGoal = nTempDist;
                nLeftMinDistIndex = i;
            }
        }
    }

    // 在右后卫区域(左下)，找出离球门最近的障碍物并对其进行防传球
    int nRightMinDistToGoal = 1800;
    int nRightMinDistIndex = -1;
    for (i = 0; i < pDataSummary->NumOfemy; i++)
    {
        // 先判断是否在该区域内
        if (m_Chk_InRightBack(pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y) == true)
        {
            // 计算该点与球门距离，如果是最短则记录下来
            nTempDist = m_CalDist(pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y, 0, 600);
            if (nTempDist < nRightMinDistToGoal)
            {
                // 记录最小值
                nRightMinDistToGoal = nTempDist;
                nRightMinDistIndex = i;
            }
        }
    }

    // 在中场区域(中段)，找出离球门最近的障碍物并对其进行防传球
    int nMidMinDistToGoal = 1800;
    int nMidMinDistIndex = -1;
    for (i = 0; i < pDataSummary->NumOfemy; i++)
    {
        // 先判断是否在该区域内
        if (m_Chk_InMidField(pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y) == true)
        {
            // 计算该点与球门距离，如果是最短则记录下来
            nTempDist = m_CalDist(pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y, 0, 600);
            if (nTempDist < nMidMinDistToGoal)
            {
                // 记录最小值
                nMidMinDistToGoal = nTempDist;
                nMidMinDistIndex = i;
            }
        }
    }

    // 左 中 右 离球门最近的障碍物都找到了，中场障碍物可能会和左右其中一个障碍物太接近，对其进行合并
    // 1）左后方和中场
    if (nLeftMinDistIndex >= 0 && nMidMinDistIndex >= 0)
    {
        nTempDist = m_CalDist(pDataSummary->t_enemy[nLeftMinDistIndex].x, pDataSummary->t_enemy[nLeftMinDistIndex].y,
                              pDataSummary->t_enemy[nMidMinDistIndex].x, pDataSummary->t_enemy[nMidMinDistIndex].y);
        if (nTempDist < 100)
        {
            nMidMinDistIndex = -1;
        }
    }

    // 2）右后方和中场
    if (nRightMinDistIndex >= 0 && nMidMinDistIndex >= 0)
    {
        nTempDist = m_CalDist(pDataSummary->t_enemy[nRightMinDistIndex].x, pDataSummary->t_enemy[nRightMinDistIndex].y,
                              pDataSummary->t_enemy[nMidMinDistIndex].x, pDataSummary->t_enemy[nMidMinDistIndex].y);
        if (nTempDist < 100)
        {
            nMidMinDistIndex = -1;
        }
    }

    // 三个防守障碍物已经选取完毕，对数量进行统计并赋值到共享数据结构里
    int nTmpIndex = 0;
    // 1)左后卫
    if (nLeftMinDistIndex >= 0)
    {
        // 赋值到共享数据以便显示
        pDataSummary->arObstIndexBackDef[nTmpIndex] = nLeftMinDistIndex;

        nTmpIndex++;
    }
    // 2）右后卫
    if (nRightMinDistIndex >= 0)
    {
        // 赋值到共享数据以便显示
        pDataSummary->arObstIndexBackDef[nTmpIndex] = nRightMinDistIndex;

        nTmpIndex++;
    }
    // 3）中卫
    if (nMidMinDistIndex >= 0)
    {
        // 赋值到共享数据以便显示
        pDataSummary->arObstIndexBackDef[nTmpIndex] = nMidMinDistIndex;

        nTmpIndex++;
    }

    // 赋值到共享数据以便显示
    pDataSummary->nNumOfObstBackDef = nTmpIndex;
}

/*

*/
static enemy enemy_NotSort[20];
static enemy tmpEnemy;
void ITactic::SortEnemyDistToOurGoal(stDataSummary *inDataSummary)
{
    if (inDataSummary->NumOfemy == 0)
    {
        return;
    }
    // 拷贝数据
    memcpy(enemy_NotSort, inDataSummary->t_enemy, 20 * sizeof(enemy));

    int nMinDist = 5000;
    int nTmpDist = 0;
    int nMinIndex = 0;
    // 冒泡法排序
    for (int k = 0; k < inDataSummary->NumOfemy; k++)
    {
        nMinDist = 5000;
        for (int i = k; i < inDataSummary->NumOfemy; i++)
        {
            // 找出当前队长与球门距离最近的障碍物，并复制到队首（k是队首）
            nTmpDist = m_CalDist(enemy_NotSort[i].x, enemy_NotSort[i].y, 0, 600);
            if (nTmpDist < nMinDist)
            {
                nMinDist = nTmpDist;
                nMinIndex = i;
            }
        }

        // 复制（k是队首）
        if (nMinIndex > k)
        {
            memcpy(&tmpEnemy, &enemy_NotSort[k], sizeof(enemy));
            memcpy(&enemy_NotSort[k], &enemy_NotSort[nMinIndex], sizeof(enemy));
            memcpy(&enemy_NotSort[nMinIndex], &tmpEnemy, sizeof(enemy));
        }
    }

    // 将排序结果复制回去
    memcpy(inDataSummary->t_enemy, enemy_NotSort, 20 * sizeof(enemy));
}

/*

*/
void ITactic::Get3ObstCloseToDef()
{
    SortEnemyDistToOurGoal(pDataSummary);
    int nNumOfObstCloseToGoal = 0;
    int nTmpDist = 0;
    // 遍历已有的障碍物
    for (size_t i = 0; i < pDataSummary->NumOfemy; i++)
    {
        // 计算每个Enemy到己方球门距离
        nTmpDist = m_CalDist(pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y, 0, 600);
        if (nTmpDist < 1000)
        {
            pDataSummary->arObstIndexBackDef[nNumOfObstCloseToGoal] = i;
            nNumOfObstCloseToGoal++;
            if (nNumOfObstCloseToGoal >= 3)
            {
                break;
            }
        }
    }
    // 障碍物个数赋值
    pDataSummary->nNumOfObstBackDef = nNumOfObstCloseToGoal;

    // 修正防守顺序
    m_BackEnemyFix();
}

/*

*/
void ITactic::m_BackEnemyFix()
{
    // 1、查看球运动方向上的敌人是否在所检测的区域内,如果在，将其放在防守队列的首位
    if (pDataSummary->nNumOfPassToEnemy > 0)
    {
        if (m_Chk_InLeftBack(pDataSummary->posPassToEnemy[0].x, pDataSummary->posPassToEnemy[0].y) ||
            m_Chk_InRightBack(pDataSummary->posPassToEnemy[0].x, pDataSummary->posPassToEnemy[0].y) ||
            m_Chk_InMidField(pDataSummary->posPassToEnemy[0].x, pDataSummary->posPassToEnemy[0].y))
        {
            // 对方接球队员在防守区域内，查看是否在防守队列内
            int curCatchEnemyIndex = -1;
            for (int i = 0; i < pDataSummary->nNumOfObstBackDef; i++)
            {
                if (pDataSummary->t_enemy[pDataSummary->arObstIndexBackDef[i]].x == pDataSummary->posPassToEnemy[0].x &&
                    pDataSummary->t_enemy[pDataSummary->arObstIndexBackDef[i]].y == pDataSummary->posPassToEnemy[0].y)
                {
                    // 对方接球队员在防守队列内
                    curCatchEnemyIndex = i;
                    break;
                }
            }

            if (curCatchEnemyIndex >= 0)
            {
                // 对方接球队员在防守队列内,将其移动到防守队列首位
                int nTmp = pDataSummary->arObstIndexBackDef[0];
                pDataSummary->arObstIndexBackDef[0] = pDataSummary->arObstIndexBackDef[curCatchEnemyIndex];
                pDataSummary->arObstIndexBackDef[curCatchEnemyIndex] = nTmp;
            }
            else
            {
                // 对方接球队员不在队列里，则放弃防守离其最近的障碍物，替换成对方接球队员位置，且移至队列首位
                int nMinIndex = 0;
                int nMinDist = 1800;
                int nTmpDist = 0;
                for (int i = 0; i < pDataSummary->nNumOfObstBackDef; i++)
                {
                    nTmpDist = m_CalDist(pDataSummary->posPassToEnemy[0].x, pDataSummary->posPassToEnemy[0].y,
                                         pDataSummary->t_enemy[pDataSummary->arObstIndexBackDef[i]].x,
                                         pDataSummary->t_enemy[pDataSummary->arObstIndexBackDef[i]].y);
                    if (nTmpDist < nMinDist)
                    {
                        nMinIndex = i;
                        nMinDist = nTmpDist;
                    }
                }
                // 此时nMinIndex里就是离对方接球队员距离最近的arObstIndexBackDef下标，
                // 寻找pDataSummary->posPassToEnemy[0]在pDataSummary->t_enemy中的下标，将其放到防守队列首位
                int nCatchEnemyIndex = 0;
                for (i = 0; i < pDataSummary->NumOfemy; i++)
                {
                    if (pDataSummary->t_enemy[i].x == pDataSummary->posPassToEnemy[0].x &&
                        pDataSummary->t_enemy[i].y == pDataSummary->posPassToEnemy[0].y)
                    {
                        nCatchEnemyIndex = i;
                        break;
                    }
                }
                // 找到对方接球队员的下标，将其放置到防守队列的首部
                for (i = nMinIndex; i > 0; i--)
                {
                    pDataSummary->arObstIndexBackDef[i] = pDataSummary->arObstIndexBackDef[i - 1];
                }
                // pDataSummary->arObstIndexBackDef[nMinIndex] = pDataSummary->arObstIndexBackDef[0];
                pDataSummary->arObstIndexBackDef[0] = nCatchEnemyIndex;
            }
        }
    }

    // 2、将离球近于一定距离的对方队员排到防守列表首位
    // 先判断是否有障碍物与球足够接近了
    int nDistToBall = DIST_TAC_DEFBALL;
    int nIndexEnemyNearBall = -1;
    int nTmpDist;
    for (int i = 0; i < pDataSummary->NumOfemy; i++)
    {
        // 先判断是否在防守区域内
        if (m_Chk_InLeftBack(pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y) == true ||
            m_Chk_InRightBack(pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y) == true ||
            m_Chk_InMidField(pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y) == true)
        {
            nTmpDist = m_CalDist(pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y, pDataSummary->cGlobalBall.x,
                                 pDataSummary->cGlobalBall.y);
            if (nTmpDist < nDistToBall)
            {
                nIndexEnemyNearBall = i;
                break;
            }
        }
    }
    // 通过nIndexEnemyNearBall判断是否有防守区域内的对方队员离球足够近
    if (nIndexEnemyNearBall >= 0)
    {
        // 查看该对方队员是否在防守队列里
        int nIndexInObstDef = -1;
        for (int i = 0; i < pDataSummary->nNumOfObstBackDef; i++)
        {
            if (pDataSummary->arObstIndexBackDef[i] == nIndexEnemyNearBall)
            {
                nIndexInObstDef = i;
                break;
            }
        }

        if (nIndexInObstDef > 0)
        {
            // 该对方队员在防守队列里，将其移动到防守队列首位（其他队员依次往后移动）
            int nTmpIndex = pDataSummary->arObstIndexBackDef[nIndexInObstDef];
            for (int i = nIndexInObstDef; i > 0; i--)
            {
                pDataSummary->arObstIndexBackDef[i] = pDataSummary->arObstIndexBackDef[i - 1];
            }
            pDataSummary->arObstIndexBackDef[0] = nTmpIndex;
        }
        else
        {
            // 该对方队员不在防守队列里,挑选离该队员最近的防守对象进行替换并移动到队列首位
            int nMinIndex = 0;
            int nMinDist = 1800;
            int nTmpDist = 0;
            for (int i = 0; i < pDataSummary->nNumOfObstBackDef; i++)
            {
                nTmpDist = m_CalDist(pDataSummary->t_enemy[nIndexEnemyNearBall].x,
                                     pDataSummary->t_enemy[nIndexEnemyNearBall].y,
                                     pDataSummary->t_enemy[pDataSummary->arObstIndexBackDef[i]].x,
                                     pDataSummary->t_enemy[pDataSummary->arObstIndexBackDef[i]].y);
                if (nTmpDist < nMinDist)
                {
                    nMinIndex = i;
                    nMinDist = nTmpDist;
                }
            }
            // 将pDataSummary->arObstIndexBackDef中的nMinIndex替换成nIndexEnemyNearBall并移动到首位(其他队列依次往后移动)
            for (i = nMinIndex; i > 0; i--)
            {
                pDataSummary->arObstIndexBackDef[i] = pDataSummary->arObstIndexBackDef[i - 1];
            }
            // pDataSummary->arObstIndexBackDef[nMinIndex] = pDataSummary->arObstIndexBackDef[0];
            pDataSummary->arObstIndexBackDef[0] = nIndexEnemyNearBall;
        }
    }
}

/*

*/
int ITactic::m_MinDistToMoveToRoleBefore(int inX, int inY, int inIndex)
{
    uRole *pRole = pTactics->role;
    int nMinDist = -1;
    int nTmpDist;
    for (int i = 0; i < inIndex; i++)
    {
        if (pRole[i].ctrl == CTRL_MOVETO)
        {
            if (nMinDist < 0)
            {
                nMinDist = m_CalDist(inX, inY, pRole[i].nDest_x, pRole[i].nDest_y);
            }
            else
            {
                nTmpDist = m_CalDist(inX, inY, pRole[i].nDest_x, pRole[i].nDest_y);
                if (nTmpDist < nMinDist)
                {
                    nMinDist = nTmpDist;
                }
            }
        }
    }

    return nMinDist;
}

/*

*/
void ITactic::m_Fix_MoveToAway()
{
    CString strInfo;
    CPoint tmpPoint;
    uRole *pRole = (uRole *)pTactics->role;
    for (int i = 1; i < AG_NUM; i++)
    {
        if (pRole[i].ctrl == CTRL_MOVETO)
        {
            int nMinDist = m_MinDistToMoveToRoleBefore(pRole[i].nDest_x, pRole[i].nDest_y, i);
            if (nMinDist > 0 && nMinDist < 100)
            {
                int nDistToBall = m_CalDist(pRole[i].nDest_x, pRole[i].nDest_y, pDataSummary->cGlobalBall.x,
                                            pDataSummary->cGlobalBall.y);
                // if (pRole[i].nDest_y < 600)
                {
                    // 右半场,顺时针旋转
                    for (int a = 1; a < 180; a++)
                    {
                        tmpPoint = m_Loca_Rotate(pRole[i].nDest_x, pRole[i].nDest_y, pDataSummary->cGlobalBall.x,
                                                 pDataSummary->cGlobalBall.y, a);
                        int nMinDist = m_MinDistToMoveToRoleBefore(tmpPoint.x, tmpPoint.y, i);
                        if (nMinDist > 100)
                        {

                            pRole[i].nDest_x = tmpPoint.x;
                            pRole[i].nDest_y = tmpPoint.y;
                            /*strInfo.Format("ITactic::m_Fix_MoveToAway顺时针转了%d度，坐标是(%d,%d)", i,
                            pRole[i].nDest_x, pRole[i].nDest_y); PrintInfo(strInfo);*/
                            break;
                        }
                    }
                }
                // else
                //{
                //	//左半场,逆时针旋转
                //	for (int a = 1; a < 180; a++)
                //	{
                //		tmpPoint = m_Loca_Rotate(pRole[i].nDest_x, pRole[i].nDest_y, pDataSummary->cGlobalBall.x,
                // pDataSummary->cGlobalBall.y, 360 - a); 		int nMinDist = m_MinDistToMoveToRoleBefore(tmpPoint.x,
                // tmpPoint.y, i); 		if (nMinDist > 100)
                //		{
                //			pRole[i].nDest_x = tmpPoint.x;
                //			pRole[i].nDest_y = tmpPoint.y;
                //			strInfo.Format("逆时针转了%d度，坐标是(%d,%d)", i, pRole[i].nDest_x, pRole[i].nDest_y);
                //			PrintInfo(strInfo);
                //			break;
                //		}
                //	}
                // }
            }
        }
    }
}

/*
打印信息
*/
void ITactic::PrintInfo(CString inStr)
{
    if (pInfoList != NULL)
    {
        pInfoList->AddString(inStr);
        pInfoList->SetCurSel(pInfoList->GetCount() - 1);
    }
}
