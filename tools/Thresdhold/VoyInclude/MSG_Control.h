// MSG_Control.h: MSG_Control struct.
// V1.0
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSG_CONTROL_H__F41F9D4C_4AB4_4C2A_9B68_F3C05F91D61E__INCLUDED_)
#define AFX_MSG_CONTROL_H__F41F9D4C_4AB4_4C2A_9B68_F3C05F91D61E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define CMTKEY 0x11112222

struct MSG_Control
{
    UINT ID;
    UINT ctrl;
    int lSpeed;
    int rSpeed;
    float speedProp;
    UINT Key;
};

// ctrl
#define CTRL_REMOTE 0
#define CTRL_ATTACK 1
#define CTRL_DEFENCE 2

#endif // !defined(AFX_MSG_CONTROL_H__F41F9D4C_4AB4_4C2A_9B68_F3C05F91D61E__INCLUDED_)
