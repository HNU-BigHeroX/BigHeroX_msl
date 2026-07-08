// VoyWith2Cap.cpp: implementation of the CVoyWith2Cap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VoyWith2Cap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVoyWith2Cap::CVoyWith2Cap()
{
    // 	m_FrontCap = m_OverCap = NULL;

    m_com.SetCmd(&m_cmd);

    m_cmd.m_fLSpdCoe = m_cmd.m_fRSpdCoe = 1.5f;

    m_emudis = NULL;

    // m_pemucap = NULL;
}

CVoyWith2Cap::~CVoyWith2Cap()
{
    CloseOverCap();
    CloseFrontCap();
    CloseEmuOverlook();
}

BOOL CVoyWith2Cap::CreatFrontCap(int iDeviceID, CStatic *inpPrev)
{
    // 	if (m_FrontCap != NULL)
    // 	{
    // 		delete m_FrontCap;
    // 		m_FrontCap = NULL;
    // 	}
    //
    // 	m_FrontCap = new CCaptureVideo();
    // 	m_FrontCap->SetCmd(&m_cmd);
    // 	HRESULT hr = m_FrontCap->Create(iDeviceID,inpPrev,0);
    //
    // 	if (SUCCEEDED(hr))
    // 	{
    // 		return TRUE;
    // 	}
    // 	else
    // 	{
    // 		return FALSE;
    // 	}
    return FALSE;
}

BOOL CVoyWith2Cap::CreatOverCap(int iDeviceID, CStatic *inpPrev)
{
    //////////////////////////////////////////////////////////////////////////
    /*if (m_OverCap != NULL)
    {
        delete m_OverCap;
        m_OverCap = NULL;
    }

    m_OverCap = new CCaptureVideo();
    m_OverCap->SetPosID(OVERLOOK);
    m_OverCap->SetCmd(&m_cmd);
    HRESULT hr = m_OverCap->Create(iDeviceID,inpPrev,0);*/
    /////////////////////////////////////////////////////////////////////////
    // HRESULT hr = m_cOverCap.Create(iDeviceID,inpPrev,0);
    BOOL res = m_cOverCap.Open(FALSE);
    if (res == TRUE)
    {
        m_cOverCap.SetCmd(&m_cmd);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void CVoyWith2Cap::CloseFrontCap()
{
    // 	if (m_FrontCap != NULL)
    // 	{
    // 		//m_FrontCap->Stop();
    // 		delete m_FrontCap;
    // 		m_FrontCap = NULL;
    // 	}
}

void CVoyWith2Cap::CloseOverCap()
{
    /*if (m_OverCap != NULL)
    {
        m_OverCap->Stop();
        delete m_OverCap;
        m_OverCap = NULL;
    }*/

    m_cOverCap.Close();
}

void CVoyWith2Cap::SetBehavior(IBehavior *inbeh)
{
    m_cmd.SetBehavior(inbeh);
}

void CVoyWith2Cap::SendSpeed(int inLeft, int inRight)
{
    m_cmd.SetBothMotorsSpeed(inLeft, inRight);
}

void CVoyWith2Cap::StopAll()
{
    m_cmd.SetBehavior(&m_idle);
    m_cmd.Brake(1);
    Sleep(100);
    m_cmd.Brake(1);
    CloseFrontCap();
    CloseOverCap();
}

BOOL CVoyWith2Cap::OpenPort(int inPort)
{
    BOOL ret = m_com.Create(inPort);
    return ret;
}

void CVoyWith2Cap::Brake()
{
    // 	//m_cmd.Brake(1);
    // 	m_cmd.VectorMove(0,0,0);
}

void CVoyWith2Cap::Kick()
{
    m_cmd.Kick();
}

void CVoyWith2Cap::ClosePort()
{
    m_com.Close();
}

void CVoyWith2Cap::VectorMove(double inAngle, int inLV, int inPSpeed)
{
    /*m_cmd.VectorMove(inAngle,inLV,inPSpeed);*/
}

void CVoyWith2Cap::EmuOverlook(CString inbmpfile, CStatic *inPrev)
{
    if (inPrev != NULL)
    {
        bool res = m_bmploader.LoadFile(inbmpfile);
        if (res == false)
        {
            return;
        }

        if (m_cmd.m_pBeh != NULL)
        {
            BYTE *imgBuf = m_bmploader.m_pImgBuf;
            int nwidth = m_bmploader.pheader->biWidth;
            int nheight = m_bmploader.pheader->biHeight;
            m_cmd.m_pBeh->AfterUpdateOverlook(imgBuf, nwidth, nheight, 0, 0);

            // ÏÔÊ¾
            m_bmploader.DisplayPic(inPrev);
        }
        m_emudis = inPrev;
    }
}

void CVoyWith2Cap::EmuFront(CString inbmpfile, CStatic *inPrev)
{
    if (inPrev != NULL)
    {
        bool res = m_bmploader.LoadFile(inbmpfile);
        if (res == false)
        {
            return;
        }

        if (m_cmd.m_pBeh != NULL)
        {
            BYTE *imgBuf = m_bmploader.m_pImgBuf;
            int nwidth = m_bmploader.pheader->biWidth;
            int nheight = m_bmploader.pheader->biHeight;
            m_cmd.m_pBeh->AfterUpdateVideoSample(imgBuf, nwidth, nheight, 0, 0);

            // ÏÔÊ¾
            m_bmploader.DisplayPic(inPrev);
        }
        m_emudis = inPrev;
    }
}

void CVoyWith2Cap::EmuOverlookVideo(CString invideofile, CStatic *inPrev)
{
    CloseEmuOverlook();

    // 	m_pemucap = new CCaptureVideo();
    // 	m_pemucap->SetCmd(&m_cmd);
    // 	m_pemucap->EmuOverlook(invideofile,inPrev->GetSafeHwnd());

    //  	m_emucap.SetCmd(&m_cmd);
    //  	m_emucap.EmuOverlook(invideofile,inPrev->GetSafeHwnd());
}

void CVoyWith2Cap::ReloadOverlook()
{
    if (m_emudis != NULL)
    {
        if (m_cmd.m_pBeh != NULL)
        {
            m_bmploader.ReloadBuf();
            BYTE *imgBuf = m_bmploader.m_pImgBuf;
            int nwidth = m_bmploader.pheader->biWidth;
            int nheight = m_bmploader.pheader->biHeight;
            //
            // 			m_cOverCap.MasaicGBRG(imgBuf,nwidth,nheight);
            // 			m_cOverCap.MalvarDemosaic(imgBuf,nwidth,nheight);
            m_cmd.m_pBeh->AfterUpdateOverlook(imgBuf, nwidth, nheight, 0, 0);

            // ÏÔÊ¾
            // m_bmploader.DisplayPic(m_emudis);
        }
    }
}

void CVoyWith2Cap::CloseEmuOverlook()
{
    // 	if (m_pemucap != NULL)
    // 	{
    // 		m_pemucap->Stop();
    // 		delete m_pemucap;
    // 		m_pemucap = NULL;
    // 	}
}
