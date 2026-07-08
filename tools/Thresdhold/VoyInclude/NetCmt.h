// NetCmt.h: interface for the CNetCmt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETCMT_H__B31A6A2D_2775_4152_8111_8B5F53570AF4__INCLUDED_)
#define AFX_NETCMT_H__B31A6A2D_2775_4152_8111_8B5F53570AF4__INCLUDED_

#include <afxsock.h> // MFC socket extensions

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 通讯类传递到监听线程里的参数
class CNetCmt;
typedef struct _tag_SOCKET_LISTEN_PARAM
{
    SOCKET listen_socket; // listen socket handle
    CNetCmt *pListener;   // NetListener pointer
} SOCKETLISTENPARAM;

// 连接用户信息，监听线程返回给通讯类
typedef struct _tag_CLIENT_INFO
{
    char fromip[16]; // client ip
    int fromport;    // client port
    SOCKET socket;   // client socket
} CLIENTINFO;

class CNetCmt
{
  public:
    void output(CString instr);
    void AfterDisconnect(CString inIP);
    void SendtoAll(char *inbuf, int inLen);
    BOOL ConnectTo(int inDesPort, char *inpIP);
    void Send(int Index, char *inbuf, int Len);
    BOOL GetHostIP(TCHAR *inData);
    void virtual AferConnect(CLIENTINFO *inClinfo);
    BOOL isListening();
    BOOL Close();
    void virtual AfterReceived(void *buf, int length, char *pFromIP);
    CPtrList *GetClientList()
    {
        return m_pClientList;
    } // retrieves client list point
    BOOL Listen(int inPort);
    CNetCmt();
    virtual ~CNetCmt();

    CListBox *m_pShowList;

    char lastIP[16];
    BOOL bToDestroy;

  protected:
    int m_nListenPort;

    BOOL m_bListening;
    SOCKET m_ListenSocket;
    CPtrList *m_pClientList;
    SOCKETLISTENPARAM *m_pParam; // socket listen thread parameter

    // send
    SOCKET m_SendSocket;
    int m_nDesPort;
    char m_DesIP[16];
};

#endif // !defined(AFX_NETCMT_H__B31A6A2D_2775_4152_8111_8B5F53570AF4__INCLUDED_)
