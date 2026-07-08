// AgentNet.cpp: implementation of the CAgentNet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AgentNet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAgentNet::CAgentNet()
{
    roffset = 0;
    m_nKey = CMTKEY;
    AttackBeh = NULL;
    DefBeh = NULL;
    //	pVoy = NULL;
}

CAgentNet::~CAgentNet()
{
}

void CAgentNet::AfterReceived(void *buf, int length, char *pFromIP)
{
    if (length + roffset >= sizeof(MSG_Control))
    {
        int reccnt = sizeof(MSG_Control) - roffset;
        memcpy(msgbuf + roffset, buf, reccnt);
        _ASSERTE(_CrtCheckMemory());
        MSG_Control *pstatus = (MSG_Control *)msgbuf;

        ParseCtrl(*pstatus);

        /*roffset = length-reccnt;
        memcpy(msgbuf,((BYTE*)buf) + reccnt,roffset);*/
        roffset = 0;
    }
    else
    {
        memcpy(msgbuf + roffset, buf, length);
        _ASSERTE(_CrtCheckMemory());
        roffset += length;
    }
}

void CAgentNet::ParseCtrl(MSG_Control inCtrl)
{
    // 检验通讯key
    if (inCtrl.Key != m_nKey)
    {
        output("通讯Key不匹配！");
        return;
    }

    // 检验目标ID
    if (inCtrl.ID != m_nID)
    {
        output("ID不匹配！");
        return;
    }

    CString str;
    // 接收控制指令
    switch (inCtrl.ctrl)
    {
    case CTRL_REMOTE:
        // 		if (pVoy != NULL)
        // 		{
        // 			pVoy->SetBehavior(&m_idle);
        // 			pVoy->SendSpeed(inCtrl.lSpeed,inCtrl.rSpeed);
        // 		}
        str.Format("远程遥控状态（%d,%d,%2f）", inCtrl.lSpeed, inCtrl.rSpeed, inCtrl.speedProp);
        output(str);
        break;

    case CTRL_ATTACK:
        // 		if (pVoy !=NULL && AttackBeh != NULL)
        // 		{
        // 			pVoy->SetBehavior(AttackBeh);
        // 		}
        output("进入进攻状态");
        break;

    case CTRL_DEFENCE:
        // 		if (pVoy != NULL && DefBeh != NULL)
        // 		{
        // 			pVoy->SetBehavior(DefBeh);
        // 		}
        output("进入防守状态");
        break;
    }
}

void CAgentNet::SetID(UINT inID)
{
    m_nID = inID;
}
