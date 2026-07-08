/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.0       Date: 2009/4/17
  Email:  zwj_uptech@126.com
  Description:    // UDP通讯
  Others:         //
*************************************************/
// UDPServer.h: interface for the CUDPServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPSERVER_H__F101C937_1FE8_4483_86B4_7212DAD702AB__INCLUDED_)
#define AFX_UDPSERVER_H__F101C937_1FE8_4483_86B4_7212DAD702AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define UPUDPBUFLEN 1024

class CUDPServer
{
  public:
    CUDPServer();
    virtual ~CUDPServer();

  public:
    void Close();
    BOOL SetTarget(CString inTIP, int inPort);
    BOOL Send(char *inbuf, int inLen);
    BOOL Send(char *inTarIP, int inPort, char *inbuf, int inLen);
    virtual void Received(void *inbuf, int inlen, SOCKADDR *inpSock);
    BOOL Listen(int inPort);
    void printf(CString instr);
    static UINT UDPReceiveThread(LPVOID pParam);
    // server
    SOCKET m_serverskt;
    SOCKADDR_IN ServerAdr;

    CListBox *m_pShowList;
    CString info;
    int sendres;
    bool bConnected; // 仅表示是否设置过对方ip

  protected:
    // target
    SOCKADDR_IN targAdr;
    int targAdrlen;
    char m_DesIP[16];
    int m_DesPort;
    SOCKET m_sendskt;
    unsigned char m_tarIP[4];
};

#endif // !defined(AFX_UDPSERVER_H__F101C937_1FE8_4483_86B4_7212DAD702AB__INCLUDED_)
