// CCaptureVideo.h
/////////////////////////////////////////////////////////////////////
/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.3       Date: 2008/10/21
  Email:  zwj_uptech@126.com
  Description:    // 视频捕捉类
  Others:         // 1.0 参考俞科的代码2007/10/19
                  // 1.1 降低帧率，去除图像紫边现象 2008/10/21
                  // 1.2 加入摄像头属性设置 2008/11/6
                  // 1.3 加入视频仿真 已不具备通用性
*************************************************/

#if !defined(AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_)
#define AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <atlbase.h>
#include <windows.h>
#include <dshow.h>
// #include <afxsock.h>		// MFC socket extensions
#include <atlbase.h>
#include <streams.h>
#include <qedit.h>
#include "VoyCmd.h"
#include "VoyHSI.h"

#define AHEAD 1
#define OVERLOOK 2

#define BUFIMGWIDTH 640
#define BUFIMGHEIGHT 480

class CSampleGrabberCB : public ISampleGrabberCB
{
  public:
    // 捕捉设备位置ID
    UINT m_nPosID;

    // 截图标记
    BOOL bOneShot;

    // 视觉行为处理线程同步标记
    volatile bool bProcess; // 行为处理标记
    volatile bool bBuffer;  // 图像缓冲标记

    // 视频图像送行为缓冲
    BYTE imgbuf[BUFIMGWIDTH * BUFIMGHEIGHT * 3];

    // 协议对象指针
    CVoyCmd *m_pCmd;

    // 显示窗体句柄
    HWND m_hStill;

    long lWidth;
    long lHeight;
    double dbTime;
    TCHAR m_szFileName[MAX_PATH]; // 位图文件名称
    CString m_strBMPFile;

    // emu
    HWND m_emuhwnd;
    BITMAPINFOHEADER m_emubih;
    UCHAR *m_emubuf;

    CSampleGrabberCB()
    {
        m_socket = INVALID_SOCKET;
        m_pCmd = NULL;
        bProcess = false;
        bBuffer = false;
        m_nPosID = OVERLOOK;

        // emu
        memset(&m_emubih, 0, sizeof(m_emubih));
        m_emubih.biSize = sizeof(m_emubih);
        m_emubih.biWidth = 640;
        m_emubih.biHeight = 480;
        m_emubih.biPlanes = 1;
        m_emubih.biBitCount = 24;
    }

    STDMETHODIMP_(ULONG) AddRef()
    {
        return 2;
    }

    STDMETHODIMP_(ULONG) Release()
    {
        return 1;
    }

    STDMETHODIMP QueryInterface(REFIID riid, void **ppv)
    {
        if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown)
        {
            *ppv = (void *)static_cast<ISampleGrabberCB *>(this);
            return NOERROR;
        }
        return E_NOINTERFACE;
    }

    STDMETHODIMP SampleCB(double SampleTime, IMediaSample *pSample)
    {
        // emu
        pSample->GetPointer(&m_emubuf);
        switch (m_nPosID)
        {
        case AHEAD:
            m_pCmd->m_pBeh->AfterUpdateVideoSample(m_emubuf, lWidth, lHeight, SampleTime, 0);
            break;
        case OVERLOOK:
            m_pCmd->m_pBeh->AfterUpdateOverlook(m_emubuf, lWidth, lHeight, SampleTime, 0);
            break;
        }

        // Display
        DisplayCapturedBits(m_emubuf, &m_emubih, m_emuhwnd);

        return 0;
    }

    STDMETHODIMP BufferCB(double dblSampleTime, BYTE *pBuffer, long lBufferSize)
    {
        if (!pBuffer)
            return E_POINTER;

        if (m_pCmd != NULL)
        {
            if (m_pCmd->m_pBeh != NULL)
            {
                /*if (bProcess == false)
                {
                    memcpy(imgbuf,pBuffer,lBufferSize);
                    dbTime = dblSampleTime;
                    bBuffer = true;		//true表示缓冲了新图像，线程中处理完行为会置回false
                }*/
                switch (m_nPosID)
                {
                case AHEAD:
                    m_pCmd->m_pBeh->AfterUpdateVideoSample(pBuffer, lWidth, lHeight, dblSampleTime, m_pCmd->nState);
                    break;
                case OVERLOOK:
                    m_pCmd->m_pBeh->AfterUpdateOverlook(pBuffer, lWidth, lHeight, dblSampleTime, m_pCmd->nState);
                    break;
                }
            }
        }

        // 发送图像
        if (m_socket != INVALID_SOCKET)
        {
            send(m_socket, (char *)pBuffer, lBufferSize, 0);
        }

        // 根据截图标记确定是否截取静态图片
        if (bOneShot)
        {
            SaveBitmap(pBuffer, lBufferSize, m_strBMPFile);
            bOneShot = FALSE;
        }

        return 0;
    }

    // 创建位图文件
    BOOL SaveBitmap(BYTE *pBuffer, long lBufferSize, CString strFileName)
    {
        strcpy(m_szFileName, strFileName);
        HANDLE hf = CreateFile(m_szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
        if (hf == INVALID_HANDLE_VALUE)
            return 0;

        // 写文件头
        BITMAPFILEHEADER bfh;
        memset(&bfh, 0, sizeof(bfh));
        bfh.bfType = 'MB';
        bfh.bfSize = sizeof(bfh) + lBufferSize + sizeof(BITMAPINFOHEADER);
        bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
        DWORD dwWritten = 0;
        WriteFile(hf, &bfh, sizeof(bfh), &dwWritten, NULL);

        // 写位图格式
        BITMAPINFOHEADER bih;
        memset(&bih, 0, sizeof(bih));
        bih.biSize = sizeof(bih);
        bih.biWidth = lWidth;
        bih.biHeight = lHeight;
        bih.biPlanes = 1;
        bih.biBitCount = 24;
        WriteFile(hf, &bih, sizeof(bih), &dwWritten, NULL);

        // 写位图数据
        WriteFile(hf, pBuffer, lBufferSize, &dwWritten, NULL);
        CloseHandle(hf);

        // 将截获图片显示到界面上
        if (m_hStill != NULL)
        {
            DisplayCapturedBits(pBuffer, &bih, m_hStill);
        }
        return 0;
    }

    BOOL DisplayCapturedBits(BYTE *pBuffer, BITMAPINFOHEADER *pbih, HWND hwndStill)
    {
        RECT rc;
        ::GetWindowRect(hwndStill, &rc);
        long lStillWidth = rc.right - rc.left;
        long lStillHeight = rc.bottom - rc.top;

        HDC hdcStill = GetDC(hwndStill);
        PAINTSTRUCT ps;
        BeginPaint(hwndStill, &ps);

        SetStretchBltMode(hdcStill, COLORONCOLOR);
        StretchDIBits(hdcStill, 0, 0, lStillWidth, lStillHeight, 0, 0, lWidth, lHeight, pBuffer, (BITMAPINFO *)pbih,
                      DIB_RGB_COLORS, SRCCOPY);

        EndPaint(hwndStill, &ps);
        ReleaseDC(hwndStill, hdcStill);

        return TRUE;
    }

    SOCKET m_socket;

  protected:
};

