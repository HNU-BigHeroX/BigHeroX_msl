// GBRGMalvar.h: interface for the CGBRGMalvar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GBRGMALVAR_H__4B7996A5_2A5F_4E67_A2BB_FC6F79E40915__INCLUDED_)
#define AFX_GBRGMALVAR_H__4B7996A5_2A5F_4E67_A2BB_FC6F79E40915__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGBRGMalvar
{
  public:
    void GreenCenter_BlueRow(BYTE *pBuf);
    void GreenCenter_RedRow(BYTE *pBuf);
    void BlueCenter(BYTE *pBuf);
    void RedCenter(BYTE *pBuf);
    void MalvarDemosaic(BYTE *inBuf, int inWidth, int inHeight);
    CGBRGMalvar();
    virtual ~CGBRGMalvar();

    int m_nWidth;
    int m_nHeight;
    int m_nWidthBytes;
};

#endif // !defined(AFX_GBRGMALVAR_H__4B7996A5_2A5F_4E67_A2BB_FC6F79E40915__INCLUDED_)
