#include "stdafx.h"
#include "Tac_Playing_AreaDef.h"

// 传球测试标记
// #define TEST_PASSING

CTac_Playing_AreaDef::CTac_Playing_AreaDef()
{
}

CTac_Playing_AreaDef::~CTac_Playing_AreaDef()
{
}

void CTac_Playing_AreaDef::Reset()
{
    if (pDataSummary != NULL)
    {
        pDataSummary->bPassing = false;
    }
}

bool CTac_Playing_AreaDef::m_bOppoHandlingBall()
{
    // pDataSummary->NumOfemy ： 融合后的障碍物数量
    // pDataSummary->t_enemy[i] 融合后的障碍物坐标
    bool res = false;

    for (int i = 0; i < pDataSummary->NumOfemy; i++)
    {
        if (m_CalDist(pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y, pDataSummary->cGlobalBall.x,
                      pDataSummary->cGlobalBall.y) < 100)
        {
            // 障碍物与球距离小于一定值即认为其已经在控球
            res = true;
            break;
        }
    }
    return res;
}

// 后场防守的角色分配(虽然参数是分左中右，但是实际分配角色的时候是不分的)
void CTac_Playing_AreaDef::AreaDefRoles(uRole *pRole, int inLeftBack, int inRightBack, int inMidBack)
{
    int nTempDist = 0;
    // 获取需要防守的障碍物列表
    // Get3BackObstToDef();
    Get3ObstCloseToDef();
    int arEnemyIndexToDef[3];
    for (int i = 0; i < 3; i++)
    {
        arEnemyIndexToDef[i] = pDataSummary->arObstIndexBackDef[i];
    }

    // 开始正式分配角色
    int nRoleDefIndex[3];
    nRoleDefIndex[0] = inLeftBack;
    nRoleDefIndex[1] = inRightBack;
    nRoleDefIndex[2] = inMidBack;

    // 对其优先级进行排序(由于只有三个数，所以进行手动冒泡)
    int nTmpIndex = 0;
    for (i = 1; i < 3; i++)
    {
        if (nRoleDefIndex[i] < nRoleDefIndex[0])
        {
            nTmpIndex = nRoleDefIndex[i];
            nRoleDefIndex[i] = nRoleDefIndex[0];
            nRoleDefIndex[0] = nTmpIndex;
        }
    }
    if (nRoleDefIndex[1] > nRoleDefIndex[2])
    {
        nTmpIndex = nRoleDefIndex[1];
        nRoleDefIndex[1] = nRoleDefIndex[2];
        nRoleDefIndex[2] = nTmpIndex;
    }

    // 到此，nRoleDefIndex[3]内的序号已经按从小到大排列了。开始分配任务

    // a)计算需要参与到防守任务的人数
    if (pDataSummary->nNumOfObstBackDef > 3)
    {
        pDataSummary->nNumOfObstBackDef = 3;
        PrintInfo("后防需要防守的障碍物数量异常");
    }

    int nNumRoleDef = pDataSummary->nNumOfObstBackDef; // 需要参与到障碍物防守中的人数

    // b)为障碍物防守角色分配任务
    for (i = 0; i < nNumRoleDef; i++)
    {
        nTempDist =
            m_CalDist(pDataSummary->t_enemy[arEnemyIndexToDef[i]].x, pDataSummary->t_enemy[arEnemyIndexToDef[i]].y,
                      pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);
        if (nTempDist > DIST_TAC_DEFBALL)
        {
            // 球离他远，防传球
            pRole[nRoleDefIndex[i]].ctrl = CTRL_DEF_BALL;
            pRole[nRoleDefIndex[i]].def_dist = DIST_TAC_DEFBALL;
            pRole[nRoleDefIndex[i]].nDest_x = pDataSummary->t_enemy[arEnemyIndexToDef[i]].x;
            pRole[nRoleDefIndex[i]].nDest_y = pDataSummary->t_enemy[arEnemyIndexToDef[i]].y;
        }
        else
        {
            // 球离他近，防射门
            pRole[nRoleDefIndex[i]].ctrl = CTRL_DEF_GOAL;
            pRole[nRoleDefIndex[i]].def_dist = DIST_TAC_DEFGOAL;
            pRole[nRoleDefIndex[i]].nDest_x = pDataSummary->cGlobalBall.x;
            pRole[nRoleDefIndex[i]].nDest_y = pDataSummary->cGlobalBall.y;
        }
    }

    // 为没有防守任务的队员分配站位任务，需要依次检测各个防区有无障碍物，所以需要手动逐个确认
    nTmpIndex = nNumRoleDef; // 没有防守任务的角色index起始位置
    bool bEnemyInZone = false;
    CPoint posDefMove;
    // 1)左后卫区域跑位(检测该区域是否有敌人，若没有就只是后卫跑位)
    bEnemyInZone = false;
    for (i = 0; i < pDataSummary->nNumOfObstBackDef; i++)
    {
        // 先判断是否在该区域内
        if (m_Chk_InLeftBack(pDataSummary->t_enemy[arEnemyIndexToDef[i]].x,
                             pDataSummary->t_enemy[arEnemyIndexToDef[i]].y) == true)
        {
            bEnemyInZone = true;
        }
    }
    if (bEnemyInZone == false)
    {
        // 没有防守任务，跑位
        if (nTmpIndex < 3)
        {
            posDefMove =
                m_Fix_KeepAwayFrom(300, 900, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, 150, 0, 100);
            pRole[nRoleDefIndex[nTmpIndex]].ctrl = CTRL_MOVETO;
            pRole[nRoleDefIndex[nTmpIndex]].nDest_x = posDefMove.x;
            pRole[nRoleDefIndex[nTmpIndex]].nDest_y = posDefMove.y;
            pRole[nRoleDefIndex[nTmpIndex]].nDest_angle =
                m_Loca_CalAngle(posDefMove.x, posDefMove.y, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);
            nTmpIndex++;
        }
    }
    // 2)右后卫区域跑位
    bEnemyInZone = false;
    for (i = 0; i < pDataSummary->nNumOfObstBackDef; i++)
    {
        // 先判断是否在该区域内
        if (m_Chk_InRightBack(pDataSummary->t_enemy[arEnemyIndexToDef[i]].x,
                              pDataSummary->t_enemy[arEnemyIndexToDef[i]].y) == true)
        {
            bEnemyInZone = true;
        }
    }
    if (bEnemyInZone == false)
    {
        // 没有防守任务，跑位
        if (nTmpIndex < 3)
        {
            posDefMove =
                m_Fix_KeepAwayFrom(300, 300, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, 150, 0, 100);
            pRole[nRoleDefIndex[nTmpIndex]].ctrl = CTRL_MOVETO;
            pRole[nRoleDefIndex[nTmpIndex]].nDest_x = posDefMove.x;
            pRole[nRoleDefIndex[nTmpIndex]].nDest_y = posDefMove.y;
            pRole[nRoleDefIndex[nTmpIndex]].nDest_angle =
                m_Loca_CalAngle(posDefMove.x, posDefMove.y, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);
            nTmpIndex++;
        }
    }
    // 3)中场区域跑位
    bEnemyInZone = false;
    for (i = 0; i < pDataSummary->nNumOfObstBackDef; i++)
    {
        // 先判断是否在该区域内
        if (m_Chk_InMidField(pDataSummary->t_enemy[arEnemyIndexToDef[i]].x,
                             pDataSummary->t_enemy[arEnemyIndexToDef[i]].y) == true)
        {
            bEnemyInZone = true;
        }
    }
    if (bEnemyInZone == false)
    {
        // 没有防守任务，跑位
        if (nTmpIndex < 3)
        {
            posDefMove =
                m_Fix_KeepAwayFrom(800, 600, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, 150, 0, 100);
            pRole[nRoleDefIndex[nTmpIndex]].ctrl = CTRL_MOVETO;
            pRole[nRoleDefIndex[nTmpIndex]].nDest_x = posDefMove.x;
            pRole[nRoleDefIndex[nTmpIndex]].nDest_y = posDefMove.y;
            pRole[nRoleDefIndex[nTmpIndex]].nDest_angle =
                m_Loca_CalAngle(posDefMove.x, posDefMove.y, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);
            nTmpIndex++;
        }
    }
}

