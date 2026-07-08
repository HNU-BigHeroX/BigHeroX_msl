// SCPlatform.h : main header file for the SCPLATFORM application
//

#if !defined(AFX_SCPLATFORM_H__33CE41E4_7325_424F_802C_EB3FDCD66822__INCLUDED_)
#define AFX_SCPLATFORM_H__33CE41E4_7325_424F_802C_EB3FDCD66822__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h" // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSCPlatformApp:
// See SCPlatform.cpp for the implementation of this class
//

class CSCPlatformApp : public CWinApp
{
  public:
    CSCPlatformApp();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSCPlatformApp)
  public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL

    // Implementation

    //{{AFX_MSG(CSCPlatformApp)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCPLATFORM_H__33CE41E4_7325_424F_802C_EB3FDCD66822__INCLUDED_)
