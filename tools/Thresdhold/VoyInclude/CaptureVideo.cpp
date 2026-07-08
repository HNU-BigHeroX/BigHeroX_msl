//-------------------------------------------------------------------
// CCaptureVideo视频捕捉类实现文件CaptureVideo.cpp
//-------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CaptureVideo.h"
#include <windows.h>
#include <io.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// 视觉行为调用线程
//////////////////////////////////////////////////////////////////////
UINT VisionThread(LPVOID pParam)
{
    CCaptureVideo *pCap = (CCaptureVideo *)pParam;
    // 只在捕捉状态有效
    while (pCap->m_bCapStatus == true)
    {
        if (pCap->mCB.bBuffer == true)
        {
            // 标记变量显示有新的图像缓冲，调用行为处理函数
            pCap->mCB.bProcess = true;
            switch (pCap->mCB.m_nPosID)
            {
            case AHEAD:
                // pCap->mCB.m_pCmd->m_pBeh->AfterUpdateVideoSample(pCap->mCB.imgbuf,pCap->mCB.lWidth,pCap->mCB.lHeight,pCap->mCB.dbTime,pCap->mCB.m_pCmd->nState);
                break;
            case OVERLOOK:
                //	pCap->mCB.m_pCmd->m_pBeh->AfterUpdateOverlook(pCap->mCB.imgbuf,pCap->mCB.lWidth,pCap->mCB.lHeight,pCap->mCB.dbTime,pCap->mCB.m_pCmd->nState);
                break;
            }
            pCap->mCB.bProcess = false;
            pCap->mCB.bBuffer = false; // 防止对同一图像缓冲进行重复处理
        }
    }
    return 0L;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCaptureVideo::CCaptureVideo()
{
    // COM Library Intialization
    if (FAILED(CoInitialize(NULL))) /*, COINIT_APARTMENTTHREADED)))*/
    {
        AfxMessageBox("COM初始化失败!\r\n");
        return;
    }
    m_hPreviewWnd = NULL;
    m_pVW = NULL;
    m_pMC = NULL;
    m_pGB = NULL;
    m_pBF = NULL;
    m_pCapture = NULL;
    m_bCapStatus = false;
    m_strVideoFile.Format(".\\video.avi");

    char *pbuff = new char[255];
    GetCurrentDirectory(255, pbuff);
    m_strBaseDir = pbuff;
    delete[] pbuff;
    pbuff = NULL;

    m_DefProc.nBrightness = 125;
    m_DefProc.nContrast = 20;
    m_DefProc.nSaturation = 20;
    m_DefProc.nSharpness = 179;
    m_DefProc.nWhiteBalance = 5000;
    m_DefProc.nBacklightCompensation = 1;
    m_DefProc.nGain = 50;
    m_DefProc.nExposure = -5;
}

CCaptureVideo::~CCaptureVideo()
{
    // 停止回放
    if (m_pMC)
        m_pMC->Stop();
    if (m_pVW)
    {
        m_pVW->put_Visible(OAFALSE);
        m_pVW->put_Owner(NULL);
    }
    /*if(!m_bCapStatus)
        return;*/

    if (NULL != m_pCapture)
    {
        m_pCapture->Release();
        m_pCapture = NULL;
    }
    if (NULL != m_pMC)
    {
        m_pMC->Release();
        m_pMC = NULL;
    }
    if (NULL != m_pGB)
    {
        m_pGB->Release();
        m_pGB = NULL;
    }
    if (NULL != m_pBF)
    {
        m_pBF->Release();
        m_pBF = NULL;
    }
    CoUninitialize();
}

// 枚举摄像设备，并将名称添加到list框里
int CCaptureVideo::EnumDevices(HWND hList)
{
    if (!hList)
        return -1;
    int id = 0;

    // 枚举视频扑捉设备（这次枚举是为了往list框里添加名称，后边使用时得再次枚举）
    ICreateDevEnum *pCreateDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
                                  (void **)&pCreateDevEnum);

    if (hr != NOERROR)
        return -1;
    CComPtr<IEnumMoniker> pEm;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);

    if (hr != NOERROR)
        return -1;
    pEm->Reset();
    ULONG cFetched;
    IMoniker *pM;
    while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK)
    {
        IPropertyBag *pBag;
        hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
        if (SUCCEEDED(hr))
        {
            VARIANT var;
            var.vt = VT_BSTR;
            hr = pBag->Read(L"FriendlyName", &var, NULL);
            if (hr == NOERROR)
            {
                TCHAR str[2048];
                id++;
                WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, str, 2048, NULL, NULL);
                ::SendMessage(hList, CB_ADDSTRING, 0, (LPARAM)str); // 向界面列表框添加视频设备
                SysFreeString(var.bstrVal);
            }
            pBag->Release();
        }
        pM->Release();
    }
    return id;
}

