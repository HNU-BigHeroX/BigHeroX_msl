// MSLFieldMonitorDoc.h : interface of the CMSLFieldMonitorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSLFIELDMONITORDOC_H__CA2DA2A5_3817_49C0_BF0A_AF77BB0C9295__INCLUDED_)
#define AFX_MSLFIELDMONITORDOC_H__CA2DA2A5_3817_49C0_BF0A_AF77BB0C9295__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMSLFieldMonitorDoc : public CDocument
{
  protected: // create from serialization only
    CMSLFieldMonitorDoc();
    DECLARE_DYNCREATE(CMSLFieldMonitorDoc)

    // Attributes
  public:
    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMSLFieldMonitorDoc)
  public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive &ar);
    //}}AFX_VIRTUAL

    // Implementation
  public:
    virtual ~CMSLFieldMonitorDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext &dc) const;
#endif

  protected:
    // Generated message map functions
  protected:
    //{{AFX_MSG(CMSLFieldMonitorDoc)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSLFIELDMONITORDOC_H__CA2DA2A5_3817_49C0_BF0A_AF77BB0C9295__INCLUDED_)
