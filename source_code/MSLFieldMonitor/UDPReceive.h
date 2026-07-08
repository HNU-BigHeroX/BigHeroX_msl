// 接受球员信息
#pragma once
class CUDPReceive
{
  public:
    CUDPReceive();
    ~CUDPReceive();

  public:
    CListBox *pRecvList;
    int nRecvPort;

    // server
    SOCKET m_serverskt;
    SOCKADDR_IN ServerAdr;
    int targAdrlen;

    // 线程退出完毕事件
    HANDLE m_hExitedEvent;

    // 打开接收端口
    void Listen(int inPort);
    void m_Release();
    void PrintToList(CString inStr);
    virtual void CallbackReceived(char *inBuf, int inLen, SOCKADDR *inpSock);
};
