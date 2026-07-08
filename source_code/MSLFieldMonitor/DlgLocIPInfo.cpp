// DlgLocIPInfo.cpp : implementation file
//

#include "stdafx.h"
#include "MSLFieldMonitor.h"
#include "DlgLocIPInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// #include"winsock2.h"
#pragma comment(lib, "ws2_32.lib")

char *fetchIPAddress()
{
    char host_name[225];
    if (gethostname(host_name, sizeof(host_name)) == SOCKET_ERROR)
    {
        // cout << "Error " << WSAGetLastError() << " when getting local host name." << endl;
    }

    struct hostent *phe = gethostbyname(host_name);
    if (phe == 0)
    {
        // cout << "Yow!Bad host lookup.";
    }

    struct in_addr addr;
    memcpy(&addr, phe->h_addr_list[0], sizeof(struct in_addr));
    return inet_ntoa(addr);
}

char *getIP()
{
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
    {
        return NULL;
    }

    char *ipAddress = fetchIPAddress();
    WSACleanup();
    return ipAddress;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgLocIPInfo dialog

CDlgLocIPInfo::CDlgLocIPInfo(CWnd *pParent /*=NULL*/) : CDialog(CDlgLocIPInfo::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDlgLocIPInfo)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

void CDlgLocIPInfo::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgLocIPInfo)
    DDX_Control(pDX, IDC_IP_LIST, m_cLocIPInfo);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgLocIPInfo, CDialog)
//{{AFX_MSG_MAP(CDlgLocIPInfo)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLocIPInfo message handlers

BOOL CDlgLocIPInfo::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    GetLocIP();

    return TRUE; // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgLocIPInfo::GetLocIP()
{
    char *ip = getIP();
    CString str(ip);
    m_cLocIPInfo.AddString(str);
}
