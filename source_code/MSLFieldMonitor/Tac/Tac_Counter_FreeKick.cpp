/*
敌方自由球应对策略
*/
#include "stdafx.h"
#include "Tac_Counter_FreeKick.h"

CTac_Counter_FreeKick::CTac_Counter_FreeKick() // ar0bstDef 需要防守的障碍物列表 m_n0bstDef 需要防守的障碍物数量
{
    arObstDef = new obstacle[AG_NUM];
    m_nObstDef = 0;
    Reset(); // 重置
}

CTac_Counter_FreeKick::~CTac_Counter_FreeKick()
{
}

void CTac_Counter_FreeKick::Reset()
{
    m_nTacStep = TAC_STEP_READY; // 1 开球准备
    m_nGoStep = GO_STEP_READY;   // 11 还在继续跑位
    bCounterCornerKick = false;  // 猜测指定为是否为角球

    // 下面初始化一些球是否在我方半场的回滞死区的参数（暂不懂回滞死区指的是啥）
    nInOwnX = 900;
    nInOppoX = 950;
    resInOwnField = true;

    // 下面初始化一些球是否在左右半场的回滞死区的参数
    nInLeft = 700;
    nInRight = 500;
    resInLeftField = true;
}

/*
判断球是不是在我方的回滞死区的具体实现
*/
bool CTac_Counter_FreeKick::isInOwnField(int inX)
{
    if (inX < nInOwnX) // 如果在，返回true
    {
        resInOwnField = true;
    }

    if (inX > nInOppoX) // 如果在敌方半场的950以后（横向看总的是1800），则放过来认为不在我方回滞死区
    {
        resInOwnField = false;
    }

    /*
    if(inX <= nInOwnX)
    {
        resInOwnField = true;
    }else
    {
        resInOwnField = false;
    }
    */
    return resInOwnField;
}

/*
判断在不在左右的回滞死区
*/
bool CTac_Counter_FreeKick::isInLeftField(int inY)
{
    if (inY < nInRight)
    {
        resInLeftField = false;
    }

    if (inY > nInLeft)
    {
        resInLeftField = true;
    }
    return resInLeftField;
}

/*
入口函数
*/
void CTac_Counter_FreeKick::PlanTac(stTactics *outTactics)
{
    pTactics = outTactics;
    switch (m_nTacStep)
    {
    case TAC_STEP_READY: // 1
        Step_Ready();
        break;
    case TAC_STEP_GO: // 2
        Step_Go();
        break;
    }
}

