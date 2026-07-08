// FieldMap.cpp : implementation file
//

#include "stdafx.h"
#include "MSLFieldLine.h"
#include "FieldMap.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFieldMap

static BOOL weitu1[320][240];

static BOOL baidianx[50000];
static BOOL baidiany[50000];

static int shendubiao[640][480]; // 存放最终的深度图
static BOOL weitu[640][480];     // 将场线图移到中心之后的临时图

static BYTE displaybuf[640 * 480 * 3];

static int f_top = 9;
static int f_bottom = 211;
static int f_left = 13;
static int f_right = 313;
static int f_midx = 163;
static int f_midy = 110;

CFieldMap::CFieldMap()
{
    //
    m_PicRect.top = 0;
    m_PicRect.left = 0;
    m_PicRect.bottom = 222;
    m_PicRect.right = 324;

    // data
    Field_Width = 1800;
    Field_Height = 1200;
    Field_PenaltyArea_Width = 225;
    Field_PenaltyArea_Height = 650;
    Field_GoalArea_Width = 75;
    Field_GoalArea_Height = 350;
    Field_DiameterofCenter = 400;
    Field_RadiusofCorner = 75;

    //
    memset(&bih, 0, sizeof(bih));
    bih.biSize = sizeof(bih);
    bih.biWidth = 640;
    bih.biHeight = 480;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
}

CFieldMap::~CFieldMap()
{
    Bitmap.DeleteObject();
    dcMem.DeleteDC();
}

BEGIN_MESSAGE_MAP(CFieldMap, CStatic)
//{{AFX_MSG_MAP(CFieldMap)
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFieldMap message handlers

void CFieldMap::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // TODO: Add your message handler code here
    // 创建缓存DC
    dcMem.DeleteDC();
    dcMem.CreateCompatibleDC(&dc);

    // 将内存dc撑开
    Bitmap.DeleteObject();
    Bitmap.CreateCompatibleBitmap(&dc, m_PicRect.right, m_PicRect.bottom);
    CBitmap *oldbmp = dcMem.SelectObject(&Bitmap);
    // dcMem.SetBkMode(TRANSPARENT);

    // 开始绘图
    // 1、图片底色刷黑
    CBrush *pBrush = new CBrush(RGB(0, 0, 0));
    dcMem.FillRect(&m_PicRect, pBrush);

    // 白色笔
    CPen wPen(PS_SOLID, 2, COLORREF(RGB(255, 255, 255)));
    dcMem.SelectObject(&wPen);

    // 2、画外边线
    dcMem.MoveTo(f_left, f_top);
    dcMem.LineTo(f_left, f_bottom);
    dcMem.LineTo(f_right, f_bottom);
    dcMem.LineTo(f_right, f_top);
    dcMem.LineTo(f_left, f_top);

    // 3、画中圈
    // 计算一个像素对应的实际长度
    if (Field_Width == 0)
    {
        m_k = 100;
    }
    else
    {
        m_k = Field_Width / 300;
    }
    // 半径在本图片上的长
    m_tmp = Field_DiameterofCenter / (2 * m_k);
    dcMem.MoveTo((int)(f_midx - m_tmp), (int)(f_midy - m_tmp));
    dcMem.SelectObject(pBrush);
    dcMem.Ellipse((int)(f_midx - m_tmp), (int)(f_midy - m_tmp), (int)(f_midx + m_tmp), (int)(f_midy + m_tmp));
    // 中线
    dcMem.MoveTo(f_midx, f_top);
    dcMem.LineTo(f_midx, f_bottom);

    // 4、画左侧禁区
    // 计算禁区高度在图片上的长
    int pwid = (int)(Field_PenaltyArea_Width / m_k);
    int phei = (int)(Field_PenaltyArea_Height / m_k);

    dcMem.MoveTo(f_left, f_midy - (phei / 2));
    dcMem.LineTo(f_left + pwid, f_midy - (phei / 2));
    dcMem.LineTo(f_left + pwid, f_midy + (phei / 2));
    dcMem.LineTo(f_left, f_midy + (phei / 2));

    // 5、画左侧球门区
    int gwid = Field_GoalArea_Width / m_k;
    int ghei = Field_GoalArea_Height / m_k;
    dcMem.MoveTo(f_left, f_midy - (ghei / 2));
    dcMem.LineTo(f_left + gwid, f_midy - (ghei / 2));
    dcMem.LineTo(f_left + gwid, f_midy + (ghei / 2));
    dcMem.LineTo(f_left, f_midy + (ghei / 2));

    // 6、画右侧禁区
    dcMem.MoveTo(f_right, f_midy - (phei / 2));
    dcMem.LineTo(f_right - pwid, f_midy - (phei / 2));
    dcMem.LineTo(f_right - pwid, f_midy + (phei / 2));
    dcMem.LineTo(f_right, f_midy + (phei / 2));

    // 7、画右侧球门区
    dcMem.MoveTo(f_right, f_midy - (ghei / 2));
    dcMem.LineTo(f_right - gwid, f_midy - (ghei / 2));
    dcMem.LineTo(f_right - gwid, f_midy + (ghei / 2));
    dcMem.LineTo(f_right, f_midy + (ghei / 2));

    // 8、画四个角球区
    int r = Field_RadiusofCorner / m_k;
    dcMem.Arc(f_left + r, f_top + r, f_left - r, f_top - r, f_left, f_top + r, f_left + r, f_top);
    dcMem.Arc(f_left + r, f_bottom + r, f_left - r, f_bottom - r, f_left + r, f_bottom, f_left, f_bottom - r);
    dcMem.Arc(f_right + r, f_top + r, f_right - r, f_top - r, f_right - r, f_top, f_right, f_top + r);
    dcMem.Arc(f_right + r, f_bottom + r, f_right - r, f_bottom - r, f_right, f_bottom - r, f_right - r, f_bottom);

    // dc拷贝
    dc.BitBlt(0, 0, m_PicRect.right, m_PicRect.bottom, &dcMem, 0, 0, SRCCOPY);
    delete pBrush;

    // Do not call CStatic::OnPaint() for painting messages
}

