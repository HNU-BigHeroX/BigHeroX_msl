// VoySetting.h: interface for the CVoySetting class.
//
//////////////////////////////////////////////////////////////////////
/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.0       Date: 2007/10/19
  Email:  zwj_uptech@126.com
  Description:    // VoyagerII常用设置及其保存读取操作
  Others:         //
*************************************************/

#if !defined(AFX_VOYSETTING_H__38E130BB_F52D_4D1F_AE75_4F6CDA398455__INCLUDED_)
#define AFX_VOYSETTING_H__38E130BB_F52D_4D1F_AE75_4F6CDA398455__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct VoySet
{
    UINT Port;
    UINT BaudRate;
    BOOL USEnable[24];
    int Speed;
    float LSpdCoe;
    float RSpdCoe;
    UINT Reserve;
};

struct Voy3Set
{
    UINT Port;
    UINT BaudRate;
    double Speed;
    UINT nOverlookIndex;
    UINT nAheadIndex;

    // to coach
    char CoachIP[16];
    UINT CoachPort;

    // id
    UINT AgentID;

    // param
    double HoldBall;

    // 罗盘port
    UINT nCompPort;

    // 画面偏移
    int nCameraOffsetX;
    int nCameraOffsetY;
};

class CVoySetting
{
  public:
    BOOL Load(CString filename, Voy3Set *setting);
    void Save(CString filename, Voy3Set setting);
    CVoySetting();
    virtual ~CVoySetting();
};

#endif // !defined(AFX_VOYSETTING_H__38E130BB_F52D_4D1F_AE75_4F6CDA398455__INCLUDED_)
