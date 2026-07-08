// IBehavior.h: interface for the IBehavior class.
//
//////////////////////////////////////////////////////////////////////
/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.1       Date: 2007/10/19
  Email:  zwj_uptech@126.com
  Description:    // VoyagerII行为基类
  Others:         // 1.1 加入初始化函数Init()，每次加载时调用
*************************************************/

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
    virtual void Init(){};                                                             // 行为加载的时候初始化 1.1加入
    virtual void AfterUpdateInfrared(UCHAR *Infrared, BOOL *EnableIR, UINT nState){};  // 红外传感器信息更新后的处理函数
    virtual void AfterUpdateUSonic(DOUBLE *Ultrasonic, BOOL *EnableUS, UINT nState){}; // 超声传感器信息更新后的处理函数
    virtual void AfterUpdateVideoSample(BYTE *pBuffer, long lWidth, long lHeight, double dbTime,
                                        UINT nState){}; // 主前视摄像装置接收到一桢图象后的处理函数
    virtual void AfterUpdateOverlook(BYTE *pBuffer, long lWidth, long lHeight, double dbTime,
                                     UINT nState){};                          // 全局摄像装置接收到一桢图象后的处理函数
    virtual void AfterSendCommand(BYTE *pBuffer, int iLength, UINT nState){}; // 发送指令完毕后的处理函数
    virtual void AfterUpdateAttitude(FLOAT inAngle, int inXMagn, int inYMagn, FLOAT inXRoll,
                                     FLOAT inYRoll){}; // 姿态信息更新
    virtual void Pass(LPVOID pParam){};
    CVoyCmd *m_pCmd; // 指令类对象指针
};

#endif // !defined(AFX_IBEHAVIOR_H__B5768320_B81E_4929_98DB_11CB134E7E1F__INCLUDED_)
