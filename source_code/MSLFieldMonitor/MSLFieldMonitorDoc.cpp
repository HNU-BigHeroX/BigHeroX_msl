// MSLFieldMonitorDoc.cpp : implementation of the CMSLFieldMonitorDoc class
//

#include "stdafx.h"
#include "MSLFieldMonitor.h"

#include "MSLFieldMonitorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMSLFieldMonitorDoc

IMPLEMENT_DYNCREATE(CMSLFieldMonitorDoc, CDocument)

BEGIN_MESSAGE_MAP(CMSLFieldMonitorDoc, CDocument)
//{{AFX_MSG_MAP(CMSLFieldMonitorDoc)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMSLFieldMonitorDoc construction/destruction

CMSLFieldMonitorDoc::CMSLFieldMonitorDoc()
{
    // TODO: add one-time construction code here
}

CMSLFieldMonitorDoc::~CMSLFieldMonitorDoc()
{
}

BOOL CMSLFieldMonitorDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMSLFieldMonitorDoc serialization

void CMSLFieldMonitorDoc::Serialize(CArchive &ar)
{
    if (ar.IsStoring())
    {
        // TODO: add storing code here
    }
    else
    {
        // TODO: add loading code here
    }
}

/////////////////////////////////////////////////////////////////////////////
// CMSLFieldMonitorDoc diagnostics

#ifdef _DEBUG
void CMSLFieldMonitorDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CMSLFieldMonitorDoc::Dump(CDumpContext &dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMSLFieldMonitorDoc commands
