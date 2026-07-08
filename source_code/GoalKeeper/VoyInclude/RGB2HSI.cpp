// RGB2HSI.cpp: implementation of the CRGB2HSI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RGB2HSI.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRGB2HSI::CRGB2HSI()
{
    HSI = NULL;
    _RGBBuf = NULL;
    Mark = NULL;
    _nWidth = _nHeight = 0;
    Map = NULL;

    // pielx fix
    int piexl[640];
    int temp;
    for (int i = 0; i < 320; i++)
    {
        // left
        temp = 319 - (i * 1.08);
        if (temp >= 0)
        {
            piexl[319 - i] = temp;
        }
        else
        {
            piexl[319 - i] = -1;
        }
        // right
        temp = 320 + (i * 1.08);
        if (temp < 640)
        {
            piexl[320 + i] = temp;
        }
        else
        {
            piexl[320 + i] = -1;
        }
    }

    // logi fix
    logifix = new int[640 * 3];
    // init logifix is the index!
    for (i = 0; i < 640; i++)
    {
        temp = piexl[i];
        if (temp > 0)
        {
            logifix[i * 3] = piexl[i] * 3;
            logifix[i * 3 + 1] = piexl[i] * 3 + 1;
            logifix[i * 3 + 2] = piexl[i] * 3 + 2;
        }
        else
        {
            logifix[i * 3] = -1;
            logifix[i * 3 + 1] = -1;
            logifix[i * 3 + 2] = -1;
        }
    }
}

CRGB2HSI::~CRGB2HSI()
{
    _DelHSI();
    _DelMark();
    _DelMap();

    delete[] logifix;
}

BOOL CRGB2HSI::Convert()
{
    if (Mark == NULL || HSI == NULL)
    {
        return FALSE;
    }

    int *lCurHSIp = HSI;
    UCHAR *lCursrcp = _RGBBuf;
    UCHAR *lmarkp = Mark;
    for (int i = 0; i < _nWidth * _nHeight; i++)
    {
        if (*lmarkp == 0)
        {
            lCursrcp += 3;
            lCurHSIp += 3;
            lmarkp++;
            continue;
        }
        lmarkp++;
        B = *lCursrcp;
        lCursrcp++;
        G = *lCursrcp;
        lCursrcp++;
        R = *lCursrcp;
        lCursrcp++;

        /*max = _Max(R,G,B);
        min = _Min(R,G,B);*/
        max = B;
        if (max < G)
        {
            max = G;
        }
        if (max < R)
        {
            max = R;
        }

        min = B;
        if (min > G)
        {
            min = G;
        }
        if (min > R)
        {
            min = R;
        }

        // 计算H
        if (max != min)
        {
            if (max == R)
            {
                H = 60 * (G - B) / (max - min);
                if (G < B)
                    H = 60 * (G - B) / (max - min) + 360;
            }
            if (max == G)
                H = 60 * (B - R) / (max - min) + 120;
            if (max == B)
                H = 60 * (R - G) / (max - min) + 240;
        }
        else
            H = 0;

        // 计算S
        S = max - min;

        // 计算I
        I = max;

        // 将结果赋值到HSI矩阵里
        *lCurHSIp = H;
        lCurHSIp++;
        *lCurHSIp = S;
        lCurHSIp++;
        *lCurHSIp = I;
        lCurHSIp++;
    }
    return TRUE;
}

void CRGB2HSI::InitBuf(BYTE *rgbBuf, int inWidth, int inHeight)
{
    _RGBBuf = rgbBuf;
    if (inWidth != _nWidth || inHeight != _nHeight)
    {
        _nWidth = inWidth;
        _nHeight = inHeight;

        // 重新分配Mark矩阵
        _DelMark();
        Mark = new UCHAR[inWidth * inHeight];

        // 初始化Mark矩阵为全部转换
        for (int i = 0; i < inWidth * inHeight; i++)
        {
            Mark[i] = 1;
        }

        // 重新分配HSI矩阵
        _DelHSI();
        HSI = new int[inWidth * inHeight * 3];

        /*for (i=0;i<inWidth*inHeight*3;i++)
        {
            HSI[i] = 0;
        }*/

        // 重新分配map矩阵
        _DelMap();
        Map = new UCHAR[inWidth * inHeight];
        for (i = 0; i < inWidth * inHeight; i++)
        {
            Map[i] = 0;
        }
    }
}

