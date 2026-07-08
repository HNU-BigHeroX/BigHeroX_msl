#if !defined(AFX_DEBUGVIEW_H__9EF76688_E683_4CDB_94D3_2F847AE24B37__INCLUDED_)
#define AFX_DEBUGVIEW_H__9EF76688_E683_4CDB_94D3_2F847AE24B37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DebugView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDebugView window

class CDebugView : public CStatic
{
    // Construction
  public:
    CDebugView();

    // Attributes
  public:
    BYTE *m_pBuf;
    int m_nWidth;
    int m_nHeight;

    // È«Í¼ÏÔÊ¾
    CDC m_dcMem;
    BITMAPINFOHEADER m_bih;
    CBitmap Bitmap;
    int m_nDisWidth;
    int m_nDisHeight;
    bool m_bInitDC;
    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDebugView)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    void DrawFocus(int inX, int inY, UCHAR inR, UCHAR inG, UCHAR inB);
    void PointColor(int inX, int inY, UCHAR inR, UCHAR inG, UCHAR inB);
    void Display();
    void ReleaseBuf();
    void InitSize(int inWidth, int inHeight, CDC *inpDC);
    virtual ~CDebugView();

    // Generated message map functions
  protected:
    //{{AFX_MSG(CDebugView)
    afx_msg void OnPaint();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEBUGVIEW_H__9EF76688_E683_4CDB_94D3_2F847AE24B37__INCLUDED_)
