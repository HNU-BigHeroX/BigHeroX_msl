#include "stdafx.h"
#include "UDPReceive.h"

#define UDP_RECV_BUF_LEN 1024

CUDPReceive::CUDPReceive()
{
    nRecvPort = 0;
    pRecvList = NULL;
    m_serverskt = INVALID_SOCKET;
    m_hExitedEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

CUDPReceive::~CUDPReceive()
{
    if (m_serverskt != INVALID_SOCKET)
    {
        closesocket(m_serverskt);
        m_serverskt = INVALID_SOCKET;
    }

    WaitForSingleObject(m_hExitedEvent, 1000);
}

/*******************************************************************/
// 接收线程函数
UINT UDPReceiveThread(LPVOID pParam)
{
    CUDPReceive *pudp = (CUDPReceive *)pParam;
    pudp->PrintToList(L"开启UDP接收线程...");

    int reclen = 0;

    char buffer[UDP_RECV_BUF_LEN] = "\0";
    SOCKADDR linkAdr;
    int len = sizeof(SOCKADDR);

    while (pudp->m_serverskt != INVALID_SOCKET)
    {
        reclen = recvfrom((pudp->m_serverskt), buffer, UDP_RECV_BUF_LEN, 0, (SOCKADDR *)&linkAdr, &len);
        if (reclen != SOCKET_ERROR)
        {
            pudp->CallbackReceived(buffer, reclen, (SOCKADDR *)&linkAdr);
        }
    }

    if (pudp->m_serverskt != INVALID_SOCKET)
    {
        closesocket(pudp->m_serverskt);
        pudp->m_serverskt = INVALID_SOCKET;
    }

    SetEvent(pudp->m_hExitedEvent);

    return 1L;
}

// 打开接收端口
void CUDPReceive::Listen(int inPort)
{
    if (m_serverskt != INVALID_SOCKET)
    {
        closesocket(m_serverskt);
        m_serverskt = INVALID_SOCKET;
    }

    SOCKADDR_IN local;
    targAdrlen = sizeof(SOCKADDR);

    local.sin_family = AF_INET;
    local.sin_port = htons(inPort);                 /// 监听端口
    local.sin_addr.S_un.S_addr = htonl(INADDR_ANY); /// 本机

    m_serverskt = socket(AF_INET, SOCK_DGRAM, 0);
    if (bind(m_serverskt, (SOCKADDR *)&local, sizeof(SOCKADDR)) != 0)
    {
        PrintToList(L"bind err!");
        return;
    }

    AfxBeginThread(UDPReceiveThread, this);
}

void CUDPReceive::m_Release()
{
}

void CUDPReceive::PrintToList(CString inStr)
{
    if (pRecvList != NULL)
    {
        pRecvList->AddString(inStr);
        pRecvList->SetCurSel(pRecvList->GetCount() - 1);
    }
}

void CUDPReceive::CallbackReceived(char *inBuf, int inLen, SOCKADDR *inpSock)
{
    if (pRecvList != NULL)
    {
        if (pRecvList->GetCount() > 100)
        {
            pRecvList->ResetContent();
        }
        CString strData, strTmp;
        strData = "[Recv] ";
        for (int i = 0; i < inLen; i++)
        {
            strTmp.Format("%.2X ", (unsigned char)inBuf[i]);
            strData += strTmp;
        }
        PrintToList(strData);
    }
    /*///////////////////////////////////////////////////////////
    info.Format("rec %d bytes: %s from ", inlen, inbuf);
    unsigned char* pch = (unsigned char*)inpSock->sa_data;
    WORD iPort = ntohs(*(WORD*)pch);
    CString ip;
    ip.Format("%d.%d.%d.%d: %d", pch[2], pch[3], pch[4], pch[5], iPort);
    info += ip;
    m_SerPringf(info);
    ////////////////////////////////////////////////////////////*/
}
