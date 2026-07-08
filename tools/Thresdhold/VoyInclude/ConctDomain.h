/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.0       Date: 2007/6/10
  Email:  zwj_uptech@126.com
  Description:    // 连通域检测
  Others:         //
*************************************************/
// ConctDomain.h: interface for the CConctDomain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONCTDOMAIN_H__F42E661E_D5F3_4CB7_810A_FB9A95FA728A__INCLUDED_)
#define AFX_CONCTDOMAIN_H__F42E661E_D5F3_4CB7_810A_FB9A95FA728A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "UPGraphics.h"

typedef struct Domain
{
    int x;
    int y;
    int sum;
} DomainNode;

class CConctDomain : public CUPGraphics
{
  public:
    Domain GetDomainNear(int x, int y);
    Domain GetDomainAt(int index);
    void SetRange(int inRge);
    void Mark(BYTE *rgbBuf);
    int GetCount();
    BOOL Process(int inMode);
    CConctDomain();
    virtual ~CConctDomain();

  protected:
    int _Range;
    DomainNode _CurDomain;
    BOOL _Expend(int x, int y);
    CList<DomainNode, DomainNode> _result;
    CList<CPoint, CPoint> _toExp;
};

#endif // !defined(AFX_CONCTDOMAIN_H__F42E661E_D5F3_4CB7_810A_FB9A95FA728A__INCLUDED_)
