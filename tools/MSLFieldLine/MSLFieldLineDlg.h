// MSLFieldLineDlg.h : header file
//

#if !defined(AFX_MSLFIELDLINEDLG_H__C00DCBE3_98B8_4FCE_8C0B_E8F34CAE9BEB__INCLUDED_)
#define AFX_MSLFIELDLINEDLG_H__C00DCBE3_98B8_4FCE_8C0B_E8F34CAE9BEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FieldMap.h"

/////////////////////////////////////////////////////////////////////////////
// CMSLFieldLineDlg dialog

class CMSLFieldLineDlg : public CDialog
{
    // Construction
  public:
    int SaveBitmapToFile(CBitmap *bitmap, LPSTR lpFileName);
    CMSLFieldLineDlg(CWnd *pParent = NULL); // standard constructor

    // Dialog Data
    //{{AFX_DATA(CMSLFieldLineDlg)
    enum
    {
        IDD = IDD_MSLFIELDLINE_DIALOG
    };
    CListBox m_cInfoList;
    CStatic m_cMapView;
    CFieldMap m_cFieldMap;
    int m_nCor_R;
    int m_nField_len;
    int m_nField_wid;
    int m_nGoal_len;
    int m_nGoal_wid;
    int m_Mid_diameter;
    int m_nPen_len;
    int m_nPen_wid;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMSLFieldLineDlg)
  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    HICON m_hIcon;

    // Generated message map functions
    //{{AFX_MSG(CMSLFieldLineDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnCreatebmp();
    afx_msg void OnCreate();
    afx_msg void OnDisplay();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSLFIELDLINEDLG_H__C00DCBE3_98B8_4FCE_8C0B_E8F34CAE9BEB__INCLUDED_)