void CTac_Playing_AreaDef::MidfieldDefRoles(uRole *pRole, int inLeftDef, int inRightDef)
{
    int nTempDist = 0;
    // 获取需要防守的障碍物列表
    // Get3BackObstToDef();
    Get3ObstCloseToDef();
    int arEnemyIndexToDef[3];
    for (int i = 0; i < 3; i++)
    {
        arEnemyIndexToDef[i] = pDataSummary->arObstIndexBackDef[i];
    }

    nTempDist = 0;
    int nLeftMinDist = 5000;
    int nRightMinDist = 5000;
    int nLeftDefEnemyID = -1;
    int nRightDefEnemyID = -1;
    // 1寻找左半场最前面的球员
    for (i = 0; i < 3; i++)
    {
        if (pDataSummary->t_enemy[arEnemyIndexToDef[i]].x > 0 && pDataSummary->t_enemy[arEnemyIndexToDef[i]].x < 1000 &&
            pDataSummary->t_enemy[arEnemyIndexToDef[i]].y > 600 && pDataSummary->t_enemy[arEnemyIndexToDef[i]].y < 1200)
        {
            nTempDist = m_CalDist(pDataSummary->t_enemy[arEnemyIndexToDef[i]].x,
                                  pDataSummary->t_enemy[arEnemyIndexToDef[i]].y, 0, 600);
            if (nTempDist < nLeftMinDist)
            {
                nLeftDefEnemyID = arEnemyIndexToDef[i];
                nLeftMinDist = nTempDist;
            }
        }
    }

    // 1寻找右半场最前面的球员
    for (i = 0; i < 3; i++)
    {
        if (pDataSummary->t_enemy[arEnemyIndexToDef[i]].x > 0 && pDataSummary->t_enemy[arEnemyIndexToDef[i]].x < 1000 &&
            pDataSummary->t_enemy[arEnemyIndexToDef[i]].y > 0 && pDataSummary->t_enemy[arEnemyIndexToDef[i]].y < 600)
        {
            nTempDist = m_CalDist(pDataSummary->t_enemy[arEnemyIndexToDef[i]].x,
                                  pDataSummary->t_enemy[arEnemyIndexToDef[i]].y, 0, 600);
            if (nTempDist < nRightMinDist)
            {
                nRightDefEnemyID = arEnemyIndexToDef[i];
                nRightDefEnemyID = nTempDist;
            }
        }
    }

    // 根据左右半场的要防守的Enemy的id来决定要防守的次序
    int arDefEnemyID[2];
    arDefEnemyID[0] = -1;
    arDefEnemyID[1] = -1;
    POINT posDefMove[2];
    bool bDefMove = false;
    if (nRightDefEnemyID >= 0 && nLeftDefEnemyID >= 0)
    {
        if (nRightMinDist > nLeftMinDist)
        {
            arDefEnemyID[0] = nLeftDefEnemyID;
            arDefEnemyID[1] = nRightDefEnemyID;
            posDefMove[0].x = 300;
            posDefMove[0].y = 900;
            posDefMove[1].x = 300;
            posDefMove[1].y = 300;
        }
        else
        {
            arDefEnemyID[0] = nRightDefEnemyID;
            arDefEnemyID[1] = nLeftDefEnemyID;
            posDefMove[0].x = 300;
            posDefMove[0].y = 300;
            posDefMove[1].x = 300;
            posDefMove[1].y = 900;
        }
    }
    else
    {
        if (nRightDefEnemyID >= 0)
        {
            // 防守右半场，左半场跑位
            arDefEnemyID[0] = nRightDefEnemyID;
            posDefMove[0].x = 300;
            posDefMove[0].y = 300;
            posDefMove[1].x = 300;
            posDefMove[1].y = 900;
        }
        if (nLeftDefEnemyID >= 0)
        {
            // 防守左半场，右半场跑位
            arDefEnemyID[0] = nLeftDefEnemyID;
            posDefMove[0].x = 300;
            posDefMove[0].y = 900;
            posDefMove[1].x = 300;
            posDefMove[1].y = 300;
        }
    }

    // 开始分配角色
    int arDefRoleIndex[2];
    arDefRoleIndex[0] = inLeftDef;
    arDefRoleIndex[1] = inRightDef;
    int nTempIndex;
    // 将防守队员Index按照
    if (arDefRoleIndex[0] > arDefRoleIndex[1])
    {
        nTempIndex = arDefRoleIndex[0];
        arDefRoleIndex[0] = arDefRoleIndex[1];
        arDefRoleIndex[1] = nTempIndex;
    }
    // 遍历分配角色
    for (i = 0; i < 2; i++)
    {
        if (arDefEnemyID[i] >= 0)
        {
            if (m_CalDist(pDataSummary->t_enemy[arDefEnemyID[i]].x, pDataSummary->t_enemy[arDefEnemyID[i]].y,
                          pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y) > DIST_TAC_DEFBALL)
            {
                // 其与球距离较远，防传球
                pRole[arDefRoleIndex[i]].ctrl = CTRL_DEF_BALL;
                pRole[arDefRoleIndex[i]].nDest_x = pDataSummary->t_enemy[arDefEnemyID[i]].x;
                pRole[arDefRoleIndex[i]].nDest_y = pDataSummary->t_enemy[arDefEnemyID[i]].y;
            }
            else
            {
                // 其与球距离较近，防射门
                pRole[arDefRoleIndex[i]].ctrl = CTRL_DEF_GOAL;
                pRole[arDefRoleIndex[i]].nDest_x = pDataSummary->cGlobalBall.x;
                pRole[arDefRoleIndex[i]].nDest_y = pDataSummary->cGlobalBall.y;
            }
        }
        else
        {
            // 没有防守任务，执行跑位任务
            pRole[arDefRoleIndex[i]].ctrl = CTRL_MOVETO;
            pRole[arDefRoleIndex[i]].nDest_x = posDefMove[i].x;
            pRole[arDefRoleIndex[i]].nDest_y = posDefMove[i].y;
        }
    }
}

