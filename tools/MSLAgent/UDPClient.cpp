// UDPClient.cpp: implementation of the UDPClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MSLAgent.h"
#include "UDPClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UDPClient::UDPClient()
{
    m_pShowList = NULL;
}

UDPClient::~UDPClient()
{
}

void UDPClient::m_printf(CString instr)
{
    if (m_pShowList != NULL)
    {
        m_pShowList->AddString(instr);
        m_pShowList->SetCurSel(m_pShowList->GetCount() - 1);
    }
}

void UDPClient::SetTarget(CString inTIP, int inPort)
{
    strcpy(m_DesIP, inTIP.GetBuffer(inTIP.GetLength()));
    m_DesPort = inPort;
}

void UDPClient::Send(char *inbuf, int inLen)
{
    Send(m_DesIP, m_DesPort, inbuf, inLen);
}

BOOL UDPClient::Send(char *inTarIP, int inPort, char *inbuf, int inLen)
{
    strcpy(m_DesIP, inTarIP);
    m_DesPort = inPort;
    targAdrlen = sizeof(SOCKADDR);

    targAdr.sin_family = AF_INET;
    targAdr.sin_port = htons(inPort);             /// target 的监听端口
    targAdr.sin_addr.s_addr = inet_addr(m_DesIP); /// target 的地址

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
    info.Format("Send %d bytes", sendres);
    printf(info);
    return TRUE;
}
