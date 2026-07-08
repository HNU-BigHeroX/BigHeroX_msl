// AgentUDP.cpp: implementation of the AgentUDP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AgentUDP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAgentUDP::CAgentUDP()
{
    memset(sendbuf, 0, AGPBUF);
    sendbuf[0] = 0x55;
    sendbuf[1] = 0xaa;

    m_nRecvIndex = 0;
    m_lastRecv = 0;
    m_bFrameStart = 0;
    m_nFrameLength = 13;

    // rec data
    rec_ID = 0;
    rec_ctrl = 0;
    re_ballx = 0;
    re_bally = 0;
    rec_targetx = 0;
    rec_targety = 0;
    rec_fromx = 0;
    rec_fromy = 0;
    rec_targetang = 0;
    rec_Def_Dist = 0;
    rec_Def_Ang = 0;
    rec_MoveSpd = 0;
    rec_bField = false;
    rec_init_x = 900;
    rec_init_y = 0;
    rec_init_angle = 0;
    rec_init_mode = 0;

    // to send
    toS_x = 100;
    toS_y = 0;
    toS_angle = 0;
    toS_status = CTRL_STOP;
    toS_foundball = false;
    toS_ballx = 0;
    toS_bally = 0;
    toS_V = 0;
    toS_V_Angle = 0;
    rec_Def_Ang = 0;
    toS_TargetX = 0;
    toS_TargetY = 0;
    toS_Kick = 0;

    for (int i = 0; i < 4; i++)
    {
        m_curIP[i] = 0; // 必须初始化为0,以供在received里判断是否未连接
    }

    for (i = 0; i < AG_OBST_NUM; i++)
    {
        m_obst[i].size = 0;
        rec_obst[i].size = 0;
    }

    for (i = 0; i < 5; i++)
    {
        m_tm[i].state = AG_ST_OFFLINE;
    }

    memset(rec_other, 0, 100);
}

CAgentUDP::~CAgentUDP()
{
}

void CAgentUDP::SetID(int inID)
{
    if (inID < 0 || inID > 6)
    {
        return;
    }

    sendbuf[3] = (unsigned char)inID;

    // 重置监听端口
    Listen(20090 + inID);
}

void CAgentUDP::Report(int inx, int iny, int inAng, int inSt, bool foundb, int inBx, int inBy)
{
    sendbuf[2] = 176;
    sendbuf[4] = MSL_INFO;
    m_Split2Bytes(&sendbuf[5], inx);   // x
    m_Split2Bytes(&sendbuf[7], iny);   // y
    m_Split2Bytes(&sendbuf[9], inAng); // ang
    sendbuf[11] = (unsigned char)inSt; // status
    if (foundb == true)                // foundball
    {
        sendbuf[11] |= 0x80;
    }
    else
    {
        sendbuf[11] &= 0x7f;
    }
    m_Split2Bytes(&sendbuf[12], inBx); // ballx
    m_Split2Bytes(&sendbuf[14], inBy); // bally

    m_Split2Bytes(&sendbuf[16], toS_V);       // V
    m_Split2Bytes(&sendbuf[18], toS_V_Angle); // V_Angle

    // kick
    sendbuf[20] = toS_Kick;
    if (toS_Kick > 0)
    {
        toS_Kick = 0;
    }

    m_Split2Bytes(&sendbuf[21], toS_TargetX); // target x
    m_Split2Bytes(&sendbuf[23], toS_TargetY); // target y

    // 障碍物信息
    m_PackInfo();

    sendbuf[175] = m_calsum(sendbuf, 175);

    Send((char *)sendbuf, sendbuf[2]);
}

void CAgentUDP::m_Split2Bytes(unsigned char *inTarg, unsigned int inSrc)
{
    if (inTarg == NULL)
    {
        return;
    }

    unsigned short temp = (unsigned short)0x0000ffff & inSrc;
    inTarg[1] = (unsigned char)temp & 0x00ff;

    temp >>= 8;

    inTarg[0] = (unsigned char)temp & 0x00ff;
}