HRESULT CCaptureVideo::Create(int iDeviceID, CStatic *inpPrev, int nMode)
{
    if (inpPrev != NULL)
    {
        m_hPreviewWnd = inpPrev->GetSafeHwnd();
    }

    if (nMode == 1)
    {
        SetCurrentDirectory(m_strBaseDir);
        SetCurrentDirectory(m_strCurDir);
        // 确定视频捕捉文件名
        char FileBuf[20];
        for (int i = 0; i < 65535; i++)
        { // 试探是否已存在捕捉文件
            sprintf(FileBuf, "Video%05d.AVI", i);
            if ((access(FileBuf, 0)) == -1)
                break;
        }
        m_strVideoFile = FileBuf;
    }

    // 使用m_bCapStatus标记防止重复初始化出错,true表示曾经初始化
    if (m_bCapStatus)
        return NULL;

    // 初始化CaptureGraph
    HRESULT hr;
    hr = InitCaptureGraphBuilder();
    if (FAILED(hr))
    {
        AfxMessageBox("获取视频捕捉接口失败!");
        return hr;
    }

    // 将m_pBF绑定到摄像头设备上,添加到graph里
    if (!BindFilter(iDeviceID, &m_pBF))
    {
        AfxMessageBox("绑定视频设备失败！");
        return S_FALSE;
    }

    hr = m_pGB->AddFilter(m_pBF, L"Capture Filter");
    if (FAILED(hr))
    {
        AfxMessageBox("Capture Filter添加失败！");
    }

    // 创建一个sample grabber
    hr = m_pGrabber.CoCreateInstance(CLSID_SampleGrabber);
    if (!m_pGrabber)
    {
        AfxMessageBox("创建SampleGrabber Filter失败，请确认qedit.dll是否已注册");
        return hr;
    }
    // 将指针转化到标准参数
    CComQIPtr<IBaseFilter, &IID_IBaseFilter> pGrabBase(m_pGrabber);

    // 设置视频格式
    AM_MEDIA_TYPE mt;
    ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
    mt.majortype = MEDIATYPE_Video;
    mt.subtype = MEDIASUBTYPE_RGB24;
    // 将媒体格式设置到m_pGrabber
    hr = m_pGrabber->SetMediaType(&mt);

    if (FAILED(hr))
    {
        AfxMessageBox("设置媒体类型失败");
        return hr;
    }

    // 往graph添加grabberfilter，截图
    hr = m_pGB->AddFilter(pGrabBase, L"Grabber");
    if (FAILED(hr))
    {
        AfxMessageBox("尝试将grabberfilter加入Graph失败！");
        return hr;
    }

    // 枚举系统中的压缩编码filter
    CComPtr<IBaseFilter> pCompFilter;
    GetCompFilter(&pCompFilter);
    if (!pCompFilter)
        return false;

    /*/向graph中添加压缩编码filter
    hr = m_pGB->AddFilter( pCompFilter, L"Compress" );
    if( FAILED( hr ) )
    {
        return false;
    }

    //AVI文件filter
    CComPtr< IBaseFilter > pMux;
    CComPtr< IFileSinkFilter > pSink;
    USES_CONVERSION;
    if (nMode)
    {
        hr = m_pCapture->SetOutputFileName( &MEDIASUBTYPE_Avi,  T2W(
    m_strVideoFile.GetBuffer(m_strVideoFile.GetLength()) ), &pMux, &pSink ); if( FAILED( hr ) )
         {
            AfxMessageBox( TEXT("创建Mux and Writer Filter失败！"));
            return false;
         }
    }*/

    // 智能连接preview pin
    hr = m_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pBF, pGrabBase, NULL);
    if (FAILED(hr))
    {
        AfxMessageBox("Preview Pin连接失败！");
        return hr;
    }

    // 根据模式标记进行Capture Pin 的连接
    switch (nMode)
    {
    case 0:
        break;
    case 1:
        // hr = m_pCapture->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,m_pBF,NULL,pMux);
        break;
    case 2:
        // hr = m_pCapture->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,m_pBF,pCompFilter,pMux);
        break;
    default:
        break;
    }

    if (FAILED(hr))
    {
        AfxMessageBox("Capture Pin 连接失败！");
        return hr;
    }

    // 从Grabber提取媒体格式
    hr = m_pGrabber->GetConnectedMediaType(&mt);
    if (FAILED(hr))
    {
        AfxMessageBox("Grabber Filter获取视频参数失败！");
        return hr;
    }

    // 设置截图缓冲区大小
    VIDEOINFOHEADER *vih = (VIDEOINFOHEADER *)mt.pbFormat;
    mCB.lWidth = vih->bmiHeader.biWidth;
    mCB.lHeight = vih->bmiHeader.biHeight;
    FreeMediaType(mt);

    hr = m_pGrabber->SetBufferSamples(FALSE); // 不再另外开辟单桢缓冲区
    hr = m_pGrabber->SetOneShot(FALSE);       // 截图后继续运行graph
    hr = m_pGrabber->SetCallback(&mCB, 1);    ////设置回调函数，0=SampleCb,1=BufferCb

    // 设置视频预览窗口
    if (m_hPreviewWnd != NULL)
    {
        SetupVideoWindow();
    }
    switch (mCB.m_nPosID)
    {
    case AHEAD:
        SetFormat(320, 240, 24, 30);
        break;
    case OVERLOOK:
        SetFormat(640, 480, 24, 15);
        // SetFormat(320,240,24,30);
        break;
    }

    // 开始运行
    hr = m_pMC->Run();

    // 	HWND h;
    // 	h = ::FindWindow(NULL,"XviD Status");
    // 	::PostMessage(h,WM_CLOSE,NULL,NULL);

    if (FAILED(hr))
    {
        AfxMessageBox("启动graph失败!");
        return hr;
    }

    // 启动运行标记
    m_bCapStatus = true;

    // DefaultSet();

    return S_OK;
}

