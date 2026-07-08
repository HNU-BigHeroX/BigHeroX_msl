// CompassEX.h: interface for the CCompassEX class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPASSEX_H__1B24CCF6_1CCF_41C3_B606_91669E7F8B3D__INCLUDED_)
#define AFX_COMPASSEX_H__1B24CCF6_1CCF_41C3_B606_91669E7F8B3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VoyInclude\IBehavior.h"
#include "VoyInclude\SerialPort.h"

#define CMDLEN 12

class CCompassEX : public CSerialPort
{
  public:
    CStatic *pAglOut;
    void m_printf(CString instr);
    IBehavior *pBeh;
    void ParseByte(BYTE *inChar);
    CCompassEX();
    virtual ~CCompassEX();

  protected:
    void m_ParseFrm();
    bool m_bFrmStart;
    BYTE m_chLast;
    UINT m_nRecIndex;
    char m_RecBuf[CMDLEN];
    CString m_strinfo;

    int m_iXField;
    int m_iYField;
    short m_sTemp;
    WORD m_wTemp;
    WORD m_nAngle;
};

#endif // !defined(AFX_COMPASSEX_H__1B24CCF6_1CCF_41C3_B606_91669E7F8B3D__INCLUDED_)
