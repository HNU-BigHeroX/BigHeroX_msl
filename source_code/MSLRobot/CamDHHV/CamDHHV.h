// CamDHHV.h: interface for the CCamDHHV class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMDHHV_H__C98C8004_E5E6_4724_970D_A9CC192E7FF2__INCLUDED_)
#define AFX_CAMDHHV_H__C98C8004_E5E6_4724_970D_A9CC192E7FF2__INCLUDED_

#include "INC\HVDef.h" // Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Inc/Raw2Rgb.h"
#include "Inc/HVDAILT.h"
#include "..\VoyInclude\VoyCmd.h"
#include "StillView.h"
#include "afxmt.h"

// 定义自己的0
#define MY_ZERO 0.000000001

// 定义彩色图像还是黑白图像
typedef enum tagHV_IMAGE_MODE
{
    HV_COLOR = 0,
    HV_BW = 1
} HV_IMAGE_MODE;

class CCamDHHV
{
  public:
    BOOL SetLutTable(double dRatioR, double dRatioG, double dRatioB);
    void SetImgSize(int inLeft, int inTop, int inWidth, int inHeight);
    void ResetFrameCount();
    void SetAllGain(int inGain);
    void SaveParamToINI(CString inFilename);
    bool Open();
    void InitHVDevice();
    void SetExposureTime(int nWindWidth, long lTintUpper, long lTintLower);
    void DrawCallback();
    HVSTATUS GetLastStatus();
    HRESULT Create(int iDeviceID, CStatic *inpPrev, int nMode);
    static int CALLBACK SnapThreadCallback(HV_SNAP_INFO *pInfo); // Snap 回调函数
    CCamDHHV();
    virtual ~CCamDHHV();

  public:
    void SetDisView(int inLeft, int inTop);
    void BehThread();
    int GetExpLower();
    int GetExpUpper();
    void SetGain(int inIndex, int val);
    int GetGain(int inIndex);
    int GetADCLevel();
    void SetADCLevel(int inLevel);
    void GetWBRatio(double *dRatioR, double *dRatioG, double *dRatioB);
    void SetLutSingle(int inIndex, double inVal);
    int GetImgWidth();
    void SetCmd(CVoyCmd *inpCmd);
    CString DeviceInfo(int inIndex);
    void End();
    void LoadParamFromINI(CString inFilename);
    bool m_bDrawToScreen;
    bool m_bDrawOrg;
    CStillView *m_pView;

    // 同步用
    bool bSycnToBeh;
    bool bSycnNewFrame;
    bool bToDestroy;
    bool m_bDisplay;

    int nViewX;
    int nViewY;

  protected:
    void m_SnapexClose();
    void m_SnapexStop();
    void m_SnapexStart();
    void m_SnapexOpen();
    CStatic *m_pPreview; // 显示控件
    int m_DisWidth;
    int m_DisHeight;
    HHV m_hhv;       // 数字摄像机句柄
    BOOL m_bOpen;    // 初始化标志
    BOOL m_bStart;   // 启动标志
    int m_nDevIndex; // 相机序号

    BITMAPINFO *m_pBmpInfo;      // BITMAPINFO 结构指针，显示图像时使用
    BYTE *m_pRawBuffer;          // 采集图像原始数据缓冲区
    BYTE *m_pImageBuffer;        // Bayer转换后缓冲区
    char m_chBmpBuf[2048];       // BIMTAPINFO 存储缓冲区，m_pBmpInfo即指向此缓冲区
    CCriticalSection m_csImgBuf; // 临界区

    // 颜色查找表
    BYTE m_pLutR[256];
    BYTE m_pLutG[256];
    BYTE m_pLutB[256];

    BYTE m_pLutGamma[256];
    BOOL m_bIsGammaCorrect;
    int dwLastStatus;

    //////////////////////////////////////////////////////////////////////////
    HV_RESOLUTION m_Resolution; // 分辨率模式
    HV_SNAP_MODE m_SnapMode;    // 视频捕获模式

    long m_lGain[4]; // 四个通道的增益
    int m_lADCLevel; // AD模式

    // White balance ratio
    double m_dRatioR;
    double m_dRatioG;
    double m_dRatioB;

    HV_IMAGE_MODE m_ImageMode;
    HV_BAYER_LAYOUT m_Layout;

    DWORD m_dwCount;
    DWORD m_dwStart;
    DWORD m_dwBehCnt;

    CString m_strDeviceType;

    // the conversion method of raw to rgb
    HV_BAYER_CONVERT_TYPE m_ConversionType;
    // the variable to control the snap speed
    HV_SNAP_SPEED m_SnapSpeed;
    int m_nHBlanking;
    int m_nVBlanking;

    // 映射文件句柄
    HANDLE m_hFileMapping;
    // 映射内存指针
    PBYTE m_pLutMapping;

    double m_dGammRatio;  // gamma 校正系数
    int *m_pPatternBuf;   // 用来进行模板测试的缓冲区
    BYTE *m_pBadPixelBuf; // 保存坏点位置的内存
    int m_nTempTimes;     // 取模板的次数

    // 曝光时间的分子和分母
    long m_nExposureTint_Upper;
    long m_nExposureTint_Lower;

    // 有效图像区域
    int m_nImgLeft;
    int m_nImgTop;
    int m_nImgWidth;
    int m_nImgHeight;

    BOOL m_bIsToGetBadPixel;
    BOOL m_bIsToRemoveBadPixel;
    BOOL m_bIsToGetTemplate;
    BOOL m_bIsTemplateCorrect;

    BOOL m_bIsSnapSpeedSprted; // 本摄像机是否支持设置采集速度
    BOOL m_bIsBlkSizeSprted;   // 本摄像机是否支持设置取得消隐取值范围

    float m_fpsCap;
    float m_fpsBeh;
    CVoyCmd *m_pCmd;
    CString strTmp;
    //////////////////////////////////////////////////////////////////////////
    unsigned char *m_pToDrawBuf;
};

#endif // !defined(AFX_CAMDHHV_H__C98C8004_E5E6_4724_970D_A9CC192E7FF2__INCLUDED_)