bool CCaptureVideo::BindFilter(int deviceId, IBaseFilter **pFilter) // 参数一是枚举到的序号，参数二是要绑定的指针
{
    if (deviceId < 0)
        return false;

    // 枚举视频输入设备
    CComPtr<ICreateDevEnum> pCreateDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
                                  (void **)&pCreateDevEnum);
    if (hr != NOERROR)
    {
        return false;
    }
    CComPtr<IEnumMoniker> pEm;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
    if (hr != NOERROR)
    {
        return false;
    }
    pEm->Reset();
    ULONG cFetched;
    IMoniker *pM;
    int index = 0;
    while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK, index <= deviceId)
    {
        IPropertyBag *pBag;
        hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
        if (SUCCEEDED(hr))
        {
            VARIANT var;
            var.vt = VT_BSTR;
            hr = pBag->Read(L"FriendlyName", &var, NULL);
            if (hr == NOERROR)
            {
                if (index == deviceId)
                {                                                              // 生成绑定到设备上的filter
                    pM->BindToObject(0, 0, IID_IBaseFilter, (void **)pFilter); // 生成绑定到设备上的filter
                }
                SysFreeString(var.bstrVal);
            }
            pBag->Release();
        }
        pM->Release();
        index++;
    }
    return true;
}

HRESULT CCaptureVideo::InitCaptureGraphBuilder()
{
    HRESULT hr;

    // 创建IGraphBuilder接口
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGB);
    // 创建ICaptureGraphBuilder2接口
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2,
                          (void **)&m_pCapture);
    if (FAILED(hr))
        return hr;
    m_pCapture->SetFiltergraph(m_pGB);
    hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
    if (FAILED(hr))
        return hr;
    hr = m_pGB->QueryInterface(IID_IVideoWindow, (LPVOID *)&m_pVW);
    if (FAILED(hr))
        return hr;
    return hr;
}

HRESULT CCaptureVideo::SetupVideoWindow()
{ // 设置显示窗体控件的属性
    HRESULT hr;
    hr = m_pVW->put_Owner((OAHWND)m_hPreviewWnd);
    if (FAILED(hr))
        return hr;
    hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
    if (FAILED(hr))
        return hr;
    ResizeVideoWindow();
    hr = m_pVW->put_Visible(OATRUE);
    return hr;
}

void CCaptureVideo::ResizeVideoWindow()
{
    if (m_pVW)
    {
        // 让图像充满整个窗口
        CRect rc;
        ::GetClientRect(m_hPreviewWnd, &rc);
        m_pVW->SetWindowPosition(0, 0, rc.right, rc.bottom);
    }
}

void CCaptureVideo::GrabOneFrame(CStatic *inpShow)
{
    HWND hStill;
    hStill = inpShow->GetSafeHwnd();

    SetCurrentDirectory(m_strBaseDir);
    SetCurrentDirectory(m_strCurDir);
    // 试探是否已存在捕捉文件
    char FileBuf[20];
    for (int i = 0; i < 65535; i++)
    {
        sprintf(FileBuf, "Pic%05d.bmp", i);
        if ((access(FileBuf, 0)) == -1)
            break;
    }
    mCB.m_strBMPFile = FileBuf;

    mCB.m_hStill = hStill;
    mCB.bOneShot = TRUE;
}

void CCaptureVideo::FreeMediaType(AM_MEDIA_TYPE &mt)
{
    if (mt.cbFormat != 0)
    {
        CoTaskMemFree((PVOID)mt.pbFormat);
        mt.cbFormat = 0;
        mt.pbFormat = NULL;
    }
    if (mt.pUnk != NULL)
    {
        mt.pUnk->Release();
        mt.pUnk = NULL;
    }
}

HRESULT CCaptureVideo::Stop()
{
    mCB.m_socket = INVALID_SOCKET;
    // ClearGraphs();
    if (m_pMC != NULL)
    {
        m_pMC->Stop();
    }
    if (m_pVW != NULL)
    {
        m_pVW->put_Visible(OAFALSE);
    }
    return NULL;
}