void CFieldMap::CreateMap()
{
    m_printf("正在生成深度图，请稍候....");
    AfxBeginThread(MapingThread, this);
}

void CFieldMap::CreateBMP()
{
    ///////////////////////////////////////////////////////////
    int tmpWidth = f_right - f_left;
    int tmpHeight = tmpWidth * Field_Height / Field_Width;
    f_top = f_midy - tmpHeight / 2;
    f_bottom = f_midy + tmpHeight / 2;
    ///////////////////////////////////////////////////////////

    OnPaint();

    m_printf("正在保存图片....");
    BOOL res = SaveBitmapToFile(&Bitmap, "field.bmp");
    if (res == TRUE)
    {
        m_printf("图片保存成功！");
    }
    else
    {
        m_printf("图片保存失败...");
    }

    Invalidate();
}

BOOL CFieldMap::SaveBitmapToFile(CBitmap *bitmap, LPSTR lpFileName)
{
    HBITMAP hBitmap;
    HDC hDC;                 // 设备描述表
    int iBits;               // 当前显示分辨率下每个像素所占字节数
    WORD wBitCount;          // 位图中每个像素所占字节数
    DWORD dwPaletteSize = 0, // 定义调色板大小， 位图中像素字节大小 ，位图文件大小 ， 写入文件字节数
        dwBmBitsSize, dwDIBSize, dwWritten;
    BITMAP Bitmap;                         // 位图属性结构
    BITMAPFILEHEADER bmfHdr;               // 位图文件头结构
    BITMAPINFOHEADER bi;                   // 位图信息头结构
    LPBITMAPINFOHEADER lpbi;               // 指向位图信息头结构
    HANDLE fh, hDib, hPal, hOldPal = NULL; // 定义文件，分配内存句柄，调色板句柄

    // 计算位图文件每个像素所占字节数
    hBitmap = (HBITMAP)*bitmap;
    hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
    iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
    DeleteDC(hDC);

    /*/////////////////////////////////////////////////////////////////////////
    if (iBits <= 1)
        wBitCount = 1;
    else if (iBits <= 4)
        wBitCount = 4;
    else if (iBits <= 8)
        wBitCount = 8;
    else if (iBits <= 24)
        wBitCount = 24;
    else if ( iBits <= 32 )
        wBitCount = 32;
    /////////////////////////////////////////////////////////////////////////*/
    wBitCount = 1;

    // 计算调色板大小
    if (wBitCount <= 8)
        dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);

    // 设置位图信息头结构
    GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = 324;  // Bitmap.bmWidth;
    bi.biHeight = 222; // Bitmap.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = wBitCount;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    dwBmBitsSize = ((bi.biWidth /*Bitmap.bmWidth*/ * wBitCount + 31) / 32) * 4 * bi.biHeight /*Bitmap.bmHeight*/;

    // 为位图内容分配内存
    hDib = ::GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
    lpbi = (LPBITMAPINFOHEADER)::GlobalLock(hDib);
    // 	int size = dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER);
    // 	lpbi = (BITMAPINFOHEADER*)new BYTE[dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER)];
    *lpbi = bi;

    // 处理调色板
    hPal = GetStockObject(DEFAULT_PALETTE);
    if (hPal)
    {
        hDC = ::GetDC(NULL);
        hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
        RealizePalette(hDC);
    }

    // 获取该调色板下新的像素值
    GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,
              (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

    // 恢复调色板
    if (hOldPal)
    {
        SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
        RealizePalette(hDC);
        ::ReleaseDC(NULL, hDC);
    }

    // 创建位图文件
    fh = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (fh == INVALID_HANDLE_VALUE)
        return FALSE;

    // 设置位图文件头
    bmfHdr.bfType = 0x4D42; // "BM"
    dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
    bmfHdr.bfSize = dwDIBSize;
    bmfHdr.bfReserved1 = 0;
    bmfHdr.bfReserved2 = 0;
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

    // 写入位图文件头
    WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

    // 写入位图文件其余内容
    WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);

    // 清除
    GlobalUnlock(hDib);
    GlobalFree(hDib);
    CloseHandle(fh);
    return TRUE;
}