unsigned char CAgentUDP::m_calsum(unsigned char *inBuf, int inlen)
{
    unsigned char sum = 0;
    for (int i = 0; i < inlen; i++)
    {
        sum += inBuf[i];
    }
    return sum;
}

void CAgentUDP::Received(void *inbuf, int inlen, SOCKADDR *inpSock)
{
    /*/////////////////////////////////////////////////////////////////////////
    info.Format("rec %d bytes: %s from ",inlen,inbuf);
    unsigned char* pch = (unsigned char*)inpSock->sa_data;
    WORD iPort=ntohs(*(WORD*)pch);
    CString ip;
    ip.Format("%d.%d.%d.%d: %d",pch[2],pch[3],pch[4],pch[5],iPort);
    info += ip;
    printf(info);
    /////////////////////////////////////////////////////////////////////////*/

    // 检查一下数据来源ip
    unsigned char *pch = (unsigned char *)inpSock->sa_data;
    if (m_curIP[0] != 0 || m_curIP[1] != 0 || m_curIP[2] != 0 || m_curIP[3] != 0)
    {
        // curip里已经有连接，检查一下数据来源ip是否与其相符
        if (m_curIP[0] != pch[2] || m_curIP[1] != pch[3] || m_curIP[2] != pch[4] || m_curIP[3] != pch[5])
        {
            // 数据来源与之前不相符，抛弃该数据
            return;
        }
    }
    else
    {
        // 这是第一次接收数据，将来源ip保存下来
        memcpy(m_curIP, pch + 2, 4);
    }

    // 解析接收到的数据
    pbuf = (unsigned char *)inbuf;
    for (int i = 0; i < inlen; i++)
    {
        m_ParseByte(pbuf[i]);
    }
}

void CAgentUDP::m_ParseByte(unsigned char inChar)
{
    m_ParseBuf[m_nRecvIndex] = inChar;

    if (m_lastRecv == 0x55 && inChar == 0xAA && m_bFrameStart == 0)
    {
        m_bFrameStart = 1;
        m_ParseBuf[0] = m_lastRecv;
        m_ParseBuf[1] = inChar;
        m_nRecvIndex = 2;
        m_lastRecv = 0x00;
        return;
    }

    if (m_bFrameStart)
    {
        if (m_nRecvIndex == 2)
        {
            m_nFrameLength = inChar;
        }

        // put received data into buffer
        m_ParseBuf[m_nRecvIndex] = inChar;
        m_nRecvIndex++;

        // receive one frame, invoke ParseFrame to parse
        if (m_nRecvIndex == m_nFrameLength)
        {
            // m_DispRecvCmd();
            m_ParseFrame(m_ParseBuf, m_nFrameLength);
            m_bFrameStart = 0;
        }

        // receive buffer overflow
        if (m_nRecvIndex >= AGPBUF)
        {
            // m_ResetRcvBuf();
            m_bFrameStart = 0;
        }
    }
    else
        m_lastRecv = inChar;
}

