// ToAgent.cpp: implementation of the CToAgent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToAgent.h"
#include "MSL_Structs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToAgent::CToAgent()
{
    m_sendbuf = new unsigned char[1024];
    memset(m_sendbuf, 0, 1024);
    m_sendbuf[0] = 0x55;
    m_sendbuf[1] = 0xaa;

    for (int i = 0; i < TO_AG_OB_NUM; i++)
    {
        m_cObstToSend[i].valid = false;
    }

    for (i = 0; i < 5; i++)
    {
        m_cTeamMate[i].ID = i + 1;
        m_cTeamMate[i].state = AG_ST_OFFLINE;
        m_cTeamMate[i].x = -1;
        m_cTeamMate[i].y = -1;
        m_cTeamMate[i].angle = 0;
    }

    m_bInitPos = false;

    m_nInit_x = 900;
    m_nInit_y = 0;
    m_nInit_angle = 0;

    bFoundBall = false;
    m_nBall_x = 0;
    m_nBall_y = 0;

    memset(toA_other, 0, 100);

    bLogMode = false;
}

CToAgent::~CToAgent()
{
    delete[] m_sendbuf;
}

void CToAgent::SetID(int inID)
{
    m_sendbuf[3] = (unsigned char)inID;
}

void CToAgent::CtrlCmd(int inCtrl, int inReBx, int inReBy, int inAng, int inDist, bool inbField)
{
    m_sendbuf[2] = 205;
    m_sendbuf[4] = MSL_CMD;

    m_nBall_x = inReBx;
    m_nBall_y = inReBy;

    m_PackInfo();

    m_sendbuf[99] = (unsigned char)inCtrl;    // ctrl
    m_Split2Bytes(&(m_sendbuf[100]), inAng);  // angle
    m_Split2Bytes(&(m_sendbuf[102]), inDist); // dist

    memcpy(&(m_sendbuf[104]), toA_other, 100);
    m_sendbuf[204] = m_calsum(m_sendbuf, 204); // sum

    Send((char *)m_sendbuf, m_sendbuf[2]);
    // 记录模式下，发送一份给记录程序
    if (bLogMode == true)
    {
        m_LogUdpSender.Send((char *)m_sendbuf, m_sendbuf[2]);
    }
}

int CToAgent::m_Piece2int(unsigned char *inTarg)
{
    short ret;
    ret = inTarg[0];
    ret <<= 8;
    ret &= 0xff00;
    ret |= ((int)inTarg[1] & 0x00ff);
    return ret;
}

void CToAgent::m_Split2Bytes(unsigned char *inTarg, unsigned int inSrc)
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

unsigned char CToAgent::m_calsum(unsigned char *inBuf, int inlen)
{
    unsigned char sum = 0;
    for (int i = 0; i < inlen; i++)
    {
        sum += inBuf[i];
    }
    return sum;
}

void CToAgent::MoveCmd(int inTargetx, int inTargety, int inTargetAng, int inDist, bool inbField)
{
    // 将角度限制在0～360里
    while (inTargetAng < 0)
    {
        inTargetAng += 360;
    }

    while (inTargetAng > 360)
    {
        inTargetAng -= 360;
    }

    if (inDist < 0)
    {
        inDist = -inDist;
    }

    m_sendbuf[2] = 208; // len
    m_sendbuf[4] = MSL_CMD;

    m_PackInfo();

    m_sendbuf[99] = CTRL_MOVETO;                   // ctrl
    m_Split2Bytes(&(m_sendbuf[100]), inTargetx);   // x
    m_Split2Bytes(&(m_sendbuf[102]), inTargety);   // y
    m_Split2Bytes(&(m_sendbuf[104]), inTargetAng); // angle
    m_sendbuf[106] = 100;

    memcpy(&(m_sendbuf[107]), toA_other, 100);
    m_sendbuf[207] = m_calsum(m_sendbuf, 207); // sum

    Send((char *)m_sendbuf, m_sendbuf[2]);
    // 记录模式下，发送一份给记录程序
    if (bLogMode == true)
    {
        m_LogUdpSender.Send((char *)m_sendbuf, m_sendbuf[2]);
    }
}

