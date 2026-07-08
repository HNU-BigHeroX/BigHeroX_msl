#include "stdafx.h"
#include "UDPSend.h"

CUDPSend::CUDPSend()
{
    pSendList = NULL;
    m_sendskt = INVALID_SOCKET;
    m_DesPort = -1;
    bConnected = false;

    m_hSendEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

    m_hExitedEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

    for (int i = 0; i < UDP_BUF_NUM; i++)
    {
        bBufToSend[i] = false;
        arBuf[i] = new char[UDP_BUF_LEN];
        nLenToSend[i] = 0;
    }
}

CUDPSend::~CUDPSend()
{
    bExiting = true;
    CloseHandle(m_hSendEvent);

    if (m_sendskt != INVALID_SOCKET)
    {
        closesocket(m_sendskt);
        m_sendskt = INVALID_SOCKET;
    }

    for (int i = 0; i < UDP_BUF_NUM - 1; i++)
    {
        if (arBuf[i] != NULL)
        {
            delete[] arBuf[i];
            arBuf[i] = NULL;
        }
    }

    WaitForSingleObject(m_hExitedEvent, 10);
}

// function: charTowchar
// purpose:char to WCHAR 、wchar_t、LPWSTR etc
void CUDPSend::charTowchar(const char *chr, wchar_t *wchar, int size)
{
    MultiByteToWideChar(CP_ACP, 0, chr, strlen(chr) + 1, wchar, size / sizeof(wchar[0]));
}

// function: wcharTochar
// purpose:WCHAR 、wchar_t、LPWSTR to char
void CUDPSend::wcharTochar(const wchar_t *wchar, char *chr, int length)
{
    WideCharToMultiByte(CP_ACP, 0, wchar, -1, chr, length, NULL, NULL);
}

void CUDPSend::m_Release()
{
}

// 设置远程端信息
void CUDPSend::SetRemote(char *inIP, int inPort)
{
    // 参数检验
    if (inPort < 0)
    {
        return;
    }

    // 复制ip地址和端口号
    memcpy(m_DesIP, inIP, 16);
    m_DesPort = inPort;

    // 生成套接字
    targAdrlen = sizeof(SOCKADDR);

    targAdr.sin_family = AF_INET;
    targAdr.sin_port = htons(m_DesPort);          /// target 的监听端口
    targAdr.sin_addr.s_addr = inet_addr(m_DesIP); /// target 的地址

    if (m_sendskt != INVALID_SOCKET)
    {
        closesocket(m_sendskt);
        m_sendskt = INVALID_SOCKET;
    }

    m_sendskt = socket(AF_INET, SOCK_DGRAM, 0);

    if (m_sendskt == INVALID_SOCKET)
    {
        PrintToSendList("m_sendskt == INVALID_SOCKET...");
        return;
    }

    bConnected = true;
    AfxBeginThread(UDPSendThread, this);
}

void CUDPSend::SetRemoteStr(CString instrIP, int inPort)
{
    char bufIP[16];
    strcpy(bufIP, instrIP.GetBuffer(instrIP.GetLength()));
    SetRemote(bufIP, inPort);
    instrIP.ReleaseBuffer();
}

// 发送数据
void CUDPSend::Send(char *inBuf, int inLen)
{
    if (m_sendskt == INVALID_SOCKET)
    {
        return;
    }

    if (inLen > UDP_BUF_LEN)
    {
        inLen = UDP_BUF_LEN;
    }

    for (int i = 0; i < UDP_BUF_NUM; i++)
    {
        if (bBufToSend[i] == false)
        {
            memcpy(arBuf[i], inBuf, inLen);
            nLenToSend[i] = inLen;
            bBufToSend[i] = true;
            SetEvent(m_hSendEvent);
            break;
        }
    }

    // 显示发送的数据
    if (pSendList != NULL)
    {
        if (pSendList->GetCount() > 100)
        {
            pSendList->ResetContent();
        }
        CString strData, strTmp;
        strData = "[Send] ";
        for (int i = 0; i < inLen; i++)
        {
            strTmp.Format("%.2X ", (unsigned char)inBuf[i]);
            strData += strTmp;
        }
        pSendList->AddString(strData);
        pSendList->SetCurSel(pSendList->GetCount() - 1);
    }
}

UINT CUDPSend::UDPSendThread(LPVOID pParam)
{
    CUDPSend *pudp = (CUDPSend *)pParam;

    int sendres;
    int targAdrlen = sizeof(SOCKADDR);
    CString info;

    while (WaitForSingleObject(pudp->m_hSendEvent, INFINITE) == WAIT_OBJECT_0)
    {
        for (int i = 0; i < UDP_BUF_NUM; i++)
        {
            if (pudp->bBufToSend[i] == true)
            {
                sendres = sendto(pudp->m_sendskt, pudp->arBuf[i], pudp->nLenToSend[i], 0, (SOCKADDR *)&(pudp->targAdr),
                                 targAdrlen);
                pudp->bBufToSend[i] = false;
                if (sendres < 0)
                {
                    int nErr = WSAGetLastError();
                    break;
                }
            }
        }
    }

    if (pudp->m_sendskt != INVALID_SOCKET)
    {
        closesocket(pudp->m_sendskt);
        pudp->m_sendskt = INVALID_SOCKET;
    }

    SetEvent(pudp->m_hExitedEvent);

    return 0L;
}

void CUDPSend::PrintToSendList(CString inStr)
{
    if (pSendList != NULL)
    {
        pSendList->AddString(inStr);
        pSendList->SetCurSel(pSendList->GetCount() - 1);
    }
}
