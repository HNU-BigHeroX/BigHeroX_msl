// IBehavior.h: interface for the IBehavior class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IBEHAVIOR_H__B5768320_B81E_4929_98DB_11CB134E7E1F__INCLUDED_)
#define AFX_IBEHAVIOR_H__B5768320_B81E_4929_98DB_11CB134E7E1F__INCLUDED_

#include "VoyCmd.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVoyCmd;

class IBehavior
{
  public:
    IBehavior()
    {
        m_pCmd = NULL;
    };
    virtual ~IBehavior(){};
    void SetCmd(CVoyCmd *pCmd)
    {
        m_pCmd = pCmd;
    };                                                                                 // 设置指令类对象
    virtual void AfterUpdateInfrared(UCHAR *Infrared, BOOL *EnableIR, UINT nState){};  // 红外传感器信息更新后的处理函数
    virtual void AfterUpdateUSonic(DOUBLE *Ultrasonic, BOOL *EnableUS, UINT nState){}; // 超声传感器信息更新后的处理函数
    virtual void AfterUpdateVideoSample(BYTE *pBuffer, long lWidth, long lHeight, double dbTime,
                                        UINT nState){}; // 主前视摄像装置接收到一桢图象后的处理函数
    virtual void AfterUpdateOverlook(BYTE *pBuffer, long lWidth, long lHeight, double dbTime,
                                     UINT nState){};                          // 全局摄像装置接收到一桢图象后的处理函数
    virtual void AfterSendCommand(BYTE *pBuffer, int iLength, UINT nState){}; // 发送指令完毕后的处理函数
    virtual void AfterUpdateAttitude(FLOAT inAngle, FLOAT inXRoll, FLOAT inYRoll){}; // 姿态信息更新
    CVoyCmd *m_pCmd;                                                                 // 指令类对象指针
};

#endif // !defined(AFX_IBEHAVIOR_H__B5768320_B81E_4929_98DB_11CB134E7E1F__INCLUDED_)
