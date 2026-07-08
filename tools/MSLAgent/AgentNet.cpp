// AgentNet.cpp: implementation of the CAgentNet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MSLAgent.h"
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
}

CAgentNet::~CAgentNet()
{
}

void CAgentNet::Received(void *inbuf, int inlen)
{
    if (inlen + roffset >= sizeof(MSG_Control))
    {
        int reccnt = sizeof(MSG_Control) - roffset;
        memcpy(msgbuf + roffset, inbuf, reccnt);
        MSG_Control *pstatus = (MSG_Control *)msgbuf;

        ParseCtrl(*pstatus);

        /*roffset = length-reccnt;
        memcpy(msgbuf,((BYTE*)buf) + reccnt,roffset);*/
        roffset = 0;
    }
    else
    {
        memcpy(msgbuf + roffset, inbuf, inlen);
        roffset += inlen;
    }
}

void CAgentNet::ParseCtrl(MSG_Control inCtrl)
{
    memcpy(&m_ctrl, &inCtrl, sizeof(MSG_Control));

    CString info;
    // 接收控制指令
    switch (inCtrl.ctrl)
    {
    case CTRL_STOP:
        printf("进入遥控状态");
        break;

    case CTRL_ATTACK:
        printf("进入进攻状态");
        break;

    case CTRL_GOALKEEP:
        printf("进入守门状态");
        break;

    case CTRL_MOVETO:
        info.Format("Moveto (%d,%d)", inCtrl.target_x, inCtrl.target_y);
        printf(info);
        break;

        /*/黄方行为
        case CTRL_KOFBLUE:
            break;

        case CTRL_ATKBLUE:
            break;

        case CTRL_DEFYELL:
            break;

        case CTRL_GUARDYEL:
            break;

        //蓝方行为
        case CTRL_KOFYELL:
            break;

        case CTRL_ATKYELL:
            break;

        case CTRL_DEFBLUE:

            break;

        case CTRL_GUARDBLUE:
            break;*/
    }
}
