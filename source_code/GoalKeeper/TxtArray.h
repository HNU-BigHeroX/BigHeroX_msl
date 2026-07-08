// TxtArray.h: interface for the CTxtArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TXTARRAY_H__A2225018_7067_4535_A3FF_B7D15F80ED70__INCLUDED_)
#define AFX_TXTARRAY_H__A2225018_7067_4535_A3FF_B7D15F80ED70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTxtArray
{
  public:
    virtual void AfterRead(void *inArr, int inLen);
    bool WriteIntFile(CString inFilename, int *inArr, int inLen);
    bool WriteFile(CString inFilename, UCHAR *inArr, int inLen);
    int ReadFromFile(CString inFilename, int *inArr, int inLen);
    CTxtArray();
    virtual ~CTxtArray();

    CListCtrl *pList;
    bool *pbAssign;

    int m_nReadLen;
};

#endif // !defined(AFX_TXTARRAY_H__A2225018_7067_4535_A3FF_B7D15F80ED70__INCLUDED_)