void CToAgent::SetRefBall(int inBallx, int inBally)
{
    m_nBall_x = inBallx;
    m_nBall_y = inBally;
}

void CToAgent::DefCmd(int inAngle, int inDist)
{
    // 将角度限制在0～360里
    while (inAngle < 0)
    {
        inAngle += 360;
    }

    while (inAngle > 360)
    {
        inAngle -= 360;
    }

    if (inDist < 0)
    {
        inDist = -inDist;
    }

    m_sendbuf[2] = 205; // len
    m_sendbuf[4] = MSL_CMD;

    m_PackInfo();

    m_sendbuf[99] = CTRL_DEFENCE;              // ctrl
    m_Split2Bytes(&(m_sendbuf[100]), inAngle); // ang
    m_Split2Bytes(&(m_sendbuf[102]), inDist);  // dist

    memcpy(&(m_sendbuf[104]), toA_other, 100);
    m_sendbuf[204] = m_calsum(m_sendbuf, 204); // sum

    Send((char *)m_sendbuf, m_sendbuf[2]);
    // 记录模式下，发送一份给记录程序
    if (bLogMode == true)
    {
        m_LogUdpSender.Send((char *)m_sendbuf, m_sendbuf[2]);
    }
}

void CToAgent::AddObst(int inX, int inY, int inSize)
{
    for (int i = 0; i < TO_AG_OB_NUM; i++)
    {
        if (m_cObstToSend[i].valid == false)
        {
            m_cObstToSend[i].ID = m_sendbuf[3];
            m_cObstToSend[i].x = inX;
            m_cObstToSend[i].y = inY;
            m_cObstToSend[i].size = inSize;
            m_cObstToSend[i].valid = true;
            break;
        }
    }
}

void CToAgent::Clear_Obst()
{
    for (int i = 0; i < TO_AG_OB_NUM; i++)
    {
        m_cObstToSend[i].valid = false;
        m_cObstToSend[i].size = 0;
    }
}

void CToAgent::MoveTo(int inTargetx, int inTargety, int inTargetAng, int inDist, bool inbField, int inSpd)
{
    // 将角度限制在0～360里
    while (inTargetAng < 0)
    {
        inTargetAng += 360;
    }

    while (inTargetAng > 360)
    {
        inTargetAng -= 360;
    }

    if (inDist < 0)
    {
        inDist = -inDist;
    }

    m_sendbuf[2] = 208; // len
    m_sendbuf[4] = MSL_CMD;

    m_PackInfo();

    m_sendbuf[99] = CTRL_MOVETO;                   // ctrl
    m_Split2Bytes(&(m_sendbuf[100]), inTargetx);   // x
    m_Split2Bytes(&(m_sendbuf[102]), inTargety);   // y
    m_Split2Bytes(&(m_sendbuf[104]), inTargetAng); // angle
    m_sendbuf[106] = (UCHAR)inSpd;

    memcpy(&(m_sendbuf[107]), toA_other, 100);
    m_sendbuf[207] = m_calsum(m_sendbuf, 207); // sum

    Send((char *)m_sendbuf, m_sendbuf[2]);
    // 记录模式下，发送一份给记录程序
    if (bLogMode == true)
    {
        m_LogUdpSender.Send((char *)m_sendbuf, m_sendbuf[2]);
    }
}

void CToAgent::PassCmd(int inTargX, int inTargY)
{
    m_sendbuf[2] = 205; // len
    m_sendbuf[4] = MSL_CMD;

    m_PackInfo();

    m_sendbuf[99] = CTRL_PASS;                 // ctrl
    m_Split2Bytes(&(m_sendbuf[100]), inTargX); // tx
    m_Split2Bytes(&(m_sendbuf[102]), inTargY); // ty

    memcpy(&(m_sendbuf[104]), toA_other, 100);
    m_sendbuf[204] = m_calsum(m_sendbuf, 204); // sum
    Send((char *)m_sendbuf, m_sendbuf[2]);
    // 记录模式下，发送一份给记录程序
    if (bLogMode == true)
    {
        m_LogUdpSender.Send((char *)m_sendbuf, m_sendbuf[2]);
    }
}

