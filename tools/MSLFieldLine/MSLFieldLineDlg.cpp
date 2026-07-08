// MSLFieldLineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MSLFieldLine.h"
#include "MSLFieldLineDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
  public:
    CAboutDlg();

    // Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum
    {
        IDD = IDD_ABOUTBOX
    };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    //{{AFX_MSG(CAboutDlg)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
// No message handlers
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMSLFieldLineDlg dialog

CMSLFieldLineDlg::CMSLFieldLineDlg(CWnd *pParent /*=NULL*/) : CDialog(CMSLFieldLineDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CMSLFieldLineDlg)
    m_nCor_R = 75;
    m_nField_len = 1800;
    m_nField_wid = 1200;
    m_nGoal_len = 75;
    m_nGoal_wid = 350;
    m_Mid_diameter = 400;
    m_nPen_len = 650;
    m_nPen_wid = 225;
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMSLFieldLineDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CMSLFieldLineDlg)
    DDX_Control(pDX, IDC_INFOLIST, m_cInfoList);
    DDX_Control(pDX, IDC_MAPVIEW, m_cMapView);
    DDX_Control(pDX, IDC_FIELDMAP, m_cFieldMap);
    DDX_Text(pDX, IDC_COR_R, m_nCor_R);
    DDX_Text(pDX, IDC_FIELD_LEN, m_nField_len);
    DDX_Text(pDX, IDC_FIELD_WID, m_nField_wid);
    DDX_Text(pDX, IDC_GOAL_LEN, m_nGoal_len);
    DDX_Text(pDX, IDC_GOAL_WID, m_nGoal_wid);
    DDX_Text(pDX, IDC_MID_DIAMETER, m_Mid_diameter);
    DDX_Text(pDX, IDC_PEN_LEN, m_nPen_len);
    DDX_Text(pDX, IDC_PEN_WID, m_nPen_wid);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMSLFieldLineDlg, CDialog)
//{{AFX_MSG_MAP(CMSLFieldLineDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_CREATEBMP, OnCreatebmp)
ON_BN_CLICKED(IDC_CREATE, OnCreate)
ON_BN_CLICKED(IDC_DISPLAY, OnDisplay)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMSLFieldLineDlg message handlers

BOOL CMSLFieldLineDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu *pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);  // Set big icon
    SetIcon(m_hIcon, FALSE); // Set small icon

    // TODO: Add extra initialization here
    m_cFieldMap.m_pView = &m_cMapView;
    m_cFieldMap.m_pInfoList = &m_cInfoList;

    return TRUE; // return TRUE  unless you set the focus to a control
}

void CMSLFieldLineDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMSLFieldLineDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMSLFieldLineDlg::OnQueryDragIcon()
{
    return (HCURSOR)m_hIcon;
}

int CMSLFieldLineDlg::SaveBitmapToFile(CBitmap *bitmap, LPSTR lpFileName)
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

    if (iBits <= 1)
        wBitCount = 1;
    else if (iBits <= 4)
        wBitCount = 4;
    else if (iBits <= 8)
        wBitCount = 8;
    else if (iBits <= 24)
        wBitCount = 24;
    else if (iBits <= 32)
        wBitCount = 32;

    // 计算调色板大小
    if (wBitCount <= 8)
        dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);

    // 设置位图信息头结构
    GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = Bitmap.bmWidth;
    bi.biHeight = Bitmap.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = wBitCount;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

    // 为位图内容分配内存
    hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
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

void CMSLFieldLineDlg::OnCreatebmp()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    m_cFieldMap.Field_Width = m_nField_len;
    m_cFieldMap.Field_Height = m_nField_wid;
    m_cFieldMap.Field_PenaltyArea_Width = m_nPen_wid;
    m_cFieldMap.Field_PenaltyArea_Height = m_nPen_len;   // 650;
    m_cFieldMap.Field_GoalArea_Width = m_nGoal_len;      // 75;
    m_cFieldMap.Field_GoalArea_Height = m_nGoal_wid;     // 350;
    m_cFieldMap.Field_DiameterofCenter = m_Mid_diameter; // 400;
    m_cFieldMap.Field_RadiusofCorner = m_nCor_R;         // 75;

    m_cFieldMap.CreateBMP();
}

void CMSLFieldLineDlg::OnCreate()
{
    // TODO: Add your control notification handler code here
    m_cFieldMap.CreateMap();
}

void CMSLFieldLineDlg::OnDisplay()
{
    // TODO: Add your control notification handler code here
    m_cFieldMap.m_DisplayMap();
}
