// NetSend.h: interface for the CNetSend class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETSEND_H__6CD6F299_4D5F_4CDC_B4D2_0D5FCD34238A__INCLUDED_)
#define AFX_NETSEND_H__6CD6F299_4D5F_4CDC_B4D2_0D5FCD34238A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxsock.h> // MFC socket extensions

class CNetSend
{
  public:
    BOOL Close();
    BOOL isConnecting();
    BOOL Send(const void *buf, const int length);
    BOOL ConnectTo(char *inpIP, int inDesPort);
    CNetSend();
    virtual ~CNetSend();

  private:
    BOOL m_bConnecting;
    SOCKET m_SendSocket;
    int m_nDesPort;
    char m_DesIP[16];
};

#endif // !defined(AFX_NETSEND_H__6CD6F299_4D5F_4CDC_B4D2_0D5FCD34238A__INCLUDED_)