// 获取压缩graph
void CCaptureVideo::GetCompFilter(IBaseFilter **pCompFilter)
{
    // 枚举系统中的压缩filter
    HRESULT hr;
    ICreateDevEnum *pSysDevEnum = NULL;
    IEnumMoniker *pEnum = NULL;
    IMoniker *pMoniker = NULL;

#ifdef CANUSEFILTER
    int iLoopFilter = 1;
#endif

    hr =
        CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
    if (FAILED(hr))
    {
        AfxMessageBox("枚举不到视频压缩Filter");
        return;
    }

    // 开始枚举压缩filter
    hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoCompressorCategory, &pEnum, 0); // 枚举压缩filter
    if (hr == S_OK) // S_FALSE表示没有枚举到这个类型的filter
    {
        while (S_OK == pEnum->Next(1, &pMoniker, NULL))
        {
            IPropertyBag *pPropBag = NULL;
            pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
            VARIANT var;
            VariantInit(&var);
            hr = pPropBag->Read(L"FriendlyName", &var, 0);
            if (SUCCEEDED(hr))
            {
                // 得到一个可以用的压缩filter
                hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void **)pCompFilter);
                if (SUCCEEDED(hr))
                {
                    IEnumPins *pEnum = NULL;
                    IPin *pPin = NULL;
                    IAMVideoCompression *pCompress = NULL;

                    // 枚举filter上的 pin，寻找支持 IAMVideoCompression 的pin.
                    (*pCompFilter)->EnumPins(&pEnum);
                    while (S_OK == pEnum->Next(1, &pPin, NULL))
                    {
                        hr = pPin->QueryInterface(IID_IAMVideoCompression, (void **)&pCompress);
                        pPin->Release();
                        if (SUCCEEDED(hr)) // 找到这个接口
                        {
                            break;
                        }
                    }
                    if (SUCCEEDED(hr))
                    {
                        long lCap;               // 性能标记
                        long lKeyFrame, lPFrame; // 真实值
                        double m_Quality;
                        long lKeyFrameDef, lPFrameDef; // 默认值
                        double QualityDef;

                        // 获取默认值
                        CString strFilterName = var.bstrVal;
                        if (strFilterName == "Microsoft MPEG-4 Video Codec V3")
                        {
                            hr = pCompress->GetInfo(0, 0, 0, 0, &lKeyFrameDef, &lPFrameDef, &QualityDef, &lCap);
                            if (SUCCEEDED(hr))
                            {
                                // 获取真实值
                                if (lCap & CompressionCaps_CanKeyFrame)
                                {
                                    hr = pCompress->get_KeyFrameRate(&lKeyFrame);
                                    if (FAILED(hr) || lKeyFrame < 0)
                                        lKeyFrame = lKeyFrameDef;
                                }
                                if (lCap & CompressionCaps_CanBFrame)
                                {
                                    hr = pCompress->get_PFramesPerKeyFrame(&lPFrame);
                                    if (FAILED(hr) || lPFrame < 0)
                                        lPFrame = lPFrameDef;
                                }
                                if (lCap & CompressionCaps_CanQuality)
                                {
                                    hr = pCompress->get_Quality(&m_Quality);
                                    if (FAILED(hr) || m_Quality < 0)
                                        m_Quality = QualityDef;
                                }
#ifdef CANUSEFILTER
                                strFilterName = var.bstrVal;
                                if (strFilterName == "Microsoft MPEG-4 Video Codec V3")
                                {
                                    VariantClear(&var);
                                    pPropBag->Release();
                                    pMoniker->Release();
                                    break;
                                }

                                iLoopFilter++;
#else
                                VariantClear(&var);
                                pPropBag->Release();
                                pMoniker->Release();
                                break;
#endif
                            }
                        }
                    }
                    //
                }
            }
            VariantClear(&var);
            pPropBag->Release();
            pMoniker->Release();
        }
    }

    pSysDevEnum->Release();
    pEnum->Release();
}

// 清除整个捕捉Graph
bool CCaptureVideo::ClearGraphs()
{
    if (m_pGB)
    {

        if (m_pMC)
            m_pMC->Stop();
        if (m_pVW)
        {
            m_pVW->put_Visible(OAFALSE);
            m_pVW->put_Owner(NULL);
        }
        m_pGB->Release();
    }
    return true;
}

BOOL CCaptureVideo::SetFileDir(CString inDir)
{
    // 定位在当前目录
    LPCSTR lpszPath = m_strBaseDir;
    SetCurrentDirectory(lpszPath);

    // 创建存储目录
    lpszPath = inDir;
    BOOL pass = CreateDirectory(lpszPath, NULL);
    if (pass == FALSE)
    {
        return pass;
    }

    // 进入新建目录
    pass = SetCurrentDirectory(lpszPath);
    m_strCurDir = lpszPath;
    return pass;
}

BOOL CCaptureVideo::ConnectTo(char *inpIP, int inDesPort)
{
    mCB.m_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (mCB.m_socket == INVALID_SOCKET)
        return FALSE;

    struct sockaddr_in serversocketaddr;
    serversocketaddr.sin_family = AF_INET;
    serversocketaddr.sin_addr.S_un.S_addr = inet_addr(inpIP);
    serversocketaddr.sin_port = htons(inDesPort);

    // 连接服务器
    if (connect(mCB.m_socket, (struct sockaddr *)&serversocketaddr, sizeof(serversocketaddr)) == SOCKET_ERROR)
        return FALSE;

    return TRUE;
}

void CCaptureVideo::SetCmd(CVoyCmd *inpCmd)
{
    if (mCB.m_pCmd != NULL && NULL == inpCmd)
    {
        mCB.m_pCmd->m_pCap = NULL;
        mCB.m_pCmd = NULL;
        return;
    }
    mCB.m_pCmd = inpCmd;
    inpCmd->m_pCap = this;
}

void CCaptureVideo::SetPosID(UINT inPosID)
{
    mCB.m_nPosID = inPosID;
}

BOOL CCaptureVideo::DisplayPropPage(HWND inOwner)
{
    if (m_pBF == NULL)
    {
        return FALSE;
    }

    ISpecifyPropertyPages *pSpec;
    CAUUID cauuid;
    HRESULT hr;

    // Get the filter's name and IUnknown pointer.
    FILTER_INFO FilterInfo;
    hr = m_pBF->QueryFilterInfo(&FilterInfo);
    IUnknown *pFilterUnk;
    m_pBF->QueryInterface(IID_IUnknown, (void **)&pFilterUnk);

    hr = m_pBF->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
    if (hr == S_OK)
    {
        hr = pSpec->GetPages(&cauuid);
        if (hr == S_OK && cauuid.cElems > 0)
        {
            hr = OleCreatePropertyFrame(inOwner, // Parent window
                                        300, 30,
                                        FilterInfo.achName, // Caption for the dialog box
                                        1,                  // Number of objects (just the filter)
                                        &pFilterUnk,        // Array of object pointers.
                                        cauuid.cElems, (GUID *)cauuid.pElems, 0, 0, NULL);
            CoTaskMemFree(cauuid.pElems);
        }
        pSpec->Release();
    }
    return TRUE;
}

