// TemMapFile.h: interface for the CTemMapFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEMMAPFILE_H__FA5549CE_43FF_49FB_AB31_2328FAC142A3__INCLUDED_)
#define AFX_TEMMAPFILE_H__FA5549CE_43FF_49FB_AB31_2328FAC142A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTemMapFile
{
  public:
    void Load(CString filename, int *inmap, int insize);
    void SaveToFile(int *inmap, int insize, CString filename);
    CTemMapFile();
    virtual ~CTemMapFile();
};

#endif // !defined(AFX_TEMMAPFILE_H__FA5549CE_43FF_49FB_AB31_2328FAC142A3__INCLUDED_)
