// ICCamera.h: interface for the CICCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICCAMERA_H__EE1C7AB6_6F68_4CEB_ABFA_CF59889D67CC__INCLUDED_)
#define AFX_ICCAMERA_H__EE1C7AB6_6F68_4CEB_ABFA_CF59889D67CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <tisgrabber.h>
#include <cv.h>
#include <highgui.h>
#include "VoyInclude/GetImage.h"

class CICCamera
{
  public:
    void CvDisplayVideo(bool inEn);
    void ShowVideoLive(BOOL inShowWindow);
    void GreenCenter_RedRow(BYTE *pBuf);
    void GreenCenter_BlueRow(BYTE *pBuf);
    void BlueCenter(BYTE *pBuf);
    void RedCenter(BYTE *pBuf);
    void MalvarDemosaic(BYTE *inBuf, int inWidth, int inHeight);
    void MasaicGBRG(BYTE *inBuf, int inWidth, int inHeight);
    float GetPrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, float fDefault, LPCSTR lpFileName);
    void LoadParamFromINI(CString inFileName);
    void SaveParamToINI(CString inFileName);
    void LoadDeviceStateFromFile(CString inFileName);
    void LoadDeviceSettings(CString inFileName);
    void SetWindowPosition(int inX, int inY, int inWidth, int inHeight);
    void SetFrameRate(float inFps);
    void ShowInternalPropertyPage();
    void SaveDeviceSettings(CString inFileName);
    void SaveDeviceStateToFile(CString inFileName);
    void DisplayText(int inX, int inY, CString inStr, int inSize);
    void ShowPropertyDialog();
    void SetDisplayWnd(CWnd *inWnd);
    void PrintToInfoList(CString inStr);
    void CapturePicture(CString inFileName);
    void Close();
    BOOL Open(BOOL inShowWindow);
    CICCamera();
    virtual ~CICCamera();

    CListBox *pInfoList;
    int nFrameCount;
    bool bBlockTest;

    unsigned char *pTmpBuf;
    float m_fFPS_Set;

    CGetImage *pGetImage;
    int m_nOffSetX;
    int m_nOffSetY;

    IplImage *iplImage;
    BOOL m_bShowWindow; // ÊÇ·ñÏÔÊ¾Í¼Ïñ

    int m_nWidth;
    int m_nHeight;
    int m_nWidthBytes;

  protected:
    HGRABBER hGrabber;
    HWND m_hwndDisplay;
    int m_nInited;
    int m_nDevOpen;

    int tmp[5];
    int res;
};

#endif // !defined(AFX_ICCAMERA_H__EE1C7AB6_6F68_4CEB_ABFA_CF59889D67CC__INCLUDED_)
