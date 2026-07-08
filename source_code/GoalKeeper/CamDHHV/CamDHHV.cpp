// CamDHHV.cpp: implementation of the CCamDHHV class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CamDHHV.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

UINT thBehThread(LPVOID pParam)
{
    CCamDHHV *pCam = (CCamDHHV *)pParam;

    while (pCam->bToDestroy == false)
    {
        pCam->BehThread();
    }

    return 0L;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCamDHHV::CCamDHHV()
{
    m_nDevIndex = 1;
    m_pPreview = NULL;
    m_bOpen = FALSE;
    m_bStart = FALSE;

    m_pBmpInfo = NULL;
    m_pRawBuffer = NULL;
    m_pImageBuffer = NULL;
    m_pView = NULL;

    for (int i = 0; i < 256; i++)
    {
        m_pLutGamma[i] = i;

        m_pLutR[i] = i;
        m_pLutG[i] = i;
        m_pLutB[i] = i;
    }
    m_bIsGammaCorrect = FALSE;

    //////////////////////////////////////////////////////////////////////////
    m_Resolution = RES_MODE0; // 分辨率模式
    m_SnapMode = CONTINUATION;
    m_ImageMode = HV_COLOR;
    m_Layout = BAYER_GR;
    m_ConversionType = BAYER2RGB_NEIGHBOUR;
    m_nExposureTint_Upper = 10;   // 曝光分子
    m_nExposureTint_Lower = 1000; // 曝光分母

    m_lADCLevel = ADC_LEVEL2; // ADC等级

    m_dRatioR = 1;
    m_dRatioG = 1;
    m_dRatioB = 1;

    // 图像截取范围
    m_nImgLeft = 320 + 18;
    m_nImgTop = 272 + 42;
    m_nImgWidth = 640;
    m_nImgHeight = 480;

    m_DisWidth = 0;
    m_DisHeight = 0;
    m_bDrawOrg = true;
    m_bDrawToScreen = true; // 显示到屏幕
    m_bDisplay = true;

    for (i = 0; i < 4; i++)
    {
        m_lGain[i] = 127;
    }

    m_dGammRatio = 1.0;

    // 映射文件句柄
    m_hFileMapping = NULL;
    m_pLutMapping = NULL;
    ;

    m_ConversionType = BAYER2RGB_NEIGHBOUR;

    m_bIsToGetBadPixel = FALSE;
    m_bIsToRemoveBadPixel = FALSE;
    m_bIsToGetTemplate = FALSE;
    m_bIsTemplateCorrect = FALSE;
    m_nTempTimes = 0;

    m_SnapSpeed = HIGH_SPEED;
    m_nHBlanking = 0;
    m_nVBlanking = 0;

    m_bIsSnapSpeedSprted = FALSE; // 本摄像机是否支持设置采集速度
    m_bIsBlkSizeSprted = FALSE;   // 本摄像机是否支持设置取得消隐取值范围
    m_fpsCap = m_fpsBeh = 0.0;
    m_pCmd = NULL;

    bSycnNewFrame = false;
    bSycnToBeh = true;
    bToDestroy = false;
    nViewX = -100;
    nViewY = 30;
    //////////////////////////////////////////////////////////////////////////
    m_pToDrawBuf = NULL;
}

CCamDHHV::~CCamDHHV()
{

    bToDestroy = true;
    m_csImgBuf.Unlock();

    Sleep(100);

    if (m_pImageBuffer != NULL)
    {
        delete[] m_pImageBuffer;
        m_pImageBuffer = NULL;
    }

    if (m_pToDrawBuf != NULL)
    {
        delete[] m_pToDrawBuf;
        m_pToDrawBuf = NULL;
    }

    if (m_pRawBuffer != NULL)
    {
        delete[] m_pRawBuffer;
        m_pRawBuffer = NULL;
    }
}

int CCamDHHV::SnapThreadCallback(HV_SNAP_INFO *pInfo)
{
    CCamDHHV *This = (CCamDHHV *)(pInfo->pParam);
    HVSTATUS status = This->GetLastStatus(); // 读取摄像机图像采集工作状态

    if (!HV_SUCCESS(status))
    { // 如果函数调用不成功，摄像机停止图像采集。
        return 1;
    }

    TRACE("LastStatus is %X", This->dwLastStatus);

    if (This->dwLastStatus == 0) // 当状态值等于0时，表示摄像机图像采集正常，则调用SendMessage显示图像；
    {
        This->DrawCallback();
        return 1;
    }
    else if (This->dwLastStatus != 0) // 当状态值不等于0时，表示摄像机图像采集异常，则调用PostMessage进行处理；
    {
        /*
         *	发送自定义消息 WM_SNAP_ERROR 到主窗口，
         *	注意：用PostMessage发送消息，不必等待消息处理完，就可以返回。
         */
        //::PostMessage(hwnd, WM_SNAP_ERROR, 0, This->dwLastStatus);
    }

    return 1;
}

CString CCamDHHV::DeviceInfo(int inIndex)
{
    HVSTATUS status = STATUS_OK;
    //	打开数字摄像机 1
    status = BeginHVDevice(inIndex, &m_hhv);
    //	检验函数执行状态
    if (status == STATUS_OK)
    {
        // 获取相机类型
        int size = sizeof(HVTYPE);
        HVTYPE type;
        HVGetDeviceInfo(m_hhv, DESC_DEVICE_TYPE, &type, &size);
        if (type == HV2001UCTYPE || type == HV1301UCTYPE)
        {
            m_bIsSnapSpeedSprted = TRUE; // 本摄像机是否支持设置采集速度
            m_bIsBlkSizeSprted = TRUE;   // 本摄像机是否支持设置取得消隐取值范围
        }
        switch (type)
        {
        case HV1300UCTYPE:
            m_strDeviceType = "HV1300UC";
            break;

        case HV2000UCTYPE:
            m_strDeviceType = "HV2000UC";
            break;

        case HV1301UCTYPE:
            m_strDeviceType = "HV1301UC";
            break;

        case HV2001UCTYPE:
            m_strDeviceType = "HV2001UC";
            break;

        case HV3000UCTYPE:
            m_strDeviceType = "HV3000UC";
            break;

        case HV1300UMTYPE:
            m_strDeviceType = "HV1300UM";
            break;

        case HV1302UCTYPE:
            m_strDeviceType = "HV1302UC";
            break;

        case HV2002UCTYPE:
            m_strDeviceType = "HV2002UC";
            break;

        case HV3102UCTYPE:
            m_strDeviceType = "HV3102UC";
            break;

        case HV1302UMTYPE:
            m_strDeviceType = "HV1302UM";
            break;

        case HV1300FCTYPE:
            m_strDeviceType = "HV1300FC";
            break;

        case HV2000FCTYPE:
            m_strDeviceType = "HV2000FC";
            break;

        case HV3100FCTYPE:
            m_strDeviceType = "HV3100FC";
            break;

        case HV1300FMTYPE:
            m_strDeviceType = "HV1300FM";
            break;

        case HV1303UCTYPE:
            m_strDeviceType = "HV1303UC";
            break;

        case HV2003UCTYPE:
            m_strDeviceType = "HV2003UC";
            break;

        case HV3103UCTYPE:
            m_strDeviceType = "HV3103UC";
            break;

        case HV1303UMTYPE:
            m_strDeviceType = "HV1303UM";
            break;

        case SV1300FMTYPE:
            m_strDeviceType = "";
            break;

        case SV1300FCTYPE:
            m_strDeviceType = "";
            break;

        case SV1310FCTYPE:
            m_strDeviceType = "";
            break;

        case SV1310FMTYPE:
            m_strDeviceType = "";
            break;

        case SV1311FCTYPE:
            m_strDeviceType = "";
            break;

        case SV1311FMTYPE:
            m_strDeviceType = "";
            break;

        case SV400FCTYPE:
            m_strDeviceType = "";
            break;

        case SV400FMTYPE:
            m_strDeviceType = "";
            break;

        case DH1394FXTYPE:
            m_strDeviceType = "";
            break;

        case SV1410FCTYPE:
            m_strDeviceType = "";
            break;

        case SV1410FMTYPE:
            m_strDeviceType = "";
            break;

        case SV1420FCTYPE:
            m_strDeviceType = "";
            break;

        case SV1420FMTYPE:
            m_strDeviceType = "";
            break;

        case SV2000FCTYPE:
            m_strDeviceType = "";
            break;

        case SV2000FMTYPE:
            m_strDeviceType = "";
            break;

        case SV1400FCTYPE:
            m_strDeviceType = "";
            break;

        case SV1400FMTYPE:
            m_strDeviceType = "";
            break;

        case HV1350UCTYPE:
            m_strDeviceType = "";
            break;

        case HV2050UCTYPE:
            m_strDeviceType = "";
            break;

        case HV3150UCTYPE:
            m_strDeviceType = "";
            break;

        case HV1350UMTYPE:
            m_strDeviceType = "";
            break;

        case HV1360UCTYPE:
            m_strDeviceType = "";
            break;

        case HV2060UCTYPE:
            m_strDeviceType = "";
            break;

        case HV3160UCTYPE:
            m_strDeviceType = "";
            break;

        case HV1360UMTYPE:
            m_strDeviceType = "";
            break;
        }

        status = HVCloseSnap(m_hhv);
    }
    else
    {
        m_strDeviceType = "";
    }

    return m_strDeviceType;
}

HRESULT CCamDHHV::Create(int iDeviceID, CStatic *inpPrev, int nMode)
{
    if (m_bOpen == TRUE)
    {
        return 0L;
    }

    HVSTATUS status = STATUS_OK;
    //	打开数字摄像机
    if (iDeviceID > 0)
    {
        m_nDevIndex = iDeviceID;
    }
    else
    {
        m_nDevIndex = 1;
    }
    status = BeginHVDevice(m_nDevIndex, &m_hhv);
    //	检验函数执行状态，如果失败，则返回错误状态消息框
    HV_VERIFY(status);

    //	设置数字摄像机分辨率
    status = HVSetResolution(m_hhv, m_Resolution);
    if (status != STATUS_OK)
    {
        AfxMessageBox("HVSetResolution Faild!");
        return 0L;
    }

    status = HVSetSnapSpeed(m_hhv, HIGH_SPEED);
    if (status != STATUS_OK)
    {
        AfxMessageBox("HVSetSnapSpeed Faild!");
        return 0L;
    }

    //	采集模式，包括 CONTINUATION(连续)、TRIGGER(外触发)
    status = HVSetSnapMode(m_hhv, m_SnapMode);
    if (status != STATUS_OK)
    {
        AfxMessageBox("HVSetSnapMode Faild!");
        return 0L;
    }

    //  设置各个分量的增益
    for (int i = 0; i < 4; i++)
    {
        status = HVAGCControl(m_hhv, RED_CHANNEL + i, m_lGain[i]);
        if (status != STATUS_OK)
        {
            AfxMessageBox("HVAGCControl Faild!");
            return 0L;
        }
    }

    //	设置曝光时间
    SetExposureTime(m_nImgWidth, m_nExposureTint_Upper, m_nExposureTint_Lower);

    //  设置ADC的级别
    status = HVADCControl(m_hhv, ADC_BITS, m_lADCLevel);
    if (status != STATUS_OK)
    {
        AfxMessageBox("HVADCControl Faild!");
        return 0L;
    }

    /*
     *	视频输出窗口，即视频输出范围，输出窗口取值范围必须在输入窗口范围以内，
     *  视频窗口左上角X坐标和窗口宽度应为4的倍数，左上角Y坐标和窗口高度应为2的倍数
     *	输出窗口的起始位置一般设置为(0, 0)即可。
     */
    status = HVSetOutputWindow(m_hhv, m_nImgLeft, m_nImgTop, m_nImgWidth, m_nImgHeight);

    //	m_pBmpInfo即指向m_chBmpBuf缓冲区，用户可以自己分配BTIMAPINFO缓冲区
    m_pBmpInfo = (BITMAPINFO *)m_chBmpBuf;
    //	初始化BITMAPINFO 结构，此结构在保存bmp文件、显示采集图像时使用
    m_pBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    //	图像宽度
    m_pBmpInfo->bmiHeader.biWidth = m_nImgWidth;
    //	图像宽度
    m_pBmpInfo->bmiHeader.biHeight = m_nImgHeight;

    /*
     *	以下设置一般相同，
     *	对于低于8位的位图，还应设置相应的位图调色板
     */
    m_pBmpInfo->bmiHeader.biPlanes = 1;
    m_pBmpInfo->bmiHeader.biBitCount = 24;
    m_pBmpInfo->bmiHeader.biCompression = BI_RGB;
    m_pBmpInfo->bmiHeader.biSizeImage = 0;
    m_pBmpInfo->bmiHeader.biXPelsPerMeter = 0;
    m_pBmpInfo->bmiHeader.biYPelsPerMeter = 0;
    m_pBmpInfo->bmiHeader.biClrUsed = 0;
    m_pBmpInfo->bmiHeader.biClrImportant = 0;

    /*
     *	分配原始图像缓冲区，一般用来存储采集图像原始数据
     *  一般图像缓冲区大小由输出窗口大小和视频格式确定。
     */
    if (m_pRawBuffer != NULL)
    {
        delete[] m_pRawBuffer;
        m_pRawBuffer = NULL;
    }
    m_pRawBuffer = new BYTE[m_nImgWidth * m_nImgHeight];
    ASSERT(m_pRawBuffer);

    /*
    分配Bayer转换后图像数据缓冲
    */
    //	m_csImgBuf.Lock();
    if (m_pImageBuffer != NULL)
    {
        delete[] m_pImageBuffer;
    }
    m_pImageBuffer = new BYTE[m_nImgWidth * m_nImgHeight * 3];
    m_pToDrawBuf = new BYTE[m_nImgWidth * m_nImgHeight * 3];
    memset(m_pToDrawBuf, 0, m_nImgWidth * m_nImgHeight * 3);
    //	m_csImgBuf.Unlock();
    ASSERT(m_pImageBuffer);

    // 显示控件位置
    m_pPreview = inpPrev;
    CRect rc;
    ::GetClientRect(m_pPreview->GetSafeHwnd(), &rc);
    m_DisWidth = rc.Width();
    m_DisHeight = rc.Height();

    // 开启操作
    m_SnapexOpen();
    m_SnapexStart();
    ResetFrameCount();

    return S_OK;
}

HVSTATUS CCamDHHV::GetLastStatus()
{
    HV_ARG_GET_LAST_STATUS ArgFeature;
    ArgFeature.type = HV_LAST_STATUS_TRANSFER;

    HV_RES_GET_LAST_STATUS ResFeature;

    HVAPI_CONTROL_PARAMETER p;
    p.pInBuf = &ArgFeature;
    p.dwInBufSize = sizeof(ArgFeature);
    p.pOutBuf = &ResFeature;
    p.dwOutBufSize = sizeof(ResFeature);
    p.pBytesRet = NULL;
    p.code = ORD_GET_LAST_STATUS;
    int dwSize = sizeof(p);

    HVSTATUS status = HVCommand(m_hhv, CMD_HVAPI_CONTROL, &p, &dwSize);
    if (status != STATUS_OK)
    {
        TRACE("HVCommand return status is %d", status);
    }

    dwLastStatus = 0;

    if (HV_SUCCESS(status))
    {
        dwLastStatus = ResFeature.status;
    }

    return status;
}

void CCamDHHV::m_SnapexOpen()
{
    HVSTATUS status = STATUS_OK;

    /*
     *	初始化数字摄像机采集图像到内存的控制，
     *	指定回调函数SnapThreadCallback和this指针
     */
    status = HVOpenSnap(m_hhv, SnapThreadCallback, this);
    HV_VERIFY(status);
    if (HV_SUCCESS(status))
    {
        m_bOpen = TRUE; // 标志已经打开Snap环境
    }
}

void CCamDHHV::m_SnapexStart()
{
    HVSTATUS status = STATUS_OK;

    /*
     *	启动数字摄像机采集图像到内存
     */
    BYTE *ppBuf[1];
    ppBuf[0] = m_pRawBuffer;
    status = HVStartSnap(m_hhv, ppBuf, 1);

    HV_VERIFY(status);
    if (HV_SUCCESS(status))
    {
        m_bStart = TRUE;
    }
}

void CCamDHHV::m_SnapexStop()
{
    HVSTATUS status = STATUS_OK;

    //	停止采集图像到内存，可以再次调用HVStartSnapEx启动数字摄像机采集
    status = HVStopSnap(m_hhv);
    HV_VERIFY(status);
    if (HV_SUCCESS(status))
    {
        m_bStart = FALSE;
    }
}

void CCamDHHV::m_SnapexClose()
{
    HVSTATUS status = STATUS_OK;

    /*
     *	终止数字摄像机采集图像到内存，同时释放所有采集环境，
     *	再次启动数字摄像机采集，必须重新初始化
     */
    status = HVCloseSnap(m_hhv);
    HV_VERIFY(status);

    if (HV_SUCCESS(status))
    {
        m_bOpen = FALSE;
        m_bStart = FALSE;
    }
}

void CCamDHHV::DrawCallback()
{
    m_dwCount++;
    bSycnNewFrame = true;
    if (bSycnToBeh == false)
    {
        return;
    }
    //	将原始图像数据进行Bayer转换，转换后为24位。
    // 同时将原始数据进行上下翻转
    m_csImgBuf.Lock();
    ConvertBayer2Rgb(m_pImageBuffer, m_pRawBuffer, m_nImgWidth, m_nImgHeight, m_ConversionType, m_pLutR, m_pLutG,
                     m_pLutB, true, m_Layout);
    _ASSERTE(_CrtCheckMemory());
    m_csImgBuf.Unlock();
}

// 根据卡的其他参数设置曝光时间
// 其他的参数如摄像机时钟频率，消隐值都取默认值，
// 参数：
// nWindWidth:当前图像宽度
// lTintUpper:曝光时间的分子, lTintUpper/lTintLower 组成实际的曝光时间
// lTintLower:曝光时间的分母，lTintUpper/lTintLower 组成实际的曝光时间
void CCamDHHV::SetExposureTime(int nWindWidth, long lTintUpper, long lTintLower)
{
    m_nExposureTint_Upper = lTintUpper;
    m_nExposureTint_Lower = lTintLower;

    int size = sizeof(HVTYPE);
    HVTYPE type;
    HVGetDeviceInfo(m_hhv, DESC_DEVICE_TYPE, &type, &size);

    // When outputwindow changes, change the exposure
    // 请参考曝光系数转换公式
    long lClockFreq = 24000000;
    int nOutputWid = nWindWidth;
    double dExposure = 0.0;
    double dTint = max((double)lTintUpper / (double)lTintLower, MY_ZERO);
    if (type == HV1300UCTYPE || type == HV1301UCTYPE)
    {
        long lTb = 0;
        dExposure = (dTint * lClockFreq + 180.0) / ((double)nOutputWid + 244.0 + lTb);
    }
    else
    {
        long lTb = 0;
        dExposure = (dTint * lClockFreq + 180.0) / ((double)nOutputWid + 305.0 + lTb) + 1;
    }

    if (dExposure > 16383)
        dExposure = 16383;
    HVAECControl(m_hhv, AEC_EXPOSURE_TIME, (long)dExposure);
}

void CCamDHHV::InitHVDevice()
{
    if (m_hhv == NULL)
        return;

    // get the max size of the output window
    int nBuffSize = 0;
    HVSTATUS status = HVGetDeviceInfo(m_hhv, DESC_RESOLUTION, NULL, &nBuffSize);
    if (!HV_SUCCESS(status))
        return;

    BYTE *pbContext = new BYTE[nBuffSize];
    DWORD *pdContext = (DWORD *)(pbContext);
    status = HVGetDeviceInfo(m_hhv, DESC_RESOLUTION, pdContext, &nBuffSize);
    int m_nMaxWidth = *(pdContext + 2 * m_Resolution);
    int m_nMaxHeight = *(pdContext + 2 * m_Resolution + 1);

    int nMaxWidMd0 = *(pdContext + 2 * RES_MODE0);
    int nMaxHeiMd0 = *(pdContext + 2 * RES_MODE0 + 1);
    delete[] pbContext;

    int size = sizeof(HVTYPE);
    HVTYPE type;
    HVGetDeviceInfo(m_hhv, DESC_DEVICE_TYPE, &type, &size);
    if (type == HV2001UCTYPE || type == HV1301UCTYPE)
    {
        m_bIsSnapSpeedSprted = TRUE; // 本摄像机是否支持设置采集速度
        m_bIsBlkSizeSprted = TRUE;   // 本摄像机是否支持设置取得消隐取值范围
    }

    HVSetResolution(m_hhv, m_Resolution);
    HVSetSnapMode(m_hhv, m_SnapMode);
    HVSetOutputWindow(m_hhv, 0, 0, m_nMaxWidth, m_nMaxHeight);
    if (m_bIsSnapSpeedSprted)
        HVSetSnapSpeed(m_hhv, m_SnapSpeed);

    HVSetBlanking(m_hhv, m_nHBlanking, m_nVBlanking);

    for (int i = 0; i < 4; i++)
    {
        HVAGCControl(m_hhv, RED_CHANNEL + i, m_lGain[i]);
    }

    // 设置曝光时间
    SetExposureTime(m_nImgWidth, m_nExposureTint_Upper, m_nExposureTint_Lower);

    HVADCControl(m_hhv, ADC_BITS, m_lADCLevel);

    // 初始化查找表
    SetLutTable(m_dRatioR, m_dRatioG, m_dRatioB);

    // allocate the memeor accord to the resolution fo RES_MODE0
    //		m_csImgBuf.Lock();
    m_pImageBuffer = new BYTE[nMaxWidMd0 * nMaxHeiMd0 * 3];

    if (m_pImageBuffer)
    {
        FillMemory(m_pImageBuffer, nMaxWidMd0 * nMaxHeiMd0 * 3, 0xff);
    }
    //		m_csImgBuf.Unlock();

    m_pRawBuffer = new BYTE[nMaxWidMd0 * nMaxHeiMd0];
    if (m_pRawBuffer)
    {
        FillMemory(m_pRawBuffer, nMaxWidMd0 * nMaxHeiMd0, 0xff);
    }

    m_pPatternBuf = new int[nMaxWidMd0 * nMaxHeiMd0];
    if (m_pPatternBuf)
    {
        FillMemory(m_pPatternBuf, nMaxWidMd0 * nMaxHeiMd0 * sizeof(int), 0x00);
    }
    m_pBadPixelBuf = new BYTE[nMaxWidMd0 * nMaxHeiMd0];
    if (m_pBadPixelBuf)
    {
        FillMemory(m_pBadPixelBuf, nMaxWidMd0 * nMaxHeiMd0, 0xff);
    }
}

BOOL CCamDHHV::SetLutTable(double dRatioR, double dRatioG, double dRatioB)
{
    if (dRatioR <= 0)
        return FALSE;
    if (dRatioG <= 0)
        return FALSE;
    if (dRatioB <= 0)
        return FALSE;

    for (int i = 0; i < 256; i++)
    {
        if (m_bIsGammaCorrect)
        {
            m_pLutR[i] = min((int)(m_pLutGamma[i] * dRatioR), 255);
            m_pLutG[i] = min((int)(m_pLutGamma[i] * dRatioG), 255);
            m_pLutB[i] = min((int)(m_pLutGamma[i] * dRatioB), 255);
        }
        else
        {
            m_pLutR[i] = min((int)(i * dRatioR), 255);
            m_pLutG[i] = min((int)(i * dRatioG), 255);
            m_pLutB[i] = min((int)(i * dRatioB), 255);
        }
    }

    return TRUE;
}

bool CCamDHHV::Open()
{
    HVSTATUS status = BeginHVDevice(1, &m_hhv);
    HV_MESSAGE(status);
    if (HV_SUCCESS(status))
    {
        InitHVDevice();
    }

    HVTYPE type;
    int nSize = sizeof(HVTYPE);
    status = HVGetDeviceInfo(m_hhv, DESC_DEVICE_TYPE, &type, &nSize);

    if (!HV_SUCCESS(status))
    {
        m_strDeviceType = "HVDevice";
    }
    else
    {
        switch (type)
        {
        case HV1300UCTYPE:
            m_strDeviceType = "HV1300UC";
            break;
        case HV2000UCTYPE:
            m_strDeviceType = "HV2000UC";
            break;
        case HV1301UCTYPE:
            m_strDeviceType = "HV1301UC";
            break;
        default:
            m_strDeviceType = "HVDevice";
            break;
        }
    }

    return true;
}

void CCamDHHV::SetAllGain(int inGain)
{
    for (int i = 0; i < 4; i++)
    {
        m_lGain[i] = inGain;
        HVAGCControl(m_hhv, RED_CHANNEL + i, inGain);
    }
    ResetFrameCount();
}

void CCamDHHV::ResetFrameCount()
{
    // Reset frame count
    m_dwCount = 0;
    m_dwBehCnt = 0;
    m_dwStart = GetTickCount();
}

void CCamDHHV::SetImgSize(int inLeft, int inTop, int inWidth, int inHeight)
{
    // 在设置输出窗口前，停止采集
    HVSTATUS status = STATUS_OK;
    if (m_bStart)
    {
        HVStopSnap(m_hhv);
        HVSetOutputWindow(m_hhv, inLeft, inTop, inWidth, inHeight);
    }

    m_nImgLeft = inLeft;
    m_nImgTop = inTop;
    m_nImgWidth = inWidth;
    m_nImgHeight = inHeight;
    // 在设置输出窗口后，开始采集
    if (m_bStart)
    {
        // 只定义一个缓冲区,
        BYTE *ppBuffer[1];
        ppBuffer[0] = m_pRawBuffer;
        status = HVStartSnap(m_hhv, ppBuffer, 1);
        // When outputwindow changes, change the exposure
        SetExposureTime(m_nImgWidth, m_nExposureTint_Upper, m_nExposureTint_Lower);
    }
    HV_VERIFY(status);
    ResetFrameCount();
}

void CCamDHHV::SaveParamToINI(CString inFilename)
{
    CString strValue;

    strValue.Format("%d", m_Resolution);
    WritePrivateProfileString("Video Parameter", "Resolution", strValue, inFilename);

    strValue.Format("%d", m_SnapMode);
    WritePrivateProfileString("Video Parameter", "SnapMode", strValue, inFilename);

    strValue.Format("%d", m_ImageMode);
    WritePrivateProfileString("Video Parameter", "ImageMode", strValue, inFilename);
    strValue.Format("%d", m_Layout);
    WritePrivateProfileString("Video Parameter", "BayerLayout", strValue, inFilename);

    strValue.Format("%d", m_nExposureTint_Lower);
    WritePrivateProfileString("Video Parameter", "TExposureintLower", strValue, inFilename);

    strValue.Format("%d", m_nExposureTint_Upper);
    WritePrivateProfileString("Video Parameter", "TExposureintUpper", strValue, inFilename);

    strValue.Format("%d", m_lADCLevel);
    WritePrivateProfileString("Video Parameter", "ADCLevel", strValue, inFilename);

    strValue.Format("%d", m_lGain[0]);
    WritePrivateProfileString("Video Parameter", "GainRCh", strValue, inFilename);
    strValue.Format("%d", m_lGain[1]);
    WritePrivateProfileString("Video Parameter", "GainRChG", strValue, inFilename);
    strValue.Format("%d", m_lGain[2]);
    WritePrivateProfileString("Video Parameter", "GainBChG", strValue, inFilename);
    strValue.Format("%d", m_lGain[3]);
    WritePrivateProfileString("Video Parameter", "GainBCh", strValue, inFilename);

    strValue.Format("%d", *((int *)&m_dRatioR));
    WritePrivateProfileString("Video Parameter", "RatioRL", strValue, inFilename);
    strValue.Format("%d", *((int *)&m_dRatioR + 1));
    WritePrivateProfileString("Video Parameter", "RatioRH", strValue, inFilename);

    strValue.Format("%d", *((int *)&m_dRatioG));
    WritePrivateProfileString("Video Parameter", "RatioGL", strValue, inFilename);
    strValue.Format("%d", *((int *)&m_dRatioG + 1));
    WritePrivateProfileString("Video Parameter", "RatioGH", strValue, inFilename);

    strValue.Format("%d", *((int *)&m_dRatioB));
    WritePrivateProfileString("Video Parameter", "RatioBL", strValue, inFilename);
    strValue.Format("%d", *((int *)&m_dRatioB + 1));
    WritePrivateProfileString("Video Parameter", "RatioBH", strValue, inFilename);

    strValue.Format("%d", m_nHBlanking);
    WritePrivateProfileString("Video Parameter", "HBlanking", strValue, inFilename);
    strValue.Format("%d", m_nVBlanking);
    WritePrivateProfileString("Video Parameter", "VBlanking", strValue, inFilename);

    strValue.Format("%d", m_SnapSpeed);
    WritePrivateProfileString("Video Parameter", "SnapSpeed", strValue, inFilename);
}

void CCamDHHV::LoadParamFromINI(CString inFilename)
{
    int n = 0;

    m_Resolution = (HV_RESOLUTION)GetPrivateProfileInt("Video Parameter", "Resolution", RES_MODE1, inFilename);

    m_SnapMode = (HV_SNAP_MODE)GetPrivateProfileInt("Video Parameter", "SnapMode", CONTINUATION, inFilename);

    m_ImageMode = (HV_IMAGE_MODE)GetPrivateProfileInt("Video Parameter", "ImageMode", HV_COLOR, inFilename);

    m_Layout = (HV_BAYER_LAYOUT)GetPrivateProfileInt("Video Parameter", "BayerLayout", BAYER_GR, inFilename);

    m_nExposureTint_Upper = (long)GetPrivateProfileInt("Video Parameter", "TExposureintUpper", 60, inFilename);
    m_nExposureTint_Lower = (long)GetPrivateProfileInt("Video Parameter", "TExposureintLower", 1000, inFilename);

    m_lADCLevel = (long)GetPrivateProfileInt("Video Parameter", "ADCLevel", ADC_LEVEL1, inFilename);

    m_lGain[0] = (long)GetPrivateProfileInt("Video Parameter", "GainRCh", 8, inFilename);
    m_lGain[1] = (long)GetPrivateProfileInt("Video Parameter", "GainRChG", 8, inFilename);
    m_lGain[2] = (long)GetPrivateProfileInt("Video Parameter", "GainBChG", 8, inFilename);
    m_lGain[3] = (long)GetPrivateProfileInt("Video Parameter", "GainBCh", 8, inFilename);

    n = GetPrivateProfileInt("Video Parameter", "RatioRL", 0, inFilename);
    CopyMemory(&m_dRatioR, &n, sizeof(int));
    n = GetPrivateProfileInt("Video Parameter", "RatioRH", 0, inFilename);
    CopyMemory((char *)&m_dRatioR + sizeof(int), &n, sizeof(int));

    n = GetPrivateProfileInt("Video Parameter", "RatioGL", 0, inFilename);
    CopyMemory(&m_dRatioG, &n, sizeof(int));
    n = GetPrivateProfileInt("Video Parameter", "RatioGH", 0, inFilename);
    CopyMemory((char *)&m_dRatioG + sizeof(int), &n, sizeof(int));

    n = GetPrivateProfileInt("Video Parameter", "RatioBL", 0, inFilename);
    CopyMemory(&m_dRatioB, &n, sizeof(int));
    n = GetPrivateProfileInt("Video Parameter", "RatioBH", 0, inFilename);
    CopyMemory((char *)&m_dRatioB + sizeof(int), &n, sizeof(int));

    m_nHBlanking = (long)GetPrivateProfileInt("Video Parameter", "HBlanking", 0, inFilename);
    m_nVBlanking = (long)GetPrivateProfileInt("Video Parameter", "VBlanking", 0, inFilename);
    m_SnapSpeed = (HV_SNAP_SPEED)GetPrivateProfileInt("Video Parameter", "SnapSpeed", HIGH_SPEED, inFilename);

    End();
    Create(m_nDevIndex, m_pPreview, 0);
}

void CCamDHHV::End()
{
    if (m_bStart == TRUE)
    {
        m_SnapexStop();
    }
    if (m_bOpen == TRUE)
    {
        m_SnapexClose();
    }

    //		m_csImgBuf.Lock();
    if (m_pImageBuffer != NULL)
    {
        delete[] m_pImageBuffer;
        m_pImageBuffer = NULL;
    }
    //		m_csImgBuf.Unlock();
    if (m_pToDrawBuf != NULL)
    {
        delete[] m_pToDrawBuf;
        m_pToDrawBuf = NULL;
    }
    if (m_pRawBuffer != NULL)
    {
        delete[] m_pRawBuffer;
        m_pRawBuffer = NULL;
    }
}

void CCamDHHV::SetCmd(CVoyCmd *inpCmd)
{
    if (m_pCmd != NULL && NULL == inpCmd)
    {
        m_pCmd->m_pCap = NULL;
        m_pCmd = NULL;
        return;
    }
    m_pCmd = inpCmd;

    // 开启行为线程
    AfxBeginThread(thBehThread, (LPVOID)this, THREAD_PRIORITY_HIGHEST);
}

int CCamDHHV::GetImgWidth()
{
    return m_nImgWidth;
}

void CCamDHHV::SetLutSingle(int inIndex, double inVal)
{
    switch (inIndex)
    {
    case 1: // red
        SetLutTable(inVal, m_dRatioG, m_dRatioB);
        break;

    case 2: // green
        SetLutTable(m_dRatioR, inVal, m_dRatioB);
        break;

    case 3: // blue
        SetLutTable(m_dRatioR, m_dRatioG, inVal);
        break;

    default: // all
        SetLutTable(inVal, inVal, inVal);
        break;
    }
}

void CCamDHHV::GetWBRatio(double *dRatioR, double *dRatioG, double *dRatioB)
{
    if (m_pImageBuffer != NULL)
    {
        if (m_ImageMode == HV_COLOR)
        {
            SetLutTable(1.0, 1.0, 1.0);

            m_pBmpInfo->bmiHeader.biBitCount = 24;
            CSize Size;
            Size.cx = m_pBmpInfo->bmiHeader.biWidth;
            Size.cy = m_pBmpInfo->bmiHeader.biHeight;
            //				m_csImgBuf.Lock();
            ConvertBayer2Rgb(m_pImageBuffer, m_pRawBuffer, Size.cx, Size.cy, m_ConversionType, m_pLutR, m_pLutG,
                             m_pLutB, true, m_Layout);
            _ASSERTE(_CrtCheckMemory());

            GetWhiteBalanceRatio(m_pImageBuffer, (int)m_pBmpInfo->bmiHeader.biWidth,
                                 (int)m_pBmpInfo->bmiHeader.biHeight, &m_dRatioR, &m_dRatioG, &m_dRatioB);
            //			m_csImgBuf.Unlock();
        }
        else
        {
            return;
        }
    }
}

void CCamDHHV::SetADCLevel(int inLevel)
{
    HVSTATUS status = STATUS_OK;
    //  设置ADC的级别
    status = HVADCControl(m_hhv, ADC_BITS, inLevel);
    if (status != STATUS_OK)
    {
        AfxMessageBox("HVADCControl Faild!");
    }
    m_lADCLevel = inLevel;
}

int CCamDHHV::GetADCLevel()
{
    return m_lADCLevel;
}

int CCamDHHV::GetGain(int inIndex)
{

    if (inIndex < 4 && inIndex >= 0)
    {
        return m_lGain[inIndex];
    }
    else
    {
        int nGain = 0;
        for (int i = 0; i < 4; i++)
        {
            nGain += m_lGain[i];
        }
        nGain /= 4;
        return nGain;
    }
}

void CCamDHHV::SetGain(int inIndex, int val)
{
    if (inIndex < 4 && inIndex >= 0)
    {
        m_lGain[inIndex] = val;
        HVAGCControl(m_hhv, RED_CHANNEL + inIndex, m_lGain[inIndex]);
        ResetFrameCount();
    }
    else
    {
        SetAllGain(val);
    }
}

int CCamDHHV::GetExpUpper()
{
    return m_nExposureTint_Upper;
}

int CCamDHHV::GetExpLower()
{
    return m_nExposureTint_Lower;
}

void CCamDHHV::BehThread()
{
    if (m_pCmd != NULL)
    {
        if (m_pCmd->m_pBeh != NULL)
        {
            if (bSycnNewFrame == true)
            {
                bSycnToBeh = false; // 告诉图像线程先别刷新图像，刷新了也没用，没处理
                m_csImgBuf.Lock();
                m_pCmd->m_pBeh->AfterUpdateOverlook(m_pImageBuffer, m_nImgWidth, m_nImgHeight, m_fpsBeh,
                                                    m_pCmd->nState);
                if (m_bDisplay == true)
                {
                    memcpy(m_pToDrawBuf, m_pImageBuffer, m_nImgWidth * m_nImgHeight * 3);
                }
                m_csImgBuf.Unlock();
                bSycnNewFrame = false; // 等待新视频帧来置位
                bSycnToBeh = true;     // 好了，行为进行完毕，视频可以刷新了
                m_dwBehCnt++;
            }
        }
    }

    m_fpsCap = (float)(m_dwCount * 1000) / (::GetTickCount() - m_dwStart + 1);
    m_fpsBeh = (float)(m_dwBehCnt * 1000) / (::GetTickCount() - m_dwStart + 1);

    if (m_bDisplay == false)
    {
        return;
    }

    // 显示
    if (m_pView != NULL && m_bDrawToScreen == false)
    {
        m_pView->m_fpsSBeh = m_fpsBeh;
        m_pView->m_fpsSCap = m_fpsCap;
        //		m_csImgBuf.Lock();
        m_pView->BufImg(m_pImageBuffer, m_nImgWidth, m_nImgHeight);
        //		m_csImgBuf.Unlock();
        /////
        // sprintf((char*)txtTmp,"%.2f / %.2f",m_fpsBeh,m_fpsCap);
        if (m_fpsBeh < 20)
        {
            m_fpsCap = m_fpsBeh;
        }
        ////
    }

    if (m_pPreview != NULL && m_bDrawToScreen == false)
    {
        //	图像宽度和高度
        m_pBmpInfo->bmiHeader.biWidth = m_nImgWidth;
        m_pBmpInfo->bmiHeader.biHeight = m_nImgHeight;

        CDC *pDC = m_pPreview->GetDC();

        //			m_csImgBuf.Lock();
        // 显示图像
        if (m_bDrawOrg == true)
        {
            StretchDIBits(pDC->GetSafeHdc(), 0, 0,
                          m_nImgWidth,  // 显示窗口宽度
                          m_nImgHeight, // 显示窗口高度
                          0, 0,
                          m_nImgWidth,    // 图像宽度
                          m_nImgHeight,   // 图像高度
                          m_pImageBuffer, // 图像缓冲区
                          m_pBmpInfo,     // BMP图像描述信息
                          DIB_RGB_COLORS, SRCCOPY);
        }
        else
        {
            StretchDIBits(pDC->GetSafeHdc(), 0, 0,
                          m_DisWidth,  // 显示窗口宽度
                          m_DisHeight, // 显示窗口高度
                          0, 0,
                          m_nImgWidth,    // 图像宽度
                          m_nImgHeight,   // 图像高度
                          m_pImageBuffer, // 图像缓冲区
                          m_pBmpInfo,     // BMP图像描述信息
                          DIB_RGB_COLORS, SRCCOPY);
        }

        m_pPreview->ReleaseDC(pDC);
    }

    else
    {
        if (m_bDrawToScreen == true)
        {
            //	图像宽度和高度
            m_pBmpInfo->bmiHeader.biWidth = m_nImgWidth;
            m_pBmpInfo->bmiHeader.biHeight = m_nImgHeight;

            HWND hwnd = GetDesktopWindow();
            HDC hdc = GetWindowDC(hwnd);

            StretchDIBits(hdc, nViewX, nViewY,
                          m_nImgWidth,  // 显示窗口宽度
                          m_nImgHeight, // 显示窗口高度
                          0, 0,
                          m_nImgWidth,  // 图像宽度
                          m_nImgHeight, // 图像高度
                          // m_pImageBuffer,			//图像缓冲区
                          m_pToDrawBuf,
                          m_pBmpInfo, // BMP图像描述信息
                          DIB_RGB_COLORS, SRCCOPY);
            char *txtTmp[100];

            sprintf((char *)txtTmp, " %.2f", m_fpsCap);
            TextOut(hdc, nViewX + 5, nViewY + 5, (char *)txtTmp, strlen((char *)txtTmp));
            ReleaseDC(hwnd, hdc);
        }
    }
    //		m_csImgBuf.Unlock();
}

void CCamDHHV::SetDisView(int inLeft, int inTop)
{
    nViewX = inLeft;
    nViewY = inTop;
}
