// Dilation.h: interface for the CDilation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DILATION_H__024B764B_AF6C_4CC8_9A34_216FDE7DA1BE__INCLUDED_)
#define AFX_DILATION_H__024B764B_AF6C_4CC8_9A34_216FDE7DA1BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UPGraphics.h"

class CDilation : public CUPGraphics
{
  public:
    BOOL Process();
    UCHAR *_tMap;
    int _tWidth;
    int _tHeight;
    CDilation();
    virtual ~CDilation();

  protected:
    void _Dilate(int x, int y);
};

#endif // !defined(AFX_DILATION_H__024B764B_AF6C_4CC8_9A34_216FDE7DA1BE__INCLUDED_)
