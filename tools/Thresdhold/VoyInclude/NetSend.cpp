// NetSend.cpp: implementation of the CNetSend class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NetSend.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetSend::CNetSend()
{
    m_bConnecting = FALSE;
}

CNetSend::~CNetSend()
{
}

BOOL CNetSend::ConnectTo(char *inpIP, int inDesPort)
{
    // 初始化
    strcpy(m_DesIP, inpIP);
    m_nDesPort = inDesPort;

    m_SendSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (m_SendSocket == INVALID_SOCKET)
        return FALSE;

    struct sockaddr_in serversocketaddr;
    serversocketaddr.sin_family = AF_INET;
    serversocketaddr.sin_addr.S_un.S_addr = inet_addr(m_DesIP);
    serversocketaddr.sin_port = htons(m_nDesPort);

    // 连接服务器
    if (connect(m_SendSocket, (struct sockaddr *)&serversocketaddr, sizeof(serversocketaddr)) == SOCKET_ERROR)
        return FALSE;

    m_bConnecting = TRUE;
    return TRUE;
}

BOOL CNetSend::Send(const void *buf, const int length)
{
    if (m_bConnecting)
    {
        if (send(m_SendSocket, (char *)buf, length, 0) != SOCKET_ERROR)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CNetSend::isConnecting()
{
    return m_bConnecting;
}

BOOL CNetSend::Close()
{
    closesocket(m_SendSocket);
    m_bConnecting = FALSE;
    return TRUE;
}
