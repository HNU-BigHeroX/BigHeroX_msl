// DlgDebugInfo.cpp : implementation file
//

#include "stdafx.h"
#include "VoyVideo.h"
#include "DlgDebugInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDebugInfo dialog

CDlgDebugInfo::CDlgDebugInfo(CWnd *pParent /*=NULL*/) : CDialog(CDlgDebugInfo::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDlgDebugInfo)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

void CDlgDebugInfo::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgDebugInfo)
    DDX_Control(pDX, IDC_SIGINFO, m_cSigInfo);
    DDX_Control(pDX, IDC_INFOLIST, m_cInfoList);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgDebugInfo, CDialog)
//{{AFX_MSG_MAP(CDlgDebugInfo)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDebugInfo message handlers
