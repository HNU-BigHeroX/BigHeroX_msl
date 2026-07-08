// TCPClient.cpp: implementation of the CTCPClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TCPClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

UINT TCPReceiveThread(LPVOID pParam)
{
    CTCPClient *pClient = (CTCPClient *)pParam;

    SOCKET cntskt = pClient->cntskt;

    // 分配一个缓冲区接收数据
    char *buffer = new char[1024];
    while (1)
    {
        // 线程将会堵塞在这个函数里直到收到数据
        int irecv = recv(cntskt, buffer, 1024, 0);
        // 假如连接断开则跳出循环
        if (irecv <= 0)
        {
            break;
        }

        pClient->Received(buffer, irecv);
    }
    pClient->printf("与 " + pClient->m_strDesIP + " 的连接断开！");
    // release resource
    delete[] buffer;
    closesocket(cntskt);
    pClient->m_bOnline = false;

    return 0;
}

// 连接线程函数体
UINT TCPConnectThread(LPVOID pParam)
{
    CTCPClient *pclient = (CTCPClient *)pParam;
    pclient->cntskt = socket(AF_INET, SOCK_STREAM, 0);

    if (pclient->cntskt == INVALID_SOCKET)
    {
        pclient->ConnectError();
        return 0L;
    }
    pclient->m_bOnline = true;
    struct sockaddr_in serversocketaddr;
    serversocketaddr.sin_family = AF_INET;
    serversocketaddr.sin_addr.S_un.S_addr = inet_addr(pclient->m_DesIP);
    serversocketaddr.sin_port = htons(pclient->m_nDesPort);

    // 生成一个ip字符串，后边显示用
    pclient->m_strDesIP.Format("%d.%d.%d.%d", serversocketaddr.sin_addr.S_un.S_un_b.s_b1,
                               serversocketaddr.sin_addr.S_un.S_un_b.s_b2, serversocketaddr.sin_addr.S_un.S_un_b.s_b3,
                               serversocketaddr.sin_addr.S_un.S_un_b.s_b4);

    // 连接服务器
    if (connect(pclient->cntskt, (struct sockaddr *)&serversocketaddr, sizeof(serversocketaddr)) != SOCKET_ERROR)
    {
        // 将连接客户端信息添加到通讯主体的客户端列表里
        if (pclient->m_pShowList != NULL)
        {
            CString msg;
            msg.Format("成功连接到  %s", pclient->m_strDesIP);
            pclient->m_pShowList->AddString(msg);
            pclient->m_pShowList->SetCurSel(pclient->m_pShowList->GetCount() - 1);
        }
        pclient->ConnectSucceed();
        // 开始接收
        AfxBeginThread(TCPReceiveThread, pParam);
        return TRUE;
    }
    pclient->m_bOnline = false;
    pclient->ConnectError();
    return FALSE;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTCPClient::CTCPClient()
{
    m_pShowList = NULL;

    m_bOnline = false;
}

CTCPClient::~CTCPClient()
{
    if (cntskt != INVALID_SOCKET)
    {
        closesocket(cntskt);
    }
}

BOOL CTCPClient::ConnectTo(CString inDesIP, int inDesPort)
{
    if (inDesPort == m_nDesPort && inDesIP.Find(m_strDesIP) != -1 && m_bOnline == true)
    {
        return FALSE;
    }

    // 初始化
    m_strDesIP = inDesIP;
    strcpy(m_DesIP, m_strDesIP.GetBuffer(m_strDesIP.GetLength()));
    m_nDesPort = inDesPort;

    AfxBeginThread(TCPConnectThread, this);
    return TRUE;
}

void CTCPClient::ConnectError()
{
    CString str(" 连接失败...");
    str = m_strDesIP + str;
    printf(str);
}

void CTCPClient::ConnectSucceed()
{
}

void CTCPClient::Received(void *inbuf, int inlen)
{
}

void CTCPClient::printf(CString instr)
{
    if (m_pShowList != NULL)
    {
        m_pShowList->AddString(instr);
        m_pShowList->SetCurSel(m_pShowList->GetCount() - 1);
    }
}

void CTCPClient::Send(char *inbuf, int inLen)
{
    if (m_bOnline == true)
    {
        send(cntskt, inbuf, inLen, 0);
    }
}