void CAgentUDP::m_ParseFrame(unsigned char *inBuf, int inLen)
{
    // 校验和检查
    unsigned char chk_sum = m_calsum(inBuf, inLen - 1);
    unsigned char rec_sum = inBuf[inLen - 1];
    if (chk_sum != rec_sum)
    {
        printf("校验和错误！");
        return;
    }

    // 解析
    rec_ID = inBuf[3];
    int i;
    switch (inBuf[4])
    {
    // 控制指令
    case MSL_CMD:
        rec_ctrl = inBuf[99];

        // 队友信息
        for (i = 0; i < 5; i++)
        {
            m_tm[i].ID = i + 1;
            m_tm[i].state = inBuf[5 + 7 * i];
            m_tm[i].x = m_Piece2int(&(inBuf[5 + 7 * i + 1]));
            m_tm[i].y = m_Piece2int(&(inBuf[5 + 7 * i + 3]));
            m_tm[i].angle = m_Piece2int(&(inBuf[5 + 7 * i + 5]));
        }

        // 障碍物信息
        for (i = 0; i < 10; i++)
        {
            rec_obst[i].size = inBuf[40 + 5 * i];
            rec_obst[i].x = m_Piece2int(&(inBuf[40 + 5 * i + 1]));
            rec_obst[i].y = m_Piece2int(&(inBuf[40 + 5 * i + 3]));
        }

        // 初始位置坐标
        rec_init_x = m_Piece2int(&(inBuf[90]));
        rec_init_y = m_Piece2int(&(inBuf[92]));

        // 球坐标
        re_ballx = m_Piece2int(&(inBuf[95]));
        re_bally = m_Piece2int(&(inBuf[97]));

        switch (rec_ctrl)
        {
        case CTRL_MOVETO:
        case CTRL_ZONEDEF:
            rec_targetx = m_Piece2int(&(inBuf[100]));
            rec_targety = m_Piece2int(&(inBuf[102]));
            rec_targetang = m_Piece2int(&(inBuf[104]));
            rec_MoveSpd = (UCHAR)inBuf[106];
            break;

        case CTRL_FOLLOW:
            rec_CFtargetx = m_Piece2int(&(inBuf[100]));
            rec_CFtargety = m_Piece2int(&(inBuf[102]));
            rec_CFtargetang = m_Piece2int(&(inBuf[104]));
            rec_CFMoveSpd = (UCHAR)inBuf[106];
            break;
        case CTRL_DEF_BALL:
            rec_CDBtargetx = m_Piece2int(&(inBuf[100]));
            rec_CDBtargety = m_Piece2int(&(inBuf[102]));
            rec_CDBtargetang = m_Piece2int(&(inBuf[104]));
            rec_CDBMoveSpd = (UCHAR)inBuf[106];
            break;
        case CTRL_DEF_GOAL:
            rec_CDGtargetx = m_Piece2int(&(inBuf[100]));
            rec_CDGtargety = m_Piece2int(&(inBuf[102]));
            rec_CDGtargetang = m_Piece2int(&(inBuf[104]));
            rec_CDGMoveSpd = (UCHAR)inBuf[106];
            break;

        case CTRL_DEFENCE:
            rec_Def_Ang = m_Piece2int(&(inBuf[100]));
            rec_Def_Dist = m_Piece2int(&(inBuf[102]));
            break;

        case CTRL_PASS:
        case CTRL_PASS_MOVE:
        case CTRL_CATCH_MOVE:
            rec_targetx = m_Piece2int(&(inBuf[100]));
            rec_targety = m_Piece2int(&(inBuf[102]));
            break;

        case CTRL_CATCH:
            rec_fromx = m_Piece2int(&(inBuf[100]));
            rec_fromy = m_Piece2int(&(inBuf[102]));
            break;

        case ROBST_PROBE:
            // 如果未连接任何目标，则响应这个来源
            if (bConnected == false)
            {
                CString coachip;
                coachip.Format("%d.%d.%d.%d", m_curIP[0], m_curIP[1], m_curIP[2], m_curIP[3]);
                SetTarget(coachip, 20090);
                // 设置目标之后，发送一帧数据
                Report(sendbuf[2] * 150, -70, 90, CTRL_STOP, false, 0, 0);
            }
            break;

        default:
            break;
        }

        // 待用数据
        switch (rec_ctrl)
        {
        case CTRL_MOVETO:
        case CTRL_ZONEDEF:
        case CTRL_FOLLOW:
        case CTRL_DEF_BALL:
        case CTRL_DEF_GOAL:
            memcpy(rec_other, &(inBuf[107]), 100);
            break;

        default:
            memcpy(rec_other, &(inBuf[104]), 100);
            break;
        }

        break;
    }

#ifdef TECHCHALLENGE2013_1
    re_ballx = 1800 - re_ballx;
    re_bally = 1200 - re_bally;
    rec_targetx = 1800 - rec_targetx;
    rec_targety = 1200 - rec_targety;
    rec_targetang = angle_add_transfer2(rec_targetang, 180);
#endif
}

int CAgentUDP::m_Piece2int(unsigned char *inTarg)
{
    short ret;
    ret = inTarg[0];
    ret <<= 8;
    ret &= 0xff00;
    ret |= ((int)inTarg[1] & 0x00ff);
    return ret;
}

