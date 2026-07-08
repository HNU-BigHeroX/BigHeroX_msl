#if !defined(AFX_DLG_FRAMEPREVIEW_H__C1DD3858_DB91_4247_8174_568421EDFEA7__INCLUDED_)
#define AFX_DLG_FRAMEPREVIEW_H__C1DD3858_DB91_4247_8174_568421EDFEA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_FramePreview.h : header file
//
#include "StillView.h"
#include "ColorTest.h"

/////////////////////////////////////////////////////////////////////////////
// CDlg_FramePreview dialog

class CDlg_FramePreview : public CDialog
{
    // Construction
  public:
    void DisplayAllFrame();
    CDlg_FramePreview(CWnd *pParent = NULL); // standard constructor

    // Dialog Data
    //{{AFX_DATA(CDlg_FramePreview)
    enum
    {
        IDD = IDD_FRAME_PREVIEW_DIALOG
    };
    CStatic m_cDisFramePre;
    CStatic m_cDisFrameNext;
    CStillView m_cFrameNext;
    CStillView m_cFramePre;
    //}}AFX_DATA

    CColorTest *pTest;
    int m_nIndexPre;
    int m_nIndexNext;
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDlg_FramePreview)
  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    // Generated message map functions
    //{{AFX_MSG(CDlg_FramePreview)
    virtual BOOL OnInitDialog();
    afx_msg void OnFrameGopre();
    afx_msg void OnFrameGonext();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_FRAMEPREVIEW_H__C1DD3858_DB91_4247_8174_568421EDFEA7__INCLUDED_)
