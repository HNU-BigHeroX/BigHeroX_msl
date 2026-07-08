// չʾ��ɫ״̬
#if !defined(AFX_DISROLESTATUS_H__80F663E0_7201_45B3_A4C0_5EE76FE580B0__INCLUDED_)
#define AFX_DISROLESTATUS_H__80F663E0_7201_45B3_A4C0_5EE76FE580B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DisRoleStatus.h : header file
//

#include "MSL_Structs.h"
#include "GreenField.h"
/////////////////////////////////////////////////////////////////////////////
// CDisRoleStatus window

class CDisRoleStatus : public CStatic
{
    // Construction
  public:
    CDisRoleStatus();

    // Attributes
  public:
    stTactics *pTac;
    CGreenField *pField;

    CDC dcMemory;
    CBitmap m_bk;
    CRect m_FrameRect;
    CRect m_rcRole[5];
    CFont fontTitle;
    CFont fontText;

    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDisRoleStatus)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    virtual ~CDisRoleStatus();

    // Generated message map functions
  protected:
    //{{AFX_MSG(CDisRoleStatus)
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISROLESTATUS_H__80F663E0_7201_45B3_A4C0_5EE76FE580B0__INCLUDED_)
