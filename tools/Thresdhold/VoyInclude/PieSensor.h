#if !defined(AFX_PIESENSOR_H__7A7E2A3D_8F77_49BA_B1BD_4BC4EF282D99__INCLUDED_)
#define AFX_PIESENSOR_H__7A7E2A3D_8F77_49BA_B1BD_4BC4EF282D99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PieSensor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPieSensor window
typedef struct _tag_PICSIZE
{
    int width;
    int height;
} PICSIZE;

#define SENSORNUM 24

class CPieSensor : public CStatic
{
    // Construction
  public:
    CPieSensor();

    // Attributes
  public:
    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CPieSensor)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    void UpdateUntrasonic(DOUBLE *pultrasonic, BOOL *pbEnable = NULL);
    void UpdateInfrared(BOOL *pinfrared);
    void SetLine(DOUBLE scale);
    void SetBitmap(UINT picid, UINT shelterid);
    virtual ~CPieSensor();

    // Generated message map functions
  protected:
    //{{AFX_MSG(CPieSensor)
    afx_msg void OnPaint();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
  private:
    void ReleaseUltrasonic();
    void ReleaseInfrared();
    void InitSensor();
    DOUBLE m_dDistance;
    int m_nSelectSensor;
    bool *m_pbSonicEnable;
    CPoint CalPoint(int iPoint, int firstindex = 0);
    bool *m_pInfrared;
    UINT m_uBkPicID, m_uShelterID;
    DOUBLE *m_pUltrasonicSensor;
    int m_nInfrared, m_nUltrasonicSensor;
    int m_nCenterX, m_nCenterY;
    PICSIZE m_bksize, m_sheltersize;
    int *m_pInfraredX;
    int *m_pInfraredY;
    DOUBLE m_scale;
    DOUBLE m_dTanD;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIESENSOR_H__7A7E2A3D_8F77_49BA_B1BD_4BC4EF282D99__INCLUDED_)