void CRGB2HSI::_DelMark()
{
    if (Mark != NULL)
    {
        delete[] Mark;
        Mark = NULL;
    }
}

void CRGB2HSI::_DelHSI()
{
    if (HSI != NULL)
    {
        delete[] HSI;
        HSI = NULL;
    }
}

void CRGB2HSI::_ConvPixel(int x, int y)
{
    index = y * _nWidth * 3 + x * 3;
    B = _RGBBuf[index];
    G = _RGBBuf[index + 1];
    R = _RGBBuf[index + 2];

    max = _Max(R, G, B);
    min = _Min(R, G, B);

    // 计算H
    if (max != min)
    {
        if (max == R)
        {
            H = 60 * (G - B) / (max - min);
            if (G < B)
                H = 60 * (G - B) / (max - min) + 360;
        }
        if (max == G)
            H = 60 * (B - R) / (max - min) + 120;
        if (max == B)
            H = 60 * (R - G) / (max - min) + 240;
    }
    else
        H = 0;

    // 计算S
    S = max - min;

    // 计算I
    I = max;

    // 将结果赋值到HSI矩阵里
    HSI[index] = H;
    HSI[index + 1] = S;
    HSI[index + 2] = I;
}

int CRGB2HSI::_Max(int x, int y, int z)
{
    // 取x,y,z中的最大值
    if (x >= y && x >= z)
        return x;
    if (y >= x && y >= z)
        return y;
    if (z >= y && z >= x)
        return z;
    return z;
}

int CRGB2HSI::_Min(int x, int y, int z)
{
    // 取x,y,z中的最小值
    if (x <= y && x <= z)
        return x;
    if (y <= x && y <= z)
        return y;
    if (z <= y && z <= x)
        return z;
    return z;
}

UCHAR CRGB2HSI::GetMark(int x, int y)
{
    return Mark[y * _nWidth + x];
}

void CRGB2HSI::SetMark(int x, int y, UCHAR inflag)
{
    Mark[y * _nWidth + x] = inflag;
}

void CRGB2HSI::_DelMap()
{
    if (Map != NULL)
    {
        delete[] Map;
        Map = NULL;
    }
}

