// MSLFieldMonitorView.h : interface of the CMSLFieldMonitorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSLFIELDMONITORVIEW_H__335F8B0A_2D8F_4924_B76F_5B21E66F2FB5__INCLUDED_)
#define AFX_MSLFIELDMONITORVIEW_H__335F8B0A_2D8F_4924_B76F_5B21E66F2FB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "DataCenter.h"
#include "CoachUDP.h"
#include "RefClient.h"
#include "WndField.h"
#include "WndReferee.h"
#include "WndMatchPanel.h"
#include "WndMap.h"
#include "WndStatus.h"

#define TMR_EXEC 1
#define TMR_REFCNT 2
#define TMR_DISUPDATE 3
#define TMR_ONE_SEC 4
#define TESTCMD 10 // 测试按钮用定时器

class CMSLFieldMonitorView : public CFormView
{
  protected: // create from serialization only
    CMSLFieldMonitorView();
    DECLARE_DYNCREATE(CMSLFieldMonitorView)

  public:
    //{{AFX_DATA(CMSLFieldMonitorView)
    enum
    {
        IDD = IDD_MSLFIELDMONITOR_FORM
    };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // Attributes
  public:
    CMSLFieldMonitorDoc *GetDocument();
    // 数据对象
    CDataCenter m_cDataCenter; // 整个的数据中心，只有一个实例对象

    // 通讯
    CCoachUDP m_cCoachUDP;
    CRefClient m_cRefClient;

    // 子窗体
    CWndField m_wndField;
    CWndReferee m_wndReferee;
    CWndMatchPanel m_wndMatchPanel;
    // CWndMap m_wndMap;
    CWndStatus m_wndStatus;
    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMSLFieldMonitorView)
  public:
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    virtual BOOL PreTranslateMessage(MSG *pMsg);

  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    virtual void OnInitialUpdate();                  // called first time after construct
    virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
    virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
    virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);
    virtual void OnPrint(CDC *pDC, CPrintInfo *pInfo);
    //}}AFX_VIRTUAL

    // Implementation
  public:
    void checkKeyPressed();
    virtual ~CMSLFieldMonitorView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext &dc) const;
#endif

  protected:
    // Generated message map functions
  protected:
    //{{AFX_MSG(CMSLFieldMonitorView)
    afx_msg void OnTimer(UINT nIDEvent);             // 时间回调函数
    afx_msg void OnSize(UINT nType, int cx, int cy); // 窗口大小改变时的回调函数
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG // debug version in MSLFieldMonitorView.cpp
inline CMSLFieldMonitorDoc *CMSLFieldMonitorView::GetDocument()
{
    return (CMSLFieldMonitorDoc *)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSLFIELDMONITORVIEW_H__335F8B0A_2D8F_4924_B76F_5B21E66F2FB5__INCLUDED_)
