// Dlg_FramePreview.cpp : implementation file
//

#include "stdafx.h"
#include "Thresdhold.h"
#include "Dlg_FramePreview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_FramePreview dialog

CDlg_FramePreview::CDlg_FramePreview(CWnd *pParent /*=NULL*/) : CDialog(CDlg_FramePreview::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDlg_FramePreview)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT

    m_nIndexPre = 0;
    m_nIndexNext = 1;
}

void CDlg_FramePreview::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlg_FramePreview)
    DDX_Control(pDX, IDC_DIS_FRAMEPRE, m_cDisFramePre);
    DDX_Control(pDX, IDC_DIS_FRAMENEXT, m_cDisFrameNext);
    DDX_Control(pDX, IDC_FRAME_NEXT, m_cFrameNext);
    DDX_Control(pDX, IDC_FRAME_PRE, m_cFramePre);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlg_FramePreview, CDialog)
//{{AFX_MSG_MAP(CDlg_FramePreview)
ON_BN_CLICKED(IDC_FRAME_GOPRE, OnFrameGopre)
ON_BN_CLICKED(IDC_FRAME_GONEXT, OnFrameGonext)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_FramePreview message handlers

BOOL CDlg_FramePreview::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    DisplayAllFrame();

    return TRUE; // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_FramePreview::DisplayAllFrame()
{
    CString strDis;
    // ◊Û±ﬂ÷°±Í«©
    if (m_nIndexPre < 0)
    {
        m_nIndexPre = m_nIndexPre + FRAME_CNT_MAX;
    }
    if (m_nIndexPre >= FRAME_CNT_MAX)
    {
        m_nIndexPre = m_nIndexPre - FRAME_CNT_MAX;
    }
    strDis.Format("%d / %d", m_nIndexPre + 1, FRAME_CNT_MAX);
    m_cDisFramePre.SetWindowText(strDis);

    // ”“±ﬂ÷°±Í«©
    if (m_nIndexNext < 0)
    {
        m_nIndexNext = m_nIndexNext + FRAME_CNT_MAX;
    }
    if (m_nIndexNext >= FRAME_CNT_MAX)
    {
        m_nIndexNext = m_nIndexNext - FRAME_CNT_MAX;
    }
    strDis.Format("%d / %d", m_nIndexNext + 1, FRAME_CNT_MAX);
    m_cDisFrameNext.SetWindowText(strDis);

    // ◊Û±ﬂ÷°ÕºœÒ
    m_cFramePre.BufImg(pTest->arFrame[m_nIndexPre], 640, 480);
    // ”“±ﬂ÷°ÕºœÒ
    m_cFrameNext.BufImg(pTest->arFrame[m_nIndexNext], 640, 480);
}

void CDlg_FramePreview::OnFrameGopre()
{
    // TODO: Add your control notification handler code here
    m_nIndexPre -= 1;
    m_nIndexNext = m_nIndexPre + 1;
    DisplayAllFrame();
}

void CDlg_FramePreview::OnFrameGonext()
{
    // TODO: Add your control notification handler code here

    m_nIndexPre += 1;
    m_nIndexNext = m_nIndexPre + 1;
    DisplayAllFrame();
}