// void CTac_Playing_AreaDef::AreaDefRoles(uRole* pRole, int inLeftBack, int inRightBack, int inMidBack)
//{
//	int nTempDist = 0;
//	//在左后卫区域(左上)，找出离球门最近的障碍物并对其进行防传球
//	int nLeftMinDistToGoal = 1800;
//	int nLeftMinDistIndex = -1;
//	for (int i = 0; i < pDataSummary->NumOfemy; i++)
//	{
//		//先判断是否在该区域内
//		if (
//			pDataSummary->t_enemy[i].x > 0 &&
//			pDataSummary->t_enemy[i].x < 900 &&
//			pDataSummary->t_enemy[i].y > 600 &&
//			pDataSummary->t_enemy[i].y < 1200
//			)
//		{
//			//计算该点与球门距离，如果是最短则记录下来
//			nTempDist = m_CalDist(pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y, 0, 600);
//			if (nTempDist < nLeftMinDistToGoal)
//			{
//				//记录最小值
//				nLeftMinDistToGoal = nTempDist;
//				nLeftMinDistIndex = i;
//			}
//		}
//	}
//
//	//在右后卫区域(左下)，找出离球门最近的障碍物并对其进行防传球
//	int nRightMinDistToGoal = 1800;
//	int nRightMinDistIndex = -1;
//	for (int i = 0; i < pDataSummary->NumOfemy; i++)
//	{
//		//先判断是否在该区域内
//		if (
//			pDataSummary->t_enemy[i].x > 0 &&
//			pDataSummary->t_enemy[i].x < 900 &&
//			pDataSummary->t_enemy[i].y > 0 &&
//			pDataSummary->t_enemy[i].y < 600
//			)
//		{
//			//计算该点与球门距离，如果是最短则记录下来
//			nTempDist = m_CalDist(pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y, 0, 600);
//			if (nTempDist < nRightMinDistToGoal)
//			{
//				//记录最小值
//				nRightMinDistToGoal = nTempDist;
//				nRightMinDistIndex = i;
//			}
//		}
//	}
//
//	//在中场区域(中段)，找出离球门最近的障碍物并对其进行防传球
//	int nMidMinDistToGoal = 1800;
//	int nMidMinDistIndex = -1;
//	for (int i = 0; i < pDataSummary->NumOfemy; i++)
//	{
//		//先判断是否在该区域内
//		if (
//			pDataSummary->t_enemy[i].x > 700 &&
//			pDataSummary->t_enemy[i].x < 1100 &&
//			pDataSummary->t_enemy[i].y > 0 &&
//			pDataSummary->t_enemy[i].y < 1200
//			)
//		{
//			//计算该点与球门距离，如果是最短则记录下来
//			nTempDist = m_CalDist(pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y, 0, 600);
//			if (nTempDist < nMidMinDistToGoal)
//			{
//				//记录最小值
//				nMidMinDistToGoal = nTempDist;
//				nMidMinDistIndex = i;
//			}
//		}
//	}
//
//	//左 中 右 离球门最近的障碍物都找到了，中场障碍物可能会和左右其中一个障碍物太接近，对其进行合并
//	//1）左后方和中场
//	nTempDist = m_CalDist(
//		pDataSummary->t_enemy[nLeftMinDistIndex].x, pDataSummary->t_enemy[nLeftMinDistIndex].y,
//		pDataSummary->t_enemy[nMidMinDistIndex].x, pDataSummary->t_enemy[nMidMinDistIndex].y
//		);
//	if (nTempDist < 100)
//	{
//		nLeftMinDistIndex = -1;
//	}
//
//	//2）右后方和中场
//	nTempDist = m_CalDist(
//		pDataSummary->t_enemy[nRightMinDistIndex].x, pDataSummary->t_enemy[nRightMinDistIndex].y,
//		pDataSummary->t_enemy[nMidMinDistIndex].x, pDataSummary->t_enemy[nMidMinDistIndex].y
//		);
//	if (nTempDist < 100)
//	{
//		nRightMinDistIndex = -1;
//	}
//
//	//开始正式分配角色
//	int nRoleDefIndex[3];
//	nRoleDefIndex[0] = inLeftBack;
//	nRoleDefIndex[1] = inRightBack;
//	nRoleDefIndex[2] = inMidBack;
//
//	//对其优先级进行排序(由于只有三个数，所以进行手动冒泡)
//	int nTmpIndex = 0;
//	for (int i = 1; i < 3; i++)
//	{
//		if (nRoleDefIndex[i] < nRoleDefIndex[0])
//		{
//			nTmpIndex = nRoleDefIndex[i];
//			nRoleDefIndex[i] = nRoleDefIndex[0];
//			nRoleDefIndex[0] = nTmpIndex;
//		}
//	}
//	if (nRoleDefIndex[1] > nRoleDefIndex[2])
//	{
//		nTmpIndex = nRoleDefIndex[1];
//		nRoleDefIndex[1] = nRoleDefIndex[2];
//		nRoleDefIndex[2] = nTmpIndex;
//	}
//
//	//到此，nRoleDefIndex[3]内的序号已经按从小到大排列了。从后往前挑选防守对象
//	nTmpIndex = 0;
//	//1)左后卫
//	if (nLeftMinDistIndex >= 0)
//	{
//		//判断其是否带球
//		nTempDist = m_CalDist(
//			pDataSummary->t_enemy[nLeftMinDistIndex].x, pDataSummary->t_enemy[nLeftMinDistIndex].y,
//			pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y
//			);
//		if (nTempDist > 50)
//		{
//			//球离他远，防传球
//			pRole[nRoleDefIndex[nTmpIndex]].ctrl = CTRL_DEF_BALL;
//			pRole[nRoleDefIndex[nTmpIndex]].def_dist = DIST_TAC_DEFBALL;
//		}
//		else
//		{
//			//球离他近，防射门
//			pRole[nRoleDefIndex[nTmpIndex]].ctrl = CTRL_DEF_GOAL;
//			pRole[nRoleDefIndex[nTmpIndex]].def_dist = DIST_TAC_DEFGOAL;
//		}
//		pRole[nRoleDefIndex[nTmpIndex]].nDest_x = pDataSummary->t_enemy[nLeftMinDistIndex].x;
//		pRole[nRoleDefIndex[nTmpIndex]].nDest_y = pDataSummary->t_enemy[nLeftMinDistIndex].y;
//
//		//赋值到共享数据以便显示
//		pDataSummary->arObstIndexBackDef[nTmpIndex] = nLeftMinDistIndex;
//
//		nTmpIndex++;
//
//	}
//	//2）右后卫
//	if (nRightMinDistIndex >= 0)
//	{
//		//判断其是否带球
//		nTempDist = m_CalDist(
//			pDataSummary->t_enemy[nRightMinDistIndex].x, pDataSummary->t_enemy[nRightMinDistIndex].y,
//			pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y
//			);
//		if (nTempDist > 50)
//		{
//			//球离他远，防传球
//			pRole[nRoleDefIndex[nTmpIndex]].ctrl = CTRL_DEF_BALL;
//			pRole[nRoleDefIndex[nTmpIndex]].def_dist = DIST_TAC_DEFBALL;
//		}
//		else
//		{
//			//球离他近，防射门
//			pRole[nRoleDefIndex[nTmpIndex]].ctrl = CTRL_DEF_GOAL;
//			pRole[nRoleDefIndex[nTmpIndex]].def_dist = DIST_TAC_DEFGOAL;
//		}
//		pRole[nRoleDefIndex[nTmpIndex]].nDest_x = pDataSummary->t_enemy[nRightMinDistIndex].x;
//		pRole[nRoleDefIndex[nTmpIndex]].nDest_y = pDataSummary->t_enemy[nRightMinDistIndex].y;
//
//		//赋值到共享数据以便显示
//		pDataSummary->arObstIndexBackDef[nTmpIndex] = nRightMinDistIndex;
//
//		nTmpIndex++;
//	}
//	//3）中卫
//	if (nMidMinDistIndex >= 0)
//	{
//		//判断其是否带球
//		nTempDist = m_CalDist(
//			pDataSummary->t_enemy[nMidMinDistIndex].x, pDataSummary->t_enemy[nMidMinDistIndex].y,
//			pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y
//			);
//		if (nTempDist > 50)
//		{
//			//球离他远，防传球
//			pRole[nRoleDefIndex[nTmpIndex]].ctrl = CTRL_DEF_BALL;
//			pRole[nRoleDefIndex[nTmpIndex]].def_dist = DIST_TAC_DEFBALL;
//		}
//		else
//		{
//			//球离他近，防射门
//			pRole[nRoleDefIndex[nTmpIndex]].ctrl = CTRL_DEF_GOAL;
//			pRole[nRoleDefIndex[nTmpIndex]].def_dist = DIST_TAC_DEFGOAL;
//		}
//		pRole[nRoleDefIndex[nTmpIndex]].nDest_x = pDataSummary->t_enemy[nMidMinDistIndex].x;
//		pRole[nRoleDefIndex[nTmpIndex]].nDest_y = pDataSummary->t_enemy[nMidMinDistIndex].y;
//
//		//赋值到共享数据以便显示
//		pDataSummary->arObstIndexBackDef[nTmpIndex] = nMidMinDistIndex;
//
//		nTmpIndex++;
//	}
//
//	//赋值到共享数据以便显示
//	pDataSummary->nNumOfObstBackDef = nTmpIndex;
//
//
//	//防守队员都分配完毕，下面该为没有防守任务的队员分配跑位任务
//	CPoint posDefMove;
//	//1)左后卫跑位
//	if (nLeftMinDistIndex < 0)
//	{
//		//没有防守任务，跑位
//		posDefMove = m_Fix_KeepAwayFrom(300, 900, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, 150, 0,
// 100); 		pRole[nRoleDefIndex[nTmpIndex]].ctrl = CTRL_MOVETO; 		pRole[nRoleDefIndex[nTmpIndex]].nDest_x =
// posDefMove.x; 		pRole[nRoleDefIndex[nTmpIndex]].nDest_y = posDefMove.y;
// pRole[nRoleDefIndex[nTmpIndex]].nDest_angle =
// m_Loca_CalAngle(posDefMove.x, posDefMove.y, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);
// nTmpIndex++;
//	}
//	//2)右后卫跑位
//	if (nRightMinDistIndex < 0)
//	{
//		//没有防守任务，跑位
//		posDefMove = m_Fix_KeepAwayFrom(300, 300, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, 150, 0,
// 100); 		pRole[nRoleDefIndex[nTmpIndex]].ctrl = CTRL_MOVETO; 		pRole[nRoleDefIndex[nTmpIndex]].nDest_x =
// posDefMove.x; 		pRole[nRoleDefIndex[nTmpIndex]].nDest_y = posDefMove.y;
// pRole[nRoleDefIndex[nTmpIndex]].nDest_angle =
// m_Loca_CalAngle(posDefMove.x, posDefMove.y, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);
// nTmpIndex++;
//	}
//	//1)左后卫跑位
//	if (nMidMinDistIndex < 0)
//	{
//		//没有防守任务，跑位
//		posDefMove = m_Fix_KeepAwayFrom(800, 600, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, 150, 0,
// 100); 		pRole[nRoleDefIndex[nTmpIndex]].ctrl = CTRL_MOVETO; 		pRole[nRoleDefIndex[nTmpIndex]].nDest_x =
// posDefMove.x; 		pRole[nRoleDefIndex[nTmpIndex]].nDest_y = posDefMove.y;
// pRole[nRoleDefIndex[nTmpIndex]].nDest_angle =
// m_Loca_CalAngle(posDefMove.x, posDefMove.y, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);
// nTmpIndex++;
//	}
// }

