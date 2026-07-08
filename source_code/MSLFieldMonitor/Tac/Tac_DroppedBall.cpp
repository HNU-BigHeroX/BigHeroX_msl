#include "stdafx.h"
#include "Tac_DroppedBall.h"

CTac_DroppedBall::CTac_DroppedBall()
{
    m_nTacStep = TAC_STEP_READY;
    m_nGoStep = GO_STEP_READY;
    pTacPlaying = NULL;
}

CTac_DroppedBall::~CTac_DroppedBall()
{
}

void CTac_DroppedBall::Reset()
{
    m_nTacStep = TAC_STEP_READY;
    m_nGoStep = GO_STEP_READY;
}

void CTac_DroppedBall::PlanTac(stTactics *outTactics)
{
    pTactics = outTactics;
    switch (m_nTacStep)
    {
    case TAC_STEP_READY:
        Step_Ready();
        break;
    case TAC_STEP_GO:
        Step_Go();
        break;
    }
}

/************************************************************************/
/* 争球准备跑位                                                         */
/************************************************************************/
void CTac_DroppedBall::Step_Ready()
{
    // 争球跑位
    uRole *pRole = pTactics->role;
    if (pDataSummary->bNoOneSeeBall == false)
    {
        strTacticInfo = "争球跑位[正常]";
        pRole[0].ctrl = CTRL_GOALKEEP;
        // 其他队员跑动到指定点
        for (int i = 1; i < 5; i++)
        {
            pRole[i].ctrl = CTRL_MOVETO;
            pRole[i].nDest_angle = 0;
        }

        uBall tmpball = pDataSummary->cGlobalBall;

        if (tmpball.x == 900 && tmpball.y == 600)
        {
            tmpball.x = 901;
        }

        // 球-警戒队员-己方球门 为一直线 （距离为争球距离）
        CPoint tmp = m_Loca_Ray(tmpball.x, tmpball.y, 100, 600,
                                (int)(DIST_TAC_DROPPEDBALL * pDataSummary->kDistance)); // 最后一个参数是距离
        pRole[1].nDest_x = tmp.x;
        pRole[1].nDest_y = tmp.y;
        pRole[1].nDest_angle = m_Loca_CalAngle(tmp.x, tmp.y, tmpball.x, tmpball.y);
        posFocusOnBall = tmp;

        int nKeepAwayFromBall = 300;
        int nKeepAwayFromObst = 100;
        tmp = m_Fix_KeepAwayFrom(700, 600,                                                 // 默认坐标
                                 pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, // 需要远离的坐标
                                 nKeepAwayFromBall,                                        // 与远离坐标保持的距离
                                 0,                                                        // 与队友保持距离
                                 nKeepAwayFromObst                                         // 与障碍物保持距离
        );
        // 其他
        pRole[2].nDest_x = tmp.x;
        pRole[2].nDest_y = tmp.y;
        pRole[2].nDest_angle = m_Loca_CalAngle(pRole[2].nDest_x, pRole[2].nDest_y, pDataSummary->cGlobalBall.x,
                                               pDataSummary->cGlobalBall.y);

        tmp = m_Fix_KeepAwayFrom(300, 900,                                                 // 默认坐标
                                 pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, // 需要远离的坐标
                                 nKeepAwayFromBall,                                        // 与远离坐标保持的距离
                                 0,                                                        // 与队友保持距离
                                 nKeepAwayFromObst                                         // 与障碍物保持距离
        );
        pRole[3].nDest_x = tmp.x;
        pRole[3].nDest_y = tmp.y;
        pRole[3].nDest_angle = m_Loca_CalAngle(pRole[3].nDest_x, pRole[3].nDest_y, pDataSummary->cGlobalBall.x,
                                               pDataSummary->cGlobalBall.y);

        tmp = m_Fix_KeepAwayFrom(300, 300,                                                 // 默认坐标
                                 pDataSummary->cGlobalBall.x, pDataSummary->cGlobalBall.y, // 需要远离的坐标
                                 nKeepAwayFromBall,                                        // 与远离坐标保持的距离
                                 0,                                                        // 与队友保持距离
                                 nKeepAwayFromObst                                         // 与障碍物保持距离
        );
        pRole[4].nDest_x = tmp.x;
        pRole[4].nDest_y = tmp.y;
        pRole[4].nDest_angle = m_Loca_CalAngle(pRole[4].nDest_x, pRole[4].nDest_y, pDataSummary->cGlobalBall.x,
                                               pDataSummary->cGlobalBall.y);
    }
    else
    {
        strTacticInfo = "争球跑位[未检测到球]";
        pRole[0].ctrl = CTRL_GOALKEEP;

        pRole[1].ctrl = CTRL_MOVETO; // 后卫
        pRole[1].nDest_x = 300;
        pRole[1].nDest_y = 600;
        pRole[1].nDest_angle = 0;

        pRole[2].ctrl = CTRL_MOVETO; // 去往上一次球出现的位置
        pRole[2].nDest_x = pDataSummary->cGlobalBall.x;
        pRole[2].nDest_y = pDataSummary->cGlobalBall.x;
        pRole[2].nDest_angle = 0;

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
/* 争球开始                                                             */
/************************************************************************/
void CTac_DroppedBall::Step_Go()
{
    strTacticInfo = "争球开始！";
    // 争球开始，立即进入自主决策状态
    pTacPlaying->Reset();
    pTacPlaying->PlanTac(pTactics);
    pDataSummary->bAction = true;
}
