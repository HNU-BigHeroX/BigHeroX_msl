// MulticastSocket.cpp : implementation file
//

#include "stdafx.h"
#include "MulticastSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMulticastSocket

CMulticastSocket::CMulticastSocket()
{
    bForceNoLoopback = FALSE;
    bDataReceived = TRUE; /* Variable defined for this project. Not necessarily part of CMulticastSocket */

    pEdit = NULL;
    plist = NULL;

    m_strRFIP = ".";
}

CMulticastSocket::~CMulticastSocket()
{
}

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CMulticastSocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CMulticastSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif // 0

/////////////////////////////////////////////////////////////////////////////
// CMulticastSocket member functions

BOOL CMulticastSocket::CreateReceivingSocket(LPCTSTR strGroupIP, UINT nGroupPort)
{
    /* Create socket for receiving packets from multicast group */
    if (!Create(nGroupPort, SOCK_DGRAM, FD_READ, NULL))
        return FALSE;

    BOOL bMultipleApps = TRUE; /* allow reuse of local port if needed */
    SetSockOpt(SO_REUSEADDR, (void *)&bMultipleApps, sizeof(BOOL), SOL_SOCKET);

    /* Fill m_saHostGroup_in for sending datagrams */
    memset(&m_saHostGroup, 0, sizeof(m_saHostGroup));
    m_saHostGroup.sin_family = AF_INET;
    m_saHostGroup.sin_addr.s_addr = inet_addr(strGroupIP);
    m_saHostGroup.sin_port = htons((USHORT)nGroupPort);

    /* Join the multicast group */
    m_mrMReq.imr_multiaddr.s_addr = inet_addr(strGroupIP); /* group addr */
    m_mrMReq.imr_interface.s_addr = htons(INADDR_ANY);     /* use default */
    if (setsockopt(m_hSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char FAR *)&m_mrMReq, sizeof(m_mrMReq)) < 0)
        return FALSE;

    return TRUE;
}

BOOL CMulticastSocket::CreateSendingSocket(UINT nTTL, BOOL bLoopBack)
{
    if (!m_SendSocket.Create(0, SOCK_DGRAM, 0)) // Create an unconnected UDP socket
        return FALSE;

    if (!SetTTL(nTTL)) // Set Time to Live as specified by user
        AfxMessageBox("Warning! Error Setting TTL");

    SetLoopBack(bLoopBack); // Enable/Disable Loopback

    return TRUE;
}

BOOL CMulticastSocket::SetTTL(UINT nTTL)
{
    /* Set Time to Live to parameter TTL */
    if (m_SendSocket.SetSockOpt(IP_MULTICAST_TTL, &nTTL, sizeof(int), IPPROTO_IP) == 0)
        return FALSE; /* Error Setting TTL */
    else
        return TRUE; /* else TTL set successfully */
}

void CMulticastSocket::SetLoopBack(BOOL bLoop)
{
    /* Set LOOPBACK option to TRUE OR FALSE according to IsLoop parameter */
    int nLoopBack = (int)bLoop;
    if (m_SendSocket.SetSockOpt(IP_MULTICAST_LOOP, &nLoopBack, sizeof(int), IPPROTO_IP) == 0)
    {
        if (!bLoop) /* if required to stop loopback */
        {
            bForceNoLoopback = TRUE; /* Internally making a note that loopback has to be disabled forcefilly */

            // Get IP/Port for send socket in order to disable loopback forcefully */
            char localHost[255];
            gethostname(localHost, 255);
            struct hostent *host = gethostbyname(localHost); /* Get local host IP */
            m_strLocalIP = inet_ntoa(*(struct in_addr *)*host->h_addr_list);
            CString Dummy;                                 // Dummy string to be passed to the GetSockName function
            m_SendSocket.GetSockName(Dummy, m_nLocalPort); /* Get Port Number for Sending Port */
        }
    }
}

