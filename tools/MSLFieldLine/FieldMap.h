#if !defined(AFX_FIELDMAP_H__3A313F2E_51AC_4282_B2F5_8D7547B9A1EB__INCLUDED_)
#define AFX_FIELDMAP_H__3A313F2E_51AC_4282_B2F5_8D7547B9A1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FieldMap.h : header file
//
#include "TemMapFile.h"

/////////////////////////////////////////////////////////////////////////////
// CFieldMap window

class CFieldMap : public CStatic
{
    // Construction
  public:
    CFieldMap();

    // Attributes
  public:
    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CFieldMap)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    void m_printf(CString inStr);
    static UINT MapingThread(LPVOID pParam);
    void m_DisplayMap();
    BOOL SaveBitmapToFile(CBitmap *bitmap, LPSTR lpFileName);
    void CreateBMP();
    // 尺寸参数
    double Field_Width;
    double Field_Height;
    double Field_PenaltyArea_Width;
    double Field_PenaltyArea_Height;
    double Field_GoalArea_Width;
    double Field_GoalArea_Height;
    double Field_DiameterofCenter;
    double Field_RadiusofCorner;

    CStatic *m_pView;
    CListBox *m_pInfoList;

    void CreateMap();
    virtual ~CFieldMap();

    // Generated message map functions
  protected:
    void m_DisplayCapturedBits(BYTE *pBuffer, BITMAPINFOHEADER *pbih, HWND hwndStill);
    CRect m_PicRect; // 绘制场线的图片大小
    CTemMapFile m_mapfile;

    CDC dcMem;
    CBitmap Bitmap;
    double m_tmp; // 临时变量
    double m_k;   // 放大系数

    // 深度图显示
    BITMAPINFOHEADER bih;

    //{{AFX_MSG(CFieldMap)
    afx_msg void OnPaint();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIELDMAP_H__3A313F2E_51AC_4282_B2F5_8D7547B9A1EB__INCLUDED_)