BOOL CCaptureVideo::SetFormat(long lWidth, long lHeight, unsigned short iColorBit, __int64 iRate)
{
    if (m_pCapture == NULL)
    {
        return FALSE;
    }
    m_pMC->Stop();

    VIDEOINFOHEADER *phead;
    IAMStreamConfig *iconfig;

    HRESULT hr;
    hr = m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, m_pBF, IID_IAMStreamConfig,
                                   (void **)&iconfig);
    if (hr != NOERROR)
    {
        hr = m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pBF, IID_IAMStreamConfig,
                                       (void **)&iconfig);
    }

    if (FAILED(hr))
        return FALSE;

    AM_MEDIA_TYPE *pmt;
    if (iconfig->GetFormat(&pmt) != S_OK)
        return FALSE;

    if (pmt->formattype == FORMAT_VideoInfo)
    {
        phead = (VIDEOINFOHEADER *)pmt->pbFormat;
        phead->bmiHeader.biBitCount = iColorBit;
        phead->bmiHeader.biWidth = lWidth;
        phead->bmiHeader.biHeight = lHeight;
        phead->bmiHeader.biSizeImage = lWidth * lHeight * iColorBit / 8;
        // phead->AvgTimePerFrame = iRate;
        if ((hr = iconfig->SetFormat(pmt)) != S_OK)
            return FALSE;
    }
    iconfig->Release();
    iconfig = NULL;
    FreeMediaType(*pmt);

    mCB.lWidth = lWidth;
    mCB.lHeight = lHeight;
    m_pMC->Run();
    return TRUE;
}

bool CCaptureVideo::SetExposure(bool bAuto, long lExposure)
{
    long lValue, Min, Max, Step, Default, lFlags;

    HRESULT hr;

    bool bOutOfRange = FALSE;

    IAMCameraControl *m_pCamControl = NULL;
    hr = m_pBF->QueryInterface(IID_IAMCameraControl, (void **)&m_pCamControl);
    if (FAILED(hr))
        return false;

    hr = m_pCamControl->Get(CameraControl_Exposure, &lValue, &lFlags);
    if (FAILED(hr))
        return false;

    if (bAuto)
    {
        if (lFlags != CameraControl_Flags_Auto)
            return SUCCEEDED(m_pCamControl->Set(CameraControl_Exposure, 0, CameraControl_Flags_Auto));
        else
            return TRUE;
    }

    // else Auto exposure OFF
    if (FAILED(m_pCamControl->GetRange(CameraControl_Exposure, &Min, &Max, &Step, &Default, &lFlags)))
        return FALSE;

    // lExposure out of range can be used to freeze autoexposure value and switch to manual
    if (lExposure < Min)
    {
        bOutOfRange = TRUE;
        lExposure = Min;
    }
    if (lExposure > Max)
    {
        bOutOfRange = TRUE;
        lExposure = Max;
    }

    hr = m_pCamControl->Set(CameraControl_Exposure, lExposure, CameraControl_Flags_Manual);
    if (FAILED(hr))
        return FALSE;

    /*/////////////////////////////////////////////////////////////////////////
    if ( FAILED( m_pCamControl->Get(CameraControl_Exposure,&lValue,&lFlags) ) )
        return FALSE;

    while (lValue != lExposure)
    {
        hr = m_pCamControl->Set(CameraControl_Exposure,lExposure,CameraControl_Flags_Manual);
        if ( FAILED(hr) )
            return FALSE;

        if ( FAILED( m_pCamControl->Get(CameraControl_Exposure,&lValue,&lFlags) ) )
        return FALSE;
    }
    /*
    if ( !bOutOfRange )
        return TRUE;

    //Detect Logitech bug and correct lExposure obtained from lValue returned earlier.
    // Workaround: since when trying to Set N, Get will return (-13-N), use that formula to re-compute N
    if ( FAILED( m_pCamControl->Get(CameraControl_Exposure,&lValue,&lFlags) ) )
        return FALSE;

    if ( lValue != lExposure ) // bug is still present
    { // (-13-lExposure)=lValue, need to find X such that (-13-X)=lExposure -> X=lValue
        if ( FAILED( m_pCamControl->Set(CameraControl_Exposure,lValue,CameraControl_Flags_Manual) ) )
            return FALSE;
    }*/
    //////////////////////////////////////////////////////////////////////////
    return TRUE;
}

bool CCaptureVideo::GetExposure(long *inpExposure)
{
    IAMCameraControl *m_pCamControl = NULL;
    HRESULT hr;
    long lFlags = CameraControl_Flags_Manual;
    hr = m_pBF->QueryInterface(IID_IAMCameraControl, (void **)&m_pCamControl);
    if (FAILED(hr))
        return false;

    hr = m_pCamControl->Get(CameraControl_Exposure, inpExposure, &lFlags);
    if (FAILED(hr))
        return false;
    return true;
}

