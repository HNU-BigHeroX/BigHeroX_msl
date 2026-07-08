// RobotPlayerAMC.h: interface for the CRobotPlayerAMC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROBOTPLAYERAMC_H__9A29BBB9_9266_4460_B52A_ACD22921F528__INCLUDED_)
#define AFX_ROBOTPLAYERAMC_H__9A29BBB9_9266_4460_B52A_ACD22921F528__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AMCMotion.h"
#include "VoyCmd.h"

class CRobotPlayerAMC
{
  public:
    void Kick(int inStrength);
    void SetFiveMotorsSpeed(short *inSpeed);
    void Brake();
    void BridgeOffAll();
    void BridgeOnAll();
    CRobotPlayerAMC();
    virtual ~CRobotPlayerAMC();

    CAMCMotion m_amc[MOTOR_NUM];
    CVoyCmd m_robot;
};

#endif // !defined(AFX_ROBOTPLAYERAMC_H__9A29BBB9_9266_4460_B52A_ACD22921F528__INCLUDED_)