void CToAgent::CatchCmd(int inCatchX, int inCatchY)
{
    m_sendbuf[2] = 205; // len
    m_sendbuf[4] = MSL_CMD;

    m_PackInfo();

    m_sendbuf[99] = CTRL_CATCH;                 // ctrl
    m_Split2Bytes(&(m_sendbuf[100]), inCatchX); // fx
    m_Split2Bytes(&(m_sendbuf[102]), inCatchY); // fy

    memcpy(&(m_sendbuf[104]), toA_other, 100);
    m_sendbuf[204] = m_calsum(m_sendbuf, 204); // sum
    Send((char *)m_sendbuf, m_sendbuf[2]);
    // 记录模式下，发送一份给记录程序
    if (bLogMode == true)
    {
        m_LogUdpSender.Send((char *)m_sendbuf, m_sendbuf[2]);
    }
}

void CToAgent::SetInitPos(int inx, int iny, int inangle)
{
    m_nInit_x = inx;
    m_nInit_y = iny;
    m_nInit_angle = inangle;
    m_bInitPos = true;
}

void CToAgent::ZoneDefToCmd(int inTargetx, int inTargety, int inTargetAng, int inDist, bool inbField, int inSpd)
{
    // 将角度限制在0～360里
    while (inTargetAng < 0)
    {
        inTargetAng += 360;
    }

    while (inTargetAng > 360)
    {
        inTargetAng -= 360;
    }

    if (inDist < 0)
    {
        inDist = -inDist;
    }

    m_sendbuf[2] = 208; // len
    m_sendbuf[4] = MSL_CMD;

    m_PackInfo();

    m_sendbuf[99] = CTRL_ZONEDEF;                  // ctrl
    m_Split2Bytes(&(m_sendbuf[100]), inTargetx);   // x
    m_Split2Bytes(&(m_sendbuf[102]), inTargety);   // y
    m_Split2Bytes(&(m_sendbuf[104]), inTargetAng); // angle
    m_sendbuf[106] = (UCHAR)inSpd;

    memcpy(&(m_sendbuf[107]), toA_other, 100);
    m_sendbuf[207] = m_calsum(m_sendbuf, 207); // sum
    Send((char *)m_sendbuf, m_sendbuf[2]);
    // 记录模式下，发送一份给记录程序
    if (bLogMode == true)
    {
        m_LogUdpSender.Send((char *)m_sendbuf, m_sendbuf[2]);
    }
}

void CToAgent::SetTeammate(int inID, int inState, int inX, int inY, int inAngle)
{
    if (inID > 0 && inID <= 5)
    {
        m_cTeamMate[inID - 1].ID = inID;
        m_cTeamMate[inID - 1].state = inState;
        m_cTeamMate[inID - 1].x = inX;
        m_cTeamMate[inID - 1].y = inY;
        m_cTeamMate[inID - 1].angle = inAngle;
    }
}

void CToAgent::Clear_Teammate()
{
    for (int i = 0; i < 5; i++)
    {
        m_cTeamMate[i].state = AG_ST_OFFLINE;
    }
}

void CToAgent::m_PackInfo()
{
    // 打包队员信息
    for (int i = 0; i < 5; i++)
    {
        m_sendbuf[5 + 7 * i] = m_cTeamMate[i].state;
        m_Split2Bytes(&(m_sendbuf[5 + 7 * i + 1]), m_cTeamMate[i].x);
        m_Split2Bytes(&(m_sendbuf[5 + 7 * i + 3]), m_cTeamMate[i].y);
        m_Split2Bytes(&(m_sendbuf[5 + 7 * i + 5]), m_cTeamMate[i].angle);
    }

    // 打包障碍物信息
    for (i = 0; i < 10; i++)
    {
        m_sendbuf[40 + 5 * i] = m_cObstToSend[i].size;
        m_Split2Bytes(&(m_sendbuf[40 + 5 * i + 1]), m_cObstToSend[i].x);
        m_Split2Bytes(&(m_sendbuf[40 + 5 * i + 3]), m_cObstToSend[i].y);
    }

    // 设置初始坐标位置
    m_Split2Bytes(&(m_sendbuf[90]), m_nInit_x);
    m_Split2Bytes(&(m_sendbuf[92]), m_nInit_y);

    // 球的坐标
    m_sendbuf[94] = bFoundBall;
    m_Split2Bytes(&(m_sendbuf[95]), m_nBall_x);
    m_Split2Bytes(&(m_sendbuf[97]), m_nBall_y);
}

