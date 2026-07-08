#include "stdafx.h"
#include "Tac_Playing.h"

CTac_Playing::CTac_Playing()
{
    Reset();
}

CTac_Playing::~CTac_Playing()
{
}

void CTac_Playing::Reset()
{
    if (pDataSummary != NULL)
    {
        pDataSummary->bPassing = false;
    }
}

bool CTac_Playing::m_bOppoHandlingBall()
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

/************************************************************************/
/*         未看见球的状况                                               */
/************************************************************************/
void CTac_Playing::SubTac_BallNoSeen(uRole *pRole)
{
    // 【未看见球的状况】

    // 传球点为实时计算的最佳接球点
    pDataSummary->posCatch = pCatchPosition->posCatch[0];
    // 2015策略：1、守门；2、角度防守（球在己方半场）或者接球跑位（球在对方半场）；3、寻球；4、防守跑位1；5、防守跑位2
    pRole[0].ctrl = CTRL_GOALKEEP;

    pRole[1].ctrl = CTRL_CATCH_MOVE;
    pRole[1].nDest_x = 300;
    pRole[1].nDest_y = 600;
    // 	if (pDataSummary->cGlobalBall.x > 900)
    // 	{
    // 		pRole[1].ctrl = CTRL_DEFENCE;
    // 		pRole[1].def_angle = 0;
    // 		pRole[1].def_dist = DIST_TAC_DEFGOAL;
    // 	}
    // 	else
    // 	{
    // 		pRole[1].ctrl = CTRL_CATCH_MOVE;
    // 		pRole[1].nDest_x = pDataSummary->posCatch.x;
    // 		pRole[1].nDest_y = pDataSummary->posCatch.y;
    // 	}

    pRole[2].ctrl = CTRL_SEARCHBALL;

    AreaDefRoles(pRole, 3, 4, 5);

    ////////////////////////////////////////////////////////////////////
    // 2014年老策略
    // 优先对球消失附件的障碍物进行防射门。
    // 核对离球消失点最近的障碍物是否是刚才防守的离球门最近的障碍物，
    // 如果是，则对离球门第二近的障碍物防射门。若不是，则，再对其进行防射门。
    // 策略优先：1、守门；2、对离球门最近的障碍物防射门(或者切成进攻)；3、对离球门第二近的障碍物防射门(或者退回后卫)；4、去往球消失点；5、留在中场
    /*pRole[0].ctrl = CTRL_GOALKEEP;

    //根据对方障碍物来确定防守策略
    int nNumOfEnemy = pDataSummary->NumOfemy;

    //2、根据球上一次保存的位置保持进攻或者传球
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

    //3~4防守
    AreaDefRoles(pRole, 2, 3, 4);*/

    // 5、去到接球位置
    /*pRole[4].ctrl = CTRL_CATCH_MOVE;
    pRole[4].nDest_x = pDataSummary->posCatch.x;;
    pRole[4].nDest_y = pDataSummary->posCatch.y;
    pRole[4].nDest_angle = 180;*/
    ///////////////////////////////////////////////////////////////////////
}

/************************************************************************/
/* 对方持球而我方未持球，球在对方控制下                                 */
/************************************************************************/
void CTac_Playing::SubTacOppoCtrlBall(uRole *pRole)
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

        pRole[2].ctrl = CTRL_CATCH_MOVE;
        pRole[2].nDest_x = pDataSummary->posCatch.x;
        pRole[2].nDest_y = pDataSummary->posCatch.y;

        // 其他队员进行防守
        AreaDefRoles(pRole, 3, 4, 5);
    }
    else
    {
        // 球在对方半场，对方控球
        pRole[1].ctrl = CTRL_ATTACK;

        // 3~5队员进行防守
        AreaDefRoles(pRole, 2, 3, 4);
    }
}

/************************************************************************/
/*             自由滚动我方未带球,有可能是对方传球                      */
/************************************************************************/
void CTac_Playing::SubTacOppoPassing(uRole *pRole)
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
        AreaDefRoles(pRole, 2, 4, 5); //(5是不会被执行到的)
    }
    else
    {
        // 球在对方半场，四号角色也参与到防守
        AreaDefRoles(pRole, 2, 3, 4);
    }
}

