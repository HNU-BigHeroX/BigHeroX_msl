// Thresdhold.h : main header file for the THRESDHOLD application
//

#if !defined(AFX_THRESDHOLD_H__108459FB_4A0D_4552_8F4D_4BBB7F88926D__INCLUDED_)
#define AFX_THRESDHOLD_H__108459FB_4A0D_4552_8F4D_4BBB7F88926D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h" // main symbols

/////////////////////////////////////////////////////////////////////////////
// CThresdholdApp:
// See Thresdhold.cpp for the implementation of this class
//

class CThresdholdApp : public CWinApp
{
  public:
    CThresdholdApp();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CThresdholdApp)
  public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL

    // Implementation

    //{{AFX_MSG(CThresdholdApp)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THRESDHOLD_H__108459FB_4A0D_4552_8F4D_4BBB7F88926D__INCLUDED_)
