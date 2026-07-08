// WndField.cpp : implementation file
//

#include "stdafx.h"
#include "MSLFieldMonitor.h"
#include "WndField.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWndField dialog

CWndField::CWndField(CWnd *pParent /*=NULL*/) : CDialog(CWndField::IDD, pParent)
{
    //{{AFX_DATA_INIT(CWndField)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

void CWndField::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CWndField)
    DDX_Control(pDX, IDC_GREENFIELD, m_cGreenField);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWndField, CDialog)
//{{AFX_MSG_MAP(CWndField)
ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWndField message handlers

void CWndField::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    // 移动场地控件位置
    if (m_cGreenField.m_hWnd)
    {
        CRect rcWnd;
        rcWnd.left = 0;
        rcWnd.top = 0;
        rcWnd.right = rcWnd.left + 700;
        rcWnd.bottom = rcWnd.top + 500;
        m_cGreenField.MoveWindow(&rcWnd);
    }
}
