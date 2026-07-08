// UDPClient.h: interface for the UDPClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPCLIENT_H__61CF2565_FE0C_40A8_85D1_94B090EBF1FC__INCLUDED_)
#define AFX_UDPCLIENT_H__61CF2565_FE0C_40A8_85D1_94B090EBF1FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class UDPClient
{
  public:
    BOOL Send(char *inTarIP, int inPort, char *inbuf, int inLen);
    void Send(char *inbuf, int inLen);
    void SetTarget(CString inTIP, int inPort);
    void m_printf(CString instr);
    UDPClient();
    virtual ~UDPClient();
    CListBox *m_pShowList;
    CString info;

    SOCKET m_sendskt;
    int sendres;
    SOCKADDR_IN targAdr;
    int targAdrlen;
    char m_DesIP[16];
    int m_DesPort;
};

#endif // !defined(AFX_UDPCLIENT_H__61CF2565_FE0C_40A8_85D1_94B090EBF1FC__INCLUDED_)