/************************************************************************/
/*         未看见球的状况                                               */
/************************************************************************/
void CTac_Playing_AreaDef::SubTac_BallNoSeen(uRole *pRole)
{
    // 【未看见球的状况】
    // 优先对球消失附件的障碍物进行防射门。
    // 核对离球消失点最近的障碍物是否是刚才防守的离球门最近的障碍物，
    // 如果是，则对离球门第二近的障碍物防射门。若不是，则，再对其进行防射门。

    // 传球点为实时计算的最佳接球点
    pDataSummary->posCatch = pCatchPosition->posCatch[0];

    // 策略优先：1、守门；2、对离球门最近的障碍物防射门(或者切成进攻)；3、对离球门第二近的障碍物防射门(或者退回后卫)；4、去往球消失点；5、留在中场
    pRole[0].ctrl = CTRL_GOALKEEP;

    // 根据对方障碍物来确定防守策略
    int nNumOfEnemy = pDataSummary->NumOfemy;

    // 2、保持进攻
    pRole[1].ctrl = CTRL_ATTACK;

    // 3~5队员进行防守
    AreaDefRoles(pRole, 2, 3, 4);
}

/************************************************************************/
/* 对方持球而我方未持球，球在对方控制下                                 */
/************************************************************************/
void CTac_Playing_AreaDef::SubTacOppoCtrlBall(uRole *pRole)
{

    // 传球点为实时计算的最佳接球点
    pDataSummary->posCatch = pCatchPosition->posCatch[0];

    // 1-守门；
    // 2-传球（或者进攻）；3-防射门(或者驻留中场)；4-对离我球门第一近障碍物防传球（障碍物不能离球太近）；5-对离我球门第二近障碍物防传球（障碍物不能离球太近）；
    pRole[0].ctrl = CTRL_GOALKEEP;
    if (pDataSummary->cGlobalBall.x < 900)
    {
        // 球在己方半场，对方控球
        pRole[1].ctrl = CTRL_PASS_MOVE;
        pRole[1].nDest_x = pDataSummary->posCatch.x;
        pRole[1].nDest_y = pDataSummary->posCatch.y;

        pRole[4].ctrl = CTRL_CATCH_MOVE;
        pRole[4].nDest_x = pDataSummary->posCatch.x;
        pRole[4].nDest_y = pDataSummary->posCatch.y;

        // 其他队员进行防守
        AreaDefRoles(pRole, 2, 3, 5);
    }
    else
    {
        // 球在对方半场，对方控球
        pRole[1].ctrl = CTRL_ATTACK;

        // 3~5队员进行防守
        AreaDefRoles(pRole, 2, 3, 4);
    }

    // 传球测试
#ifdef TEST_PASSING
    strTacticInfo = "测试-对方持球";
    if (pDataSummary->cGlobalBall.x < 900)
    {
        pRole[1].ctrl = CTRL_PASS_MOVE;
        pRole[1].nDest_x = pDataSummary->posCatch.x;
        pRole[1].nDest_y = pDataSummary->posCatch.y;

        pRole[2].ctrl = CTRL_CATCH_MOVE;
        pRole[2].nDest_x = pDataSummary->posCatch.x;
        pRole[2].nDest_y = pDataSummary->posCatch.y;

        pRole[3].ctrl = CTRL_STOP;
        pRole[4].ctrl = CTRL_STOP;
    }
    else
    {
        pRole[1].ctrl = CTRL_ATTACK;
        pRole[2].ctrl = CTRL_STOP;
        pRole[3].ctrl = CTRL_STOP;
        pRole[4].ctrl = CTRL_STOP;
    }
#endif // TEST_PASSING
}