BOOL CRGB2HSI::Mapping(HSIParam *inparam, UCHAR inVal)
{
    if (inparam == NULL || Mark == NULL || HSI == NULL)
    {
        return FALSE;
    }

    // 将map清零
    /*for (int i=0;i<_nWidth*_nHeight;i++)
    {
        Map[i] = 0;
    }*/
    memset(Map, 0, _nWidth * _nHeight);

    if (inparam != NULL)
    {
        // 将HSIparam上次运算清零
        inparam->Clear();
    }

    // 规范H阈值范围
    while (inparam->Hue_max > 360)
    {
        inparam->Hue_max -= 360;
    }
    while (inparam->Hue_max < 0)
    {
        inparam->Hue_max += 360;
    }
    while (inparam->Hue_min > 360)
    {
        inparam->Hue_min -= 360;
    }
    while (inparam->Hue_min < 0)
    {
        inparam->Hue_min += 360;
    }

    // 判断H阈值是否跨界
    bool bPiece;
    if (inparam->Hue_max > inparam->Hue_min)
    {
        bPiece = false;
    }
    else
    {
        bPiece = true;
    }

    // 扫描像素数组
    int *lCursrc = HSI;
    UCHAR *lCurmap = Map;

    if (bPiece == false)
    {
        // h阈值范围没有跨越边限
        for (int i = 0; i < _nWidth * _nHeight; i++)
        {
            H = *lCursrc;
            lCursrc++;
            S = *lCursrc;
            lCursrc++;
            I = *lCursrc;
            lCursrc++;

            if (H > (inparam->Hue_min - 1) && H < (inparam->Hue_max + 1) && S > (inparam->Satu_min - 1) &&
                S < (inparam->Satu_max + 1) && I > (inparam->I_min - 1) && I < (inparam->I_max + 1))
            {
                // 符合条件，map中标记，并对像素进行统计
                *lCurmap = inVal;
                /*inparam->x += i;
                inparam->y += j;
                inparam->sum ++;*/
            }
            lCurmap++;
        }
        //////////////////////////////////////////////////////////////////////////
        /*for (int i=0;i<_nWidth;i++)
        {
            for (int j=0;j<_nHeight;j++)
            {
                tempp = &HSI[j*_nWidth*3 + i*3];
                H = *tempp;
                tempp ++;
                S = *tempp;
                tempp ++;
                I = *tempp;

                if (H>(inparam->Hue_min) && H<(inparam->Hue_max) && S>(inparam->Satu_min) && S<(inparam->Satu_max) &&
        I>(inparam->I_min) && I<(inparam->I_max))
                {
                    //符合条件，map中标记，并对像素进行统计
                    SetMap(i,j,inVal);
                    inparam->x += i;
                    inparam->y += j;
                    inparam->sum ++;
                }
            }
        }*/
        //////////////////////////////////////////////////////////////////////////
    }
    else
    {
        // h阈值范围跨越了边限
        for (int i = 0; i < _nWidth * _nHeight; i++)
        {
            H = *lCursrc;
            lCursrc++;
            S = *lCursrc;
            lCursrc++;
            I = *lCursrc;
            lCursrc++;

            if (H > -1 && H < (inparam->Hue_max + 1) && S > (inparam->Satu_min - 1) && S < (inparam->Satu_max + 1) &&
                I > (inparam->I_min - 1) && I < (inparam->I_max + 1))
            {
                // 符合条件，map中标记，并对像素进行统计
                *lCurmap = inVal;
                /*inparam->x += i;
                inparam->y += j;
                inparam->sum ++;*/
            }

            if (H > (inparam->Hue_min - 1) && H < 361 && S > (inparam->Satu_min - 1) && S < (inparam->Satu_max + 1) &&
                I > (inparam->I_min - 1) && I < (inparam->I_max + 1))
            {
                // 符合条件，map中标记，并对像素进行统计
                *lCurmap = inVal;
                /*inparam->x += i;
                inparam->y += j;
                inparam->sum ++;*/
            }
            lCurmap++;
        }

        /*/////////////////////////////////////////////////////////////////////////
        for (int i=0;i<_nWidth;i++)
        {
            for (int j=0;j<_nHeight;j++)
            {
                tempp = &HSI[j*_nWidth*3 + i*3];
                H = *tempp;
                tempp ++;
                S = *tempp;
                tempp ++;
                I = *tempp;

                if (H>0 && H<(inparam->Hue_max) && S>(inparam->Satu_min) && S<(inparam->Satu_max) && I>(inparam->I_min)
        && I<(inparam->I_max))
                {
                    //符合条件，map中标记，并对像素进行统计
                    SetMap(i,j,inVal);
                    inparam->x += i;
                    inparam->y += j;
                    inparam->sum ++;
                }

                if (H>(inparam->Hue_min) && H<360 && S>(inparam->Satu_min) && S<(inparam->Satu_max) &&
        I>(inparam->I_min) && I<(inparam->I_max))
                {
                    //符合条件，map中标记，并对像素进行统计
                    SetMap(i,j,inVal);
                    inparam->x += i;
                    inparam->y += j;
                    inparam->sum ++;
                }
            }
        }*/
        //////////////////////////////////////////////////////////////////////////
    }

    lastVal = inVal;

    /*/结算统计量
    if (inparam->sum > 0)
    {
        inparam->x /= inparam->sum;
        inparam->y /= inparam->sum;
    }*/
    return TRUE;
}

void CRGB2HSI::SetMap(int x, int y, UCHAR inVal)
{
    Map[y * _nWidth + x] = inVal;
}

BOOL CRGB2HSI::ShowMap(BYTE *rgbBuf, int inWidth, int inHeight)
{
    if (rgbBuf == NULL || inWidth != _nWidth || inHeight != _nHeight)
    {
        return FALSE;
    }
    UCHAR *lCurmapp = Map;
    UCHAR *lCursrcp = rgbBuf;
    for (int i = 0; i < _nWidth * _nHeight; i++)
    {
        if (*lCurmapp == lastVal)
        {
            *lCursrcp = 0x0;
            *(lCursrcp + 1) = 0x0;
            *(lCursrcp + 2) = 0xff;
        }
        lCurmapp++;
        lCursrcp += 3;
    }
    return TRUE;
}

