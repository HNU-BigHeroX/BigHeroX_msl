// RobotPlayerAMC.cpp: implementation of the CRobotPlayerAMC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RobotPlayerAMC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

static nDefAddr = 63;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRobotPlayerAMC::CRobotPlayerAMC()
{
}

CRobotPlayerAMC::~CRobotPlayerAMC()
{
}

void CRobotPlayerAMC::BridgeOnAll()
{
    for (int i = 0; i < MOTOR_NUM; i++)
    {
        m_amc[i].SingleGWA(nDefAddr);
        Sleep(200);
        m_amc[i].SingleBridge(nDefAddr, true);
        Sleep(100);
    }
}

void CRobotPlayerAMC::BridgeOffAll()
{
    for (int i = 0; i < MOTOR_NUM; i++)
    {
        m_amc[i].SingleBridge(nDefAddr, false);
        Sleep(200);
    }
}

void CRobotPlayerAMC::Brake()
{
    for (int i = 0; i < MOTOR_NUM; i++)
    {
        m_amc[i].AMCSetVelocity(0);
    }
}

void CRobotPlayerAMC::SetFiveMotorsSpeed(short *inSpeed)
{
    for (int i = 0; i < MOTOR_NUM; i++)
    {
        m_amc[i].AMCSetVelocity(-inSpeed[i]);
    }
}

void CRobotPlayerAMC::Kick(int inStrength)
{
    m_robot.Kick(inStrength);
}