BOOL CCaptureVideo::Restart()
{
    m_pVW->put_Visible(OATRUE);
    HRESULT hr;
    // 开始运行
    hr = m_pMC->Run();

    HWND h;
    h = ::FindWindow(NULL, "XviD Status");
    ::PostMessage(h, WM_CLOSE, NULL, NULL);

    if (FAILED(hr))
    {
        AfxMessageBox("启动graph失败!");
        return FALSE;
    }
    return TRUE;
}

void CCaptureVideo::SetCameraProperty(CameraProperty *inCP)
{
    IAMVideoProcAmp *pProcAmp = NULL;
    HRESULT hr;
    hr = m_pBF->QueryInterface(IID_IAMVideoProcAmp, (void **)&pProcAmp);
    if (FAILED(hr))
        return;

    pProcAmp->Set(VideoProcAmp_Brightness, inCP->nBrightness, VideoProcAmp_Flags_Manual);

    pProcAmp->Set(VideoProcAmp_Contrast, inCP->nContrast, VideoProcAmp_Flags_Manual);

    // pProcAmp->Set(VideoProcAmp_Hue,inCP->nHue,VideoProcAmp_Flags_Manual);

    pProcAmp->Set(VideoProcAmp_Saturation, inCP->nSaturation, VideoProcAmp_Flags_Manual);

    pProcAmp->Set(VideoProcAmp_Sharpness, inCP->nSharpness, VideoProcAmp_Flags_Manual);

    // pProcAmp->Set(VideoProcAmp_Gamma,inCP->nGamma,VideoProcAmp_Flags_Manual);

    // pProcAmp->Set(VideoProcAmp_ColorEnable,inCP->nColorEnable,VideoProcAmp_Flags_Manual);

    pProcAmp->Set(VideoProcAmp_WhiteBalance, inCP->nWhiteBalance, VideoProcAmp_Flags_Manual);

    pProcAmp->Set(VideoProcAmp_BacklightCompensation, inCP->nBacklightCompensation, VideoProcAmp_Flags_Manual);

    pProcAmp->Set(VideoProcAmp_Gain, inCP->nGain, VideoProcAmp_Flags_Manual);

    //	Sleep(1000);

    // exp
    SetExposure(false, inCP->nExposure);
}

void CCaptureVideo::GetCameraProperty(CameraProperty *inCP)
{
    IAMVideoProcAmp *pProcAmp = NULL;
    HRESULT hr;
    hr = m_pBF->QueryInterface(IID_IAMVideoProcAmp, (void **)&pProcAmp);
    if (FAILED(hr))
        return;

    long lflag = VideoProcAmp_Flags_Manual;

    hr = pProcAmp->Get(VideoProcAmp_Brightness, &(inCP->nBrightness), &lflag);

    hr = pProcAmp->Get(VideoProcAmp_Contrast, &(inCP->nContrast), &lflag);

    // hr = pProcAmp->Get(VideoProcAmp_Hue,&(inCP->nHue),&lflag);

    hr = pProcAmp->Get(VideoProcAmp_Saturation, &(inCP->nSaturation), &lflag);

    hr = pProcAmp->Get(VideoProcAmp_Sharpness, &(inCP->nSharpness), &lflag);

    // hr = pProcAmp->Get(VideoProcAmp_Gamma,&(inCP->nGamma),&lflag);

    // hr = pProcAmp->Get(VideoProcAmp_ColorEnable,&(inCP->nColorEnable),&lflag);

    hr = pProcAmp->Get(VideoProcAmp_WhiteBalance, &(inCP->nWhiteBalance), &lflag);

    hr = pProcAmp->Get(VideoProcAmp_BacklightCompensation, &(inCP->nBacklightCompensation), &lflag);

    hr = pProcAmp->Get(VideoProcAmp_Gain, &(inCP->nGain), &lflag);

    // exp
    GetExposure(&(inCP->nExposure));
}

