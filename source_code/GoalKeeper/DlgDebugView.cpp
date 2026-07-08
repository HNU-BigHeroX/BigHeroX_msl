// DlgDebugView.cpp : implementation file
//

#include "stdafx.h"
#include "VoyVideo.h"
#include "DlgDebugView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDebugView dialog

CDlgDebugView::CDlgDebugView(CWnd *pParent /*=NULL*/) : CDialog(CDlgDebugView::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDlgDebugView)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

void CDlgDebugView::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgDebugView)
    DDX_Control(pDX, IDC_DEBUG_VIEW, m_cDbgView);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgDebugView, CDialog)
//{{AFX_MSG_MAP(CDlgDebugView)
ON_WM_PAINT()
ON_BN_CLICKED(IDC_CLEAN, OnClean)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDebugView message handlers

BOOL CDlgDebugView::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here

    return TRUE; // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDebugView::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // TODO: Add your message handler code here

    // Do not call CDialog::OnPaint() for painting messages
}

void CDlgDebugView::OnClean()
{
    // TODO: Add your control notification handler code here
    for (int i = 100; i < 600; i++)
    {
        m_cDbgView.PointColor(i, 100, 255, 0, 255);
        m_cDbgView.PointColor(i, 101, 255, 0, 255);
        m_cDbgView.PointColor(i, 102, 255, 0, 255);
        m_cDbgView.PointColor(i, 103, 255, 0, 255);
        m_cDbgView.PointColor(i, 104, 255, 0, 255);
    }
    m_cDbgView.DrawFocus(200, 200, 0, 255, 0);
    m_cDbgView.Display();
}
