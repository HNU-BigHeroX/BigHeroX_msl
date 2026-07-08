// VoyHSI.h: interface for the VoyHSI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOYHSI_H__D78F00D2_08FB_4A33_A064_65D0BA955151__INCLUDED_)
#define AFX_VOYHSI_H__D78F00D2_08FB_4A33_A064_65D0BA955151__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HSIParam.h"

struct CameraProperty
{
    long nBrightness;
    long nContrast;
    // long nHue;
    long nSaturation;
    long nSharpness;
    // long nGamma;
    // long nColorEnable;
    long nWhiteBalance;
    long nBacklightCompensation;
    long nGain;

    long nExposure;
};

typedef struct VoyHSISet
{
    HSIParam black;      // 黑色障碍
    HSIParam green;      // 绿色地毯
    HSIParam whitelines; // 白色场线
    HSIParam overball;   // 全景球
    HSIParam teammate;   // 我方队标
    HSIParam opponent;   // 对手队标
    int year, month, day, hour, minute, second;

    CameraProperty cameraproc;
} HSISet;

class VoyHSI
{
  public:
    BOOL LoadHSIFromINI(CString inFilename, HSISet *setting, CListBox *inList);
    void SaveHSIToINI(CString inFilename, HSISet *setting, CListBox *inList);
    void Load(CString filename, HSISet *setting, CListBox *inList);
    void SaveToTxt(CString filename, HSISet setting);
    void Save(CString filename, HSISet setting);
    BOOL Load(CString filename, HSISet *setting);
    VoyHSI();
    virtual ~VoyHSI();
};

#endif // !defined(AFX_VOYHSI_H__D78F00D2_08FB_4A33_A064_65D0BA955151__INCLUDED_)
