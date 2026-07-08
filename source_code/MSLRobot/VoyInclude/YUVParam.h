// YUVParam.h: YUVParam struct.
//
//////////////////////////////////////////////////////////////////////
/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.0       Date: 2007/10/19
  Email:  zwj_uptech@126.com
  Description:    // YUV阈值结构体
  Others:         //
*************************************************/

#if !defined(AFX_YUVPARAM_H__717E93F0_4A9A_429C_AED3_51936C051128__INCLUDED_)
#define AFX_YUVPARAM_H__717E93F0_4A9A_429C_AED3_51936C051128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct YUVParam
{
    long Ymin, Ymax;
    long Yx, Yy, Ysum;
    BOOL Yen;
    long Umin, Umax;
    long Ux, Uy, Usum;
    BOOL Uen;
    long Vmin, Vmax;
    long Vx, Vy, Vsum;
    BOOL Ven;

    void Clear()
    {
        Yx = Yy = Ysum = 0;
        Ux = Uy = Usum = 0;
        Vx = Vy = Vsum = 0;
    }

    void ReSet()
    {
        Ymin = Ymax = Yx = Yy = Ysum = 0;
        Umin = Umax = Ux = Uy = Usum = 0;
        Vmin = Vmax = Vx = Vy = Vsum = 0;
        Yen = Uen = Ven = FALSE;
        bBusy = FALSE;
    }

    BOOL bBusy;
};

#endif // !defined(AFX_YUVPARAM_H__717E93F0_4A9A_429C_AED3_51936C051128__INCLUDED_)