/************************************************************************/
/*             自由滚动我方未带球,有可能是对方传球                      */
/************************************************************************/
void CTac_Playing_AreaDef::SubTacOppoPassing(uRole *pRole)
{

    // 传球点为实时计算的最佳接球点
    pDataSummary->posCatch = pCatchPosition->posCatch[0];

    // 1-守门； 2-传球（或者进攻）；3-防守；4-接球跑位；5-防守
    pRole[0].ctrl = CTRL_GOALKEEP;

    if (pDataSummary->cGlobalBall.x < 900)
    {
        pRole[1].ctrl = CTRL_PASS_MOVE;
        pRole[1].nDest_x = pDataSummary->posCatch.x;
        pRole[1].nDest_y = pDataSummary->posCatch.y;
    }
    else
    {
        pRole[1].ctrl = CTRL_ATTACK;
    }

    if (pDataSummary->cGlobalBall.x < 900)
    {
        // 球在己方半场，接球跑位
        pRole[3].ctrl = CTRL_CATCH_MOVE;
        pRole[3].nDest_x = pDataSummary->posCatch.x;
        pRole[3].nDest_y = pDataSummary->posCatch.y;
        pRole[3].nDest_angle = m_Loca_CalAngle(pRole[3].nDest_x, pRole[3].nDest_y, pDataSummary->cGlobalBall.x,
                                               pDataSummary->cGlobalBall.y);
        AreaDefRoles(pRole, 2, 4, 5);
    }
    else
    {
        // 球在对方半场，四号角色也参与到防守
        AreaDefRoles(pRole, 2, 3, 4);
    }

    // 传球测试
#ifdef TEST_PASSING
    strTacticInfo = "测试-自由滚动";
    if (pDataSummary->cGlobalBall.x < 900)
    {
        pRole[1].ctrl = CTRL_PASS_MOVE;
        pRole[1].nDest_x = pDataSummary->posCatch.x;
        pRole[1].nDest_y = pDataSummary->posCatch.y;

        pRole[2].ctrl = CTRL_CATCH_MOVE;
        pRole[2].nDest_x = pDataSummary->posCatch.x;
        pRole[2].nDest_y = pDataSummary->posCatch.y;

        pRole[3].ctrl = CTRL_STOP;
        pRole[4].ctrl = CTRL_STOP;
    }
    else
    {
        pRole[1].ctrl = CTRL_ATTACK;
        pRole[2].ctrl = CTRL_STOP;
        pRole[3].ctrl = CTRL_STOP;
        pRole[4].ctrl = CTRL_STOP;
    }
#endif // TEST_PASSING
}

