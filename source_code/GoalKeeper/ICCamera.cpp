// ICCamera.cpp: implementation of the CICCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ICCamera.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CICCamera::CICCamera()
{
    m_nInited = IC_InitLibrary(0);
    m_nDevOpen = IC_NO_DEVICE;
    pInfoList = NULL;
    m_hwndDisplay = 0;
    nFrameCount = 0;
    m_bShowWindow = FALSE;

    pTmpBuf = new unsigned char[640 * 480 * 3];
    memset(pTmpBuf, 0, 640 * 480 * 3);
    m_nOffSetX = 50;
    m_nOffSetY = 40;

    m_nWidth = 640;
    m_nHeight = 480;
    m_nWidthBytes = m_nWidth * 3;

    m_fFPS_Set = 90;

    bBlockTest = false;

    // 显示处理后的图像
    iplImage = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
    iplImage->origin = 1; // 设置左下角为图像数据坐标原点
    pGetImage = NULL;
}

CICCamera::~CICCamera()
{
    if (m_nDevOpen == IC_SUCCESS)
    {
        Close();
    }
    if (m_nInited == IC_SUCCESS)
    {
        IC_CloseLibrary();
    }

    delete[] pTmpBuf;
    cvReleaseImage(&iplImage);
}

/************************************************************************/
/* 获取新图像数据的回调函数                                             */
/************************************************************************/
int cbFrameReadyDelay = 100; //
static void cbFrameReady(HGRABBER hGrabber, unsigned char *pData, unsigned long frameNumber, void *Data)
{
    CICCamera *pCam = (CICCamera *)Data;

    ///////////////////////////////////////////////
    // 马赛克
    // pCam->MasaicGBRG(pData,640,480);
    // pCam->MalvarDemosaic(pData,640,480);
    ///////////////////////////////////////////////

    ///////////////////////////////////////////////
    // 画面移动
    /*unsigned char* pTmpData = (unsigned char*)(pCam->iplImage->imageDataOrigin);
    memset(pTmpData,0,640*480*3);
    int nWidthLen =640*3;
    int nHeightLen = 480;
    int nOrgX =0;
    int nOrgY =0;
    int nDestX =0;
    int nDestY = 0;

    if (pCam->m_nOffSetX > 0)	//右移
    {
        nDestX = pCam->m_nOffSetX*3;
        nOrgX = 0;
        nWidthLen = (640 - pCam->m_nOffSetX)*3;
    }
    if (pCam->m_nOffSetX < 0)	//左移
    {
        nDestX = 0;
        nOrgX = -pCam->m_nOffSetX*3;
        nWidthLen = (640 + pCam->m_nOffSetX)*3;
    }

    if (pCam->m_nOffSetY > 0)	//上移
    {
        nDestY = pCam->m_nOffSetY;
        nOrgY = 0;
        nHeightLen = 480 - pCam->m_nOffSetY;
    }

    if (pCam->m_nOffSetY < 0)	//下移
    {
        nDestY = 0;
        nOrgY = -pCam->m_nOffSetY;
        nHeightLen = 480 + pCam->m_nOffSetY;
    }

    if (nWidthLen > 0 && nWidthLen <= 640*3 && nHeightLen > 0 && nHeightLen <= 480)
    {
        for (int i=0;i<nHeightLen;i++)
        {
            memcpy(
                pTmpData + ((i+nDestY)*640*3) + nDestX,
                pData + ((i+nOrgY)*640*3) + nOrgX,
                nWidthLen
                );
        }
    }*/
    ///////////////////////////////////////////////

    if (pCam->pGetImage != NULL) // && cbFrameReadyDelay<=0)
    {
        pCam->pGetImage->AfterUpdateOverlook(pData, 640, 480, 60, 0);
        // pCam->m_pCmd->m_pBeh->AfterUpdateOverlook(pData,640,480,60,0);
        // pCam->m_pCmd->m_pBeh->AfterUpdateOverlook(pCam->pTmpBuf,640,480,60,0);
    }
    // else
    //	cbFrameReadyDelay--;

    // 拷贝显示
    unsigned char *pTmpData = (unsigned char *)(pCam->iplImage->imageDataOrigin);
    memcpy(pTmpData, pData, 640 * 480 * 3);
    if (pCam->m_bShowWindow == TRUE)
    {
        cvShowImage("Camera", pCam->iplImage);
    }

    return;
}

/************************************************************************/
/* 设备断开的回调函数											         */
/************************************************************************/
static void cbDeviceLost(HGRABBER hGrabber, void *Data)
{
    CICCamera *pCam = (CICCamera *)Data;
    pCam->PrintToInfoList("设备断开！");
    return;
}

