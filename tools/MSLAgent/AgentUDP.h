/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.0       Date: 2009/6/9
  Email:  zwj_uptech@126.com
  Description:    // 球员Agent UDP通讯
  Others:         //
*************************************************/
//////////////////////////////////////////////////////////////////////////
//
// AgentUDP.h: interface for the CAgentUDP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGENTUDP_H__7F811CF5_110C_45AD_8C95_2AF1A7D482E8__INCLUDED_)
#define AFX_AGENTUDP_H__7F811CF5_110C_45AD_8C95_2AF1A7D482E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UDPServer.h"
#include "MSL_Protocol.h"

#define AGPBUF 512
#define AG_OBST_NUM 10

class CAgentUDP : public CUDPServer
{
  public:
    void PrintInfo(CString inStr);
    void SetAttitudeFPS(int inFPS);
    void SetVisionFPS(int inFPS);
    void SetSerialPortFPS(int inFPS);
    void SetMulticastFPS(int inFPS);
    void SetRobotStatus(BYTE inRobotPower, BYTE inRobotCharge);
    void SetSoftwareVer(float inVer);
    void SetCPUStatus(int inCPULoad, float inCPUFrequency);
    void SetBatteryStatus(BYTE inACLineStatus, BYTE inBatteryFlag, BYTE inBatteryLifePercent, DWORD inBatteryLifeTime);
    void ReportNew();
    void Report();
    void m_PackInfo();
    void SetCoachIP(CString inStr);
    void AttentionKick(int inSelfX, int inSelfY, int inTargetX, int inTargetY);
    void ClearRecvTM();
    void ClearRecvObst();
    void Clear_Obst();
    void AddObst(int inX, int inY, int inSize);
    int m_Piece2int(unsigned char *inTarg);
    void m_ParseFrame(unsigned char *inBuf, int inLen);
    void m_ParseByte(unsigned char inChar);
    void Received(void *inbuf, int inlen, SOCKADDR *inpSock);
    unsigned char m_calsum(unsigned char *inBuf, int inlen);
    void m_Split2Bytes(unsigned char *inTarg, unsigned int inSrc);
    void Report(int inx, int iny, int inAng, int inSt, bool foundb, int inBx, int inBy);
    void SetID(int inID);
    CAgentUDP();
    virtual ~CAgentUDP();

    unsigned char *pbuf;
    unsigned char m_ParseBuf[AGPBUF];
    int m_nRecvIndex;         // 接收索引
    unsigned char m_lastRecv; // 上一个字符
    int m_bFrameStart;        // 帧解析开始
    int m_nFrameLength;       // 帧长度

    // rec data
    int rec_ID;
    int rec_ctrl;
    int re_ballx;
    int re_bally;
    int rec_targetx;
    int rec_targety;

    int rec_targetang;
    int rec_MoveSpd;
    int rec_Def_Dist;
    int rec_Def_Ang;
    bool rec_bField;
    obstacle rec_obst[AG_OBST_NUM];
    int rec_init_x;
    int rec_init_y;
    int rec_init_angle;
    BYTE rec_init_mode;
    teammate m_tm[5];
    unsigned char rec_other[100];

    // to send
    int toS_x;
    int toS_y;
    int toS_angle;
    int toS_status;
    bool toS_foundball;
    int toS_ballx;
    int toS_bally;
    int toS_V;
    int toS_V_Angle;
    int toS_Kick;    // 是否击球
    int toS_TargetX; // 传球目标点
    int toS_TargetY;
    // 障碍物信息
    obstacle m_obst[AG_OBST_NUM];

    CListBox *pInfoList;

  protected:
    void m_Float2Bytes(unsigned char *inTarg, float inVal);
    unsigned char sendbuf[AGPBUF];
    unsigned char m_curIP[4]; // 当前数据的源ip
};

#endif // !defined(AFX_AGENTUDP_H__7F811CF5_110C_45AD_8C95_2AF1A7D482E8__INCLUDED_)
