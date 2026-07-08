// WndReferee.cpp : implementation file
//

#include "stdafx.h"
#include "MSLFieldMonitor.h"
#include "WndReferee.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TMR_CNT_REFBOX 1

static CString strCFGFile = L".\\MSLFieldMonitorCfg.ini";

/////////////////////////////////////////////////////////////////////////////
// CWndReferee dialog

CWndReferee::CWndReferee(CWnd *pParent /*=NULL*/) : CDialog(CWndReferee::IDD, pParent)
{
    //{{AFX_DATA_INIT(CWndReferee)
    m_strRefBoxIP = _T("");
    //}}AFX_DATA_INIT
}

void CWndReferee::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CWndReferee)
    DDX_Control(pDX, IDC_SIDE_CLR, m_cSideClr);
    DDX_Control(pDX, IDC_DATA_INFOLIST, m_cDataInfoList);
    DDX_Control(pDX, IDC_REF_INFOLIST, m_cRefInfoList);
    DDX_Control(pDX, IDC_COMBO_REF_PROT, m_cRefProtocol);
    DDX_Text(pDX, IDC_EDIT_REFBOX_IP, m_strRefBoxIP);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWndReferee, CDialog)
//{{AFX_MSG_MAP(CWndReferee)
ON_BN_CLICKED(IDC_REF_CNT, OnRefCnt)
ON_BN_CLICKED(IDC_REF_DISCNT, OnRefDiscnt)
ON_WM_SIZE()
ON_WM_TIMER()
ON_BN_CLICKED(IDC_BTN_LOC_IP, OnBtnLocIp)
ON_BN_CLICKED(IDC_BTN_AGENT_IP, OnBtnAgentIp)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWndReferee message handlers

void CWndReferee::OnRefCnt()
{
    // TODO: Add your control notification handler code here

    m_cRefInfoList.AddString("正在连接裁判盒...");
    UpdateData();
    pRefClient->ConnectTo(m_strRefBoxIP, 28097);

    // 保存
    WritePrivateProfileString("Monitor", "RefreeBox_IP", m_strRefBoxIP, strCFGFile);

    SetTimer(TMR_CNT_REFBOX, 1000, NULL);
}

void CWndReferee::OnRefDiscnt()
{
    // TODO: Add your control notification handler code here
    KillTimer(TMR_CNT_REFBOX);
    Sleep(100);
    pRefClient->CloseSocket();
    pRefClient->m_bConnected = false;
}

void CWndReferee::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    CWnd *pWnd = GetDlgItem(IDC_SIDE_CLR);
    if (pWnd != NULL)
    {
        CRect rc;
        pWnd->GetWindowRect(rc);

        m_cSideClr.nDividLine = rc.Width() / 8;
        if (m_cSideClr.nFlag == 0)
        {
            // magenta 分界线在右侧
            m_cSideClr.nDividLine = rc.Width() - m_cSideClr.nDividLine;
        }
        m_cSideClr.nCurPosition = m_cSideClr.nDividLine;
    }
}

void CWndReferee::OnTimer(UINT nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    if (nIDEvent == TMR_CNT_REFBOX)
    {
        if (pRefClient->m_bConnected != true)
        {
            OnRefCnt();
        }
    }

    CDialog::OnTimer(nIDEvent);
}

BOOL CWndReferee::OnInitDialog()
{
    CDialog::OnInitDialog();
    m_cRefProtocol.SetCurSel(0);

    // TODO: Add extra initialization here

    return TRUE; // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWndReferee::PreTranslateMessage(MSG *pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    if (pMsg->message == WM_KEYDOWN) // 判断是否为按键消息
    {

        switch (pMsg->wParam) // 根据虚拟键表值来判断按下的是什么键
        {
        case VK_SHIFT:
            // AfxMessageBox(L"CWndReferee::PreTranslateMessage");
            pKey->bShiftPressed = true;
            break;
        case VK_CONTROL:
            pKey->bCtrlPressed = true;
            break;
        case 65:
            // AfxMessageBox(_T("a按下"));
            pKey->bKeyAPressed = true;
            break;
        case 83:
            // AfxMessageBox(_T("s按下"));
            pKey->bKeySPressed = true;
            break;
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CWndReferee::OnBtnLocIp()
{
    // TODO: Add your control notification handler code here

    m_dlgLocIPInfo.DoModal();
}

void CWndReferee::OnBtnAgentIp()
{
    // TODO: Add your control notification handler code here

    m_dlgAgentIPList.DoModal();
}