/************************************************************************/
/*         【触球】准备传球阶段                                      */
/************************************************************************/
void CTac_Playing::SubTacToPass(uRole *pRole)
{
    // strTacticInfo = "SubTacToPass";
    strTacticInfo.Format("ToPass(%d,%d)", pCatchPosition->posCatch[0].x, pCatchPosition->posCatch[0].y);
    PrintInfo(strTacticInfo);
    // 传球点为实时计算的最佳接球点
    pDataSummary->posCatch = pCatchPosition->posCatch[0];

    // 1-守门；2-传球跑位；3-1角度防守；4和5进行防守
    pRole[0].ctrl = CTRL_GOALKEEP;

    pRole[1].ctrl = CTRL_PASS_MOVE;
    pRole[1].nDest_x = pDataSummary->posCatch.x;
    pRole[1].nDest_y = pDataSummary->posCatch.y;

    pRole[2].ctrl = CTRL_DEFENCE;
    pRole[2].nDest_angle = 1;
    pRole[2].def_dist = DIST_TAC_DEFGOAL;

    AreaDefRoles(pRole, 3, 4, 5); //(5是不会被执行到的)
}

/************************************************************************/
/*         【1-1-2-2-1】自由滚动,我方传球                               */
/************************************************************************/
void CTac_Playing::SubTacPassing(uRole *pRole)
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

    // 1-守门；2-接球ing；3-追球；4和5防守
    pRole[0].ctrl = CTRL_GOALKEEP;

    pRole[1].ctrl = CTRL_CATCH_FOCUS;
    pRole[1].nDest_x = pDataSummary->posCatch.x;
    pRole[1].nDest_y = pDataSummary->posCatch.y;

    pRole[2].ctrl = CTRL_MOVETO;
    pRole[2].nDest_x = pDataSummary->cGlobalBall.x;
    pRole[2].nDest_y = pDataSummary->cGlobalBall.y;

    AreaDefRoles(pRole, 3, 4, 5); //(5是不会被执行到的)
}

/************************************************************************/
/*         【触球】直接射门阶段                                         */
/************************************************************************/
void CTac_Playing::SubTacGoal(uRole *pRole)
{
    // 传球点为实时计算的最佳接球点
    pDataSummary->posCatch = pCatchPosition->posCatch[0];

    // 1-守门；2-进攻；3-接球跑位（吸引对手防御）；4-对离球门最近的障碍物进行防传球；5-对球门第二近的队员进行防传球
    pRole[0].ctrl = CTRL_GOALKEEP;

    pRole[1].ctrl = CTRL_ATTACK;

    pRole[2].ctrl = CTRL_CATCH_MOVE;
    pRole[2].nDest_x = pDataSummary->posCatch.x;
    pRole[2].nDest_y = pDataSummary->posCatch.y;

    AreaDefRoles(pRole, 3, 4, 5); //(5是不会被执行到的)
}

/************************************************************************/
/*          CTac_Playing决策主逻辑                                      */
/************************************************************************/
void CTac_Playing::PlanTac(stTactics *outTactics)
{
    uRole *pRole = outTactics->role;
    strTacticInfo = "比赛进行中";

    // 计算需要防守的障碍物列表
    // Get3BackObstToDef();
    Get3ObstCloseToDef();

    // 【1-2】未传球，按照是否看见球来进行分支
    if (pDataSummary->bNoOneSeeBall == true)
    {
        // 【1-2-1】看不到球
        strTacticInfo += "【1-2-1】看不到球！";
        SubTac_BallNoSeen(outTactics->role);
    }
    else
    {
        // 【1-2-2】能看见球，根据所处的半场来决策
        if (pDataSummary->cGlobalBall.x < 900)
        {
            // 【1-2-2-1】球在己方半场，需要贴近防守
            SubTacDef(pRole);
        }
        else
        {
            // 【1-2-2-2】球在对方半场，进攻模式
            SubTacAttack(pRole);
        }
    }
}