BOOL CICCamera::Open(BOOL inShowWindow)
{
    if (m_nInited != IC_SUCCESS)
    {
        PrintToInfoList("初始化失败！");
        return FALSE;
    }

    hGrabber = IC_CreateGrabber();
    if (hGrabber == NULL)
    {
        PrintToInfoList("创建图像捕捉对象失败！");
        return FALSE;
    }

    /////////////////////////////////////////////////////////////////////////////
    int NumOfDev = IC_GetDeviceCount();
    if (NumOfDev <= 0)
    {
        PrintToInfoList("没有合法设备，退出！");
        return FALSE;
    }

    char *pStrDevName = IC_GetDevice(0);
    if (pStrDevName == NULL)
    {
        PrintToInfoList("设备名称获取失败！");
        return FALSE;
    }
    else
    {
        CString strTmp;
        strTmp.Format("发现设备 %s ", pStrDevName);
        PrintToInfoList(strTmp);
    }

    // m_nDevOpen = IC_OpenVideoCaptureDevice(hGrabber,"DFK 23F618");
    m_nDevOpen = IC_OpenVideoCaptureDevice(hGrabber, pStrDevName);
    if (m_nDevOpen != IC_SUCCESS)
    {
        PrintToInfoList("相机设备打开失败！");
        Close();
        return FALSE;
    }
    ///////////////////////////////////////////////////////////////////////

    // 	m_nDevOpen = IC_OpenVideoCaptureDevice(hGrabber,"DFK 23G618");
    // 	if( m_nDevOpen != IC_SUCCESS )
    // 	{
    // 		PrintToInfoList("相机设备打开失败！");
    // 		Close();
    // 		return FALSE;
    // 	}

    if (IC_SetVideoFormat(hGrabber, "RGB24 (640x480)") != IC_SUCCESS)
    {
        PrintToInfoList("设置视频格式失败！");
        Close();
        return FALSE;
    }

    // 设置回调函数
    // int resSetCallBack =  IC_SetFrameReadyCallback(hGrabber,cbFrameReady,this);			//只设置图像回调函数
    int resSetCallBack =
        IC_SetCallbacks(hGrabber, cbFrameReady, this, cbDeviceLost, this); // 设置图像和设备断开回调函数
    if (resSetCallBack != 0)
    {
        PrintToInfoList("设置回调函数失败！");
        return FALSE;
    }
    // 设置捕捉模式
    // 0 : Snap continouos, 1 : do not automatically snap.
    // In continuous mode, the callback is called for each frame
    IC_SetContinuousMode(hGrabber, 0);

    // 	if (m_hwndDisplay !=0)
    // 	{
    // 		IC_SetHWnd(hGrabber,m_hwndDisplay);
    // 	}

    // 准备开启视频流（所有相机设置工作应该在这个函数之前）
    IC_PrepareLive(hGrabber, inShowWindow);
    nFrameCount = 0;
    // 开始视频流
    int Started = IC_StartLive(hGrabber, inShowWindow);
    if (Started != IC_SUCCESS)
    {
        PrintToInfoList("开启视频流失败！");
        return FALSE;
    }
    m_bShowWindow = inShowWindow;

    // 设置帧率
    SetFrameRate(m_fFPS_Set);
    return TRUE;
}

void CICCamera::Close()
{
    if (m_nDevOpen == IC_SUCCESS)
    {
        IC_StopLive(hGrabber);
        IC_CloseVideoCaptureDevice(hGrabber);
        IC_ReleaseGrabber(&hGrabber);
        m_nDevOpen = IC_NO_DEVICE;
    }
}

void CICCamera::CapturePicture(CString inFileName)
{
    //[1]获取图片信息
    long lWidth, lHeight;
    int iBitsPerPixel;
    COLORFORMAT fmt;
    IC_GetImageDescription(hGrabber, &lWidth, &lHeight, &iBitsPerPixel, &fmt);
    CString strTmp;
    strTmp.Format("[截图]w=%d;h=%d;", lWidth, lHeight);
    PrintToInfoList(strTmp);

    //[2]截一幅图
    IC_SnapImage(hGrabber, 1);
    //[3]存成文件
    IC_SaveImage(hGrabber, "./ttttt.bmp", FILETYPE_BMP, 100);
}

void CICCamera::PrintToInfoList(CString inStr)
{
    if (pInfoList != NULL)
    {
        pInfoList->AddString(inStr);
        pInfoList->SetCurSel(pInfoList->GetCount() - 1);
    }
}

void CICCamera::SetDisplayWnd(CWnd *inWnd)
{
    m_hwndDisplay = inWnd->GetSafeHwnd();
}

void CICCamera::ShowPropertyDialog()
{
    if (m_nDevOpen == IC_SUCCESS)
    {
        IC_ShowPropertyDialog(hGrabber);
    }
    else
    {
        PrintToInfoList("设备未打开，无法弹出属性对话框!");
    }
}

