// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "MSLFieldMonitor.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
//{{AFX_MSG_MAP(CMainFrame)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code !
ON_WM_CREATE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] = {
    ID_SEPARATOR, // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    // TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    // 去除菜单
    SetMenu(NULL);
    // 	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
    // 		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
    // 		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
    // 	{
    // 		TRACE0("Failed to create toolbar\n");
    // 		return -1;      // fail to create
    // 	}
    //
    // 	if (!m_wndStatusBar.Create(this) ||
    // 		!m_wndStatusBar.SetIndicators(indicators,
    // 		  sizeof(indicators)/sizeof(UINT)))
    // 	{
    // 		TRACE0("Failed to create status bar\n");
    // 		return -1;      // fail to create
    // 	}
    //
    // 	// TODO: Delete these three lines if you don't want the toolbar to
    // 	//  be dockable
    // 	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    // 	EnableDocking(CBRS_ALIGN_ANY);
    // 	DockControlBar(&m_wndToolBar);

    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT &cs)
{
    if (!CFrameWnd::PreCreateWindow(cs))
        return FALSE;
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
               WS_MAXIMIZE | WS_SYSMENU;

    cs.style &= ~FWS_ADDTOTITLE;     // 去掉“无标题”
    m_strTitle = "MSL_Coach V 1.05"; // 设置新的标题

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext &dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

BOOL CMainFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT &rect,
                        CWnd *pParentWnd, UINT nID, CCreateContext *pContext)
{
    // TODO: Add your specialized code here and/or call the base class

    return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

BOOL CMainFrame::PreTranslateMessage(MSG *pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    if (pMsg->message == WM_KEYDOWN) // 判断是否为按键消息
    {

        switch (pMsg->wParam) // 根据虚拟键表值来判断按下的是什么键
        {
        case VK_SHIFT:
            // AfxMessageBox(L"CMainFrame::PreTranslateMessage");
            // pKey->bShiftPressed = true;
            break;
        case VK_CONTROL:
            // pKey->bCtrlPressed = true;
            break;
        case 65:
            // AfxMessageBox(_T("a按下"));
            // pKey->bKeyAPressed = true;
            break;
        }
    }
    return CFrameWnd::PreTranslateMessage(pMsg);
}
