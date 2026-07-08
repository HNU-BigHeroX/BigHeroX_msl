// DlgView.cpp : implementation file
//

#include "stdafx.h"
#include "VoyVideo.h"
#include "DlgView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgView dialog

CDlgView::CDlgView(CWnd *pParent /*=NULL*/) : CDialog(CDlgView::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDlgView)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT

    m_hParent = NULL;
}

void CDlgView::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgView)
    // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgView, CDialog)
//{{AFX_MSG_MAP(CDlgView)
ON_WM_CLOSE()
ON_WM_MOVE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgView message handlers

void CDlgView::OnClose()
{
    // TODO: Add your message handler code here and/or call default
    ::PostMessage(m_hParent, WM_VIEWCLOSED, 0, 0);
    CDialog::OnClose();
}

void CDlgView::OnMove(int x, int y)
{
    CDialog::OnMove(x, y);

    // TODO: Add your message handler code here
    ::PostMessage(m_hParent, WM_VIEWMOVEING, 0, 0);
    Invalidate();
}

BOOL CDlgView::PreTranslateMessage(MSG *pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    int nVirtKey;
    if (pMsg->message == WM_KEYDOWN)
    {
        // ÅÐ¶Ï°´ÏÂµÄ¼ü
        nVirtKey = int(pMsg->wParam);

        if (nVirtKey == VK_SHIFT)
        {
            ::PostMessage(m_hParent, WM_VIEWKEYDOWN, pMsg->wParam, 0);
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}
