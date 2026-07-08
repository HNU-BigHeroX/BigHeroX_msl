// VoySetting.h: interface for the CVoySetting class.
//
//////////////////////////////////////////////////////////////////////

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

class CVoySetting
{
  public:
    BOOL Load(CString filename, VoySet *setting);
    void Save(CString filename, VoySet setting);
    CVoySetting();
    virtual ~CVoySetting();
};

#endif // !defined(AFX_VOYSETTING_H__38E130BB_F52D_4D1F_AE75_4F6CDA398455__INCLUDED_)
