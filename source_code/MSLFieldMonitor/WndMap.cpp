// WndMap.cpp : implementation file
//

#include "stdafx.h"
#include "MSLFieldMonitor.h"
#include "WndMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWndMap dialog

CWndMap::CWndMap(CWnd *pParent /*=NULL*/) : CDialog(CWndMap::IDD, pParent)
{
    //{{AFX_DATA_INIT(CWndMap)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

void CWndMap::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CWndMap)
    // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWndMap, CDialog)
//{{AFX_MSG_MAP(CWndMap)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWndMap message handlers
