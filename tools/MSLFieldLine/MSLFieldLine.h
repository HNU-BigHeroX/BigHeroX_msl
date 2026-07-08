// MSLFieldLine.h : main header file for the MSLFIELDLINE application
//

#if !defined(AFX_MSLFIELDLINE_H__03C97E8C_7D3E_4BC7_9CD6_82AA9B7BE452__INCLUDED_)
#define AFX_MSLFIELDLINE_H__03C97E8C_7D3E_4BC7_9CD6_82AA9B7BE452__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h" // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMSLFieldLineApp:
// See MSLFieldLine.cpp for the implementation of this class
//

class CMSLFieldLineApp : public CWinApp
{
  public:
    CMSLFieldLineApp();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMSLFieldLineApp)
  public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL

    // Implementation

    //{{AFX_MSG(CMSLFieldLineApp)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSLFIELDLINE_H__03C97E8C_7D3E_4BC7_9CD6_82AA9B7BE452__INCLUDED_)
