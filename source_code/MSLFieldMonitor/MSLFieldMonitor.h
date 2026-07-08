// MSLFieldMonitor.h : main header file for the MSLFIELDMONITOR application
//

#if !defined(AFX_MSLFIELDMONITOR_H__4AB1FBD2_C8A4_4FEB_AEB3_8928B4024DAC__INCLUDED_)
#define AFX_MSLFIELDMONITOR_H__4AB1FBD2_C8A4_4FEB_AEB3_8928B4024DAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h" // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMSLFieldMonitorApp:
// See MSLFieldMonitor.cpp for the implementation of this class
//

class CMSLFieldMonitorApp : public CWinApp
{
  public:
    CMSLFieldMonitorApp();
    ULONG_PTR m_gdiplusToken;

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMSLFieldMonitorApp)
  public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    //}}AFX_VIRTUAL

    // Implementation
    //{{AFX_MSG(CMSLFieldMonitorApp)
    afx_msg void OnAppAbout();
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSLFIELDMONITOR_H__4AB1FBD2_C8A4_4FEB_AEB3_8928B4024DAC__INCLUDED_)
