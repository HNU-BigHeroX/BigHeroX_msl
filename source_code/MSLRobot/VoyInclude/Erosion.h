// Erosion.h: interface for the CErosion class.
//
//////////////////////////////////////////////////////////////////////
/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.0       Date: 2007/10/19
  Email:  zwj_uptech@126.com
  Description:    // 图像色块腐蚀类
  Others:         //
*************************************************/

#if !defined(AFX_EROSION_H__3716A10A_ABE9_4F3E_8AFF_003182643E76__INCLUDED_)
#define AFX_EROSION_H__3716A10A_ABE9_4F3E_8AFF_003182643E76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UPGraphics.h"

class CErosion : public CUPGraphics
{
  public:
    BOOL Process();
    CErosion();
    virtual ~CErosion();

  protected:
    UCHAR *_tMap;
    int _tWidth;
    int _tHeight;
    bool Cluster(int x, int y);
};

#endif // !defined(AFX_EROSION_H__3716A10A_ABE9_4F3E_8AFF_003182643E76__INCLUDED_)
