// IntPID.cpp: implementation of the CIntPID class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IntPID.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIntPID::CIntPID()
{
    m_nPreError = 0;
    IMax = IMin = 0;
    DeadlineMinus = DeadlinePlus = 0;
    Kd = Ki = Kp = 0;
    m_nIntegral = 0;
    resMax = 1000;
    resMin = -1000;
}

CIntPID::~CIntPID()
{
}

int CIntPID::Calc(int inerror, UINT flag)
{
    // 死区
    if (inerror < DeadlinePlus && inerror > DeadlineMinus)
    {
        m_nIntegral = 0;
        return 0;
    }

    // 微分误差
    int derror = inerror - m_nPreError;

    switch (flag)
    {
    case 0:
        // 方向切换的时候积分项清零，以防超调过度
        if (inerror > 0 && m_nIntegral < 0)
        {
            m_nIntegral = 0;
        }

        if (inerror < 0 && m_nIntegral > 0)
        {
            m_nIntegral = 0;
        }
        break;

    default:
        break;
    }

    // 积分
    if (m_nIntegral < IMax && m_nIntegral > IMin)
    {
        m_nIntegral += inerror;
    }
    else
    {
        if (m_nIntegral > IMax)
        {
            m_nIntegral = IMax - 1;
        }
        else
        {
            m_nIntegral = IMin + 1;
        }
    }

    // 位置PID算法
    int result = Kp * inerror + Ki * m_nIntegral + Kd * derror;

    result >>= 10;

    if (result < resMin)
    {
        result = resMin;
    }

    if (result > resMax)
    {
        result = resMax;
    }

    m_nPreError = inerror;

    return result;
}
