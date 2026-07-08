// DlgMotorInfo.cpp : implementation file
//

#include "stdafx.h"
#include "VoyVideo.h"
#include "DlgMotorInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMotorInfo dialog

CDlgMotorInfo::CDlgMotorInfo(CWnd *pParent /*=NULL*/) : CDialog(CDlgMotorInfo::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDlgMotorInfo)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

void CDlgMotorInfo::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgMotorInfo)
    DDX_Control(pDX, IDC_IOINFO, m_cIOInfo);
    DDX_Control(pDX, IDC_ADINFO, m_cADInfo);
    DDX_Control(pDX, IDC_MOTORINFO_LIST, m_cMInfo_List);
    DDX_Control(pDX, IDC_MOTORINFO_DIS, m_cMotorInfo);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgMotorInfo, CDialog)
//{{AFX_MSG_MAP(CDlgMotorInfo)
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_LOG, OnLog)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMotorInfo message handlers

void CDlgMotorInfo::OnClose()
{
    // TODO: Add your message handler code here and/or call default
    ::SendMessage(m_hParent, WM_MINFOCLOSED, 0, 0);
    // Sleep(500);

    CDialog::OnClose();
}

void CDlgMotorInfo::OnLog()
{
    // TODO: Add your control notification handler code here
    CTime tm;
    tm = CTime::GetCurrentTime();
    CString fn;
    fn.Format("%d-%d-%d_%d-%d-%d.txt", tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour(), tm.GetMinute(),
              tm.GetSecond());

    CStdioFile mFile;
    CFileException mExcept;
    mFile.Open(fn, CFile::modeCreate | CFile::modeWrite, &mExcept);
    CString string;
    for (int i = 0; i < m_cMInfo_List.GetCount(); i++)
    {
        m_cMInfo_List.GetText(i, string);
        string += "\r\n";
        mFile.WriteString(string);
    }
    mFile.Close();
}