void CAgentUDP::AddObst(int inX, int inY, int inSize)
{
    for (int i = 0; i < AG_OBST_NUM; i++)
    {
        if (m_obst[i].valid == false)
        {
            m_obst[i].ID = sendbuf[3];
            m_obst[i].x = inX;
            m_obst[i].y = inY;
            m_obst[i].size = inSize;
            m_obst[i].valid = true;
            break;
        }
    }
}

void CAgentUDP::Clear_Obst()
{
    for (int i = 0; i < AG_OBST_NUM; i++)
    {
        m_obst[i].valid = false;
        m_obst[i].size = 0;
    }
}

void CAgentUDP::ClearRecvObst()
{
    for (int i = 0; i < AG_OBST_NUM; i++)
    {
        rec_obst[i].valid = false;
    }
}

void CAgentUDP::ClearRecvTM()
{
    for (int i = 0; i < AG_OBST_NUM; i++)
    {
        m_tm[i].state = AG_ST_OFFLINE;
    }
}

void CAgentUDP::AttentionKick(int inSelfX, int inSelfY, int inTargetX, int inTargetY)
{
    toS_TargetX = inTargetX;
    toS_TargetY = inTargetY;
    toS_Kick = 10; // 1;
}

void CAgentUDP::SetCoachIP(CString inStr)
{
    CString tmp;
    CString strIP = inStr;
    int pos = 1;
    for (int i = 0; i < 3; i++)
    {
        pos = strIP.Find(".", 1);
        if (pos == -1)
        {
            return;
        }
        tmp = strIP.Left(pos);
        m_curIP[i] = atoi(tmp);

        pos = strIP.GetLength() - pos - 1;
        strIP = strIP.Right(pos);
    }

    m_curIP[3] = atoi(strIP);
}

void CAgentUDP::m_PackInfo()
{
    for (int i = 0; i < AG_OBST_NUM; i++)
    {
        sendbuf[25 + 5 * i] = m_obst[i].size;
        m_Split2Bytes(&(sendbuf[25 + 5 * i + 1]), m_obst[i].x);
        m_Split2Bytes(&(sendbuf[25 + 5 * i + 3]), m_obst[i].y);
    }
}

void CAgentUDP::Report()
{
#ifdef TECHCHALLENGE2013_1
    toS_x = 1800 - toS_x;
    toS_y = 1200 - toS_y;
    toS_angle = angle_add_transfer2(toS_angle, 180);
    toS_ballx = 1800 - toS_ballx;
    toS_bally = 1200 - toS_bally;
#endif

    sendbuf[2] = 176;
    sendbuf[4] = MSL_INFO;
    m_Split2Bytes(&sendbuf[5], toS_x);       // x
    m_Split2Bytes(&sendbuf[7], toS_y);       // y
    m_Split2Bytes(&sendbuf[9], toS_angle);   // ang
    sendbuf[11] = (unsigned char)toS_status; // status
    if (toS_foundball == true)               // foundball
    {
        sendbuf[11] |= 0x80;
    }
    else
    {
        sendbuf[11] &= 0x7f;
    }
    m_Split2Bytes(&sendbuf[12], toS_ballx); // ballx
    m_Split2Bytes(&sendbuf[14], toS_bally); // bally

    m_Split2Bytes(&sendbuf[16], toS_V);       // V
    m_Split2Bytes(&sendbuf[18], toS_V_Angle); // V_Angle

    // kick
    sendbuf[20] = toS_Kick;
    if (toS_Kick > 0)
    {
        //		toS_Kick = 0;
        toS_Kick--;
    }

    m_Split2Bytes(&sendbuf[21], toS_TargetX); // target x
    m_Split2Bytes(&sendbuf[23], toS_TargetY); // target y

// 障碍物信息
#ifndef TECHCHALLENGE2013_1
    m_PackInfo();
#endif

    sendbuf[175] = m_calsum(sendbuf, 175);

    Send((char *)sendbuf, sendbuf[2]);
}

