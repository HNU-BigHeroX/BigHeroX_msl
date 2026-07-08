// NetCmt.cpp: implementation of the CNetCmt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NetCmt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define MAX_SOCKETBUFFER 1024

// 监听线程函数体
// There are two prototype of server socket, one is for large data flow
// another is for little data flow
// here I adopted the first one, which will cost more system resource
UINT ListenThread(LPVOID pParam)
{
    SOCKETLISTENPARAM *psockparam = (SOCKETLISTENPARAM *)pParam;

    sockaddr_in from;
    int fromlen = sizeof(from);

    // 线程将会在下面这个函数阻塞，直到有客户端连接进来
    SOCKET ClientSocket = accept(psockparam->listen_socket, (struct sockaddr *)&from, &fromlen);

    // 判断是否连接上
    if (ClientSocket == INVALID_SOCKET)
    {
        int iErr = WSAGetLastError();
        if (iErr != WSAEINTR)
        {
            AfxBeginThread(ListenThread, pParam);
        }
        return 0L;
    }

    // 生成一个ip字符串，后边用
    CString strIP;
    strIP.Format("%d.%d.%d.%d", from.sin_addr.S_un.S_un_b.s_b1, from.sin_addr.S_un.S_un_b.s_b2,
                 from.sin_addr.S_un.S_un_b.s_b3, from.sin_addr.S_un.S_un_b.s_b4);

    // 连接成功，新开启一个监听线程
    AfxBeginThread(ListenThread, pParam);

    // 将连接客户端信息添加到通讯主体的客户端列表里
    if (psockparam->pListener->GetClientList() != NULL)
    {
        CLIENTINFO *pInfo = new CLIENTINFO;
        memset(pInfo->fromip, 0, sizeof(char) * 16);
        pInfo->socket = ClientSocket;
        pInfo->fromport = from.sin_port;
        strcpy(pInfo->fromip, strIP.GetBuffer(strIP.GetLength()));
        psockparam->pListener->GetClientList()->AddTail(pInfo);

        psockparam->pListener->AferConnect(pInfo);
    }

    // 分配一个缓冲区接收数据
    char *buffer = new char[MAX_SOCKETBUFFER];

    while (1)
    {
        // 线程将会堵塞在这个函数里直到收到数据
        int irecv = recv(ClientSocket, buffer, MAX_SOCKETBUFFER, 0);
        // 假如连接断开则跳出循环
        if (irecv <= 0)
        {
            // delete []buffer;
            // return 0;
            break;
        }

        psockparam->pListener->AfterReceived(buffer, irecv, strIP.GetBuffer(strIP.GetLength()));
    }

    // here remove client info from m_pClientList
    // if server wanna shut down a socket, it only need to close that socket

    POSITION pos;
    if (psockparam->pListener->GetClientList() != NULL)
    {
        for (pos = psockparam->pListener->GetClientList()->GetHeadPosition(); pos != NULL;)
        {
            POSITION oldpos = pos;
            CLIENTINFO *pInfo = (CLIENTINFO *)psockparam->pListener->GetClientList()->GetNext(pos);
            if (pInfo->socket == ClientSocket)
            {
                psockparam->pListener->GetClientList()->RemoveAt(oldpos);
                delete pInfo;
                break;
            }
        }
    }

    // release resource
    delete[] buffer;
    closesocket(ClientSocket);

    // 如果对象没有析构，则调用对应函数
    if (psockparam->pListener != NULL && psockparam->pListener->bToDestroy != TRUE)
    {
        psockparam->pListener->AfterDisconnect(strIP);
    }

    return 0;
}

