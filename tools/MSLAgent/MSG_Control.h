// MSG_Control.h: MSG_Control struct.
//
//////////////////////////////////////////////////////////////////////

/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.2       Date: 2008/11/04
  Email:  zwj_uptech@126.com
  Description:    // RoboCup远程控制指令结构体
  Others:         //
*************************************************/

#if !defined(AFX_MSG_CONTROL_H__F41F9D4C_4AB4_4C2A_9B68_F3C05F91D61E__INCLUDED_)
#define AFX_MSG_CONTROL_H__F41F9D4C_4AB4_4C2A_9B68_F3C05F91D61E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct MSG_Control
{
    UINT ctrl;

    bool targetGoal;
    int target_x;
    int target_y;
    double target_angle;

    int re_ballx;
    int re_bally;
};

#endif // !defined(AFX_MSG_CONTROL_H__F41F9D4C_4AB4_4C2A_9B68_F3C05F91D61E__INCLUDED_)