void CICCamera::DisplayText(int inX, int inY, CString inStr, int inSize)
{
    if (m_nDevOpen == IC_SUCCESS)
    {
        if (IC_IsDevValid(hGrabber) == IC_SUCCESS)
        {
            HDC hPaintDC;
            hPaintDC = (HDC)IC_BeginPaint(hGrabber);
            if (hPaintDC != NULL)
            {
                TextOut(hPaintDC, 10, 10, "Text", 4);
            }
            IC_EndPaint(hGrabber);
        }
    }
}

void CICCamera::SaveDeviceStateToFile(CString inFileName)
{
    if (m_nDevOpen == IC_SUCCESS)
    {
        IC_SaveDeviceStateToFile(hGrabber, inFileName.GetBuffer(inFileName.GetLength()));
        inFileName.ReleaseBuffer(-1);
    }
}

void CICCamera::SaveDeviceSettings(CString inFileName)
{
    if (m_nDevOpen == IC_SUCCESS)
    {
        IC_SaveDeviceSettings(hGrabber, inFileName.GetBuffer(inFileName.GetLength()));
        inFileName.ReleaseBuffer(-1);
    }
}

void CICCamera::ShowInternalPropertyPage()
{
    if (m_nDevOpen == IC_SUCCESS)
    {
        IC_ShowInternalPropertyPage(hGrabber);
    }
    else
    {
        PrintToInfoList("设备未打开，无法弹出属性对话框!");
    }
}

void CICCamera::SetFrameRate(float inFps)
{
    if (m_nDevOpen == IC_SUCCESS)
    {
        IC_StopLive(hGrabber);
    }
    int res = IC_SetFrameRate(hGrabber, inFps);
    switch (res)
    {
    case IC_NOT_AVAILABLE: // The property is not supported by the current device
        PrintToInfoList("[设置帧率]帧率数值不正确！");
        break;
    case IC_NO_HANDLE: // Invalid grabber handle
        PrintToInfoList("[设置帧率]未初始化！");
        break;
    case IC_NO_DEVICE: // No video capture device opened
        PrintToInfoList("[设置帧率]未检测到设备！");
        break;
        // 	case IC_NOT_IN_LIVEMODE:  //Frame rate can not set, while live video is shown. Stop Live video first!
        // 		PrintToInfoList("[设置帧率]请关闭视频再设置！");
        // 		break;
    }
    if (m_nDevOpen == IC_SUCCESS)
    {
        IC_StartLive(hGrabber, m_bShowWindow);
    }
}

void CICCamera::SetWindowPosition(int inX, int inY, int inWidth, int inHeight)
{
    if (m_nDevOpen == IC_SUCCESS)
    {
        IC_SetDefaultWindowPosition(hGrabber, 0);
        IC_SetWindowPosition(hGrabber, inX, inY, inWidth, inHeight);
    }
}

void CICCamera::LoadDeviceSettings(CString inFileName)
{
    if (m_nDevOpen == IC_SUCCESS)
    {
        int res = IC_LoadDeviceSettings(hGrabber, inFileName.GetBuffer(inFileName.GetLength()));
        inFileName.ReleaseBuffer(-1);
        if (res != IC_SUCCESS)
        {
            PrintToInfoList("读取设置文件失败！");
        }
    }
}

void CICCamera::LoadDeviceStateFromFile(CString inFileName)
{
    if (m_nDevOpen == IC_SUCCESS)
    {
        hGrabber = IC_LoadDeviceStateFromFile(hGrabber, inFileName.GetBuffer(inFileName.GetLength()));
        inFileName.ReleaseBuffer(-1);
    }
}