/************************************************************************/
/* 应对对方任意球准备跑位                                                         */
/************************************************************************/
void CTac_Counter_FreeKick::Step_Ready()
{
    nNumToOurPenaltyArea = 1; // 允许一台机器人在己方罚球区内,如果已经有队员在禁区里了，标记起来，避免后面队员再进禁区
                              // nNumToOurPenaltyArea这个参数定义在itactic中了
    // 应对对方任意球跑位
    uRole *pRole = pTactics->role;
    if (pDataSummary->bNoOneSeeBall == false) // 有任何一个设备看到球，进这个循环
    {
        strTacticInfo = "应对对方任意球跑位[正常]";
        pRole[0].ctrl = CTRL_GOALKEEP; // 2 守门员设置
        // 其他队员跑动到指定点（根据自己分配到的身份）
        for (int i = 1; i < 5; i++)
        {
            pRole[i].ctrl = CTRL_MOVETO; // 11
            pRole[i].nDest_angle = 0;
        }

        uBall tmpball = pDataSummary->cGlobalBall;

        // 球-警戒队员-己方球门 为一直线
        CPoint tmp = m_Loca_Ray(tmpball.x, tmpball.y, 150, 600,
                                (int)(DIST_TAC_COUNTER * pDataSummary->kDistance)); // 最后一个参数是距离
        // 球在中点时，计算结果会是该球员在中点本身，需要修正
        if (abs(tmpball.x - 900) < 100 && abs(tmpball.y - 600) < 100)
        {
            tmp.x = tmpball.x - (DIST_TAC_COUNTER * pDataSummary->kDistance);
            tmp.y = 400;
        }
        // 第一名队员如果退到我方球门或禁区里，需要予以修正（可利用禁区里待一名防守队员的特殊细则）（因为是第一名球员，所以单独处理）
        // m_Loc_DefInOurPenaltyArea();
        if (tmp.x <= 0 ||                               // 退到己方球门以后了
            (tmp.x < 250 && tmp.y > 270 && tmp.y < 930) // 进入禁区了
        )
        {
            tmp = m_Loc_DefInOurPenaltyArea();
            nNumToOurPenaltyArea--;
        }
        // 第一名队员为球观察员
        pRole[1].nDest_x = tmp.x;
        pRole[1].nDest_y = tmp.y;
        pRole[1].nDest_angle = m_Loca_CalAngle(tmp.x, tmp.y, tmpball.x, tmpball.y); // 算角度朝向
        posFocusOnBall = tmp;

        //[!]其他防守距离参数
        int nDefDist = 150;
        int nKeepAwayFromBall = DIST_TAC_COUNTER; // 避开球的距离 该参数定义在MSL_Structs.h中了
        int nKeepAwayFromObst = 250;              // 避开对方队员的距离
        int nTempDist = 0;
        CPoint posDefMove;

        /////////////////////////////////////////////////////////
        // 方案一：计算我方半场范围内需要防守的障碍物
        // a)获取需要防守的障碍物列表
        // Get3BackObstToDef();
        Get3ObstCloseToDef(); // 该函数定义在了itactic中了
        int arEnemyIndexToDef[3];
        for (i = 0; i < 3; i++)
        {
            arEnemyIndexToDef[i] = pDataSummary->arObstIndexBackDef[i];
        }

        // b)计算需要参与到防守任务的人数
        if (pDataSummary->nNumOfObstBackDef > 3)
        {
            pDataSummary->nNumOfObstBackDef = 3;
            PrintInfo("后防需要防守的障碍物数量异常");
        }

        /////////////////////////////////////////////////////////////////////////////////////////////
        // 己方半场的对方任意球需要特殊处理，因为对方很可能就是直接传球射门（尤其荷兰队的来回传球射门）
        int nNumRoleDef;
        bool bInOwnField = isInOwnField(pDataSummary->cGlobalBall.x);
        if (bInOwnField == true)
        {
            int nRotateDegree = 10;
            if (isInLeftField(pDataSummary->cGlobalBall.y) == true)
            {
                nRotateDegree = -45;
            }
            else
            {
                nRotateDegree = 45;
            }
            // 从观察员角色旋转一个角度得到
            tmp = m_Loca_Ray(tmpball.x, tmpball.y, pRole[1].nDest_x, pRole[1].nDest_y, DIST_TAC_COUNTER);
            tmp = m_Loca_Rotate(tmp.x, tmp.y, tmpball.x, tmpball.y, nRotateDegree);
            tmp = FixSingleMove(2, tmp.x, tmp.y);
            posFocusOnBall = tmp;
            pRole[2].nDest_x = tmp.x;
            pRole[2].nDest_y = tmp.y;
            pRole[2].nDest_angle = m_Loca_CalAngle(tmp.x, tmp.y, tmpball.x, tmpball.y);
            nNumRoleDef = pDataSummary->nNumOfObstBackDef; // 需要参与到障碍物防守中的人数
        }
        else
        {
            nNumRoleDef = pDataSummary->nNumOfObstBackDef; // 需要参与到障碍物防守中的人数
        }

        // c)为障碍物防守角色分配任务(在障碍物和球之间，偏出一个角度)
        for (i = 0; i < nNumRoleDef; i++)
        {
            posDefMove =
                m_Loca_Ray(pDataSummary->t_enemy[arEnemyIndexToDef[i]].x, pDataSummary->t_enemy[arEnemyIndexToDef[i]].y,
                           pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, nDefDist);
            int defAngle;
            if (pDataSummary->t_enemy[arEnemyIndexToDef[i]].y > 600)
            {
                defAngle = 45;
            }
            else
            {
                defAngle = -45;
            }
            posDefMove = m_Loca_Rotate(posDefMove.x, posDefMove.y, pDataSummary->t_enemy[arEnemyIndexToDef[i]].x,
                                       pDataSummary->t_enemy[arEnemyIndexToDef[i]].y, defAngle);
            tmp = m_Fix_KeepAwayFrom(posDefMove.x, posDefMove.y,                               // 默认坐标
                                     pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, // 需要远离的坐标
                                     nKeepAwayFromBall,                                        // 与远离坐标保持的距离
                                     0,                                                        // 与队友保持距离
                                     50 // 与障碍物保持距离（防守队员这个相对比较小一点）
            );
            // 其他防守队员
            if (bInOwnField == true)
            {
                int nIndex = 3 + i;
                pRole[nIndex].nDest_x = tmp.x;
                pRole[nIndex].nDest_y = tmp.y;
                CPoint tmpPoint = FixSingleMove(nIndex, pRole[nIndex].nDest_x, pRole[nIndex].nDest_y);
                pRole[nIndex].nDest_x = tmpPoint.x;
                pRole[nIndex].nDest_y = tmpPoint.y;
                pRole[nIndex].nDest_angle = m_Loca_CalAngle(pRole[nIndex].nDest_x, pRole[nIndex].nDest_y,
                                                            pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);
            }
            else
            {
                int nIndex = 2 + i;
                pRole[nIndex].nDest_x = tmp.x;
                pRole[nIndex].nDest_y = tmp.y;
                CPoint tmpPoint = FixSingleMove(nIndex, pRole[nIndex].nDest_x, pRole[nIndex].nDest_y);
                pRole[nIndex].nDest_x = tmpPoint.x;
                pRole[nIndex].nDest_y = tmpPoint.y;
                pRole[nIndex].nDest_angle = m_Loca_CalAngle(pRole[nIndex].nDest_x, pRole[nIndex].nDest_y,
                                                            pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y);
            }
        }

        // 为没有防守任务的队员分配站位任务，需要依次检测各个防区有无障碍物，所以需要手动逐个确认
        int nTmpIndex;
        if (bInOwnField == true)
        {
            nTmpIndex = nNumRoleDef + 3; // 没有防守任务的角色index起始位置
        }
        else
        {
            nTmpIndex = nNumRoleDef + 2; // 没有防守任务的角色index起始位置
        }
        bool bEnemyInZone = false;
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
            if (nTmpIndex < 5)
            {
                posDefMove = m_Fix_KeepAwayFrom(300, 900, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,
                                                nKeepAwayFromBall, 0, nKeepAwayFromObst);
                pRole[nTmpIndex].ctrl = CTRL_MOVETO;
                pRole[nTmpIndex].nDest_x = posDefMove.x;
                pRole[nTmpIndex].nDest_y = posDefMove.y;
                tmp = FixSingleMove(nTmpIndex, pRole[nTmpIndex].nDest_x, pRole[nTmpIndex].nDest_y);
                pRole[nTmpIndex].nDest_x = tmp.x;
                pRole[nTmpIndex].nDest_y = tmp.y;
                pRole[nTmpIndex].nDest_angle = m_Loca_CalAngle(posDefMove.x, posDefMove.y, pDataSummary->cGlobalBall.x,
                                                               pDataSummary->cGlobalBall.y);
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
            if (nTmpIndex < 5)
            {
                posDefMove = m_Fix_KeepAwayFrom(300, 300, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,
                                                nKeepAwayFromBall, 0, nKeepAwayFromObst);
                pRole[nTmpIndex].ctrl = CTRL_MOVETO;
                pRole[nTmpIndex].nDest_x = posDefMove.x;
                pRole[nTmpIndex].nDest_y = posDefMove.y;
                tmp = FixSingleMove(nTmpIndex, pRole[nTmpIndex].nDest_x, pRole[nTmpIndex].nDest_y);
                pRole[nTmpIndex].nDest_x = tmp.x;
                pRole[nTmpIndex].nDest_y = tmp.y;
                pRole[nTmpIndex].nDest_angle = m_Loca_CalAngle(posDefMove.x, posDefMove.y, pDataSummary->cGlobalBall.x,
                                                               pDataSummary->cGlobalBall.y);
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
            if (nTmpIndex < 5)
            {
                posDefMove = m_Fix_KeepAwayFrom(400, 600, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,
                                                nKeepAwayFromBall, 0, nKeepAwayFromObst);
                pRole[nTmpIndex].ctrl = CTRL_MOVETO;
                pRole[nTmpIndex].nDest_x = posDefMove.x;
                pRole[nTmpIndex].nDest_y = posDefMove.y;
                tmp = FixSingleMove(nTmpIndex, pRole[nTmpIndex].nDest_x, pRole[nTmpIndex].nDest_y);
                pRole[nTmpIndex].nDest_x = tmp.x;
                pRole[nTmpIndex].nDest_y = tmp.y;
                pRole[nTmpIndex].nDest_angle = m_Loca_CalAngle(posDefMove.x, posDefMove.y, pDataSummary->cGlobalBall.x,
                                                               pDataSummary->cGlobalBall.y);
                nTmpIndex++;
            }
        }

        // 对MoveTo修正
        // m_Fix_MoveToAway();

        /////////////////////////////////////////////
        /*/方案2：跑固定点
        tmp = m_Fix_KeepAwayFrom(
            700, 600,	//默认坐标
            pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,	//需要远离的坐标
            nKeepAwayFromBall,		//与远离坐标保持的距离
            0,			//与队友保持距离
            nKeepAwayFromObst			//与障碍物保持距离
            );
        //其他
        pRole[2].nDest_x = tmp.x;
        pRole[2].nDest_y = tmp.y;
        pRole[2].nDest_angle = m_Loca_CalAngle(pRole[2].nDest_x, pRole[2].nDest_y, pDataSummary->cGlobalBall.x,
        pDataSummary->cGlobalBall.y);


        tmp = m_Fix_KeepAwayFrom(
            300, 900,	//默认坐标
            pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,	//需要远离的坐标
            nKeepAwayFromBall,		//与远离坐标保持的距离
            0,			//与队友保持距离
            nKeepAwayFromObst			//与障碍物保持距离
            );
        //其他
        pRole[3].nDest_x = tmp.x;
        pRole[3].nDest_y = tmp.y;
        pRole[3].nDest_angle = m_Loca_CalAngle(pRole[3].nDest_x, pRole[3].nDest_y, pDataSummary->cGlobalBall.x,
        pDataSummary->cGlobalBall.y);


        tmp = m_Fix_KeepAwayFrom(
            300, 300,	//默认坐标
            pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,	//需要远离的坐标
            nKeepAwayFromBall,		//与远离坐标保持的距离
            0,			//与队友保持距离
            nKeepAwayFromObst			//与障碍物保持距离
            );
        //其他
        pRole[4].nDest_x = tmp.x;
        pRole[4].nDest_y = tmp.y;
        pRole[4].nDest_angle = m_Loca_CalAngle(pRole[4].nDest_x, pRole[4].nDest_y, pDataSummary->cGlobalBall.x,
        pDataSummary->cGlobalBall.y);
        */
        //////////////////////////////////////////////////////////////////
    }
    else
    {
        strTacticInfo = "应对对方任意球跑位[未检测到球]";
        pRole[0].ctrl = CTRL_GOALKEEP;

        if (bCounterCornerKick == false)
        {
            pRole[1].ctrl = CTRL_MOVETO; // 后卫
            pRole[1].nDest_x = 300;
            pRole[1].nDest_y = 600;
            pRole[1].nDest_angle = 0;

            pRole[2].ctrl = CTRL_MOVETO; // 去往上一次球出现的位置
            pRole[2].nDest_x = pDataSummary->cGlobalBall.x;
            pRole[2].nDest_y = pDataSummary->cGlobalBall.x;
            pRole[2].nDest_angle = 0;
        }
        else
        {

            strTacticInfo = "对方角球跑位 - [未检测到球]";
            // 这是个对方的角球，去往己方半场角球位置
            pRole[1].ctrl = CTRL_MOVETO;
            pRole[1].nDest_x = 200;
            pRole[1].nDest_y = 200;
            pRole[1].nDest_angle = 0;

            pRole[2].ctrl = CTRL_MOVETO; // 去往上一次球出现的位置
            pRole[2].nDest_x = 200;
            pRole[2].nDest_y = 1000;
            pRole[2].nDest_angle = 0;
        }

        pRole[3].ctrl = CTRL_MOVETO;
        pRole[3].nDest_x = 800;
        pRole[3].nDest_y = 600;
        pRole[3].nDest_angle = 0;

        pRole[4].ctrl = CTRL_MOVETO;
        pRole[4].nDest_x = 800;
        pRole[4].nDest_y = 300;
        pRole[4].nDest_angle = 0;
    }
}

/************************************************************************/
/* 对方开球开始                                                             */
/************************************************************************/
void CTac_Counter_FreeKick::Step_Go()
{
    // 开球进行中...
    // 一旦有人进入进攻状态（检测到对方已经开球了！），则激活自主决策
    int i;
    for (i = 0; i < AG_NUM; i++)
    {
        if (pDataSummary->agent[i].online == true && pDataSummary->agent[i].status != ROBST_ERR)
        {
            if (pDataSummary->agent[i].status == CTRL_ATTACK)
            {
                pDataSummary->bAction = true;
                return;
            }
        }
    }

    // 时间超过开球时间（10秒），激活自主决策
    if (pDataSummary->nStartTimeCountSec > 10)
    {
        pDataSummary->bAction = true;
    }
    // 如果球的位置相对于Ready移动了很长的一个距离，也激活自主决策（未实现）

    uBall tmpball = pDataSummary->cGlobalBall;
    uRole *pRole = pTactics->role;
    CPoint tmp;
    strTacticInfo = "对方任意球开始！[正常]";
    switch (m_nGoStep)
    {
    case GO_STEP_READY:
        strTacticInfo = "对方任意球开始！[继续跑位准备中..]";

        // 一旦距离接近立刻开始（不然对方开球射门我方还在慢慢跑位太慢了，和我方开球不一样，这里不能太从容）
        if (isSomeoneNear(pDataSummary->agent, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, 400))
        {
            m_nGoStep = GO_STEP_ALERT;
        }
        else
        {
            // 看不到球的情况都包含在跑位里
            Step_Ready();
        }

        break;

    case GO_STEP_ALERT:
        strTacticInfo = "对方任意球开始！[开始警戒..]";
        // 警戒跑位队员进入警戒状态
        pRole[1].ctrl = CTRL_FOCUSONBALL;

        // 其他队员继续防守
        //[!]防守距离参数
        int nDefDist = 150;
        int nKeepAwayFromBall = 350; // 避开球的距离
        int nKeepAwayFromObst = 250; // 避开对方队员的距离
        int nTempDist = 0;
        CPoint posDefMove;
        /////////////////////////////////////////////////////////
        // MRL后新增：计算我方半场范围内需要防守的障碍物
        // a)获取需要防守的障碍物列表
        // Get3BackObstToDef();
        Get3ObstCloseToDef();
        int arEnemyIndexToDef[3];
        for (int i = 0; i < 3; i++)
        {
            arEnemyIndexToDef[i] = pDataSummary->arObstIndexBackDef[i];
        }

        // b)计算需要参与到防守任务的人数
        if (pDataSummary->nNumOfObstBackDef > 3)
        {
            pDataSummary->nNumOfObstBackDef = 3;
            PrintInfo("后防需要防守的障碍物数量异常");
        }

        int nNumRoleDef = pDataSummary->nNumOfObstBackDef; // 需要参与到障碍物防守中的人数

        // c)为障碍物防守角色分配任务(挡在障碍物和球之间)
        for (i = 0; i < nNumRoleDef; i++)
        {
            posDefMove =
                m_Loca_Ray(pDataSummary->t_enemy[arEnemyIndexToDef[i]].x, pDataSummary->t_enemy[arEnemyIndexToDef[i]].y,
                           pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, nDefDist);
            tmp = m_Fix_KeepAwayFrom(posDefMove.x, posDefMove.y,                               // 默认坐标
                                     pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, // 需要远离的坐标
                                     nKeepAwayFromBall,                                        // 与远离坐标保持的距离
                                     0,                                                        // 与队友保持距离
                                     50 // 与障碍物保持距离（防守队员这个相对比较小一点）
            );
            // 其他
            pRole[2 + i].nDest_x = tmp.x;
            pRole[2 + i].nDest_y = tmp.y;
            pRole[2 + i].nDest_angle = m_Loca_CalAngle(pRole[2 + i].nDest_x, pRole[2 + i].nDest_y,
                                                       pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y) +
                                       30;
        }

        // 为没有防守任务的队员分配站位任务，需要依次检测各个防区有无障碍物，所以需要手动逐个确认
        int nTmpIndex = nNumRoleDef + 2; // 没有防守任务的角色index起始位置
        bool bEnemyInZone = false;
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
            if (nTmpIndex < 5)
            {
                posDefMove = m_Fix_KeepAwayFrom(300, 900, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,
                                                nKeepAwayFromBall, 0, nKeepAwayFromObst);
                pRole[nTmpIndex].ctrl = CTRL_MOVETO;
                pRole[nTmpIndex].nDest_x = posDefMove.x;
                pRole[nTmpIndex].nDest_y = posDefMove.y;
                pRole[nTmpIndex].nDest_angle = m_Loca_CalAngle(posDefMove.x, posDefMove.y, pDataSummary->cGlobalBall.x,
                                                               pDataSummary->cGlobalBall.y);
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
            if (nTmpIndex < 5)
            {
                posDefMove = m_Fix_KeepAwayFrom(300, 300, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,
                                                nKeepAwayFromBall, 0, nKeepAwayFromObst);
                pRole[nTmpIndex].ctrl = CTRL_MOVETO;
                pRole[nTmpIndex].nDest_x = posDefMove.x;
                pRole[nTmpIndex].nDest_y = posDefMove.y;
                pRole[nTmpIndex].nDest_angle = m_Loca_CalAngle(posDefMove.x, posDefMove.y, pDataSummary->cGlobalBall.x,
                                                               pDataSummary->cGlobalBall.y);
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
            if (nTmpIndex < 5)
            {
                posDefMove = m_Fix_KeepAwayFrom(400, 600, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y,
                                                nKeepAwayFromBall, 0, nKeepAwayFromObst);
                pRole[nTmpIndex].ctrl = CTRL_MOVETO;
                pRole[nTmpIndex].nDest_x = posDefMove.x;
                pRole[nTmpIndex].nDest_y = posDefMove.y;
                pRole[nTmpIndex].nDest_angle = m_Loca_CalAngle(posDefMove.x, posDefMove.y, pDataSummary->cGlobalBall.x,
                                                               pDataSummary->cGlobalBall.y);
                nTmpIndex++;
            }
        }

        // 对MoveTo修正
        m_Fix_MoveToAway();

        /////////////////////////////////////////////

        // 一旦有队员检测到球动将自己切换成进攻，则激活正常比赛
        for (i = 0; i < AG_NUM; i++)
        {
            if (pDataSummary->agent[i].online == true &&
                (pDataSummary->agent[i].status == CTRL_ATTACK || pDataSummary->agent[i].status == CTRL_PASS_MOVE))
            {
                pDataSummary->bAction = true;
                break;
            }
        }
        break;
    }
}

/*

*/
CPoint CTac_Counter_FreeKick::m_Loc_DefInOurPenaltyArea()
{
    CPoint newpoint;
    // 按照球到球门前区域的斜率来
    int nBall_x = pDataSummary->cGlobalBall.x;
    int nBall_y = pDataSummary->cGlobalBall.y;
    int rayAngle = m_Loca_CalAngle(100, 600, nBall_x, nBall_y);
    if (rayAngle > 180)
    {
        rayAngle = rayAngle - 360;
    }

    /*CString str;
    str.Format("DefInOurPenaltyArea %d", rayAngle);
    PrintInfo(str);*/

    // a)rayAngle >= 90 左半场防御极端点
    if (rayAngle >= 90)
    {
        newpoint.x = 200;
        newpoint.y = 900;
    }

    // b) 90~60 沿左半场侧面线
    if (rayAngle < 90 && rayAngle >= 60)
    {
        newpoint.x = 100 + ((double)(nBall_x - 100) / (nBall_y - 600)) * 300;
        newpoint.y = 900;
    }

    // c) 60~-60 沿正面线
    if (rayAngle > -60 && rayAngle < 60)
    {
        newpoint.x = 200;
        newpoint.y = 600 + ((double)(nBall_y - 600) / (nBall_x - 100)) * 100;
    }

    // d）-60~-90 沿右半场侧面线
    if (rayAngle > -90 && rayAngle <= -60)
    {
        newpoint.x = 100 + ((double)(nBall_x - 100) / (600 - nBall_y)) * 300;
        newpoint.y = 300;
    }

    // e) <= -90 跑右侧面极端点
    if (rayAngle <= -90)
    {
        newpoint.x = 200;
        newpoint.y = 300;
    }

    // 对最终结果进行最后修正
    if (newpoint.x > 170)
    {
        newpoint.x = 170;
    }

    return newpoint;
}

/*

*/
CPoint CTac_Counter_FreeKick::FixSingleMove(int inID, int inX, int inY)
{
    CPoint resPoint;
    resPoint = m_Loca_Ray(pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, 1800, 600,
                          DIST_TAC_COUNTER + 100); // 瞎设一个初值

    int tmpX = inX;
    int tmpY = inY;

    if (m_CalDist(tmpX, tmpY, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y) < DIST_TAC_COUNTER)
    {
        resPoint = m_Loca_Ray(pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, tmpX, tmpY, DIST_TAC_COUNTER);
        tmpX = resPoint.x;
        tmpY = resPoint.y;
    }

    int nRotAngle = 0;
    CPoint tmpPoint;
    while (chkPosRight(tmpX, tmpY) == false || m_MinDistToMoveToRoleBefore(tmpX, tmpY, inID) < 100)
    {
        nRotAngle++;
        tmpPoint = m_Loca_Rotate(tmpX, tmpY, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, nRotAngle);
        tmpX = tmpPoint.x;
        tmpY = tmpPoint.y;
    }

    resPoint.x = tmpX;
    resPoint.y = tmpY;
    return resPoint;
}

/*

*/
bool CTac_Counter_FreeKick::chkPosRight(int inX, int inY)
{
    bool res = true;

    // 场地外
    if (inX < 0 || inX > 1800 || inY < 0 || inY > 1200)
    {
        res = false;
    }

    // 禁区里
    if (inX < 230 && inY > 270 && inY < 930)
    {
        res = false;
    }

    return res;
}
