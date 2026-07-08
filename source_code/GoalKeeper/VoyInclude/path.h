// path.h: interface for the Cpath class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATH_H__2D1835E9_D375_4CFC_83E9_5359B45743B9__INCLUDED_)
#define AFX_PATH_H__2D1835E9_D375_4CFC_83E9_5359B45743B9__INCLUDED_
#include "Yuzhi.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Cpath
{
  public:
    void write(CString str);
    void starwrite();
    int Getfig(char a, char b, char sz[5000]);
    void GetDefault(Yuzhi *yu);
    Cpath();
    virtual ~Cpath();
    CStdioFile f;
};

#endif // !defined(AFX_PATH_H__2D1835E9_D375_4CFC_83E9_5359B45743B9__INCLUDED_)