void CICCamera::SaveParamToINI(CString inFileName)
{
    if (m_nDevOpen != IC_SUCCESS)
    {
        return;
    }
    // 初始变量
    CString strValue;
    int nVal = 0;
    long lVal = 0;
    float fVal = 0.0f;
    int res;

    ////////////////////////////////////////////////////////////////////////////
    //[1]白平衡
    res = IC_GetPropertySwitch(hGrabber, "WhiteBalance", "Auto", &nVal);
    if (res == IC_SUCCESS)
    {
        strValue.Format("%d", nVal);
        WritePrivateProfileString("Camera Parameter", "WhiteBalance_Auto", strValue, inFileName);
    }
    else
    {
        PrintToInfoList("[保存参数]获取WhiteBalance_Auto失败！");
    }
    // WB_红
    res = IC_GetPropertyValue(hGrabber, "WhiteBalance", "White Balance Red", &nVal);
    if (res == IC_SUCCESS)
    {
        strValue.Format("%d", nVal);
        WritePrivateProfileString("Camera Parameter", "WhiteBalance_Red", strValue, inFileName);
    }
    else
    {
        PrintToInfoList("[保存参数]获取WhiteBalance Red失败！");
    }

    // WB_绿
    res = IC_GetPropertyValue(hGrabber, "WhiteBalance", "White Balance Green", &nVal);
    if (res == IC_SUCCESS)
    {
        strValue.Format("%d", nVal);
        WritePrivateProfileString("Camera Parameter", "WhiteBalance_Green", strValue, inFileName);
    }
    else
    {
        PrintToInfoList("[保存参数]获取WhiteBalance Green失败！");
    }

    // WB_蓝
    res = IC_GetPropertyValue(hGrabber, "WhiteBalance", "White Balance Blue", &nVal);
    if (res == IC_SUCCESS)
    {
        strValue.Format("%d", nVal);
        WritePrivateProfileString("Camera Parameter", "WhiteBalance_Blue", strValue, inFileName);
    }
    else
    {
        PrintToInfoList("[保存参数]获取WhiteBalance Blue失败！");
    }

    ////////////////////////////////////////////////////////////////////////////
    //[2]ColorEnhancement(色彩增强)
    res = IC_GetColorEnhancement(hGrabber, &nVal);
    if (res == IC_SUCCESS)
    {
        strValue.Format("%d", nVal);
        WritePrivateProfileString("Camera Parameter", "ColorEnhancement", strValue, inFileName);
    }
    else
    {
        PrintToInfoList("[保存参数]获取ColorEnhancement失败！");
    }

    ////////////////////////////////////////////////////////////////////////////
    //[3]Gain(增益)
    res = IC_GetPropertySwitch(hGrabber, "Gain", "Auto", &nVal);
    if (res == IC_SUCCESS)
    {
        strValue.Format("%d", nVal);
        WritePrivateProfileString("Camera Parameter", "Gain_Auto", strValue, inFileName);
    }
    else
    {
        PrintToInfoList("[保存参数]获取Gain_Auto失败！");
    }

    // Gain_Val(整形数值，不够直观，废弃不用)
    // 	res = IC_GetPropertyValue(hGrabber,"Gain","Value", &nVal);
    // 	if (res == IC_SUCCESS)
    // 	{
    // 		strValue.Format("%d", nVal);
    // 		WritePrivateProfileString("Camera Parameter", "Gain_Value", strValue, inFileName);
    // 	}
    // 	else
    // 	{
    // 		PrintToInfoList("[保存参数]获取Gain_Value失败！");
    // 	}

    // Gain_Val(浮点数值，比较直观)
    res = IC_GetPropertyAbsoluteValue(hGrabber, "Gain", "Value", &fVal);
    if (res == IC_SUCCESS)
    {
        strValue.Format("%f", fVal);
        WritePrivateProfileString("Camera Parameter", "Gain_Value", strValue, inFileName);
    }
    else
    {
        PrintToInfoList("[保存参数]获取Gain_Value失败！");
    }

    ////////////////////////////////////////////////////////////////////////////
    //[4]Exposure(曝光)
    //! （下面的代码无法获取Exposure Auto的信号，所以强制将Auto禁止）
    // 	res = IC_GetPropertyValue(hGrabber,"Exposure","Auto", &nVal);
    // 	if (res == IC_SUCCESS)
    // 	{
    // 		strValue.Format("%d", nVal);
    // 		WritePrivateProfileString("Camera Parameter", "Exposure_Auto", strValue, inFileName);
    // 	}
    // 	else
    // 	{
    // 		PrintToInfoList("[保存参数]获取Exposure_Auto失败！");
    // 	}
    WritePrivateProfileString("Camera Parameter", "Exposure_Auto", "0", inFileName);

    // Exposure_Val(整形，不直观，废弃不用)
    // 	res = IC_GetPropertyValue(hGrabber,"Exposure","Value", &nVal);
    // 	if (res == IC_SUCCESS)
    // 	{
    // 		strValue.Format("%d", nVal);
    // 		WritePrivateProfileString("Camera Parameter", "Exposure_Value", strValue, inFileName);
    // 	}
    // 	else
    // 	{
    // 		PrintToInfoList("[保存参数]获取Exposure_Value失败！");
    // 	}

    // Exposure_Val(浮点数，替代整形)
    res = IC_GetPropertyAbsoluteValue(hGrabber, "Exposure", "Value", &fVal);
    if (res == IC_SUCCESS)
    {
        strValue.Format("%f", fVal);
        WritePrivateProfileString("Camera Parameter", "Exposure_Value", strValue, inFileName);
    }
    else
    {
        PrintToInfoList("[保存参数]获取Exposure_Value失败！");
    }

    // Exposure_Auto_Reference
    res = IC_GetPropertyValue(hGrabber, "Exposure", "Auto Reference", &nVal);
    if (res == IC_SUCCESS)
    {
        strValue.Format("%d", nVal);
        WritePrivateProfileString("Camera Parameter", "Exposure_Auto_Reference", strValue, inFileName);
    }
    else
    {
        PrintToInfoList("[保存参数]获取Exposure_Auto Reference失败！");
    }
    // Exposure_Auto_Max_Val
    res = IC_GetPropertySwitch(hGrabber, "Exposure", "Auto", &nVal);
    if (res == IC_SUCCESS)
    {
        strValue.Format("%d", nVal);
        WritePrivateProfileString("Camera Parameter", "Exposure_Auto_Max_Val", strValue, inFileName);
    }
    else
    {
        PrintToInfoList("[保存参数]获取Exposure_Auto_Max_Val失败！");
    }
}