/************************************************************************/
/*         【触球】准备传球阶段                                      */
/************************************************************************/
void CTac_Playing_AreaDef::SubTacToPass(uRole *pRole)
{
    // strTacticInfo = "SubTacToPass";
    strTacticInfo.Format("ToPass(%d,%d)", pCatchPosition->posCatch[0].x, pCatchPosition->posCatch[0].y);
    PrintInfo(strTacticInfo);
    // 传球点为实时计算的最佳接球点
    pDataSummary->posCatch = pCatchPosition->posCatch[0];

    // 1-守门；2-传球跑位；3-接球跑位；4和5进行防守
    pRole[0].ctrl = CTRL_GOALKEEP;

    pRole[1].ctrl = CTRL_PASS_MOVE;
    pRole[1].nDest_x = pDataSummary->posCatch.x;
    pRole[1].nDest_y = pDataSummary->posCatch.y;

    pRole[4].ctrl = CTRL_CATCH_MOVE;
    pRole[4].nDest_x = pDataSummary->posCatch.x;
    pRole[4].nDest_y = pDataSummary->posCatch.y;

    AreaDefRoles(pRole, 2, 3, 5); //(5是不会被执行到的)

    // 传球测试
#ifdef TEST_PASSING
    strTacticInfo = "测试-准备传球";
    if (pDataSummary->cGlobalBall.x < 900)
    {
        pRole[1].ctrl = CTRL_PASS_MOVE;
        pRole[1].nDest_x = pDataSummary->posCatch.x;
        pRole[1].nDest_y = pDataSummary->posCatch.y;

        pRole[2].ctrl = CTRL_CATCH_MOVE;
        pRole[2].nDest_x = pDataSummary->posCatch.x;
        pRole[2].nDest_y = pDataSummary->posCatch.y;

        pRole[3].ctrl = CTRL_STOP;
        pRole[4].ctrl = CTRL_STOP;
    }
    else
    {
        pRole[1].ctrl = CTRL_ATTACK;
        pRole[2].ctrl = CTRL_STOP;
        pRole[3].ctrl = CTRL_STOP;
        pRole[4].ctrl = CTRL_STOP;
    }
#endif // TEST_PASSING
}