void CCaptureVideo::DefaultSet()
{
    if (m_pBF == NULL)
    {
        return;
    }
    // 1、全自动
    HRESULT hr;

    IAMCameraControl *m_pCamControl = NULL;
    hr = m_pBF->QueryInterface(IID_IAMCameraControl, (void **)&m_pCamControl);
    if (FAILED(hr))
        return;

    m_pCamControl->Set(CameraControl_Exposure, m_DefProc.nExposure, CameraControl_Flags_Auto);

    IAMVideoProcAmp *pProcAmp = NULL;
    hr = m_pBF->QueryInterface(IID_IAMVideoProcAmp, (void **)&pProcAmp);
    if (FAILED(hr))
        return;

    hr = pProcAmp->Set(VideoProcAmp_WhiteBalance, m_DefProc.nWhiteBalance, VideoProcAmp_Flags_Auto);
    hr = pProcAmp->Set(VideoProcAmp_Brightness, m_DefProc.nBrightness, VideoProcAmp_Flags_Auto);
    hr = pProcAmp->Set(VideoProcAmp_Contrast, m_DefProc.nContrast, VideoProcAmp_Flags_Auto);
    hr = pProcAmp->Set(VideoProcAmp_Saturation, m_DefProc.nSaturation, VideoProcAmp_Flags_Auto);
    hr = pProcAmp->Set(VideoProcAmp_Sharpness, m_DefProc.nSharpness, VideoProcAmp_Flags_Auto);
    hr = pProcAmp->Set(VideoProcAmp_BacklightCompensation, m_DefProc.nBacklightCompensation, VideoProcAmp_Flags_Auto);
    hr = pProcAmp->Set(VideoProcAmp_Gain, m_DefProc.nGain, VideoProcAmp_Flags_Auto);

    // 	Sleep(200);
    // 	//2、去除自动曝光
    // 	m_pCamControl->Set(CameraControl_Exposure,m_DefProc.nExposure,CameraControl_Flags_Manual);
    // 	m_pCamControl->Set(CameraControl_Exposure,m_DefProc.nExposure,CameraControl_Flags_Manual);
    // 	m_pCamControl->Set(CameraControl_Exposure,m_DefProc.nExposure,CameraControl_Flags_Manual);
    //
    // 	Sleep(200);
    // 	//3、去除自动白平衡
    // 	hr = pProcAmp->Set(VideoProcAmp_WhiteBalance,m_DefProc.nWhiteBalance,VideoProcAmp_Flags_Manual);
    // 	//4、白平衡赋值改变
    // 	hr = pProcAmp->Set(VideoProcAmp_WhiteBalance,m_DefProc.nWhiteBalance-10,VideoProcAmp_Flags_Manual);
    // 	//5、恢复
    // 	hr = pProcAmp->Set(VideoProcAmp_WhiteBalance,m_DefProc.nWhiteBalance,VideoProcAmp_Flags_Manual);
}

void CCaptureVideo::Bar_Init(VideoProcAmpProperty inproc, CSliderCtrl *pslider)
{
    HRESULT hr;

    // Query the capture filter for the IAMVideoProcAmp interface.
    IAMVideoProcAmp *pProcAmp = 0;
    hr = m_pBF->QueryInterface(IID_IAMVideoProcAmp, (void **)&pProcAmp);
    if (FAILED(hr))
    {
        // The device does not support IAMVideoProcAmp, so disable the control.
        pslider->EnableWindow(FALSE);
        return;
    }
    else
    {
        long Min, Max, Step, Default, Flags, Val;

        // Get the range and default value.
        hr = pProcAmp->GetRange(inproc, &Min, &Max, &Step, &Default, &Flags);
        if (SUCCEEDED(hr))
        {
            pslider->SetRange(Min, Max);
            // pslider->SetStep(Step);
        }
        else
        {
            pslider->EnableWindow(FALSE);
            return;
        }

        // Get the current value.
        hr = pProcAmp->Get(inproc, &Val, &Flags);
        if (SUCCEEDED(hr))
        {
            pslider->SetPos(Val);
        }
        else
        {
            // This property is not supported, so disable the control.
            pslider->EnableWindow(FALSE);
        }
    }
}

void CCaptureVideo::Bar_Init(CameraControlProperty inproc, CSliderCtrl *pslider)
{
    IAMCameraControl *m_pCamControl = NULL;
    HRESULT hr;
    long lFlags = CameraControl_Flags_Manual;
    hr = m_pBF->QueryInterface(IID_IAMCameraControl, (void **)&m_pCamControl);
    if (FAILED(hr))
        return;

    long Min, Max, Step, Default, Flags, Val;

    hr = m_pCamControl->GetRange(inproc, &Min, &Max, &Step, &Default, &Flags);
    if (SUCCEEDED(hr))
    {
        pslider->SetRange(Min, Max);
        // pslider->SetStep(Step);
    }
    else
    {
        pslider->EnableWindow(FALSE);
        return;
    }

    hr = m_pCamControl->Get(CameraControl_Exposure, &Val, &lFlags);
    if (FAILED(hr))
    {
        pslider->EnableWindow(FALSE);
        return;
    }
    else
    {
        pslider->SetPos(Val);
    }
}