void CICCamera::LoadParamFromINI(CString inFileName)
{
    if (m_nDevOpen != IC_SUCCESS)
    {
        return;
    }
    // 初始变量
    CString strValue;
    int nVal = 0;
    long lVal = 0;
    float fVal = 0.0f;
    int res;

    ////////////////////////////////////////////////////////////////////////////
    //[1]白平衡
    int nWhiteBalance_Auto = GetPrivateProfileInt("Camera Parameter", "WhiteBalance_Auto", 0, inFileName);
    // res = IC_SetPropertySwitch(hGrabber,"WhiteBalance","Auto", nWhiteBalance_Auto);
    res = IC_SetWhiteBalanceAuto(hGrabber, nWhiteBalance_Auto);
    if (res != IC_SUCCESS)
    {
        PrintToInfoList(" WhiteBalance Auto 设置失败！");
    }

    if (nWhiteBalance_Auto == 0) // 不是Auto的时候才设置具体数值（否则会强制取消Auto）
    {
        // WB_红
        int nWhiteBalance_Red = GetPrivateProfileInt("Camera Parameter", "WhiteBalance_Red", 0, inFileName);
        res = IC_SetPropertyValue(hGrabber, "WhiteBalance", "White Balance Red", nWhiteBalance_Red);
        if (res != IC_SUCCESS)
        {
            PrintToInfoList(" WhiteBalance Red 设置失败！");
        }

        // WB_绿
        int nWhiteBalance_Green = GetPrivateProfileInt("Camera Parameter", "WhiteBalance_Green", 0, inFileName);
        res = IC_SetPropertyValue(hGrabber, "WhiteBalance", "White Balance Green", nWhiteBalance_Green);
        if (res != IC_SUCCESS)
        {
            PrintToInfoList(" WhiteBalance Green 设置失败！");
        }

        // WB_蓝
        int nWhiteBalance_Blue = GetPrivateProfileInt("Camera Parameter", "WhiteBalance_Blue", 0, inFileName);
        res = IC_SetPropertyValue(hGrabber, "WhiteBalance", "White Balance Blue", nWhiteBalance_Blue);
        if (res != IC_SUCCESS)
        {
            PrintToInfoList(" WhiteBalance Blue 设置失败！");
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    //[2]ColorEnhancement(色彩增强)
    int nColorEnhancement = GetPrivateProfileInt("Camera Parameter", "ColorEnhancement", 1, inFileName);
    res = IC_SetColorEnhancement(hGrabber, nColorEnhancement);
    if (res != IC_SUCCESS)
    {
        PrintToInfoList(" ColorEnhancement(色彩增强) 设置失败！");
    }

    ////////////////////////////////////////////////////////////////////////////
    //[3]Gain(增益)
    int nGain_Auto = GetPrivateProfileInt("Camera Parameter", "Gain_Auto", 0, inFileName);
    res = IC_SetPropertySwitch(hGrabber, "Gain", "Auto", nGain_Auto);
    if (res != IC_SUCCESS)
    {
        PrintToInfoList(" Gain_Auto 设置失败！");
    }

    if (nGain_Auto == 0) // 不是Auto的时候才设置具体数值（否则会强制取消Auto）
    {
        // Gain_Val(浮点数值，比较直观)
        float fGain_Val = GetPrivateProfileFloat("Camera Parameter", "Gain_Value", 21.5f, inFileName);
        res = IC_SetPropertyAbsoluteValue(hGrabber, "Gain", "Value", fGain_Val);
        if (res != IC_SUCCESS)
        {
            PrintToInfoList(" Gain_Value 设置失败！");
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    //[4]Exposure(曝光)
    int nExposure_Auto = GetPrivateProfileInt("Camera Parameter", "Exposure_Auto", 0, inFileName);
    res = IC_SetPropertySwitch(hGrabber, "Exposure", "Auto", nExposure_Auto);
    if (res != IC_SUCCESS)
    {
        PrintToInfoList(" Exposure_Auto 设置失败！");
    }

    if (nExposure_Auto == 0) // 不是Auto的时候才设置具体数值（否则会强制取消Auto）
    {
        // Exposure_Val(浮点数，替代整形)
        float fExposure_Val = GetPrivateProfileFloat("Camera Parameter", "Exposure_Value", 0.005f, inFileName);
        res = IC_SetPropertyAbsoluteValue(hGrabber, "Exposure", "Value", fExposure_Val);
        if (res != IC_SUCCESS)
        {
            PrintToInfoList(" fExposure_Val 设置失败！");
        }

        // Exposure_Auto_Max_Val
        // 		int nExposure_Auto_Max_Val = GetPrivateProfileInt("Camera Parameter", "Exposure_Auto_Max_Val", 0,
        // inFileName ); 		res = IC_SetPropertySwitch(hGrabber,"Exposure","Auto",nGain_Auto); 		if (res !=
        // IC_SUCCESS)
        // 		{
        // 			PrintToInfoList(" Exposure_Auto_Max_Val 设置失败！");
        // 		}
    }
    else
    {
        // 自动曝光时才起作用
        // Exposure_Auto_Reference
        int nExposure_Auto_Reference =
            GetPrivateProfileInt("Camera Parameter", "Exposure_Auto_Reference", 0, inFileName);
        res = IC_SetPropertyValue(hGrabber, "Exposure", "Auto Reference", nExposure_Auto_Reference);
        if (res != IC_SUCCESS)
        {
            PrintToInfoList(" nExposure_Auto_Reference 设置失败！");
        }
    }
}

float CICCamera::GetPrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, float fDefault, LPCSTR lpFileName)
{
    float ret = fDefault;
    char arVal[64];
    GetPrivateProfileString(lpAppName, lpKeyName, "", arVal, 64, lpFileName);
    ret = (float)atof(arVal);
    return ret;
}

void CICCamera::MasaicGBRG(BYTE *inBuf, int inWidth, int inHeight)
{
    BYTE *pCurPos = inBuf;
    for (int h = 0; h < inHeight; h += 2)
    {
        for (int w = 0; w < inWidth; w += 2)
        {
            pCurPos = &inBuf[(h * inWidth + w) * 3];
            // left bottom - Red;
            pCurPos[0] = 0;
            pCurPos[1] = 0;
            // right bottom - Green;
            pCurPos[3] = 0;
            pCurPos[5] = 0;
            // left top - Green;
            pCurPos += inWidth * 3;
            pCurPos[0] = 0;
            pCurPos[2] = 0;
            // right top - Blue;
            pCurPos[4] = 0;
            pCurPos[5] = 0;
        }
    }
}

void CICCamera::MalvarDemosaic(BYTE *inBuf, int inWidth, int inHeight)
{
    BYTE *pCurPixel = inBuf;
    bool bRedRow = true;
    bool bGreen = false;
    for (int h = 2; h < inHeight - 2; h++)
    {
        if (h % 2 == 0)
        {
            bRedRow = true;
            bGreen = false;
        }
        else
        {
            bRedRow = false;
            bGreen = true;
        }
        // pCurPixel = inBuf +(h*inWidth)*3 + 6;
        for (int w = 2; w < inWidth - 2; w++)
        {
            pCurPixel = inBuf + (h * inWidth + w) * 3;
            if (bRedRow == true)
            {
                // red row
                if (bGreen == true)
                {
                    GreenCenter_RedRow(pCurPixel);
                }
                else
                {
                    RedCenter(pCurPixel);
                }
            }
            else
            {
                // blue row
                if (bGreen == true)
                {
                    GreenCenter_BlueRow(pCurPixel);
                }
                else
                {
                    BlueCenter(pCurPixel);
                }
            }
            bGreen = !bGreen;

            // pCurPixel += 3;
        }
    }
}

/**红色像素**/
void CICCamera::RedCenter(BYTE *pBuf)
{
    // 绿色分量
    tmp[0] = *(pBuf - 3 + 1);                   //(-1,0,g)
    tmp[0] = tmp[0] + *(pBuf + 3 + 1)           //(1,0,g)
             + *(pBuf - m_nWidthBytes + 1)      //(0,-1,g)
             + *(pBuf + m_nWidthBytes + 1);     //(0,1,g)
    tmp[1] = *(pBuf - 6 + 2);                   //(-2,0,r)
    tmp[1] = tmp[1] + *(pBuf + 6 + 2)           //(2,0,r)
             + *(pBuf - m_nWidthBytes * 2 + 2)  //(0,-2,r)
             + *(pBuf + m_nWidthBytes * 2 + 2); //(0,2,r)
    tmp[2] = *(pBuf + 2);                       //(0,0,r)
    res = (tmp[0] * 2 - tmp[1] + tmp[2] * 4) / 8;
    if (res < 0)
    {
        res = 0;
    }
    if (res > 0xff)
    {
        res = 0xff;
    }
    pBuf[1] = res;

    // 蓝色分量
    tmp[0] = *(pBuf - m_nWidthBytes - 3);         //(-1,-1,b)
    tmp[0] = tmp[0] + *(pBuf + m_nWidthBytes - 3) //(-1,1,b)
             + *(pBuf - m_nWidthBytes + 3)        //(1,-1,b)
             + *(pBuf + m_nWidthBytes + 3);       //(1,1,b)
    tmp[1] = *(pBuf - 6 + 2);                     //(-2,0,r)
    tmp[1] = tmp[1] + *(pBuf + 6 + 2)             //(2,0,r)
             + *(pBuf - m_nWidthBytes * 2 + 2)    //(0,-2,r)
             + *(pBuf + m_nWidthBytes * 2 + 2);   //(0,2,r)
    tmp[2] = *(pBuf + 2);                         //(0,0,r)*12
    res = (tmp[0] * 4 - tmp[1] * 3 + tmp[2] * 12) / 16;
    if (res < 0)
    {
        res = 0;
    }
    if (res > 0xff)
    {
        res = 0xff;
    }
    pBuf[0] = res;
}

/**蓝色像素**/
void CICCamera::BlueCenter(BYTE *pBuf)
{
    // 红色分量
    tmp[0] = *(pBuf - m_nWidthBytes - 3 + 2);         //(-1,-1,r)
    tmp[0] = tmp[0] + *(pBuf + m_nWidthBytes - 3 + 2) //(-1,1,r)
             + *(pBuf - m_nWidthBytes + 3 + 2)        //(1,-1,r)
             + *(pBuf + m_nWidthBytes + 3 + 2);       //(1,1,r)

    tmp[1] = *(pBuf - 6);                   //(-2,0,b)
    tmp[1] = tmp[1] + *(pBuf + 6)           //(2,0,b)
             + *(pBuf - m_nWidthBytes * 2)  //(0,-2,b)
             + *(pBuf + m_nWidthBytes * 2); //(0,2,r)

    tmp[2] = (*pBuf); //(0,0,b)

    res = (tmp[0] * 4 - tmp[1] * 3 + tmp[2] * 12) / 16;
    if (res < 0)
    {
        res = 0;
    }
    if (res > 0xff)
    {
        res = 0xff;
    }
    pBuf[2] = res;

    // 绿色分量
    tmp[0] = *(pBuf - 3 + 1);               //(-1,0,g)
    tmp[0] = tmp[0] + *(pBuf + 3 + 1)       //(1,0,g)
             + *(pBuf - m_nWidthBytes + 1)  //(0,-1,g)
             + *(pBuf + m_nWidthBytes + 1); //(0,1,g)

    tmp[1] = *(pBuf - 6);                   //(-2,0,b)
    tmp[1] = tmp[1] + *(pBuf + 6)           //(2,0,b)
             + *(pBuf - m_nWidthBytes * 2)  //(0,-2,b)
             + *(pBuf + m_nWidthBytes * 2); //(0,2,b)

    tmp[2] = (*pBuf); //(0,0,b)

    res = ((tmp[0]) * 2 - (tmp[1]) + (tmp[2] * 4)) / 8;
    if (res < 0)
    {
        res = 0;
    }
    if (res > 0xff)
    {
        res = 0xff;
    }
    pBuf[1] = res;
}

/**绿色像素（在蓝色行）**/
void CICCamera::GreenCenter_BlueRow(BYTE *pBuf)
{
    // 红色分量
    tmp[0] = *(pBuf - m_nWidthBytes + 2);          //(0,-1,r)
    tmp[0] = tmp[0] + *(pBuf + m_nWidthBytes + 2); //(0,1,r)

    tmp[1] = *(pBuf - m_nWidthBytes - 3 + 1);         //(-1,-1,g)
    tmp[1] = tmp[1] + *(pBuf + m_nWidthBytes - 3 + 1) //(-1,1,g)
             + *(pBuf - m_nWidthBytes + 3 + 1)        //(1,-1,g)
             + *(pBuf + m_nWidthBytes + 3 + 1)        //(1,1,g)
             + *(pBuf - m_nWidthBytes * 2 + 1)        //(0,-2,g)
             + *(pBuf + m_nWidthBytes * 2 + 1);       //(0,2,g)

    tmp[2] = *(pBuf - 6 + 1);          //(-2,0,g)
    tmp[2] = tmp[2] + *(pBuf + 6 + 1); //(2,0,g)

    tmp[3] = *(pBuf + 1); //(0,0,g)*10

    res = (tmp[0] * 8 - tmp[1] * 2 + tmp[2] + tmp[3] * 10) / 16;
    if (res < 0)
    {
        res = 0;
    }
    if (res > 0xff)
    {
        res = 0xff;
    }
    pBuf[2] = res;

    // 蓝色分量
    tmp[0] = *(pBuf - 3);          //(-1,0,b)
    tmp[0] = tmp[0] + *(pBuf + 3); //(1,0,b)

    tmp[1] = *(pBuf - m_nWidthBytes - 3 + 1);         //(-1,-1,g)
    tmp[1] = tmp[1] + *(pBuf + m_nWidthBytes - 3 + 1) //(-1,1,g)
             + *(pBuf - m_nWidthBytes + 3 + 1)        //(1,-1,g)
             + *(pBuf + m_nWidthBytes + 3 + 1)        //(1,1,g)
             + *(pBuf - 6 + 1)                        //(-2,0,g)
             + *(pBuf + 6 + 1);                       //(2,0,g)

    tmp[2] = *(pBuf - m_nWidthBytes * 2 + 1);          //(0,-2,g)
    tmp[2] = tmp[2] + *(pBuf + m_nWidthBytes * 2 + 1); //(0,2,g)

    tmp[3] = *(pBuf + 1); //(0,0,g)

    res = (tmp[0] * 8 - tmp[1] * 2 + tmp[2] + tmp[3] * 10) / 16;
    if (res < 0)
    {
        res = 0;
    }
    if (res > 0xff)
    {
        res = 0xff;
    }
    pBuf[0] = res;
}

/**绿色像素（在红色行）**/
void CICCamera::GreenCenter_RedRow(BYTE *pBuf)
{
    // 红色分量
    tmp[0] = *(pBuf - 3 + 2);          //(-1,0,r)
    tmp[0] = tmp[0] + *(pBuf + 3 + 2); //(1,0,r)

    tmp[1] = *(pBuf - m_nWidthBytes - 3 + 1);         //(-1,-1,g)
    tmp[1] = tmp[1] + *(pBuf + m_nWidthBytes - 3 + 1) //(-1,1,g)
             + *(pBuf - m_nWidthBytes + 3 + 1)        //(1,-1,g)
             + *(pBuf + m_nWidthBytes + 3 + 1)        //(1,1,g)
             + *(pBuf - 6 + 1)                        //(-2,0,g)
             + *(pBuf + 6 + 1);                       //(2,0,g)

    tmp[2] = *(pBuf - m_nWidthBytes * 2 + 1);          //(0,-2,g)
    tmp[2] = tmp[2] + *(pBuf + m_nWidthBytes * 2 + 1); //(0,2,g)

    tmp[3] = *(pBuf + 1); //(0,0,g)

    res = (tmp[0] * 8 - tmp[1] * 2 + (tmp[2]) + (tmp[3] * 10)) / 16;
    if (res < 0)
    {
        res = 0;
    }
    if (res > 0xff)
    {
        res = 0xff;
    }
    pBuf[2] = res;

    // 蓝色分量
    tmp[0] = *(pBuf - m_nWidthBytes);          //(0,-1,b)
    tmp[0] = tmp[0] + *(pBuf + m_nWidthBytes); //(0,1,b)

    tmp[1] = *(pBuf - m_nWidthBytes - 3 + 1);         //(-1,-1,g)
    tmp[1] = tmp[1] + *(pBuf + m_nWidthBytes - 3 + 1) //(-1,1,g)
             + *(pBuf - m_nWidthBytes + 3 + 1)        //(1,-1,g)
             + *(pBuf + m_nWidthBytes + 3 + 1)        //(1,1,g)
             + *(pBuf - m_nWidthBytes * 2 + 1)        //(0,-2,g)
             + *(pBuf + m_nWidthBytes * 2 + 1);       //(0,2,g)

    tmp[2] = *(pBuf - 6 + 1);          //(-2,0,g)
    tmp[2] = tmp[2] + *(pBuf + 6 + 1); //(2,0,g)

    tmp[3] = *(pBuf + 1); //(0,0,g)

    res = (tmp[0] * 8 - tmp[1] * 2 + tmp[2] + tmp[3] * 10) / 16;
    if (res < 0)
    {
        res = 0;
    }
    if (res > 0xff)
    {
        res = 0xff;
    }
    pBuf[0] = res;
}

void CICCamera::ShowVideoLive(BOOL inShowWindow)
{
    if (inShowWindow != m_bShowWindow)
    {
        IC_StopLive(hGrabber);
        // 开始视频流
        int Started = IC_StartLive(hGrabber, inShowWindow);
        if (Started != IC_SUCCESS)
        {
            PrintToInfoList("开启视频流失败！");
        }
        m_bShowWindow = inShowWindow;

        // 设置帧率
        SetFrameRate(m_fFPS_Set);
    }
}

void CICCamera::CvDisplayVideo(bool inEn)
{
    if (inEn == true)
    {
        cvNamedWindow("Camera");
        cvMoveWindow("Camera", 0, 0);
    }
    else
    {
        cvDestroyWindow("Camera");
    }
    m_bShowWindow = inEn;
}
