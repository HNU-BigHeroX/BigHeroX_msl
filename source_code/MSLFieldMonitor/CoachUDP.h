// CoachUDP.h: interface for the CCoachUDP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COACHUDP_H__2CB0229F_841F_4740_9F86_33E8577CABBA__INCLUDED_)
#define AFX_COACHUDP_H__2CB0229F_841F_4740_9F86_33E8577CABBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UDPReceive.h"
#include "ToAgent.h"
#include "MSL_Structs.h"

#define CNTMAX 20 // 最大连接数

#define UPUDPBUFLEN 1024

typedef struct AgCnt
{
    unsigned char ip[4];
    int ID;
    int reccnt;
} AgCnt;

class CCoachUDP : public CUDPReceive
{
  public:
    void AddCredit(int inUnknown, int inCredit);
    void ClearCredit(int inIndex);
    void AllCredit();
    void UpdateCntList();
    void m_ag_Scan();
    int m_Piece2int(unsigned char *inTarg);
    void m_ParseFrame(unsigned char *inBuf, int inLen);
    void m_ParseByte(unsigned char inChar);
    void CallbackReceived(char *inBuf, int inLen, SOCKADDR *inpSock);
    CCoachUDP();
    virtual ~CCoachUDP();

    AgCnt m_unknownCnt[CNTMAX]; // 未知的连接
    int m_nCntNum;              // 未知连接数量
    CListCtrl *m_pCntList;      // 显示未知连接

    int m_recvCnt[5];  // 数据包计数
    int m_recvLast[5]; // 上一秒收到的数据包总数

    AgCnt m_creditCnt[5]; // 信任连接
    bool m_bEnIPFilter;   // 是否启用ip过滤

    CString strCurIP;  // 当前数据的源ip
    BYTE m_arCurIP[4]; // 以字节数组记录当前IP，以供与来源比对
    int m_curID;       // 当前正在解析的数据球员ID

    CToAgent m_ToAgent[AG_NUM]; // 向agents发送数据
    unsigned char *m_sendbuf;   // 发送缓冲

    bool m_bFoundBall;   // 临时变量
    unsigned char *pbuf; // 临时变量
    bool m_bPassKick;    // 传球队员已传球
    UINT nPassKickTime;  // 传球队员起脚时间

    unsigned char *m_ParseBuf;
    int m_nRecvIndex;           // 接收索引
    unsigned char m_lastRecv;   // 上一个字符
    int m_bFrameStart;          // 帧解析开始
    int m_nFrameLength;         // 帧长度
    uAgent m_recvAgent[AG_NUM]; // 缓冲数据

    CPoint posTouchBall;    // 触球点坐标
    bool bHoldBall;         // 是否在触球
    int m_nLastDribblingID; // 记录带球队员的ID

    CListBox *pInfoList; // 信息输出

    // 记录传球队员发回的接球坐标
    int posPasserKickTo_x;
    int posPasserKickTo_y;

    // 发送数据给历史记录程序
    CUDPSend m_LogUdpSender[5];

  protected:
    bool m_CheckCredit(unsigned char *inIP);
    unsigned char m_calsum(unsigned char *inBuf, int inlen);

  public:
    void CopyDataSummaryTo(stDataSummary *inData);
    void AgentCmdMoveTo(int inID, int inTargetx, int inTargety, int inTargetAng, int inDist, bool inbField, int inSpd);
    float m_Piece2float(unsigned char *inBuf);
    void CheckAgentReply();
    void PrintInfo(CString inStr);
};

#endif // !defined(AFX_COACHUDP_H__2CB0229F_841F_4740_9F86_33E8577CABBA__INCLUDED_)
