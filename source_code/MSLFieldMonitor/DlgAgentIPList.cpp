// DlgAgentIPList.cpp : implementation file
//

#include "stdafx.h"
#include "MSLFieldMonitor.h"
#include "DlgAgentIPList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAgentIPList dialog

CDlgAgentIPList::CDlgAgentIPList(CWnd *pParent /*=NULL*/) : CDialog(CDlgAgentIPList::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDlgAgentIPList)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

void CDlgAgentIPList::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgAgentIPList)
    DDX_Control(pDX, IDC_AGENT_IPLIST, m_cAgentIPList);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgAgentIPList, CDialog)
//{{AFX_MSG_MAP(CDlgAgentIPList)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAgentIPList message handlers

BOOL CDlgAgentIPList::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    CString strInfo;
    for (int i = 0; i < 5; i++)
    {
        if (pCoachUDP->m_ToAgent[i].bConnected == true)
        {
            strInfo.Format("【%d】号机：%s", i + 1, pCoachUDP->m_ToAgent[i].strRemoteIP);
        }
        else
        {
            strInfo.Format("【%d】号机：未连接", i + 1);
        }
        m_cAgentIPList.AddString(strInfo);
    }

    return TRUE; // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}
