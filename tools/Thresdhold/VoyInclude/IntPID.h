// IntPID.h: interface for the CIntPID class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTPID_H__25366243_23AE_40DB_A065_21D5A1956A8D__INCLUDED_)
#define AFX_INTPID_H__25366243_23AE_40DB_A065_21D5A1956A8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIntPID
{
  public:
    int Calc(int error, UINT flag);

    // 输入误差的死区范围
    int DeadlinePlus;
    int DeadlineMinus;

    // 积分范围
    int IMax;
    int IMin;

    // 参数
    int Kp;
    int Ki;
    int Kd;

    // 调节范围
    int resMax;
    int resMin;
    int m_nIntegral;

    CIntPID();
    virtual ~CIntPID();

  protected:
    int m_nPreError;
    // int m_nError;
    // int m_nIntegral;
};

#endif // !defined(AFX_INTPID_H__25366243_23AE_40DB_A065_21D5A1956A8D__INCLUDED_)
