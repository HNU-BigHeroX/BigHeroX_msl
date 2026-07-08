// MSG_Status.h: MSG_Status struct.
// V1.0
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSG_STATUS_H__EE274A02_1F29_48F6_806E_C4D81D6A6454__INCLUDED_)
#define AFX_MSG_STATUS_H__EE274A02_1F29_48F6_806E_C4D81D6A6454__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define CMTKEY 0x11112222

struct MSG_Status
{
    UINT ID;
    UINT position;
    UINT status;

    int leftspeed;
    int rightspeed;
    float speedProp;

    int ball_x;
    int ball_y;

    int blue_x;
    int blue_y;

    int yellow_x;
    int yellow_y;
};

// position
#define REMOTE 0
#define ATTACK 1
#define DEFENCE 2

// STATUS
#define ERROR 0
#define TOBALL 1
#define TODOOR 2
#define BLOCK 3

//

#endif // !defined(AFX_MSG_STATUS_H__EE274A02_1F29_48F6_806E_C4D81D6A6454__INCLUDED_)
