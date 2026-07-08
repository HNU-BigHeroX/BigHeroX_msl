#include "stdafx.h"
#include "Tac_Counter_KickOff.h"

CTac_Counter_KickOff::CTac_Counter_KickOff() // 此函数会在创建新的实例的时候自动执行
{
    m_nTacStep = TAC_STEP_READY; // 1 开球准备
    m_nGoStep = GO_STEP_READY;   // 11 还没到位置，继续跑位
}

CTac_Counter_KickOff::~CTac_Counter_KickOff()
{
}

void CTac_Counter_KickOff::Reset() // 重置参数
{
    m_nTacStep = TAC_STEP_READY; // 1
    m_nGoStep = GO_STEP_READY;   // 11

    // 通过下面这两个参数的设置可以动态调整对方开球时自己的站位
    bWaitLeft = false;   // 开球站位时偏向左边（接近对方接球队员）
    nLeftCount = 0;      // 对方接球队员在左侧的计数
    nRightCount = 0;     // 对方接球队员在右侧的计数
    bAdjustDone = false; // 调整完毕
}

void CTac_Counter_KickOff::PlanTac(stTactics *outTactics)
{
    pTactics = outTactics; // stTactics在MSL_Structs.h中定义，是生成的策略结果
    // 此switch用于调整比赛时的状态，分为准备阶段和开始阶段
    switch (m_nTacStep)
    {
    case TAC_STEP_READY:
        Step_Ready();
        break;
    case TAC_STEP_GO:
        Step_Go();
        break;

    default:
        m_nTacStep = TAC_STEP_GO;
        PrintInfo("CTac_Counter_KickOff:未知状态");
        break;
    }
}

/************************************************************************/
/* 应对对方开球准备跑位                                                         */
/************************************************************************/
void CTac_Counter_KickOff::Step_Ready()
{
    // 应对对方开球跑位
    uRole *pRole = pTactics->role;
    if (pDataSummary->bNoOneSeeBall == false) // bNoOneSeeBall指的是 是否侦测到球
    {
        strTacticInfo = "应对对方开球跑位 - 正常";
        pRole[0].ctrl = CTRL_GOALKEEP; // CTRL_GOALKEEP=2  猜测为守门员
        // 其他队员跑动到指定点
        // 这样的写法可以保证在有球员缺失的情况下还能正确执行
        for (int i = 1; i < 5; i++)
        {
            pRole[i].ctrl = CTRL_MOVETO; // CTRL_MOVETO=11 猜测为运动到某个位置的标志
            pRole[i].nDest_angle = 0;    // 猜测为机器人角度朝向设置
        }

        uBall tmpball = pDataSummary->cGlobalBall; // cGlobalBall球坐标  uBall是一个结构体

        /*
        //球 记录单个球信息
        typedef struct uBall
        {
            UINT ID;
            int x;
            int y;
            bool updated;
        }uBall;
        */

        // 警戒队员1
        // 球-警戒队员-己方球门 为一直线
        //  m_Loca_Ray函数出处还未找到 Cpoint类型还未找到
        CPoint tmp = m_Loca_Ray(tmpball.x, tmpball.y, 150, 600, (int)(DIST_TAC_COUNTER * pDataSummary->kDistance));
        pRole[1].nDest_x = tmp.x; // 设置机器人x坐标  左下角开始 1800×1200（宽×高）
        pRole[1].nDest_y = tmp.y; // 设置机器人y坐标
        pRole[1].nDest_angle = m_Loca_CalAngle(tmp.x, tmp.y, tmpball.x, tmpball.y); // 角度朝向计算并传值

        /////////////////////////////////////
        // 根据对方站位来确定第二名队员站位
        /*for (int i = 0; i < AG_NUM;i++)
        {
        m_Chk_InMidField()
        }*/
        /////////////////////////////////////

        // 警戒队员2
        pRole[2].nDest_x = 600;
        pRole[2].nDest_y = 350;
        pRole[2].nDest_angle = 0;
        // 警戒队员3
        pRole[3].nDest_x = 600;
        pRole[3].nDest_y = 850;
        pRole[3].nDest_angle = 0;
        // 警戒队员4
        pRole[4].nDest_x = 300;
        pRole[4].nDest_y = 600;
        pRole[4].nDest_angle = 0;
    }
    else // 没检测到球的情况
    {
        strTacticInfo = "应对对方开球跑位 - 未检测到球";
        pRole[0].ctrl = CTRL_GOALKEEP; // 同样设置守门员的转态

        // 其他队员跑动到指定点
        for (int i = 1; i < 5; i++)
        {
            pRole[i].ctrl = CTRL_MOVETO; // CTRL_MOVETO=11 猜测为运动到某个位置的标志
            pRole[i].nDest_angle = 0;
        }

        // 计算跑动位置
        //  下面四个位置是直接固定的，是没看到球后的一个策略
        pRole[1].nDest_x = 600;
        pRole[1].nDest_y = 600;
        pRole[1].nDest_angle = 0;

        pRole[2].nDest_x = 600;
        pRole[2].nDest_y = 900;
        pRole[2].nDest_angle = 0;

        pRole[3].nDest_x = 300;
        pRole[3].nDest_y = 300;
        pRole[3].nDest_angle = 0;

        pRole[4].nDest_x = 300;
        pRole[4].nDest_y = 900;
        pRole[4].nDest_angle = 0;
    }
}

