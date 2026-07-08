/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 2.0       Date: 2008/9/23
  Email:  zwj_uptech@126.com
  Description:    // agent端TCP通讯类，单个连接
  Others:         //
*************************************************/
// AgentNet.h: interface for the CAgentNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGENTNET_H__E46F5841_20A7_49C0_900D_B20A1AB448B9__INCLUDED_)
#define AFX_AGENTNET_H__E46F5841_20A7_49C0_900D_B20A1AB448B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TCPClient.h"
#include "MSG_Control.h"

class CAgentNet : public CTCPClient
{
  public:
    MSG_Control m_ctrl;
    void Received(void *inbuf, int inlen);
    CAgentNet();
    virtual ~CAgentNet();

  protected:
    void ParseCtrl(MSG_Control inCtrl);

    int roffset;
    BYTE msgbuf[512];
};

#endif // !defined(AFX_AGENTNET_H__E46F5841_20A7_49C0_900D_B20A1AB448B9__INCLUDED_)
