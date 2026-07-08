#if !defined(AFX_NUMDISPLAY_H__5A3025B0_0E85_473D_8662_3A53490ED762__INCLUDED_)
#define AFX_NUMDISPLAY_H__5A3025B0_0E85_473D_8662_3A53490ED762__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NumDisplay.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNumDisplay window

class CNumDisplay : public CStatic
{
    // Construction
  public:
    CNumDisplay();

    // Attributes
  public:
    bool bisBall;
    int Number;
    CString tstr;
    CBitmap m_bk;
    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNumDisplay)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    virtual ~CNumDisplay();
    CFont font;

    // Generated message map functions
  protected:
    //{{AFX_MSG(CNumDisplay)
    afx_msg void OnPaint();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NUMDISPLAY_H__5A3025B0_0E85_473D_8662_3A53490ED762__INCLUDED_)
