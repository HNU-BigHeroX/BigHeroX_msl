/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.3       Date: 2008/9/23
  Email:  zwj_uptech@126.com
  Description:    // 网络client类，单个连接
  Others:         //
*************************************************/
// TCPClient.h: interface for the CTCPClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPCLIENT_H__07E63E23_AF0C_4025_BFD9_EA8E97CFEB4B__INCLUDED_)
#define AFX_TCPCLIENT_H__07E63E23_AF0C_4025_BFD9_EA8E97CFEB4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif               // _MSC_VER > 1000
#include <afxsock.h> // MFC socket extensions

class CTCPClient
{
  public:
    CTCPClient();
    virtual ~CTCPClient();

  public:
    void Send(char *inbuf, int inLen);
    void printf(CString instr);
    virtual void Received(void *inbuf, int inlen);
    virtual void ConnectSucceed();
    CListBox *m_pShowList;

    virtual void ConnectError();
    BOOL ConnectTo(CString inDesIP, int inDesPort);
    SOCKET cntskt;
    int m_nDesPort;
    CString m_strDesIP;
    char m_DesIP[16];

    bool m_bOnline;
};

#endif // !defined(AFX_TCPCLIENT_H__07E63E23_AF0C_4025_BFD9_EA8E97CFEB4B__INCLUDED_)
