// UDPServer.cpp: implementation of the CUDPServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UDPServer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUDPServer::CUDPServer()
{
    m_sendskt = INVALID_SOCKET;
    m_pShowList = NULL;
    m_DesPort = -1;
    bConnected = false;
}

CUDPServer::~CUDPServer()
{
}

UINT CUDPServer::UDPReceiveThread(LPVOID pParam)
{
    CUDPServer *pudp = (CUDPServer *)pParam;

    int reclen = 0;

    char buffer[UPUDPBUFLEN] = "\0";
    SOCKADDR linkAdr;
    int len = sizeof(SOCKADDR);

    while (pudp->m_serverskt != INVALID_SOCKET)
    {
        reclen = recvfrom((pudp->m_serverskt), buffer, UPUDPBUFLEN, 0, (SOCKADDR *)&linkAdr, &len);
        if (reclen != SOCKET_ERROR)
        {
            pudp->Received(buffer, reclen, (SOCKADDR *)&linkAdr);
        }
    }

    if (pudp->m_serverskt != INVALID_SOCKET)
    {
        closesocket(pudp->m_serverskt);
        pudp->m_serverskt = INVALID_SOCKET;
    }

    return 1L;
}

BOOL CUDPServer::Listen(int inPort)
{
    if (m_serverskt != INVALID_SOCKET)
    {
        closesocket(m_serverskt);
        m_serverskt = INVALID_SOCKET;
        Sleep(100); // 等待监听线程退出
    }

    SOCKADDR_IN local;
    targAdrlen = sizeof(SOCKADDR);

    local.sin_family = AF_INET;
    local.sin_port = htons(inPort);                 /// 监听端口
    local.sin_addr.S_un.S_addr = htonl(INADDR_ANY); /// 本机

    m_serverskt = socket(AF_INET, SOCK_DGRAM, 0);
    if (bind(m_serverskt, (SOCKADDR *)&local, sizeof(SOCKADDR)) != 0)
    {
        printf("bind err!");
        return FALSE;
    }

    AfxBeginThread(UDPReceiveThread, this);
    return TRUE;
}

void CUDPServer::Received(void *inbuf, int inlen, SOCKADDR *inpSock)
{
    info.Format("rec %d bytes: %s from ", inlen, inbuf);
    unsigned char *pch = (unsigned char *)inpSock->sa_data;
    WORD iPort = ntohs(*(WORD *)pch);
    CString ip;
    ip.Format("%d.%d.%d.%d: %d", pch[2], pch[3], pch[4], pch[5], iPort);
    info += ip;
    printf(info);
}

BOOL CUDPServer::Send(char *inTarIP, int inPort, char *inbuf, int inLen)
{
    // 参数检验
    if (inPort < 0 || inTarIP == NULL)
    {
        return FALSE;
    }

    targAdrlen = sizeof(SOCKADDR);

    targAdr.sin_family = AF_INET;
    targAdr.sin_port = htons(inPort);             /// target 的监听端口
    targAdr.sin_addr.s_addr = inet_addr(inTarIP); /// target 的地址

    // 转换ip至数值
    SOCKADDR *pAddr = (SOCKADDR *)&targAdr;
    unsigned char *pch = (unsigned char *)pAddr->sa_data;
    pch += 2;
    memcpy(m_tarIP, pch, 4);

    if (m_sendskt != INVALID_SOCKET)
    {
        closesocket(m_sendskt);
        m_sendskt = INVALID_SOCKET;
    }

    m_sendskt = socket(AF_INET, SOCK_DGRAM, 0);

    if (m_sendskt == INVALID_SOCKET)
    {
        printf("m_sendskt == INVALID_SOCKET...");
        return FALSE;
    }

    sendres = sendto(m_sendskt, inbuf, inLen, 0, (SOCKADDR *)&targAdr, targAdrlen);
    if (sendres == SOCKET_ERROR)
    {
        info.Format("Send err : %d!", WSAGetLastError());
        printf(info);
        return FALSE;
    }
    // info.Format("Send %d bytes",sendres);
    // printf(info);
    return TRUE;
}

BOOL CUDPServer::Send(char *inbuf, int inLen)
{
    if (m_sendskt == INVALID_SOCKET)
    {
        return FALSE;
    }

    sendres = sendto(m_sendskt, inbuf, inLen, 0, (SOCKADDR *)&targAdr, targAdrlen);
    if (sendres == SOCKET_ERROR)
    {
        info.Format("Send err : %d!", WSAGetLastError());
        printf(info);
        return FALSE;
    }
    // info.Format("Send %d bytes",sendres);
    // printf(info);
    return TRUE;
}

void CUDPServer::printf(CString instr)
{
    if (m_pShowList != NULL)
    {
        m_pShowList->AddString(instr);
        m_pShowList->SetCurSel(m_pShowList->GetCount() - 1);
    }
}

BOOL CUDPServer::SetTarget(CString inTIP, int inPort)
{
    // 参数检验
    if (inPort < 0)
    {
        return FALSE;
    }

    // 复制ip地址和端口号
    strcpy(m_DesIP, inTIP.GetBuffer(inTIP.GetLength()));
    m_DesPort = inPort;

    // 生成套接字
    targAdrlen = sizeof(SOCKADDR);

    targAdr.sin_family = AF_INET;
    targAdr.sin_port = htons(m_DesPort);          /// target 的监听端口
    targAdr.sin_addr.s_addr = inet_addr(m_DesIP); /// target 的地址

    // 转换ip至数值
    SOCKADDR *pAddr = (SOCKADDR *)&targAdr;
    unsigned char *pch = (unsigned char *)pAddr->sa_data;
    pch += 2;
    memcpy(m_tarIP, pch, 4);

    if (m_sendskt != INVALID_SOCKET)
    {
        closesocket(m_sendskt);
        m_sendskt = INVALID_SOCKET;
    }

    m_sendskt = socket(AF_INET, SOCK_DGRAM, 0);

    if (m_sendskt == INVALID_SOCKET)
    {
        printf("m_sendskt == INVALID_SOCKET...");
        return FALSE;
    }

    bConnected = true;
    return TRUE;
}

void CUDPServer::Close()
{
    if (m_sendskt != INVALID_SOCKET)
    {
        closesocket(m_sendskt);
    }

    if (m_serverskt != INVALID_SOCKET)
    {
        closesocket(m_serverskt);
    }

    //	bConnected = false;
}