/************************************************************************/
/*         【1-1-2-2-1】自由滚动,我方传球                               */
/************************************************************************/
void CTac_Playing_AreaDef::SubTacPassing(uRole *pRole)
{
    // strTacticInfo = "Passing";
    pDataSummary->posCatch = posSelfCatch;

    // 这里对pDataSummary->posCatch的赋值是从传球队友发回来的数据
    // int indexPass = GetAgentIndex(CTRL_PASS_MOVE);
    /*if (indexPass > 0 && indexPass < AG_NUM)
    {
        pDataSummary->posCatch.x = pDataSummary->agent[indexPass].dest_x;
        pDataSummary->posCatch.y = pDataSummary->agent[indexPass].dest_y;
    }*/

    // 1-守门；2-接球ing；3-传球跑位；4和5防守
    pRole[0].ctrl = CTRL_GOALKEEP;

    pRole[1].ctrl = CTRL_CATCH_FOCUS;
    pRole[1].nDest_x = pDataSummary->posCatch.x;
    pRole[1].nDest_y = pDataSummary->posCatch.y;

    AreaDefRoles(pRole, 2, 3, 4);

    // 传球测试
#ifdef TEST_PASSING
    strTacticInfo.Format("Passing(%d,%d)", pDataSummary->posCatch.x, pDataSummary->posCatch.y);
    PrintInfo(strTacticInfo);
    strTacticInfo = "测试-传球中";
    pRole[1].ctrl = CTRL_CATCH_FOCUS;
    pRole[1].nDest_x = pDataSummary->posCatch.x;
    pRole[1].nDest_y = pDataSummary->posCatch.y;
    pRole[2].ctrl = CTRL_STOP;
    pRole[3].ctrl = CTRL_STOP;
    pRole[4].ctrl = CTRL_STOP;

#endif // TEST_PASSING
}

/************************************************************************/
/*         【触球】直接射门阶段                                         */
/************************************************************************/
void CTac_Playing_AreaDef::SubTacGoal(uRole *pRole)
{
    // 传球点为实时计算的最佳接球点
    pDataSummary->posCatch = pCatchPosition->posCatch[0];

    // 1-守门；2-进攻；3-接球跑位（吸引对手防御）；4-对离球门最近的障碍物进行防传球；5-对球门第二近的队员进行防传球
    pRole[0].ctrl = CTRL_GOALKEEP;

    pRole[1].ctrl = CTRL_ATTACK;

    AreaDefRoles(pRole, 2, 3, 4);

    // 传球测试
#ifdef TEST_PASSING
    strTacticInfo = "测试-直接射门";
    pRole[1].ctrl = CTRL_ATTACK;
    pRole[2].ctrl = CTRL_STOP;
    pRole[3].ctrl = CTRL_STOP;
    pRole[4].ctrl = CTRL_STOP;

#endif // TEST_PASSING
}