void CMulticastSocket::OnReceive(int nErrorCode)
{
    int nError = ReceiveFrom(m_strBuffer, 1024, m_strSendersIP, m_nSendersPort);
    if (nError == SOCKET_ERROR)
    {
        AfxMessageBox("Error receiving data from the host group");
    }
    else
    {
        if (!bForceNoLoopback ||
            (bForceNoLoopback && !(m_strSendersIP == m_strLocalIP && m_nSendersPort == m_nLocalPort)))
        {
            // 1. If loopbackback is not to be forced then interface handles the loopback itself
            // 2. If you have to loopback and SOCKOPT LOOPBACK fails, no problem, interfaces loopback by default
            // 3. If you have to stop loopback and SOCKOPT LOOPBACK fails, ignore messages coming from your own sending
            // socket

            // TODO : Add your code for here. The packet received is in m_strBuffer
            bDataReceived = TRUE; /* Making note that a message has arrived */

            // 把数据传给回调函数
            Recieved(m_strBuffer, nError);
        }
    }

    CAsyncSocket::OnReceive(nErrorCode);
}

BOOL CMulticastSocket::LeaveGroup()
{
    if (setsockopt(m_hSocket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char FAR *)&m_mrMReq, sizeof(m_mrMReq)) < 0)
        return FALSE;

    m_SendSocket.Close(); // Close sending socket
    Close();              // Close receving socket
    return TRUE;
}

BOOL CMulticastSocket::Post(const void *strMessage, int nSize)
{
    if (m_SendSocket.SendTo(strMessage, nSize, (SOCKADDR *)&m_saHostGroup, sizeof(SOCKADDR), 0) == SOCKET_ERROR)
        return FALSE;
    else
        return TRUE;
}

BOOL CMulticastSocket::JoinGroup(CString GroupIP, UINT nGroupPort, UINT nTTL, BOOL bLoopback)
{
    LeaveGroup();
    if (!CreateReceivingSocket(GroupIP, nGroupPort)) /* Create Socket for receiving and join the host group */
    {
        m_SendSocket.Close(); // Close sending socket
        Close();              // Close receving socket
        return FALSE;
    }
    if (!CreateSendingSocket(nTTL, bLoopback)) /* Create Socket for sending */
    {
        m_SendSocket.Close(); // Close sending socket
        Close();              // Close receving socket

        if (plist != NULL)
        {
            PrintList("加入组操作失败！！");
        }
        return FALSE;
    }

    if (plist != NULL)
    {
        CString strInfo;
        strInfo.Format("成功加入组播 [%s] 端口:[%d]", GroupIP, nGroupPort);
        PrintList(strInfo);
    }

    return TRUE;
}

BOOL CMulticastSocket::Create(UINT nSocketPort, int nSocketType, long lEvent, LPCTSTR lpszSocketAddress)
{
    if (Socket(nSocketType, lEvent))
    {
        BOOL bMultipleApps = TRUE;
        SetSockOpt(SO_REUSEADDR, (void *)&bMultipleApps, sizeof(BOOL), SOL_SOCKET);

        if (Bind(nSocketPort, lpszSocketAddress))
        {
            return TRUE;
        }
        int nResult = GetLastError();
        Close();
        WSASetLastError(nResult);
    }
    return FALSE;
}

void CMulticastSocket::SetRefBoxIP(CString inRFIP)
{
    m_strRFIP = inRFIP;
}

void CMulticastSocket::PrintList(CString inStr)
{
    if (plist != NULL)
    {
        plist->AddString(inStr);
        plist->SetCurSel(plist->GetCount() - 1);
    }
}

void CMulticastSocket::Recieved(BYTE *inBuf, int inLen)
{
    CString cmd, str;
    CTime tm;
    tm = CTime::GetCurrentTime();
    cmd = tm.Format("[%X]");
    for (int i = 0; i < inLen; i++)
    {
        str.Format("%.2X ", inBuf[i]);
        cmd += str;
    }
    PrintList(cmd);
}
