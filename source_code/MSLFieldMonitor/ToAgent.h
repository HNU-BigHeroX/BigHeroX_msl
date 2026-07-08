// ToAgent.h: interface for the CToAgent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOAGENT_H__A14C67D1_131F_4902_BA0A_F5DBC730260B__INCLUDED_)
#define AFX_TOAGENT_H__A14C67D1_131F_4902_BA0A_F5DBC730260B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UDPSend.h"
#include "MSL_Structs.h"

#define TO_AG_OB_NUM 10

class CToAgent : public CUDPSend
{
  public:
    void DEF_GOAL(int inTargetx, int inTargety, int inDist = 0);
    void DEF_BALL(int inTargetx, int inTargety, int inDist = 0);
    void Follow(int inTargetx, int inTargety, int inDist = 0);
    void m_PackInfo();
    void Clear_Teammate();
    void SetTeammate(int inID, int inState, int inX, int inY, int inAngle);
    void ZoneDefToCmd(int inTargetx, int inTargety, int inTargetAng, int inDist, bool inbField, int inSpd);
    void SetInitPos(int inx, int iny, int inangle);
    void CatchCmd(int inCatchX, int inCatchY);
    void PassCmd(int inTargX, int inTargY);
    void MoveTo(int inTargetx, int inTargety, int inTargetAng, int inDist, bool inbField, int inSpd);
    void Clear_Obst();
    void AddObst(int inX, int inY, int inSize);
    void DefCmd(int inAngle, int inDist);
    void SetRefBall(int inBallx, int inBally);
    void MoveCmd(int inTargetx, int inTargety, int inTargetAng, int inDist, bool inbField);
    void CtrlCmd(int inCtrl, int inReBx, int inReBy, int inAng, int inDist, bool inbField);
    void SetID(int inID);
    CToAgent();
    virtual ~CToAgent();

  public:
    unsigned char m_calsum(unsigned char *inBuf, int inlen);
    void m_Split2Bytes(unsigned char *inTarg, unsigned int inSrc);
    int m_Piece2int(unsigned char *inTarg);
    unsigned char *m_sendbuf;

    // 障碍物
    obstacle m_cObstToSend[TO_AG_OB_NUM];
    // 队员信息
    teammate m_cTeamMate[5];

    // 扩展数据
    unsigned char toA_other[100];

    CString strRemoteIP; // 某队员的ip地址

    bool m_bInitPos;
    int m_nInit_x;
    int m_nInit_y;
    int m_nInit_angle;

    bool bFoundBall;
    int m_nBall_x;
    int m_nBall_y;

    // 发送数据给历史记录程序
    CUDPSend m_LogUdpSender;
    bool bLogMode;
    void SimpleCtrl(int inCtrl);
};

#endif // !defined(AFX_TOAGENT_H__A14C67D1_131F_4902_BA0A_F5DBC730260B__INCLUDED_)