class CCaptureVideo
{
    friend class CSampleGrabberCB;

  public:
    void DefaultSet();
    BOOL SetFormat(long lWidth, long lHeight, unsigned short iColorBit, __int64 iRate);
    BOOL DisplayPropPage(HWND inOwner);
    void SetPosID(UINT inPosID);
    HRESULT Create(int iDeviceID, CStatic *inpPrev, int nMode); // 开启视频函数(可直接使用)
    void SetCmd(CVoyCmd *inpCmd);                               // 与指令类接口
    BOOL ConnectTo(char *inpIP, int inDesPort);                 // 与图像接收端连接
    BOOL SetFileDir(CString inDir);                             // 设置图像文件保存目录
    void GrabOneFrame(CStatic *inpShow);                        // 截图操作
    int EnumDevices(HWND hList);                                // 枚举摄像设备到list（仅显示）
    HRESULT Stop();                                             // 停止预览
    CCaptureVideo();
    virtual ~CCaptureVideo();
    CSampleGrabberCB mCB;

  protected:
    HWND m_hPreviewWnd;                 // 御览显示窗体
    IGraphBuilder *m_pGB;               // GraphBuilder
    ICaptureGraphBuilder2 *m_pCapture;  //
    IBaseFilter *m_pBF;                 // 绑定filter
    IMediaControl *m_pMC;               // 媒体控制器指针
    IVideoWindow *m_pVW;                // 显示窗体指针
    CComPtr<ISampleGrabber> m_pGrabber; //

    void GetCompFilter(IBaseFilter **pCompFilter);
    void FreeMediaType(AM_MEDIA_TYPE &mt);
    bool BindFilter(int deviceId, IBaseFilter **pFilter);
    void ResizeVideoWindow();
    HRESULT SetupVideoWindow();
    HRESULT InitCaptureGraphBuilder();
    bool ClearGraphs();

    // 截图和录像
    CString m_strBaseDir;
    CString m_strCurDir;
    CString m_strVideoFile;

    // emu
    CComPtr<IMediaDet> pDet;

  public:
    void Pause();
    HRESULT EmuOverlook(CString infilename, HWND inDishwnd);
    void Bar_Init(CameraControlProperty inproc, CSliderCtrl *pslider);
    void Bar_Init(VideoProcAmpProperty inproc, CSliderCtrl *pslider);
    bool GetExposure(long *inpExposure);
    void GetCameraProperty(CameraProperty *inCP);
    void SetCameraProperty(CameraProperty *inCP);
    BOOL Restart();
    bool SetExposure(bool bAuto, long lExposure);
    bool m_bCapStatus; // 工作状态

    CameraProperty m_DefProc;
};
#endif // !defined(AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_)