HRESULT CCaptureVideo::EmuOverlook(CString infilename, HWND inDishwnd)
{
    mCB.m_emuhwnd = inDishwnd;
    mCB.lWidth = 640;
    mCB.lHeight = 480;

    USES_CONVERSION;

    HRESULT hr;
    BOOL bFoundVideo = FALSE;

    //     if (!pFilename)
    //         return E_POINTER;

    //     _tprintf(TEXT("Grabbing samples from %s.\r\n"), pFilename);

    // Create a media detector
    //
    hr = CoCreateInstance(CLSID_MediaDet, NULL, CLSCTX_INPROC_SERVER, IID_IMediaDet, (void **)&pDet);
    if (FAILED(hr))
    {
        /*_tprintf( TEXT("Failed in CoCreateInstance!  hr=0x%x\r\n"), hr );*/
        return hr;
    }

    // Set filename
    //
    hr = pDet->put_Filename(T2W(infilename.GetBuffer(infilename.GetLength())));
    if (FAILED(hr))
    {
        /* _tprintf( TEXT("Couldn't load the file!  hr=0x%x\r\n"), hr );*/
        return hr;
    }

    // Look for a video stream
    //
    long Streams = 0;
    hr = pDet->get_OutputStreams(&Streams);
    if (FAILED(hr))
    {
        /* _tprintf( TEXT("Couldn't get the output streams!  hr=0x%x\r\n"), hr );*/
        return hr;
    }

    for (int i = 0; i < Streams; i++)
    {
        BOOL bIsVideo = FALSE;

        AM_MEDIA_TYPE Type;
        memset(&Type, 0, sizeof(Type));

        // Select a media stream
        hr = pDet->put_CurrentStream(i);
        if (FAILED(hr))
        {
            /*_tprintf( TEXT("Couldn't put stream %d  hr=0x%x\r\n"), i, hr );*/
            return hr;
        }

        // Read the media type of the selected stream
        hr = pDet->get_StreamMediaType(&Type);
        if (FAILED(hr))
        {
            //             _tprintf( TEXT("Couldn't get stream media type for stream %d  hr=0x%x\r\n"),
            //                     i, hr );
            return hr;
        }

        // Does this stream contain video?
        if (Type.majortype == MEDIATYPE_Video)
            bIsVideo = TRUE;

        FreeMediaType(Type);

        if (!bIsVideo)
            continue;

        // Found a video stream
        bFoundVideo = TRUE;
        break;
    }

    if (!bFoundVideo)
    {
        /*_tprintf( TEXT("Couldn't find a video stream\r\n") );*/
        return hr;
    }

    // This method will change the MediaDet to go into
    // "sample grabbing mode" at time 0.
    //
    hr = pDet->EnterBitmapGrabMode(0.0);
    if (FAILED(hr))
    {
        /*_tprintf( TEXT("Failed in EnterBitmapGrabMode!  hr=0x%x\r\n"), hr );*/
        return hr;
    }

    // Ask for the sample grabber filter that we know lives inside the
    // graph made by the MediaDet
    //
    hr = pDet->GetSampleGrabber(&m_pGrabber);
    if (FAILED(hr) || !m_pGrabber)
    {
        /*_tprintf( TEXT("Couldn't find the sample grabber filter!  hr=0x%x\r\n"), hr );*/
        return hr;
    }

    // Set the callback (our COM object callback)
    //
    CComQIPtr<ISampleGrabberCB, &IID_ISampleGrabberCB> pCB(&mCB);
    CComQIPtr<IBaseFilter, &IID_IBaseFilter> pFilter(m_pGrabber);

    hr = m_pGrabber->SetCallback(pCB, 0);
    if (FAILED(hr))
    {
        _tprintf(TEXT("Failed in SetCallback!  hr=0x%x\r\n"), hr);
        return hr;
    }

    hr = m_pGrabber->SetOneShot(FALSE); // Don't do one-shot mode
    if (FAILED(hr))
    {
        _tprintf(TEXT("Failed in SetOneShot!  hr=0x%x\r\n"), hr);
        return hr;
    }

    hr = m_pGrabber->SetBufferSamples(FALSE); // Don't buffer samples
    if (FAILED(hr))
    {
        _tprintf(TEXT("Failed in SetBufferSamples!  hr=0x%x\r\n"), hr);
        return hr;
    }

    // Find the filter graph interface from the sample grabber filter
    //
    FILTER_INFO fi;
    memset(&fi, 0, sizeof(fi));

    hr = pFilter->QueryFilterInfo(&fi);
    if (FAILED(hr))
    {
        _tprintf(TEXT("Failed in QueryFilterInfo!  hr=0x%x\r\n"), hr);
        return hr;
    }

    // Release the filter's graph reference
    if (fi.pGraph)
        fi.pGraph->Release();
    /*IFilterGraph * pGraph = fi.pGraph;*/
    m_pGB = (IGraphBuilder *)fi.pGraph;

    // The graph will have been paused by entering bitmap grab mode.
    // We'll need to seek back to 0 to get it to deliver correctly.
    //
    CComQIPtr<IMediaSeeking, &IID_IMediaSeeking> pSeeking(m_pGB);
    REFERENCE_TIME Start = 0;
    REFERENCE_TIME Duration = 0;

    hr = pSeeking->GetDuration(&Duration);
    if (FAILED(hr))
    {
        _tprintf(TEXT("Failed in GetDuration!  hr=0x%x\r\n"), hr);
        return hr;
    }

    hr = pSeeking->SetPositions(&Start, AM_SEEKING_AbsolutePositioning, &Duration, AM_SEEKING_AbsolutePositioning);
    if (FAILED(hr))
    {
        _tprintf(TEXT("Failed in SetPositions!  hr=0x%x\r\n"), hr);
        return hr;
    }

    // Run the graph
    //     //
    //     CComQIPtr< IMediaEvent, &IID_IMediaEvent > pEvent( pGraph );
    //     CComQIPtr< IMediaControl, &IID_IMediaControl > pControl( pGraph );
    hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);

    /*hr = pControl->Run( );*/
    hr = m_pMC->Run();
    if (FAILED(hr))
    {
        _tprintf(TEXT("Failed to run the graph!  hr=0x%x\r\n"), hr);
        return hr;
    }

    return 1L;
    // Wait for the file to complete
    //
    //     long EventCode = 0;
    //
    //     hr = pEvent->WaitForCompletion( INFINITE, &EventCode );
    //     if( FAILED( hr ) )
    //     {
    //         _tprintf( TEXT("Failed in WaitForCompletion!  hr=0x%x\r\n"), hr );
    //         return hr;
    //     }
    //
    //     _tprintf( TEXT("Sample grabbing complete.\r\n") );
    //     return S_OK;
}

void CCaptureVideo::Pause()
{
    if (m_pMC != NULL)
    {
        m_pMC->Pause();
    }
}