void CToAgent::Follow(int inTargetx, int inTargety, int inDist)
{

    if (inDist < 0)
    {
        inDist = -inDist;
    }

    m_sendbuf[2] = 208; // len
    m_sendbuf[4] = MSL_CMD;

    m_PackInfo();

    m_sendbuf[99] = CTRL_FOLLOW;                 // ctrl
    m_Split2Bytes(&(m_sendbuf[100]), inTargetx); // x
    m_Split2Bytes(&(m_sendbuf[102]), inTargety); // y
    m_Split2Bytes(&(m_sendbuf[104]), inDist);    // distance
    m_sendbuf[106] = 100;

    memcpy(&(m_sendbuf[107]), toA_other, 100);
    m_sendbuf[207] = m_calsum(m_sendbuf, 207); // sum
    Send((char *)m_sendbuf, m_sendbuf[2]);
    // 记录模式下，发送一份给记录程序
    if (bLogMode == true)
    {
        m_LogUdpSender.Send((char *)m_sendbuf, m_sendbuf[2]);
    }
}

void CToAgent::DEF_BALL(int inTargetx, int inTargety, int inDist)
{
    if (inDist < 0)
    {
        inDist = -inDist;
    }

    m_sendbuf[2] = 208; // len
    m_sendbuf[4] = MSL_CMD;

    m_PackInfo();

    m_sendbuf[99] = CTRL_DEF_BALL;               // ctrl
    m_Split2Bytes(&(m_sendbuf[100]), inTargetx); // x
    m_Split2Bytes(&(m_sendbuf[102]), inTargety); // y
    m_Split2Bytes(&(m_sendbuf[104]), inDist);    // distance
    m_sendbuf[106] = 100;

    memcpy(&(m_sendbuf[107]), toA_other, 100);
    m_sendbuf[207] = m_calsum(m_sendbuf, 207); // sum
    Send((char *)m_sendbuf, m_sendbuf[2]);
    // 记录模式下，发送一份给记录程序
    if (bLogMode == true)
    {
        m_LogUdpSender.Send((char *)m_sendbuf, m_sendbuf[2]);
    }
}

void CToAgent::DEF_GOAL(int inTargetx, int inTargety, int inDist)
{
    if (inDist < 0)
    {
        inDist = -inDist;
    }

    m_sendbuf[2] = 208; // len
    m_sendbuf[4] = MSL_CMD;

    m_PackInfo();

    m_sendbuf[99] = CTRL_DEF_GOAL;               // ctrl
    m_Split2Bytes(&(m_sendbuf[100]), inTargetx); // x
    m_Split2Bytes(&(m_sendbuf[102]), inTargety); // y
    m_Split2Bytes(&(m_sendbuf[104]), inDist);    // distance
    m_sendbuf[106] = 100;

    memcpy(&(m_sendbuf[107]), toA_other, 100);
    m_sendbuf[207] = m_calsum(m_sendbuf, 207); // sum
    Send((char *)m_sendbuf, m_sendbuf[2]);
    // 记录模式下，发送一份给记录程序
    if (bLogMode == true)
    {
        m_LogUdpSender.Send((char *)m_sendbuf, m_sendbuf[2]);
    }
}

void CToAgent::SimpleCtrl(int inCtrl)
{
    m_sendbuf[2] = 205; // len
    m_sendbuf[4] = MSL_CMD;

    m_PackInfo();

    m_sendbuf[99] = inCtrl;              // ctrl
    m_Split2Bytes(&(m_sendbuf[100]), 0); // fx
    m_Split2Bytes(&(m_sendbuf[102]), 0); // fy

    memcpy(&(m_sendbuf[104]), toA_other, 100);
    m_sendbuf[204] = m_calsum(m_sendbuf, 204); // sum
    Send((char *)m_sendbuf, m_sendbuf[2]);
    // 记录模式下，发送一份给记录程序
    if (bLogMode == true)
    {
        m_LogUdpSender.Send((char *)m_sendbuf, m_sendbuf[2]);
    }
}