// 后场防守的角色分配(虽然参数是分左中右，但是实际分配角色的时候是不分的)
void CTac_Playing::AreaDefRoles(uRole *pRole, int inLeftBack, int inRightBack, int inMidBack)
{
    int nTempDist = 0;

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
    bool bGoalDefed = false; // 用来标记是否已经有人在防射门（多个角色防射门会导致撞到一起）
    for (i = 0; i < nNumRoleDef; i++)
    {
        int nEnemy_x = pDataSummary->t_enemy[arEnemyIndexToDef[i]].x;
        int nEnemy_y = pDataSummary->t_enemy[arEnemyIndexToDef[i]].y;
        nTempDist =
            m_CalDist(pDataSummary->t_enemy[arEnemyIndexToDef[i]].x, pDataSummary->t_enemy[arEnemyIndexToDef[i]].y,
                      pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);
        if (nTempDist > DIST_TAC_DEFBALL + 100)
        {
            // 能看见球时防传球，看不见球时统一防射门
            if (pDataSummary->bNoOneSeeBall == false)
            {
                // 看得见球，球离他远，防传球
                pRole[nRoleDefIndex[i]].ctrl = CTRL_DEF_BALL;
                pRole[nRoleDefIndex[i]].def_dist = DIST_TAC_DEFBALL;
            }
            else
            {
                // 看不见球，防射门
                pRole[nRoleDefIndex[i]].ctrl = CTRL_DEF_GOAL;
                pRole[nRoleDefIndex[i]].def_dist = DIST_TAC_DEFGOAL;
            }
            pRole[nRoleDefIndex[i]].nDest_x = pDataSummary->t_enemy[arEnemyIndexToDef[i]].x;
            pRole[nRoleDefIndex[i]].nDest_y = pDataSummary->t_enemy[arEnemyIndexToDef[i]].y;
        }
        else
        {
            if (bGoalDefed == false)
            {
                // 球离他近，防射门
                if (pDataSummary->bNoOneSeeBall)
                {
                    // 若看不到球，则用防射门防障碍物
                    pRole[nRoleDefIndex[i]].ctrl = CTRL_DEF_GOAL;
                    pRole[nRoleDefIndex[i]].def_dist = DIST_TAC_DEFGOAL;
                    pRole[nRoleDefIndex[i]].nDest_x = pDataSummary->cGlobalBall.x;
                    pRole[nRoleDefIndex[i]].nDest_y = pDataSummary->cGlobalBall.y;
                }
                else
                {
                    // 能看见球的时候，还是使用防守角色，这样
                    pRole[nRoleDefIndex[i]].ctrl = CTRL_DEFENCE;
                    pRole[nRoleDefIndex[i]].def_angle = 1;
                    pRole[nRoleDefIndex[i]].def_dist = DIST_TAC_DEFGOAL;
                }
                bGoalDefed = true;
            }
            else
            {
                // 已经有人在防射门了，不需要他，将他换到防守任务尾部，填充到
                nNumRoleDef--;

                int nTmp = nRoleDefIndex[i];
                nRoleDefIndex[i] = nRoleDefIndex[nNumRoleDef];
                nRoleDefIndex[nNumRoleDef] = nTmp;
            }
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
                m_Fix_KeepAwayFrom(400, 600, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, 150, 0, 100);
            pRole[nRoleDefIndex[nTmpIndex]].ctrl = CTRL_MOVETO;
            pRole[nRoleDefIndex[nTmpIndex]].nDest_x = posDefMove.x;
            pRole[nRoleDefIndex[nTmpIndex]].nDest_y = posDefMove.y;
            pRole[nRoleDefIndex[nTmpIndex]].nDest_angle =
                m_Loca_CalAngle(posDefMove.x, posDefMove.y, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);
            nTmpIndex++;
        }
    }
}

void CTac_Playing::SubTacAttack(uRole *pRole)
{
    // 1-守门；2-进攻；3-定点站位
    pRole[0].ctrl = CTRL_GOALKEEP;

    pRole[1].ctrl = CTRL_ATTACK;

    pRole[2].ctrl = CTRL_MOVETO;
    pRole[2].nDest_x = 300;
    pRole[2].nDest_y = 600;
}

void CTac_Playing::SubTacDef(uRole *pRole)
{
    // 1-守门；2-传球跑位；3-1角度防守
    pRole[0].ctrl = CTRL_GOALKEEP;

    pRole[1].ctrl = CTRL_ATTACK;

    pRole[2].ctrl = CTRL_DEFENCE;
    pRole[2].nDest_angle = 1;
    pRole[2].def_dist = DIST_TAC_DEFGOAL;
}
