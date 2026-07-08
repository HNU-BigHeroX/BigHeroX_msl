/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.0       Date: 2006/5/12
  Email:  zwj_uptech@126.com
  Description:    // 串口
  Others:         //
*************************************************/
// SerialCom.h: interface for the CSerialCom class.
// For Vorager 2.0
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIALCOM_H__78D38E89_1031_495D_8967_84842B40DF86__INCLUDED_)
#define AFX_SERIALCOM_H__78D38E89_1031_495D_8967_84842B40DF86__INCLUDED_

#include "IPhy.h"
#include "VoyCmd.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 发送指令的缓冲列表成员
struct CMDBUF
{
    UCHAR *pCmdBuf; // 指令数组指针
    UINT nLen;      // 指令长度
};

class CSerialCom : public IPhy
{
  public:
    void SetBaudRate(int inBaud);
    BOOL Create(int inCom);                               // 打开一个串口端口
    void Close();                                         // 关闭当前打开的端口
    void SetCmd(CVoyCmd *pCmd);                           // 设置指令类对象（标准接口）
    void Send(const void *pBuffer, const int iLength);    // 通过缓冲列表进行指令发送（标准接口）
    void ComSend(const void *pBuffer, const int iLength); // 直接发送指令
    CSerialCom();
    virtual ~CSerialCom();

    CPtrList m_cmdlist; // 发送指令的缓冲列表
    CMDBUF *m_pTempCmd; // 发送指令缓冲成员临时指针
    HANDLE m_hCom;      // 已打开的串口句柄

    CVoyCmd *m_pCmd; // 指令类对象指针

  protected:
    // 串口属性
    int m_com;
    int m_baudrate;
    BYTE m_bytesize;
    BYTE m_parity;
    BYTE m_stopbits;

  private:
};

#endif // !defined(AFX_SERIALCOM_H__78D38E89_1031_495D_8967_84842B40DF86__INCLUDED_)
