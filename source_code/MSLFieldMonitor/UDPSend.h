// 发送球员信息
#pragma once

#define UDP_BUF_NUM 16
#define UDP_BUF_LEN 1024

class CUDPSend
{
  public:
    CUDPSend();
    ~CUDPSend();

  protected:
    void m_Release();
    void charTowchar(const char *chr, wchar_t *wchar, int size);
    void wcharTochar(const wchar_t *wchar, char *chr, int length);

    char m_DesIP[16];
    int m_DesPort;
    SOCKET m_sendskt; // target
    SOCKADDR_IN targAdr;
    int targAdrlen;

  public:
    CListBox *pSendList;
    bool bConnected; // 仅表示是否设置过远程端ip

    // 发送缓冲
    bool bBufToSend[UDP_BUF_NUM];
    char *arBuf[UDP_BUF_NUM];
    int nLenToSend[UDP_BUF_NUM];
    bool bExiting;

    // 缓冲发送事件
    HANDLE m_hSendEvent;
    // 线程退出完毕事件
    HANDLE m_hExitedEvent;

    // 设置远程端信息
    void SetRemote(char *inIP, int inPort);
    // 发送数据
    void Send(char *inBuf, int inLen);
    void SetRemoteStr(CString instrIP, int inPort);
    static UINT UDPSendThread(LPVOID pParam);
    void PrintToSendList(CString inStr);
};