void CAgentUDP::ReportNew()
{
    sendbuf[2] = 176;
    sendbuf[4] = MSL_INFO;
    m_Split2Bytes(&sendbuf[5], toS_x);     // x
    m_Split2Bytes(&sendbuf[7], toS_y);     // y
    m_Split2Bytes(&sendbuf[9], toS_angle); // ang
    sendbuf[11] = ROBST_ERR;               //(unsigned char)toS_status;//status
    if (toS_foundball == true)             // foundball
    {
        sendbuf[11] |= 0x80;
    }
    else
    {
        sendbuf[11] &= 0x7f;
    }
    m_Split2Bytes(&sendbuf[12], toS_ballx); // ballx
    m_Split2Bytes(&sendbuf[14], toS_bally); // bally

    m_Split2Bytes(&sendbuf[16], toS_V);       // V
    m_Split2Bytes(&sendbuf[18], toS_V_Angle); // V_Angle

    // kick
    sendbuf[20] = toS_Kick;
    if (toS_Kick > 0)
    {
        //		toS_Kick = 0;
        toS_Kick--;
    }

    m_Split2Bytes(&sendbuf[21], toS_TargetX); // target x
    m_Split2Bytes(&sendbuf[23], toS_TargetY); // target y

    // 障碍物信息
    m_PackInfo();

    sendbuf[175] = m_calsum(sendbuf, 175);

    Send((char *)sendbuf, sendbuf[2]);
}

void CAgentUDP::m_Float2Bytes(unsigned char *inTarg, float inVal)
{
    if (inTarg == NULL)
    {
        return;
    }

    unsigned char *pVal = (unsigned char *)&inVal;
    memcpy(inTarg, pVal, sizeof(float));
}

void CAgentUDP::SetBatteryStatus(BYTE inACLineStatus, BYTE inBatteryFlag, BYTE inBatteryLifePercent,
                                 DWORD inBatteryLifeTime)
{
    sendbuf[75] = inACLineStatus;
    sendbuf[76] = inBatteryFlag;
    sendbuf[77] = inBatteryLifePercent;
    m_Split2Bytes(&sendbuf[78], inBatteryLifeTime);
}

void CAgentUDP::SetCPUStatus(int inCPULoad, float inCPUFrequency)
{
    if (inCPULoad < 0)
    {
        inCPULoad = 0;
    }
    if (inCPULoad > 255)
    {
        inCPULoad = 255;
    }

    sendbuf[80] = (BYTE)inCPULoad;
    m_Float2Bytes(&sendbuf[81], inCPUFrequency);
}

void CAgentUDP::SetSoftwareVer(float inVer)
{
    m_Float2Bytes(&sendbuf[85], inVer);
}

void CAgentUDP::SetRobotStatus(BYTE inRobotPower, BYTE inRobotCharge)
{
    sendbuf[89] = inRobotPower;
    sendbuf[90] = inRobotCharge;
}

void CAgentUDP::SetMulticastFPS(int inFPS)
{
    if (inFPS < 0)
    {
        inFPS = 0;
    }
    if (inFPS > 255)
    {
        inFPS = 255;
    }
    sendbuf[93] = (BYTE)inFPS;
}

void CAgentUDP::SetDestToS(int inX, int inY)
{
    toS_TargetX = inX;
    toS_TargetY = inY;
}

void CAgentUDP::SetSerialPortFPS(int inFPS)
{
    if (inFPS < 0)
    {
        inFPS = 0;
    }
    if (inFPS > 255)
    {
        inFPS = 255;
    }
    sendbuf[91] = (BYTE)inFPS;
}

void CAgentUDP::SetVisionFPS(int inFPS)
{
    if (inFPS < 0)
    {
        inFPS = 0;
    }
    if (inFPS > 255)
    {
        inFPS = 255;
    }
    sendbuf[92] = (BYTE)inFPS;
}

void CAgentUDP::SetAttitudeFPS(int inFPS)
{
    if (inFPS < 0)
    {
        inFPS = 0;
    }
    if (inFPS > 255)
    {
        inFPS = 255;
    }
    sendbuf[94] = (BYTE)inFPS;
}
