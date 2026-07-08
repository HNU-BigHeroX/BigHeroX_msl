/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.1       Date: 2008/11/08
  Email:  zwj_uptech@126.com
  Description:    // BMP图像加载
  Others:         // 1.1融入Voyager体系
                  // 1.2将图像buf备份出来，避免被行为函数污染
*************************************************/
// BmpLoader.h: interface for the CBmpLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BMPLOADER_H__62111B64_3D9E_427A_B74A_7BD4A2CCC63E__INCLUDED_)
#define AFX_BMPLOADER_H__62111B64_3D9E_427A_B74A_7BD4A2CCC63E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBmpLoader
{
  public:
    bool ReloadBuf();
    BOOL DisplayPic(CStatic *inDisp);
    bool LoadFile(CString infilename);
    CBmpLoader();
    virtual ~CBmpLoader();

    UCHAR *m_pbuf;
    UCHAR *m_pdata;
    BITMAPINFOHEADER *pheader;
    int bufsize;

    UCHAR *m_pImgBuf;
    int m_imgbufsize;
};

#endif // !defined(AFX_BMPLOADER_H__62111B64_3D9E_427A_B74A_7BD4A2CCC63E__INCLUDED_)