/************************************************************************/
/*          CTac_Playing决策主逻辑                                      */
/************************************************************************/
void CTac_Playing_AreaDef::PlanTac(stTactics *outTactics)
{
    uRole *pRole = outTactics->role;
    strTacticInfo = "比赛进行中";

    // 接球位置计算
    pCatchPosition->CalCatchPosition();

    // 【传接球】传接球的信号处理
    if (pDataSummary->bPassKick == true)
    {
        pDataSummary->bPassing = true;
        posSelfCatch.x = pDataSummary->posPasserKickTo_x;
        posSelfCatch.y = pDataSummary->posPasserKickTo_y;

        strTacticInfo.Format("bPassKick(%d,%d)", posSelfCatch.x, posSelfCatch.y);
        PrintInfo(strTacticInfo);
    }
    if (pDataSummary->bPassing == true)
    {
        // 传球状态终止条件一：传球超时
        UINT nCurTimeSec = GetTickCount() / 1000;
        pDataSummary->nPassKickTimeCountSec = nCurTimeSec - pDataSummary->nPassKickTime;
        if (pDataSummary->nPassKickTimeCountSec > TIMEOUT_PASSING)
        {
            PrintInfo("传球状态终止条件一");
            pDataSummary->bPassing = false;
        }
        // 传球状态终止条件二：接球队员已经拿到球
        if (pDataSummary->nCatchID > 0 && pDataSummary->nCatchID <= 5)
        {
            if (pDataSummary->agent[pDataSummary->nCatchID - 1].x ==
                    pDataSummary->agent[pDataSummary->nCatchID - 1].ball.x &&
                pDataSummary->agent[pDataSummary->nCatchID - 1].y ==
                    pDataSummary->agent[pDataSummary->nCatchID - 1].ball.y)
            {
                PrintInfo("传球状态终止条件二");
                pDataSummary->bPassing = false;
            }
        }
        // 传球状态终止条件三：接球队员接球后切换成进攻
        if (pDataSummary->nCatchID > 0 && pDataSummary->nCatchID <= 5)
        {
            uAgent *pAgent = pDataSummary->agent;
            int nCatchID = pDataSummary->nCatchID;
            if (
                /*m_CalDist(pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, pAgent[nCatchID - 1].x,
                   pAgent[nCatchID - 1].y) < 50 &&*/
                (pDataSummary->agent[pDataSummary->nCatchID - 1].status == CTRL_ATTACK ||
                 pDataSummary->agent[pDataSummary->nCatchID - 1].status == CTRL_PASS_MOVE))
            {
                PrintInfo("传球状态终止条件三");
                pDataSummary->bPassing = false;
            }
        }
    }

    // 【1】按是否看见球来做处理
    if (pDataSummary->bNoOneSeeBall == false)
    {
        // 【1-1】看见球，按我方是否正在触球来进行下一分支
        if (pDataSummary->bHoldBall == true)
        {
            // 【1-1-1】我方持球，根据球位置进行下一级分支
            if (pDataSummary->cGlobalBall.x > 900)
            {
                // 【1-1-1-1】我方持球，球在对方半场，根据触球点坐标来进行下一级分支
                if (pDataSummary->posTouchBall.x > 900)
                {
                    // 【1-1-1-1-1】我方持球，球在对方半场，触球点在对方半场，根据我方队员是否有将球带回过己方半场来进行下一分支
                    if (pDataSummary->bCtrlBallBackHalf == false)
                    {
                        // 【1-1-1-1-1-1】我方持球，球在对方半场，触球点在对方半场，我方队员未将球带回己方半场，直接射门吧
                        strTacticInfo += "_【1-1-1-1-1-1】我持，球对，触对，未回，射门";
                        // PrintInfo(strTacticInfo);
                        SubTacGoal(outTactics->role);
                    }
                    else
                    {
                        // 【1-1-1-1-1-2】我方持球，球在对方半场，触球点在对方半场，我方队员曾将球带回过己方半场,老老实实走传球流程吧
                        strTacticInfo += "_【1-1-1-1-1-2】我持，球对，触对，回了，传球";
                        // PrintInfo(strTacticInfo);
                        SubTacToPass(outTactics->role);
                    }
                }
                else
                {
                    // 【1-1-1-1-2】我方持球，球在对方半场，触球点在己方半场，根据对方在对方半场是否有过触球经历来进行下一分支
                    if (pDataSummary->bOppoTouchedBall == true && pDataSummary->posOppoTouchBall.x > 900)
                    {
                        // 【1-1-1-1-2-1】我方持球，球在对方半场，触球点在己方半场，对方在对方半场触过球，可以直接射门
                        strTacticInfo += "_【1-1-1-1-2-1】我持，球对，触己，对触对，射门";
                        // PrintInfo(strTacticInfo);
                        SubTacGoal(outTactics->role);
                    }
                    else
                    {
                        // 【1-1-1-1-2-2】我方持球，球在对方半场，触球点在己方半场，对方未在对方半场触过球，老老实实传球吧
                        strTacticInfo += "_【1-1-1-1-2-2】我持，球对，触己，对未触对，传球";
                        // PrintInfo(strTacticInfo);
                        SubTacToPass(outTactics->role);
                    }
                }
            }
            else
            {
                // 【1-1-1-2】我方持球，球在我方半场，老老实实准备传球吧
                strTacticInfo += "_【1-2-1】球在己方半场，我方持球";
                // PrintInfo(strTacticInfo);
                SubTacToPass(outTactics->role);
                if (pDataSummary->posTouchBall.x > 900)
                {
                    // 触球点在对方半场，我方队员竟持球到了己方半场，说明是从对方半场带回来的
                    pDataSummary->bCtrlBallBackHalf = true;
                }
            }
        }
        else
        {
            // 【1-1-2】我方未持球，按照对方是否在带球来进行下一分支
            pDataSummary->bCtrlBallBackHalf = false; // 我方已失去控球权，清零
            if (m_bOppoHandlingBall() == true)
            {
                // 【1-1-2-1】对方持球而我方未持球，球在对方控制下
                strTacticInfo += "【1-1-2-1】对方控球";
                // PrintInfo(strTacticInfo);
                SubTacOppoCtrlBall(outTactics->role);
            }
            else
            {
                // 【1-1-2-2】对方未控球，此时球在自由滚动中，根据是否为我方传球来进行分支
                if (pDataSummary->bPassing == true)
                {
                    // 【1-1-2-2-1】球在自由滚动中,根据判断是我方传出的球
                    strTacticInfo += "【1-1-2-2-1】自由滚动,我方传球";
                    // PrintInfo(strTacticInfo);
                    SubTacPassing(outTactics->role);
                }
                else
                {
                    // 【1-1-2-2-2】球在自由滚动中,根据判断不是我方传出的球，有可能是对方的传球
                    strTacticInfo += "【1-1-2-2-2】自由滚动,有可能对方传球";
                    // PrintInfo(strTacticInfo);
                    SubTacOppoPassing(outTactics->role);
                }
            }
        }
    }
    else
    {
        // 【1-2】未看见球
        strTacticInfo += "【1-2】未看见球";
        SubTac_BallNoSeen(outTactics->role);
    }
}
