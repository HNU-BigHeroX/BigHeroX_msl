// AgentNet.h: interface for the CAgentNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGENTNET_H__E99F7876_6F48_4E93_8DA6_CFE218D7E9F8__INCLUDED_)
#define AFX_AGENTNET_H__E99F7876_6F48_4E93_8DA6_CFE218D7E9F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "NetCmt.h"
#include "MSG_Control.h"
#include "IBehavior.h"
#include "VoyWith2Cap.h"

#define CMTKEY 0x11112222

class CAgentNet : public CNetCmt
{
  public:
    IBehavior m_idle;
    IBehavior *AttackBeh;
    IBehavior *DefBeh;
    void SetID(UINT inID);
    void AfterReceived(void *buf, int length, char *pFromIP);
    CAgentNet();
    virtual ~CAgentNet();

  protected:
    void ParseCtrl(MSG_Control inCtrl);
    MSG_Control m_ctrl;
    int roffset;
    BYTE msgbuf[50];
    UINT m_nID;
    UINT m_nKey;
};

#endif // !defined(AFX_AGENTNET_H__E99F7876_6F48_4E93_8DA6_CFE218D7E9F8__INCLUDED_)