UCHAR CRGB2HSI::GetPoint(int x, int y, UCHAR *inbuf)
{
    if (x < 0 || x >= _nWidth || y < 0 || y >= _nHeight)
    {
        return 0;
    }

    if (inbuf == NULL)
    {
        return 0;
    }

    return inbuf[y * _nWidth + x];
}

void CRGB2HSI::SetColor(BYTE *rgbBuf, int x, int y, RGBTRIPLE *prgb)
{
    if (x > _nWidth - 1 || y > _nHeight - 1 || x < 0 || y < 0)
        return;

    index = y * _nWidth * 3 + x * 3;
    rgbBuf[index] = prgb->rgbtBlue;
    rgbBuf[index + 1] = prgb->rgbtGreen;
    rgbBuf[index + 2] = prgb->rgbtRed;
}

void CRGB2HSI::PreviewMark(BYTE *rgbBuf, int inWidth, int inHeight)
{
    if (rgbBuf == NULL || inWidth != _nWidth || inHeight != _nHeight)
    {
        return;
    }

    UCHAR *lCurmapp = Mark;
    UCHAR *lCursrcp = rgbBuf;
    for (int i = 0; i < _nWidth * _nHeight; i++)
    {
        if (*lCurmapp == 0)
        {
            *lCursrcp = 0x0;
            *(lCursrcp + 1) = 0x0;
            *(lCursrcp + 2) = 0xff;
        }
        lCurmapp++;
        lCursrcp += 3;
    }
}

BOOL CRGB2HSI::ShowMap(BYTE *rgbBuf, int inWidth, int inHeight, UCHAR inVal, RGBTRIPLE inColor)
{
    if (rgbBuf == NULL || inWidth != _nWidth || inHeight != _nHeight)
    {
        return FALSE;
    }
    UCHAR *lCurmapp = Map;
    UCHAR *lCursrcp = rgbBuf;
    for (int i = 0; i < _nWidth * _nHeight; i++)
    {
        if (*lCurmapp == inVal)
        {
            *lCursrcp = inColor.rgbtBlue;
            *(lCursrcp + 1) = inColor.rgbtGreen;
            *(lCursrcp + 2) = inColor.rgbtRed;
        }
        lCurmapp++;
        lCursrcp += 3;
    }
    return TRUE;
}

void CRGB2HSI::Convert(BYTE *rgbBuf, int inWidth, int inHeight)
{
    InitBuf(rgbBuf, inWidth, inHeight);
    Convert();
}

void CRGB2HSI::m_FixOverlookOnLogiDeluxe(UCHAR *inbuf, int inwidth, int inheight)
{
    if (inwidth != 640 || inheight != 480)
    {
        return;
    }

    UCHAR *pmid = inbuf + 960;
    UCHAR *pline = inbuf;
    UCHAR *lefttp, *righttp;
    int tempindex;
    for (int i = 0; i < inheight; i++)
    {
        lefttp = righttp = pmid;
        for (int j = 0; j < 960; j++)
        {
            tempindex = 960 - j;
            if (logifix[tempindex] > 0)
            {
                *lefttp = *(pline + logifix[tempindex]);
            }
            tempindex = 960 + j;
            if (logifix[tempindex] > 0)
            {
                *righttp = *(pline + logifix[tempindex]);
            }
            lefttp--;
            righttp++;
        }
        pmid += 1920;
        pline += 1920;
    }
    /*int i;
    int j;

    for(i=0;i<590;i++)
    {
        for(j=0;j<480;j++)
        {
            inbuf[j*1920 + i*3 ]=inbuf[j*1920 + int(i*1.08)*3 ];
            inbuf[j*1920 + i*3 + 1]=inbuf[j*1920 + int(i*1.08)*3 + 1];
            inbuf[j*1920 + i*3 + 2]=inbuf[j*1920 + int(i*1.08)*3 + 2];
        }
    }


    for(i=590;i>25;i--)
    {
        for(j=0;j<480;j++)
        {
            inbuf[j*1920 + i*3 ]=inbuf[j*1920 + (i-24)*3 ];
            inbuf[j*1920 + i*3 + 1]=inbuf[j*1920 + (i-24)*3 + 1];
            inbuf[j*1920 + i*3 + 2]=inbuf[j*1920 + (i-24)*3 + 2];
        }
    }*/
}
