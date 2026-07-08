// MSLAgent.h : main header file for the MSLAGENT application
//

#if !defined(AFX_MSLAGENT_H__CF3D23BD_4406_474E_9E1B_EF3395A45470__INCLUDED_)
#define AFX_MSLAGENT_H__CF3D23BD_4406_474E_9E1B_EF3395A45470__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h" // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMSLAgentApp:
// See MSLAgent.cpp for the implementation of this class
//

class CMSLAgentApp : public CWinApp
{
  public:
    CMSLAgentApp();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMSLAgentApp)
  public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL

    // Implementation

    //{{AFX_MSG(CMSLAgentApp)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSLAGENT_H__CF3D23BD_4406_474E_9E1B_EF3395A45470__INCLUDED_)