void CFieldMap::m_DisplayMap()
{
    // debug
    printf("Display...");

    /*/////////////////////////////////////////////////////////////////////////
    for (int i=0;i<640*480;i++)
    {
        displaybuf[3*i] = i%0xff;
        displaybuf[3*i+1] = i%0xff;
        displaybuf[3*i+2] = i%0xff;
    }
    /////////////////////////////////////////////////////////////////////////*/
    BYTE tmpb;
    int baseAdr = 0;
    for (int i = 0; i < 480; i++)
    {
        baseAdr = i * 640 * 3;
        for (int j = 0; j < 640; j++)
        {
            tmpb = shendubiao[j][i] % 0xff;
            if (tmpb < (255 / 3))
            {
                tmpb *= 3;
            }
            else
            {
                tmpb = 0x60;
            }
            displaybuf[baseAdr + j * 3] = tmpb;
            displaybuf[baseAdr + j * 3 + 1] = tmpb;
            displaybuf[baseAdr + j * 3 + 2] = tmpb;
        }
    }

    m_DisplayCapturedBits(/*m_pFrameBuf*/ (BYTE *)displaybuf, &bih, m_pView->GetSafeHwnd());
}

void CFieldMap::m_DisplayCapturedBits(BYTE *pBuffer, BITMAPINFOHEADER *pbih, HWND hwndStill)
{
    RECT rc;
    ::GetWindowRect(hwndStill, &rc);
    long lStillWidth = rc.right - rc.left;
    long lStillHeight = rc.bottom - rc.top;

    HDC hdcStill = ::GetDC(hwndStill);
    PAINTSTRUCT ps;
    ::BeginPaint(hwndStill, &ps);

    SetStretchBltMode(hdcStill, COLORONCOLOR);
    StretchDIBits(hdcStill, 0, 0, 640, 480, 0, 0, 640, 480, pBuffer, (BITMAPINFO *)pbih, DIB_RGB_COLORS, SRCCOPY);

    ::EndPaint(hwndStill, &ps);
    ::ReleaseDC(hwndStill, hdcStill);
}

UINT CFieldMap::MapingThread(LPVOID pParam)
{
    CFieldMap *pfm = (CFieldMap *)pParam;
    int dianshu111;

    // 将位图信息读进来，并二值化
    BOOL weitu1[320][240];
    for (int i = 0; i < 320; i++)
        for (int j = 0; j < 240; j++)
            weitu1[i][j] = 0;
    for (i = 0; i < 324; i++)
        for (int j = 0; j < 222; j++)
        {
            //	weitu1[i/2][j/2]=0;
            if (pfm->dcMem.GetPixel(i, j) != 0)
            {
                weitu1[i][j] = 1; // 将dcmen复制到weitu[][]这个数组中
            }
        }
    for (i = 0; i <= 313; i++)
        for (int j = 0; j <= 220; j++)
        {
            weitu[i + 158][j + 130] = weitu1[i][j]; // 将这个weitu[][]平移到图像中间
        }
    dianshu111 = 0;
    for (i = 158; i <= 471; i++)
        for (int j = 130; j <= 350; j++)
        {
            if (weitu[i][j] == 1)
            {
                baidianx[dianshu111] = i;
                baidiany[dianshu111] = j;
                dianshu111++;
            }
        }
    // dianshu=dianshu;

    // 深度图生成
    int val_1;
    int juli;
    int shendux;
    int shenduy;
    for (i = 0; i < 640; i++)
        for (int j = 0; j < 480; j++)
        {
            juli = 30000;
            for (int z = 0; z < dianshu111; z++)
            {
                val_1 = sqrt((i - baidianx[z]) * (i - baidianx[z]) +
                             (j - baidiany[z]) *
                                 (j - baidiany[z])); // sqrt((i-x)*(i-x)+(j-y)*(j-y));//(i-x)*(i-x)+(j-y)*(j-y)/10;
                                                     // ////找目标点和场线点的距离
                if (val_1 < juli)
                {
                    shendux = baidianx[z];
                    shenduy = baidiany[z];
                    juli = val_1;
                    shendubiao[i][j] = juli; // 可以找到的目标点和场线点最小的距离
                }
            }
        }
    pfm->m_mapfile.SaveToFile((int *)shendubiao, 640 * 480 * 4, "fieldmap");

    pfm->m_printf("深度表生成完毕！可以进行显示了！");

    return 1L;
}

void CFieldMap::m_printf(CString inStr)
{
    if (m_pInfoList != NULL)
    {
        m_pInfoList->AddString(inStr);
        m_pInfoList->SetCurSel(m_pInfoList->GetCount() - 1);
    }
}
