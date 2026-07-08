/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 2.0       Date: 2008/11/19
  Email:  zwj_uptech@126.com
  Description:    // 图片显示类
  Others:         // 1.1为MSLCoach做了改动，作为环境模型
                  // 1.4改进了连接断开时引发的错误
                  // 1.5加入了时间计量
                  // 1.6加入比赛状态显示
                  // 1.7对球的筛选方法进行了改进
                  // 1.8m_pb的管理方式参照m_pa，避免频繁的链表操作造成不稳定
                  // 1.9为彻底避免链表操作造成的不稳定可能性，改用数组管理球
                  // 2.0鉴于数组的稳定性，将球员管理也改用数组方式
                  // 2.1修正了多个转换到数组以后的bug
                  // 特别版，专为agent模拟程序优化
*************************************************/

#if !defined(AFX_UPFULLFIELD_H__6D15FBB6_5DE4_408C_AE94_83C240DA6ED4__INCLUDED_)
#define AFX_UPFULLFIELD_H__6D15FBB6_5DE4_408C_AE94_83C240DA6ED4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UPFullField.h : header file
//
#include <math.h>
#include <afxmt.h>
#include "NumDisplay.h"
#include "AgentUDP.h"

// 智能体结构体
struct uagent
{
    UINT ID;
    int x;
    int y;
    double angle;
    uagent *next;

    // move target    /*-仿真中是Coach指示的移动目标点-*/
    int moveto_x;
    int moveto_y;
    int moveto_angle;
    int moveto_spd;

    // pass and catch
    int target_x;
    int target_y;
    int from_x;
    int from_y;

    // status
    UINT status;
    bool foundball;
    int dist_ball;
    int V;
    int V_Angle;

    // def
    int def_dist;
    int def_angle;

    /*仿真中为使能功能*/
    bool online;
};

// 球 记录单个球信息
struct uball
{
    UINT ID;
    int x;
    int y;
    bool updated;
    uball *next;
};

#define OBST_NUM 100

#define BLUEGOAL 0
#define YELLOWGOAL 1

/////////////////////////////////////////////////////////////////////////////
// CUPFullField window

class CUPFullField : public CStatic
{
    // Construction
  public:
    CUPFullField();

    // Attributes
  public:
    UINT OwnGoalColor;

    // select
    int SelID;
    CPoint m_self;
    CPoint m_sTp;
    double m_SelAng;

    bool m_bAllBall;
    bool m_bKeyHold;

    // output
    CListCtrl *pAglist;
    CNumDisplay *pnumdis;

    // 发送对象
    CAgentUDP m_AgUDPs[7];
    CAgentUDP m_AgToCap[7];

    // 虚拟角色
    UINT m_nVirtualSt[7];

    // 将开球队员和球观察员迅速切换进攻标记
    bool m_bAtkIm;
    int m_nCycle;

    // 设置障碍物
    bool m_bSetObst;
    int m_curObst;
    int m_nObstID;

    // 设置全局障碍物
    obstacle m_trueobst[OBST_NUM / 5];
    bool m_bSetTrueObst;
    int m_MoveTrueObstIndex;

    bool bObstBust;

    enum
    {
        none = 0,
        BallSel,
        AgentSel,
        ObstSel,
        TrueobstSel
    } m_eSel;

  protected:
    CBitmap m_BkBmp;
    uagent m_agents[7]; // 球员管理
    CCriticalSection m_asc;
    uball m_balls[7]; // 球的管理
    CCriticalSection m_bsc;
    CPoint m_recball;

    obstacle m_obst[OBST_NUM];

    int m_nAgentNum; // agent的个数

    // 状态显示
    CFont font;
    CFont txtfont;
    CString m_strCurSt;
    CString m_strTime;
    CCriticalSection m_critiTime;
    int m_chkagnt;

    // 球的移动
    bool m_bBallMove;
    int m_nBallTarX;
    int m_nBallTarY;
    int m_nPassPlayer;

    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CUPFullField)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    int m_Loca_CalAngle(int inXbase, int inYbase, int inXtarg, int inYtarg);
    double m_CalAngle(int inx1, int iny1, int inVx, int inVy, int inx2, int iny2);
    void m_BehCatch(uagent *inagent);
    void m_BehPassMove(uagent *inagent);
    void AgentGetBall(int inID);
    void SetAllBall(bool inEnable);
    void AddAgent(int inID, bool inFoundball, CString inCoachIP, int inPort);
    void ReplyAgentsStatus();
    void ParseRecCtrl(int inID, CAgentUDP *inAg);
    void KickAgent(int inID);
    CString GetTime();
    CString GetCTRLString(int inCtrlCode);
    void Exec(); // 被定时调用的动态

    void GetCurTime();
    void printf(CString instr);
    uagent *GetAgent(UINT inID);
    void RemoveBall(UINT inID);
    void RemoveAgent(int inID);
    void SetBall(UINT inID, int inx, int iny);
    virtual ~CUPFullField();

    // Generated message map functions
  protected:
    void m_BehDEF_Goal(uagent *inagent);
    void m_BehDEF_Ball(uagent *inagent);
    void m_BehFollow(uagent *inagent);
    void m_BallMoveTo(int inTarX, int inTarY);
    void m_ClearObstsNear(int inx, int iny, int inrag);
    void m_SetObstOnTrueobsts();
    void m_VirtualRole(int inID, UINT inStatus);
    CPoint m_Loca_Rotate(int inx, int iny, int inCx, int inCy, int inAng);
    CPoint m_Loca_Ray(int inXbase, int inYbase, int inXtarg, int inYtarg, int inDist);
    void m_BehKOSReady(uagent *inagent);
    void m_BehKOPReady(uagent *inagent);
    void m_BehMoveto(uagent *inagent, int inX, int inY, int inAngle);
    void m_BehAttack(uagent *inagent);
    void m_BehDef(uagent *inagent);
    void m_BehGoalKeeper(uagent *inagent);
    void m_BehMoveto(uagent *inagent);
    void m_ReleaseAgentList(uagent *inList);
    uagent *m_GetOnlineAgent();
    void m_SortAgent();
    CPoint m_CalMoveTo(int x, int y);
    int m_CalDist(int inx1, int iny1, int inx2, int iny2);
    CPoint m_CalPoint(int inx, int iny);
    void m_Clearpa();
    void m_Clearpb();
    //{{AFX_MSG(CUPFullField)
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnClose();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPFULLFIELD_H__6D15FBB6_5DE4_408C_AE94_83C240DA6ED4__INCLUDED_)
