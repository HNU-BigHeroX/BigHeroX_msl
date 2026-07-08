/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.2       Date: 2008/11/3
  Email:  zwj_uptech@126.com
  Description:    // 裁判盒指令解析类，单个连接
  Others:         // 1.2 与控制连接
*************************************************/
// RefClient.h: interface for the CRefClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REFCLIENT_H__3456AE23_F539_40D6_B727_C7471942F8FE__INCLUDED_)
#define AFX_REFCLIENT_H__3456AE23_F539_40D6_B727_C7471942F8FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TCPClient.h"
#include "DataCenter.h"

#define MAGENTA 0
#define CYAN 1

// play commands
#define COMM_STOP 'S'
#define COMM_START 's'
#define COMM_HALT 'H'
#define COMM_READY ' '
#define COMM_PARKING 'L'
// game flow commands
#define COMM_FIRST_HALF '1'
#define COMM_HALF_TIME 'h'
#define COMM_SECOND_HALF '2'
#define COMM_END_GAME 'e'
#define COMM_CANCEL 'x'
// goal status
#define COMM_GOAL_MAGENTA 'a'
#define COMM_GOAL_CYAN 'A'
#define COMM_SUBGOAL_MAGENTA 'd'
#define COMM_SUBGOAL_CYAN 'D'
/* game flow commands */
#define COMM_RESTART 'n'
#define COMM_KICKOFF_MAGENTA 'k'
#define COMM_KICKOFF_CYAN 'K'
#define COMM_FREEKICK_MAGENTA 'f'
#define COMM_FREEKICK_CYAN 'F'
#define COMM_GOALKICK_MAGENTA 'g'
#define COMM_GOALKICK_CYAN 'G'
#define COMM_THROWIN_MAGENTA 't'
#define COMM_THROWIN_CYAN 'T'
#define COMM_CORNER_MAGENTA 'c'
#define COMM_CORNER_CYAN 'C'
#define COMM_PENALTY_MAGENTA 'p'
#define COMM_PENALTY_CYAN 'P'
#define COMM_DROPPED_BALL 'N'
#define COMM_DUMMY '*'
// acceptable referee commands
#define COMM_CMD_STRING "pPkKNnCcTtdDgGfFxaAZe2h1 HsS*"

class CRefClient : public CTCPClient
{
  public:
    void PlayerSubstitution(CString inTeam, int inIn, int inOut);
    void PlayerIn(CString inTeam, int inID);
    void PlayerOut(CString inTeam, int inID, int inSec);
    void ConnectSucceed();
    void isDisconnected();
    void Ref_Start();
    char *pRec;
    void Parse(char inchar);
    void Received(void *inbuf, int inlen);
    CRefClient();
    virtual ~CRefClient();

    //
    char m_lastflag;
    CDataCenter *pDataCenter;

    bool m_bConnected;

    // self
    UINT Self;

    // message
    CString m_strMessage;
    int m_nLenofStr;

    enum
    {
        BeforeGame,
        FirstHalf,
        HalfTime,
        SecondHalf,
        EndGame
    } m_eGameFlow;
};

#endif // !defined(AFX_REFCLIENT_H__3456AE23_F539_40D6_B727_C7471942F8FE__INCLUDED_)