/************************************************************************/
/* 对方开球开始                                                             */
/************************************************************************/
void CTac_Counter_KickOff::Step_Go()
{
    // 对方开球进行中...
    // 一旦有人进入进攻状态（检测到对方已经开球了！），则激活自主决策
    int i;
    for (i = 0; i < AG_NUM; i++) // AG_NUM猜测指的是总球员数
    {
        if (pDataSummary->agent[i].online == true && pDataSummary->agent[i].status != ROBST_ERR)
        { // pDataSummary共享数据 agent队员情况 .online指的是在不在线 .status指的是其当前的状态
            if (pDataSummary->agent[i].status == CTRL_ATTACK) // CTRL_ATTACK=1 猜测为进攻状态
            {
                pDataSummary->bAction = true; // pDataSummary->bAction共享数据中启动自主决策的标志位
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
    uBall tmpball = pDataSummary->cGlobalBall; // cGlobalBall球坐标
    uRole *pRole = pTactics->role;
    CPoint tmp;

    strTacticInfo = "对方开球开始！ - 正常";
    switch (m_nGoStep) // m_nGoStep是step_ready()后要执行的动作的参数
    {
    case GO_STEP_READY:
        strTacticInfo = "对方开球开始！ - 继续跑位准备中..";

        // 一旦距离接近立刻开始（不然对方开球射门我方还在慢慢跑位太慢了，和我方开球不一样，这里不能太从容）
        if (isSomeoneNear(pDataSummary->agent, pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, 500)
            // 猜测isSomeoneNear这个函数是让最接近的那个机器人去把守球
        )
        {
            m_nGoStep = GO_STEP_ALERT; // GO_STEP_ALERT对方开球警戒（一旦球动就开始）
        }
        else
        {
            // 看不到球的情况都包含在跑位里
            Step_Ready();
        }
        break;

    case GO_STEP_ALERT:
        strTacticInfo = "对方开球开始！ - 开始警戒..";
        // 警戒跑位队员进入警戒状态
        pRole[1].ctrl = CTRL_FOCUSONBALL; // CTRL_FOCUSONBALL=13 猜测为警戒的标志位
        pRole[2].ctrl = CTRL_FOCUSONBALL;
        pRole[3].ctrl = CTRL_FOCUSONBALL;
        // 一旦有队员检测到球动将自己切换成进攻，则激活正常比赛
        for (int i = 0; i < AG_NUM; i++)
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