UINT ReceiveThread(LPVOID pParam)
{
    SOCKETLISTENPARAM *psockparam = (SOCKETLISTENPARAM *)pParam;

    POSITION pos = psockparam->pListener->GetClientList()->GetHeadPosition();
    CLIENTINFO *pInfo = (CLIENTINFO *)psockparam->pListener->GetClientList()->GetAt(pos);
    SOCKET cntsocket = pInfo->socket;

    // 分配一个缓冲区接收数据
    char *buffer = new char[MAX_SOCKETBUFFER];
    while (1)
    {
        // 线程将会堵塞在这个函数里直到收到数据
        int irecv = recv(cntsocket, buffer, MAX_SOCKETBUFFER, 0);
        // 假如连接断开则跳出循环
        if (irecv <= 0)
        {
            // delete []buffer;
            // return 0;
            break;
        }

        psockparam->pListener->AfterReceived(buffer, irecv, pInfo->fromip);
    }

    CString strIP(pInfo->fromip);

    // here remove client info from m_pClientList
    // if server wanna shut down a socket, it only need to close that socket

    if (psockparam->pListener->GetClientList() != NULL)
    {
        for (pos = psockparam->pListener->GetClientList()->GetHeadPosition(); pos != NULL;)
        {
            POSITION oldpos = pos;
            CLIENTINFO *pInfo = (CLIENTINFO *)psockparam->pListener->GetClientList()->GetNext(pos);
            if (pInfo->socket == cntsocket)
            {
                psockparam->pListener->GetClientList()->RemoveAt(oldpos);
                delete pInfo;
                break;
            }
        }
    }

    // release resource
    delete[] buffer;
    closesocket(cntsocket);

    // 如果对象没有析构，则调用对应函数
    if (psockparam->pListener != NULL && psockparam->pListener != NULL && psockparam->pListener->bToDestroy != TRUE)
    {
        psockparam->pListener->AfterDisconnect(strIP);
    }

    return 0;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetCmt::CNetCmt()
{
    m_pClientList = new CPtrList();
    m_pParam = new SOCKETLISTENPARAM;
    m_pClientList = new CPtrList();
    m_ListenSocket = INVALID_SOCKET;

    m_pShowList = NULL;
    m_bListening = FALSE;
    bToDestroy = FALSE;
}

CNetCmt::~CNetCmt()
{
    bToDestroy = TRUE;
    // clear all client information
    POSITION pos;
    for (pos = m_pClientList->GetHeadPosition(); pos != NULL;)
    {
        CLIENTINFO *pinfo = (CLIENTINFO *)m_pClientList->GetNext(pos);
        closesocket(pinfo->socket);
        // delete pinfo;
    }

    Sleep(100);

    for (pos = m_pClientList->GetHeadPosition(); pos != NULL;)
    {
        CLIENTINFO *pinfo = (CLIENTINFO *)m_pClientList->GetNext(pos);
        // closesocket(pinfo->socket);
        delete pinfo;
    }

    m_pClientList->RemoveAll();
    delete m_pClientList;
    m_pClientList = NULL;
}

BOOL CNetCmt::Listen(int inPort)
{
    // 初始化
    m_nListenPort = inPort;
    m_ListenSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in socketaddr;
    socketaddr.sin_family = AF_INET;
    socketaddr.sin_addr.S_un.S_addr = INADDR_ANY;
    socketaddr.sin_port = htons(inPort);

    if (bind(m_ListenSocket, (sockaddr *)&socketaddr, sizeof(socketaddr)) != 0)
    {
        return FALSE;
    }

    if (listen(m_ListenSocket, 300) != 0)
    {
        return FALSE;
    }

    // 开始监听
    m_pParam->listen_socket = m_ListenSocket;
    m_pParam->pListener = this;
    AfxBeginThread(ListenThread, m_pParam);

    m_bListening = TRUE;

    return TRUE;
}

BOOL CNetCmt::Close()
{
    closesocket(m_ListenSocket);
    m_bListening = FALSE;
    return TRUE;
}

BOOL CNetCmt::isListening()
{
    return m_bListening;
}

// 接收数据以后进行的处理
void CNetCmt::AfterReceived(void *buf, int length, char *pFromIP)
{
    if (m_pShowList != NULL)
    {
        char *s = new char[length + 1];
        memcpy(s, buf, length);
        s[length] = '\0';
        _ASSERTE(_CrtCheckMemory());
        CString strIP(pFromIP);
        strIP += ":";
        CString msg(s);
        delete[] s;
        msg = strIP + msg;
        m_pShowList->AddString(msg);
    }
}

// 连接后的处理
void CNetCmt::AferConnect(CLIENTINFO *inClinfo)
{
    if (m_pShowList != NULL)
    {
        CString ip(inClinfo->fromip);
        CString port;
        port.Format("接入，端口: %d", inClinfo->fromport);
        ip += port;
        m_pShowList->AddString(ip);
    }

    memcpy(lastIP, inClinfo->fromip, 16);
}

// 连接断开后的处理
void CNetCmt::AfterDisconnect(CString inIP)
{
    if (m_pShowList != NULL)
    {
        inIP += "断开连接";
        m_pShowList->AddString(inIP);
    }
}

BOOL CNetCmt::GetHostIP(TCHAR *inData)
{
    char name[255];
    if (gethostname(name, sizeof(name)) == 0)
    {
        hostent *pHostent = gethostbyname(name);
        hostent &he = *pHostent;
        sockaddr_in sa;
        for (int nAdapter = 0; he.h_addr_list[nAdapter]; nAdapter++)
        {
            memcpy(&sa.sin_addr.s_addr, he.h_addr_list[nAdapter], he.h_length);
        }
    }
    return TRUE;
}

void CNetCmt::Send(int Index, char *inbuf, int inLen)
{
    if (m_pClientList != NULL)
    {
        POSITION pos = m_pClientList->FindIndex(Index);
        if (pos == NULL)
        {
            return;
        }

        CLIENTINFO *pInfo = (CLIENTINFO *)m_pClientList->GetAt(pos);
        SOCKET clients = pInfo->socket;

        send(clients, inbuf, inLen, 0);
    }
}

BOOL CNetCmt::ConnectTo(int inDesPort, char *inpIP)
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

    // 生成一个ip字符串，后边用
    CString strIP;
    strIP.Format("%d.%d.%d.%d", serversocketaddr.sin_addr.S_un.S_un_b.s_b1, serversocketaddr.sin_addr.S_un.S_un_b.s_b2,
                 serversocketaddr.sin_addr.S_un.S_un_b.s_b3, serversocketaddr.sin_addr.S_un.S_un_b.s_b4);

    // 连接服务器
    if (connect(m_SendSocket, (struct sockaddr *)&serversocketaddr, sizeof(serversocketaddr)) != SOCKET_ERROR)
    {
        // 将连接客户端信息添加到通讯主体的客户端列表里
        if (m_pClientList != NULL)
        {
            CLIENTINFO *pInfo = new CLIENTINFO;
            memset(pInfo->fromip, 0, sizeof(char) * 16);
            pInfo->socket = m_SendSocket;
            pInfo->fromport = serversocketaddr.sin_port;
            strcpy(pInfo->fromip, strIP.GetBuffer(strIP.GetLength()));
            m_pClientList->AddTail(pInfo);

            if (m_pShowList != NULL)
            {
                CString msg;
                msg.Format("成功连接到  %s", strIP);
                m_pShowList->AddString(msg);
            }
        }
        // 开始接收
        m_pParam->pListener = this;
        AfxBeginThread(ReceiveThread, m_pParam);
        return TRUE;
    }
    return FALSE;
}

void CNetCmt::SendtoAll(char *inbuf, int inLen)
{
    POSITION pos;
    for (pos = m_pClientList->GetHeadPosition(); pos != NULL;)
    {
        CLIENTINFO *pinfo = (CLIENTINFO *)m_pClientList->GetNext(pos);
        send(pinfo->socket, inbuf, inLen, 0);
    }
}

void CNetCmt::output(CString instr)
{
    m_pShowList->AddString(instr);
    m_pShowList->SetCurSel(m_pShowList->GetCount() - 1);
}
