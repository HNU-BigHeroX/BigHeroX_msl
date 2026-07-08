// UPRColor.cpp: implementation of the UPRColor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
// #include "capt.h"
#include "UPRColor.h"
#include "math.h"
#include "..\.\ConstantFile\CamConstantSet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int Sector_Num = 144; // 整个屏幕由 多少 射线 分成多少区域
// int       In_field_R_map[Sector_Num]={10,20,30,40,50,60,70,80,90,100,110,120,130,140,150,160,170};//存储在场地内要
// 进行处理的点的每个扇区的半径
int In_field_R_map
    [Sector_Num]
    [2]; // 保存每个扇区当前贞作处理的最小半径和最大半径={240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240};//存储在场地内要
         // 进行处理的点的每个扇区的半径

// 要将哪几个角度排除在图像计算范围外 用来扣除 镜子支柱 和护球网//void UPRColor::make_in_film_radius_table(float
// f_x,float f_y,float f_a)
const float angle_num = 0;
const float angle[10] = {58, 60, 76, 78, 178, 180, 284, 282, 298, 300};

UPRColor::UPRColor()
{
    m_LoadTables();
    Init_Mask_sector();
    Init_Mask_sectorA();
    Init_Mask_sectorBLACK();
    Init_Mask_Checked_Box();
    Init_mask_checked_box_lookup_table();
    make_in_film_radius_table_init();

    xbshang = 900;
    ybshang = 600;
    pipeikaishi = 0;
    cdbl = MK;
    bool_max_connectivity_analyze_mask_int(); // 陈嵩加
    pipeijishu = 100;
    startpipei = 0;
    qishix = 0;
    qishiy = 0;
    qishijiao = 0;
    zuixiaoy2 = 0;
    zuixiaoy2 = 0;
    // 半径查找表生成
    for (int x1 = 0; x1 <= 640; x1++)
        for (int y = 0; y <= 480; y++)
        {
            if (((x1 - 320) * (x1 - 320) + (y - 240) * (y - 240)) < 60000)
                banjing[x1][y] = (x1 - 320) * (x1 - 320) + (y - 240) * (y - 240);
            else
                banjing[x1][y] = 60000;
        }
    for (int i = 0; i < 240; i++)
    {
        quxian[i] = 0;
    }
    for (i = 0; i < 240; i++)
    {
        quxian[i] = qulvb[i];
    }
    int j;
    for (i = 1; i < 240; i++)
    {
        if (quxian[i] == 0)
        {
            for (j = i + 1; j <= 240; j++)
            {
                if (quxian[j] != 0)
                {
                    quxian[i] = (quxian[j] - quxian[i - 1]) / (j - i + 1) + quxian[i - 1];
                    break;
                }
            }
        }
    }
    for (i = 0; i < 240; i++) // cs 后加无缩放畸变矫正表 不知道为什么 原来用 0 ~ 242
    {
        adjust_table[i] = quxian[i]; // 2.4;//1.67;//2.1;
    }
    /*
    for(i=1;i<240;i++)
    {
        quxian11[i]=quxian[i];
        for(j=quxian[i];j<quxian[i+1];j++)
            fanquxian[j]=i;
    }
    */
    for (i = 1; i < 240; i++)
    {
        fanquxian[int(quxian[i])] = (float)i;
    }
    for (i = 1; i < 3000; i++)
    {
        if (fanquxian[i] == 0)
        {
            for (j = i + 1; j <= 3000; j++)
            {
                if (fanquxian[j] != 0)
                {
                    fanquxian[i] = (fanquxian[j] - fanquxian[i - 1]) / (j - i + 1) + fanquxian[i - 1];
                    break;
                }
            }
        }
    }

    for (int x2 = -72; x2 < 72; x2++)
        for (int y2 = 0; y2 < 240; y2++)
        {
            //	xx=(double)sin(x2*2.5/57.4)*y2+320;
            //	xx=(double)cos(x2*2.5/57.4)*y2+240;
            shexianx[x2 + 72][y2] = int((double)sin(double(x2) * 2.5 / 57.29578) * y2 + 320);
            shexiany[x2 + 72][y2] = int((double)cos(double(x2) * 2.5 / 57.29578) * y2 + 240);
        }
    // 半径查找表生成完毕

    // 生成HSI查找表
    /*
    int R1;
    int G1;
    int B1;
//get_h[64][64][64]
    int max1;
    int min1;
    for(i=0;i<256;i++)
        for(int j=0;j<256;j++)
            for(int k=0;k<256;k++)
            {
                max1=Max(k,j,i);
                min1=Min(k,j,i);
                if(max1!=min1)
                {
                    if(max1==k)
                    {
                        get_h[i][j][k]=60*(j-i)/(max1-min1);
                        if(j<i)
                            get_h[i][j][k]=60*(j-i)/(max1-min1)+360;
                    }
                    if(max1==j)
                        get_h[i][j][k]=60*(i-k)/(max1-min1)+120;
                    if(max1==i)
                        get_h[i][j][k]=60*(k-j)/(max1-min1)+240;
                }
                else
                    get_h[i][j][k]=0;
                get_i[i][j][k] =(k+j+i)/3;// max;
                get_s[i][j][k] = max1-min1;

            }


*/
    // HSI查找表生成完毕
    // 深度图生成
    /*
        int juli;
        //int val_1;
for(i=0;i<=200;i++)
    for(int j=0;j<=150;j++)
    {
        juli=3600;
        for( int x=0;x<=200;x++)
            for(int y=0;y<=150;y++)
            {
                if(weitu[x][y]==1 && (i-x)*(i-x)+(j-y)*(j-y)<juli)
                {
                    juli=(i-x)*(i-x)+(j-y)*(j-y);
                    shendubiao[i][j]=juli;
                }

            }

    }
    */
    // 深度图生成完毕

    m_pImageBuf = NULL;
    xxx = 0;
    path_generate_init();

    if (zhi_yichu != 2)
    {
        zhi_yichu = 2;
        path_generate_init();
        int r, du;       //,pian,dian_shu,diandian
        int shan[19200]; // 单个扇形暂时存储区
        // int obstacle_shan_xing[36][1360];//扇形一 每个扇形10度 一个扇形1600个点 1600 = (240*240)/36
        // int obstacle_shan_xingx[36][1360];//扇形一 每个扇形10度 一个扇形1600个点 1600 = (240*240)/36
        // int obstacle_shan_xingy[36][1360];//扇形一 每个扇形10度 一个扇形1600个点 1600 = (240*240)/36
        // 36代表扇形号,1360代表该扇形的某点
        int x;
        int y;
        const int sacenter_x = 320;   // 扇中心X
        const int sacenter_y = 240;   // 扇中心Y
        const int saradius_min = 20;  // 扇半径最小长度
        const int saradius_max = 240; // 扇半径最大长度
        const int sageshu = 36;       // 扇个数  w              360/sageshu/0.25
        for (int j = 0; j < 36; j++)
            shandian[j] = 0;
        for (j = 0; j < sageshu; j++) // 存储空间初始化 0
            for (int i = 0; i < 6000; i++)
            { //	obstacle_shan_xing[j][i]=0;
                obstacle_shan_xingx[j][i] = 0;
                obstacle_shan_xingy[j][i] = 0;
            }
        /*
        int nxxxx;
        int x121;
        int y121;
        int jiao121;
        int chang121;
        for (i=80;i<560;i++)
            for(j=0;j<480;j++)
            {
                if(banjing[i][j]<57600)
                {
                    x121=i-320;
                    y121=j-240;
                    chang121=sqrt(banjing[i][j]);
                    if (chang121!=0)
                    {
                        //jiao121=int((asin(float(float(y121)/float(chang121)))*57.29578));

                        if(x121>0)
                            jiao121=int((90-asin(float(float(y121)/float(chang121)))*57.29578));
                        else if (x121<0)
                            jiao121=-int((90-asin(float(float(y121)/float(chang121)))*57.29578));
                        else if(y>0)
                            jiao121=180;
                        else
                            jiao121=0;
                    }
                    shandian[(jiao121+180)/10-1]++;
                    nxxxx=(jiao121+180)/10;
                    obstacle_shan_xingx[(jiao121+180)/10-1][shandian[(jiao121+180)/10-1]]=i;
                    obstacle_shan_xingy[(jiao121+180)/10-1][shandian[(jiao121+180)/10-1]]=j;
                }
            }
            */

        for (int k = 0; k < sageshu; k++) // 总循环 36个扇型
        {                                 //?????????????????????
            for (j = 0; j < 19200; j++)   // 初始化 0.25度一条射线 10度*4=40 条线 每条线120点 40*120=4800点
            {
                shan[j] = 0; // 存储记录每个点的 地址（数组偏移量 编号）
            }
            for (j = 0; j < 307200; j++) // 初始化
            {
                C1_Bool_Data[j] = 0;
            }

            for (du = 0; du < 360 / sageshu / 0.25; du++) // for(du=0;du<10*4;du++)
            {
                for (r = saradius_min; r < saradius_max; r++) // 半径40以下是机体不算
                {
                    x = (int)(sin((du + k * 40) * 3.14 / (180 * 4)) * r) + sacenter_x;
                    y = (int)(cos((du + k * 40) * 3.14 / (180 * 4)) * r) + sacenter_y;
                    shan[du * 240 + r] = 640 * y + x;
                    // y=(sin(2))*r+120;
                    // x=(cos(2))*r+160;
                    // z=(320*y)+x;
                    //	 str.Format("y=%f x=%f z=%f",y,x,z);
                    //	 AfxMessageBox(str);
                    // du_r[(int)z]=1;
                }
            }

            for (j = 0; j < 19200; j++) // 将当前扇型倒入 图像使 重复点重合掉
            {
                C1_Bool_Data[shan[j]] = 1;
            }
            for (int i = 0, j = 0; j < 307200;
                 j++) // 求出一个小扇形像素 按横扫描线存入obstacle_shan_xing  防止有重复点（消除3000多个重复点）
            {
                if (C1_Bool_Data[j] == 1)
                {
                    // obstacle_shan_xing[k][i]=j;
                    obstacle_shan_xingx[k][i] = j % 640;
                    obstacle_shan_xingy[k][i] = j / 640;
                    i++;
                }
            }
        }
    }

    pList = NULL;
    pEdit = NULL;
}

UPRColor::~UPRColor()
{
}

BOOL UPRColor::GetRGB(int x, int y, RGBTRIPLE *prgb)
{
    // 取x,y点的RGB颜色
    if (x > m_nWidth - 1 || y > m_nHeight - 1 || x < 0 || y < 0)
        return FALSE;
    prgb->rgbtBlue = m_pImageBuf[y * m_nWidth * 3 + x * 3];
    prgb->rgbtGreen = m_pImageBuf[y * m_nWidth * 3 + x * 3 + 1];
    prgb->rgbtRed = m_pImageBuf[y * m_nWidth * 3 + x * 3 + 2];
    return TRUE;
}

int UPRColor::Max(int x, int y, int z)
{
    // 取x,y,z中的最大值
    if (x >= y && x >= z)
        return x;
    else if (y >= x && y >= z)
        return y;
    else
        return z;
}

int UPRColor::Min(int x, int y, int z)
{
    // 取x,y,z中的最小值
    if (x <= y && x <= z)
        return x;
    else if (y <= x && y <= z)
        return y;
    else
        return z;
}

// DEL BOOL UPRColor::GetHSI(int x, int y, HSIParam *HSI)
// DEL //将x,y点的RGB转换成 HSI输出
// DEL {
// DEL //	HSI->H=zhizhen[B2][G2][R2];
// DEL //	HSI->S=get_s[B2][G2][R2];
// DEL //	HSI->I=get_i[B2][G2][R2];
// DEL 	B2 = m_pImageBuf[y*m_nWidth*3 + x*3];
// DEL 	G2 = m_pImageBuf[y*m_nWidth*3 + x*3 + 1];
// DEL 	R2 = m_pImageBuf[y*m_nWidth*3 + x*3 + 2];
// DEL 	max1=Max(R2,G2,B2);
// DEL 	min1=Min(R2,G2,B2);
// DEL 	if(max1!=min1)
// DEL 	{
// DEL 		if(max1==R2)
// DEL 		{
// DEL 			H=60*(G2-B2)/(max1-min1);
// DEL 			if(G2<B2)
// DEL 				H=60*(G2-B2)/(max1-min1)+360;
// DEL 		}
// DEL 		if(max1==G2)
// DEL 			H=60*(B2-R2)/(max1-min1)+120;
// DEL 		if(max1==B2)
// DEL 			H=60*(R2-G2)/(max1-min1)+240;
// DEL 	}
// DEL 	else
// DEL 	H=0;
// DEL 	I = max1;//(R+G+B)/3;
// DEL 	S = max1-min1;
// DEL 	return TRUE;
// DEL }

BOOL UPRColor::GetYUV(int x, int y, YUVParam *YUV)
{
    /*int R;
    int G;
    int B;
    //取x,y点的YUV颜色
    if (x>m_nWidth-1 || y>m_nHeight-1 || x<0 || y<0)
    return FALSE;
    B = m_pImageBuf[y*m_nWidth*3 + x*3];
    G = m_pImageBuf[y*m_nWidth*3 + x*3 + 1];
    R = m_pImageBuf[y*m_nWidth*3 + x*3 + 2];

    YUV->Y=0.3*R + 0.6*G + 0.1*B;

    YUV->U=-0.2*R - 0.3*G + 0.5*B;

    YUV->V=0.5*R - 0.45*G - 0.05*B;*/
    return TRUE;
}

BOOL UPRColor::GetHSIFix(Yuzhi *zz)
{
    // 取球和两个门的重心坐标，并对相应的2值化数组赋值。
    // 返回：球和两个门的直角坐标以及极坐标。
    // 只对全景（中间方块范围内）的区域做。

    RGBTRIPLE door1;
    door1.rgbtBlue = 0xFF;
    door1.rgbtGreen = 0x00;
    door1.rgbtRed = 0x00;
    RGBTRIPLE ball;
    ball.rgbtBlue = 0x00;
    ball.rgbtGreen = 0x00;
    ball.rgbtRed = 0xFF;
    RGBTRIPLE door2;
    door2.rgbtBlue = 0x88;
    door2.rgbtGreen = 0xFF;
    door2.rgbtRed = 0xFF;
    RGBTRIPLE heimo;
    heimo.rgbtBlue = 0x00;
    heimo.rgbtGreen = 0x00;
    heimo.rgbtRed = 0x00;
    HSIParam *hsi = new HSIParam;
    RGBTRIPLE dimian;
    dimian.rgbtBlue = 0x00;
    dimian.rgbtGreen = 0xFF;
    dimian.rgbtRed = 0x00;
    RGBTRIPLE heise;
    heise.rgbtBlue = 0x4D;
    heise.rgbtGreen = 0x00;
    heise.rgbtRed = 0x4D;
    RGBTRIPLE baise;
    baise.rgbtBlue = 0x88;
    baise.rgbtGreen = 0x88;
    baise.rgbtRed = 0xFF;
    int volh;
    int vols;
    int voli;
    int i;
    int j;
    int num1;

    memset(erzhi_b, 0, 640 * 480 * sizeof(BOOL));
    memset(erzhi_black, 0, 640 * 480 * sizeof(BOOL));
    memset(erzhi_di, 0, 640 * 480 * sizeof(BOOL));
    memset(erzhi_black, 0, 640 * 480 * sizeof(BOOL));
    memset(erzhi_bai, 0, 640 * 480 * sizeof(BOOL));

    for (i = 0; i < 480; i++)
    {
        SetColor(80, i, baise);
        SetColor(560, i, baise);
    }
    // 将HSI颜色，放入HSI颜色的矩阵。
    for (i = 80; i < 560; i++)
    {
        for (j = 0; j < 480; j++)
        {
            if (banjing[i][j] < 57600)
            {
                // GetHSI(i,j,hsi);
                //	hsi_h[i][j]=hsi->H;
                //	hsi_s[i][j]=hsi->S;
                //	hsi_i[i][j]=hsi->I;
                ///////////////////////////////////
                num1 = j * 1920 + i * 3;
                B2 = m_pImageBuf[num1];
                G2 = m_pImageBuf[num1 + 1];
                R2 = m_pImageBuf[num1 + 2];
                max1 = Max(R2, G2, B2);
                min1 = Min(R2, G2, B2);
                if (max1 != min1)
                {
                    if (max1 == R2)
                    {
                        volh = 60 * (G2 - B2) / (max1 - min1);
                        if (G2 < B2)
                            volh = 60 * (G2 - B2) / (max1 - min1) + 360;
                    }
                    if (max1 == G2)
                        volh = 60 * (B2 - R2) / (max1 - min1) + 120;
                    if (max1 == B2)
                        volh = 60 * (R2 - G2) / (max1 - min1) + 240;
                }
                else
                    volh = 0;
                voli = max1; //(R+G+B)/3;
                vols = max1 - min1;

                if (zz->bhs > zz->bhx)
                {
                    if (volh <= zz->bhs && volh >= zz->bhx && vols >= zz->bs && voli >= zz->bix && voli <= zz->bis)
                    {
                        erzhi_b[i][j] = 1;
                        SetColor(i, j, ball);
                    }
                }

                if (zz->bhs < zz->bhx)
                {
                    if ((volh >= zz->bhx || volh <= zz->bhs) && vols >= zz->bs && voli >= zz->bix && voli <= zz->bis)
                    {
                        erzhi_b[i][j] = 1;
                        SetColor(i, j, ball);
                    }
                }
                //	if ((volh<=zz->dhs && volh>=zz->dhx) && vols>=zz->ds && voli>=zz->dix && voli<=zz->dis)
                //	{
                //   		erzhi_d[i][j]=1;
                //	SetColor(i,j,door1);
                //	}
                else if ((volh <= zz->chs && volh >= zz->chx) && vols >= zz->cs && voli >= zz->cix && voli <= zz->cis)
                {
                    erzhi_di[i][j] = 1;
                    //	SetColor(i,j,dimian);
                }
                else if (vols <= zz->sh && voli <= zz->ih)
                {
                    erzhi_black[i][j] = 1;
                    SetColor(i, j, heise);
                }
                else if (vols <= zz->sw && voli >= zz->iw)
                // else if (vols<=4)//  && voli >=200)
                {
                    erzhi_bai[i][j] = 1;
                    SetColor(i, j, baise);
                }
            }
        }
    }

    // 完毕
    // 球2值化

    delete hsi;
    return TRUE;
}

void UPRColor::ImageBuf(int Width, int Height, BYTE *Buf)
{
    m_nHeight = Height;
    m_nWidth = Width;
    m_pImageBuf = Buf;
}

BOOL UPRColor::SetColor(int x, int y, RGBTRIPLE rgb)
{
#ifndef UPRColor_DISPLAY_SetColor
    return TRUE;
#endif
    //	return TRUE;
    // 将x,y点赋值成rgb色
    if (x > m_nWidth - 1 || y > m_nHeight - 1 || x < 0 || y < 0)
        return FALSE;

    psetclr = m_pImageBuf + y * 1920 + x * 3;
    /*
    *psetclr = (*psetclr+rgb.rgbtBlue*2)/3;
    psetclr ++;
    *psetclr = (*psetclr+rgb.rgbtGreen*2)/3;
    psetclr ++;
    *psetclr = (*psetclr+rgb.rgbtRed*2)/3;
        */

    m_pImageBuf[y * 1920 + x * 3] = rgb.rgbtBlue;
    m_pImageBuf[y * 1920 + x * 3 + 1] = rgb.rgbtGreen;
    m_pImageBuf[y * 1920 + x * 3 + 2] = rgb.rgbtRed;

    return TRUE;
}

void UPRColor::DrawFocus(int x, int y, RGBTRIPLE *prgb)
// x,y位置,画PRGB颜色十字线
{
#ifndef UPRColor_DISPLAY_DrawFocus
    return;
#endif

    for (int i = -12; i < 13; i++)
    {
        for (int j = -12; j < 13; j++)
        {
            if (abs(i) < 2 || abs(j) < 2) // || i*i+j*j<25)
            {
                SetColor(x + i, y + j, *prgb);
            }
        }
    }
}

int UPRColor::quzhi(int x)
{
    x = xxx;
    return xxx;
}

void UPRColor::jizuobiao(double x, double y, double *jiao, double *chang, int zhongxinx, int zhongxiny)
{
    // 将结构体Yuzhi中的x,y转换成极坐标赋值给chang,jiao

    double x1;
    double y1;
    if ((x != zhongxinx) || (y != zhongxiny))
    {
        x1 = double(x - zhongxinx);
        y1 = double(y - zhongxiny);
        *chang = sqrt(x1 * x1 + y1 * y1);
        if (x1 > 0)
            *jiao = (90 - asin(y1 / *chang) * 57.29578);
        else
            *jiao = -(90 - asin(y1 / *chang) * 57.29578);
    }
    else
    {
        *chang = 0;
        *jiao = 0;
    }
}

void UPRColor::zhizuobiao(double *x, double *y, double jiao, double chang, double zhongxinx, double zhongxiny)
{
    //	a->
    //	double x,y,jiao,chang;
    *x = sin(jiao / 57.29578) * chang + zhongxinx;
    *y = cos(jiao / 57.29578) * chang + zhongxiny;
}

void UPRColor::lianxuyu(int val, int fangshi, Yuzhi *HSI11)
{
    RGBTRIPLE ball1;
    ball1.rgbtBlue = 0x00;
    ball1.rgbtGreen = 0x00;
    ball1.rgbtRed = 0xFF;
    RGBTRIPLE door1;
    door1.rgbtBlue = 0xFF;
    door1.rgbtGreen = 0x00;
    door1.rgbtRed = 0x00;
    RGBTRIPLE door2;
    door2.rgbtBlue = 0x88;
    door2.rgbtGreen = 0xFF;
    door2.rgbtRed = 0xFF;
    RGBTRIPLE dimian;
    dimian.rgbtBlue = 0x00;
    dimian.rgbtGreen = 0xFF;
    dimian.rgbtRed = 0x00;

    int x1 = 0;
    x1 = 10;
    int n2 = 0;
    n2 = 1;
    int i = 0;
    int j = 0;
    int o = 0;
    int p = 0;
    int n = 0;
    int n1 = 0;
    n = 0;
    int n22;
    // erzhi_b[1][1]=1;
    int x = 0;
    int y = 0;
    int val11 = 0;
    int meiyong = 0;
    int x4 = 0;
    int y4 = 0;
    int biaoding[6000] = {0};
    //	for(long a10=0;a10<60000;a10++)
    //	{
    //		ax1[a10]=0;
    //		ay1[a10]=0;
    //	}
    memset(ax1, 0, 60000 * sizeof(int));
    memset(ay1, 0, 60000 * sizeof(int));
    memset(aa1, 0, 6000 * sizeof(int));
    memset(lianxu, 0, 645 * 485 * sizeof(int));
    memset(a_1, 0, 640 * 480 * sizeof(int));

    if (val == 1)
    {
        for (i = 80; i < 560; i++)
            for (j = 0; j < 480; j++)
            {
                lianxu[i][j] = erzhi_b[i][j];
                //	if(erzhi_di[i][j]==1)
                //	SetColor(i,j,ball1);
            }
    }
    if (val == 2)
    {
        //	for(i=80;i<560;i++)
        //		for(j=0;j<480;j++)
        //		{
        //			lianxu[i][j]=erzhi_d[i][j];
        //		}
    }

    int pengzhangdian;
    if (val == 4)
    {
        memcpy(erzhi_yuandi, erzhi_di, 640 * 480 * sizeof(BOOL));
        for (i = 80; i < 560; i++)
            for (j = 0; j < 480; j++)
            {
                //	lianxu[i][j]=0;
                if (erzhi_di[i][j] == 1 && banjing[i][j] < 50000)
                {
                    erzhi_yuandi[i][j] = erzhi_di[i][j];

                    if ((erzhi_di[i - 1][j] != 1 || erzhi_di[i + 1][j] != 1 || erzhi_di[i][j - 1] != 1 ||
                         erzhi_di[i][j + 1] != 1))
                    // SetColor(i,j,dimian);
                    {
                        for (pengzhangdian = -16; pengzhangdian < 16; pengzhangdian++)
                        {
                            lianxu[i + pengzhangdian][j] = 1;
                            lianxu[i][j + pengzhangdian] = 1;
                            // SetColor(i,j,dimian);
                            //	SetColor(i+pengzhangdian,j,dimian);
                            //	SetColor(i,j+pengzhangdian,dimian);
                        }
                    }
                    else
                    {
                        lianxu[i][j] = 1;
                        //	SetColor(i,j,dimian);
                    }
                }
                //	else
                //	erzhi_di[i][j]=0;
                //	if(erzhi_bai[i][j]==1)
                //		lianxu[i][j]=1;
            }
    }

    for (i = 83; i < 557; i++)
        for (j = 3; j < 477; j++)
        {
            if (banjing[i][j] <= 52900)
            {
                if (lianxu[i][j] == 1 && a_1[i][j] == 0)
                {
                    n++;
                    aa1[n] = 0;
                    n2 = 1;
                    ax1[n2] = i;
                    ay1[n2] = j;
                    x1 = 10;
                    while (x1 > 0)
                    {
                        x1 = 0;
                        for (int z2 = 1; z2 <= n2; z2++)
                            for (int z = -1; z <= 1; z++)
                                for (int z1 = -1; z1 <= 1; z1++)
                                {
                                    if (lianxu[ax1[z2] + z][ay1[z2] + z1] == 1 && a_1[ax1[z2] + z][ay1[z2] + z1] == 0)
                                    {
                                        a_1[ax1[z2] + z][ay1[z2] + z1] = n;
                                        aa1[n]++;
                                        n2++;
                                        ax1[n2] = ax1[z2] + z;
                                        ay1[n2] = ay1[z2] + z1;
                                        x1++;
                                    }
                                }
                    }
                }
            }
        }
    // n1=a11[i];//

    if (fangshi == 1)
    {
        for (i = 80; i < 560; i++)
            for (j = 0; j < 480; j++)
            {
                if (a_1[i][j] != 0 && biaoding[a_1[i][j]] == 0 && banjing[i][j] < 55225)
                {
                    if (erzhi_di[i][j] == 1 ||
                        banjing[i][j] <
                            900) // ||erzhi_di[i-6][j]==1||erzhi_di[i-5][j]==1||erzhi_di[i-4][j]==1||erzhi_di[i-3][j]==1||erzhi_di[i-2][j]==1||erzhi_di[i-1][j]==1||erzhi_di[i][j]==1||erzhi_di[i+1][j]==1||erzhi_di[i+2][j]==1||erzhi_di[i+3][j]==1||erzhi_di[i-4][j]==1||erzhi_di[i-5][j]==1||erzhi_di[i-6][j]==1||erzhi_di[i][j-6]==1||erzhi_di[i][j-5]==1||erzhi_di[i][j-4]==1||erzhi_di[i][j-3]==1||erzhi_di[i][j-2]==1||erzhi_di[i][j-1]==1||erzhi_di[i][j+1]==1||erzhi_di[i][j+2]==1||erzhi_di[i][j+3]==1||erzhi_di[i][j+4]==1||erzhi_di[i][j+5]==1||erzhi_di[i][j+6]==1)
                    {
                        biaoding[a_1[i][j]] = 1;
                    }
                }
            }
    }

    n1 = 0;
    if (fangshi == 1)
    {
        for (i = 0; i < 6000; i++)
        {
            if (aa1[i] > n1 && biaoding[i] == 1) // n1)
            {
                n1 = aa1[i];
                n22 = i;
            }
        }
    }
    n1 = 0;

    if (fangshi != 1)
    {

        for (i = 0; i < 6000; i++)
        {
            if (aa1[i] > n1) //&& biaoding[i]==1)//n1)
            {
                n1 = aa1[i];
                n22 = i;
            }
        }
    }

    val11 = 0;
    x = 0;
    y = 0;

    for (i = 80; i < 560; i++)
        for (j = 0; j < 480; j++)
        {

            //	if(a[i][j]==4 ||a[i][j]==5||a[i][j]==2||a[i][j]==6||a[i][j]==7)
            if (a_1[i][j] == n22)
            {
                if (val == 1)
                {
                    erzhi_b[i][j] = 1;
                    //	SetColor(i,j,ball1);
                }

                if (val == 2)
                {
                    //	SetColor(i,j,door1);
                    //					erzhi_d[i][j]=1;
                }

                if (val == 4)
                {
                    // SetColor(i,j,dimian);
                    erzhi_di[i][j] = 1;
                }

                val11++;
                x = x + i;
                y = y + j;
            }
            else
            {
                if (val == 1)
                {
                    erzhi_b[i][j] = 0;
                }

                if (val == 2)
                {
                    //					erzhi_d[i][j]=0;
                }

                if (val == 4)
                {
                    erzhi_di[i][j] = 0;
                }
            }
        }

    if (val11 != 0)
    {
        if (val == 1)
        {
            HSI11->ball_x = x / val11;
            HSI11->ball_y = y / val11;
            HSI11->ball_zhong = val11;
            jizuobiao(HSI11->ball_x, HSI11->ball_y, &HSI11->ball_jiao, &HSI11->ball_chang, 320, 240);
        }
        if (val == 3)
        {
            HSI11->jdoor_x = x / val11;
            HSI11->jdoor_y = y / val11;
            HSI11->jdoor_zhong = val11;
            jizuobiao(HSI11->jdoor_x, HSI11->jdoor_y, &HSI11->jdoor_jiao, &HSI11->jdoor_chang, 320, 240);
        }
        if (val == 2)
        {
            HSI11->ddoor_x = x / val11;
            HSI11->ddoor_y = y / val11;
            HSI11->ddoor_zhong = val11;
            jizuobiao(HSI11->ddoor_x, HSI11->ddoor_y, &HSI11->ddoor_jiao, &HSI11->ddoor_chang, 320, 240);
        }
    }
    else
    {
        if (val == 1)
        {
            HSI11->ball_x = 0;
            HSI11->ball_y = 0;
            HSI11->ball_zhong = 0;
            jizuobiao(HSI11->ball_x, HSI11->ball_y, &HSI11->ball_jiao, &HSI11->ball_chang, 320, 240);
        }
        if (val == 3)
        {
            HSI11->jdoor_x = 0;
            HSI11->jdoor_y = 0;
            HSI11->jdoor_zhong = 0;
            jizuobiao(HSI11->jdoor_x, HSI11->jdoor_y, &HSI11->jdoor_jiao, &HSI11->jdoor_chang, 320, 240);
        }
        if (val == 2)
        {
            HSI11->ddoor_x = 0;
            HSI11->ddoor_y = 0;
            HSI11->ddoor_zhong = 0;
            jizuobiao(HSI11->ddoor_x, HSI11->ddoor_y, &HSI11->ddoor_jiao, &HSI11->ddoor_chang, 320, 240);
        }
    }
}

void UPRColor::Gethei(int s, int i)
{
    // 取球和两个门的重心坐标，并对相应的2值化数组赋值。
    // 返回：球和两个门的直角坐标以及极坐标。
    // 只对全景（中间方块范围内）的区域做。
}

void UPRColor::path_generate_init(void)
{
    int i;
    BW_data_ = new unsigned char *[240];
    //	unsigned char *BW_data_[240][320];
    for (i = 0; i < 240; i++)
    {
        BW_data_[i] = new unsigned char[320];
    }

    area_grow_dataA = new int *[240];
    for (i = 0; i < 240; i++)
    {
        area_grow_dataA[i] = new int[320];
    }

    area_grow_datao = new bool *[240];
    for (i = 0; i < 240; i++)
    {
        area_grow_datao[i] = new bool[320];
    }

    area_grow_dataAtcx = new unsigned int[10000]; // currently当前生长的一圈的纪录
    area_grow_dataAtcy = new unsigned int[10000]; // currently当前生长的一圈的纪录
    area_grow_dataAtcl = 0;                       // 纪录currently当前这一圈有几个像素

    area_grow_dataAtpx = new unsigned int[10000]; // pre-	上一次生长的一圈的纪录
    area_grow_dataAtpy = new unsigned int[10000]; // pre-	上一次生长的一圈的纪录
    area_grow_dataAtpl = 0;                       // 纪录pre-	上一次这一圈有几个像素

    pathx = new int[10000]; // 存储路径的 顺序坐标
    pathy = new int[10000];
    pathl = 0;

    //	pathxII[1000];//用线段 拟和后的路径 端点坐标
    //	pathyII[1000];
    //	pathlII=0;
}

// 给出一点种子seed.x seed.y 得到由此点开始生长的连通域
// int starty  int startx    开始点  中止点
// int endy    int endx      终止点  终止点
// 分辨率	  320*240
// BW_data	  320*240       二值化 输入地图 1可以走的点 0不能走的点
// path_data_out		320*240
// char UPRColor::path_generate(unsigned char *BW_data  ,int starty,int startx,int endy,int endx,unsigned char
// *path_data_out)
char UPRColor::path_generate(int startx, int starty, int endx, int endy, int *xx, int *yy)
{
    RGBTRIPLE door1;
    door1.rgbtBlue = 0x00;
    door1.rgbtGreen = 0xFF;
    door1.rgbtRed = 0x00;
    *xx = 400;
    *yy = 400;
    char path_result =
        0; // 0默认无状态  1成功 2失败一开始点黑色 3失败二目标点黑色 4找不到路径超过1500点还没到达目标点
           // 5生长完连通域还没生长到目标点 失败 6失败一开始点出了范围[320*240] 7失败二目标点出了范围[320*240]
    const int x320 = 320; // 320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240 = 240; // 240;//处理图像数据的大小设置

    int circumscription_weight;
    int i, j, x, y;
    int x_add, x_reduce, y_add, y_reduce;

    if ((starty < 0) || (y240 < starty) || (startx < 0) || (x320 < startx))
    {
        path_result = 6;      // 6失败一开始点出了范围[320*240]
        return (path_result); // 如果 没成功就返回
    }
    if ((endy < 0) || (y240 < endy) || (endx < 0) || (x320 < endx))
    {
        path_result = 7;      // 7失败二目标点出了范围[320*240]
        return (path_result); // 如果 没成功就返回
    }
    // C[n]的存储结构类似于 连通域分块的输出int aaa[y][x] 每个像素存储的数值是其所属的生长区的编号
    // 计算每个生长区的中心颜色 按照颜色排序 将相邻的两个颜色差在5内的区域合并到小的区号中
    // bool area_grow_rule(unsigned char cH,unsigned char cS,unsigned char cV,unsigned char eH,unsigned char eS,unsigned
    // char eV)//currently  entourage {bool a;
    // a=((BW_data_out[y+1][x-1]==1)&&(abs(dataHSV[y+1][x-1][0]-tempH)<3)&&(abs(dataHSV[y+1][x-1][2]-tempV)<30));
    // return(a)
    // }
    // 生长法则：如果当前点周围有颜色差5以内的已标定点就把当前点归入这个以标定点的区域中去
    // 手动限定亮度变化极限相临两点差40以上就不算同一颜色 用于判定边线 颜色边界 颜色分割 首先从0，0点开始区域生长
    // 并记录面积如果 生长不动了[2次生长后面积不变] 就换已标定区域外的一点接着再做新的一轮生长
    // 如此把整个屏幕生长填充90%以上     记录每个区域面积 中心颜色
    // 将中心颜色相差5以上同时每种颜色面积在20以上的分成两个不同颜色类 最后将每种颜色区域合并 按照包含象素生成查找表或
    // 树型域值表

    // 新快速区域生长法 使用数组Atpx Atpy纪录上一次生长出的一圈像素A`每个像素的x和y   Atpl纪录上一圈有几个像素
    // 检测A`中的每个像素 周围不是A中的有效点且符合生长规则   记录这些点为新生长的一圈纪录在A``中
    // 将A``合并入A 并A``倒入A`
    // 注:A`由Atpx Atpy Atpl实际纪录
    // 注:A``由Atcx Atcy Atcl实际纪录
    // 注:种子初始化时由A`第一个存储单元存储 同时Atpl=1
    // 注意：因为有时种子不发芽 变成孤立点因为种子存储在A`[0]中平常不保存这里的数据 所以一定注意保存此特殊情况点
    //*circumscription_weight=0;//连通域重量计数清零
    i = 0;
    for (y = 0; y < y240; y++)
    {
        for (x = 0; x < x320; x++)
        {
            BW_data_[y][x] = erzhi_black[x][y]; // 将源数据一维数据倒到二维
            i++;
            area_grow_dataA[y][x] = 0; // 清零
            area_grow_datao[y][x] = 0;
        }
    }
    // 黑边处理  防止出现边缘冒了//////
    for (x = 0; x < x320; x++)
    {
        BW_data_[0][x] = 0;
        BW_data_[y240 - 1][x] = 0;
    }
    for (y = 1; y < y240 - 1; y++)
    {
        BW_data_[y][0] = 0;
        BW_data_[y][x320 - 1] = 0;
    }
    // 黑边处理  防止出现边缘冒了//////
    if (BW_data_[endy][endx] == 1)
    {
        if (BW_data_[starty][startx] == 1)
        {
            // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●

            area_grow_dataAtcl = 0; // 纪录当前次这一圈有几个像素
            area_grow_dataAtpl = 0; // 纪录当前次这一圈有几个像素
            // 1设置这次生长的起点种子//int starty  int startx    开始点  中止点
            area_grow_dataAtpx[0] = startx; ////手动设置种子
            area_grow_dataAtpy[0] = starty; ////手动设置种子
            // area_grow_dataAtcl=1;
            area_grow_dataAtpl = 1; ////手动设置种子

            // 注意：因为有时种子不发芽 变成孤立点因为种子存储在A`[0]中平常不保存这里的数据 所以一定注意保存此特殊情况点
            // 表像种子长不开时 会出现一个点都没有的情况
            // 所以在这里优先纪录
            area_grow_dataA[starty][startx] = 1;
            circumscription_weight = circumscription_weight + 1; // 连通域重量+1

            // 2开始生长
            for (i = 2; i < 1500; i++) // 生成生长区域 的生长次数
            {

                area_grow_dataAtcl = 0; // 纪录当前次这一圈有几个像素 注意：有重复的
                for (j = 0; j < (int)area_grow_dataAtpl; j++)
                {                              // 周围不是A中的有效点且符合生长规则
                    x = area_grow_dataAtpx[j]; // 当前次生长的一圈的纪录
                    y = area_grow_dataAtpy[j]; // 当前次生长的一圈的纪录
                    x_add = x + 1;
                    x_reduce = x - 1;
                    y_add = y + 1;
                    y_reduce = y - 1;
                    if ((area_grow_dataA[y_add][x_reduce] == 0) && (BW_data_[y_add][x_reduce] == 1))
                    {
                        area_grow_dataAtcy[area_grow_dataAtcl] = y_add;    // 当前次生长的一圈的纪录
                        area_grow_dataAtcx[area_grow_dataAtcl] = x_reduce; // 当前次生长的一圈的纪录
                        area_grow_dataAtcl++;
                        area_grow_dataA[y_add][x_reduce] = i; // 将A``合并入A 不会有重复点了
                    }
                    if ((area_grow_dataA[y_add][x + 0] == 0) && (BW_data_[y_add][x + 0] == 1))
                    {
                        area_grow_dataAtcy[area_grow_dataAtcl] = y_add; // 当前次生长的一圈的纪录
                        area_grow_dataAtcx[area_grow_dataAtcl] = x + 0; // 当前次生长的一圈的纪录
                        area_grow_dataAtcl++;
                        area_grow_dataA[y_add][x + 0] = i; // 将A``合并入A 不会有重复点了
                    }
                    if ((area_grow_dataA[y_add][x_add] == 0) && (BW_data_[y_add][x_add] == 1))
                    {
                        area_grow_dataAtcy[area_grow_dataAtcl] = y_add; // 当前次生长的一圈的纪录
                        area_grow_dataAtcx[area_grow_dataAtcl] = x_add; // 当前次生长的一圈的纪录
                        area_grow_dataAtcl++;
                        area_grow_dataA[y_add][x_add] = i; // 将A``合并入A 不会有重复点了
                    }
                    if ((area_grow_dataA[y + 0][x_reduce] == 0) && (BW_data_[y + 0][x_reduce] == 1))
                    {
                        area_grow_dataAtcy[area_grow_dataAtcl] = y + 0;    // 当前次生长的一圈的纪录
                        area_grow_dataAtcx[area_grow_dataAtcl] = x_reduce; // 当前次生长的一圈的纪录
                        area_grow_dataAtcl++;
                        area_grow_dataA[y + 0][x_reduce] = i; // 将A``合并入A 不会有重复点了
                    }
                    //	||
                    //((area_grow_dataA[y+0][x+0]==1)&&(abs(dataHSV[y+0][x+0][0]-tempH)<5)&&(abs(dataHSV[y+0][x+0][2]-tempV)<30)&&(abs(dataHSV[y+0][x+0][1]-tempS)<40))
                    if ((area_grow_dataA[y + 0][x_add] == 0) && (BW_data_[y + 0][x_add] == 1))
                    {
                        area_grow_dataAtcy[area_grow_dataAtcl] = y + 0; // 当前次生长的一圈的纪录
                        area_grow_dataAtcx[area_grow_dataAtcl] = x_add; // 当前次生长的一圈的纪录
                        area_grow_dataAtcl++;
                        area_grow_dataA[y + 0][x_add] = i; // 将A``合并入A 不会有重复点了
                    }
                    if ((area_grow_dataA[y_reduce][x_reduce] == 0) && (BW_data_[y_reduce][x_reduce] == 1))
                    {
                        area_grow_dataAtcy[area_grow_dataAtcl] = y_reduce; // 当前次生长的一圈的纪录
                        area_grow_dataAtcx[area_grow_dataAtcl] = x_reduce; // 当前次生长的一圈的纪录
                        area_grow_dataAtcl++;
                        area_grow_dataA[y_reduce][x_reduce] = i; // 将A``合并入A 不会有重复点了
                    }
                    if ((area_grow_dataA[y_reduce][x + 0] == 0) && (BW_data_[y_reduce][x + 0] == 1))
                    {
                        area_grow_dataAtcy[area_grow_dataAtcl] = y_reduce; // 当前次生长的一圈的纪录
                        area_grow_dataAtcx[area_grow_dataAtcl] = x + 0;    // 当前次生长的一圈的纪录
                        area_grow_dataAtcl++;
                        area_grow_dataA[y_reduce][x + 0] = i; // 将A``合并入A 不会有重复点了
                    }
                    if ((area_grow_dataA[y_reduce][x_add] == 0) && (BW_data_[y_reduce][x_add] == 1))
                    {
                        area_grow_dataAtcy[area_grow_dataAtcl] = y_reduce; // 当前次生长的一圈的纪录
                        area_grow_dataAtcx[area_grow_dataAtcl] = x_add;    // 当前次生长的一圈的纪录
                        area_grow_dataAtcl++;
                        area_grow_dataA[y_reduce][x_add] = i; // 将A``合并入A 不会有重复点了
                    }
                }

                if (area_grow_dataA[endy][endx] != 0) // 说明生长过了
                {
                    path_result = 1; // 成功
                    break;
                }
                if (area_grow_dataAtcl == 0)
                {
                    path_result = 5; // 失败 生长完连通域还没生长到目标点
                    break;
                }
                for (j = 0; j < int(area_grow_dataAtcl); j++)
                {
                    area_grow_dataAtpx[j] = area_grow_dataAtcx[j];
                    area_grow_dataAtpy[j] = area_grow_dataAtcy[j];
                }
                area_grow_dataAtpl = area_grow_dataAtcl;      // A``倒入At先去掉重复的点重新计算Atcl
                circumscription_weight += area_grow_dataAtcl; // 连通域重量+当前一圈的点数
            }
            // 生长完成
            if (i == 1500)
            {
                path_result = 4; // 失败 找完1500层点还没生长到目标点
            }

            // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
        }
        else
        {
            path_result = 2; // 失败一开始点黑色
        }
    }
    else
    {
        path_result = 2; // 3失败二目标点黑色
    }

    if (path_result != 1)
    {
        return (path_result); // 如果 没成功就返回
    }
    // i现在存的是 长了多少层
    // 1设置这次生长的起点种子int endy,int endx
    x = endx; ////手动设置种子
    y = endy; ////手动设置种子
    pathl = i;
    // 生长不开时的问题??????????????????????????????????????
    // 2开始反向生长 几乎为单线生长
    for (; i > 1; i--) // 生成生长区域 的生长次数  计数最低为1 开始点层数1
    {
        area_grow_datao[y][x] = 1; //=i-1
        pathx[i] = x;              // 存储路径的 顺序坐标
        pathy[i] = y;
        // 因为一个当前点可能对应多个父点 具体向哪个父点回溯前进 就要看那个父点方向的数值减小率最大即最速下降方向
        // 在方形的像素中是  横竖 比斜角 变化速率大 1.414:1
        // 起始比较斜率应该算三个点的 综合的斜率但是简单起见就只认为 横竖 比斜角大了
        // 这段程序查找规律是先找 横竖正方向的点  再找四个斜角方向的点
        // 最先符合规则的有效点为下次前进目标父点
        if (area_grow_dataA[y + 1][x + 0] == (i - 1))
        {
            y = y + 1; // 当前次生长的一圈的纪录
            x = x + 0; // 当前次生长的一圈的纪录
            continue;
        }
        if (area_grow_dataA[y + 0][x - 1] == (i - 1))
        {
            y = y + 0; // 当前次生长的一圈的纪录
            x = x - 1; // 当前次生长的一圈的纪录
            continue;
        }
        if (area_grow_dataA[y + 0][x + 1] == (i - 1))
        {
            y = y + 0; // 当前次生长的一圈的纪录
            x = x + 1; // 当前次生长的一圈的纪录
            continue;
        }
        if (area_grow_dataA[y - 1][x + 0] == (i - 1))
        {
            y = y - 1; // 当前次生长的一圈的纪录
            x = x + 0; // 当前次生长的一圈的纪录
            continue;
        }
        //	||
        //((area_grow_dataA[y+0][x+0]==1)&&(abs(dataHSV[y+0][x+0][0]-tempH)<5)&&(abs(dataHSV[y+0][x+0][2]-tempV)<30)&&(abs(dataHSV[y+0][x+0][1]-tempS)<40))
        if (area_grow_dataA[y + 1][x + 1] == (i - 1))
        {
            y = y + 1; // 当前次生长的一圈的纪录
            x = x + 1; // 当前次生长的一圈的纪录
            continue;
        }
        if (area_grow_dataA[y + 1][x - 1] == (i - 1))
        {
            y = y + 1; // 当前次生长的一圈的纪录
            x = x - 1; // 当前次生长的一圈的纪录
            continue;
        }
        if (area_grow_dataA[y - 1][x - 1] == (i - 1))
        {
            y = y - 1; // 当前次生长的一圈的纪录
            x = x - 1; // 当前次生长的一圈的纪录
            continue;
        }
        if (area_grow_dataA[y - 1][x + 1] == (i - 1))
        {
            y = y - 1; // 当前次生长的一圈的纪录
            x = x + 1; // 当前次生长的一圈的纪录
            continue;
        }
    }
    area_grow_datao[starty][startx] = 1; //=i-1
    pathx[1] = startx;                   // 存储路径的 顺序坐标
    pathy[1] = starty;

    int ax, ay, k; // 线段起始点
    int bx, by;    // 线段终点
    int pathxII[1000];
    int pathyII[1000];
    const int nihebuchang = 5; // 用直线 拟合 路径点//步长3  j++  <=> j=j+1 说明以起点开始 后每个点都逐个进行连线段
                               // 测试是否有障碍   可以为j=j+2 j=j+3等 加大每次增长长度
    /*for (i=1;i<pathl+1;i++)
    {
        if ((BW_data_[pathy[i]-1][pathx[i]-1]==0)||
            (BW_data_[pathy[i]-1][pathx[i]+0]==0)||
            (BW_data_[pathy[i]-1][pathx[i]+1]==0)||
            (BW_data_[pathy[i]+0][pathx[i]-1]==0)||
            (BW_data_[pathy[i]+0][pathx[i]+1]==0)||
            (BW_data_[pathy[i]+1][pathx[i]-1]==0)||
            (BW_data_[pathy[i]+1][pathx[i]+0]==0)||
            (BW_data_[pathy[i]+1][pathx[i]+1]==0)
            )
            {	pathxII[0]=pathx[i];
                pathyII[0]=pathy[i];
                break;
            }
        pathxII[0]=pathx[i];
        pathyII[0]=pathy[i];
    }*/

    for (i = 1; i < pathl + 1; i++)
    {
        ax = pathx[i];
        ay = pathy[i];
        for (j = i + 1; j < pathl + 1; j = j + nihebuchang)
        {
            bx = pathx[j];
            by = pathy[j];
            if (abs(ax - bx) > abs(ay - by))
            { // 以x为自变量
                if (ax > bx)
                { // 从x 从bx 加到 ax
                    for (k = bx; k < ax; k++)
                    {
                        y = ay - (ay - by) * (ax - k) / (ax - bx);
                        if (BW_data_[y][k] == 0)
                        { // pathxII[0]=pathx[j-nihebuchang];
                            // pathyII[0]=pathy[j-nihebuchang];
                            pathxII[0] = k;
                            pathyII[0] = y;
                            goto path_generate;
                        }
                    }
                }
                else
                { // 从x 从ax 加到 bx
                    for (k = ax; k < bx; k++)
                    {
                        y = ay - (ay - by) * (ax - k) / (ax - bx);
                        if (BW_data_[y][k] == 0)
                        { // pathxII[0]=pathx[j-nihebuchang];
                            // pathyII[0]=pathy[j-nihebuchang];
                            pathxII[0] = k;
                            pathyII[0] = y;
                            goto path_generate;
                        }
                    }
                }
            }
            else
            { // 以y为自变量
                if (ay > by)
                { // 从x 从bx 加到 ax
                    for (k = by; k < ay; k++)
                    {
                        x = ax - (ax - bx) * (ay - k) / (ay - by);
                        if (BW_data_[k][x] == 0)
                        { // pathxII[0]=pathx[j-nihebuchang];
                            // pathyII[0]=pathy[j-nihebuchang];
                            pathxII[0] = x;
                            pathyII[0] = k;
                            goto path_generate;
                        }
                    }
                }
                else
                { // 从x 从ax 加到 bx
                    for (k = ay; k < by; k++)
                    {
                        x = ax - (ax - bx) * (ay - k) / (ay - by);
                        if (BW_data_[k][x] == 0)
                        { // pathxII[0]=pathx[j-nihebuchang];
                            // pathyII[0]=pathy[j-nihebuchang];
                            pathxII[0] = x;
                            pathyII[0] = k;
                            goto path_generate;
                        }
                    }
                }
            }
            pathxII[0] = pathx[j];
            pathyII[0] = pathy[j];
        }
    }

path_generate:

    if (pathxII[0] != endx || pathyII[0] != endy)
    {
        *xx = pathxII[0];
        *yy = pathyII[0];
    }
    else
    {
        *xx = 400;
        *yy = 400;
    }

    DrawFocus(pathxII[0], pathyII[0], &door1);

    i = 0;
    for (y = 0; y < y240; y++)
    {
        for (x = 0; x < x320; x++)
        { //	path_data_out[i]=area_grow_datao[y][x];
            //
            if (area_grow_datao[y][x] == 1)

                SetColor(x, y, door1);

            //
            i++;
        }
    }

    // path_data_out[ pathyII[0]   *320+pathxII[0]  ] = 1;
    // path_data_out[(pathyII[0]+1)*320+pathxII[0]+1] = 1;
    // path_data_out[(pathyII[0]+1)*320+pathxII[0]-1] = 1;
    // path_data_out[(pathyII[0]-1)*320+pathxII[0]+1] = 1;
    // path_data_out[(pathyII[0]-1)*320+pathxII[0]-1] = 1;

    return (path_result); // 成功返回
}

void UPRColor::yanmo()
{
    // 注 现在视野中心在 x122 y120
    // 一个测量障碍用扇形的掩膜%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    int r, du;       //,pian,dian_shu,diandian
    int shan[19200]; // 单个扇形暂时存储区
    // int obstacle_shan_xing[36][1360];//扇形一 每个扇形10度 一个扇形1600个点 1600 = (240*240)/36
    // int obstacle_shan_xingx[36][1360];//扇形一 每个扇形10度 一个扇形1600个点 1600 = (240*240)/36
    // int obstacle_shan_xingy[36][1360];//扇形一 每个扇形10度 一个扇形1600个点 1600 = (240*240)/36
    // 36代表扇形号,1360代表该扇形的某点
    int x;
    int y;
    const int sacenter_x = 160;   // 扇中心X
    const int sacenter_y = 120;   // 扇中心Y
    const int saradius_min = 20;  // 扇半径最小长度
    const int saradius_max = 120; // 扇半径最大长度
    const int sageshu = 36;       // 扇个数  w              360/sageshu/0.25

    for (int j = 0; j < sageshu; j++) // 存储空间初始化 0
    {
        for (int i = 0; i < 6000; i++)
        { //	obstacle_shan_xing[j][i]=0;
            obstacle_shan_xingx[j][i] = 0;
            obstacle_shan_xingy[j][i] = 0;
        }
    }

    for (int k = 0; k < sageshu; k++) // 总循环 36个扇型
    {                                 //?????????????????????
        for (j = 0; j < 19200; j++)   // 初始化 0.25度一条射线 10度*4=40 条线 每条线120点 40*120=4800点
        {
            shan[j] = 0; // 存储记录每个点的 地址（数组偏移量 编号）
        }
        for (j = 0; j < 307200; j++) // 初始化
        {
            C1_Bool_Data[j] = 0;
        }

        for (du = 0; du < 360 / sageshu / 0.25; du++) // for(du=0;du<10*4;du++)
        {
            for (r = saradius_min; r < saradius_max; r++) // 半径40以下是机体不算
            {
                x = (int)(sin((du + k * 40) * 3.14 / (180 * 4)) * r) + sacenter_x;
                y = (int)(cos((du + k * 40) * 3.14 / (180 * 4)) * r) + sacenter_y;
                shan[du * 120 + r] = 320 * y + x;
                // y=(sin(2))*r+120;
                // x=(cos(2))*r+160;
                // z=(320*y)+x;
                //	 str.Format("y=%f x=%f z=%f",y,x,z);
                //	 AfxMessageBox(str);
                // du_r[(int)z]=1;
            }
        }

        for (j = 0; j < 19200; j++) // 将当前扇型倒入 图像使 重复点重合掉
        {
            C1_Bool_Data[shan[j]] = 1;
        }
        for (int i = 0, j = 0; j < 307200;
             j++) // 求出一个小扇形像素 按横扫描线存入obstacle_shan_xing  防止有重复点（消除3000多个重复点）
        {
            if (C1_Bool_Data[j] == 1)
            { //	obstacle_shan_xing[k][i]=j;
                obstacle_shan_xingx[k][i] = j % 640;
                obstacle_shan_xingy[k][i] = j / 640;
                i++;
            }
        }
    }
}

void UPRColor::xianshi()
{
    RGBTRIPLE ball1;
    ball1.rgbtBlue = 0x00;
    ball1.rgbtGreen = 0x00;
    ball1.rgbtRed = 0xFF;
    RGBTRIPLE door1;
    door1.rgbtBlue = 0xFF;
    door1.rgbtGreen = 0xFF;
    door1.rgbtRed = 0xFF;
    RGBTRIPLE door2;
    door2.rgbtBlue = 0x00;
    door2.rgbtGreen = 0xFF;
    door2.rgbtRed = 0x00;
    RGBTRIPLE door3;
    door3.rgbtBlue = 0x00;
    door3.rgbtGreen = 0x00;
    door3.rgbtRed = 0x00;
    for (int i = 0; i < 320; i++)
        for (int j = 0; j < 240; j++)
        {
            if (erzhi_b[i][j] == 1)
                SetColor(i, j, ball1);
            //		if(erzhi_d[i][j]==1)
            //			SetColor(i,j,door1);
            //		if(erzhi_j[i][j]==1)
            //			SetColor(i,j,door2);
            //		if(erzhi_black[i][j]==0)
            //			SetColor(i,j,door3);
        }
    // 	for(i=0;i<240;i++)
    // 	{
    // 		SetColor(i,quxian[i]/5,door1);
    // 	}
}

void UPRColor::guibi()
{
    RGBTRIPLE ball1;
    ball1.rgbtBlue = 0xFF;
    ball1.rgbtGreen = 0xFF;
    ball1.rgbtRed = 0xFF;
    for (int x2 = -72; x2 < 72; x2++)
        for (int y2 = 0; y2 < 240; y2++)
        {
            //	if (erzhi_black[shexianx[x2+72][y2]][shexiany[x2+72][y2]]==1)
            //		SetColor(shexianx[x2+72][y2],shexiany[x2+72][y2],ball1);
        }
}

void UPRColor::xianshiweitu()
{

    RGBTRIPLE ball1;
    ball1.rgbtBlue = 0xFF;
    ball1.rgbtGreen = 0xFF;
    ball1.rgbtRed = 0xFF;

    RGBTRIPLE door1;
    door1.rgbtBlue = 0xFF;
    door1.rgbtGreen = 0x00;
    door1.rgbtRed = 0x00;
    int i;
    int j;
    /*
    for( i=80;i<560;i++)         //显示深度图
        for(j=0;j<480;j++)
        {
            ball1.rgbtBlue = 255-shendubiao[i][j]*2;	ball1.rgbtGreen = 255-shendubiao[i][j]*2; ball1.rgbtRed =
    255-shendubiao[i][j]*2; SetColor(i,j,ball1);
        }
    */
    // 标定曲率

    for (j = 0; j < 240; j += 10)
    {

        for (i = 310; i < 330; i++)
        {
            SetColor(i, j + 240, ball1);
        }
    }
    for (j = 5; j < 240; j += 10)
    {

        for (i = 315; i < 325; i++)
        {
            SetColor(i, j + 240, ball1);
        }
    }
    for (j = 0; j < 240; j += 50)
    {

        for (i = 300; i < 340; i++)
        {
            SetColor(i, j + 240, ball1);
        }
    }

    for (i = 150; i < 530; i++)
        for (j = 100; j < 380; j++)
        {
            if (weitu[i][j] == 0)
            {
            }
            // SetColor(i,j,ball1);
            else
                SetColor(i, j, door1);
        }
    double jiao;
    double chang;
    double x;
    double y;
    for (i = 0; i < 230; i++)
    {
        zhizuobiao(&x, &y, -60, i, 320, 240);
        SetColor(x, y, ball1);
        zhizuobiao(&x, &y, 60, i, 320, 240);
        SetColor(x, y, ball1);
        zhizuobiao(&x, &y, -180, i, 320, 240);
        SetColor(x, y, ball1);
    }
    for (x = -30; x <= 30; x++)
        for (y = -30; y <= 30; y++)
            if (x * x + y * y <= 440 && x * x + y * y >= 400)
                SetColor(x + 320, y + 240, ball1);
    //	RGBTRIPLE ball1;
    //	ball1.rgbtBlue = 0xFF;	ball1.rgbtGreen = 0xFF; ball1.rgbtRed = 0xFF;
}

void UPRColor::getchangxian()
{
    //	for()
    xiandianshu = 1;
    RGBTRIPLE ball1;
    ball1.rgbtBlue = 0x00;
    ball1.rgbtGreen = 0x90;
    ball1.rgbtRed = 0xFF;
    double jiao1;
    double chang1;
    double x1;
    double y1;
    /*
    for(int  i=0;i<320;i++)
        for(int j=0;j<240;j++)
        {
            if (hsi_s[i][j]<20  && hsi_i[i][j]>200) //
            {
                changxianx[xiandianshu]=i/4;
                changxiany[xiandianshu]=j/4;
                    xiandianshu++;
            }
        }
*/
    int zdianshu = 500;
    int baiseshu;
    baiseshu = 0;
    int lv1;
    int bai1;
    int xx[640];
    int yy[640];
    int xxh;
    int yyh;
    int nn;
    int xundian;
    int x;

    for (x = 0; x < 144; x += 2)
    {
        xxh = 0;
        yyh = 0;
        lv1 = 0;
        bai1 = 0;
        baiseshu = 0;
        nn = 0;
        for (int y = 30; y <= 220; y++)
        {
            if (erzhi_di[shexianx[x][y]][shexiany[x][y]] == 1)
            {

                if (lv1 == 0)
                    if (erzhi_yuandi[shexianx[x][y]][shexiany[x][y]] == 1)
                        //	SetColor(huanyuanx[shexianx[x][y]][shexiany[x][y]],huanyuany[shexianx[x][y]][shexiany[x][y]],ball1);
                        // 如果射线是绿色
                        lv1 = 1;
                if (lv1 == 1)
                    if (erzhi_bai[shexianx[x][y]][shexiany[x][y]] == 1)
                    {
                        // SetColor(shexianx[x][y],shexiany[x][y],ball1);
                        // 如果射线是绿色后又是白色
                        bai1 = 1;
                        baiseshu++;

                        xx[nn] = x;
                        yy[nn] = y;
                        nn++;
                    }
                if (bai1 == 1 && baiseshu < 20)
                    if (erzhi_yuandi[shexianx[x][y]][shexiany[x][y]] == 1)
                    {
                        //	SetColor(huanyuanx[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,huanyuany[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,ball1);
                        lv1 = 0;
                        bai1 = 0;
                        //	changxianx[xiandianshu]=huanyuanx[shexianx[x][y-baiseshu/2]][shexiany[x][y-baiseshu/2]];
                        //	changxiany[xiandianshu]=huanyuany[shexianx[x][y-baiseshu/2]][shexiany[x][y-baiseshu/2]];
                        for (xundian = 0; xundian < nn; xundian++)
                        {
                            xxh = xxh + xx[xundian];
                            yyh = yyh + yy[xundian];
                        }
                        xxh = xxh / nn;
                        yyh = yyh / nn;
                        xiandianshu++;
                        baiseshu = 0;
                        jizuobiao(shexianx[xxh][yyh], shexiany[xxh][yyh], &jiao1, &chang1, 320, 240);
                        chang1 = double(quxian[int(chang1)]) * 0.49 / cdbl;
                        zhizuobiao(&x1, &y1, jiao1, chang1, 320, 240);
                        changxianx[xiandianshu] = float(x1);
                        changxiany[xiandianshu] = float(y1);
                        //						changxianx[xiandianshu]=huanyuanx[shexianx[xxh][yyh]][shexiany[xxh][yyh]];
                        //						changxiany[xiandianshu]=huanyuany[shexianx[xxh][yyh]][shexiany[xxh][yyh]];
                        // 						SetColor(shexianx[xxh][yyh],shexiany[xxh][yyh],ball1);
                        // 						SetColor(shexianx[xxh][yyh]+1,shexiany[xxh][yyh],ball1);
                        // 						SetColor(shexianx[xxh][yyh],shexiany[xxh][yyh]+1,ball1);
                        // 						SetColor(shexianx[xxh][yyh]+1,shexiany[xxh][yyh]+1,ball1);
                        lv1 = 0;
                        bai1 = 0;
                        nn = 0;
                        xxh = 0;
                        yyh = 0;
                    }
            }
        }
    }

    //******************************************************************

    for (x = 100; x < 540; x += 20)
    {
        xxh = 0;
        yyh = 0;
        nn = 0;
        lv1 = 0;
        bai1 = 0;
        baiseshu = 0;
        for (int y = 20; y <= 460; y++)
        {
            if (banjing[x][y] < 40000)
            {
                if (erzhi_di[x][y] == 1)
                {

                    if (lv1 == 0)
                        if (erzhi_yuandi[x][y] == 1)
                            //	SetColor(huanyuanx[x][y],huanyuany[x][y],ball1);
                            // 如果射线是绿色
                            lv1 = 1;
                    if (lv1 == 1)
                    {
                        if (erzhi_bai[x][y] == 1)
                        {
                            //	SetColor(x,y,ball1);
                            // 如果射线是绿色后又是白色
                            bai1 = 1;
                            baiseshu++;

                            xx[nn] = x;
                            yy[nn] = y;
                            nn++;
                        }
                    }
                    if (bai1 == 1 && baiseshu < 20)
                        if (erzhi_yuandi[x][y] == 1)
                        {

                            // SetColor(huanyuanx[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,huanyuany[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,ball1);

                            for (xundian = 0; xundian < nn; xundian++)
                            {
                                xxh = xxh + xx[xundian];
                                yyh = yyh + yy[xundian];
                            }
                            xxh = xxh / nn;
                            yyh = yyh / nn;

                            //
                            // 					SetColor(xxh,yyh,ball1);
                            // 					SetColor(xxh+1,yyh,ball1);
                            // 					SetColor(xxh,yyh+1,ball1);
                            // 					SetColor(xxh+1,yyh+1,ball1);

                            lv1 = 0;
                            bai1 = 0;
                            xiandianshu++;
                            jizuobiao(xxh, yyh, &jiao1, &chang1, 320, 240);
                            chang1 = double(quxian[int(chang1)]) * 0.49 / cdbl;
                            zhizuobiao(&x1, &y1, jiao1, chang1, 320, 240);
                            changxianx[xiandianshu] = float(x1);
                            changxiany[xiandianshu] = float(y1);
                            //	changxianx[xiandianshu]=huanyuanx[xxh][yyh];
                            //	changxiany[xiandianshu]=huanyuany[xxh][yyh];

                            baiseshu = 0;
                            nn = 0;
                            xxh = 0;
                            yyh = 0;
                        }
                }
            }
        }
    }

    //******************************************************************

    for (int y = 0; y < 480; y += 20)
    {
        xxh = 0;
        yyh = 0;
        nn = 0;
        lv1 = 0;
        bai1 = 0;
        baiseshu = 0;
        for (int x = 80; x <= 560; x++)
        {
            if (banjing[x][y] < 40000)
            {
                if (erzhi_di[x][y] == 1)
                {

                    if (lv1 == 0)
                        if (erzhi_yuandi[x][y] == 1)
                            //	SetColor(huanyuanx[x][y],huanyuany[x][y],ball1);
                            // 如果射线是绿色
                            lv1 = 1;
                    if (lv1 == 1)
                    {
                        if (erzhi_bai[x][y] == 1)
                        {
                            // SetColor(x,y,ball1);
                            // 如果射线是绿色后又是白色
                            bai1 = 1;
                            baiseshu++;

                            xx[nn] = x;
                            yy[nn] = y;
                            nn++;
                        }
                    }
                    if (bai1 == 1 && baiseshu < 20)
                        if (erzhi_yuandi[x][y] == 1)
                        {

                            for (xundian = 0; xundian < nn; xundian++)
                            {
                                xxh = xxh + xx[xundian];
                                yyh = yyh + yy[xundian];
                            }
                            xxh = xxh / nn;
                            yyh = yyh / nn;
                            //
                            // 					SetColor(xxh,yyh,ball1);
                            // 					SetColor(xxh+1,yyh,ball1);
                            // 					SetColor(xxh,yyh+1,ball1);
                            // 					SetColor(xxh+1,yyh+1,ball1);
                            //	SetColor(huanyuanx[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,huanyuany[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,ball1);
                            lv1 = 0;
                            bai1 = 0;
                            jizuobiao(xxh, yyh, &jiao1, &chang1, 320, 240);
                            chang1 = double(quxian[int(chang1)]) * 0.49 / cdbl;
                            zhizuobiao(&x1, &y1, jiao1, chang1, 320, 240);
                            changxianx[xiandianshu] = float(x1);
                            changxiany[xiandianshu] = float(y1);
                            //	changxianx[xiandianshu]=huanyuanx[xxh][yyh];
                            //	changxiany[xiandianshu]=huanyuany[xxh][yyh];
                            xiandianshu++;
                            baiseshu = 0;
                            nn = 0;
                            xxh = 0;
                            yyh = 0;
                        }
                }
            }
        }
    }
    float zjl;
    if (xiandianshu > zdianshu)
    {
        zjl = xiandianshu / zdianshu;
        for (x = 0; x <= zdianshu; x++)
        {
            changxianx[x] = changxianx[int(x * zjl)];
            changxiany[x] = changxiany[int(x * zjl)];
            SetColor(changxianx[int(x * zjl)], changxiany[int(x * zjl)], ball1);
            SetColor(changxianx[int(x * zjl)] + 1, changxiany[int(x * zjl)], ball1);
            SetColor(changxianx[int(x * zjl)], changxiany[int(x * zjl)] + 1, ball1);
            SetColor(changxianx[int(x * zjl)] - 1, changxiany[int(x * zjl)], ball1);
            SetColor(changxianx[int(x * zjl)], changxiany[int(x * zjl)] - 1, ball1);
        }
        xiandianshu = zdianshu;
    }

    //******************************************************************

    // changxian[i/5][j/5]=1;
    //	SetColor(i/4,j/4,ball1);
    //	}
    //	else
    //	changxian[i][j]=0;
    /*
double jiao;
double chang;
    double xuanzhuan=100;
        for( i=1;i<=xiandianshu;i++)
{
jizuobiao(changxianx[i],changxiany[i],&jiao,&chang,40,30);
jiao+=xuanzhuan;
zhizuobiao(&changxianx[i],&changxiany[i],jiao,chang,40,30);
}
*/
    //	for(int i=1;i<=xiandianshu;i++)
    // SetColor(changxianx[i]+30,changxiany[i]+30,ball1);
}

double UPRColor::pipei(Yuzhi *changdi)
{
    RGBTRIPLE ball1;
    ball1.rgbtBlue = 0xFF;
    ball1.rgbtGreen = 0x00;
    ball1.rgbtRed = 0xFF;
    RGBTRIPLE ball;
    ball.rgbtBlue = 0xFF;
    ball.rgbtGreen = 0xFF;
    ball.rgbtRed = 0xFF;

    RGBTRIPLE shizi;
    shizi.rgbtBlue = 0x00;
    shizi.rgbtGreen = 0x00;
    shizi.rgbtRed = 0xFF;
    int zuixiaox; // 和匹配图距离差距最小的，x需要的移动量
    int zuixiaoy;
    int zuixiaox1;
    int zuixiaoy1;
    int zuixiaojiao;
    int changdu;
    double jiao;
    double chang;
    double zuixiaojuli;
    zuixiaojuli = 1000000;
    double xuanzhuan;
    double xzhongjian;
    double yzhongjian;
    int i;
    int m;
    double changxianx1[20000];
    double changxiany1[20000];
    double juli;
    int x;
    int y;
    double out_x;
    double out_y;
    int qishipingyi_x; //
    int qishipingyi_y; //
    juli = 0;
    int dianshu;
    //	dianshu=0;
    pipeijishu++;
    if (((qishix == 0 && qishiy == 0)) && changdi->ddoor_zhong > 100 && changdi->jdoor_zhong > 100)
    {
        pipeijishu = 0;
        qishix = int((pizhi->benjix - 900) / 6);
        qishiy = int((600 - pizhi->benjiy) / 6);
        qishijiao = int(pizhi->benjijiao + 90);
    }
    qishix = int((changdi->benjix - 900) / 6);
    qishiy = int((600 - changdi->benjiy) / 6);
    qishijiao = int(changdi->benjijiao + 90);
    if (dianjipanding > 0)
    {
        dianjipanding--;
        qishix = (dianjix - 900) / 6;
        qishiy = (600 - dianjiy) / 6;
        xbshang = qishix;
        ybshang = qishiy;
        qishijiao = int(dianjijiao + 90);
    }
    qishipingyi_x = qishix; //
    qishipingyi_y = qishiy; //
    if (luopanqingchu == 1)
    {
        qishijiao = int(luopanjiao);
        luopanqingchu = 0;
    }

    xuanzhuan = 0;

    /*
                qishix=int((pizhi->benjix-900)/6);
            qishiy=int((600-pizhi->benjiy)/6);
            qishijiao=int(pizhi->benjijiao+90);
            */
    /*
        for ( xuanzhuan=0;xuanzhuan<180;xuanzhuan+=180)
        {
            jiao=0;
            for( i=1;i<=xiandianshu;i++)
            {
                changxianx1[i]=changxianx[i];
                changxiany1[i]=changxiany[i];
            //	SetColor(changxianx[i]-1,changxiany[i]-1,ball1);
            //	SetColor(changxianx[i]-1,changxiany[i]+1,ball1);
            //	SetColor(changxianx[i]-1,changxiany[i],ball1);
            //	SetColor(changxianx[i],changxiany[i]-1,ball1);
            //	SetColor(changxianx[i],changxiany[i]+1,ball1);
            //	SetColor(changxianx[i],changxiany[i],ball1);
            //	SetColor(changxianx[i]+1,changxiany[i]-1,ball1);
            //	SetColor(changxianx[i]+1,changxiany[i]+1,ball1);
            //	SetColor(changxianx[i]+1,changxiany[i],ball1);
                jizuobiao(changxianx1[i],changxiany1[i],&jiao,&chang,320,240);
                jiao+=xuanzhuan;
                zhizuobiao(&changxianx1[i],&changxiany1[i],jiao,chang,320,240);
            }
        }
        */
    //	if(qishix==0 && qishiy==0 )
    if (false)
    {
        for (i = 1; i <= xiandianshu; i++)
        {
            changxianx1[i] = changxianx[i];
            changxiany1[i] = changxiany[i];
        }
        for (x = -150; x <= 150; x = x + 2)
            for (y = -120; y <= 120; y = y + 2)
            {
                juli = 0;
                dianshu = 0;
                for (i = 1; i <= xiandianshu; i++)
                {
                    juli += shendubiao[int(changxianx1[i]) + x][int(changxiany1[i]) + y];
                }
                juli = juli / xiandianshu;
                if (juli < zuixiaojuli)
                {
                    zuixiaojuli = juli;
                    zuixiaox1 = x;
                    zuixiaoy1 = y;
                }
            }
        qishix = zuixiaox1;
        qishiy = zuixiaoy1;
        qishijiao = 0;
    }

    else
    {
        for (m = 0; m < 2; m++)
        {
            //	zuixiaojuli=10000;
            for (xuanzhuan = qishijiao - 5; xuanzhuan <= qishijiao + 5; xuanzhuan++)
            {
                jiao = 0;
                for (i = 1; i <= xiandianshu; i++)
                {
                    jizuobiao(changxianx[i], changxiany[i], &jiao, &chang, 320, 240);
                    jiao += xuanzhuan;
                    zhizuobiao(&changxianx1[i], &changxiany1[i], jiao, chang, 320, 240);
                }
                qishipingyi_x = qishix; //
                qishipingyi_y = qishiy; //
                for (int n = 0; n < 10; n++)
                {
                    for (x = qishipingyi_x - 1; x <= qishipingyi_x + 1; x++)
                    {
                        for (y = qishipingyi_y - 1; y <= qishipingyi_y + 1; y++)
                        {
                            juli = 0;
                            dianshu = 0;
                            for (i = 1; i <= xiandianshu; i++)
                            {
                                changdu = shendubiao[int(changxianx1[i] + x)][int(changxiany1[i] + y)];
                                if (changdu < 40)
                                    juli += changdu; // shendubiao[int(changxianx1[i]+x)][int(changxiany1[i]+y)];
                                else
                                    juli += 40;
                            }
                            juli = juli / xiandianshu;
                            if (juli < zuixiaojuli)
                            {
                                zuixiaojuli = juli;
                                zuixiaox1 = x;
                                zuixiaoy1 = y;
                                //		qishix=x;
                                //		qishiy=
                                zuixiaojiao = int(xuanzhuan);
                                if (zuixiaox < -240)
                                    zuixiaox = -240;
                                if (zuixiaox > 240)
                                    zuixiaox = 240;
                                if (zuixiaoy < -240)
                                    zuixiaoy = -240;
                                if (zuixiaoy > 240)
                                    zuixiaoy = 240;
                            }
                        }
                        if (qishipingyi_x > xbshang + 5)
                        {
                            zuixiaox1 = xbshang + 5;
                            qishipingyi_x = xbshang + 5;
                        }
                        if (qishipingyi_x < xbshang - 5)
                        {
                            zuixiaox1 = xbshang - 5;
                            qishipingyi_x = xbshang - 5;
                        }
                        if (qishipingyi_y > ybshang + 5)
                        {
                            zuixiaoy1 = ybshang + 5;
                            qishipingyi_y = ybshang + 5;
                        }
                        if (qishipingyi_y < ybshang - 5)
                        {
                            zuixiaoy1 = ybshang - 5;
                            qishipingyi_y = ybshang - 5;
                        }
                    }

                    if (qishipingyi_x == zuixiaox1 && qishipingyi_y == zuixiaoy1)
                    {
                        //	x=qishipingyi_x+3;
                        break;
                    }
                    else
                    {
                        qishipingyi_x = zuixiaox1;
                        qishipingyi_y = zuixiaoy1;
                    }
                }
            }

            if (qishijiao == zuixiaojiao)
            {
                qishijiao = zuixiaojiao;
                break;
            }
            else
            {
                qishijiao = zuixiaojiao;
            }
        }
    }
    qishix = zuixiaox1; //
    qishiy = zuixiaoy1; //
    for (i = 1; i <= xiandianshu; i++)
    {
        jizuobiao(changxianx[i], changxiany[i], &jiao, &chang, 320, 240);
        jiao += zuixiaojiao;
        zhizuobiao(&changxianx1[i], &changxiany1[i], jiao, chang, 320, 240);
    }
    out_x = zuixiaox;
    out_y = zuixiaoy;
    jizuobiao(out_x, out_y, &jiao, &chang, 320, 240);
    jiao -= qishijiao;
    zhizuobiao(&out_x, &out_y, jiao, chang, 320, 240);
    benjijiao = -qishijiao;
    /*
        for( x=zuixiaox-10;x<=zuixiaox+10;x++)
            for(int y=zuixiaoy-10;y<=zuixiaoy+10;y++)
            {
                juli=0;
                dianshu=0;
                    for(int i=1;i<=xiandianshu;i++)
                    {
                        juli+=shendubiao[changxianx[i]+x][changxiany[i]+y];
                    }
                    juli=juli/xiandianshu;
                    if(juli<=zuixiaojuli)
                    {
                        zuixiaojuli=juli;
                        zuixiaox1=x;
                        zuixiaoy1=y;
                    }
            }

    */
    //	}
    /*
        if (zuixiaox2>zuixiaox1+50)
            zuixiaox2+=50;
        else if (zuixiaox2<zuixiaox1-50)
            zuixiaox2-=50;
        else
            zuixiaox2=zuixiaox1;

            if (zuixiaoy2>zuixiaoy1+50)
            zuixiaoy2+=50;
        else if (zuixiaoy2<zuixiaoy1-50)
            zuixiaoy2-=50;
        else
            zuixiaoy2=zuixiaoy1;
    */

    //	CString str;
    //	str.Format("x=%d,y=%d,jiao=%f",zuixiaox1,zuixiaoy1,pizhi->benjix);
    //	AfxMessageBox(str);
    xbshang = zuixiaox1;
    ybshang = zuixiaoy1;
    for (i = 1; i <= xiandianshu; i++)
    {
        SetColor(zuixiaox1 + changxianx1[i], zuixiaoy1 + changxiany1[i], ball1);
        SetColor(zuixiaox1 + changxianx1[i] - 1, zuixiaoy1 + changxiany1[i] - 1, ball1);
        SetColor(zuixiaox1 + changxianx1[i] - 1, zuixiaoy1 + changxiany1[i], ball1);
        SetColor(zuixiaox1 + changxianx1[i] - 1, zuixiaoy1 + changxiany1[i] + 1, ball1);
        SetColor(zuixiaox1 + changxianx1[i], zuixiaoy1 + changxiany1[i] - 1, ball1);
        SetColor(zuixiaox1 + changxianx1[i], zuixiaoy1 + changxiany1[i] + 1, ball1);
        SetColor(zuixiaox1 + changxianx1[i] + 1, zuixiaoy1 + changxiany1[i] - 1, ball1);
        SetColor(zuixiaox1 + changxianx1[i] + 1, zuixiaoy1 + changxiany1[i], ball1);
        SetColor(zuixiaox1 + changxianx1[i] + 1, zuixiaoy1 + changxiany1[i] + 1, ball1);
    }
    /////////////////////////////////////////////////////曲率校准
    /*
    float neichang;
    float waichang;
    float shenduzhi;
    float shenduzhi1;
    double chang11;
    double x22,y22;
    float quxianchang;
    float quxianchang1;
    double aa;
    double bb;
    double cc;
    double dd;
    for(i=1;i<=xiandianshu;i++)
    {
        shenduzhi=shendubiao[int(changxianx1[i])+zuixiaox1][int(changxiany1[i])+zuixiaoy1];
        if(shenduzhi<5 && shendubiao[int(changxianx1[i])+zuixiaox1][int(changxiany1[i])+zuixiaoy1]!=0)
        {
            jizuobiao(changxianx1[i]+zuixiaox1,changxiany1[i]+zuixiaoy1,&jiao,&chang,zuixiaox1+320,zuixiaoy1+240);
            chang11=chang-1;
            zhizuobiao(&x22,&y22,jiao,chang11,zuixiaox1+320,zuixiaoy1+240);

        shenduzhi1=shendubiao[int(x22)][int(y22)];
        quxianchang=quxian[int(fanquxian[int(double(chang*cdbl/0.49))])];
        quxianchang1=quxian11[int(fanquxian[int(double(chang*cdbl/0.49))])];

        if(shenduzhi-shenduzhi1>1 &&  quxianchang/quxianchang1>0.6)
        {
            SetColor(x22,y22,shizi);
            SetColor(x22-1,y22,shizi);
            SetColor(x22,y22-1,shizi);
            SetColor(x22-1,y22-1,shizi);
            quxian[int(fanquxian[int(chang*cdbl/0.49)])]-=(shenduzhi-shenduzhi1)/10;
            quxian[int(fanquxian[int(chang*cdbl/0.49)])+1]-=(shenduzhi-shenduzhi1)/20;
            quxian[int(fanquxian[int(chang*cdbl/0.49)])-1]-=(shenduzhi-shenduzhi1)/20;
            quxian[int(fanquxian[int(chang*cdbl/0.49)])+2]-=(shenduzhi-shenduzhi1)/40;
            quxian[int(fanquxian[int(chang*cdbl/0.49)])-2]-=(shenduzhi-shenduzhi1)/40;
            aa=chang*cdbl/0.49;
            bb=fanquxian[int(chang*cdbl/0.49)];
            cc=int(fanquxian[int(chang*cdbl/0.49)]);
            dd=quxian[int(fanquxian[int(chang*cdbl/0.49)])];
        }
        if(shenduzhi-shenduzhi1<1 && quxianchang/quxianchang1<1.3)
        {
            SetColor(x22,y22,ball);
            SetColor(x22-1,y22,ball);
            SetColor(x22,y22-1,ball);
            SetColor(x22-1,y22-1,ball);
            quxian[int(fanquxian[int(chang*cdbl/0.49)])]+=(-shenduzhi+shenduzhi1)/10;
            quxian[int(fanquxian[int(chang*cdbl/0.49)])-1]+=(-shenduzhi+shenduzhi1)/20;
            quxian[int(fanquxian[int(chang*cdbl/0.49)])+1]+=(-shenduzhi+shenduzhi1)/20;
            quxian[int(fanquxian[int(chang*cdbl/0.49)])-2]+=(-shenduzhi+shenduzhi1)/40;
            quxian[int(fanquxian[int(chang*cdbl/0.49)])+2]+=(-shenduzhi+shenduzhi1)/40;
        }
        }
    }
    int j;
    for(i=1;i<240;i++)
{
    for(j=quxian[i-1];j<quxian[i];j++)
        fanquxian[j]=i;
}

        for(i=0;i<240;i++)
        {
            SetColor(i,int(quxian[i]/2+1),shizi);
            SetColor(i,int(quxian[i]/2),shizi);
            SetColor(i,int(quxian[i]/2-1),shizi);
            SetColor(i,int(quxian11[i]/2+1),ball1);
            SetColor(i,int(quxian11[i]/2),ball1);
            SetColor(i,int(quxian11[i]/2-1),ball1);
        }
    */
    /////////////////////////////////////////////////////曲率校准完毕
    DrawFocus(zuixiaox1 + 320, zuixiaoy1 + 240, &shizi);
    jizuobiao(76, 104, &menjiao, &chang, zuixiaox1 + 320, zuixiaoy1 + 240);

    //	pField->SetAgent(1,900+zuixiaox1*6,600-zuixiaoy1*6,zuixiaojiao-90);

    /// 位置估测
    /*
    double xx2;
    double yy2;
//	jizuobiao(net.m_ctrl.target_x,net.m_ctrl.target_y,&dingweijiao,&dingweichang,y.benjix,y.benjiy);
    if(pipeikaishi%200==0)
    {
        pipeikaishi++;
    zhizuobiao(&xx2,&yy2,ycjiao+ycj,ycchang,900+zuixiaox1*6,600-zuixiaoy1*6);
    pField->SetAgent(3,xx2,yy2,zuixiaojiao-90+yczhuan);
    ycx=xx2;
    ycy=yy2;
    ycj=zuixiaojiao-90+yczhuan;
    }
    else if(pipeikaishi<100)
    {
        pipeikaishi++;
    zhizuobiao(&xx2,&yy2,ycjiao+ycj,ycchang,900+zuixiaox1*6,600-zuixiaoy1*6);
    pField->SetAgent(3,xx2,yy2,zuixiaojiao-90+yczhuan);
    ycx=xx2;
    ycy=yy2;
    ycj=zuixiaojiao-90+yczhuan;
    }
    else
    {
        pipeikaishi++;
            zhizuobiao(&xx2,&yy2,-zuixiaojiao+180,ycchang,ycx,ycy);
            zhizuobiao(&xx2,&yy2,-(ycj+yczhuan)+90,ycchang,ycx,ycy);
            pField->SetAgent(1,xx2,yy2,ycj+yczhuan);
//			pField->SetAgent(3,xx2,yy2,ycj+yczhuan);
            ycx=xx2;
            ycy=yy2;
            ycj=ycj+yczhuan;
    }
    zhizuobiao(&xx2,&yy2,-(ycj+yczhuan)+90,ycchang,900+zuixiaox1*6,600-zuixiaoy1*6);

    qishix=(xx2-900)/6; //
    qishiy=(600-yy2)/6; //
    qishijiao=qishijiao+yczhuan;
    */
    // 位置估测完毕
    pField->SetAgent(2, dianjix, dianjiy, dianjijiao);
    changdi->benjix = 900 + zuixiaox1 * 6;
    changdi->benjiy = 600 - zuixiaoy1 * 6;
    changdi->benjijiao = zuixiaojiao - 90;
    pField->SetAgent(3, changdi->benjix, changdi->benjiy, changdi->benjijiao);
    if (changdi->benjijiao > 360)
        changdi->benjijiao -= 360;
    if (changdi->benjijiao < -360)
        changdi->benjijiao += 360;

    double balljiao;
    double ballchang;
    double ballx;
    double bally;
    balljiao = -changdi->ball_jiao - changdi->benjijiao + 90;
    ballchang = quxian[int(changdi->ball_chang)] / cdbl * 2.7;
    zhizuobiao(&ballx, &bally, balljiao, ballchang, changdi->benjix, changdi->benjiy);

    pField->SetBall(2, ballx, bally);
    //	pField->SetBall(10,xianshix[1],xianshiy[1]);
    changdi->quanbx = ballx;
    changdi->quanby = bally;

    return zuixiaojuli;
}

void UPRColor::changdiyidong(int x, int y, double *jiao, double *chang)
{
    jizuobiao(x, y, jiao, chang, baoliux + 130, baoliuy + 95);
}

void UPRColor::pengzhang(int n)
{
    //	int x;
    //	int y;
    //	for(int i=83;i<557;i++)
    //		for(int j=3;j<477;j++)
    //			if(banjing[i][j]<10000)
    //			{
    //				if(erzhi_di[i][j]==1)
    //					for(x=i-3;x<=i+3;i++)
    //						for(y=j-3;y<=j+3;i++)
    //							erzhi_di[i][j]=1;
    //			}
}
int UPRColor::jiajiao(int jizhun, int jiao)
{
    double jiajiao;
    jiajiao = jiao - jizhun;
    if (jiajiao > 180)
        jiajiao = -360 + jiajiao;
    if (jiajiao < -180)
        jiajiao = 360 + jiajiao;
    return jiajiao;
}

int disAB[144][10];
// 注意 不用 每次计算 叉子到所有点的距离 只用计算到学习更新过的 叉子的距离就可  所有两两距离存储入m*n表里 （m学习点数
// n聚类数） 可以用 存10个以上聚类 但只使用 点最多的10个
void UPRColor::getblackobj(float f_x, float f_y, float f_a)
{
    /*	for(k=0;k<in_obj_l;k++)//为了 加速可以跳过一半点
                ty=	in_obj_y[k]*zoomY+MAP_oY*zoomY;//输入生成花费地图用 障碍物数组 全局变量
                tx=	in_obj_x[k]*zoomX+MAP_oX*zoomX;
    */
    for (in_obj_l = 0; in_obj_l < in_obj_ll; in_obj_l++)
    {
        in_obj_x[in_obj_l] =
            l2f_x(f_x, f_y, f_a, in_obj_la[in_obj_l], -in_obj_lr[in_obj_l]); // 输入本机测量得到的障碍物数组 全局变量
        in_obj_y[in_obj_l] = l2f_y(f_x, f_y, f_a, in_obj_la[in_obj_l], -in_obj_lr[in_obj_l]);
        in_obj_w[in_obj_l] = 1;
    }
    float Learn_A_t, Learn_R_t;
    int L_temp, L_temp2;
    int L_loop = 100;
    L_temp2 = -Learn_num / 2;
    for (L_temp = 0; L_temp < Learn_num; L_temp++)
    {
        Learn_A[L_temp] = L_temp2 * 360 / Learn_num;
        Learn_R[L_temp] = 120;
        L_temp2++;
    }

    int Learn_loop = 100;
    float min_err, min_err_t;
    int min_err_num;
    for (L_temp2 = 0; L_temp2 < in_obj_ll; L_temp2++)
    {
        for (L_temp = 0; L_temp < Learn_num; L_temp++)
        {
            disAB[L_temp2][L_temp] =
                coordinate_transfer_radius(1 * angle_add_transfer(Learn_A[L_temp], -in_obj_la[L_temp2]),
                                           1.0 * Learn_R[L_temp] - in_obj_org_lr[L_temp2]);
        }
    }
    for (Learn_loop = 20; Learn_loop >= 0; Learn_loop--)
    {
        for (L_temp2 = 0; L_temp2 < Learn_num; L_temp2++)
        {
            Learn_Flag[L_temp2] = 0;
        }
        for (L_temp2 = 0; L_temp2 < in_obj_ll; L_temp2++)
        {
            min_err = 10000000;
            for (L_temp = 0; L_temp < Learn_num; L_temp++)
            {
                min_err_t = disAB
                    [L_temp2]
                    [L_temp]; // coordinate_transfer_radius(1*angle_add_transfer(Learn_A[L_temp],-in_obj_la[L_temp2])
                              //						 ,1.0*Learn_R[L_temp] -in_obj_org_lr[L_temp2]);
                if (min_err > min_err_t)
                {
                    min_err = min_err_t;
                    min_err_num = L_temp;
                }
            }
            Learn_Flag[min_err_num]++;
            // 学习算法
            Learn_A_t = angle_add_transfer(in_obj_la[L_temp2], -Learn_A[min_err_num]);
            Learn_R_t = in_obj_org_lr[L_temp2] - Learn_R[min_err_num];
            Learn_A[min_err_num] = angle_add_transfer(Learn_A[min_err_num], +Learn_A_t * 0.36);
            Learn_R[min_err_num] = Learn_R[min_err_num] + Learn_R_t * 0.36;
            disAB[L_temp2][L_temp] =
                coordinate_transfer_radius(1 * angle_add_transfer(Learn_A[L_temp], -in_obj_la[L_temp2]),
                                           1.0 * Learn_R[L_temp] - in_obj_org_lr[L_temp2]);
        }
    }

    /*	for (L_temp=0;L_temp<Learn_num;L_temp++)
        {	Learn_Flag[L_temp]=0;
        }
        for (L_temp2=0;L_temp2<in_obj_ll;L_temp2++)
        {	min_err=10000000;
            for (L_temp=0;L_temp<Learn_num;L_temp++)
            {	min_err_t=coordinate_transfer_radius(1*angle_add_transfer(Learn_A[L_temp],-in_obj_la[L_temp2])
                                                                         ,1.0*Learn_R[L_temp] -in_obj_org_lr[L_temp2]);
                if (min_err>min_err_t)
                {	min_err=min_err_t;
                            min_err_num=L_temp;
                }
            }
            Learn_Flag[min_err_num]++;
            if (min_err_num==6)
            {Learn_Flag[min_err_num];}
        }
    */
    RGBTRIPLE ball1;
    ball1.rgbtBlue = 0x00;
    ball1.rgbtGreen = 0xff;
    ball1.rgbtRed = 0xff;
    double jiaoxx;
    double changxx;
    for (L_temp = 0; L_temp < Learn_num; L_temp++)
    {
        if (Learn_Flag[L_temp] > 1) // 大于等于两个障碍射线 就计算在内
        {

            DrawFocus(coordinate_transfer_x(-Learn_A[L_temp] + 90, Learn_R[L_temp]) + 320,
                      coordinate_transfer_y(-Learn_A[L_temp] + 90, Learn_R[L_temp]) + 240, &ball1);
            jizuobiao(coordinate_transfer_x(-Learn_A[L_temp] + 90, Learn_R[L_temp]) + 320,
                      coordinate_transfer_y(-Learn_A[L_temp] + 90, Learn_R[L_temp]) + 240, &jiaoxx, &changxx, 320, 240);
            Learn_A[L_temp] = jiaoxx;
            Learn_R[L_temp] = changxx;
        }
    }
}
void UPRColor::getzhangaiwu2(int nei, int wai, int chang, int bian)
{
    if (wai > 239)
        wai = 239;
    RGBTRIPLE ball1;
    ball1.rgbtBlue = 0xAA;
    ball1.rgbtGreen = 0xAA;
    ball1.rgbtRed = 0xAA;
    int lianxuchang;
    int j = 0;
    int zuixiaojiajiao = 15;
    int jiao;
    int zuixiaoi = 15;
    int zuixiaoj = 0;
    int x;
    int y;
    int i;
    //	memset(erzhi_pz,0,640*480*sizeof(BOOL));
    int waichang;
    //	ball1.rgbtBlue = 0xAA;	ball1.rgbtGreen = 0xAA; ball1.rgbtRed = 0xAA;
    //	for(i=0;i<640;i++)
    //		for(j=0;j<480;j++)
    //			if(erzhi_black[i][j]==1)
    //			SetColor(i,j,ball1);
    ball1.rgbtBlue = 0xAA;
    ball1.rgbtGreen = 0xAA;
    ball1.rgbtRed = 00;
    in_obj_ll = 0;
    for (i = 0; i < 144; i++)
    {
        bishe[i] = 0;
        lianxuchang = 0;
        waichang = wai;
        if (bian == 1)
        {
            for (j = 1; j < 50; j++)
                if (erzhi_bian[shexianx[i][j]][shexiany[i][j]] == 1)
                {
                    bishe[i] = j + 30;
                }
        }
        for (j = nei; j <= waichang; j++)
        {
            if (erzhi_black[shexianx[i][j]][shexiany[i][j]] == 1) // && erzhi_di[shexianx[i][j]][shexiany[i][j]]==1)
            {
                lianxuchang++;
                if (j == waichang && j < 240)
                    waichang++;
            }
            else if (lianxuchang > 0)
                lianxuchang = 0;

            if (lianxuchang > chang)
            {

                bishe[i] = j - chang + 1;
                // #ifdef UPRColor_FUNCTION_DSPLAY//显示每个障碍的十字标示
                DrawFocus(shexianx[i][j - 10], shexiany[i][j - 10], &ball1);
                // #endif
                in_obj_org_lr[in_obj_ll] = bishe[i];                          // 存储无畸变的数据
                in_obj_la[in_obj_ll] = angle_add_transfer(0, -i * 360 / 144); // 输入本机测量得到的障碍物数组 全局变量
                in_obj_lr[in_obj_ll++] =
                    quxian[int(bishe[i])] / cdbl *
                    2.7; // quxian[bishe[i]];//quxian[i];////DrawFocus(shexianx[i][i],shexiany[i][i],&ball1);
                break;
            }
        }
    }
}
int UPRColor::getzhangaiwu(int nei, int wai, int chang, int mubiao, int fanwei, int bian, int *zuo, int *you)
{
    if (wai > 239)
        wai = 239;
    RGBTRIPLE ball1;
    ball1.rgbtBlue = 0xAA;
    ball1.rgbtGreen = 0xAA;
    ball1.rgbtRed = 0xAA;
    int lianxuchang;
    int j = 0;
    int zuixiaojiajiao = 15;
    int jiao;
    int zuixiaoi = 15;
    int zuixiaoj = 0;
    int x;
    int y;
    int i;
    //	memset(erzhi_pz,0,640*480*sizeof(BOOL));
    int waichang;
    //	ball1.rgbtBlue = 0xAA;	ball1.rgbtGreen = 0xAA; ball1.rgbtRed = 0xAA;
    //	for(i=0;i<640;i++)
    //		for(j=0;j<480;j++)
    //			if(erzhi_black[i][j]==1)
    //			SetColor(i,j,ball1);
    ball1.rgbtBlue = 0xAA;
    ball1.rgbtGreen = 0xAA;
    ball1.rgbtRed = 00;
    for (i = 0; i < 144; i++)
    {
        bishe[i] = 0;
        lianxuchang = 0;
        waichang = wai;
        if (bian == 1)
        {
            for (j = 1; j < 50; j++)
                if (erzhi_bian[shexianx[i][j]][shexiany[i][j]] == 1)
                {
                    bishe[i] = j + 30;
                }
        }
        for (j = nei; j <= waichang; j++)
        {
            if (erzhi_black[shexianx[i][j]][shexiany[i][j]] == 1 && erzhi_di[shexianx[i][j]][shexiany[i][j]] == 1)
            {
                lianxuchang++;
                if (j == waichang && j < 240)
                    waichang++;
            }
            else if (lianxuchang > 0)
                lianxuchang = 0;

            if (lianxuchang > chang)
            {

                bishe[i] = j - chang + 1;
                DrawFocus(shexianx[i][j - 10], shexiany[i][j - 10], &ball1);
                break;
            }
        }
    }
    /*
        for( i=-72;i<72;i++)
        {
            for(j=0;j<240;j++)
            {

                    ball1.rgbtBlue = 0;	ball1.rgbtGreen = 0; ball1.rgbtRed =i*1.5;
                    SetColor(shexianx[i+72][j],shexiany[i+72][j],ball1);

            }
        }
    */
    /*
    for( i=-72;i<72;i++)
    {
        if(bishe[i+72]>0)
        {
            for(j=0;j<240;j++)
            {
                    ball1.rgbtBlue = 0;	ball1.rgbtGreen = 0; ball1.rgbtRed =255;
                    SetColor(shexianx[i+72][j],shexiany[i+72][j],ball1);
            }
            DrawFocus(shexianx[i+72][bishe[i+72]],shexiany[i+72][bishe[i+72]],&ball1);
        }
    }
    */
    int pqu; // p（膨）胀qu角度（区）区
    int a;
    double guibiquyu;
    guibiquyu = fanwei;
    ball1.rgbtBlue = 0;
    ball1.rgbtGreen = 0;
    ball1.rgbtRed = 255;
    lianxuchang = 0;
    double shiyanx;
    for (i = 0; i < 144; i++)
    {
        if (bishe[i] != 0)
        {
            shiyanx = double(guibiquyu / double(quxian[bishe[i]])) * 12;
            for (pqu = 0; pqu < (double(guibiquyu / double(quxian[bishe[i]])) * 12); pqu++)
            //	for(pqu=0;pqu<20;pqu++)
            {
                a = i - pqu;
                if (a < 0)
                    a += 144;
                bishe[a] = bishe[i];
                //			break;
            }
        }
    }
    // 按宽度转圈扩大
    for (i = 144; i > 0; i--)
    {
        if (bishe[i] != 0)
            for (pqu = 0; pqu < (double(guibiquyu / double(quxian[bishe[i]])) * 12); pqu++)
            //	for(pqu=0;pqu<20;pqu++)
            {
                a = i + pqu;
                if (a >= 144)
                    a -= 144;
                bishe[a] = bishe[i];
                //	break;
            }
    }
    // 按宽度反转圈扩大
    ball1.rgbtBlue = 250;
    ball1.rgbtGreen = 255;
    ball1.rgbtRed = 255;

    double chang1;
    int jiao1;
    double x1;
    double y1;
    jiao1 = *you;
    //	zhizuobiao(&x1,&y1,jiao1,100,320,240);
    //	DrawFocus(x1,y1,&ball1);

    *you = mubiao / 2.5;
    *zuo = mubiao / 2.5;
    int zuixiaoyou;
    int zuixiaozuo;
    zuixiaoyou = 1000;
    zuixiaozuo = 1000;
    //	CString strxx;
    //	strxx.Format("%d",mubiao);
    //	AfxMessageBox(strxx);
    for (i = mubiao / 2.5; i < mubiao / 2.5 + 144; i++)
    {
        a = i;
        if (i >= 72)
            a = i - 144;
        if (i < -72)
            a = i + 144;
        if (bishe[a + 72] == 0)
        {
            *you = a;
            break;
        }
    }
    // 转圈找出离目标物最近的可通行射线
    for (i = mubiao / 2.5; i > mubiao / 2.5 - 144; i--)
    {
        a = i;
        if (i >= 72)
            a = i - 144;
        if (i < -72)
            a = i + 144;
        if (bishe[a + 72] == 0)
        {
            *zuo = a;
            break;
        }
    }
    // 反转圈找出离目标物最近的可通行射线
    int panding;
    panding = 0;
    for (i = 0; i < 144; i++)
        if (bishe[i] == 0)
        {
            panding++;
            for (j = nei; j < wai; j++)
            {
                ball1.rgbtBlue = 0;
                ball1.rgbtGreen = 0;
                ball1.rgbtRed = 255;
                SetColor(shexianx[i][j], shexiany[i][j], ball1);
            }
        }
    /*
for(j=0;j<240;j++)
{
    ball1.rgbtBlue =0;	ball1.rgbtGreen = 0; ball1.rgbtRed =255;
    SetColor(shexianx[*you][j],shexiany[*you][j],ball1);
}
for(j=0;j<240;j++)
{
    ball1.rgbtBlue =0;	ball1.rgbtGreen = 255; ball1.rgbtRed =255;
    SetColor(shexianx[*zuo][j],shexiany[*zuo][j],ball1);
}
*/
    *you = *you * 2.5;
    *zuo = *zuo * 2.5;
    double xxx12;
    double yyy12;
    for (j = 1; j < 240; j++)
    {

        zhizuobiao(&xxx12, &yyy12, *you, j, 320, 240);
        ball1.rgbtBlue = 0;
        ball1.rgbtGreen = 255;
        ball1.rgbtRed = 255;
        SetColor(xxx12, yyy12, ball1);
    }
    for (j = 1; j < 240; j++)
    {

        zhizuobiao(&xxx12, &yyy12, *zuo, j, 320, 240);
        ball1.rgbtBlue = 0;
        ball1.rgbtGreen = 0;
        ball1.rgbtRed = 255;
        SetColor(xxx12, yyy12, ball1);
    }
    return panding;
}

void UPRColor::jingqumen(Yuzhi *yu)
{
    /*
    RGBTRIPLE door1;
    door1.rgbtBlue = 0x00;door1.rgbtGreen = 0x00; door1.rgbtRed = 0xFF;

    //	DrawFocus(30,30,&door1);
    double chang=56000;
    double chang1=56000;
    double zuixiaox;
    double zuixiaoy;
    double zuixiaox1;
    double zuixiaoy1;

    for(int x=80;x<560;x++)
    for(int y=0;y<480;y++)
    {
        if(erzhi_d[x][y]==1 && banjing[x][y]<chang)
        {
            chang=banjing[x][y];
            zuixiaox=x;
            zuixiaoy=y;
            //DrawFocus(zuixiaox,zuixiaoy,&door1);
        }
        if(erzhi_j[x][y]==1 && banjing[x][y]<chang1)
        {
            chang1=banjing[x][y];
            zuixiaox1=x;
            zuixiaoy1=y;
        //	DrawFocus(zuixiaox,zuixiaoy,&door1);
        }
    }
    chang=sqrt(chang);
    chang1=sqrt(chang1);
    zhizuobiao(&zuixiaox,&zuixiaoy,yu->ddoor_jiao,chang,320,240);
    DrawFocus(zuixiaox,zuixiaoy,&door1);
    yu->ddoor_x=zuixiaox;
    yu->ddoor_y=zuixiaoy;
    yu->ddoor_chang=chang;
    zhizuobiao(&zuixiaox1,&zuixiaoy1,yu->jdoor_jiao,chang1,320,240);
    DrawFocus(zuixiaox1,zuixiaoy1,&door1);
    yu->jdoor_x=zuixiaox1;
    yu->jdoor_y=zuixiaoy1;
    yu->jdoor_chang=chang1;
    */
}

void UPRColor::dingwei(Yuzhi *changdi)
{
    cdbl = MK;
    double dmenjiao;
    double jmenjiao;
    double dmenchang;
    double jmenchang;
    double chedimen;
    double xfangxiang;
    double yfangxiang;
    double lingjiao;
    double lingchang;
    double balljiao;
    double ballchang;
    double ballx;
    double bally;
    if (changdi->ddoor_chang < changdi->jdoor_chang)
    {
        jizuobiao(320, 240, &dmenjiao, &dmenchang, changdi->ddoor_x, changdi->ddoor_y);
        jizuobiao(changdi->jdoor_x, changdi->jdoor_y, &jmenjiao, &jmenchang, changdi->ddoor_x, changdi->ddoor_y);
        chedimen = jiajiao(dmenjiao, jmenjiao);
        xfangxiang = cos(float(float(chedimen) / 57.296)) * double(quxian[int(changdi->ddoor_chang) - 3]);
        yfangxiang = sin(float(float(chedimen) / 57.296)) * double(quxian[int(changdi->ddoor_chang) - 3]);

        //		pField->SetAgent(1,1800-xfangxiang*2,600+yfangxiang*2,-chedimen-changdi->ddoor_jiao);
        changdi->benjix = 1800 - xfangxiang * 2;
        changdi->benjiy = 600 + yfangxiang * 2;
        changdi->benjijiao = -chedimen - changdi->ddoor_jiao;
    }
    else
    {
        jizuobiao(320, 240, &jmenjiao, &jmenchang, changdi->jdoor_x, changdi->jdoor_y);
        jizuobiao(changdi->ddoor_x, changdi->ddoor_y, &dmenjiao, &dmenchang, changdi->jdoor_x, changdi->jdoor_y);
        chedimen = jiajiao(jmenjiao, dmenjiao);
        xfangxiang = cos(float(float(chedimen) / 57.296)) * double(quxian[int(changdi->jdoor_chang) - 3]);
        yfangxiang = sin(float(float(chedimen) / 57.296)) * double(quxian[int(changdi->jdoor_chang) - 3]);
        //	CString str;
        //	str.Format("x=%f,y=%f",xfangxiang,yfangxiang);
        //	AfxMessageBox(str);
        //	jizuobiao(1800-xfangxiang*2.4+100,(250+yfangxiang)*2.4,&lingjiao,&lingchang,1800-xfangxiang*2.4,(250+yfangxiang)*2.4);
        //	CString str;
        //	str.Format("jiao=%f",lingjiao);
        //	AfxMessageBox(str);
        //	pField->SetAgent(1,xfangxiang*2,600-yfangxiang*2,(-chedimen-changdi->jdoor_jiao)-180);
        changdi->benjix = xfangxiang * 2;
        changdi->benjiy = 600 - yfangxiang * 2;
        changdi->benjijiao = (-chedimen - changdi->jdoor_jiao) - 180;
        //	pField->SetAgent(1,1800-xfangxiang*2.4,(250+yfangxiang)*2.4,0);
    }

    pizhi = changdi;
}

void UPRColor::menbianjie(Yuzhi *y)
{
    /*
    double x1;
    double y1;
    int dzuo;
    int dyou;
    int jzuo;
    int jyou;
    int jiao;
    int i;
    int j;
    RGBTRIPLE ddoor;
    ddoor.rgbtBlue=255;
    ddoor.rgbtGreen=0;
    ddoor.rgbtRed=0;
        RGBTRIPLE jdoor;
    jdoor.rgbtBlue=0;
    jdoor.rgbtGreen=255;
    jdoor.rgbtRed=0;

    for(int i11=0;i11<640;i11++)
            for(int j11=0;j11<480;j11++)
            {
                if(erzhi_d[i11][j11]==1)
                    SetColor(i11,j11,ddoor);
                if(erzhi_j[i11][j11]==1)
                    SetColor(i11,j11,jdoor);

            }
                RGBTRIPLE xdoor ;

    xdoor.rgbtBlue=0;
    xdoor.rgbtGreen=255;
    xdoor.rgbtRed=0;
//		for( i=0;i<144;i++)
//		for( j=0;j<240;j++)
//			SetColor(shexianx[i][j],shexiany[i][j],xdoor);

    dzuo=y->ddoor_jiao/2.5;
    dyou=y->ddoor_jiao/2.5;
    jzuo=y->jdoor_jiao/2.5;
    jyou=y->jdoor_jiao/2.5;
    for( i=y->ddoor_jiao/2.5;i<y->ddoor_jiao/2.5+20;i++)
    {
        jiao=i+72;
        if(jiao>=144)
            jiao=jiao-144;
        for( j=30;j<240;j++)
        {
            if(erzhi_d[shexianx[jiao][j]][shexiany[jiao][j]]==1)
            {
                if(jiajiao((dyou-72)*2.5,(jiao-72)*2.5)>0)
                {
                    dyou=jiao;
                }
                break;
            }
        }
    }
    for(i=y->ddoor_jiao/2.5;i>y->ddoor_jiao/2.5-20;i--)
    {
        jiao=i+72;
        if(jiao<0)
            jiao=jiao+144;
        if(jiao<-72)
            break;
        for( j=30;j<240;j++)
        {
            if(erzhi_d[shexianx[jiao][j]][shexiany[jiao][j]]==1)
            {
                if(jiajiao((dyou-72)*2.5,(jiao-72)*2.5)<=0)
                {
                    dzuo=jiao;
                }
                break;
            }
        }
    }
    for(i=y->jdoor_jiao/2.5;i<y->jdoor_jiao/2.5+20;i++)
    {
        jiao=i+72;
        if(jiao>=144)
            jiao=jiao-144;
        for( j=30;j<240;j++)
        {
            if(erzhi_j[shexianx[jiao][j]][shexiany[jiao][j]]==1)
            {
                if(jiajiao((jyou-72)*2.5,(jiao-72)*2.5)>0)
                {
                    jyou=jiao;
                }
                break;
            }
        }
    }
    for(i=y->jdoor_jiao/2.5;i>y->jdoor_jiao/2.5-20;i--)
    {
        jiao=i+72;
        if(jiao<0)
            jiao=jiao+144;
        if(jiao<-72)
            break;
        for(int j=30;j<240;j++)
        {
            if(erzhi_j[shexianx[jiao][j]][shexiany[jiao][j]]==1)
            {
                if(jiajiao((jyou-72)*2.5,(jiao-72)*2.5)<=0)
                {
                    jzuo=jiao;
                }
                break;
            }
        }
    }
    RGBTRIPLE ball;ball.rgbtBlue = 0xFF;ball.rgbtGreen = 0x00; ball.rgbtRed = 0x00;

    zhizuobiao(&x1,&y1,jiajiao(0,(jzuo-72)*2.5),y->jdoor_chang,320,240);
    y->jdoor_zuo=jiajiao(0,(jzuo-72)*2.5);
    DrawFocus(x1,y1,&ball);
    zhizuobiao(&x1,&y1,jiajiao(0,(jyou-72)*2.5),y->jdoor_chang,320,240);
    y->jdoor_you=jiajiao(0,(jyou-72)*2.5),
    DrawFocus(x1,y1,&ball);
    y->jdoor_jiao =jiajiao(0,jiajiao(y->jdoor_zuo,y->jdoor_you)/2+y->jdoor_zuo);
    y->jdoor_kuan = abs(jiajiao(y->jdoor_zuo,y->jdoor_you));
    zhizuobiao(&x1,&y1,y->jdoor_jiao,y->jdoor_chang,320,240);
    DrawFocus(x1,y1,&ball);
    y->jdoor_x=x1;
    y->jdoor_y=y1;
    zhizuobiao(&x1,&y1,jiajiao(0,(dzuo-72)*2.5),y->ddoor_chang,320,240);
    y->ddoor_zuo=jiajiao(0,(dzuo-72)*2.5);
    DrawFocus(x1,y1,&ball);
    zhizuobiao(&x1,&y1,jiajiao(0,(dyou-72)*2.5),y->ddoor_chang,320,240);
    y->ddoor_you=jiajiao(0,(dyou-72)*2.5),
    DrawFocus(x1,y1,&ball);
    y->ddoor_jiao =jiajiao(0,jiajiao(y->ddoor_zuo,y->ddoor_you)/2+y->ddoor_zuo);
    y->ddoor_kuan=abs(jiajiao(y->ddoor_zuo,y->ddoor_you));
    zhizuobiao(&x1,&y1,y->ddoor_jiao,y->ddoor_chang,320,240);
    DrawFocus(x1,y1,&ball);
    y->ddoor_x=x1;
    y->ddoor_y=y1;


        RGBTRIPLE ddoorx;
    ddoorx.rgbtBlue=255;
    ddoorx.rgbtGreen=255;
    ddoorx.rgbtRed=255;

        double xx1;
    double yy1;
    double ix;
    for(ix=-180;ix<180;ix=ix+0.2)
    {

        zhizuobiao(&xx1,&yy1,ix,y->jdoor_chang,320,240);
        SetColor(xx1,yy1,ddoorx);
                zhizuobiao(&xx1,&yy1,ix,y->jdoor_chang-1,320,240);
        SetColor(xx1,yy1,ddoorx);
    };
//	CString str;
//	str.Format("%5f",y->ddoor_kuan);
//	AfxMessageBox(str);
*/
}

// DEL int UPRColor::hejiao(int jiao1, int jiao2)
// DEL {
// DEL 	if(jiao1>0 && jiao2>0)
// DEL 		return jiao1+jiao2
// DEL }

/*//这些常量 转移到#include "./ConstantFile/CamConstantSet.h"里定义
const INT OMNI_VISION_CAM_WIDTH = 640;//320;//全景分辨率x
const INT OMNI_VISION_CAM_HEIGHT= 480;//240;//全景分辨率y
const INT MIRROR_CENTER_X		= OMNI_VISION_CAM_WIDTH/2;//镜子中心x
const INT MIRROR_CENTER_Y		= OMNI_VISION_CAM_HEIGHT/2;//镜子中心y
const INT MIRROR_RADIUS			= OMNI_VISION_CAM_HEIGHT/2;//镜子半径r
const INT MIRROR_LIMIT_L		= MIRROR_CENTER_X-MIRROR_RADIUS;//镜子左边界	自动计算出的常量
const INT MIRROR_LIMIT_R		= MIRROR_CENTER_X+MIRROR_RADIUS;//镜子左边界	自动计算出的常量
const INT MIRROR_LIMIT_UP		= MIRROR_CENTER_Y+MIRROR_RADIUS;//镜子上边界	自动计算出的常量
const INT MIRROR_LIMIT_DOWN		= MIRROR_CENTER_Y-MIRROR_RADIUS;//镜子下边界	自动计算出的常量

const INT circle_mask_radius_min =0;//20//0 //设置全景掩膜 半径最小长度
const INT circle_mask_radius_max =240;//90  //设置全景掩膜 半径最大长度
*/

const INT CS_OMNI_VISION_CAM_WIDTH = OMNI_VISION_CAM_HEIGHT; // 全景分辨率x
const INT CS_OMNI_VISION_CAM_HEIGHT = OMNI_VISION_CAM_WIDTH; // 全景分辨率y
unsigned int bwdata_temp[CS_OMNI_VISION_CAM_HEIGHT][CS_OMNI_VISION_CAM_WIDTH];
#define CS_MCAMI_MIRROR_CENTER_X MIRROR_CENTER_Y                                // 320//160///镜头中心位置
#define CS_MCAMI_MIRROR_CENTER_Y MIRROR_CENTER_X                                // 240//120//镜头中心位置
#define circle_mask_set_long CS_OMNI_VISION_CAM_WIDTH *CS_OMNI_VISION_CAM_WIDTH // 480*480//320*240//扇半径最大长度
unsigned int circle_mask_long;
int circle_maskX[circle_mask_set_long];
int circle_maskY[circle_mask_set_long];

void UPRColor::bool_max_connectivity_analyze_mask_int(void)
{
    const int x320 = CS_OMNI_VISION_CAM_WIDTH; // 320;//320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240 = CS_OMNI_VISION_CAM_HEIGHT; // 240;//240;//处理图像数据的大小设置
    // 一个扇形的掩膜%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // 像素个数估算方法 取半径r像素 PI*r*r =D像素面积    1像素面积1*1
    // int circle_area=3.14*saradius_max*saradius_max - 3.14*saradius_min*saradius_min +
    // 2*3.14*saradius_max;//大圆面积+小圆面积+大圆周长
    int r, du, i, j, x, y; //,pian,dian_shu,diandian
    // int circle_mask[1360*36];//扇形一 每个扇形10度 一个扇形1600个点 1600 = (240*240)/36
    const int sacenter_x = CS_MCAMI_MIRROR_CENTER_X; // 扇中心X
    const int sacenter_y = CS_MCAMI_MIRROR_CENTER_Y; // 扇中心Y
    int saradius_min = circle_mask_radius_min;       // 扇半径最小长度
    int saradius_max = circle_mask_radius_max;       // 扇半径最大长度

    for (i = 0; i < circle_mask_set_long; i++)
    {
        circle_maskX[i] = 0;
        circle_maskY[i] = 0;
    }
    for (y = 0; y < y240; y++) // 初始化
    {
        for (x = 0; x < x320; x++) // 初始化
        {
            bwdata_temp[y][x] = 0;
        }
    }
    /*	for(du=0;du<10*4*36;du++)
        {	for(r=saradius_min;r<saradius_max;r++)//半径20以下是机体不算
            {	x=(int)(sin((du+0*40)*3.14/(180*4))*r)+sacenter_x;//0*40是偏移量
                y=(int)(cos((du+0*40)*3.14/(180*4))*r)+sacenter_y;
                bwdata_temp[y][x]=1;
        }	}
    */

    for (y = 0; y < saradius_max; y++)
    {
        j = saradius_max * saradius_max - y * y;
        if (j >= 0)
        {
            j = (int)(sqrt((double)(j)));
            for (x = -j + CS_MCAMI_MIRROR_CENTER_X; x < j + CS_MCAMI_MIRROR_CENTER_X; x++)
            {
                bwdata_temp[CS_MCAMI_MIRROR_CENTER_Y + y][x] = 1;
                bwdata_temp[CS_MCAMI_MIRROR_CENTER_Y - y][x] = 1;
            }
        }
    }
    for (y = 0; y < saradius_min; y++)
    {
        j = saradius_min * saradius_min - y * y;
        if (j >= 0)
        {
            j = (int)(sqrt((double)(j)));
            for (x = -j + CS_MCAMI_MIRROR_CENTER_X; x < j + CS_MCAMI_MIRROR_CENTER_X; x++)
            {
                bwdata_temp[CS_MCAMI_MIRROR_CENTER_Y + y][x] = 0;
                bwdata_temp[CS_MCAMI_MIRROR_CENTER_Y - y][x] = 0;
            }
        }
    }
    circle_mask_long = 0;
    for (y = 0; y < y240; y++) // 初始化
    {
        for (x = 0; x < x320; x++) // 初始化
            if (bwdata_temp[y][x] == 1)
            {
                bwdata_temp[y][x] = 0;
                circle_maskX[circle_mask_long] = x;
                circle_maskY[circle_mask_long] = y;
                circle_mask_long++;
            }
    }
    // delete shan;
    //("实测点数%d 计算点数%d 现在使用%d",i,abcd,1360*36);
    // 完成扇形的掩膜%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // 测试代码
    /*
        RGBTRIPLE cPoint;
        cPoint.rgbtBlue = 0x00;
        cPoint.rgbtGreen = 0xFF;
        cPoint.rgbtRed = 0x00;
    i=0;while(i<circle_mask_long)
    {
    y=circle_maskY[i];
    x=circle_maskX[i];
        SetColor(x,y,cPoint);
    i++;
    }
    */
}

const long int E_G_H_MAX_LONG_SET =
    640 * 480; // 一般图像总像素*2都不会溢出  最长边缘历史纪录设置 如果超过这个值程序 要报错 需重新设置此值
int e_g_hx[E_G_H_MAX_LONG_SET]; // 边缘生长历史记录x
int e_g_hy[E_G_H_MAX_LONG_SET]; // 边缘生长历史记录y
int e_g_hd[E_G_H_MAX_LONG_SET]; // 边缘生长历史记录 方向
long int e_g_lh;                // 边缘生长历史记录长度
int connect_area_x[CS_OMNI_VISION_CAM_HEIGHT * CS_OMNI_VISION_CAM_WIDTH];
int connect_area_y[CS_OMNI_VISION_CAM_HEIGHT * CS_OMNI_VISION_CAM_WIDTH];
long int connect_area_l;

inline bool edge_judge(int y, int x) // C1_Bool_Data可替换成e_g_d
{
    if (erzhi_b[y][x] == 0)
    {
        if (erzhi_b[y + 1][x] == 1)
            return (1);
        else if (erzhi_b[y - 1][x] == 1)
            return (1);
        else if (erzhi_b[y][x + 1] == 1)
            return (1);
        else if (erzhi_b[y][x - 1] == 1)
            return (1);
    }
    return (0);
}

// 基于 边缘检测的膨胀
/////////////////////

// 判断设定[有效]像素 设定方向 是否是边沿
inline bool d_edge_judge(int y, int x, char direct) // C1_Bool_Data可替换成e_g_d
{                                                   // direct 可赋值0 1 2 3 对应 右 下 左 上  四个边沿
    switch (direct)
    {
    case 0:
        if ((erzhi_b[y][x] == 1) && (erzhi_b[y][x + 1] != 1))
            return (1);
        else
            return (0);
    case 1:
        if ((erzhi_b[y][x] == 1) && (erzhi_b[y - 1][x] != 1))
            return (1);
        else
            return (0);
    case 2:
        if ((erzhi_b[y][x] == 1) && (erzhi_b[y][x - 1] != 1))
            return (1);
        else
            return (0);
    case 3:
        if ((erzhi_b[y][x] == 1) && (erzhi_b[y + 1][x] != 1))
            return (1);
        else
            return (0);
    default:
        return (0);
    }
}
// 判断设定[无效]像素 设定方向 是否是边沿
inline bool d_edge_judge_(int y, int x, char direct) // C1_Bool_Data可替换成e_g_d
{                                                    // direct 可赋值0 1 2 3 对应 右 下 左 上  四个边沿
    switch (direct)
    {
    case 0:
        if ((erzhi_b[y][x] != 1) && (erzhi_b[y][x + 1] == 1))
            return (1);
        else
            return (0);
    case 1:
        if ((erzhi_b[y][x] != 1) && (erzhi_b[y - 1][x] == 1))
            return (1);
        else
            return (0);
    case 2:
        if ((erzhi_b[y][x] != 1) && (erzhi_b[y][x - 1] == 1))
            return (1);
        else
            return (0);
    case 3:
        if ((erzhi_b[y][x] != 1) && (erzhi_b[y + 1][x] == 1))
            return (1);
        else
            return (0);
    default:
        return (0);
    }
}
inline char direct_add(char direct, char a)
{
    direct += a;
    if (direct > 3)
        return (direct - 4);
    if (direct < 0)
        return (direct + 4);
    return (direct);
}

void UPRColor::bool_max_connectivity_analyze_e2(float *obj_weight_map)
{
    long int all_edge_long = 0; // 所有边沿长度总和
    all_edge_long = 0;

    RGBTRIPLE cPointG;
    cPointG.rgbtBlue = 0x00;
    cPointG.rgbtGreen = 0xFF;
    cPointG.rgbtRed = 0x00;
    RGBTRIPLE cPointB;
    cPointB.rgbtBlue = 0xFF;
    cPointB.rgbtGreen = 0x00;
    cPointB.rgbtRed = 0x00;
    register int x, y, i, j;
    register long int e_g_lh;
    const int x320 = CS_OMNI_VISION_CAM_WIDTH; // 320;//320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240 = CS_OMNI_VISION_CAM_HEIGHT; // 240;//240;//处理图像数据的大小设置

    connect_area_l = 0;
    int connect_area_num =
        1; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点 2连通域的第一个编号 3 4 5....
    int connect_max_area_num = 1;     // 最大连通域的编号	默认无效值1
    int connect_area_w = 0;           // 记录连通域重量值
    int connect_max_area_w = 0;       // 记录连通域最大重量值
    int connect_area_s_e_w[65536][3]; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                      // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]

    char go_dircet = 0; // 起始行走方向
    char go_dircet_t = go_dircet;
    char direct;

    int add_mm[y240][2]; // add_mm[240][0] = min_X        add_mm[240][1] = max_X
    int max_y = 0;
    int min_y = y240 - 1;
    int max_x = 0;
    int min_x = x320 - 1;

    // 边缘检测
    int x_t, y_t;
    // for (y=0;y<y240;y++)//找边缘上第一个点
    //{
    // for (x=0;x<x320;x++)
    //{
    j = 0;
    while (j < circle_mask_long)
    {
        y = circle_maskY[j];
        x = circle_maskX[j];

        if (erzhi_b[y][x] == 1)
        { //	x_t=x;y_t=y;  y=y240;break;
            y_t = y;
            x_t = x;
            // 设置种子
            y--; // 取下面的一个点

            // 如果不是上边缘([x][y-1]==0)&&([x][y]==1) 就向下查找下边缘
            if (d_edge_judge_(y, x, 3) == 0)
            {
                for (; y >= 0; y--)
                {
                    if (d_edge_judge_(y, x, 3) == 1)
                        break; // 向下查找边缘 找到后退出
                }
            }

            go_dircet = 0; // 起始行走方向 向右走
            go_dircet_t = go_dircet;
            direct = direct_add(go_dircet, -1); // 贴左手走 边沿方向应该是行走方向-1
            e_g_lh = 0;
            e_g_hx[e_g_lh] = x;         // 边缘生长历史记录x
            e_g_hy[e_g_lh] = y;         // 边缘生长历史记录y
            e_g_hd[e_g_lh] = go_dircet; // 边缘生长历史记录 方向
            e_g_lh++;
            //	data[(y*320+x)*3+0]=254;//测试代码//显示种子点
            //	data[(y*320+x)*3+1]=0;//测试代码
            //	data[(y*320+x)*3+2]=254;//测试代码
            //	SetColor(y,x,cPointG);//测试代码
            //	m_VideoOCX.Show(m_Image);//测试代码				// display capture frame
            for (i = 0; i < E_G_H_MAX_LONG_SET; i++)
            { // 在既撞墙又摸不着  的判定下 优先摸不着判定
                if (d_edge_judge_(y, x, direct_add(go_dircet, -1)) ==
                    0) // 左手摸不着判定//direct 可赋值0 1 2 3 对应 右 下 左 上  四个边沿
                {
                    go_dircet = direct_add(go_dircet, -1);
                    if (go_dircet == 0)
                        x++;
                    if (go_dircet == 1)
                        y--;
                    if (go_dircet == 2)
                        x--;
                    if (go_dircet == 3)
                        y++;
                    e_g_hx[e_g_lh] = x;         // 边缘生长历史记录x
                    e_g_hy[e_g_lh] = y;         // 边缘生长历史记录y
                    e_g_hd[e_g_lh] = go_dircet; // 边缘生长历史记录 方向
                    e_g_lh++;
                }
                else
                {
                    if (d_edge_judge_(y, x, go_dircet) == 1) // 撞墙判定//direct 可赋值0 1 2 3 对应 右 下 左 上 四个边沿
                    {
                        go_dircet = direct_add(go_dircet, +1);
                    }
                }
                if (go_dircet_t == go_dircet)
                {
                    if (go_dircet == 0)
                        x++;
                    if (go_dircet == 1)
                        y--;
                    if (go_dircet == 2)
                        x--;
                    if (go_dircet == 3)
                        y++;
                    e_g_hx[e_g_lh] = x;         // 边缘生长历史记录x
                    e_g_hy[e_g_lh] = y;         // 边缘生长历史记录y
                    e_g_hd[e_g_lh] = go_dircet; // 边缘生长历史记录 方向
                    e_g_lh++;
                }
                go_dircet_t = go_dircet;
                if ((e_g_hx[0] == x) && (e_g_hy[0] == y) && (e_g_hd[0] == go_dircet) && (e_g_lh > 1))
                    break;
                //	data[(y*320+x)*3+0]=1;//测试代码//显示边缘生长 单点生长
                //	data[(y*320+x)*3+1]=254;//测试代码
                //	data[(y*320+x)*3+2]=1;//测试代码
                //	SetColor(y,x,cPointG);//测试代码
                //	Sleep(100);//测试代码
            }
            //	m_VideoOCX.Show(m_Image);//测试代码				// display capture frame

            // all_edge_long+=e_g_lh;
            //		 CString str;
            //		 str.Format("e_g_lh=%d",e_g_lh);
            //		 AfxMessageBox(str);
            //		 return;

            if (e_g_lh >= E_G_H_MAX_LONG_SET)
            {
                CString str;
                str.Format("常量E_G_H_MAX_LONG_SET [最长边缘历史纪录设置]设置过小");
                AfxMessageBox(str);
                str.Format("connect_area_num=%d", connect_area_num);
                AfxMessageBox(str);
                return;
            }
            // 开始进行连通域填充			//注意其他方法 可以用如中值定理 就是每行x排序
            // x1--填充--x2--空--x3--填充--x4--空--x5--填充--x6 找到同y值的对应几个x值作一地址表 int
            // add_map[240][320];//最多有240个y值 最多每个y有320个x值  不算重复的 int add_mapL[320];
            // CS_OMNI_VISION_CAM_HEIGHT][CS_OMNI_VISION_CAM_WIDTH];//[240][320];//生长过的边缘
            max_y = 0;
            min_y = y240 - 1;
            max_x = 0;
            min_x = x320 - 1;
            for (i = 0; i < y240; i++)
            {
                add_mm[i][1] = 0;
                add_mm[i][0] = x320 - 1;
            }
            for (i = 0; i < e_g_lh; i++)
            {
                if (add_mm[e_g_hy[i]][0] > e_g_hx[i])
                    add_mm[e_g_hy[i]][0] = e_g_hx[i];
                if (add_mm[e_g_hy[i]][1] < e_g_hx[i])
                    add_mm[e_g_hy[i]][1] = e_g_hx[i];
                if (min_y > e_g_hy[i])
                    min_y = e_g_hy[i];
                if (max_y < e_g_hy[i])
                    max_y = e_g_hy[i];
                if (min_x > e_g_hx[i])
                    min_x = e_g_hx[i];
                if (max_x < e_g_hx[i])
                    max_x = e_g_hx[i];
            }
            connect_area_num++;                                       // 记录用到的连通域号
            connect_area_s_e_w[connect_area_num][0] = connect_area_l; // 存储1000个连通域的[首]connect_area_s_e[n][0]
            connect_area_w = 0;
            for (y = min_y; y < max_y; y++)
            {
                for (x = add_mm[y][0]; x < add_mm[y][1]; x++)
                {
                    if (erzhi_b[y][x] == 1)
                    {
                        erzhi_b[y][x] = connect_area_num;
                        //	SetColor(y,x,cPointB);

                        connect_area_x[connect_area_l] = x;
                        connect_area_y[connect_area_l] = y;
                        connect_area_l++;
                        connect_area_w++;

                        //				data[(y*320+x)*3+0]=254;//测试代码
                        //				data[(y*320+x)*3+1]=1;//测试代码
                        //				data[(y*320+x)*3+2]=1;//测试代码

                        // else
                        //	erzhi_b[y][x]=0;
                    }
                }
            }
            //	m_VideoOCX.Show(m_Image);//测试代码				// display capture frame
            //	Sleep(200);
            //	if (connect_area_num>42)//测试代码
            //	{connect_area_num=connect_area_num;//测试代码
            //	}//测试代码
            connect_area_s_e_w[connect_area_num][1] = connect_area_l; // 存储1000个连通域的[尾]connect_area_s_e[n][1]
            connect_area_s_e_w[connect_area_num][2] = connect_area_w;
            if (connect_max_area_w < connect_area_w)
            {
                connect_max_area_w = connect_area_w;
                connect_max_area_num = connect_area_num;
            }

            y = y_t;
            x = x_t;
        }
        j++;
    } // while()
    //}//for(320)
    //}//for(240)
    //		 CString str;//显示种子点
    //		 str.Format("边缘长度=%d",lerjia);//显示种子点
    //		 AfxMessageBox(str);//显示种子点
    //		 return;//显示种子点
    register float OBJ_temp_b = 0;
    register float OBJ_temp_b_ = 0;
    register float OBJ_temp_c = 0;
    register float ball_op_x = 0;
    register float ball_op_y = 0;
    register float ball_op_weight = 0;
    if (connect_max_area_num > 1) // 最大连通域的编号	默认无效值1
    {
        for (i = connect_area_s_e_w[connect_max_area_num][0]; i < connect_area_s_e_w[connect_max_area_num][1]; i++)
        {
            SetColor(connect_area_y[i], connect_area_x[i], cPointB);
            OBJ_temp_c++;
            OBJ_temp_b += connect_area_x[i];
            OBJ_temp_b_ += connect_area_y[i];
            //	data[(connect_area_y[i]*320+connect_area_x[i])*3+0]=254;//测试代码//显示种子点
            //	data[(connect_area_y[i]*320+connect_area_x[i])*3+1]=254;//测试代码
            //	data[(connect_area_y[i]*320+connect_area_x[i])*3+2]=254;//测试代码
        }
    }
    // m_VideoOCX.Show(m_Image);//测试代码				// display capture frame

    if (OBJ_temp_c != 0) // 保险程序  当同步不佳时  有时Capture不到图象 就会出现所有象素都为零 这时做OBJ_temp_b /
                         // OBJ_temp_c的除法就会产生分母为零的错误
    {
        ball_op_x = OBJ_temp_b / OBJ_temp_c;  //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+320
        ball_op_y = OBJ_temp_b_ / OBJ_temp_c; //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+240
    }
    ball_op_weight = OBJ_temp_c; // 准备把象素重量传给主逻辑函数
}

unsigned int area_grow_data_arex[CS_OMNI_VISION_CAM_HEIGHT *
                                 CS_OMNI_VISION_CAM_WIDTH]; //[640*480];//currently当前生长的一圈的纪录
unsigned int area_grow_data_arey[CS_OMNI_VISION_CAM_HEIGHT *
                                 CS_OMNI_VISION_CAM_WIDTH]; //[640*480];//currently当前生长的一圈的纪录
void UPRColor::bool_max_connectivity_analyze2_1(float *obj_weight_map)
{
    register unsigned long int area_grow_data_arel = 0; // 纪录currently当前这一圈有几个像素
    RGBTRIPLE cPointG;
    cPointG.rgbtBlue = 0x00;
    cPointG.rgbtGreen = 0xFF;
    cPointG.rgbtRed = 0x00;
    RGBTRIPLE cPointB;
    cPointB.rgbtBlue = 0xFF;
    cPointB.rgbtGreen = 0x00;
    cPointB.rgbtRed = 0x00;

    // const char H_min_diff		=8;	//这是一个可调值关系到颜色的分辨粗略程度 如15以上黄色和橘黄就分不开了
    // const int grow_area_times =12000;//整个生成 生长区的次数//5000 要1min 40s
    // const int grow_min_weight	=40;//30如果单个生长区重量大于grow_min_weight 则把纪录A存入C[n]
    const int x320 = CS_OMNI_VISION_CAM_WIDTH; // 320;//320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240 = CS_OMNI_VISION_CAM_HEIGHT; // 240;//240;//处理图像数据的大小设置
    const int x320r1 = x320 - 1;                // 320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240r1 = y240 - 1;                // 240;//处理图像数据的大小设置

    register int i, j, k, l, x, y; // seedx,seedy,,used_num
    // int x_grow,y_grow;
    // static unsigned char  area_grow_dataC[y240][x320];// 标号输出
    // register float  circumscription_weight;
    // register float  max_area_w;//最大的连通域的重量
    // register int    max_area_num;//最大的连通域的标号
    register int x_add, x_reduce, y_add, y_reduce, xg, yg;

    //	connect_area_l=0;
    register int connect_area_num =
        1; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点 2连通域的第一个编号 3 4 5....
    register int connect_max_area_num = 1;  // 最大连通域的编号	默认无效值1
    register int connect_area_w = 0;        // 记录连通域重量值
    register int connect_max_area_w = 0;    // 记录连通域最大重量值
    int connect_area_s_e_w[65536][3];       // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                            // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
    register int connect_area_layer_l = 0;  // 存储连通域 层的 点数 长度
    register int connect_area_layer_s = 0;  // 存储连通域 层的 首
    register int connect_area_layer_e = 0;  // 存储连通域 层的 尾
    register int connect_area_layer_pl = 0; // 存储连通域 层的 点数 长度
    register int connect_area_layer_ps = 0; // 存储连通域 上层的 首
    register int connect_area_layer_pe = 0; // 存储连通域 上层的 尾

    // used_num  =1;
    // max_area_w=0;
    // seedx=0;////////////////////
    // seedy=0;////////////////////

    ////////////////////////////////////
    // 黑边处理  防止出现边缘冒了//////
    for (x = 0; x < x320; x++)
    {
        erzhi_b[x][0] = 0;
        erzhi_b[x][y240 - 1] = 0;
    }
    for (y = 1; y < y240 - 1; y++)
    {
        erzhi_b[0][y] = 0;
        erzhi_b[x320 - 1][y] = 0;
    }
    // 黑边处理  防止出现边缘冒了//////

    area_grow_data_arel = 0; // 所有连通域纪录 位置置零
    // 1自动设置这次生长的起点种子
    // for(i=0;i<circle_mask_long;i++)
    i = 0;
    while (i < circle_mask_long)
    {
        y = circle_maskY[i];
        x = circle_maskX[i];
        if (erzhi_b[y][x] == 1) // 没生长过 且是有效点
        {                       // seedx=x;
            // seedy=y;
            // 2开始生长
            connect_area_num++; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点 2连通域的第一个编号 3
                                // 4 5....
            connect_area_w = 0; // 记录连通域重量值	清零
            connect_area_s_e_w[connect_area_num][0] =
                area_grow_data_arel; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                     // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
            // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
            // 1设置这次生长的起点种子
            area_grow_data_arex[area_grow_data_arel] = x;   // 当前次生长的一圈的纪录
            area_grow_data_arey[area_grow_data_arel++] = y; // 当前次生长的一圈的纪录
            connect_area_layer_l = 1;
            connect_area_layer_pe = connect_area_layer_ps + 1;
            //    		area_grow_dataAtpx[0]=x;//seedx;////手动设置种子
            //    		area_grow_dataAtpy[0]=y;//seedy;////手动设置种子
            //    		area_grow_dataAtpl=1;////手动设置种子
            connect_area_w++; // 连通域重量+1

            // 开始生长
            while (1) // for (i=0;i<1500;i++)//生成生长区域 的生长次数
            {
                connect_area_layer_s = area_grow_data_arel;
                j = connect_area_layer_ps;
                while (j < connect_area_layer_pe)
                {                                // 周围不是A中的有效点且符合生长规则
                    xg = area_grow_data_arex[j]; // 当前次生长的一圈的纪录
                    yg = area_grow_data_arey[j]; // 当前次生长的一圈的纪录
                    x_add = xg + 1;
                    x_reduce = xg - 1;
                    y_add = yg + 1;
                    y_reduce = yg - 1;
                    if (erzhi_b[y_add][x_reduce] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_add;
                        area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                        erzhi_b[y_add][x_reduce] = connect_area_num;
                    }
                    if (erzhi_b[y_add][xg] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_add;
                        area_grow_data_arex[area_grow_data_arel++] = xg; // 当前次生长的一圈的纪录
                        erzhi_b[y_add][xg] = connect_area_num;
                    }
                    if (erzhi_b[y_add][x_add] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_add;
                        area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                        erzhi_b[y_add][x_add] = connect_area_num;
                    }
                    if (erzhi_b[yg][x_reduce] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = yg;
                        area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                        erzhi_b[yg][x_reduce] = connect_area_num;
                    }
                    //	||
                    //((area_grow_dataA[y+0][x+0]==1)&&(abs(dataHSV[y+0][x+0][0]-tempH)<5)&&(abs(dataHSV[y+0][x+0][2]-tempV)<30)&&(abs(dataHSV[y+0][x+0][1]-tempS)<40))
                    if (erzhi_b[yg][x_add] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = yg;
                        area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                        erzhi_b[yg][x_add] = connect_area_num;
                    }
                    if (erzhi_b[y_reduce][x_reduce] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_reduce;
                        area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                        erzhi_b[y_reduce][x_reduce] = connect_area_num;
                    }
                    if (erzhi_b[y_reduce][xg] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_reduce;
                        area_grow_data_arex[area_grow_data_arel++] = xg; // 当前次生长的一圈的纪录
                        erzhi_b[y_reduce][xg] = connect_area_num;
                    }
                    if (erzhi_b[y_reduce][x_add] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_reduce;
                        area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                        erzhi_b[y_reduce][x_add] =
                            connect_area_num; ////////////////////////////////////////////改区域生长 加参数
                    }
                    j++;
                }
                connect_area_layer_e = area_grow_data_arel;
                connect_area_layer_l = connect_area_layer_e - connect_area_layer_s; // 连通域重量+当前一圈的点数
                connect_area_layer_ps = connect_area_layer_s;                       // 存储连通域 上层的 首
                connect_area_layer_pe = connect_area_layer_e;                       // 存储连通域 上层的 尾

                connect_area_w += connect_area_layer_l;
                if (connect_area_layer_l == 0)
                    break;
            }
            // 生长完成
            // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
            connect_area_s_e_w[connect_area_num][1] =
                area_grow_data_arel; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                     // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
            connect_area_s_e_w[connect_area_num][2] =
                connect_area_w; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0] [尾]connect_area_s_e[n][1]
                                // [重量]connect_area_s_e[n][2]
            if (connect_max_area_w < connect_area_w)
            {
                connect_max_area_w = connect_area_w;
                connect_max_area_num = connect_area_num;
            }
        }

        //	}
        //}
        i++;
    }

    //	for (i=0;i<area_grow_data_arel;i++)//测试代码
    //	{
    //		data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+0]=1;//测试代码//显示种子点
    //		data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+1]=254;//测试代码
    //		data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+2]=1;//测试代码
    //		m_VideoOCX.Show(m_Image);//测试代码				// display capture frame
    //		Sleep(200);
    //	}//测试代码

    // 不进行输出了 输出\\掉了
    // for (i=0;i<x320*y240;i++)
    //{	data_C_out[i]=0;
    // }
    // if (max_area_w>0)//(max_area_num>0)//////////////////////////////////////////////////////////////////
    //{	for (l=0;l<area_grow_data_areml;l++)
    //	{	data_C_out[area_grow_data_aremy[l]*320+area_grow_data_aremx[l]]=1;
    //	}
    // }
    /*
    i=0;while(i<circle_mask_long)
    {
    y=circle_maskY[i];
    x=circle_maskX[i];
    //	SetColor(x,y,cPointB);
        SetColor(y,x,cPointB);
    i++;
    }
    if (max_area_w>0)//(max_area_num>0)//////////////////////////////////////////////////////////////////
    {	for (l=0;l<area_grow_data_areml;l++)
        {
    //		SetColor(area_grow_data_aremx[l],area_grow_data_aremy[l],cPointG);
            SetColor(area_grow_data_aremy[l],area_grow_data_aremx[l],cPointG);
        }
    }
    */

    ////////////////////////////////////////////////////////////////////////////////////
    *obj_weight_map =
        connect_area_s_e_w[connect_max_area_num][2]; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                                     // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]

    // 带出黑白图象重心横坐标  obj_properties *All  All.x结果的重心的x坐标 All.y结果的重心的y坐标
    // 需要处理部分的掩膜数组  int *yan_mo_x,int *yan_mo_y
    // 需要处理部分的掩膜数组长度int yan_mo_long,
    register float OBJ_temp_b = 0;
    register float OBJ_temp_b_ = 0;
    register float OBJ_temp_c = 0;
    register float ball_op_x = 0;
    register float ball_op_y = 0;
    register float ball_op_weight = 0;
    if (connect_max_area_num > 1) // 最大连通域的编号	默认无效值1
    {
        for (i = connect_area_s_e_w[connect_max_area_num][0]; i < connect_area_s_e_w[connect_max_area_num][1]; i++)
        { //	SetColor(area_grow_data_arey[i],area_grow_data_arex[i],cPointB);
            OBJ_temp_c++;
            OBJ_temp_b += area_grow_data_arex[i];
            OBJ_temp_b_ += area_grow_data_arey[i];
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+0]=254;//测试代码//显示种子点
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+1]=254;//测试代码
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+2]=254;//测试代码
            //	SetColor(y,x,cPointG);//测试代码
        }
    }
    // m_VideoOCX.Show(m_Image);//测试代码				// display capture frame

    if (OBJ_temp_c != 0) // 保险程序  当同步不佳时  有时Capture不到图象 就会出现所有象素都为零 这时做OBJ_temp_b /
                         // OBJ_temp_c的除法就会产生分母为零的错误
    {
        ball_op_x = OBJ_temp_b / OBJ_temp_c;  //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+320
        ball_op_y = OBJ_temp_b_ / OBJ_temp_c; //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+240
    }
    ball_op_weight = OBJ_temp_c; // 准备把象素重量传给主逻辑函数
    //					HSI11->ball_x=ball_op_x;
    //					HSI11->ball_y=ball_op_y;
    //					HSI11->ball_zhong=ball_op_weight;

    // 完成二值化图像标重心程序///////////////////////////////////////////////////////////////////
}

void UPRColor::bool_max_connectivity_analyze2_1ball(float *obj_weight_map, Yuzhi *HSI11)
{
    register unsigned long int area_grow_data_arel = 0; // 纪录currently当前这一圈有几个像素
    RGBTRIPLE cPointG;
    cPointG.rgbtBlue = 0x00;
    cPointG.rgbtGreen = 0xFF;
    cPointG.rgbtRed = 0x00;
    RGBTRIPLE cPointB;
    cPointB.rgbtBlue = 0xFF;
    cPointB.rgbtGreen = 0x00;
    cPointB.rgbtRed = 0x00;

    // const char H_min_diff		=8;	//这是一个可调值关系到颜色的分辨粗略程度 如15以上黄色和橘黄就分不开了
    // const int grow_area_times =12000;//整个生成 生长区的次数//5000 要1min 40s
    // const int grow_min_weight	=40;//30如果单个生长区重量大于grow_min_weight 则把纪录A存入C[n]
    const int x320 = CS_OMNI_VISION_CAM_WIDTH; // 320;//320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240 = CS_OMNI_VISION_CAM_HEIGHT; // 240;//240;//处理图像数据的大小设置
    const int x320r1 = x320 - 1;                // 320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240r1 = y240 - 1;                // 240;//处理图像数据的大小设置

    register int i, j, k, l, x, y; // seedx,seedy,,used_num
    // int x_grow,y_grow;
    // static unsigned char  area_grow_dataC[y240][x320];// 标号输出
    // register float  circumscription_weight;
    // register float  max_area_w;//最大的连通域的重量
    // register int    max_area_num;//最大的连通域的标号
    register int x_add, x_reduce, y_add, y_reduce, xg, yg;

    //	connect_area_l=0;
    register int connect_area_num =
        1; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点 2连通域的第一个编号 3 4 5....
    register int connect_max_area_num = 1;  // 最大连通域的编号	默认无效值1
    register int connect_area_w = 0;        // 记录连通域重量值
    register int connect_max_area_w = 0;    // 记录连通域最大重量值
    int connect_area_s_e_w[65536][3];       // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                            // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
    register int connect_area_layer_l = 0;  // 存储连通域 层的 点数 长度
    register int connect_area_layer_s = 0;  // 存储连通域 层的 首
    register int connect_area_layer_e = 0;  // 存储连通域 层的 尾
    register int connect_area_layer_pl = 0; // 存储连通域 层的 点数 长度
    register int connect_area_layer_ps = 0; // 存储连通域 上层的 首
    register int connect_area_layer_pe = 0; // 存储连通域 上层的 尾

    // used_num  =1;
    // max_area_w=0;
    // seedx=0;////////////////////
    // seedy=0;////////////////////

    ////////////////////////////////////
    // 黑边处理  防止出现边缘冒了//////
    for (x = 0; x < x320; x++)
    {
        erzhi_b[x][0] = 0;
        erzhi_b[x][y240 - 1] = 0;
    }
    for (y = 1; y < y240 - 1; y++)
    {
        erzhi_b[0][y] = 0;
        erzhi_b[x320 - 1][y] = 0;
    }
    // 黑边处理  防止出现边缘冒了//////

    area_grow_data_arel = 0; // 所有连通域纪录 位置置零
    // 1自动设置这次生长的起点种子
    // for(i=0;i<circle_mask_long;i++)
    i = 0;
    while (i < circle_mask_long)
    {
        y = circle_maskY[i];
        x = circle_maskX[i];
        if (erzhi_b[y][x] == 1) // 没生长过 且是有效点
        {                       // seedx=x;
            // seedy=y;
            // 2开始生长
            connect_area_num++; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点 2连通域的第一个编号 3
                                // 4 5....
            connect_area_w = 0; // 记录连通域重量值	清零
            connect_area_s_e_w[connect_area_num][0] =
                area_grow_data_arel; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                     // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
            // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
            // 1设置这次生长的起点种子
            area_grow_data_arex[area_grow_data_arel] = x;   // 当前次生长的一圈的纪录
            area_grow_data_arey[area_grow_data_arel++] = y; // 当前次生长的一圈的纪录
            connect_area_layer_l = 1;
            connect_area_layer_pe = connect_area_layer_ps + 1;
            //    		area_grow_dataAtpx[0]=x;//seedx;////手动设置种子
            //    		area_grow_dataAtpy[0]=y;//seedy;////手动设置种子
            //    		area_grow_dataAtpl=1;////手动设置种子
            connect_area_w++; // 连通域重量+1

            // 开始生长
            while (1) // for (i=0;i<1500;i++)//生成生长区域 的生长次数
            {
                connect_area_layer_s = area_grow_data_arel;
                j = connect_area_layer_ps;
                while (j < connect_area_layer_pe)
                {                                // 周围不是A中的有效点且符合生长规则
                    xg = area_grow_data_arex[j]; // 当前次生长的一圈的纪录
                    yg = area_grow_data_arey[j]; // 当前次生长的一圈的纪录
                    x_add = xg + 1;
                    x_reduce = xg - 1;
                    y_add = yg + 1;
                    y_reduce = yg - 1;
                    if (erzhi_b[y_add][x_reduce] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_add;
                        area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                        erzhi_b[y_add][x_reduce] = connect_area_num;
                    }
                    if (erzhi_b[y_add][xg] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_add;
                        area_grow_data_arex[area_grow_data_arel++] = xg; // 当前次生长的一圈的纪录
                        erzhi_b[y_add][xg] = connect_area_num;
                    }
                    if (erzhi_b[y_add][x_add] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_add;
                        area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                        erzhi_b[y_add][x_add] = connect_area_num;
                    }
                    if (erzhi_b[yg][x_reduce] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = yg;
                        area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                        erzhi_b[yg][x_reduce] = connect_area_num;
                    }
                    //	||
                    //((area_grow_dataA[y+0][x+0]==1)&&(abs(dataHSV[y+0][x+0][0]-tempH)<5)&&(abs(dataHSV[y+0][x+0][2]-tempV)<30)&&(abs(dataHSV[y+0][x+0][1]-tempS)<40))
                    if (erzhi_b[yg][x_add] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = yg;
                        area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                        erzhi_b[yg][x_add] = connect_area_num;
                    }
                    if (erzhi_b[y_reduce][x_reduce] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_reduce;
                        area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                        erzhi_b[y_reduce][x_reduce] = connect_area_num;
                    }
                    if (erzhi_b[y_reduce][xg] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_reduce;
                        area_grow_data_arex[area_grow_data_arel++] = xg; // 当前次生长的一圈的纪录
                        erzhi_b[y_reduce][xg] = connect_area_num;
                    }
                    if (erzhi_b[y_reduce][x_add] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_reduce;
                        area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                        erzhi_b[y_reduce][x_add] =
                            connect_area_num; ////////////////////////////////////////////改区域生长 加参数
                    }
                    j++;
                }
                connect_area_layer_e = area_grow_data_arel;
                connect_area_layer_l = connect_area_layer_e - connect_area_layer_s; // 连通域重量+当前一圈的点数
                connect_area_layer_ps = connect_area_layer_s;                       // 存储连通域 上层的 首
                connect_area_layer_pe = connect_area_layer_e;                       // 存储连通域 上层的 尾

                connect_area_w += connect_area_layer_l;
                if (connect_area_layer_l == 0)
                    break;
            }
            // 生长完成
            // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
            connect_area_s_e_w[connect_area_num][1] =
                area_grow_data_arel; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                     // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
            connect_area_s_e_w[connect_area_num][2] =
                connect_area_w; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0] [尾]connect_area_s_e[n][1]
                                // [重量]connect_area_s_e[n][2]
            if (connect_max_area_w < connect_area_w)
            {
                connect_max_area_w = connect_area_w;
                connect_max_area_num = connect_area_num;
            }
        }

        //	}
        //}
        i++;
    }

    //	for (i=0;i<area_grow_data_arel;i++)//测试代码
    //	{
    //		data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+0]=1;//测试代码//显示种子点
    //		data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+1]=254;//测试代码
    //		data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+2]=1;//测试代码
    //		m_VideoOCX.Show(m_Image);//测试代码				// display capture frame
    //		Sleep(200);
    //	}//测试代码

    // 不进行输出了 输出\\掉了
    // for (i=0;i<x320*y240;i++)
    //{	data_C_out[i]=0;
    // }
    // if (max_area_w>0)//(max_area_num>0)//////////////////////////////////////////////////////////////////
    //{	for (l=0;l<area_grow_data_areml;l++)
    //	{	data_C_out[area_grow_data_aremy[l]*320+area_grow_data_aremx[l]]=1;
    //	}
    // }
    /*
    i=0;while(i<circle_mask_long)
    {
    y=circle_maskY[i];
    x=circle_maskX[i];
    //	SetColor(x,y,cPointB);
        SetColor(y,x,cPointB);
    i++;
    }
    if (max_area_w>0)//(max_area_num>0)//////////////////////////////////////////////////////////////////
    {	for (l=0;l<area_grow_data_areml;l++)
        {
    //		SetColor(area_grow_data_aremx[l],area_grow_data_aremy[l],cPointG);
            SetColor(area_grow_data_aremy[l],area_grow_data_aremx[l],cPointG);
        }
    }
    */

    ////////////////////////////////////////////////////////////////////////////////////
    *obj_weight_map =
        connect_area_s_e_w[connect_max_area_num][2]; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                                     // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]

    // 带出黑白图象重心横坐标  obj_properties *All  All.x结果的重心的x坐标 All.y结果的重心的y坐标
    // 需要处理部分的掩膜数组  int *yan_mo_x,int *yan_mo_y
    // 需要处理部分的掩膜数组长度int yan_mo_long,
    register float OBJ_temp_b = 0;
    register float OBJ_temp_b_ = 0;
    register float OBJ_temp_c = 0;
    register float ball_op_x = 0;
    register float ball_op_y = 0;
    register float ball_op_weight = 0;
    if (connect_max_area_num > 1) // 最大连通域的编号	默认无效值1
    {
        for (i = connect_area_s_e_w[connect_max_area_num][0]; i < connect_area_s_e_w[connect_max_area_num][1]; i++)
        { //	SetColor(area_grow_data_arey[i],area_grow_data_arex[i],cPointB);
            OBJ_temp_c++;
            OBJ_temp_b += area_grow_data_arex[i];
            OBJ_temp_b_ += area_grow_data_arey[i];
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+0]=254;//测试代码//显示种子点
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+1]=254;//测试代码
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+2]=254;//测试代码
            //	SetColor(y,x,cPointG);//测试代码
        }
    }
    // m_VideoOCX.Show(m_Image);//测试代码				// display capture frame

    if (OBJ_temp_c != 0) // 保险程序  当同步不佳时  有时Capture不到图象 就会出现所有象素都为零 这时做OBJ_temp_b /
                         // OBJ_temp_c的除法就会产生分母为零的错误
    {
        ball_op_x = OBJ_temp_b / OBJ_temp_c;  //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+320
        ball_op_y = OBJ_temp_b_ / OBJ_temp_c; //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+240
    }
    ball_op_weight = OBJ_temp_c; // 准备把象素重量传给主逻辑函数
    //					HSI11->ball_x=ball_op_x;
    //					HSI11->ball_y=ball_op_y;
    //					HSI11->ball_zhong=ball_op_weight;
    HSI11->ball_x = ball_op_y;
    HSI11->ball_y = ball_op_x;
    HSI11->ball_zhong = ball_op_weight;
    jizuobiao(HSI11->ball_x, HSI11->ball_y, &HSI11->ball_jiao, &HSI11->ball_chang, 320, 240);

    // 完成二值化图像标重心程序///////////////////////////////////////////////////////////////////

    // 兼容鑫鑫 回存连同域数据
    memset(erzhi_b, 0, OMNI_VISION_CAM_HEIGHT * OMNI_VISION_CAM_WIDTH * sizeof(int));
    if (connect_max_area_num > 1) // 最大连通域的编号	默认无效值1
    {
        for (i = connect_area_s_e_w[connect_max_area_num][0]; i < connect_area_s_e_w[connect_max_area_num][1]; i++)
        {
            erzhi_b[area_grow_data_arey[i]][area_grow_data_arex[i]] = 1;
        }
    }
    RGBTRIPLE cPoint;
    cPoint.rgbtBlue = 0x00;
    cPoint.rgbtGreen = 0x00;
    cPoint.rgbtRed = 0x7f;
    if (display_process_image == 1) // 判断 显示过程图像	//显示最大连通域
        for (y = 0; y < OMNI_VISION_CAM_HEIGHT; y++)
            for (x = 0; x < OMNI_VISION_CAM_WIDTH; x++)
            {
                if (erzhi_b[y][x] == 1)
                {
                    SetColor(y, x, cPoint); // 测试代码
                }
            }
}

char close_green_Bool_Data[CS_OMNI_VISION_CAM_HEIGHT][CS_OMNI_VISION_CAM_WIDTH]; // 膨胀过的绿色的数据

// #define Cross_Ling_Add//使用十字线的方法扩边
// #define GROW_LONG 8//定义 使用十字线的扩边长度  是十字线一端到另一端的点数   注：只有#define Cross_Ling_add时有效

#define Box_Add // 使用 画圈的 方法扩边 代码要自己编 只提供上下左右长度各8个点的
const char GROW_BOX_LONG =
    10; // 定义 使用十字线的扩边长度  是十字线一端到另一端的点数   注：只有#define Cross_Ling_add时有效
const char GROW_BOX_LONG_ =
    GROW_BOX_LONG - 1; // 定义 使用十字线的扩边长度  是十字线一端到另一端的点数   注：只有#define Cross_Ling_add时有效

#define Cross_Ling_add      // 使用十字线的方法扩边
#define use_green_grow_mask // 使用绿色省长部分掩膜
#ifdef use_green_grow_mask
char close_green_grow_Data[CS_OMNI_VISION_CAM_HEIGHT][CS_OMNI_VISION_CAM_WIDTH];  // 膨胀过的绿色的数据 膨胀程序加的区域
int close_green_grow_maskY[CS_OMNI_VISION_CAM_HEIGHT * CS_OMNI_VISION_CAM_WIDTH]; // 膨胀过的绿色的数据 掩膜Y
int close_green_grow_maskX[CS_OMNI_VISION_CAM_HEIGHT * CS_OMNI_VISION_CAM_WIDTH]; // 膨胀过的绿色的数据 掩膜X
int close_green_grow_maskL;                                                       // 膨胀过的绿色的数据 掩膜长度
#endif
// unsigned int
// area_grow_data_arex[CS_OMNI_VISION_CAM_HEIGHT*CS_OMNI_VISION_CAM_WIDTH];//[640*480];//currently当前生长的一圈的纪录
// unsigned int
// area_grow_data_arey[CS_OMNI_VISION_CAM_HEIGHT*CS_OMNI_VISION_CAM_WIDTH];//[640*480];//currently当前生长的一圈的纪录
void UPRColor::bool_max_connectivity_analyze2_1green(float *obj_weight_map)
{
#ifdef use_green_grow_mask
    close_green_grow_maskL = 0; // 膨胀过的绿色的数据 掩膜长度
#endif
    register unsigned long int area_grow_data_arel = 0; // 纪录currently当前这一圈有几个像素
    RGBTRIPLE cPointG;
    cPointG.rgbtBlue = 0x00;
    cPointG.rgbtGreen = 0xFF;
    cPointG.rgbtRed = 0x00;
    RGBTRIPLE cPointB;
    cPointB.rgbtBlue = 0xFF;
    cPointB.rgbtGreen = 0x00;
    cPointB.rgbtRed = 0x00;

    // const char H_min_diff		=8;	//这是一个可调值关系到颜色的分辨粗略程度 如15以上黄色和橘黄就分不开了
    // const int grow_area_times =12000;//整个生成 生长区的次数//5000 要1min 40s
    // const int grow_min_weight	=40;//30如果单个生长区重量大于grow_min_weight 则把纪录A存入C[n]
    const int x320 = CS_OMNI_VISION_CAM_WIDTH; // 320;//320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240 = CS_OMNI_VISION_CAM_HEIGHT; // 240;//240;//处理图像数据的大小设置
    const int x320r1 = x320 - 1;                // 320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240r1 = y240 - 1;                // 240;//处理图像数据的大小设置

    register int i, j, k, l, x, y; // seedx,seedy,,used_num
    // int x_grow,y_grow;
    // static unsigned char  area_grow_dataC[y240][x320];// 标号输出
    // register float  circumscription_weight;
    // register float  max_area_w;//最大的连通域的重量
    // register int    max_area_num;//最大的连通域的标号
    register int x_add, x_reduce, y_add, y_reduce, xg, yg;

    //	connect_area_l=0;
    register int connect_area_num =
        1; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点 2连通域的第一个编号 3 4 5....
    register int connect_max_area_num = 1;  // 最大连通域的编号	默认无效值1
    register int connect_area_w = 0;        // 记录连通域重量值
    register int connect_max_area_w = 0;    // 记录连通域最大重量值
    int connect_area_s_e_w[65536][3];       // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                            // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
    register int connect_area_layer_l = 0;  // 存储连通域 层的 点数 长度
    register int connect_area_layer_s = 0;  // 存储连通域 层的 首
    register int connect_area_layer_e = 0;  // 存储连通域 层的 尾
    register int connect_area_layer_pl = 0; // 存储连通域 层的 点数 长度
    register int connect_area_layer_ps = 0; // 存储连通域 上层的 首
    register int connect_area_layer_pe = 0; // 存储连通域 上层的 尾

    // used_num  =1;
    // max_area_w=0;
    // seedx=0;////////////////////
    // seedy=0;////////////////////
    i = 0;
    while (i < circle_mask_long)
    {
        y = circle_maskY[i];
        x = circle_maskX[i];
        close_green_Bool_Data[y][x] = 0; // 膨胀过的绿色的数据
#ifdef use_green_grow_mask
        close_green_grow_Data[y][x] = 0; // 膨胀过的绿色的数据 膨胀程序加的区域
#endif
        i++;
    }
    // 膨胀
    i = 0;
    while (i < circle_mask_long)
    {
        y = circle_maskY[i];
        x = circle_maskX[i];
        //	lianxu[i][j]=0;
        if (erzhi_di[y][x] == 1) //&& banjing[i][j]<50000
        {
            if ((erzhi_di[y - 1][x] != 1 || erzhi_di[y + 1][x] != 1 || erzhi_di[y][x - 1] != 1 ||
                 erzhi_di[y][x + 1] != 1))
            // SetColor(i,j,dimian);
            {
#ifdef Cross_Ling_Add                                     // 使用十字线的方法扩边
                for (j = -GROW_LONG; j <= GROW_LONG; j++) // 十字线扩展长度
                {
                    close_green_Bool_Data[y + j][x] = 1;
                    close_green_Bool_Data[y][x + j] = 1;
                    //	SetColor(i+pengzhangdian,j,dimian);
                    //	SetColor(i,j+pengzhangdian,dimian);
                    //							data[((y+j)*320+x)*3+0]=1;//测试代码//显示种子点
                    //							data[((y+j)*320+x)*3+1]=254;//测试代码
                    //							data[((y+j)*320+x)*3+2]=1;//测试代码
                    //							data[((y)*320+x+j)*3+0]=1;//测试代码//显示种子点
                    //							data[((y)*320+x+j)*3+1]=254;//测试代码
                    //							data[((y)*320+x+j)*3+2]=1;//测试代码
#ifdef use_green_grow_mask
                    close_green_grow_Data[y + j][x] = 1;
                    close_green_grow_Data[y][x + j] = 1;
#endif
                }
#endif
#ifdef Box_Add // 使用 画圈的 方法扩边 代码要自己编 只提供上下左右长度各8个点的
                close_green_Bool_Data[y][x] = 1;
                for (k = GROW_BOX_LONG_, j = 1; j <= GROW_BOX_LONG; j++, k--) // 十字线扩展长度
                {
                    close_green_Bool_Data[y + k][x + j] = 1;
                    close_green_Bool_Data[y - j][x + k] = 1;
                    close_green_Bool_Data[y - k][x - j] = 1;
                    close_green_Bool_Data[y + j][x - k] = 1;
#ifdef use_green_grow_mask
                    close_green_grow_Data[y + k][x + j] = 1;
                    close_green_grow_Data[y - j][x + k] = 1;
                    close_green_grow_Data[y - k][x - j] = 1;
                    close_green_grow_Data[y + j][x - k] = 1;
#endif
                    //	data[((y+k)*320+(x+j))*3+1]=254;//测试代码
                    //	data[((y-j)*320+(x+k))*3+1]=254;//测试代码
                    //	data[((y-k)*320+(x-j))*3+1]=254;//测试代码
                    //	data[((y+j)*320+(x-k))*3+1]=254;//测试代码
                }
#endif
            }
            else
            {
                close_green_Bool_Data[y][x] = 1;
                //	SetColor(i,j,dimian);
                //						data[(y*320+x)*3+0]=1;//测试代码//显示种子点
                //						data[(y*320+x)*3+1]=254;//测试代码
                //						data[(y*320+x)*3+2]=1;//测试代码
            }
        }
        i++;
    }
    // m_VideoOCX.Show(m_Image);//测试代码				// display capture frame
    /*
                                close_green_Bool_Data[y][x]=1;
                                close_green_Bool_Data[y+2][x]=1;
                                close_green_Bool_Data[y+1][x]=1;
                                close_green_Bool_Data[y+0][x]=1;
                                close_green_Bool_Data[y-1][x]=1;
                                close_green_Bool_Data[y-2][x]=1;
                                close_green_Bool_Data[y][x+2]=1;
                                close_green_Bool_Data[y][x+1]=1;
                                close_green_Bool_Data[y][x+0]=1;
                                close_green_Bool_Data[y][x-1]=1;
                                close_green_Bool_Data[y][x-2]=1;
    */
    ////////////////////////////////////
    // 黑边处理  防止出现边缘冒了//////
    for (x = 0; x < x320; x++)
    {
        close_green_Bool_Data[0][x] = 0;
        close_green_Bool_Data[y240 - 1][x] = 0;
    }
    for (y = 1; y < y240 - 1; y++)
    {
        close_green_Bool_Data[y][0] = 0;
        close_green_Bool_Data[y][x320 - 1] = 0;
    }
    // 黑边处理  防止出现边缘冒了//////

    area_grow_data_arel = 0; // 所有连通域纪录 位置置零
    // 1自动设置这次生长的起点种子
    // for(i=0;i<circle_mask_long;i++)
    i = 0;
    while (i < circle_mask_long)
    {
        y = circle_maskY[i];
        x = circle_maskX[i];
        if (close_green_Bool_Data[y][x] == 1) // 没生长过 且是有效点
        {                                     // seedx=x;
            // seedy=y;
            // 2开始生长
            connect_area_num++; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点 2连通域的第一个编号 3
                                // 4 5....
            connect_area_w = 0; // 记录连通域重量值	清零
            connect_area_s_e_w[connect_area_num][0] =
                area_grow_data_arel; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                     // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
            // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
            // 1设置这次生长的起点种子
            area_grow_data_arex[area_grow_data_arel] = x;   // 当前次生长的一圈的纪录
            area_grow_data_arey[area_grow_data_arel++] = y; // 当前次生长的一圈的纪录
            connect_area_layer_l = 1;
            connect_area_layer_pe = connect_area_layer_ps + 1;
            //    		area_grow_dataAtpx[0]=x;//seedx;////手动设置种子
            //    		area_grow_dataAtpy[0]=y;//seedy;////手动设置种子
            //    		area_grow_dataAtpl=1;////手动设置种子
            connect_area_w++; // 连通域重量+1

            // 开始生长
            while (1) // for (i=0;i<1500;i++)//生成生长区域 的生长次数
            {
                connect_area_layer_s = area_grow_data_arel;
                j = connect_area_layer_ps;
                while (j < connect_area_layer_pe)
                {                                // 周围不是A中的有效点且符合生长规则
                    xg = area_grow_data_arex[j]; // 当前次生长的一圈的纪录
                    yg = area_grow_data_arey[j]; // 当前次生长的一圈的纪录
                    x_add = xg + 1;
                    x_reduce = xg - 1;
                    y_add = yg + 1;
                    y_reduce = yg - 1;
                    if (close_green_Bool_Data[y_add][x_reduce] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_add;
                        area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                        close_green_Bool_Data[y_add][x_reduce] = connect_area_num;
                    }
                    if (close_green_Bool_Data[y_add][xg] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_add;
                        area_grow_data_arex[area_grow_data_arel++] = xg; // 当前次生长的一圈的纪录
                        close_green_Bool_Data[y_add][xg] = connect_area_num;
                    }
                    if (close_green_Bool_Data[y_add][x_add] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_add;
                        area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                        close_green_Bool_Data[y_add][x_add] = connect_area_num;
                    }
                    if (close_green_Bool_Data[yg][x_reduce] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = yg;
                        area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                        close_green_Bool_Data[yg][x_reduce] = connect_area_num;
                    }
                    //	||
                    //((area_grow_dataA[y+0][x+0]==1)&&(abs(dataHSV[y+0][x+0][0]-tempH)<5)&&(abs(dataHSV[y+0][x+0][2]-tempV)<30)&&(abs(dataHSV[y+0][x+0][1]-tempS)<40))
                    if (close_green_Bool_Data[yg][x_add] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = yg;
                        area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                        close_green_Bool_Data[yg][x_add] = connect_area_num;
                    }
                    if (close_green_Bool_Data[y_reduce][x_reduce] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_reduce;
                        area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                        close_green_Bool_Data[y_reduce][x_reduce] = connect_area_num;
                    }
                    if (close_green_Bool_Data[y_reduce][xg] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_reduce;
                        area_grow_data_arex[area_grow_data_arel++] = xg; // 当前次生长的一圈的纪录
                        close_green_Bool_Data[y_reduce][xg] = connect_area_num;
                    }
                    if (close_green_Bool_Data[y_reduce][x_add] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_reduce;
                        area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                        close_green_Bool_Data[y_reduce][x_add] =
                            connect_area_num; ////////////////////////////////////////////改区域生长 加参数
                    }
                    j++;
                }
                connect_area_layer_e = area_grow_data_arel;
                connect_area_layer_l = connect_area_layer_e - connect_area_layer_s; // 连通域重量+当前一圈的点数
                connect_area_layer_ps = connect_area_layer_s;                       // 存储连通域 上层的 首
                connect_area_layer_pe = connect_area_layer_e;                       // 存储连通域 上层的 尾

                connect_area_w += connect_area_layer_l;
                if (connect_area_layer_l == 0)
                    break;
            }
            // 生长完成
            // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
            connect_area_s_e_w[connect_area_num][1] =
                area_grow_data_arel; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                     // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
            connect_area_s_e_w[connect_area_num][2] =
                connect_area_w; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0] [尾]connect_area_s_e[n][1]
                                // [重量]connect_area_s_e[n][2]
            if (connect_max_area_w < connect_area_w)
            {
                connect_max_area_w = connect_area_w;
                connect_max_area_num = connect_area_num;
            }
        }

        //	}
        //}
        i++;
    }

    //	for (i=0;i<area_grow_data_arel;i++)//测试代码
    //	{
    //		data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+0]=254;//测试代码//显示种子点
    //		data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+1]=1;//测试代码
    //		data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+2]=1;//测试代码
    //		m_VideoOCX.Show(m_Image);//测试代码				// display capture frame
    //		Sleep(200);
    //	}//测试代码

    // 不进行输出了 输出\\掉了
    // for (i=0;i<x320*y240;i++)
    //{	data_C_out[i]=0;
    // }
    // if (max_area_w>0)//(max_area_num>0)//////////////////////////////////////////////////////////////////
    //{	for (l=0;l<area_grow_data_areml;l++)
    //	{	data_C_out[area_grow_data_aremy[l]*320+area_grow_data_aremx[l]]=1;
    //	}
    // }
    /*
    i=0;while(i<circle_mask_long)
    {
    y=circle_maskY[i];
    x=circle_maskX[i];
    //	SetColor(x,y,cPointB);
        SetColor(y,x,cPointB);
    i++;
    }
    if (max_area_w>0)//(max_area_num>0)//////////////////////////////////////////////////////////////////
    {	for (l=0;l<area_grow_data_areml;l++)
        {
    //		SetColor(area_grow_data_aremx[l],area_grow_data_aremy[l],cPointG);
            SetColor(area_grow_data_aremy[l],area_grow_data_aremx[l],cPointG);
        }
    }
    */

    ////////////////////////////////////////////////////////////////////////////////////
    *obj_weight_map =
        connect_area_s_e_w[connect_max_area_num][2]; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                                     // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]

    // 带出黑白图象重心横坐标  obj_properties *All  All.x结果的重心的x坐标 All.y结果的重心的y坐标
    // 需要处理部分的掩膜数组  int *yan_mo_x,int *yan_mo_y
    // 需要处理部分的掩膜数组长度int yan_mo_long,
    register float OBJ_temp_b = 0;
    register float OBJ_temp_b_ = 0;
    register float OBJ_temp_c = 0;
    register float ball_op_x = 0;
    register float ball_op_y = 0;
    register float ball_op_weight = 0;

    if (connect_max_area_num > 1) // 最大连通域的编号	默认无效值1
    {
        for (i = connect_area_s_e_w[connect_max_area_num][0]; i < connect_area_s_e_w[connect_max_area_num][1]; i++)
        { //	SetColor(area_grow_data_arey[i],area_grow_data_arex[i],cPointB);
            OBJ_temp_c++;
            x = area_grow_data_arex[i];
            y = area_grow_data_arey[i];
            OBJ_temp_b += x;
            OBJ_temp_b_ += y;
#ifdef use_green_grow_mask
            if (close_green_grow_Data[y][x] == 1)
            {
                close_green_grow_maskX[close_green_grow_maskL] = x;
                close_green_grow_maskY[close_green_grow_maskL++] = y;
            }
#endif
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+0]=254;//测试代码//显示种子点
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+1]=254;//测试代码
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+2]=254;//测试代码
            //	SetColor(y,x,cPointG);//测试代码
        }
    }

    if (display_process_image == 1)   // 判断 显示过程图像	//显示最大连通域
        if (connect_max_area_num > 1) // 最大连通域的编号	默认无效值1
        {
            for (i = connect_area_s_e_w[connect_max_area_num][0]; i < connect_area_s_e_w[connect_max_area_num][1]; i++)
            {
                SetColor(area_grow_data_arey[i], area_grow_data_arex[i], cPointG);
            }
        }
    if (display_process_image == 1)                  // 判断 显示过程图像
        for (i = 0; i < close_green_grow_maskL; i++) // 测试代码//显示最大连通域 所接的扩展边缘 的点
        { // data[(close_green_grow_maskY[i]*320+close_green_grow_maskX[i])*3+0]=1;//测试代码
            // data[(close_green_grow_maskY[i]*320+close_green_grow_maskX[i])*3+1]=254;//测试代码
            // data[(close_green_grow_maskY[i]*320+close_green_grow_maskX[i])*3+2]=254;//测试代码
            SetColor(close_green_grow_maskY[i], close_green_grow_maskX[i], cPointB); // 测试代码
        }
    if (0)                        //( display_process_image == 1)//判断 显示过程图像
        for (x = 0; x < 480; x++) // 显示所有边缘数据
            for (y = 0; y < 640; y++)
            {
                if (close_green_grow_Data[y][x] == 1)
                {
                    SetColor(y, x, cPointG); // 测试代码
                }
            }

    // for (i=0;i<circle_mask_long;i++)//测试代码				//测试扩边得到的边缘
    //{	y=circle_maskY[i];	x=circle_maskX[i];//测试代码
    //	if (close_green_grow_Data [y][x]==1)//测试代码
    //	{	data[(y*320+x)*3+0]=254;//测试代码
    //		data[(y*320+x)*3+1]=1;//测试代码
    //		data[(y*320+x)*3+2]=254;//测试代码
    //	}//测试代码
    // }//测试代码

    // m_VideoOCX.Show(m_Image);//测试代码				// display capture frame

    if (OBJ_temp_c != 0) // 保险程序  当同步不佳时  有时Capture不到图象 就会出现所有象素都为零 这时做OBJ_temp_b /
                         // OBJ_temp_c的除法就会产生分母为零的错误
    {
        ball_op_x = OBJ_temp_b / OBJ_temp_c;  //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+320
        ball_op_y = OBJ_temp_b_ / OBJ_temp_c; //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+240
    }
    ball_op_weight = OBJ_temp_c; // 准备把象素重量传给主逻辑函数
    //					HSI11->ball_x=ball_op_x;
    //					HSI11->ball_y=ball_op_y;
    //					HSI11->ball_zhong=ball_op_weight;

    // 完成二值化图像标重心程序///////////////////////////////////////////////////////////////////

    // 兼容鑫鑫 回存连同域数据
    memset(erzhi_di, 0, OMNI_VISION_CAM_HEIGHT * OMNI_VISION_CAM_WIDTH * sizeof(int));
    if (connect_max_area_num > 1) // 最大连通域的编号	默认无效值1
    {
        for (i = connect_area_s_e_w[connect_max_area_num][0]; i < connect_area_s_e_w[connect_max_area_num][1]; i++)
        {
            erzhi_di[area_grow_data_arey[i]][area_grow_data_arex[i]] = 1;
        }
    }
    // if (connect_max_area_num>1)//最大连通域的编号	默认无效值1
    //{	for (i=connect_area_s_e_w[connect_max_area_num][0];i<connect_area_s_e_w[connect_max_area_num][1];i++)
    //	{	erzhi_di[area_grow_data_arey[i]][area_grow_data_arex[i]];
    // }	}

    RGBTRIPLE cPoint;
    cPoint.rgbtBlue = 0x00;
    cPoint.rgbtGreen = 0x7f;
    cPoint.rgbtRed = 0x00;
    if (display_process_image == 1) // 判断 显示过程图像	//显示最大连通域
        for (y = 0; y < OMNI_VISION_CAM_HEIGHT; y++)
            for (x = 0; x < OMNI_VISION_CAM_WIDTH; x++)
            {
                if (erzhi_di[y][x] == 1)
                {
                    SetColor(y, x, cPoint); // 测试代码
                }
            }
}

void UPRColor::bool_max_connectivity_analyze2_1_InGreen_ball(float *obj_weight_map, Yuzhi *HSI11)
{
    register unsigned long int area_grow_data_arel = 0; // 纪录currently当前这一圈有几个像素
    RGBTRIPLE cPointG;
    cPointG.rgbtBlue = 0x00;
    cPointG.rgbtGreen = 0xFF;
    cPointG.rgbtRed = 0x00;
    RGBTRIPLE cPointB;
    cPointB.rgbtBlue = 0xFF;
    cPointB.rgbtGreen = 0x00;
    cPointB.rgbtRed = 0x00;

    // const char H_min_diff		=8;	//这是一个可调值关系到颜色的分辨粗略程度 如15以上黄色和橘黄就分不开了
    // const int grow_area_times =12000;//整个生成 生长区的次数//5000 要1min 40s
    // const int grow_min_weight	=40;//30如果单个生长区重量大于grow_min_weight 则把纪录A存入C[n]
    const int x320 = CS_OMNI_VISION_CAM_WIDTH; // 320;//320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240 = CS_OMNI_VISION_CAM_HEIGHT; // 240;//240;//处理图像数据的大小设置
    const int x320r1 = x320 - 1;                // 320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240r1 = y240 - 1;                // 240;//处理图像数据的大小设置

    register int i, j, l, x, y; // seedx,seedy,,used_num
    // int x_grow,y_grow;
    // static unsigned char  area_grow_dataC[y240][x320];// 标号输出
    // register float  circumscription_weight;
    // register float  max_area_w;//最大的连通域的重量
    // register int    max_area_num;//最大的连通域的标号
    register int x_add, x_reduce, y_add, y_reduce, xg, yg;

    //	connect_area_l=0;
    register int connect_area_num =
        1; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点 2连通域的第一个编号 3 4 5....
    register int connect_max_area_num = 1;  // 最大连通域的编号	默认无效值1
    register int connect_area_w = 0;        // 记录连通域重量值
    register int connect_max_area_w = 0;    // 记录连通域最大重量值
    int connect_area_s_e_w[65536][3];       // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                            // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
    register int connect_area_layer_l = 0;  // 存储连通域 层的 点数 长度
    register int connect_area_layer_s = 0;  // 存储连通域 层的 首
    register int connect_area_layer_e = 0;  // 存储连通域 层的 尾
    register int connect_area_layer_pl = 0; // 存储连通域 层的 点数 长度
    register int connect_area_layer_ps = 0; // 存储连通域 上层的 首
    register int connect_area_layer_pe = 0; // 存储连通域 上层的 尾

    // used_num  =1;
    // max_area_w=0;
    // seedx=0;////////////////////
    // seedy=0;////////////////////

    ////////////////////////////////////
    // 黑边处理  防止出现边缘冒了//////
    for (x = 0; x < x320; x++)
    {
        erzhi_b[x][0] = 0;
        erzhi_b[x][y240 - 1] = 0;
    }
    for (y = 1; y < y240 - 1; y++)
    {
        erzhi_b[0][y] = 0;
        erzhi_b[x320 - 1][y] = 0;
    }
    // 黑边处理  防止出现边缘冒了//////

    area_grow_data_arel = 0; // 所有连通域纪录 位置置零
    // 1自动设置这次生长的起点种子
    // for(i=0;i<close_green_grow_maskL;i++)
    i = 0;
    while (i < close_green_grow_maskL)
    {
        y = close_green_grow_maskY[i];
        x = close_green_grow_maskX[i];
        if (erzhi_b[y][x] == 1) // 没生长过 且是有效点
        {                       // seedx=x;
            // seedy=y;
            // 2开始生长
            connect_area_num++; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点 2连通域的第一个编号 3
                                // 4 5....
            connect_area_w = 0; // 记录连通域重量值	清零
            connect_area_s_e_w[connect_area_num][0] =
                area_grow_data_arel; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                     // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
            // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
            // 1设置这次生长的起点种子
            area_grow_data_arex[area_grow_data_arel] = x;   // 当前次生长的一圈的纪录
            area_grow_data_arey[area_grow_data_arel++] = y; // 当前次生长的一圈的纪录
            connect_area_layer_l = 1;
            connect_area_layer_pe = connect_area_layer_ps + 1;
            //    		area_grow_dataAtpx[0]=x;//seedx;////手动设置种子
            //    		area_grow_dataAtpy[0]=y;//seedy;////手动设置种子
            //    		area_grow_dataAtpl=1;////手动设置种子
            connect_area_w++; // 连通域重量+1

            // 开始生长
            while (1) // for (i=0;i<1500;i++)//生成生长区域 的生长次数
            {
                connect_area_layer_s = area_grow_data_arel;
                j = connect_area_layer_ps;
                while (j < connect_area_layer_pe)
                {                                // 周围不是A中的有效点且符合生长规则
                    xg = area_grow_data_arex[j]; // 当前次生长的一圈的纪录
                    yg = area_grow_data_arey[j]; // 当前次生长的一圈的纪录
                    x_add = xg + 1;
                    x_reduce = xg - 1;
                    y_add = yg + 1;
                    y_reduce = yg - 1;
                    if (erzhi_b[y_add][x_reduce] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_add;
                        area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                        erzhi_b[y_add][x_reduce] = connect_area_num;
                    }
                    if (erzhi_b[y_add][xg] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_add;
                        area_grow_data_arex[area_grow_data_arel++] = xg; // 当前次生长的一圈的纪录
                        erzhi_b[y_add][xg] = connect_area_num;
                    }
                    if (erzhi_b[y_add][x_add] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_add;
                        area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                        erzhi_b[y_add][x_add] = connect_area_num;
                    }
                    if (erzhi_b[yg][x_reduce] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = yg;
                        area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                        erzhi_b[yg][x_reduce] = connect_area_num;
                    }
                    //	||
                    //((area_grow_dataA[y+0][x+0]==1)&&(abs(dataHSV[y+0][x+0][0]-tempH)<5)&&(abs(dataHSV[y+0][x+0][2]-tempV)<30)&&(abs(dataHSV[y+0][x+0][1]-tempS)<40))
                    if (erzhi_b[yg][x_add] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = yg;
                        area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                        erzhi_b[yg][x_add] = connect_area_num;
                    }
                    if (erzhi_b[y_reduce][x_reduce] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_reduce;
                        area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                        erzhi_b[y_reduce][x_reduce] = connect_area_num;
                    }
                    if (erzhi_b[y_reduce][xg] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_reduce;
                        area_grow_data_arex[area_grow_data_arel++] = xg; // 当前次生长的一圈的纪录
                        erzhi_b[y_reduce][xg] = connect_area_num;
                    }
                    if (erzhi_b[y_reduce][x_add] == 1)
                    {
                        area_grow_data_arey[area_grow_data_arel] = y_reduce;
                        area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                        erzhi_b[y_reduce][x_add] =
                            connect_area_num; ////////////////////////////////////////////改区域生长 加参数
                    }
                    j++;
                }
                connect_area_layer_e = area_grow_data_arel;
                connect_area_layer_l = connect_area_layer_e - connect_area_layer_s; // 连通域重量+当前一圈的点数
                connect_area_layer_ps = connect_area_layer_s;                       // 存储连通域 上层的 首
                connect_area_layer_pe = connect_area_layer_e;                       // 存储连通域 上层的 尾

                connect_area_w += connect_area_layer_l;
                if (connect_area_layer_l == 0)
                    break;
            }
            // 生长完成
            // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
            connect_area_s_e_w[connect_area_num][1] =
                area_grow_data_arel; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                     // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
            connect_area_s_e_w[connect_area_num][2] =
                connect_area_w; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0] [尾]connect_area_s_e[n][1]
                                // [重量]connect_area_s_e[n][2]
            if (connect_max_area_w < connect_area_w)
            {
                connect_max_area_w = connect_area_w;
                connect_max_area_num = connect_area_num;
            }
        }

        //	}
        //}
        i++;
    }

    //	for (i=0;i<area_grow_data_arel;i++)//测试代码
    //	{
    //		data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+0]=1;//测试代码//显示种子点
    //		data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+1]=254;//测试代码
    //		data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+2]=1;//测试代码
    //		m_VideoOCX.Show(m_Image);//测试代码				// display capture frame
    //		Sleep(200);
    //	}//测试代码

    // 不进行输出了 输出\\掉了
    // for (i=0;i<x320*y240;i++)
    //{	data_C_out[i]=0;
    // }
    // if (max_area_w>0)//(max_area_num>0)//////////////////////////////////////////////////////////////////
    //{	for (l=0;l<area_grow_data_areml;l++)
    //	{	data_C_out[area_grow_data_aremy[l]*320+area_grow_data_aremx[l]]=1;
    //	}
    // }
    /*
    i=0;while(i<close_green_grow_maskL)
    {
    y=close_green_grow_maskY[i];
    x=close_green_grow_maskX[i];
    //	SetColor(x,y,cPointB);
        SetColor(y,x,cPointB);
    i++;
    }
    if (max_area_w>0)//(max_area_num>0)//////////////////////////////////////////////////////////////////
    {	for (l=0;l<area_grow_data_areml;l++)
        {
    //		SetColor(area_grow_data_aremx[l],area_grow_data_aremy[l],cPointG);
            SetColor(area_grow_data_aremy[l],area_grow_data_aremx[l],cPointG);
        }
    }
    */

    ////////////////////////////////////////////////////////////////////////////////////
    *obj_weight_map =
        connect_area_s_e_w[connect_max_area_num][2]; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                                     // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]

    // 带出黑白图象重心横坐标  obj_properties *All  All.x结果的重心的x坐标 All.y结果的重心的y坐标
    // 需要处理部分的掩膜数组  int *yan_mo_x,int *yan_mo_y
    // 需要处理部分的掩膜数组长度int yan_mo_long,
    register float OBJ_temp_b = 0;
    register float OBJ_temp_b_ = 0;
    register float OBJ_temp_c = 0;
    register float ball_op_x = 0;
    register float ball_op_y = 0;
    register float ball_op_weight = 0;
    if (connect_max_area_num > 1) // 最大连通域的编号	默认无效值1
    {
        for (i = connect_area_s_e_w[connect_max_area_num][0]; i < connect_area_s_e_w[connect_max_area_num][1]; i++)
        { //	SetColor(area_grow_data_arey[i],area_grow_data_arex[i],cPointB);
            OBJ_temp_c++;
            OBJ_temp_b += area_grow_data_arex[i];
            OBJ_temp_b_ += area_grow_data_arey[i];
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+0]=254;//测试代码//显示种子点
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+1]=254;//测试代码
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+2]=254;//测试代码
            //	SetColor(y,x,cPointG);//测试代码
        }
    }
    // m_VideoOCX.Show(m_Image);//测试代码				// display capture frame
    if (display_process_image == 1)   // 判断 显示过程图像	//显示最大连通域
        if (connect_max_area_num > 1) // 最大连通域的编号	默认无效值1
        {
            RGBTRIPLE cPoint;
            cPoint.rgbtBlue = 0x00;
            cPoint.rgbtGreen = 0x00;
            cPoint.rgbtRed = 0xFF;
            for (i = connect_area_s_e_w[connect_max_area_num][0]; i < connect_area_s_e_w[connect_max_area_num][1]; i++)
            {
                SetColor(area_grow_data_arey[i], area_grow_data_arex[i], cPoint); // 测试代码
            }
        } // 测试代码

    if (OBJ_temp_c != 0) // 保险程序  当同步不佳时  有时Capture不到图象 就会出现所有象素都为零 这时做OBJ_temp_b /
                         // OBJ_temp_c的除法就会产生分母为零的错误
    {
        ball_op_x = OBJ_temp_b / OBJ_temp_c;  //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+320
        ball_op_y = OBJ_temp_b_ / OBJ_temp_c; //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+240
    }
    ball_op_weight = OBJ_temp_c; // 准备把象素重量传给主逻辑函数
    //					HSI11->ball_x=ball_op_x;
    //					HSI11->ball_y=ball_op_y;
    //					HSI11->ball_zhong=ball_op_weight;
    HSI11->ball_x = ball_op_y;
    HSI11->ball_y = ball_op_x;
    HSI11->ball_zhong = ball_op_weight;
    jizuobiao(HSI11->ball_x, HSI11->ball_y, &HSI11->ball_jiao, &HSI11->ball_chang, 320, 240);

    // 完成二值化图像标重心程序///////////////////////////////////////////////////////////////////

    // 兼容鑫鑫 回存连同域数据
    memset(erzhi_b, 0, OMNI_VISION_CAM_HEIGHT * OMNI_VISION_CAM_WIDTH * sizeof(int));
    if (connect_max_area_num > 1) // 最大连通域的编号	默认无效值1
    {
        for (i = connect_area_s_e_w[connect_max_area_num][0]; i < connect_area_s_e_w[connect_max_area_num][1]; i++)
        {
            erzhi_b[area_grow_data_arey[i]][area_grow_data_arex[i]] = 1;
        }
    }
    RGBTRIPLE cPoint;
    cPoint.rgbtBlue = 0x00;
    cPoint.rgbtGreen = 0x00;
    cPoint.rgbtRed = 0x7f;
    if (display_process_image == 1) // 判断 显示过程图像	//显示最大连通域
        for (y = 0; y < OMNI_VISION_CAM_HEIGHT; y++)
            for (x = 0; x < OMNI_VISION_CAM_WIDTH; x++)
            {
                if (erzhi_b[y][x] == 1)
                {
                    SetColor(y, x, cPoint); // 测试代码
                }
            }
}

// 使用掩膜circle_maskX[circle_mask_long] circle_maskY[circle_mask_long]进行 需要区域的颜色域转换
BOOL UPRColor::GetHSIFix2(Yuzhi *zz)
{
    // 取球和两个门的重心坐标，并对相应的2值化数组赋值。
    // 返回：球和两个门的直角坐标以及极坐标。
    // 只对全景（中间方块范围内）的区域做。

    RGBTRIPLE door1;
    door1.rgbtBlue = 0xFF;
    door1.rgbtGreen = 0x00;
    door1.rgbtRed = 0x00;
    RGBTRIPLE ball;
    ball.rgbtBlue = 0x00;
    ball.rgbtGreen = 0x00;
    ball.rgbtRed = 0xFF;
    RGBTRIPLE door2;
    door2.rgbtBlue = 0x88;
    door2.rgbtGreen = 0xFF;
    door2.rgbtRed = 0xFF;
    RGBTRIPLE heimo;
    heimo.rgbtBlue = 0x00;
    heimo.rgbtGreen = 0x00;
    heimo.rgbtRed = 0x00;
    HSIParam *hsi = new HSIParam;
    RGBTRIPLE dimian;
    dimian.rgbtBlue = 0x00;
    dimian.rgbtGreen = 0xFF;
    dimian.rgbtRed = 0x00;
    RGBTRIPLE heise;
    heise.rgbtBlue = 0x4D;
    heise.rgbtGreen = 0x00;
    heise.rgbtRed = 0x4D;
    RGBTRIPLE baise;
    baise.rgbtBlue = 0xDD;
    baise.rgbtGreen = 0xDD;
    baise.rgbtRed = 0xFF;
    register int volh = 0;
    register int vols = 0;
    register int voli = 0;
    register int i = 0;
    register int j = 0;
    register int k = 0;
    register int l = 0;
    register int r, g, b, H, S, V, Vmin;
    // 将HSI颜色，放入HSI颜色的矩阵。
    k = 0;
    j = 0;
    for (i = 0; i < 480; i++)
    {
        SetColor(80, i, baise);
        SetColor(560, i, baise);
    }
    i = 0;

    if (zz->bhs > zz->bhx) // 切换红色 跨180度问题
    {

        l = 0;
        while (l < circle_mask_long)
        {
            i = circle_maskY[l];
            j = circle_maskX[l];
            k = (j * OMNI_VISION_CAM_WIDTH + i) * 3;

            b = m_pImageBuf[k];
            g = m_pImageBuf[k + 1];
            r = m_pImageBuf[k + 2];
            //	b = *m_pImageBuf+k;
            //	g = *m_pImageBuf+k + 1;
            //	r = *m_pImageBuf+k + 2;
            if (b > g)
            {
                if (r > b)
                {
                    V = r;
                    Vmin = g; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=g//A=0;B=(g-b);
                    if (V != Vmin)
                    {
                        H = 360 + (g - b) * 60 / (V - Vmin); // 0*60
                    }
                    else
                        H = 0;
                }
                else // V=b   Vmin=?
                {
                    if (r < g)
                    {
                        V = b;
                        Vmin = r; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=r//A=4;B=(r-g);
                        if (V != Vmin)
                            H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                        else
                            H = 0;
                    }
                    else
                    {
                        V = b;
                        Vmin = g; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=g//A=4;B=(r-g);
                        if (V != Vmin)
                            H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                        else
                            H = 0;
                    }
                }
            }
            else
            {
                if (r > g)
                {
                    V = r;
                    Vmin = b; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=b//A=0;B=(g-b);
                    if (V != Vmin)
                    {
                        H = 0 + (g - b) * 60 / (V - Vmin); // 0*60
                        if (H < 0)
                        {
                            H = H + 255; // H=H+360;
                        }
                    }
                    else
                        H = 0;
                }
                else // V=g   Vmin=?
                {
                    if (r < b)
                    {
                        V = g;
                        Vmin = r; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=r//A=2;B=(b-r);
                        if (V != Vmin)
                            H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                        else
                            H = 0;
                    }
                    else
                    {
                        V = g;
                        Vmin = b; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=b//A=2;B=(b-r);
                        if (V != Vmin)
                            H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                        else
                            H = 0;
                    }
                }
            }
            S = V - Vmin; //*255/V;    //这里改了 原来是S=(V-Vmin)*255/V
            volh = H;
            vols = S;
            voli = V;
            // if ((((volh<zz->bhs && volh>zz->bhx)  ||  (volh+360<zz->bhs && volh+360>zz->bhx) || (volh-360<zz->bhs &&
            // volh-360>zz->bhx)) && vols>zz->bs))
            if ((volh >= zz->bhx && volh <= zz->bhs) && vols >= zz->bs)
            {
                //					erzhi_fushi[i][j]=1;
                erzhi_b[i][j] = 1;
                SetColor(i, j, ball);
                //	SetColor(i-1,j,ball);
                //	SetColor(i+1,j,ball);
                //	SetColor(i,j-1,ball);
                //	SetColor(i,j+1,ball);
                erzhi_yuandi[i][j] = 0;
                erzhi_di[i][j] = 0;
                erzhi_black[i][j] = 0;
                erzhi_bai[i][j] = 0;
            }
            else if (vols <= zz->sw && voli >= zz->iw)
            {
                erzhi_bai[i][j] = 1;
                //		SetColor(i,j,baise);
                erzhi_yuandi[i][j] = 0;
                erzhi_di[i][j] = 0;
                erzhi_black[i][j] = 0;
            }

            else if ((volh <= zz->chs && volh >= zz->chx) && vols >= zz->cs && voli <= zz->cis && voli >= zz->cix)
            {
                erzhi_di[i][j] = 1;
                erzhi_yuandi[i][j] = 1;
                //		SetColor(i,j,dimian);
                erzhi_b[i][j] = 0;
                erzhi_black[i][j] = 0;
                erzhi_bai[i][j] = 0;
            }
            //	/*
            else if (vols <= zz->sh && voli <= zz->ih)
            {
                erzhi_black[i][j] = 1;
                erzhi_black[i - 1][j] = 1;
                erzhi_black[i + 1][j] = 1;
                erzhi_black[i][j - 1] = 1;
                erzhi_black[i][j + 1] = 1;

                //	SetColor(i,j,heise);
                erzhi_yuandi[i][j] = 0;
                erzhi_b[i][j] = 0;
                erzhi_di[i][j] = 0;
                erzhi_bai[i][j] = 0;
            }
            //	*/
            else
            {
                erzhi_b[i][j] = 0;
                erzhi_di[i][j] = 0;
                erzhi_yuandi[i][j] = 0;
                erzhi_black[i][j] = 0;
                erzhi_bai[i][j] = 0;
            }
            //*/
            l++;
        }
        ///*
    }
    else
    {

        int num1;
        l = 0;
        while (l < circle_mask_long)
        {
            i = circle_maskY[l];
            j = circle_maskX[l];
            k = (j * OMNI_VISION_CAM_WIDTH + i) * 3;

            b = m_pImageBuf[k];
            g = m_pImageBuf[k + 1];
            r = m_pImageBuf[k + 2];
            //	b = *m_pImageBuf+k;
            //	g = *m_pImageBuf+k + 1;
            //	r = *m_pImageBuf+k + 2;
            if (b > g)
            {
                if (r > b)
                {
                    V = r;
                    Vmin = g; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=g//A=0;B=(g-b);
                    if (V != Vmin)
                    {
                        H = 360 + (g - b) * 60 / (V - Vmin); // 0*60
                    }
                    else
                        H = 0;
                }
                else // V=b   Vmin=?
                {
                    if (r < g)
                    {
                        V = b;
                        Vmin = r; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=r//A=4;B=(r-g);
                        if (V != Vmin)
                            H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                        else
                            H = 0;
                    }
                    else
                    {
                        V = b;
                        Vmin = g; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=g//A=4;B=(r-g);
                        if (V != Vmin)
                            H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                        else
                            H = 0;
                    }
                }
            }
            else
            {
                if (r > g)
                {
                    V = r;
                    Vmin = b; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=b//A=0;B=(g-b);
                    if (V != Vmin)
                    {
                        H = 0 + (g - b) * 60 / (V - Vmin); // 0*60
                        if (H < 0)
                        {
                            H = H + 255; // H=H+360;
                        }
                    }
                    else
                        H = 0;
                }
                else // V=g   Vmin=?
                {
                    if (r < b)
                    {
                        V = g;
                        Vmin = r; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=r//A=2;B=(b-r);
                        if (V != Vmin)
                            H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                        else
                            H = 0;
                    }
                    else
                    {
                        V = g;
                        Vmin = b; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=b//A=2;B=(b-r);
                        if (V != Vmin)
                            H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                        else
                            H = 0;
                    }
                }
            }
            S = V - Vmin; //*255/V;    //这里改了 原来是S=(V-Vmin)*255/V
            volh = H;
            vols = S;
            voli = V;
            // if ((((volh<zz->bhs && volh>zz->bhx)  ||  (volh+360<zz->bhs && volh+360>zz->bhx) || (volh-360<zz->bhs &&
            // volh-360>zz->bhx)) && vols>zz->bs))
            if ((volh >= zz->bhx || volh <= zz->bhs) && vols >= zz->bs)
            {
                //					erzhi_fushi[i][j]=1;
                erzhi_b[i][j] = 1;
                SetColor(i, j, ball);
                //	SetColor(i-1,j,ball);
                //	SetColor(i+1,j,ball);
                //	SetColor(i,j-1,ball);
                //	SetColor(i,j+1,ball);
                erzhi_yuandi[i][j] = 0;
                erzhi_di[i][j] = 0;
                erzhi_black[i][j] = 0;
                erzhi_bai[i][j] = 0;
            }
            else if (vols <= zz->sw && voli >= zz->iw)
            {
                erzhi_bai[i][j] = 1;
                //		SetColor(i,j,baise);
                erzhi_yuandi[i][j] = 0;
                erzhi_di[i][j] = 0;
                erzhi_black[i][j] = 0;
            }

            else if ((volh <= zz->chs && volh >= zz->chx) && vols >= zz->cs && voli <= zz->cis && voli >= zz->cix)
            {
                erzhi_di[i][j] = 1;
                erzhi_yuandi[i][j] = 1;
                //		SetColor(i,j,dimian);
                erzhi_b[i][j] = 0;
                erzhi_black[i][j] = 0;
                erzhi_bai[i][j] = 0;
            }
            //	/*
            else if (vols <= zz->sh && voli <= zz->ih)
            {
                erzhi_black[i][j] = 1;
                erzhi_black[i - 1][j] = 1;
                erzhi_black[i + 1][j] = 1;
                erzhi_black[i][j - 1] = 1;
                erzhi_black[i][j + 1] = 1;

                //	SetColor(i,j,heise);
                erzhi_yuandi[i][j] = 0;
                erzhi_b[i][j] = 0;
                erzhi_di[i][j] = 0;
                erzhi_bai[i][j] = 0;
            }
            //	*/
            else
            {
                erzhi_b[i][j] = 0;
                erzhi_di[i][j] = 0;
                erzhi_yuandi[i][j] = 0;
                erzhi_black[i][j] = 0;
                erzhi_bai[i][j] = 0;
            }
            //*/
            l++;
        }
        ///*
    }
    /*	for(i=80;i<560;i++)
        {
            for(j=0;j<480;j++)
            {
                if (banjing[i][j]<57600)
                {
                    if(erzhi_fushi[i][j]==1 && erzhi_fushi[i-1][j-1]==1 && erzhi_fushi[i-1][j]==1  &&
       erzhi_fushi[i-1][j+1]==1  && erzhi_fushi[i][j-1]==1 && erzhi_fushi[i][j+1]==1 && erzhi_fushi[i-1][j-1]==1 &&
       erzhi_fushi[i-1][j]==1 && erzhi_fushi[i-1][j+1]==1)
                    {
                        erzhi_b[i][j]=1;
                        SetColor(i,j,ball);
                    }
                }
            }
        }
    *///*/	//完毕
    // 球2值化

    delete hsi;
    return TRUE;
}

void UPRColor::guibiqiu(Yuzhi *y)
{
    int i;
    int j;
    for (i = y->ball_x - 16; i < y->ball_x + 16; i++)
        for (j = y->ball_y - 16; j < y->ball_y + 16; j++)
        {
            erzhi_black[i][j] = 1;
        }
}

void UPRColor::DelWL(int a)
{
    /*
    int i;
    int j;
    RGBTRIPLE yanse;
    yanse.rgbtBlue=0;
    yanse.rgbtGreen=255;
    yanse.rgbtRed=0;
    for(i=0;i<640;i++)
        for(j=0;j<480;j++)
        {
            if(erzhi_bai[i][j]==1)
            {
                SetColor(i,j,yanse);
            }
        }
        */
}

void UPRColor::bianxiantouying(Yuzhi n)
{
    //	memset(erzhi_bian,0,640*480*sizeof(bool));
    RGBTRIPLE yanse;
    yanse.rgbtBlue = 255;
    yanse.rgbtGreen = 255;
    yanse.rgbtRed = 255;
    int changbian; // 长边扩大
    int duanbian;  // 短边扩大
    changbian = 20;
    duanbian = 20;
    double x;
    double y;
    double jiao;
    double chang;
    int i;
    int j;
    for (i = 0; i < 640; i++)
        for (j = 0; j < 480; j++)
            erzhi_bian[i][j] = 0;
    for (i = 0; i < 1800; i += 10)
    {
        jizuobiao(i, 0 - duanbian, &jiao, &chang, n.benjix, n.benjiy);
        jiao = -jiajiao(-jiao, n.benjijiao - 90);
        chang = fanquxian[int(chang * 2.7 / cdbl)];
        zhizuobiao(&x, &y, jiao, chang, 320, 240);

        erzhi_bian[int(x)][int(y)] = 1;
        erzhi_bian[int(x) + 1][int(y)] = 1;
        erzhi_bian[int(x)][int(y) + 1] = 1;
        erzhi_bian[int(x) + 1][int(y) + 1] = 1;
        jizuobiao(i, 1200 + duanbian, &jiao, &chang, n.benjix, n.benjiy);
        jiao = -jiajiao(-jiao, n.benjijiao - 90);
        chang = fanquxian[int(chang * 2.7 / cdbl)];
        zhizuobiao(&x, &y, jiao, chang, 320, 240);
        erzhi_bian[int(x)][int(y)] = 1;
        erzhi_bian[int(x) + 1][int(y)] = 1;
        erzhi_bian[int(x)][int(y) + 1] = 1;
        erzhi_bian[int(x) + 1][int(y) + 1] = 1;
    }
    for (i = 0; i < 500; i += 10)
    {
        jizuobiao(0 - changbian, i, &jiao, &chang, n.benjix, n.benjiy);
        jiao = -jiajiao(-jiao, n.benjijiao - 90);
        chang = fanquxian[int(chang * 2.7 / cdbl)];
        zhizuobiao(&x, &y, jiao, chang, 320, 240);
        erzhi_bian[int(x)][int(y)] = 1;
        erzhi_bian[int(x) + 1][int(y)] = 1;
        erzhi_bian[int(x)][int(y) + 1] = 1;
        erzhi_bian[int(x) + 1][int(y) + 1] = 1;
        jizuobiao(1800 + changbian, i, &jiao, &chang, n.benjix, n.benjiy);
        jiao = -jiajiao(-jiao, n.benjijiao - 90);
        chang = fanquxian[int(chang * 2.7 / cdbl)];
        zhizuobiao(&x, &y, jiao, chang, 320, 240);
        erzhi_bian[int(x)][int(y)] = 1;
        erzhi_bian[int(x) + 1][int(y)] = 1;
        erzhi_bian[int(x)][int(y) + 1] = 1;
        erzhi_bian[int(x) + 1][int(y) + 1] = 1;
    }
    for (i = 700; i < 1200; i += 10)
    {
        jizuobiao(0 - changbian, i, &jiao, &chang, n.benjix, n.benjiy);
        jiao = -jiajiao(-jiao, n.benjijiao - 90);
        chang = fanquxian[int(chang * 2.7 / cdbl)];
        zhizuobiao(&x, &y, jiao, chang, 320, 240);
        erzhi_bian[int(x)][int(y)] = 1;
        erzhi_bian[int(x) + 1][int(y)] = 1;
        erzhi_bian[int(x)][int(y) + 1] = 1;
        erzhi_bian[int(x) + 1][int(y) + 1] = 1;
        jizuobiao(1800 + changbian, i, &jiao, &chang, n.benjix, n.benjiy);
        jiao = -jiajiao(-jiao, n.benjijiao - 90);
        chang = fanquxian[int(chang * 2.7 / cdbl)];
        zhizuobiao(&x, &y, jiao, chang, 320, 240);
        erzhi_bian[int(x)][int(y)] = 1;
        erzhi_bian[int(x) + 1][int(y)] = 1;
        erzhi_bian[int(x)][int(y) + 1] = 1;
        erzhi_bian[int(x) + 1][int(y) + 1] = 1;
    }
    /*
        for(i=0;i<640;i++)
            for(j=0;j<480;j++)
            {
                if(erzhi_bian[i][j]==1)
                    SetColor(i,j,yanse);
                if(erzhi_black[i][j]==1)
                    SetColor(i,j,yanse);
            }
    */
}

void UPRColor::vhecheng(double jiao1, double sudu1, double top1, double jiao2, double sudu2, double top2, double *jiao,
                        double *sudu)
{
    double x1;
    double y1;
    double x2;
    double y2;
    double x;
    double y;
    if (sudu1 > top1)
        sudu1 = top1;
    if (sudu2 > top2)
        sudu2 = top2;
    zhizuobiao(&x1, &y1, jiao1, sudu1, 0, 0);
    zhizuobiao(&x2, &y2, jiao2, sudu2, 0, 0);
    x = x1 + x2;
    y = y1 + y2;
    jizuobiao(x, y, jiao, sudu, 0, 0);
}

void UPRColor::shichangfa(int vol, double *jiao, double *chang)
{
    RGBTRIPLE yanse;
    yanse.rgbtBlue = 255;
    yanse.rgbtGreen = 255;
    yanse.rgbtRed = 255;
    RGBTRIPLE yanse1;
    yanse1.rgbtBlue = 0;
    yanse1.rgbtGreen = 0;
    yanse1.rgbtRed = 255;
    int hei[144];
    int i = 0;
    int j = 0;
    double x1 = 0;
    double y1 = 0;
    double jiao1 = 0;
    double chang1 = 0;
    double jiaoh = 0;
    double changh = 0;
    for (i = 0; i < 144; i++)
    {
        hei[i] = 0;
        for (j = 40; j < 200; j++)
        {
            if (erzhi_black[shexianx[i][j]][shexiany[i][j]] == 1)
            {
                //	DrawFocus(shexianx[i][j],shexiany[i][j],&yanse);
                hei[i] = j;
                break;
            }
        }
    }

    for (i = 0; i < 144; i++)
    {
        if (hei[i] != 0)
        {
            jizuobiao(shexianx[i][hei[i]], shexiany[i][hei[i]], &jiao1, &chang1, 320, 240);
            vhecheng(jiao1 + 180, 10000 / (chang1 * chang1), 10000, jiaoh, changh, 20000, &jiaoh, &changh);
        }
    }
    zhizuobiao(&x1, &y1, jiaoh, changh, 320, 240);
    DrawFocus(x1, y1, &yanse1);
    *jiao = jiaoh;
    *chang = changh;
}

void UPRColor::DebugInfoM(CString inStr)
{
    if (pList != NULL)
    {
        if (pList->GetCount() > 5000)
        {
            pList->ResetContent();
        }
        pList->AddString(inStr);
        pList->SetCurSel(pList->GetCount() - 1);
    }
}

void UPRColor::DebugInfoS(CString inStr)
{
    if (pEdit != NULL)
    {
        pEdit->SetWindowText(inStr);
    }
}

// void Init_Mask_sector(void);
// void Init_Mask_Checked_Box(void);
// void Init_mask_checked_box_lookup_table(void);
// void bool_max_connectivity_analyze2_2_checked_box_lookup(float *obj_weight_map,float *outx,float *outy);
const float n_p_err =
    0.8; // 函数bool_max_connectivity_analyze2_2_checked_box_lookup中定义 当前次 和上次找到的目标面积最大误差百分比

int sector_mask[Sector_Num][(int)(OMNI_VISION_CAM_WIDTH * OMNI_VISION_CAM_WIDTH / Sector_Num * 1.414)]
               [2];                                        // 存放每个点的[0]y[1]x
int sector_mask_R[Sector_Num][OMNI_VISION_CAM_HEIGHT / 2]; // 存放每个扇区的每个半径在掩模中的起始位置
void UPRColor::Init_Mask_sector(void)
{
    // int temp_mask[OMNI_VISION_CAM_HEIGHT][OMNI_VISION_CAM_WIDTH][2];//临时存放每个点的[0]扇区[1]半径
    // int temp_mask_L[OMNI_VISION_CAM_HEIGHT/2];//临时存放每个半径的有效存储长度
    // int sector_mask_L[Sector_Num];//存放每个扇区的有效存储长度
    // 申请临时空间 用来按半径排序存储
    int ***temp_mask; //[OMNI_VISION_CAM_HEIGHT/2][OMNI_VISION_CAM_HEIGHT/2][2];//临时存放每个点的[0]x[1]y[2]l
    int i, j;
    temp_mask = new int **[OMNI_VISION_CAM_HEIGHT / 2];
    for (i = 0; i < OMNI_VISION_CAM_HEIGHT / 2; i++)
    {
        temp_mask[i] = new int *[OMNI_VISION_CAM_HEIGHT / 2];
    }
    for (i = 0; i < OMNI_VISION_CAM_HEIGHT / 2; i++)
    {
        for (j = 0; j < OMNI_VISION_CAM_HEIGHT / 2; j++)
        {
            temp_mask[i][j] = new int[3];
        }
    }
    int *temp_mask_L = new int[OMNI_VISION_CAM_HEIGHT / 2]; // 临时存放每个半径的有效存储长度
    int *sector_mask_L = new int[Sector_Num];               // 存放每个扇区的有效存储长度

    long int sort[1000]; //*sort= new int[1000];//排序数组

    // 需要更快可以隔点扫描
    int x, y, s, l, r, t, sector, radius, n, m, temp;
    for (s = 0; s < Sector_Num; s++)
    {
        sector_mask_L[s] = 0;
    }
    for (y = 0; y < OMNI_VISION_CAM_HEIGHT; y++)
    {
        if (y % 2 == 0)
            x = 0;
        else
            x = 1;
        for (x = x; x < OMNI_VISION_CAM_WIDTH; x += 2)
        {
            //[所属扇区号]
            sector = atan2((x - MIRROR_CENTER_X), (y - MIRROR_CENTER_Y)) * Sector_Num / 6.283 + Sector_Num / 2;
            if (sector >= Sector_Num)
                sector = sector - Sector_Num;
            sector_mask[sector][sector_mask_L[sector]][0] = y;
            sector_mask[sector][sector_mask_L[sector]][1] = x;
            sector_mask_L[sector]++;
            //	if ((sector>=Sector_Num)||(sector<0)) //测试越界代码
            //		 sector =sector;
            //	if ((sector==100))
            //		 sector= sector;
        }
    }
    for (s = 0; s < Sector_Num; s++)
    {
        for (r = 0; r < OMNI_VISION_CAM_HEIGHT / 2; r++)
        {
            temp_mask_L[r] = 0;
            sector_mask_R[s][r] = 0; //
        }
        for (l = 0; l < sector_mask_L[s]; l++)
        {
            y = sector_mask[s][l][0];
            x = sector_mask[s][l][1];
            // [半径]
            radius =
                sqrt((x - MIRROR_CENTER_X) * (x - MIRROR_CENTER_X) + (y - MIRROR_CENTER_Y) * (y - MIRROR_CENTER_Y));
            if (radius >= OMNI_VISION_CAM_HEIGHT / 2)
                continue;
            temp_mask[radius][temp_mask_L[radius]][0] = y;
            temp_mask[radius][temp_mask_L[radius]][1] = x;
            sector = (atan2((x - MIRROR_CENTER_X), (y - MIRROR_CENTER_Y)) * (float)(Sector_Num) / 6.283 +
                      (float)(Sector_Num) / 2) *
                     1000; // atan2((y-MIRROR_CENTER_Y),(x-MIRROR_CENTER_X));
            if (sector >= Sector_Num * 1000)
                sector = sector - Sector_Num * 1000;
            temp_mask[radius][temp_mask_L[radius]][2] = sector;
            //		if (temp_mask[radius][temp_mask_L[radius]][2]==14400)
            //			temp_mask[radius][temp_mask_L[radius]][2]=temp_mask[radius][temp_mask_L[radius]][2];
            temp_mask_L[radius]++;
        }

        for (l = 0, r = 0; r < OMNI_VISION_CAM_HEIGHT / 2; r++)
        {
            for (t = 0; t < temp_mask_L[r]; t++)
            {
                sort[t] = t;
            }
            for (t = 0; t < temp_mask_L[r] - 1; t++)
            {
                for (m = 0; m < temp_mask_L[r] - 1 - t; m++)
                {
                    if (temp_mask[r][sort[m]][2] > temp_mask[r][sort[m + 1]][2])
                    {
                        temp = sort[m + 1];
                        sort[m + 1] = sort[m];
                        sort[m] = temp;
                    }
                }
            }
            for (t = 0; t < temp_mask_L[r]; t++)
            {
                sector_mask[s][l][0] = temp_mask[r][sort[t]][0];
                sector_mask[s][l][1] = temp_mask[r][sort[t]][1];
                l++;
            }
            sector_mask_R[s][r] = l; //
        }
    }

    // 释放申请的临时空间
    for (i = 0; i < OMNI_VISION_CAM_HEIGHT / 2; i++)
    {
        for (j = 0; j < OMNI_VISION_CAM_HEIGHT / 2; j++)
        {
            delete temp_mask[i][j];
        }
    }
    for (i = 0; i < OMNI_VISION_CAM_HEIGHT / 2; i++)
    {
        delete temp_mask[i];
    }

    delete temp_mask;

    delete temp_mask_L;   // 临时存放每个半径的有效存储长度
    delete sector_mask_L; // 存放每个扇区的有效存储长度
}

int sector_maskA[Sector_Num][(int)(OMNI_VISION_CAM_WIDTH * OMNI_VISION_CAM_WIDTH / Sector_Num * 1.414)]
                [3];                                        // 存放每个点的[0]y[1]x
int sector_maskA_R[Sector_Num][OMNI_VISION_CAM_HEIGHT / 2]; // 存放每个扇区的每个半径在掩模中的起始位置
void UPRColor::Init_Mask_sectorA(void)
{
    // int temp_mask[OMNI_VISION_CAM_HEIGHT][OMNI_VISION_CAM_WIDTH][2];//临时存放每个点的[0]扇区[1]半径
    // int temp_mask_L[OMNI_VISION_CAM_HEIGHT/2];//临时存放每个半径的有效存储长度
    // int sector_maskA_L[Sector_Num];//存放每个扇区的有效存储长度
    // 申请临时空间 用来按半径排序存储
    int ***temp_mask; //[OMNI_VISION_CAM_HEIGHT/2][OMNI_VISION_CAM_HEIGHT/2][2];//临时存放每个点的[0]x[1]y[2]l
    int i, j;
    temp_mask = new int **[OMNI_VISION_CAM_HEIGHT / 2];
    for (i = 0; i < OMNI_VISION_CAM_HEIGHT / 2; i++)
    {
        temp_mask[i] = new int *[OMNI_VISION_CAM_HEIGHT / 2];
    }
    for (i = 0; i < OMNI_VISION_CAM_HEIGHT / 2; i++)
    {
        for (j = 0; j < OMNI_VISION_CAM_HEIGHT / 2; j++)
        {
            temp_mask[i][j] = new int[3];
        }
    }
    int *temp_mask_L = new int[OMNI_VISION_CAM_HEIGHT / 2]; // 临时存放每个半径的有效存储长度
    int *sector_maskA_L = new int[Sector_Num];              // 存放每个扇区的有效存储长度

    long int sort[1000]; //*sort= new int[1000];//排序数组

    // 需要更快可以隔点扫描
    int x, y, s, l, r, t, sector, radius, n, m, temp;
    for (s = 0; s < Sector_Num; s++)
    {
        sector_maskA_L[s] = 0;
    }
    for (y = 0; y < OMNI_VISION_CAM_HEIGHT; y++)
    { //	   if(y%2==0)
        //				x=0;
        //			else
        //				x=1;
        for (x = 0; x < OMNI_VISION_CAM_WIDTH; x++)
        {
            //[所属扇区号]
            sector = atan2((x - MIRROR_CENTER_X), (y - MIRROR_CENTER_Y)) * Sector_Num / 6.283 + Sector_Num / 2;
            if (sector >= Sector_Num)
                sector = sector - Sector_Num;
            sector_maskA[sector][sector_maskA_L[sector]][0] = y;
            sector_maskA[sector][sector_maskA_L[sector]][1] = x;
            sector_maskA_L[sector]++;
            //	if ((sector>=Sector_Num)||(sector<0)) //测试越界代码
            //		 sector =sector;
            //	if ((sector==100))
            //		 sector= sector;
        }
    }
    for (s = 0; s < Sector_Num; s++)
    {
        for (r = 0; r < OMNI_VISION_CAM_HEIGHT / 2; r++)
        {
            temp_mask_L[r] = 0;
            sector_maskA_R[s][r] = 0; //
        }
        for (l = 0; l < sector_maskA_L[s]; l++)
        {
            y = sector_maskA[s][l][0];
            x = sector_maskA[s][l][1];
            // [半径]
            radius =
                sqrt((x - MIRROR_CENTER_X) * (x - MIRROR_CENTER_X) + (y - MIRROR_CENTER_Y) * (y - MIRROR_CENTER_Y));
            if (radius >= OMNI_VISION_CAM_HEIGHT / 2)
                continue;
            temp_mask[radius][temp_mask_L[radius]][0] = y;
            temp_mask[radius][temp_mask_L[radius]][1] = x;
            sector = (atan2((x - MIRROR_CENTER_X), (y - MIRROR_CENTER_Y)) * (float)(360) / 6.283 + (float)(360) / 2) *
                     1000; // atan2((y-MIRROR_CENTER_Y),(x-MIRROR_CENTER_X));
            if (sector >= 360 * 1000)
                sector = sector - 360 * 1000;
            temp_mask[radius][temp_mask_L[radius]][2] = sector;
            //	if (temp_mask[radius][temp_mask_L[radius]][2]==14400)
            //		temp_mask[radius][temp_mask_L[radius]][2]=temp_mask[radius][temp_mask_L[radius]][2];
            temp_mask_L[radius]++;
        }

        for (l = 0, r = 0; r < OMNI_VISION_CAM_HEIGHT / 2; r++)
        {
            for (t = 0; t < temp_mask_L[r]; t++)
            {
                sort[t] = t;
            }
            for (t = 0; t < temp_mask_L[r] - 1; t++)
            {
                for (m = 0; m < temp_mask_L[r] - 1 - t; m++)
                {
                    if (temp_mask[r][sort[m]][2] > temp_mask[r][sort[m + 1]][2])
                    {
                        temp = sort[m + 1];
                        sort[m + 1] = sort[m];
                        sort[m] = temp;
                    }
                }
            }
            for (t = 0; t < temp_mask_L[r]; t++)
            {
                sector_maskA[s][l][0] = temp_mask[r][sort[t]][0];
                sector_maskA[s][l][1] = temp_mask[r][sort[t]][1];
                sector_maskA[s][l][2] = temp_mask[r][sort[t]][2];
                l++;
            }
            sector_maskA_R[s][r] = l; //
        }
    }

    // 释放申请的临时空间
    for (i = 0; i < OMNI_VISION_CAM_HEIGHT / 2; i++)
    {
        for (j = 0; j < OMNI_VISION_CAM_HEIGHT / 2; j++)
        {
            delete temp_mask[i][j];
        }
    }
    for (i = 0; i < OMNI_VISION_CAM_HEIGHT / 2; i++)
    {
        delete temp_mask[i];
    }

    delete temp_mask;

    delete temp_mask_L;    // 临时存放每个半径的有效存储长度
    delete sector_maskA_L; // 存放每个扇区的有效存储长度
}

// 640_480  64*48
const int mask_box_W = 10; // 240;//单个 生成用矩形的 宽 高 点个数
const int mask_box_H = 10; // 240;
const int mask_box_L = mask_box_W * mask_box_H / 2;

const int mask_checked_box_W = OMNI_VISION_CAM_WIDTH / mask_box_W; // 棋盘格 拥有 生成用矩形 的 横 纵个数
const int mask_checked_box_H = OMNI_VISION_CAM_HEIGHT / mask_box_H;

int mask_checked_box[mask_checked_box_H][mask_checked_box_W][mask_box_L]
                    [4]; //[y编号] [x编号] [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标] [所属扇区号] [半径]  棋盘格
                         // 生成掩膜用掩膜 用来生成 查找连通域撒种的区域
void UPRColor::Init_Mask_Checked_Box(void)
{ // 需要更快可以隔点扫描
    int w, h, x, y, i;
    for (w = 0; w < mask_checked_box_W; w++)
    {
        for (h = 0; h < mask_checked_box_H; h++)
        {
            i = 0;
            for (y = 0; y < mask_box_H; y++)
            {
                if (y % 2 == 0)
                    x = 0;
                else
                    x = 1;
                for (x = x; x < mask_box_W; x += 2)
                {
                    mask_checked_box[h][w][i][0] = y + h * mask_box_H;
                    mask_checked_box[h][w][i][1] = x + w * mask_box_W;
                    //[所属扇区号]
                    mask_checked_box[h][w][i][2] =
                        atan2((x + w * mask_box_W - MIRROR_CENTER_X), (y + h * mask_box_H - MIRROR_CENTER_Y)) *
                            Sector_Num / 6.283 +
                        Sector_Num / 2;
                    if (mask_checked_box[h][w][i][2] >= Sector_Num)
                        mask_checked_box[h][w][i][2] = mask_checked_box[h][w][i][2] - Sector_Num;
                    // [半径]
                    mask_checked_box[h][w][i][3] =
                        sqrt((x + w * mask_box_W - MIRROR_CENTER_X) * (x + w * mask_box_W - MIRROR_CENTER_X) +
                             (y + h * mask_box_H - MIRROR_CENTER_Y) * (y + h * mask_box_H - MIRROR_CENTER_Y));
                    i++;
                }
            }
        }
    }
}

int mask_checked_box_lookup_table[mask_checked_box_W * mask_checked_box_H * 4]
                                 [2]; //[格子数量] [格子y编号] [格子x编号]  为了保证当 开始点在某个角
                                      // 也能找遍整个屏幕设置  [格子数量*4] [y] [x]
// 生成 棋盘格扫描查找顺序的掩膜
void UPRColor::Init_mask_checked_box_lookup_table(void)
{ // 开始从x=0 y=0点 逆时针 开始扩展
    int max_layer = mask_checked_box_W;
    int x, y, l, i;
    i = 0;
    mask_checked_box_lookup_table[i][0] = 0;
    mask_checked_box_lookup_table[i++][1] = 0;
    for (l = 1; l < max_layer; l++)
    {

        for (x = -l; x < l; x++) // 1边
        {
            y = l;
            mask_checked_box_lookup_table[i][0] = y;
            mask_checked_box_lookup_table[i++][1] = x;
        }
        for (y = l; y > -l; y--) // 2边
        {
            x = l;
            mask_checked_box_lookup_table[i][0] = y;
            mask_checked_box_lookup_table[i++][1] = x;
        }
        for (x = l; x > -l; x--) // 3边
        {
            y = -l;
            mask_checked_box_lookup_table[i][0] = y;
            mask_checked_box_lookup_table[i++][1] = x;
        }
        for (y = -l; y < l; y++) // 4边
        {
            x = -l;
            mask_checked_box_lookup_table[i][0] = y;
            mask_checked_box_lookup_table[i++][1] = x;
        }
    }
}
// 检验 棋盘格是否在有效屏幕区域内0<=x<mask_checked_box_W   0<=y<mask_checked_box_H

void UPRColor::make_in_film_radius_table_init(void) // make_in_film_radius_table()参数初始化函数
{
    film_w = 1900;                       // 设置make_in_film_radius_table()场地范围参数 右边线坐标
    film_h = 1300;                       // 设置make_in_film_radius_table()场地范围参数 上边线坐标
    film_w_ = -100;                      // 设置make_in_film_radius_table()场地范围参数 左边线坐标
    film_h_ = -100;                      // 设置make_in_film_radius_table()场地范围参数 下边线坐标
    max_radius = 215;                    // 设置所有扇区的最大半径????????????????????????????????????????????????
    min_radius = 15;                     // 标示最小半径 即之内所有点不做计算
    for (int i = 0; i < Sector_Num; i++) // 将 不要处理的 扇区 去掉  镜子支架 防高球网
    {
        In_field_R_map[i][0] = min_radius;
        In_field_R_map[i][1] = max_radius;
    }
}
void UPRColor::make_in_film_radius_table(float f_x, float f_y, float f_a)
{
    // f_x=1801;//
    // f_y=601;
    // f_a=180;
    // f_x=5166;f_y=-1752;f_a=-1577;
    int intT = f_a / 360;
    f_a = f_a - intT * 360;
    RGBTRIPLE ball2;
    ball2.rgbtBlue = 128;
    ball2.rgbtGreen = 128;
    ball2.rgbtRed = 255;
    int i, j;

    // f_x=f_x (film_w-film_w_)/2;
    int min_radiusR = quxian[(int)min_radius]; // 表示实际地面多少厘米 之内不做计算
    float coner[4][4];
    int conerT[4];
    int conerTL = 0;

    coner[0][3] = film_w_; //()
    coner[0][2] = film_h_; //()
    coner[0][1] = film_w;  //()
    coner[0][0] = film_h_; //()

    coner[1][3] = film_w;  //()
    coner[1][2] = film_h_; //()
    coner[1][1] = film_w;  //()
    coner[1][0] = film_h;  //()

    coner[2][3] = film_w;  //()
    coner[2][2] = film_h;  //()
    coner[2][1] = film_w_; //()
    coner[2][0] = film_h;  //()

    coner[3][3] = film_w_; //()
    coner[3][2] = film_h;  //()
    coner[3][1] = film_w_; //()
    coner[3][0] = film_h_; //()

    // f2l_radius(f_x,f_y,f_a, film_w,film_h);
    double x1, y1, x2, y2, a1, r1, a2, r2, Ca, Sa, Ca1, Sa1, Ca2, Sa2, diff_angle, flog;
    double ta, tr;
    double outA, outA_, outR;
    int radius_num, outAT;
    double step_angle = 360.0 / (float)Sector_Num;
    double radius_line[Sector_Num][2];
    char flogP[Sector_Num];

    int flogTest = 1;
    for (i = 0; i < Sector_Num; i++)
    {
        radius_line[i][0] = min_radius;
        radius_line[i][1] = min_radius;
        flogP[i] = 0;
    }
    for (i = 0; i < 4; i++)
    {
        y1 = coner[i][0];
        x1 = coner[i][1];
        y2 = coner[i][2];
        x2 = coner[i][3];
        a1 = f2l_angle(f_x, f_y, f_a, x1, y1);  //(0,0~0,1200)
        r1 = f2l_radius(f_x, f_y, f_a, x1, y1); //(0,0~0,1200)
        a2 = f2l_angle(f_x, f_y, f_a, x2, y2);
        r2 = f2l_radius(f_x, f_y, f_a, x2, y2);
        outA = a1 / step_angle;
        outAT = outA; // 取整
        //	outA =outAT*step_angle;
        float tt = angle_add_transfer((float)(outAT)*step_angle, -a1);
        if (tt < 0) //(outAT*step_angle<outA)
            outA = (outAT + 1) * step_angle;
        else
            outA = (outAT)*step_angle;
        diff_angle = angle_add_transfer(a2, -outA);

        a1 = a1 * 3.14 / 180.0;
        a2 = a2 * 3.14 / 180.0;
        Ca1 = cos(a1);
        Sa1 = sin(a1);
        Ca2 = cos(a2);
        Sa2 = sin(a2);
        if (diff_angle < 0)
        {
            diff_angle = -diff_angle;
            flog = -1;
        }
        else
        {
            diff_angle = diff_angle;
            flog = 1;
        }
        flogTest = 1;
        for (ta = 0; ta < diff_angle; ta += step_angle)
        {
            if (flogTest == 1)
            {
                flogTest = 1;
            }

            outA_ = angle_add_transfer(outA, ta * flog);
            radius_num = outA_ / step_angle;
            if (radius_num < 0)
                radius_num += Sector_Num;
            if (radius_num >= Sector_Num)
                radius_num -= Sector_Num;
            outA_ = outA_ * 3.14 / 180.0;
            Ca = cos(outA_);
            Sa = sin(outA_);
            outR = r1 * r2 * (Ca1 * Sa2 - Ca2 * Sa1) / (r2 * Ca * Sa2 - r1 * Ca * Sa1 - r2 * Ca2 * Sa + r1 * Ca1 * Sa);
            if (outR > 2400) // 这里 是 反曲线查找表的 最大输入量
                outR = 2400;
            if (outR < min_radiusR)
                outR = min_radiusR;
            outR = fanquxian[(int)(outR * field_zoom_time)];
            if (outR < min_radius)
                outR = min_radius;

            if (radius_line[radius_num][1] == min_radius)
                radius_line[radius_num][1] = outR;
            else
            {
                if (outR > radius_line[radius_num][1])
                {
                    radius_line[radius_num][0] = radius_line[radius_num][1];
                    radius_line[radius_num][1] = outR;
                }
                else
                {
                    radius_line[radius_num][0] = outR;
                }
            }
            //	}

            flogP[radius_num] = flogP[radius_num] + 1;

            //	m_find.
            // SetColor(coordinate_transfer_x(outA_*180.0/3.14,outR)+320,coordinate_transfer_y(outA_*180.0/3.14,outR)+240,ball2);
            if (flogTest == 1)
            {
                flogTest = 0;
                conerT[conerTL++] = radius_num;
                DrawFocus(coordinate_transfer_x(outA_ * 180.0 / 3.14, outR) + 320,
                          coordinate_transfer_y(outA_ * 180.0 / 3.14, outR) + 240, &ball2);
                //	SetColor(coordinate_transfer_x(outA_*180.0/3.14,outR)+320,coordinate_transfer_y(outA_*180.0/3.14,outR)+240,ball1);
            }
        }
    }
    int tt = 0;
    for (i = 0; i < Sector_Num; i++) // 如果 所有射线 都只有一个点 那么机器在场地里 否则在场地外
    {
        if (flogP[i] == 1)
            tt++;
    }
    // if ((f_x<film_w_ || f_x>film_w ||
    //	f_y<film_h_ || f_y>film_h)&&(tt<Sector_Num))
    if (tt < Sector_Num)
    {
        for (i = 0; i < Sector_Num; i++)
        {
            if (flogP[i] == 1)
            //	if (radius_line[conerT[i]][0]==radius_line[conerT[i]][1])
            {
                radius_line[i][1] = min_radius;
                radius_line[i][0] = min_radius;
            }
        }
    }

    for (i = 0; i < Sector_Num; i++) // 镜像
    {
        /*	j=-i+Sector_Num;//j=-i+Sector_Num-f_a*Sector_Num*2/360;
            if (j>=Sector_Num)
                j-=Sector_Num;
            if (j<0)
                j+=Sector_Num;
            j=j-(int)(f_a*Sector_Num*2/360)
            if (j>=Sector_Num)
                j-=Sector_Num;
            if (j<0)
                j+=Sector_Num;
            j=j+Sector_Num/2;//-5
            if (j>=Sector_Num)
                j-=Sector_Num;
            if (j<0)
                j+=Sector_Num;
        */
        j = -i + Sector_Num - (int)(f_a * Sector_Num * 2 / 360) + Sector_Num / 2;
        int t = j / Sector_Num;
        j = j - t * Sector_Num;
        if (j >= Sector_Num)
            j -= Sector_Num;
        if (j < 0)
            j += Sector_Num;
        In_field_R_map[j][0] = radius_line[i][0]; // 存储在场地内要 进行处理的点的每个扇区的半径
        In_field_R_map[j][1] = radius_line[i][1]; // 存储在场地内要 进行处理的点的每个扇区的半径
    }
    for (i = 0; i < Sector_Num; i++)
    {
        radius_line[i][0] = In_field_R_map[i][0]; // 存储在场地内要 进行处理的点的每个扇区的半径
        radius_line[i][1] = In_field_R_map[i][1]; // 存储在场地内要 进行处理的点的每个扇区的半径
    }
    /*
    for (i=0;i<Sector_Num;i++)//显示覆盖射线 测试
    {
        for (j=radius_line[i][0];j<radius_line[i][1];j++)
        {
            SetColor(shexianx[i][j],shexiany[i][j],ball2);
        }
    }
    */
    for (i = 0; i < Sector_Num; i++) // 将机体区域 去掉
    {
        if (In_field_R_map[i][0] < robot_min_radius[i])
        {
            In_field_R_map[i][0] = robot_min_radius[i]; // 存储在场地内要 进行处理的点的每个扇区的半径
        }
        //	if(In_field_R_map[i][0]>max_radius)//将扇区限制在最大半径以内 因为远处反曲线查找表不准确
        //	{  In_field_R_map[i][0]=max_radius;
        //	}
        if (In_field_R_map[i][1] > max_radius) // 将扇区限制在最大半径以内 因为远处反曲线查找表不准确
        {
            In_field_R_map[i][1] = max_radius;
        }
    }
    int s;
    for (i = 0; i < angle_num; i++) // 将 不要处理的 扇区 去掉  镜子支架 防高球网
    {
        s = angle[i] * Sector_Num / 360 + Sector_Num / 2;
        if (s >= Sector_Num)
            s = s - Sector_Num;
        In_field_R_map[s][0] = 0;
        In_field_R_map[s][1] = 0;
    }
}

unsigned int area_grow_data_are[OMNI_VISION_CAM_HEIGHT * OMNI_VISION_CAM_WIDTH]
                               [2]; //[640*480][y][x];//currently当前生长的一圈的纪录
// unsigned int
// area_grow_data_arey[OMNI_VISION_CAM_HEIGHT*OMNI_VISION_CAM_WIDTH];//[640*480];//currently当前生长的一圈的纪录

unsigned int connect_area_s_e_w[65536][3]; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                           // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]

float target_x_p = 0; // 上周期 得到目标的 x y w
float target_y_p = 0;
float target_w_p = 0;

void UPRColor::bool_max_connectivity_analyze2_2_checked_box_lookup(float *obj_weight_map, float *outx, float *outy,
                                                                   float *min_r)
{
    register unsigned long int area_grow_data_arel = 0; // 纪录currently当前这一圈有几个像素
    RGBTRIPLE cPointG;
    cPointG.rgbtBlue = 0x00;
    cPointG.rgbtGreen = 0xFF;
    cPointG.rgbtRed = 0x00;
    RGBTRIPLE cPointB;
    cPointB.rgbtBlue = 0xFF;
    cPointB.rgbtGreen = 0x00;
    cPointB.rgbtRed = 0x00;

    // const char H_min_diff		=8;	//这是一个可调值关系到颜色的分辨粗略程度 如15以上黄色和橘黄就分不开了
    // const int grow_area_times =12000;//整个生成 生长区的次数//5000 要1min 40s
    // const int grow_min_weight	=40;//30如果单个生长区重量大于grow_min_weight 则把纪录A存入C[n]
    const int x320 = OMNI_VISION_CAM_WIDTH;  // 320;//320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240 = OMNI_VISION_CAM_HEIGHT; // 240;//240;//处理图像数据的大小设置
    const int x320r1 = x320 - 1;             // 320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240r1 = y240 - 1;             // 240;//处理图像数据的大小设置

    register int m_i, i, j, k, l, x, y, mask_checked_box_x, mask_checked_box_y; // seedx,seedy,,used_num
    // int x_grow,y_grow;
    // static unsigned char  area_grow_dataC[y240][x320];// 标号输出
    // register float  circumscription_weight;
    // register float  max_area_w;//最大的连通域的重量
    // register int    max_area_num;//最大的连通域的标号
    register int x_add, x_reduce, y_add, y_reduce, xg, yg;

    //	connect_area_l=0;
    register int connect_area_num =
        1; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点 2连通域的第一个编号 3 4 5....
    register int connect_max_area_num = 1; // 最大连通域的编号	默认无效值1
    register int connect_area_w = 0;       // 记录连通域重量值
    register int connect_max_area_w = 0;   // 记录连通域最大重量值
    // int connect_area_s_e_w[65536][3];//存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
    // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
    register int connect_area_layer_l = 0;  // 存储连通域 层的 点数 长度
    register int connect_area_layer_s = 0;  // 存储连通域 层的 首
    register int connect_area_layer_e = 0;  // 存储连通域 层的 尾
    register int connect_area_layer_pl = 0; // 存储连通域 层的 点数 长度
    register int connect_area_layer_ps = 0; // 存储连通域 上层的 首
    register int connect_area_layer_pe = 0; // 存储连通域 上层的 尾

    // used_num  =1;
    // max_area_w=0;
    // seedx=0;////////////////////
    // seedy=0;////////////////////

    ////////////////////////////////////
    // 黑边处理  防止出现边缘冒了//////
    for (x = 0; x < x320; x++)
    {
        erzhi_b[x][0] = 0;
        erzhi_b[x][y240 - 1] = 0;
    }
    for (y = 1; y < y240 - 1; y++)
    {
        erzhi_b[0][y] = 0;
        erzhi_b[x320 - 1][y] = 0;
    }
    // 黑边处理  防止出现边缘冒了//////

    area_grow_data_arel = 0; // 所有连通域纪录 位置置零
    unsigned int *area_grow_data_are_PO = area_grow_data_are[0];
    // 1自动设置这次生长的起点种子
    // for(i=0;i<close_green_grow_maskL;i++)
    int *mask_checked_box_address; // 存储格子内 掩膜数据指针
    register int A, R;             // 所属扇区号 半径
    for (m_i = 0; m_i < mask_checked_box_W * mask_checked_box_H * 4; m_i++)
    {
        // 是否重新找到目标的判定
        if (target_w_p > 0)
            if ((fabs(connect_max_area_w - target_w_p) / target_w_p < n_p_err) && (connect_max_area_w > 10)) // 0.5
                break;
        //  else
        //      如果上周期丢失目标 就将全屏搜索

        // 按照渐开顺序 查找 类似于上一贞图象的目标
        mask_checked_box_y = mask_checked_box_lookup_table[m_i][0];
        mask_checked_box_x = mask_checked_box_lookup_table[m_i][1];
        if (target_x_p > 0)
        {
            mask_checked_box_x = mask_checked_box_x + target_x_p / mask_box_W;
            mask_checked_box_y = mask_checked_box_y + target_y_p / mask_box_H;
        }
        else
        {
            mask_checked_box_x = mask_checked_box_x + x320 / 2 / mask_box_W;
            mask_checked_box_y = mask_checked_box_y + y240 / 2 / mask_box_H;
        }
        // 检验 棋盘格是否在有效屏幕区域内0<=x<mask_checked_box_W   0<=y<mask_checked_box_H
        if ((mask_checked_box_x < 0) || (mask_checked_box_x >= mask_checked_box_W) || (mask_checked_box_y < 0) ||
            (mask_checked_box_y >= mask_checked_box_H))
            continue;

        mask_checked_box_address = mask_checked_box[mask_checked_box_y][mask_checked_box_x]
                                                   [0]; //[y编号] [x编号] [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标]
                                                        // 棋盘格 生成掩膜用掩膜 用来生成 查找连通域撒种的区域

        i = 0;
        while (i < mask_box_L)
        {
            y = *mask_checked_box_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][0];//[y编号]
                                             // [x编号] [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标]  棋盘格
                                             // 生成掩膜用掩膜 用来生成 查找连通域撒种的区域
            x = *mask_checked_box_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][1];
            // mask_checked_box_address+=2;
            A = *mask_checked_box_address++; //[所属扇区号]
            R = *mask_checked_box_address++; // [半径]
            if ((In_field_R_map[A][0] < R) && (R < In_field_R_map[A][1]))
            {
                ///*测试代码*/SetColor(x,y,0,0,255,img);
#ifdef UPRColor_FUNCTION_DSPLAY
                /*测试代码*/ SetColor(x, y, cPointB);
#endif

                if (erzhi_b[x][y] == 1) // 没生长过 且是有效点
                {                       // seedx=x;
                    // seedy=y;
                    // 2开始生长
                    connect_area_num++; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点
                                        // 2连通域的第一个编号 3 4 5....
                    connect_area_w = 0; // 记录连通域重量值	清零
                    connect_area_s_e_w[connect_area_num][0] =
                        area_grow_data_arel; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                             // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
                    // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
                    // 1设置这次生长的起点种子
                    //			area_grow_data_arex[area_grow_data_arel]=x;//当前次生长的一圈的纪录
                    //			area_grow_data_arey[area_grow_data_arel++]=y;//当前次生长的一圈的纪录
                    *area_grow_data_are_PO++ = y;
                    *area_grow_data_are_PO++ = x;
                    area_grow_data_arel++;
                    connect_area_layer_l = 1;
                    connect_area_layer_pe = connect_area_layer_ps + 1;
                    //    		area_grow_dataAtpx[0]=x;//seedx;////手动设置种子
                    //    		area_grow_dataAtpy[0]=y;//seedy;////手动设置种子
                    //    		area_grow_dataAtpl=1;////手动设置种子
                    connect_area_w++; // 连通域重量+1

                    // 开始生长
                    while (1) // for (i=0;i<1500;i++)//生成生长区域 的生长次数
                    {
                        connect_area_layer_s = area_grow_data_arel;
                        j = connect_area_layer_ps;
                        while (j < connect_area_layer_pe)
                        { // 周围不是A中的有效点且符合生长规则
                            //    				xg=area_grow_data_arex[j];//当前次生长的一圈的纪录
                            //    				yg=area_grow_data_arey[j];//当前次生长的一圈的纪录
                            xg = area_grow_data_are[j][1]; // 当前次生长的一圈的纪录
                            yg = area_grow_data_are[j][0]; // 当前次生长的一圈的纪录
                            x_add = xg + 1;
                            x_reduce = xg - 1;
                            y_add = yg + 1;
                            y_reduce = yg - 1;
                            if (erzhi_b[x_reduce][y_add] == 1)
                            { // area_grow_data_arey[area_grow_data_arel]=y_add;
                                // area_grow_data_arex[area_grow_data_arel++]=x_reduce;//当前次生长的一圈的纪录
                                *area_grow_data_are_PO++ = y_add;
                                *area_grow_data_are_PO++ = x_reduce;
                                area_grow_data_arel++;
                                erzhi_b[x_reduce][y_add] = connect_area_num;
                            }
                            if (erzhi_b[xg][y_add] == 1)
                            { // area_grow_data_arey[area_grow_data_arel]=y_add;
                                // area_grow_data_arex[area_grow_data_arel++]=xg;//当前次生长的一圈的纪录
                                *area_grow_data_are_PO++ = y_add;
                                *area_grow_data_are_PO++ = xg;
                                area_grow_data_arel++;
                                erzhi_b[xg][y_add] = connect_area_num;
                            }
                            if (erzhi_b[x_add][y_add] == 1)
                            { // area_grow_data_arey[area_grow_data_arel]=y_add;
                                // area_grow_data_arex[area_grow_data_arel++]=x_add;//当前次生长的一圈的纪录
                                *area_grow_data_are_PO++ = y_add;
                                *area_grow_data_are_PO++ = x_add;
                                area_grow_data_arel++;
                                erzhi_b[x_add][y_add] = connect_area_num;
                            }
                            if (erzhi_b[x_reduce][yg] == 1)
                            { // area_grow_data_arey[area_grow_data_arel]=yg;
                                // area_grow_data_arex[area_grow_data_arel++]=x_reduce;//当前次生长的一圈的纪录
                                *area_grow_data_are_PO++ = yg;
                                *area_grow_data_are_PO++ = x_reduce;
                                area_grow_data_arel++;
                                erzhi_b[x_reduce][yg] = connect_area_num;
                            }
                            //	||
                            //((area_grow_dataA[y+0][x+0]==1)&&(abs(dataHSV[y+0][x+0][0]-tempH)<5)&&(abs(dataHSV[y+0][x+0][2]-tempV)<30)&&(abs(dataHSV[y+0][x+0][1]-tempS)<40))
                            if (erzhi_b[x_add][yg] == 1)
                            { // area_grow_data_arey[area_grow_data_arel]=yg;
                                // area_grow_data_arex[area_grow_data_arel++]=x_add;//当前次生长的一圈的纪录
                                *area_grow_data_are_PO++ = yg;
                                *area_grow_data_are_PO++ = x_add;
                                area_grow_data_arel++;
                                erzhi_b[x_add][yg] = connect_area_num;
                            }
                            if (erzhi_b[x_reduce][y_reduce] == 1)
                            { // area_grow_data_arey[area_grow_data_arel]=y_reduce;
                                // area_grow_data_arex[area_grow_data_arel++]=x_reduce;//当前次生长的一圈的纪录
                                *area_grow_data_are_PO++ = y_reduce;
                                *area_grow_data_are_PO++ = x_reduce;
                                area_grow_data_arel++;
                                erzhi_b[x_reduce][y_reduce] = connect_area_num;
                            }
                            if (erzhi_b[xg][y_reduce] == 1)
                            { // area_grow_data_arey[area_grow_data_arel]=y_reduce;
                                // area_grow_data_arex[area_grow_data_arel++]=xg;//当前次生长的一圈的纪录
                                *area_grow_data_are_PO++ = y_reduce;
                                *area_grow_data_are_PO++ = xg;
                                area_grow_data_arel++;
                                erzhi_b[xg][y_reduce] = connect_area_num;
                            }
                            if (erzhi_b[x_add][y_reduce] == 1)
                            { // area_grow_data_arey[area_grow_data_arel]=y_reduce;
                                // area_grow_data_arex[area_grow_data_arel++]=x_add;//当前次生长的一圈的纪录
                                *area_grow_data_are_PO++ = y_reduce;
                                *area_grow_data_are_PO++ = x_add;
                                area_grow_data_arel++;
                                erzhi_b[x_add][y_reduce] =
                                    connect_area_num; ////////////////////////////////////////////改区域生长 加参数
                            }
                            j++;
                        }
                        connect_area_layer_e = area_grow_data_arel;
                        connect_area_layer_l = connect_area_layer_e - connect_area_layer_s; // 连通域重量+当前一圈的点数
                        connect_area_layer_ps = connect_area_layer_s;                       // 存储连通域 上层的 首
                        connect_area_layer_pe = connect_area_layer_e;                       // 存储连通域 上层的 尾

                        connect_area_w += connect_area_layer_l;
                        if (connect_area_layer_l == 0)
                            break;
                    }
                    // 生长完成
                    // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
                    connect_area_s_e_w[connect_area_num][1] =
                        area_grow_data_arel; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                             // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
                    connect_area_s_e_w[connect_area_num][2] =
                        connect_area_w; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                        // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
                    if (connect_max_area_w < connect_area_w)
                    {
                        connect_max_area_w = connect_area_w;
                        connect_max_area_num = connect_area_num;
                    }
                }
            }
            //	else
            //	{A=A;
            //	}
            i++;
        }
    }

    //	for (i=0;i<area_grow_data_arel;i++)//测试代码
    //	{
    //		data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+0]=1;//测试代码//显示种子点
    //		data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+1]=254;//测试代码
    //		data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+2]=1;//测试代码
    //		m_VideoOCX.Show(m_Image);//测试代码				// display capture frame
    //		Sleep(200);
    //	}//测试代码

    // 不进行输出了 输出\\掉了
    // for (i=0;i<x320*y240;i++)
    //{	data_C_out[i]=0;
    // }
    // if (max_area_w>0)//(max_area_num>0)//////////////////////////////////////////////////////////////////
    //{	for (l=0;l<area_grow_data_areml;l++)
    //	{	data_C_out[area_grow_data_aremy[l]*320+area_grow_data_aremx[l]]=1;
    //	}
    // }
    /*
    i=0;while(i<close_green_grow_maskL)
    {
    y=close_green_grow_maskY[i];
    x=close_green_grow_maskX[i];
    //	SetColor(x,y,cPointB);
        SetColor(y,x,cPointB);
    i++;
    }
    if (max_area_w>0)//(max_area_num>0)//////////////////////////////////////////////////////////////////
    {	for (l=0;l<area_grow_data_areml;l++)
        {
    //		SetColor(area_grow_data_aremx[l],area_grow_data_aremy[l],cPointG);
            SetColor(area_grow_data_aremy[l],area_grow_data_aremx[l],cPointG);
        }
    }
    */

    ////////////////////////////////////////////////////////////////////////////////////
    *obj_weight_map =
        (float)connect_area_s_e_w[connect_max_area_num][2]; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                                            // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]

    // 带出黑白图象重心横坐标  obj_properties *All  All.x结果的重心的x坐标 All.y结果的重心的y坐标
    // 需要处理部分的掩膜数组  int *yan_mo_x,int *yan_mo_y
    // 需要处理部分的掩膜数组长度int yan_mo_long,
    register float OBJ_temp_b = 0;
    register float OBJ_temp_b_ = 0;
    register float OBJ_temp_c = 0;
    register float ball_op_x = 0;
    register float ball_op_y = 0;
    register float ball_op_weight = 0;
    register int tx, ty, minR, minRx, minRy;
    minR = 60000;
    minRx = 0;
    minRy = 0;
    if (connect_max_area_num > 1) // 最大连通域的编号	默认无效值1
    {
        for (i = connect_area_s_e_w[connect_max_area_num][0]; i < connect_area_s_e_w[connect_max_area_num][1]; i++)
        { //	SetColor(area_grow_data_arey[i],area_grow_data_arex[i],cPointB);
            OBJ_temp_c++;
            tx = area_grow_data_are[i][1];
            ty = area_grow_data_are[i][0];
            OBJ_temp_b += tx;
            OBJ_temp_b_ += ty;
            if (minR > banjing[tx][ty])
            {
                minR = banjing[tx][ty];
                minRx = tx;
                minRy = ty;
            }
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+0]=254;//测试代码//显示种子点
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+1]=254;//测试代码
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+2]=254;//测试代码
            //	SetColor(y,x,cPointG);//测试代码
            ///*测试代码*/SetColor(area_grow_data_are[i][1],area_grow_data_are[i][0],0,255,255,img);
        }
    }
    *min_r = (float)sqrt(minR);
    // m_VideoOCX.Show(m_Image);//测试代码				// display capture frame

    if (OBJ_temp_c != 0) // 保险程序  当同步不佳时  有时Capture不到图象 就会出现所有象素都为零 这时做OBJ_temp_b /
                         // OBJ_temp_c的除法就会产生分母为零的错误
    {
        ball_op_x = OBJ_temp_b / OBJ_temp_c;  //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+320
        ball_op_y = OBJ_temp_b_ / OBJ_temp_c; //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+240
    }
    ball_op_weight = OBJ_temp_c; // 准备把象素重量传给主逻辑函数
    target_x_p = ball_op_x;      // 上周期 得到目标的 x y w
    target_y_p = ball_op_y;
    target_w_p = ball_op_weight;

    *outx = target_x_p;
    *outy = target_y_p;

    /*
    if ( display_process_image == 1)//判断 显示过程图像	//显示最大连通域
    if (connect_max_area_num>1)//最大连通域的编号	默认无效值1
    {	RGBTRIPLE cPoint;	cPoint.rgbtBlue = 0x00;	cPoint.rgbtGreen = 0x00; 	cPoint.rgbtRed = 0xFF;
        for (i=connect_area_s_e_w[connect_max_area_num][0];i<connect_area_s_e_w[connect_max_area_num][1];i++)
        {	SetColor(area_grow_data_arey[i],area_grow_data_arex[i],cPoint);//测试代码
    }	}//测试代码

        if (OBJ_temp_c!=0)  //保险程序  当同步不佳时  有时Capture不到图象 就会出现所有象素都为零 这时做OBJ_temp_b /
    OBJ_temp_c的除法就会产生分母为零的错误
        {
            ball_op_x =OBJ_temp_b  / OBJ_temp_c;//(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+320
            ball_op_y =OBJ_temp_b_ / OBJ_temp_c;//(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+240
        }
        ball_op_weight=OBJ_temp_c;//准备把象素重量传给主逻辑函数
    //					HSI11->ball_x=ball_op_x;
    //					HSI11->ball_y=ball_op_y;
    //					HSI11->ball_zhong=ball_op_weight;
                        HSI11->ball_x=ball_op_y;
                        HSI11->ball_y=ball_op_x;
                        HSI11->ball_zhong=ball_op_weight;
                        jizuobiao(HSI11->ball_x,HSI11->ball_y,&HSI11->ball_jiao,&HSI11->ball_chang,320,240);

    //完成二值化图像标重心程序///////////////////////////////////////////////////////////////////

    //兼容鑫鑫 回存连同域数据
    memset(erzhi_b,0,OMNI_VISION_CAM_HEIGHT*OMNI_VISION_CAM_WIDTH*sizeof(int));
    if (connect_max_area_num>1)//最大连通域的编号	默认无效值1
    {	for (i=connect_area_s_e_w[connect_max_area_num][0];i<connect_area_s_e_w[connect_max_area_num][1];i++)
        {	erzhi_b[area_grow_data_arey[i]][area_grow_data_arex[i]]=1;
        }
    }
    RGBTRIPLE cPoint;	cPoint.rgbtBlue = 0x00;	cPoint.rgbtGreen = 0x00; 	cPoint.rgbtRed = 0x7f;
    if ( display_process_image == 1)//判断 显示过程图像	//显示最大连通域
    for (y=0;y<OMNI_VISION_CAM_HEIGHT;y++)
    for (x=0;x<OMNI_VISION_CAM_WIDTH;x++)
    {	if (erzhi_b[y][x]==1)
        {	SetColor(y,x,cPoint);//测试代码
        }
    }
    */
}

void UPRColor::bool_max_connectivity_analyze2_2_checked_box_lookupA(float *obj_weight_map, float *outx, float *outy,
                                                                    float *min_r, float *max_r)
{
    register unsigned long int area_grow_data_arel = 0; // 纪录currently当前这一圈有几个像素
    RGBTRIPLE cPointG;
    cPointG.rgbtBlue = 0xff;
    cPointG.rgbtGreen = 0x00;
    cPointG.rgbtRed = 0xff;
    RGBTRIPLE cPointB;
    cPointB.rgbtBlue = 0xFF;
    cPointB.rgbtGreen = 0x00;
    cPointB.rgbtRed = 0x00;

    // const char H_min_diff		=8;	//这是一个可调值关系到颜色的分辨粗略程度 如15以上黄色和橘黄就分不开了
    // const int grow_area_times =12000;//整个生成 生长区的次数//5000 要1min 40s
    // const int grow_min_weight	=40;//30如果单个生长区重量大于grow_min_weight 则把纪录A存入C[n]
    const int x320 = OMNI_VISION_CAM_WIDTH;  // 320;//320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240 = OMNI_VISION_CAM_HEIGHT; // 240;//240;//处理图像数据的大小设置
    const int x320r1 = x320 - 1;             // 320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240r1 = y240 - 1;             // 240;//处理图像数据的大小设置

    register int s, t, m_i, i, j, k, l, x, y, mask_checked_box_x, mask_checked_box_y; // seedx,seedy,,used_num
    // int x_grow,y_grow;
    // static unsigned char  area_grow_dataC[y240][x320];// 标号输出
    // register float  circumscription_weight;
    // register float  max_area_w;//最大的连通域的重量
    // register int    max_area_num;//最大的连通域的标号
    register int x_add, x_reduce, y_add, y_reduce, xg, yg;

    //	connect_area_l=0;
    register int connect_area_num =
        1; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点 2连通域的第一个编号 3 4 5....
    register int connect_max_area_num = 1; // 最大连通域的编号	默认无效值1
    register int connect_area_w = 0;       // 记录连通域重量值
    register int connect_max_area_w = 0;   // 记录连通域最大重量值
    // int connect_area_s_e_w[65536][3];//存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
    // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
    register int connect_area_layer_l = 0;  // 存储连通域 层的 点数 长度
    register int connect_area_layer_s = 0;  // 存储连通域 层的 首
    register int connect_area_layer_e = 0;  // 存储连通域 层的 尾
    register int connect_area_layer_pl = 0; // 存储连通域 层的 点数 长度
    register int connect_area_layer_ps = 0; // 存储连通域 上层的 首
    register int connect_area_layer_pe = 0; // 存储连通域 上层的 尾

    // used_num  =1;
    // max_area_w=0;
    // seedx=0;////////////////////
    // seedy=0;////////////////////

    x = 80; // x 0+80
    y = 3;  // y 0+1
    while (y < 477)
    {
        x = 80;
        while (x < 560) // 640-80
        {
            if (erzhi_b[x][y] > 0)
            {
                // 方法1 上下都为0 或 左右都为0 即去掉
                //	if (((erzhi_b[x+1][y]==0)&&(erzhi_b[x-1][y]==0))||((erzhi_b[x][y+1]==0)&&(erzhi_b[x][y-1]==0)))
                //	{	erzhi_b[x][y]=0;
                //	//	#ifdef UPRColor_FUNCTION_DSPLAY
                //		/*测试代码*/SetColor(x,y,cPointG);
                //	//	#endif
                //	}
                // 方法1 上下都为0 或 左右都为0 即去掉
                //		if (((erzhi_b[x+2][y]==0)&&(erzhi_b[x-2][y]==0))||((erzhi_b[x][y+2]==0)&&(erzhi_b[x][y-2]==0)))
                //		{	erzhi_b[x][y]=0;
                //		//	#ifdef UPRColor_FUNCTION_DSPLAY
                //		/*测试代码*/SetColor(x,y,cPointG);
                //		//	#endif
                //		}
                // 方法2	上下左右都为0 即去掉
                //	if ((erzhi_b[x+1][y]==0)&&(erzhi_b[x-1][y]==0)&&(erzhi_b[x][y+1]==0)&&(erzhi_b[x][y-1]==0))
                //	{	erzhi_b[x][y]=0;
                //	}
                // 方法3 上下左右有1个0 即去掉 //会有问题 如果为0 一行点都会去掉
                //	if ((erzhi_b[x+1][y]==0)||(erzhi_b[x-1][y]==0)||(erzhi_b[x][y+1]==0)||(erzhi_b[x][y-1]==0))
                //	{	erzhi_b[x][y]=65535;
                //	}

                // 方法4 上下左右有1个0 即去掉 //会有问题 如果为0 一行点都会去掉
                if ((erzhi_b[x + 1][y] == 0) || (erzhi_b[x - 1][y] == 0) || (erzhi_b[x][y + 1] == 0) ||
                    (erzhi_b[x][y - 1] == 0))
                {
                    erzhi_b[x][y] = 65535;
                }
            }
            x++;
        }
        y++;
    }
    x = 80; // x 0+80
    y = 3;  // y 0+1
    while (y < 477)
    {
        x = 80;
        while (x < 560) // 640-80
        {
            if (erzhi_b[x][y] == 65535)
            {
                erzhi_b[x][y] = 0;
                /*测试代码*/ SetColor(x, y, cPointG);
            }
            x++;
        }
        y++;
    }

    ////////////////////////////////////
    // 黑边处理  防止出现边缘冒了//////
    for (x = 0; x < x320; x++)
    {
        erzhi_b[x][0] = 0;
        erzhi_b[x][y240 - 1] = 0;
    }
    for (y = 1; y < y240 - 1; y++)
    {
        erzhi_b[0][y] = 0;
        erzhi_b[x320 - 1][y] = 0;
    }
    // 黑边处理  防止出现边缘冒了//////

    area_grow_data_arel = 0; // 所有连通域纪录 位置置零
    unsigned int *area_grow_data_are_PO = area_grow_data_are[0];
    // 1自动设置这次生长的起点种子
    // for(i=0;i<close_green_grow_maskL;i++)
    int *mask_checked_box_address; // 存储格子内 掩膜数据指针
    register int A, R;             // 所属扇区号 半径

    for (s = 0; s < Sector_Num; s++)
    {

        t = sector_mask_R[s][In_field_R_map[s][1]];
        i = sector_mask_R[s][In_field_R_map[s][0]];
        mask_checked_box_address = sector_mask[s][i];
        while (i < t)
        {
            // int
            // sector_mask[Sector_Num][(int)(OMNI_VISION_CAM_WIDTH*OMNI_VISION_CAM_WIDTH/Sector_Num*1.414)][2];//临时存放每个点的[0]y[1]x
            // int sector_mask_R[Sector_Num][OMNI_VISION_CAM_HEIGHT/2];//存放每个扇区的每个半径在掩模中的起始位置

            y = *mask_checked_box_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][0];//[y编号]
                                             // [x编号] [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标]  棋盘格
                                             // 生成掩膜用掩膜 用来生成 查找连通域撒种的区域
            x = *mask_checked_box_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][1];
// mask_checked_box_address++;
// mask_checked_box_address+=2;
// A=*mask_checked_box_address++;//[所属扇区号]
// R=*mask_checked_box_address++;// [半径]
//	if((In_field_R_map[A][0]<R	)&&(R<In_field_R_map[A][1]	))
//	{
///*测试代码*/SetColor(x,y,0,0,255,img);
#ifdef UPRColor_FUNCTION_DSPLAY
            /*测试代码*/ SetColor(x, y, cPointB);
#endif
            if (erzhi_b[x][y] == 1) // 没生长过 且是有效点
            {                       // seedx=x;
                // seedy=y;
                // 2开始生长
                connect_area_num++; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点
                                    // 2连通域的第一个编号 3 4 5....
                connect_area_w = 0; // 记录连通域重量值	清零
                connect_area_s_e_w[connect_area_num][0] =
                    area_grow_data_arel; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                         // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
                // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
                // 1设置这次生长的起点种子
                //			area_grow_data_arex[area_grow_data_arel]=x;//当前次生长的一圈的纪录
                //			area_grow_data_arey[area_grow_data_arel++]=y;//当前次生长的一圈的纪录
                *area_grow_data_are_PO++ = y;
                *area_grow_data_are_PO++ = x;
                area_grow_data_arel++;
                connect_area_layer_l = 1;
                connect_area_layer_pe = connect_area_layer_ps + 1;
                //    		area_grow_dataAtpx[0]=x;//seedx;////手动设置种子
                //    		area_grow_dataAtpy[0]=y;//seedy;////手动设置种子
                //    		area_grow_dataAtpl=1;////手动设置种子
                connect_area_w++; // 连通域重量+1

                // 开始生长
                while (1) // for (i=0;i<1500;i++)//生成生长区域 的生长次数
                {
                    connect_area_layer_s = area_grow_data_arel;
                    j = connect_area_layer_ps;
                    while (j < connect_area_layer_pe)
                    { // 周围不是A中的有效点且符合生长规则
                        //    				xg=area_grow_data_arex[j];//当前次生长的一圈的纪录
                        //    				yg=area_grow_data_arey[j];//当前次生长的一圈的纪录
                        xg = area_grow_data_are[j][1]; // 当前次生长的一圈的纪录
                        yg = area_grow_data_are[j][0]; // 当前次生长的一圈的纪录
                        x_add = xg + 1;
                        x_reduce = xg - 1;
                        y_add = yg + 1;
                        y_reduce = yg - 1;
                        if (erzhi_b[x_reduce][y_add] == 1)
                        { // area_grow_data_arey[area_grow_data_arel]=y_add;
                            // area_grow_data_arex[area_grow_data_arel++]=x_reduce;//当前次生长的一圈的纪录
                            *area_grow_data_are_PO++ = y_add;
                            *area_grow_data_are_PO++ = x_reduce;
                            area_grow_data_arel++;
                            erzhi_b[x_reduce][y_add] = connect_area_num;
                        }
                        if (erzhi_b[xg][y_add] == 1)
                        { // area_grow_data_arey[area_grow_data_arel]=y_add;
                            // area_grow_data_arex[area_grow_data_arel++]=xg;//当前次生长的一圈的纪录
                            *area_grow_data_are_PO++ = y_add;
                            *area_grow_data_are_PO++ = xg;
                            area_grow_data_arel++;
                            erzhi_b[xg][y_add] = connect_area_num;
                        }
                        if (erzhi_b[x_add][y_add] == 1)
                        { // area_grow_data_arey[area_grow_data_arel]=y_add;
                            // area_grow_data_arex[area_grow_data_arel++]=x_add;//当前次生长的一圈的纪录
                            *area_grow_data_are_PO++ = y_add;
                            *area_grow_data_are_PO++ = x_add;
                            area_grow_data_arel++;
                            erzhi_b[x_add][y_add] = connect_area_num;
                        }
                        if (erzhi_b[x_reduce][yg] == 1)
                        { // area_grow_data_arey[area_grow_data_arel]=yg;
                            // area_grow_data_arex[area_grow_data_arel++]=x_reduce;//当前次生长的一圈的纪录
                            *area_grow_data_are_PO++ = yg;
                            *area_grow_data_are_PO++ = x_reduce;
                            area_grow_data_arel++;
                            erzhi_b[x_reduce][yg] = connect_area_num;
                        }
                        //	||
                        //((area_grow_dataA[y+0][x+0]==1)&&(abs(dataHSV[y+0][x+0][0]-tempH)<5)&&(abs(dataHSV[y+0][x+0][2]-tempV)<30)&&(abs(dataHSV[y+0][x+0][1]-tempS)<40))
                        if (erzhi_b[x_add][yg] == 1)
                        { // area_grow_data_arey[area_grow_data_arel]=yg;
                            // area_grow_data_arex[area_grow_data_arel++]=x_add;//当前次生长的一圈的纪录
                            *area_grow_data_are_PO++ = yg;
                            *area_grow_data_are_PO++ = x_add;
                            area_grow_data_arel++;
                            erzhi_b[x_add][yg] = connect_area_num;
                        }
                        if (erzhi_b[x_reduce][y_reduce] == 1)
                        { // area_grow_data_arey[area_grow_data_arel]=y_reduce;
                            // area_grow_data_arex[area_grow_data_arel++]=x_reduce;//当前次生长的一圈的纪录
                            *area_grow_data_are_PO++ = y_reduce;
                            *area_grow_data_are_PO++ = x_reduce;
                            area_grow_data_arel++;
                            erzhi_b[x_reduce][y_reduce] = connect_area_num;
                        }
                        if (erzhi_b[xg][y_reduce] == 1)
                        { // area_grow_data_arey[area_grow_data_arel]=y_reduce;
                            // area_grow_data_arex[area_grow_data_arel++]=xg;//当前次生长的一圈的纪录
                            *area_grow_data_are_PO++ = y_reduce;
                            *area_grow_data_are_PO++ = xg;
                            area_grow_data_arel++;
                            erzhi_b[xg][y_reduce] = connect_area_num;
                        }
                        if (erzhi_b[x_add][y_reduce] == 1)
                        { // area_grow_data_arey[area_grow_data_arel]=y_reduce;
                            // area_grow_data_arex[area_grow_data_arel++]=x_add;//当前次生长的一圈的纪录
                            *area_grow_data_are_PO++ = y_reduce;
                            *area_grow_data_are_PO++ = x_add;
                            area_grow_data_arel++;
                            erzhi_b[x_add][y_reduce] =
                                connect_area_num; ////////////////////////////////////////////改区域生长 加参数
                        }
                        j++;
                    }
                    connect_area_layer_e = area_grow_data_arel;
                    connect_area_layer_l = connect_area_layer_e - connect_area_layer_s; // 连通域重量+当前一圈的点数
                    connect_area_layer_ps = connect_area_layer_s;                       // 存储连通域 上层的 首
                    connect_area_layer_pe = connect_area_layer_e;                       // 存储连通域 上层的 尾

                    connect_area_w += connect_area_layer_l;
                    if (connect_area_layer_l == 0)
                        break;
                }
                // 生长完成
                // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
                connect_area_s_e_w[connect_area_num][1] =
                    area_grow_data_arel; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                         // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
                connect_area_s_e_w[connect_area_num][2] =
                    connect_area_w; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                    // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
                if (connect_max_area_w < connect_area_w)
                {
                    connect_max_area_w = connect_area_w;
                    connect_max_area_num = connect_area_num;
                }
            }

            //	}
            //	else
            //	{A=A;
            //	}
            i++;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////
    *obj_weight_map =
        (float)connect_area_s_e_w[connect_max_area_num][2]; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                                            // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]

    // 带出黑白图象重心横坐标  obj_properties *All  All.x结果的重心的x坐标 All.y结果的重心的y坐标
    // 需要处理部分的掩膜数组  int *yan_mo_x,int *yan_mo_y
    // 需要处理部分的掩膜数组长度int yan_mo_long,
    register float OBJ_temp_b = 0;
    register float OBJ_temp_b_ = 0;
    register float OBJ_temp_c = 0;
    register float ball_op_x = 0;
    register float ball_op_y = 0;
    register float ball_op_weight = 0;
    register int tx, ty, minR, minRx, minRy, maxR, maxRx, maxRy;
    minR = 100000;
    minRx = 0;
    minRy = 0;
    maxR = 0;
    maxRx = 0;
    maxRy = 0;
    if (connect_max_area_num > 1) // 最大连通域的编号	默认无效值1
    {
        for (i = connect_area_s_e_w[connect_max_area_num][0]; i < connect_area_s_e_w[connect_max_area_num][1]; i++)
        { //	SetColor(area_grow_data_arey[i],area_grow_data_arex[i],cPointB);
            OBJ_temp_c++;
            tx = area_grow_data_are[i][1];
            ty = area_grow_data_are[i][0];
            OBJ_temp_b += tx;
            OBJ_temp_b_ += ty;
            if (minR > banjing[tx][ty]) //(minR>sqrt((tx-320)*(tx-320)+(ty-240)*(ty-240)))//
            {
                minR = banjing[tx][ty]; // sqrt((tx-320)*(tx-320)+(ty-240)*(ty-240));
                minRx = tx;
                minRy = ty;
            }
            if (maxR < banjing[tx][ty]) //(minR>sqrt((tx-320)*(tx-320)+(ty-240)*(ty-240)))//
            {
                maxR = banjing[tx][ty]; // sqrt((tx-320)*(tx-320)+(ty-240)*(ty-240));
                maxRx = tx;
                maxRy = ty;
            }
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+0]=254;//测试代码//显示种子点
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+1]=254;//测试代码
            //	data[(area_grow_data_arey[i]*320+area_grow_data_arex[i])*3+2]=254;//测试代码
            //	SetColor(tx,ty,cPointG);//测试代码
            ///*测试代码*/SetColor(area_grow_data_are[i][1],area_grow_data_are[i][0],0,255,255,img);
        }
    }
    *min_r = sqrt((float)(minR)); // minR;//
    *max_r = sqrt((float)(maxR)); // minR;//
    // m_VideoOCX.Show(m_Image);//测试代码				// display capture frame

    if (OBJ_temp_c != 0) // 保险程序  当同步不佳时  有时Capture不到图象 就会出现所有象素都为零 这时做OBJ_temp_b /
                         // OBJ_temp_c的除法就会产生分母为零的错误
    {
        ball_op_x = OBJ_temp_b / OBJ_temp_c;  //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+320
        ball_op_y = OBJ_temp_b_ / OBJ_temp_c; //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+240
    }
    ball_op_weight = OBJ_temp_c; // 准备把象素重量传给主逻辑函数
    target_x_p = ball_op_x;      // 上周期 得到目标的 x y w
    target_y_p = ball_op_y;
    target_w_p = ball_op_weight;

    *outx = target_x_p;
    *outy = target_y_p;

    /*
    if ( display_process_image == 1)//判断 显示过程图像	//显示最大连通域
    if (connect_max_area_num>1)//最大连通域的编号	默认无效值1
    {	RGBTRIPLE cPoint;	cPoint.rgbtBlue = 0x00;	cPoint.rgbtGreen = 0x00; 	cPoint.rgbtRed = 0xFF;
        for (i=connect_area_s_e_w[connect_max_area_num][0];i<connect_area_s_e_w[connect_max_area_num][1];i++)
        {	SetColor(area_grow_data_arey[i],area_grow_data_arex[i],cPoint);//测试代码
    }	}//测试代码

        if (OBJ_temp_c!=0)  //保险程序  当同步不佳时  有时Capture不到图象 就会出现所有象素都为零 这时做OBJ_temp_b /
    OBJ_temp_c的除法就会产生分母为零的错误
        {
            ball_op_x =OBJ_temp_b  / OBJ_temp_c;//(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+320
            ball_op_y =OBJ_temp_b_ / OBJ_temp_c;//(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+240
        }
        ball_op_weight=OBJ_temp_c;//准备把象素重量传给主逻辑函数
    //					HSI11->ball_x=ball_op_x;
    //					HSI11->ball_y=ball_op_y;
    //					HSI11->ball_zhong=ball_op_weight;
                        HSI11->ball_x=ball_op_y;
                        HSI11->ball_y=ball_op_x;
                        HSI11->ball_zhong=ball_op_weight;
                        jizuobiao(HSI11->ball_x,HSI11->ball_y,&HSI11->ball_jiao,&HSI11->ball_chang,320,240);

    //完成二值化图像标重心程序///////////////////////////////////////////////////////////////////

    //兼容鑫鑫 回存连同域数据
    memset(erzhi_b,0,OMNI_VISION_CAM_HEIGHT*OMNI_VISION_CAM_WIDTH*sizeof(int));
    if (connect_max_area_num>1)//最大连通域的编号	默认无效值1
    {	for (i=connect_area_s_e_w[connect_max_area_num][0];i<connect_area_s_e_w[connect_max_area_num][1];i++)
        {	erzhi_b[area_grow_data_arey[i]][area_grow_data_arex[i]]=1;
        }
    }
    RGBTRIPLE cPoint;	cPoint.rgbtBlue = 0x00;	cPoint.rgbtGreen = 0x00; 	cPoint.rgbtRed = 0x7f;
    if ( display_process_image == 1)//判断 显示过程图像	//显示最大连通域
    for (y=0;y<OMNI_VISION_CAM_HEIGHT;y++)
    for (x=0;x<OMNI_VISION_CAM_WIDTH;x++)
    {	if (erzhi_b[y][x]==1)
        {	SetColor(y,x,cPoint);//测试代码
        }
    }
    */
}

void UPRColor::getchangxian2()
{
    //	for()
    int max_W_width = 20;
    xiandianshu = 1;
    RGBTRIPLE ball1;
    ball1.rgbtBlue = 0x00;
    ball1.rgbtGreen = 0xff;
    ball1.rgbtRed = 0xff;
    double jiao1;
    double chang1;
    double x1;
    double y1;

    const char radius_step = 1; // 射线采集步长
    const char x_step = 20;     // 纵向 采集步长
    const char y_step = 20;     // 横向 采集步长

    int zdianshu = 110; // 90;
    int baiseshu;
    baiseshu = 0;
    int lv1;
    int bai1;
    int xx[640];
    int yy[640];
    float xxh;
    float yyh;
    int nn;
    int xundian;
    int s, r;
    int tx, ty;
    int startR;
    for (s = 0; s < 144; s += radius_step)
    {
        xxh = 0;
        yyh = 0;
        lv1 = 0;
        bai1 = 0;
        baiseshu = 0;
        nn = 0;
        if (s % 2 == 0)
            startR = In_field_R_map[s][0];
        else
            startR = 120;
        for (r = startR; r < In_field_R_map[s][1]; r++)
        {
            //	if(erzhi_di[shexianx[x][y]][shexiany[x][y]]==1)
            //	{
            tx = shexianx[s][r];
            ty = shexiany[s][r];
            if (lv1 == 0)
                if (erzhi_yuandi[tx][ty] == 1)
                    // 如果射线是绿色
                    lv1 = 1;
            if (lv1 == 1)
                if (erzhi_bai[tx][ty] == 1)
                {
                    // 如果射线是绿色后又是白色
                    bai1 = 1;
                    baiseshu++;

                    //	xx[nn]=s;
                    yy[nn] = r;
                    nn++;
                }
            if (bai1 == 1 && baiseshu < max_W_width)
                if (erzhi_yuandi[tx][ty] == 1)
                {
                    //	SetColor(huanyuanx[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,huanyuany[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,ball1);
                    lv1 = 0;
                    bai1 = 0;
                    for (xundian = 0; xundian < nn; xundian++)
                    {
                        //	xxh=xxh+xx[xundian];
                        yyh = yyh + yy[xundian];
                    }
                    //	xxh=xxh/nn;
                    xxh = s;
                    yyh = (float)(yyh) / (float)(nn);
                    baiseshu = 0;
                    //	jizuobiao(shexianx[xxh][yyh],shexiany[xxh][yyh],&jiao1,&chang1,320,240);
                    jiao1 = s * 360.0 / 144.0 + 180.0;
                    if (jiao1 > 180)
                        jiao1 -= 360;
                    if (jiao1 < -180)
                        jiao1 += 360;
                    chang1 = yyh;
                    SetColor(shexianx[s][(int)chang1], shexiany[s][(int)chang1], ball1);
                    SetColor(shexianx[s][(int)chang1] + 2, shexiany[s][(int)chang1], ball1);
                    SetColor(shexianx[s][(int)chang1] + 1, shexiany[s][(int)chang1], ball1);
                    SetColor(shexianx[s][(int)chang1], shexiany[s][(int)chang1] + 1, ball1);
                    SetColor(shexianx[s][(int)chang1], shexiany[s][(int)chang1] + 2, ball1);
                    SetColor(shexianx[s][(int)chang1] - 1, shexiany[s][(int)chang1], ball1);
                    SetColor(shexianx[s][(int)chang1] - 2, shexiany[s][(int)chang1], ball1);
                    SetColor(shexianx[s][(int)chang1], shexiany[s][(int)chang1] - 1, ball1);
                    SetColor(shexianx[s][(int)chang1], shexiany[s][(int)chang1] - 2, ball1);
                    float rin = chang1;
                    int rt1 = rin;
                    int rt2;
                    if ((rin - rt1) >= 0) // 注意 windows取整基本只是吧小数点抹了 缩小取整
                        rt2 = rt1 + 1;
                    else
                        rt2 = rt1 - 1;
                    float rout = (quxian[rt2] - quxian[rt1]) * (rin - rt1) / (rt2 - rt1) + quxian[rt1];
                    chang1 = rout * 0.45 / cdbl; // double(quxian[int(chang1)])*0.49/cdbl;
                    //	chang1=double(quxian[int(chang1)])*0.45/cdbl;
                    zhizuobiao(&x1, &y1, jiao1, chang1, 320, 240);
                    changxianx[xiandianshu] = float(x1);
                    changxiany[xiandianshu] = float(y1);
                    xiandianshu++;
                    lv1 = 0;
                    bai1 = 0;
                    nn = 0;
                    xxh = 0;
                    yyh = 0;
                }
            //}
        }
    }
    /*	int s,r,a,l,t,i;
        int *mask_checked_box_address;//存储格子内 掩膜数据指针
        for(r=10;r<240;r+=10)
        {
            for(s=0;s<144;s++)
            {
                if ((In_field_R_map[s][0]<r)&&(r>In_field_R_map[s][1]))
                {
                    t=sector_maskA_R[s][r+1];
                    i=sector_maskA_R[s][r];
                    mask_checked_box_address=sector_maskA[s][i];
                    while(i<t)
                    {
                        y=*mask_checked_box_address++;//mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][0];//[y编号]
       [x编号] [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标]  棋盘格 生成掩膜用掩膜 用来生成 查找连通域撒种的区域
                        x=*mask_checked_box_address++;//mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][1];
                        a=(*mask_checked_box_address++)/1000;//mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][1];
                        i++;
                    }

                register int A,R;//所属扇区号 半径
                }
            }

        }
    */
    //******************************************************************

    int x, y;
    xiandianshuT = 0;
    for (x = 100; x < 540; x += x_step)
    {
        xxh = 0;
        yyh = 0;
        lv1 = 0;
        bai1 = 0;
        baiseshu = 0;
        nn = 0;
        for (y = 20; y <= 460; y++)
        {
            if (banjing[x][y] < 40000)
            {
                //				SetColor(x,y,ball1);
                if (lv1 == 0)
                    if (erzhi_yuandi[x][y] == 1)
                        // 如果射线是绿色
                        lv1 = 1;
                if (lv1 == 1)
                    if (erzhi_bai[x][y] == 1)
                    {
                        // 如果射线是绿色后又是白色
                        bai1 = 1;
                        baiseshu++;

                        xx[nn] = x;
                        yy[nn] = y;
                        nn++;
                    }
                if (bai1 == 1 && baiseshu < max_W_width)
                    if (erzhi_yuandi[x][y] == 1)
                    {
                        //	SetColor(huanyuanx[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,huanyuany[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,ball1);
                        lv1 = 0;
                        bai1 = 0;
                        for (xundian = 0; xundian < nn; xundian++)
                        {
                            xxh = xxh + xx[xundian];
                            yyh = yyh + yy[xundian];
                        }
                        xxh = xxh / nn;
                        yyh = yyh / nn;
                        baiseshu = 0;
                        jizuobiao(xxh, yyh, &jiao1, &chang1, 320, 240);
                        SetColor(xxh, yyh, ball1);
                        SetColor(xxh + 2, yyh, ball1);
                        SetColor(xxh + 1, yyh, ball1);
                        SetColor(xxh, yyh + 1, ball1);
                        SetColor(xxh, yyh + 2, ball1);
                        SetColor(xxh - 1, yyh, ball1);
                        SetColor(xxh - 2, yyh, ball1);
                        SetColor(xxh, yyh - 1, ball1);
                        SetColor(xxh, yyh - 2, ball1);
                        float rin = chang1;
                        int rt1 = rin;
                        int rt2;
                        if ((rin - rt1) >= 0) // 注意 windows取整基本只是吧小数点抹了 缩小取整
                            rt2 = rt1 + 1;
                        else
                            rt2 = rt1 - 1;
                        float rout = (quxian[rt2] - quxian[rt1]) * (rin - rt1) / (rt2 - rt1) + quxian[rt1];
                        chang1 = rout * 0.45 / cdbl; // double(quxian[int(chang1)])*0.49/cdbl;
                        //								chang1=double(quxian[int(chang1)])*0.45/cdbl;
                        zhizuobiao(&x1, &y1, jiao1, chang1, 320, 240);
                        changxianxT[xiandianshuT] = float(x1);
                        //	changxianx[xiandianshuT]=float(x1);
                        changxianyT[xiandianshuT] = float(y1);
                        //	changxiany[xiandianshuT]=float(y1);
                        xiandianshuT++;
                        lv1 = 0;
                        bai1 = 0;
                        nn = 0;
                        xxh = 0;
                        yyh = 0;
                    }
            }
        }
    }

    //******************************************************************
    for (y = 20; y < 480; y += y_step)
    {
        xxh = 0;
        yyh = 0;
        lv1 = 0;
        bai1 = 0;
        baiseshu = 0;
        nn = 0;
        for (x = 80; x <= 560; x++)
        {
            if (banjing[x][y] < 40000)
            {
                //	SetColor(x,y,ball1);
                if (lv1 == 0)
                    if (erzhi_yuandi[x][y] == 1)
                        // 如果射线是绿色
                        lv1 = 1;
                if (lv1 == 1)
                    if (erzhi_bai[x][y] == 1)
                    {
                        // 如果射线是绿色后又是白色
                        bai1 = 1;
                        baiseshu++;

                        xx[nn] = x;
                        yy[nn] = y;
                        nn++;
                    }
                if (bai1 == 1 && baiseshu < max_W_width)
                    if (erzhi_yuandi[x][y] == 1)
                    {
                        //	SetColor(huanyuanx[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,huanyuany[shexianx[x][xxx1]][shexiany[x][xxx1]]*0.5,ball1);
                        lv1 = 0;
                        bai1 = 0;
                        for (xundian = 0; xundian < nn; xundian++)
                        {
                            xxh = xxh + xx[xundian];
                            yyh = yyh + yy[xundian];
                        }
                        xxh = xxh / nn;
                        yyh = yyh / nn;
                        baiseshu = 0;
                        jizuobiao(xxh, yyh, &jiao1, &chang1, 320, 240);
                        SetColor(xxh, yyh, ball1);
                        SetColor(xxh + 2, yyh, ball1);
                        SetColor(xxh + 1, yyh, ball1);
                        SetColor(xxh, yyh + 1, ball1);
                        SetColor(xxh, yyh + 2, ball1);
                        SetColor(xxh - 1, yyh, ball1);
                        SetColor(xxh - 2, yyh, ball1);
                        SetColor(xxh, yyh - 1, ball1);
                        SetColor(xxh, yyh - 2, ball1);
                        float rin = chang1;
                        int rt1 = rin;
                        int rt2;
                        if ((rin - rt1) >= 0) // 注意 windows取整基本只是吧小数点抹了 缩小取整
                            rt2 = rt1 + 1;
                        else
                            rt2 = rt1 - 1;
                        float rout = (quxian[rt2] - quxian[rt1]) * (rin - rt1) / (rt2 - rt1) + quxian[rt1];
                        chang1 = rout * 0.45 / cdbl; // double(quxian[int(chang1)])*0.49/cdbl;
                        //								chang1=double(quxian[int(chang1)])*0.45/cdbl;
                        zhizuobiao(&x1, &y1, jiao1, chang1, 320, 240);
                        changxianxT[xiandianshuT] = float(x1);
                        changxianyT[xiandianshuT] = float(y1);
                        xiandianshuT++;
                        lv1 = 0;
                        bai1 = 0;
                        nn = 0;
                        xxh = 0;
                        yyh = 0;
                    }
            }
        }
    }

    //******************************************************************
    // xiandianshu=0;
    for (int i = 0; i < xiandianshuT; i++)
    {
        // 注意 此段代码的原意是 把在场地范围内的场线点摘出来并存入changxianx/y数组
        // 但因为先前已经经过畸变校正所以已经不能用In_field_R_map里德值判断是否 在场地内了
        // 但是因为之前只有场地范围内图像做了 二值化所以得到的 此处也就 不需要了
        /*		x=changxianxT[i];//
                y=changxianyT[i];
                //[所属扇区号]
                s=atan2((x-MIRROR_CENTER_X),(y-MIRROR_CENTER_Y))*Sector_Num/6.283+Sector_Num/2;
                if (s>=Sector_Num)
                    s =s-Sector_Num;
                r=sqrt((x-MIRROR_CENTER_X)*(x-MIRROR_CENTER_X)+(y-MIRROR_CENTER_Y)*(y-MIRROR_CENTER_Y));
                if (r>=OMNI_VISION_CAM_HEIGHT/2)
                    continue;


                if ((r>In_field_R_map[s][0])&&(r<In_field_R_map[s][1]))
                {
        */
        changxianx[xiandianshu] = changxianxT[i];
        changxiany[xiandianshu] = changxianyT[i];
        xiandianshu++;
        //		}
    }

    /*
        float zjl;
        if(xiandianshu>zdianshu)
        {
            zjl=xiandianshu/zdianshu;
            for(x=0;x<=zdianshu;x++)
            {
                changxianx[x]=changxianx[int(x*zjl)];
                changxiany[x]=changxiany[int(x*zjl)];
        //		SetColor(changxianx[int(x*zjl)],changxiany[int(x*zjl)],ball1);
        //		SetColor(changxianx[int(x*zjl)]+1,changxiany[int(x*zjl)],ball1);
        //		SetColor(changxianx[int(x*zjl)],changxiany[int(x*zjl)]+1,ball1);
        //		SetColor(changxianx[int(x*zjl)]-1,changxiany[int(x*zjl)],ball1);
        //		SetColor(changxianx[int(x*zjl)],changxiany[int(x*zjl)]-1,ball1);
            }
            xiandianshu=zdianshu;
        }
    */
    for (i = 0; i < xiandianshu; i++)
    {
        x = changxianx[i];
        y = changxiany[i];
        /*	SetColor(x  ,y+1,ball1);
            SetColor(x+1,y+1,ball1);
            SetColor(x-1,y+1,ball1);
            SetColor(x  ,y,ball1);
            SetColor(x+1,y,ball1);
            SetColor(x-1,y,ball1);
            SetColor(x  ,y-1,ball1);
            SetColor(x+1,y-1,ball1);
            SetColor(x-1,y-1,ball1);
        */
    }

    xiandianshu--;
    if (xiandianshu < 0)
        xiandianshu = 0;
}

// 使用掩膜circle_maskX[circle_mask_long] circle_maskY[circle_mask_long]进行 需要区域的颜色域转换
// 无 绿色 白色 版本
BOOL UPRColor::GetHSIFix3_no_G_W(Yuzhi *zz)
{
    // 取球和两个门的重心坐标，并对相应的2值化数组赋值。
    // 返回：球和两个门的直角坐标以及极坐标。
    // 只对全景（中间方块范围内）的区域做。

    RGBTRIPLE door1;
    door1.rgbtBlue = 0xFF;
    door1.rgbtGreen = 0x00;
    door1.rgbtRed = 0x00;
    RGBTRIPLE ball;
    ball.rgbtBlue = 0x00;
    ball.rgbtGreen = 0x00;
    ball.rgbtRed = 0xFF;
    RGBTRIPLE door2;
    door2.rgbtBlue = 0x88;
    door2.rgbtGreen = 0xFF;
    door2.rgbtRed = 0xFF;
    RGBTRIPLE heimo;
    heimo.rgbtBlue = 0x00;
    heimo.rgbtGreen = 0x00;
    heimo.rgbtRed = 0x00;
    //	HSIParam  *hsi =new  HSIParam;
    RGBTRIPLE dimian;
    dimian.rgbtBlue = 0x00;
    dimian.rgbtGreen = 0xFF;
    dimian.rgbtRed = 0x00;
    RGBTRIPLE heise;
    heise.rgbtBlue = 0x4D;
    heise.rgbtGreen = 0x00;
    heise.rgbtRed = 0x4D;
    RGBTRIPLE baise;
    baise.rgbtBlue = 0xDD;
    baise.rgbtGreen = 0xDD;
    baise.rgbtRed = 0xFF;
    register int volh = 0;
    register int vols = 0;
    register int voli = 0;
    register int i = 0;
    register int j = 0;
    register int k = 0;
    register int l = 0;
    register int s = 0;
    register int t = 0;
    register int r, g, b, H, S, V, Vmin;
    int *mask_checked_box_address;
    // 将HSI颜色，放入HSI颜色的矩阵。
    k = 0;
    j = 0;
    for (i = 0; i < 480; i++)
    {
        SetColor(80, i, baise);
        SetColor(560, i, baise);
    }

    unsigned int *erzhi_bT = &erzhi_b[80][0];
    unsigned int *erzhi_diT = &erzhi_di[80][0];
    unsigned int *erzhi_yuandiT = &erzhi_yuandi[80][0];
    unsigned int *erzhi_blackT = &erzhi_black[80][0];
    unsigned int *erzhi_baiT = &erzhi_bai[80][0];

    ///*
    i = 0;
    while (i < 480 * 480)
    {
        *erzhi_bT++ = 0;
        *erzhi_diT++ = 0;
        *erzhi_yuandiT++ = 0;
        *erzhi_blackT++ = 0;
        *erzhi_baiT++ = 0;
        i++;
    }
    //*/
    if (zz->bhs > zz->bhx) // 切换红色 跨180度问题
    {
        // 不垮180

        while (s < Sector_Num)
        {
            t = sector_maskA_R[s][In_field_R_map[s][1]];
            l = sector_maskA_R[s][In_field_R_map[s][0]];
            mask_checked_box_address = sector_maskA[s][l];
            while (l < t)
            {
                // int
                // sector_mask[Sector_Num][(int)(OMNI_VISION_CAM_WIDTH*OMNI_VISION_CAM_WIDTH/Sector_Num*1.414)][2];//临时存放每个点的[0]y[1]x
                // int sector_mask_R[Sector_Num][OMNI_VISION_CAM_HEIGHT/2];//存放每个扇区的每个半径在掩模中的起始位置

                j = *mask_checked_box_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][0];//[y编号]
                                                 // [x编号] [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标]  棋盘格
                                                 // 生成掩膜用掩膜 用来生成 查找连通域撒种的区域
                i = *mask_checked_box_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][1];
                mask_checked_box_address++;
                //		SetColor(i,j,dimian);

                k = (j * OMNI_VISION_CAM_WIDTH + i) * 3;

                b = m_pImageBuf[k];
                g = m_pImageBuf[k + 1];
                r = m_pImageBuf[k + 2];
                //	b = *m_pImageBuf+k;
                //	g = *m_pImageBuf+k + 1;
                //	r = *m_pImageBuf+k + 2;
                if (b > g)
                {
                    if (r > b)
                    {
                        V = r;
                        Vmin = g; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=g//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 360 + (g - b) * 60 / (V - Vmin); // 0*60
                        }
                        else
                            H = 0;
                    }
                    else // V=b   Vmin=?
                    {
                        if (r < g)
                        {
                            V = b;
                            Vmin = r; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=r//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = b;
                            Vmin = g; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=g//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                    }
                }
                else
                {
                    if (r > g)
                    {
                        V = r;
                        Vmin = b; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=b//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 0 + (g - b) * 60 / (V - Vmin); // 0*60
                            if (H < 0)
                            {
                                H = H + 255; // H=H+360;
                            }
                        }
                        else
                            H = 0;
                    }
                    else // V=g   Vmin=?
                    {
                        if (r < b)
                        {
                            V = g;
                            Vmin = r; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=r//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = g;
                            Vmin = b; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=b//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                    }
                }
                S = V - Vmin; //*255/V;    //这里改了 原来是S=(V-Vmin)*255/V
                volh = H;
                vols = S;
                voli = V;
                // if ((((volh<zz->bhs && volh>zz->bhx)  ||  (volh+360<zz->bhs && volh+360>zz->bhx) || (volh-360<zz->bhs
                // && volh-360>zz->bhx)) && vols>zz->bs))
                if ((volh >= zz->bhx && volh <= zz->bhs) && vols >= zz->bs)
                {
                    //					erzhi_fushi[i][j]=1;
                    erzhi_b[i][j] = 1;
                    SetColor(i, j, ball);
                    //	SetColor(i-1,j,ball);
                    //	SetColor(i+1,j,ball);
                    //	SetColor(i,j-1,ball);
                    //	SetColor(i,j+1,ball);
                    //			erzhi_yuandi[i][j]=0;
                    //			erzhi_di[i][j]=0;
                    //			erzhi_black[i][j]=0;
                    //			erzhi_bai[i][j]=0;
                }
                /*				else if (vols<=zz->sw  && voli >=zz->iw)
                                {
                                    erzhi_bai[i][j]=1;
                            //		SetColor(i,j,baise);
                        //			erzhi_yuandi[i][j]=0;
                        //		erzhi_di[i][j]=0;
                        //		erzhi_black[i][j]=0;
                                }


                                else  if ((volh<=zz->chs && volh>=zz->chx)  && vols>=zz->cs && voli<=zz->cis &&
                voli>=zz->cix)
                                {
                                    erzhi_di[i][j]=1;
                                    erzhi_yuandi[i][j]=1;
                //					SetColor(i,j,dimian);
                        //			erzhi_b[i][j]=0;
                        //			erzhi_black[i][j]=0;
                        //			erzhi_bai[i][j]=0;
                                }
                */
                //	/*
                else if (vols <= zz->sh && voli <= zz->ih)
                {
                    erzhi_black[i][j] = 1;
                    erzhi_black[i - 1][j] = 1;
                    erzhi_black[i + 1][j] = 1;
                    erzhi_black[i][j - 1] = 1;
                    erzhi_black[i][j + 1] = 1;

#ifdef UPRColor_FUNCTION_DSPLAY // 设置显示 场地范围信息
                    SetColor(i, j, heise);
#endif
                    //			erzhi_yuandi[i][j]=0;
                    //			erzhi_b[i][j]=0;
                    //			erzhi_di[i][j]=0;
                    //			erzhi_bai[i][j]=0;
                }
                //	*/
                //		else
                //		{
                //			erzhi_b[i][j]=0;
                //			erzhi_di[i][j]=0;
                //			erzhi_yuandi[i][j]=0;
                //			erzhi_black[i][j]=0;
                //			erzhi_bai[i][j]=0;
                //		}
                //*/
                l++;
            }
            s++;
        }
        ///*
    }
    else
    {

        // int	num1;

        while (s < Sector_Num)
        {
            t = sector_maskA_R[s][In_field_R_map[s][1]];
            l = sector_maskA_R[s][In_field_R_map[s][0]];
            mask_checked_box_address = sector_maskA[s][l];
            while (l < t)
            {
                // int
                // sector_mask[Sector_Num][(int)(OMNI_VISION_CAM_WIDTH*OMNI_VISION_CAM_WIDTH/Sector_Num*1.414)][2];//临时存放每个点的[0]y[1]x
                // int sector_mask_R[Sector_Num][OMNI_VISION_CAM_HEIGHT/2];//存放每个扇区的每个半径在掩模中的起始位置

                j = *mask_checked_box_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][0];//[y编号]
                                                 // [x编号] [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标]  棋盘格
                                                 // 生成掩膜用掩膜 用来生成 查找连通域撒种的区域
                i = *mask_checked_box_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][1];
                mask_checked_box_address++;

                k = (j * OMNI_VISION_CAM_WIDTH + i) * 3;

                b = m_pImageBuf[k];
                g = m_pImageBuf[k + 1];
                r = m_pImageBuf[k + 2];
                //	b = *m_pImageBuf+k;
                //	g = *m_pImageBuf+k + 1;
                //	r = *m_pImageBuf+k + 2;
                if (b > g)
                {
                    if (r > b)
                    {
                        V = r;
                        Vmin = g; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=g//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 360 + (g - b) * 60 / (V - Vmin); // 0*60
                        }
                        else
                            H = 0;
                    }
                    else // V=b   Vmin=?
                    {
                        if (r < g)
                        {
                            V = b;
                            Vmin = r; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=r//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = b;
                            Vmin = g; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=g//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                    }
                }
                else
                {
                    if (r > g)
                    {
                        V = r;
                        Vmin = b; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=b//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 0 + (g - b) * 60 / (V - Vmin); // 0*60
                            if (H < 0)
                            {
                                H = H + 255; // H=H+360;
                            }
                        }
                        else
                            H = 0;
                    }
                    else // V=g   Vmin=?
                    {
                        if (r < b)
                        {
                            V = g;
                            Vmin = r; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=r//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = g;
                            Vmin = b; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=b//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                    }
                }
                S = V - Vmin; //*255/V;    //这里改了 原来是S=(V-Vmin)*255/V
                volh = H;
                vols = S;
                voli = V;
                // if ((((volh<zz->bhs && volh>zz->bhx)  ||  (volh+360<zz->bhs && volh+360>zz->bhx) || (volh-360<zz->bhs
                // && volh-360>zz->bhx)) && vols>zz->bs))
                if ((volh >= zz->bhx || volh <= zz->bhs) && vols >= zz->bs)
                {
                    //					erzhi_fushi[i][j]=1;
                    erzhi_b[i][j] = 1;
                    SetColor(i, j, ball);
                    //	SetColor(i-1,j,ball);
                    //	SetColor(i+1,j,ball);
                    //	SetColor(i,j-1,ball);
                    //	SetColor(i,j+1,ball);
                    //				erzhi_yuandi[i][j]=0;
                    //				erzhi_di[i][j]=0;
                    //				erzhi_black[i][j]=0;
                    //				erzhi_bai[i][j]=0;
                }
                /*				else if (vols<=zz->sw  && voli >=zz->iw)
                                {
                                    erzhi_bai[i][j]=1;
                            //		SetColor(i,j,baise);
                    //				erzhi_yuandi[i][j]=0;
                    //			erzhi_di[i][j]=0;
                    //			erzhi_black[i][j]=0;
                                }


                                else  if ((volh<=zz->chs && volh>=zz->chx)  && vols>=zz->cs && voli<=zz->cis &&
                   voli>=zz->cix)
                                {
                                    erzhi_di[i][j]=1;
                                    erzhi_yuandi[i][j]=1;
                            //		SetColor(i,j,dimian);
                    //				erzhi_b[i][j]=0;
                    //				erzhi_black[i][j]=0;
                    //				erzhi_bai[i][j]=0;
                                }
                */
                //	/*
                else if (vols <= zz->sh && voli <= zz->ih)
                {
                    erzhi_black[i][j] = 1;
                    erzhi_black[i - 1][j] = 1;
                    erzhi_black[i + 1][j] = 1;
                    erzhi_black[i][j - 1] = 1;
                    erzhi_black[i][j + 1] = 1;

#ifdef UPRColor_FUNCTION_DSPLAY // 设置显示 场地范围信息
                    SetColor(i, j, heise);
#endif
                    //				erzhi_yuandi[i][j]=0;
                    //				erzhi_b[i][j]=0;
                    //				erzhi_di[i][j]=0;
                    //				erzhi_bai[i][j]=0;
                }
                //	*/
                //			else
                //			{
                //				erzhi_b[i][j]=0;
                //				erzhi_di[i][j]=0;
                //				erzhi_yuandi[i][j]=0;
                //				erzhi_black[i][j]=0;
                //				erzhi_bai[i][j]=0;
                //			}
                //*/
                l++;
            }
            s++;
        }
        ///*
    }
    // 球2值化
    //	delete hsi;
    return TRUE;
}
void UPRColor::m_LoadTables()
{
    m_txtar.ReadFromFile(".\\Config\\shemen.txt", shemenb, 2500);
    m_txtar.ReadFromFile(".\\Config\\qulv.txt", qulvb, 2500);
    m_txtar.ReadFromFile(".\\Config\\luodian.txt", luodianb, 2500);
}

int UPRColor::qijiaopanding(Yuzhi y, int a)
{
    RGBTRIPLE baise;
    baise.rgbtBlue = 0xFF;
    baise.rgbtGreen = 0xFF;
    baise.rgbtRed = 0xFF;
    RGBTRIPLE gray;
    gray.rgbtBlue = 128;
    gray.rgbtGreen = 128;
    gray.rgbtRed = 128;
    int duan = 50;
    int chang = 100;
    double xx;
    double yy;
    int heishu = 0;
    if (y.ddoor_chang - 50 < chang)
        chang = y.ddoor_chang - 50;
    int i = 0;
    int j = 0;
    if (a == 0)
        for (i = y.ddoor_jiao - 10; i <= y.ddoor_jiao + 10; i++)
            for (j = duan; j < chang /*-abs(y.ddoor_jiao-i)*/; j++)
            {
                zhizuobiao(&xx, &yy, i, j, 320, 240);
                if (erzhi_black[int(xx)][int(yy)] == 1)
                {
                    heishu++;
                    //	SetColor(xx,yy,baise);
                }

                //			SetColor(xx,yy,gray);//SetColor(xx,yy,baise);
            }
    else if (a == 1)
    {
        duan = 50;
        chang = 120;
        for (i = y.ddoor_jiao - 40; i < y.ddoor_jiao - 10; i++)
            for (j = duan; j < chang /*-abs(y.ddoor_jiao-i)*/; j++)
            {
                zhizuobiao(&xx, &yy, i, j, 320, 240);
                if (erzhi_black[int(xx)][int(yy)] == 1)
                {
                    heishu++;
                    //	SetColor(xx,yy,baise);
                }

                //		SetColor(xx,yy,baise);
            }
        for (i = y.ddoor_jiao + 10 + 1; i <= y.ddoor_jiao + 40; i++)
            for (j = duan; j < chang /*-abs(y.ddoor_jiao-i)*/; j++)
            {
                zhizuobiao(&xx, &yy, i, j, 320, 240);
                if (erzhi_black[int(xx)][int(yy)] == 1)
                {
                    heishu++;
                }

                //		SetColor(xx,yy,baise);
            }
        for (i = y.ddoor_jiao - 10; i <= y.ddoor_jiao + 10; i++)
            for (j = 100; j < chang /*-abs(y.ddoor_jiao-i)*/; j++)
            {
                zhizuobiao(&xx, &yy, i, j, 320, 240);
                if (erzhi_black[int(xx)][int(yy)] == 1)
                {
                    heishu++;
                }

                //		SetColor(xx,yy,baise);
            }
    }
    else if (a == 2)
    {
        duan = 50;
        chang = 120;
        for (i = y.ddoor_jiao - 180; i <= y.ddoor_jiao + 180; i++)
            for (j = duan; j < chang /*-abs(y.ddoor_jiao-i)*/; j++)
            {
                zhizuobiao(&xx, &yy, i, j, 320, 240);
                if (erzhi_black[int(xx)][int(yy)] == 1)
                {
                    heishu++;
                    //		SetColor(xx,yy,baise);
                }

                SetColor(xx, yy, baise);
            }
    }
    else if (a == 3)
    {
        duan = 50;
        chang = 120;
        for (i = 0 - 40; i <= 0; i++)
            for (j = duan; j < chang - abs(i); j++)
            {
                zhizuobiao(&xx, &yy, i, j, 320, 240);
                if (erzhi_black[int(xx)][int(yy)] == 1)
                {
                    heishu++;
                    //		SetColor(xx,yy,baise);
                }
                //	else
                //		SetColor(xx,yy,gray);
            }
    }
    else if (a == 4)
    {
        duan = 50;
        chang = 120;
        for (i = 0; i <= 0 + 40; i++)
            for (j = duan; j < chang - abs(i); j++)
            {
                zhizuobiao(&xx, &yy, i, j, 320, 240);
                if (erzhi_black[int(xx)][int(yy)] == 1)
                {
                    heishu++;
                    //		SetColor(xx,yy,baise);
                }
                //	else
                //		SetColor(xx,yy,baise);//gray);
            }
    }
    else if (a == 5)
    {
        duan = 30;
        chang = 55;
        for (i = -40; i <= 0 + 40; i++)
            for (j = duan; j < chang - abs(i); j += 2)
            {
                zhizuobiao(&xx, &yy, i, j, 320, 240);
                if (erzhi_black[int(xx)][int(yy)] == 1)
                {
                    heishu++;
                    //		SetColor(xx,yy,baise);
                }
                //	else
                //		SetColor(xx,yy,baise);//gray);
            }
    }
    //	CString str;
    //	str.Format("%d",heishu);
    //	AfxMessageBox(str);
    return heishu;
}

int UPRColor::sijiao(Yuzhi y)
{
    RGBTRIPLE baise;
    baise.rgbtBlue = 0xFF;
    baise.rgbtGreen = 0xFF;
    baise.rgbtRed = 0xFF;
    double xx;
    double yy;
    double xsc;
    double ysc;
    double dingweijiao;
    double dingweichang;
    int zuo = 0;
    int you = 0;
    for (xx = 1800; xx < 2000; xx += 10)
        for (yy = 450; yy < 750; yy += 10)
        {
            jizuobiao(xx, yy, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
            dingweijiao = -jiajiao1(-dingweijiao, y.benjijiao - 90);
            // 根据教练机给的球的位置,将这个坐标投影到视觉
            dingweichang = fanquxian[int(dingweichang * cdbl / 2.7)];
            zhizuobiao(&xsc, &ysc, dingweijiao, dingweichang, 320, 240);

            if (erzhi_black[int(xsc)][int(ysc)] == 1)
            {
                SetColor(int(xsc), int(ysc), baise);
                if (yy > 600)
                    you++;
                else
                    zuo++;
            }
        }
    if (you == zuo)
        return 2;
    if (you > zuo)
        return 0;
    else
        return 1;
}

double UPRColor::jiajiao1(double jizhun, double jiao)
{
    double jiajiao;
    jiajiao = jiao - jizhun;
    if (jiajiao > 180)
        jiajiao = -360 + jiajiao;
    if (jiajiao < -180)
        jiajiao = 360 + jiajiao;
    return jiajiao;
}

// 使用掩膜circle_maskX[circle_mask_long] circle_maskY[circle_mask_long]进行 需要区域的颜色域转换
BOOL UPRColor::GetHSIFix3(Yuzhi *zz)
{
    // 取球和两个门的重心坐标，并对相应的2值化数组赋值。
    // 返回：球和两个门的直角坐标以及极坐标。
    // 只对全景（中间方块范围内）的区域做。

    RGBTRIPLE door1;
    door1.rgbtBlue = 0xFF;
    door1.rgbtGreen = 0x00;
    door1.rgbtRed = 0x00;
    RGBTRIPLE ball;
    ball.rgbtBlue = 0x00;
    ball.rgbtGreen = 0x00;
    ball.rgbtRed = 0xFF;
    RGBTRIPLE door2;
    door2.rgbtBlue = 0x88;
    door2.rgbtGreen = 0xFF;
    door2.rgbtRed = 0xFF;
    RGBTRIPLE heimo;
    heimo.rgbtBlue = 0x00;
    heimo.rgbtGreen = 0x00;
    heimo.rgbtRed = 0x00;
    //	HSIParam  *hsi =new  HSIParam;
    RGBTRIPLE dimian;
    dimian.rgbtBlue = 0x00;
    dimian.rgbtGreen = 0xFF;
    dimian.rgbtRed = 0x00;
    RGBTRIPLE heise;
    heise.rgbtBlue = 0x4D;
    heise.rgbtGreen = 0x00;
    heise.rgbtRed = 0x4D;
    RGBTRIPLE baise;
    baise.rgbtBlue = 0xDD;
    baise.rgbtGreen = 0xDD;
    baise.rgbtRed = 0xFF;
    register int volh = 0;
    register int vols = 0;
    register int voli = 0;
    register int i = 0;
    register int j = 0;
    register int k = 0;
    register int l = 0;
    register int s = 0;
    register int t = 0;
    register int r, g, b, H, S, V, Vmin;
    int *mask_checked_box_address;
    // 将HSI颜色，放入HSI颜色的矩阵。
    k = 0;
    j = 0;
    for (i = 0; i < 480; i++)
    {
        SetColor(80, i, baise);
        SetColor(560, i, baise);
    }

    unsigned int *erzhi_bT = &erzhi_b[80][0];
    unsigned int *erzhi_diT = &erzhi_di[80][0];
    unsigned int *erzhi_yuandiT = &erzhi_yuandi[80][0];
    unsigned int *erzhi_blackT = &erzhi_black[80][0];
    unsigned int *erzhi_baiT = &erzhi_bai[80][0];

    ///*
    i = 0;
    while (i < 480 * 480)
    {
        *erzhi_bT++ = 0;
        *erzhi_diT++ = 0;
        *erzhi_yuandiT++ = 0;
        *erzhi_blackT++ = 0;
        *erzhi_baiT++ = 0;
        i++;
    }
    //*/
    if (zz->bhs > zz->bhx) // 切换红色 跨180度问题
    {
        // 不垮180

        while (s < Sector_Num)
        {
            t = sector_maskA_R[s][In_field_R_map[s][1]];
            l = sector_maskA_R[s][In_field_R_map[s][0]];
            mask_checked_box_address = sector_maskA[s][l];
            while (l < t)
            {
                // int
                // sector_mask[Sector_Num][(int)(OMNI_VISION_CAM_WIDTH*OMNI_VISION_CAM_WIDTH/Sector_Num*1.414)][2];//临时存放每个点的[0]y[1]x
                // int sector_mask_R[Sector_Num][OMNI_VISION_CAM_HEIGHT/2];//存放每个扇区的每个半径在掩模中的起始位置

                j = *mask_checked_box_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][0];//[y编号]
                                                 // [x编号] [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标]  棋盘格
                                                 // 生成掩膜用掩膜 用来生成 查找连通域撒种的区域
                i = *mask_checked_box_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][1];
                mask_checked_box_address++;
                //		SetColor(i,j,dimian);

                k = (j * OMNI_VISION_CAM_WIDTH + i) * 3;

                b = m_pImageBuf[k];
                g = m_pImageBuf[k + 1];
                r = m_pImageBuf[k + 2];
                //	b = *m_pImageBuf+k;
                //	g = *m_pImageBuf+k + 1;
                //	r = *m_pImageBuf+k + 2;
                if (b > g)
                {
                    if (r > b)
                    {
                        V = r;
                        Vmin = g; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=g//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 360 + (g - b) * 60 / (V - Vmin); // 0*60
                        }
                        else
                            H = 0;
                    }
                    else // V=b   Vmin=?
                    {
                        if (r < g)
                        {
                            V = b;
                            Vmin = r; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=r//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = b;
                            Vmin = g; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=g//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                    }
                }
                else
                {
                    if (r > g)
                    {
                        V = r;
                        Vmin = b; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=b//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 0 + (g - b) * 60 / (V - Vmin); // 0*60
                            if (H < 0)
                            {
                                H = H + 255; // H=H+360;
                            }
                        }
                        else
                            H = 0;
                    }
                    else // V=g   Vmin=?
                    {
                        if (r < b)
                        {
                            V = g;
                            Vmin = r; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=r//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = g;
                            Vmin = b; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=b//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                    }
                }
                S = V - Vmin; //*255/V;    //这里改了 原来是S=(V-Vmin)*255/V
                volh = H;
                vols = S;
                voli = V;
                // if ((((volh<zz->bhs && volh>zz->bhx)  ||  (volh+360<zz->bhs && volh+360>zz->bhx) || (volh-360<zz->bhs
                // && volh-360>zz->bhx)) && vols>zz->bs))
                if ((volh >= zz->bhx && volh <= zz->bhs) && vols >= zz->bs)
                {
                    //					erzhi_fushi[i][j]=1;
                    erzhi_b[i][j] = 1;
                    SetColor(i, j, ball);
                    //	SetColor(i-1,j,ball);
                    //	SetColor(i+1,j,ball);
                    //	SetColor(i,j-1,ball);
                    //	SetColor(i,j+1,ball);
                    //			erzhi_yuandi[i][j]=0;
                    //			erzhi_di[i][j]=0;
                    //			erzhi_black[i][j]=0;
                    //			erzhi_bai[i][j]=0;
                }
                else if (vols <= zz->sw && voli >= zz->iw)
                {
                    erzhi_bai[i][j] = 1;
                    //		SetColor(i,j,baise);
                    //			erzhi_yuandi[i][j]=0;
                    //		erzhi_di[i][j]=0;
                    //		erzhi_black[i][j]=0;
                }

                else if ((volh <= zz->chs && volh >= zz->chx) && vols >= zz->cs && voli <= zz->cis && voli >= zz->cix)
                {
                    erzhi_di[i][j] = 1;
                    erzhi_yuandi[i][j] = 1;
                    //					SetColor(i,j,dimian);
                    //			erzhi_b[i][j]=0;
                    //			erzhi_black[i][j]=0;
                    //			erzhi_bai[i][j]=0;
                }
                //	/*
                else if (vols <= zz->sh && voli <= zz->ih)
                {
                    erzhi_black[i][j] = 1;
                    erzhi_black[i - 1][j] = 1;
                    erzhi_black[i + 1][j] = 1;
                    erzhi_black[i][j - 1] = 1;
                    erzhi_black[i][j + 1] = 1;

#ifdef UPRColor_FUNCTION_DSPLAY // 设置显示 场地范围信息
                    SetColor(i, j, heise);
#endif
                    //			erzhi_yuandi[i][j]=0;
                    //			erzhi_b[i][j]=0;
                    //			erzhi_di[i][j]=0;
                    //			erzhi_bai[i][j]=0;
                }
                //	*/
                //		else
                //		{
                //			erzhi_b[i][j]=0;
                //			erzhi_di[i][j]=0;
                //			erzhi_yuandi[i][j]=0;
                //			erzhi_black[i][j]=0;
                //			erzhi_bai[i][j]=0;
                //		}
                //*/
                l++;
            }
            s++;
        }
        ///*
    }
    else
    {

        // int	num1;

        while (s < Sector_Num)
        {
            t = sector_maskA_R[s][In_field_R_map[s][1]];
            l = sector_maskA_R[s][In_field_R_map[s][0]];
            mask_checked_box_address = sector_maskA[s][l];
            while (l < t)
            {
                // int
                // sector_mask[Sector_Num][(int)(OMNI_VISION_CAM_WIDTH*OMNI_VISION_CAM_WIDTH/Sector_Num*1.414)][2];//临时存放每个点的[0]y[1]x
                // int sector_mask_R[Sector_Num][OMNI_VISION_CAM_HEIGHT/2];//存放每个扇区的每个半径在掩模中的起始位置

                j = *mask_checked_box_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][0];//[y编号]
                                                 // [x编号] [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标]  棋盘格
                                                 // 生成掩膜用掩膜 用来生成 查找连通域撒种的区域
                i = *mask_checked_box_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][1];
                mask_checked_box_address++;

                k = (j * OMNI_VISION_CAM_WIDTH + i) * 3;

                b = m_pImageBuf[k];
                g = m_pImageBuf[k + 1];
                r = m_pImageBuf[k + 2];
                //	b = *m_pImageBuf+k;
                //	g = *m_pImageBuf+k + 1;
                //	r = *m_pImageBuf+k + 2;
                if (b > g)
                {
                    if (r > b)
                    {
                        V = r;
                        Vmin = g; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=g//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 360 + (g - b) * 60 / (V - Vmin); // 0*60
                        }
                        else
                            H = 0;
                    }
                    else // V=b   Vmin=?
                    {
                        if (r < g)
                        {
                            V = b;
                            Vmin = r; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=r//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = b;
                            Vmin = g; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=g//A=4;B=(r-g);
                            if (V != Vmin)
                                H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                            else
                                H = 0;
                        }
                    }
                }
                else
                {
                    if (r > g)
                    {
                        V = r;
                        Vmin = b; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=b//A=0;B=(g-b);
                        if (V != Vmin)
                        {
                            H = 0 + (g - b) * 60 / (V - Vmin); // 0*60
                            if (H < 0)
                            {
                                H = H + 255; // H=H+360;
                            }
                        }
                        else
                            H = 0;
                    }
                    else // V=g   Vmin=?
                    {
                        if (r < b)
                        {
                            V = g;
                            Vmin = r; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=r//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                        else
                        {
                            V = g;
                            Vmin = b; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=b//A=2;B=(b-r);
                            if (V != Vmin)
                                H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                            else
                                H = 0;
                        }
                    }
                }
                S = V - Vmin; //*255/V;    //这里改了 原来是S=(V-Vmin)*255/V
                volh = H;
                vols = S;
                voli = V;
                // if ((((volh<zz->bhs && volh>zz->bhx)  ||  (volh+360<zz->bhs && volh+360>zz->bhx) || (volh-360<zz->bhs
                // && volh-360>zz->bhx)) && vols>zz->bs))
                if ((volh >= zz->bhx || volh <= zz->bhs) && vols >= zz->bs)
                {
                    //					erzhi_fushi[i][j]=1;
                    erzhi_b[i][j] = 1;
                    SetColor(i, j, ball);
                    //	SetColor(i-1,j,ball);
                    //	SetColor(i+1,j,ball);
                    //	SetColor(i,j-1,ball);
                    //	SetColor(i,j+1,ball);
                    //				erzhi_yuandi[i][j]=0;
                    //				erzhi_di[i][j]=0;
                    //				erzhi_black[i][j]=0;
                    //				erzhi_bai[i][j]=0;
                }
                else if (vols <= zz->sw && voli >= zz->iw)
                {
                    erzhi_bai[i][j] = 1;
                    //		SetColor(i,j,baise);
                    //				erzhi_yuandi[i][j]=0;
                    //			erzhi_di[i][j]=0;
                    //			erzhi_black[i][j]=0;
                }

                else if ((volh <= zz->chs && volh >= zz->chx) && vols >= zz->cs && voli <= zz->cis && voli >= zz->cix)
                {
                    erzhi_di[i][j] = 1;
                    erzhi_yuandi[i][j] = 1;
                    //		SetColor(i,j,dimian);
                    //				erzhi_b[i][j]=0;
                    //				erzhi_black[i][j]=0;
                    //				erzhi_bai[i][j]=0;
                }
                //	/*
                else if (vols <= zz->sh && voli <= zz->ih)
                {
                    erzhi_black[i][j] = 1;
                    erzhi_black[i - 1][j] = 1;
                    erzhi_black[i + 1][j] = 1;
                    erzhi_black[i][j - 1] = 1;
                    erzhi_black[i][j + 1] = 1;

#ifdef UPRColor_FUNCTION_DSPLAY // 设置显示 场地范围信息
                    SetColor(i, j, heise);
#endif
                    //				erzhi_yuandi[i][j]=0;
                    //				erzhi_b[i][j]=0;
                    //				erzhi_di[i][j]=0;
                    //				erzhi_bai[i][j]=0;
                }
                //	*/
                //			else
                //			{
                //				erzhi_b[i][j]=0;
                //				erzhi_di[i][j]=0;
                //				erzhi_yuandi[i][j]=0;
                //				erzhi_black[i][j]=0;
                //				erzhi_bai[i][j]=0;
                //			}
                //*/
                l++;
            }
            s++;
        }
        ///*
    }
    // 球2值化
    //	delete hsi;
    return TRUE;
}

const int Sector_B_Num = Sector_Num * 4;
int black_Sector_ORGdata_add[Sector_B_Num + 1][OMNI_VISION_CAM_HEIGHT / 2]; // 增量存储

int sector_maskB[Sector_B_Num][(int)(OMNI_VISION_CAM_WIDTH * OMNI_VISION_CAM_WIDTH / Sector_B_Num * 1.414)]
                [2];                                          // 存放每个点的[0]y[1]x
int sector_maskB_R[Sector_B_Num][OMNI_VISION_CAM_HEIGHT / 2]; // 存放每个扇区的每个半径在掩模中的起始位置
void UPRColor::Init_Mask_sectorBLACK(void)
{
    // int temp_mask[OMNI_VISION_CAM_HEIGHT][OMNI_VISION_CAM_WIDTH][2];//临时存放每个点的[0]扇区[1]半径
    // int temp_mask_L[OMNI_VISION_CAM_HEIGHT/2];//临时存放每个半径的有效存储长度
    // int sector_maskB_L[Sector_B_Num];//存放每个扇区的有效存储长度
    // 申请临时空间 用来按半径排序存储
    int ***temp_mask; //[OMNI_VISION_CAM_HEIGHT/2][OMNI_VISION_CAM_HEIGHT/2][2];//临时存放每个点的[0]x[1]y[2]l
    int i, j;
    temp_mask = new int **[OMNI_VISION_CAM_HEIGHT / 2];
    for (i = 0; i < OMNI_VISION_CAM_HEIGHT / 2; i++)
    {
        temp_mask[i] = new int *[OMNI_VISION_CAM_HEIGHT / 2];
    }
    for (i = 0; i < OMNI_VISION_CAM_HEIGHT / 2; i++)
    {
        for (j = 0; j < OMNI_VISION_CAM_HEIGHT / 2; j++)
        {
            temp_mask[i][j] = new int[3];
        }
    }
    int *temp_mask_L = new int[OMNI_VISION_CAM_HEIGHT / 2]; // 临时存放每个半径的有效存储长度
    int *sector_maskB_L = new int[Sector_B_Num];            // 存放每个扇区的有效存储长度

    long int sort[1000]; //*sort= new int[1000];//排序数组

    // 需要更快可以隔点扫描
    int x, y, s, l, r, t, sector, radius, n, m, temp;
    for (s = 0; s < Sector_B_Num; s++)
    {
        sector_maskB_L[s] = 0;
    }
    for (y = 0; y < OMNI_VISION_CAM_HEIGHT; y++)
    { //	   if(y%2==0)
        //				x=0;
        //			else
        //				x=1;
        for (x = 0; x < OMNI_VISION_CAM_WIDTH; x++)
        {
            //[所属扇区号]
            sector = atan2((x - MIRROR_CENTER_X), (y - MIRROR_CENTER_Y)) * Sector_B_Num / 6.283 + Sector_B_Num / 2;
            if (sector >= Sector_B_Num)
                sector = sector - Sector_B_Num;
            sector_maskB[sector][sector_maskB_L[sector]][0] = y;
            sector_maskB[sector][sector_maskB_L[sector]][1] = x;
            sector_maskB_L[sector]++;
            //	if ((sector>=Sector_B_Num)||(sector<0)) //测试越界代码
            //		 sector =sector;
            //	if ((sector==100))
            //		 sector= sector;
        }
    }
    for (s = 0; s < Sector_B_Num; s++)
    {
        for (r = 0; r < OMNI_VISION_CAM_HEIGHT / 2; r++)
        {
            temp_mask_L[r] = 0;
            sector_maskB_R[s][r] = 0; //
        }
        for (l = 0; l < sector_maskB_L[s]; l++)
        {
            y = sector_maskB[s][l][0];
            x = sector_maskB[s][l][1];
            // [半径]
            radius =
                sqrt((x - MIRROR_CENTER_X) * (x - MIRROR_CENTER_X) + (y - MIRROR_CENTER_Y) * (y - MIRROR_CENTER_Y));
            if (radius >= OMNI_VISION_CAM_HEIGHT / 2)
                continue;
            temp_mask[radius][temp_mask_L[radius]][0] = y;
            temp_mask[radius][temp_mask_L[radius]][1] = x;
            sector = (atan2((x - MIRROR_CENTER_X), (y - MIRROR_CENTER_Y)) * (float)(360) / 6.283 + (float)(360) / 2) *
                     1000; // atan2((y-MIRROR_CENTER_Y),(x-MIRROR_CENTER_X));
            if (sector >= 360 * 1000)
                sector = sector - 360 * 1000;
            temp_mask[radius][temp_mask_L[radius]][2] = sector;
            //	if (temp_mask[radius][temp_mask_L[radius]][2]==14400)
            //		temp_mask[radius][temp_mask_L[radius]][2]=temp_mask[radius][temp_mask_L[radius]][2];
            temp_mask_L[radius]++;
        }

        for (l = 0, r = 0; r < OMNI_VISION_CAM_HEIGHT / 2; r++)
        {
            for (t = 0; t < temp_mask_L[r]; t++)
            {
                sort[t] = t;
            }
            for (t = 0; t < temp_mask_L[r] - 1; t++)
            {
                for (m = 0; m < temp_mask_L[r] - 1 - t; m++)
                {
                    if (temp_mask[r][sort[m]][2] > temp_mask[r][sort[m + 1]][2])
                    {
                        temp = sort[m + 1];
                        sort[m + 1] = sort[m];
                        sort[m] = temp;
                    }
                }
            }
            for (t = 0; t < temp_mask_L[r]; t++)
            {
                sector_maskB[s][l][0] = temp_mask[r][sort[t]][0];
                sector_maskB[s][l][1] = temp_mask[r][sort[t]][1];
                sector_maskB[s][l][2] = temp_mask[r][sort[t]][2];
                l++;
            }
            sector_maskB_R[s][r] = l; //
        }
    }

    // 释放申请的临时空间
    for (i = 0; i < OMNI_VISION_CAM_HEIGHT / 2; i++)
    {
        for (j = 0; j < OMNI_VISION_CAM_HEIGHT / 2; j++)
        {
            delete temp_mask[i][j];
        }
    }
    for (i = 0; i < OMNI_VISION_CAM_HEIGHT / 2; i++)
    {
        delete temp_mask[i];
    }

    delete temp_mask;

    delete temp_mask_L;    // 临时存放每个半径的有效存储长度
    delete sector_maskB_L; // 存放每个扇区的有效存储长度

    int *sector_address;
    long black_Sector_ORGdata_add_p = 0;
    // 计算每个扇区点数
    for (r = 0; r < OMNI_VISION_CAM_HEIGHT / 2; r++) // Sector_B_Num
    {
        black_Sector_ORGdata_add_p = 0;
        black_Sector_ORGdata_add[0][r] = black_Sector_ORGdata_add_p;
        for (s = 0; s < Sector_B_Num; s++) // Sector_B_Num
        {
            t = sector_maskB_R[s][r + 1];
            i = sector_maskB_R[s][r];
            sector_address = sector_maskB[s][i];
            while (i < t)
            {
                y = *sector_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][0];//[y编号]
                                       // [x编号] [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标]  棋盘格 生成掩膜用掩膜
                                       // 用来生成 查找连通域撒种的区域
                x = *sector_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][1];
                black_Sector_ORGdata_add_p++;

                i++;
            }
            black_Sector_ORGdata_add[s + 1][r] = black_Sector_ORGdata_add_p;
        }
    }
}

int black_OBJ_L = 0;
float black_OBJ_A[1000];  // 记录空间1000个 但只使用 最近的10个做输出到教练机
float black_OBJ_R[1000];  // 记录空间1000个 但只使用 最近的10个做输出到教练机
float black_OBJ_W[1000];  // 记录空间1000个 但只使用 最近的10个做输出到教练机
float black_OBJ_fx[1000]; // 记录空间1000个 但只使用 最近的10个做输出到教练机
float black_OBJ_fy[1000]; // 记录空间1000个 但只使用 最近的10个做输出到教练机
int black_OBJ_sequence[1000];

int black_Sector_data[Sector_B_Num][OMNI_VISION_CAM_HEIGHT / 2];         // 扇区存在黑点数
int black_Sector_ORGdata[Sector_B_Num][OMNI_VISION_CAM_HEIGHT / 2];      // 扇区存在点数
int black_Sector_data_add[Sector_B_Num + 1][OMNI_VISION_CAM_HEIGHT / 2]; // 增量存储
int black_Sector_have_OBJ[Sector_B_Num];                                 // 扇区存在物体
int black_Sector_have_1_OBJ[Sector_B_Num];                               // 扇区存在物体
void UPRColor::getblackobj2(float f_x, float f_y, float f_a)
{
    in_obj_ll = 0;
    in_obj_l = 0;
    //	int S_start=10;
    //	int S_end=20;
    //	int R_min=100;
    //	int R_max=200;

    int s, r, t, i, x, y;
    int *sector_address;

    for (s = 0; s < Sector_B_Num; s++)
    {
        black_Sector_have_OBJ[s] = 0;
        black_Sector_have_1_OBJ[s] = 0;
    }

    black_OBJ_L = 0;

    int minR = 36;  // 10;
    int midR = 150; // 100;
    int maxR = 210;
    int BLong = 15;
    // 射线法 侧障碍物
    RGBTRIPLE ball2;
    //	ball1.rgbtBlue = 0xAA;	ball1.rgbtGreen = 0xAA; ball1.rgbtRed = 0xAA;
    float lianxuchang;
    int j = 0;
    int zuixiaojiajiao = 15;
    int jiao;
    int zuixiaoi = 15;
    int zuixiaoj = 0;
    //	int  x;
    //	int  y;
    //	int i;
    //	memset(erzhi_pz,0,640*480*sizeof(BOOL));
    int waichang;
    //	ball1.rgbtBlue = 0xAA;	ball1.rgbtGreen = 0xAA; ball1.rgbtRed = 0xAA;
    //	for(i=0;i<640;i++)
    //		for(j=0;j<480;j++)
    //			if(erzhi_black[i][j]==1)
    //			SetColor(i,j,ball1);
    ball2.rgbtBlue = 0xAA;
    ball2.rgbtGreen = 0xAA;
    ball2.rgbtRed = 00;
    in_obj_ll = 0;

    // 测量那些线有障碍物
    for (i = 0; i < Sector_Num; i++)
    {
        //		bishe[i]=0;
        //		black_Sector_have_OBJ[i]=0;
        lianxuchang = 0;
        //		waichang=wai;
        for (j = minR; j <= midR + 20; j++)
        {
            //		m_pImageBuf[shexiany[i][j]*1920 + shexianx[i][j]*3 + 0] = 0;//显示障碍测量射线
            //		m_pImageBuf[shexiany[i][j]*1920 + shexianx[i][j]*3 + 1] = 0;//显示障碍测量射线
            //		m_pImageBuf[shexiany[i][j]*1920 + shexianx[i][j]*3 + 2] = 255;//i*1.5;//显示障碍测量射线
            if (erzhi_black[shexianx[i][j]][shexiany[i][j]] == 1) // && erzhi_di[shexianx[i][j]][shexiany[i][j]]==1)
            {
                lianxuchang++;
                //				if(j==waichang && j<240)
                //					waichang++;
            }
            else
            {
                if (lianxuchang > 0)
                    //	lianxuchang=0;
                    lianxuchang -= 1.5;
            }

            if (lianxuchang > BLong)
            {

                //	bishe[i]=j-BLong+1;
                //				black_Sector_have_1_OBJ[i*4+3]=j-BLong+1;
                //				black_Sector_have_1_OBJ[i*4+2]=j-BLong+1;
                //				black_Sector_have_1_OBJ[i*4+1]=j-BLong+1;
                black_Sector_have_1_OBJ[i * 4 + 0] = j - BLong + 1;
                //	#ifdef UPRColor_FUNCTION_DSPLAY//显示每个障碍的十字标示
                DrawFocus(shexianx[i][j - BLong + 1], shexiany[i][j - BLong + 1], &ball2);
                //	#endif
                //	m_pImageBuf[shexiany[i][black_Sector_have_1_OBJ[i*4]]*1920 +
                // shexianx[i][black_Sector_have_1_OBJ[i*4]]*3 + 0] = 255;//rgb.rgbtRed;
                //	m_pImageBuf[shexiany[i][black_Sector_have_1_OBJ[i*4]]*1920 +
                // shexianx[i][black_Sector_have_1_OBJ[i*4]]*3 + 1] = 255;//rgb.rgbtRed;
                //	m_pImageBuf[shexiany[i][black_Sector_have_1_OBJ[i*4]]*1920 +
                // shexianx[i][black_Sector_have_1_OBJ[i*4]]*3 + 2] = 255;//rgb.rgbtRed;

                in_obj_org_lr[in_obj_ll] = black_Sector_have_1_OBJ[i * 4]; // 存储无畸变的数据
                in_obj_la[in_obj_ll] =
                    angle_add_transfer(i * 360.0 / (float)Sector_Num,
                                       180); // angle_add_transfer(0,-i*360/144);//输入本机测量得到的障碍物数组 全局变量
                in_obj_lr[in_obj_ll++] =
                    quxian[int(black_Sector_have_1_OBJ[i * 4])] / cdbl *
                    2.7; // quxian[bishe[i]];//quxian[i];////DrawFocus(shexianx[i][i],shexiany[i][i],&ball1);
                break;
            }
        }
    }

    long tempA, tempB, sfor, stfor, sttfor, sttadd, sttRadd, sttSadd, st, stt, rt, dr, ds, Rr;
    // 将障碍物测量射线精简成物体
    ///*
    int temp_r = 0;
    for (sfor = 0; sfor < Sector_B_Num; sfor += 4) // Sector_B_Num<<<<<<<<<<<<<<<----------------100改
    {
        if ((black_Sector_have_1_OBJ[sfor] > 0) && (black_Sector_have_1_OBJ[sfor] < midR + 2) &&
            (black_Sector_have_OBJ[sfor] == 0))
        {
            temp_r = black_Sector_have_1_OBJ[sfor];
            Rr = quxian[temp_r]; //                                                /cdbl*2.7;
            dr = 5 + 900 / Rr;   // r/Rr;
            // dr=3;//r/Rr;

            ds = 1 + 4100 / Rr; //(1+4500/Rr);
            if (ds > 80)
                ds = 80;
            sttadd = 0;
            sttRadd = 0;
            sttSadd = 0;
            for (sttfor = sfor; sttfor < sfor + ds; sttfor += 4) // Sector_B_Num
            {
                if (sttfor < Sector_B_Num)
                    stt = sttfor;
                else
                    stt = sttfor - Sector_B_Num;
                if (abs(temp_r - black_Sector_have_1_OBJ[stt]) < 30)
                {
                    sttadd++;
                    sttRadd += black_Sector_have_1_OBJ[stt];
                    sttSadd += sttfor;
                }
            }
            /*			if (sttadd*4>ds*0.80)
                        {
                            float black_OBJ_Angle=(float)sfor+(float)ds/2.0-2;
                            if ((int)black_OBJ_Angle > Sector_B_Num)
                                     black_OBJ_Angle = black_OBJ_Angle-(float)Sector_B_Num;
                            black_OBJ_Angle = black_OBJ_Angle*360.0/(float)Sector_B_Num;
                            black_OBJ_Angle = angle_add_transfer(black_OBJ_Angle,180);


                            black_OBJ_A[black_OBJ_L]=black_OBJ_Angle;//记录空间1000个 但只使用 最近的10个做输出到教练机
                            black_OBJ_R[black_OBJ_L]=sttRadd/sttadd;//记录空间1000个 但只使用 最近的10个做输出到教练机
                            black_OBJ_W[black_OBJ_L]=sttadd*4/ds*0.85;//记录空间1000个 但只使用 最近的10个做输出到教练机
                            black_OBJ_L++;

                            r=sttRadd/sttadd;
                            for (stfor=sfor;stfor<sfor+ds;stfor++)//Sector_B_Num
                            {
                                if (   stfor<Sector_B_Num)
                                    st=stfor;
                                else
                                    st=stfor-Sector_B_Num;
                                black_Sector_have_OBJ[st]=r;
                            //	black_Sector_have_OBJ[i*4+3]=sttRadd/sttadd;
                            //	black_Sector_have_OBJ[i*4+2]=sttRadd/sttadd;
                            //	black_Sector_have_OBJ[i*4+1]=sttRadd/sttadd;
                            //	black_Sector_have_OBJ[i*4+0]=sttRadd/sttadd;
                            }
                            sfor+=ds/4;//sttfor-s;//stfor-s+sttadd;
                            //continue;//break;
                        }
            */
            if (sttadd > 2) // 2.4米以内只要有2条射线就认为有 一个有效物体
            {
                float black_OBJ_Sector = (float)sttSadd / (float)sttadd; //(float)sfor+(float)ds/2.0-2;
                if ((int)black_OBJ_Sector > Sector_B_Num)
                    black_OBJ_Sector = black_OBJ_Sector - (float)Sector_B_Num;

                float black_OBJ_Angle = black_OBJ_Sector * 360.0 / (float)Sector_B_Num;
                black_OBJ_Angle = angle_add_transfer(black_OBJ_Angle, 180);

                float black_OBJ_radius = sttRadd / sttadd;

                if (sttfor >
                    Sector_B_Num) // 对机器尾部跨180 度的障碍 特殊处理 符合条件的去掉起始测量的第一个障碍 用当前障碍代替
                {
                    stt = sttfor - Sector_B_Num;
                    float black_OBJ_Angle_T = stt * 360.0 / (float)Sector_B_Num;
                    for (int black_OBJ_L_T = 0; black_OBJ_L_T < black_OBJ_L; black_OBJ_L_T++)
                    {
                        // 条件:被去除障碍物坐标角度在 当前障碍测量扇区内 且半径 差距不超过30
                        if ((black_OBJ_Angle_T > angle_add_transfer(black_OBJ_A[black_OBJ_L_T], 180)) &&
                            (fabs(black_OBJ_radius - black_OBJ_R[black_OBJ_L_T]) < 30))
                        {
                            for (int black_OBJ_L_TT = black_OBJ_L_T; black_OBJ_L_TT < black_OBJ_L - 1; black_OBJ_L_TT++)
                            {
                                black_OBJ_A[black_OBJ_L_TT] =
                                    black_OBJ_A[black_OBJ_L_TT + 1]; // 记录空间1000个 但只使用 最近的10个做输出到教练机
                                black_OBJ_R[black_OBJ_L_TT] =
                                    black_OBJ_R[black_OBJ_L_TT + 1]; // 记录空间1000个 但只使用 最近的10个做输出到教练机
                                black_OBJ_W[black_OBJ_L_TT] =
                                    black_OBJ_W[black_OBJ_L_TT + 1]; // 记录空间1000个 但只使用 最近的10个做输出到教练机
                            }
                            black_OBJ_L--;
                        }
                    }
                }

                black_OBJ_A[black_OBJ_L] = black_OBJ_Angle;        // 记录空间1000个 但只使用 最近的10个做输出到教练机
                black_OBJ_R[black_OBJ_L] = black_OBJ_radius;       // 记录空间1000个 但只使用 最近的10个做输出到教练机
                black_OBJ_W[black_OBJ_L] = sttadd * 4 / ds * 0.85; // 记录空间1000个 但只使用 最近的10个做输出到教练机
                black_OBJ_L++;

                r = sttRadd / sttadd;
                for (stfor = black_OBJ_Sector - ds / 2; stfor < black_OBJ_Sector + ds / 2; stfor++) // Sector_B_Num
                {
                    st = stfor;
                    if (stfor >= Sector_B_Num)
                        st = stfor - Sector_B_Num;
                    if (stfor < 0)
                        st = stfor + Sector_B_Num;
                    black_Sector_have_OBJ[st] = r;
                }
                sfor += (int(ds / 4)) * 4; // sttfor-s;//stfor-s+sttadd;
                // continue;//break;
            }
        }
    }
    //*/
    // 显示测出的障碍 占用的射线
    /*	for( i=0;i<Sector_Num;i++)
        {
            if(black_Sector_have_OBJ[i*4]>0)
            for(j=minR;j<=black_Sector_have_OBJ[i*4];j++)
            {
                m_pImageBuf[shexiany[i][j]*1920 + shexianx[i][j]*3 + 0] = 255;//rgb.rgbtRed;
                m_pImageBuf[shexiany[i][j]*1920 + shexianx[i][j]*3 + 1] = 255;//rgb.rgbtRed;
                m_pImageBuf[shexiany[i][j]*1920 + shexianx[i][j]*3 + 2] = 255;//i*1.5;//rgb.rgbtRed;
            }
        }
    */

    // 用扇形填充法测量 远处障碍
    long black_Sector_data_add_p = 0;
    // long black_Sector_ORGdata_add_p=0;
    // 计算每个扇区点数
    //	for (r=0;r<OMNI_VISION_CAM_HEIGHT/2;r++)//Sector_B_Num
    for (r = midR; r < maxR; r++) // Sector_B_Num
    {
        black_Sector_data_add_p = 0;
        //		black_Sector_ORGdata_add_p=0;
        black_Sector_data_add[0][r] = black_Sector_data_add_p;
        //		black_Sector_ORGdata_add[0][r]=black_Sector_ORGdata_add_p;
        for (s = 0; s < Sector_B_Num; s++) // Sector_B_Num
        {
            t = sector_maskB_R[s][r + 1];
            i = sector_maskB_R[s][r];
            sector_address = sector_maskB[s][i];
            while (i < t)
            {
                y = *sector_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][0];//[y编号]
                                       // [x编号] [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标]  棋盘格 生成掩膜用掩膜
                                       // 用来生成 查找连通域撒种的区域
                x = *sector_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][1];
                if (erzhi_black[x][y] == 1)
                {
                    black_Sector_data_add_p++;
                    //					m_pImageBuf[y*1920 + x*3	] = 255;//rgb.rgbtBlue;
                    //					m_pImageBuf[y*1920 + x*3 + 1] = 255;//rgb.rgbtGreen;
                    //					m_pImageBuf[y*1920 + x*3 + 2] = 0;//rgb.rgbtRed;
                }
                //				black_Sector_ORGdata_add_p++;
                //				erzhi_black[x][y]=0;//清掉二值化解果
                //		else
                //		{
                //			m_pImageBuf[y*1920 + x*3	] = m_pImageBuf[y*1920 + x*3	]/2;//rgb.rgbtBlue;
                //			m_pImageBuf[y*1920 + x*3 + 1] = s/2.5;//rgb.rgbtGreen;
                //			m_pImageBuf[y*1920 + x*3 + 2] = m_pImageBuf[y*1920 + x*3 + 2]/2;//rgb.rgbtRed;
                //		}
                i++;
            }
            black_Sector_data_add[s + 1][r] = black_Sector_data_add_p;
            //			black_Sector_ORGdata_add[s+1][r]=black_Sector_ORGdata_add_p;
        }
    }
    // 计算哪个扇区有障碍
    // return;
    int raddt = 2;
    for (r = midR; r < maxR; r += raddt) // r=r  Sector_B_Num
    {
        if (r < 180)
            raddt = 2;
        else
            raddt = 1;
        for (sfor = 0; sfor < Sector_B_Num; sfor++) // Sector_B_Num<<<<<<<<<<<<<<<----------------100改
        {

            if (black_Sector_have_OBJ[sfor] > 0)
                continue;

            if (sfor < Sector_B_Num)
                s = sfor;
            else
                s = sfor - Sector_B_Num;

            Rr = quxian[r];     //                                                      /cdbl*2.7;
                                //		dr=5+900/Rr;//r/Rr;
            dr = 3;             // r/Rr;
            ds = 1 + 4100 / Rr; // 1+4500/Rr;
            tempA = 0;
            tempB = 0;

            /*		for (stfor=s;stfor<Sector_B_Num;stfor++)//Sector_B_Num
                    {
                        if (   stfor<Sector_B_Num)
                            st=stfor;
                        else
                            st=stfor-Sector_B_Num;
                        if (black_Sector_have_OBJ[st]==0)
                        {
                            sfor+=stfor-s;
                            break;
                        }
                    }
                    if (sfor>=Sector_B_Num)
                        break;
                    if (  sfor<Sector_B_Num)
                        s=sfor;
                    else
                        s=sfor-Sector_B_Num;
                    //s=st;
            */

            sttadd = 0;
            for (sttfor = s; sttfor < s + ds; sttfor++) // Sector_B_Num
            {
                if (sttfor < Sector_B_Num)
                    stt = sttfor;
                else
                    stt = sttfor - Sector_B_Num;
                if (black_Sector_have_OBJ[stt] == 0)
                    sttadd++;
            }
            if (sttadd < ds * 0.85)
            {
                //	sfor+=ds;//sttfor-s;//stfor-s+sttadd;
                continue; // break;
            }
            // stfor+=ds;

            // 计算此 机体测试区域黑色点数
            if (s + ds < Sector_B_Num) // 不跨180度
            {
                for (rt = r; rt < r + dr; rt++) // Sector_B_Num
                {
                    tempA += black_Sector_data_add[s + ds][rt] - black_Sector_data_add[s][rt];
                    tempB += black_Sector_ORGdata_add[s + ds][rt] - black_Sector_ORGdata_add[s][rt];
                }
            }
            else // 跨180度
            {
                for (rt = r; rt < r + dr; rt++) // Sector_B_Num
                {
                    tempA += black_Sector_data_add[Sector_B_Num][rt] - black_Sector_data_add[s][rt];
                    tempB += black_Sector_ORGdata_add[Sector_B_Num][rt] - black_Sector_ORGdata_add[s][rt];
                }
                for (rt = r; rt < r + dr; rt++) // Sector_B_Num
                {
                    tempA += black_Sector_data_add[s + ds - Sector_B_Num][rt] - black_Sector_data_add[0][rt];
                    tempB += black_Sector_ORGdata_add[s + ds - Sector_B_Num][rt] - black_Sector_ORGdata_add[0][rt];
                }
            }

            if (tempA > tempB * 0.69) // 扇形 填充率>0.69 符合  //填充率>0.8 符合
            {
                sfor += ds;
                for (stfor = s; stfor < s + ds; stfor++) // Sector_B_Num
                {
                    if (stfor < Sector_B_Num)
                        st = stfor;
                    else
                        st = stfor - Sector_B_Num;
                    black_Sector_have_OBJ[st] = r;

                    ///*				//显示部分。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。
                    t = sector_maskB_R[st][r + dr];
                    i = sector_maskB_R[st][r];
                    sector_address = sector_maskB[st][i];
                    while (i < t)
                    {
                        y = *sector_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][0];//[y编号]
                                               // [x编号] [矩形内点编号] [矩形内点y坐标] [矩形内点x坐标]  棋盘格
                                               // 生成掩膜用掩膜 用来生成 查找连通域撒种的区域
                        x = *sector_address++; // mask_checked_box[mask_checked_box_y][mask_checked_box_x][i][1];
                        //		m_pImageBuf[y*1920 + x*3	] = 0;//255*(r%2);//rgb.rgbtBlue;
                        //		m_pImageBuf[y*1920 + x*3 + 1] = 0;//rgb.rgbtGreen;
                        //		m_pImageBuf[y*1920 + x*3 + 2] = 255;//rgb.rgbtRed;
                        if (erzhi_black[x][y] == 1)
                        {
                            m_pImageBuf[y * 1920 + x * 3] = 0;       // 255*(r%2);//rgb.rgbtBlue;
                            m_pImageBuf[y * 1920 + x * 3 + 1] = 0;   // rgb.rgbtGreen;
                            m_pImageBuf[y * 1920 + x * 3 + 2] = 255; // rgb.rgbtRed;
                        }
                        else
                        {
                            m_pImageBuf[y * 1920 + x * 3] = 0;     // 255*(r%2);//rgb.rgbtBlue;
                            m_pImageBuf[y * 1920 + x * 3 + 1] = 0; // rgb.rgbtGreen;
                            m_pImageBuf[y * 1920 + x * 3 + 2] = 0; // rgb.rgbtRed;
                        }
                        i++;
                    }
                    //*/				//显示部分。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。
                }
                float black_OBJ_Angle = (float)s + (float)ds / 2.0;
                if ((int)black_OBJ_Angle > Sector_B_Num)
                    black_OBJ_Angle = black_OBJ_Angle - (float)Sector_B_Num;
                black_OBJ_Angle = black_OBJ_Angle * 360.0 / (float)Sector_B_Num;
                black_OBJ_Angle = angle_add_transfer(black_OBJ_Angle, 180);

                black_OBJ_A[black_OBJ_L] = black_OBJ_Angle; // 记录空间1000个 但只使用 最近的10个做输出到教练机
                black_OBJ_R[black_OBJ_L] = r;               // 记录空间1000个 但只使用 最近的10个做输出到教练机
                black_OBJ_W[black_OBJ_L] = tempA / tempB;   // 记录空间1000个 但只使用 最近的10个做输出到教练机
                black_OBJ_L++;

                in_obj_org_lr[in_obj_ll] = r;                                  // 存储畸变的数据
                in_obj_la[in_obj_ll] = angle_add_transfer(black_OBJ_Angle, 0); // 输入本机测量得到的障碍物数组 全局变量
                in_obj_lr[in_obj_ll++] =
                    quxian[r] / cdbl *
                    2.7; // quxian[bishe[i]];//quxian[i];////DrawFocus(shexianx[i][i],shexiany[i][i],&ball1);

                //			in_obj_org_lr[in_obj_ll]=r;//存储畸变的数据
                //			in_obj_la[in_obj_ll]=angle_add_transfer(black_OBJ_Angle,
                //(float)ds/2.0*360.0/(float)Sector_B_Num);//输入本机测量得到的障碍物数组 全局变量
                //			in_obj_lr[in_obj_ll++]=quxian[r]/cdbl*2.7;//quxian[bishe[i]];//quxian[i];////DrawFocus(shexianx[i][i],shexiany[i][i],&ball1);

                //			in_obj_org_lr[in_obj_ll]=r;//存储畸变的数据
                //			in_obj_la[in_obj_ll]=angle_add_transfer(black_OBJ_Angle,-(float)ds/2.0*360.0/(float)Sector_B_Num);//输入本机测量得到的障碍物数组
                // 全局变量
                //			in_obj_lr[in_obj_ll++]=quxian[r]/cdbl*2.7;//quxian[bishe[i]];//quxian[i];////DrawFocus(shexianx[i][i],shexiany[i][i],&ball1);
                //	Learn_A[min_err_num]=angle_add_transfer(Learn_A[min_err_num],+Learn_A_t *0.36);
                //	Learn_R[min_err_num]=					Learn_R[min_err_num] +Learn_R_t *0.36;
            }
        }
    }
    // return;
    int n, m, o;
    RGBTRIPLE ball1;
    ball1.rgbtBlue = 0x00;
    ball1.rgbtGreen = 0xff;
    ball1.rgbtRed = 0xff;
    for (m = 0; m < Learn_num; m++)
    {
        if (m < black_OBJ_L)
        {
            Learn_A[m] = black_OBJ_A[m];
            Learn_R[m] = black_OBJ_R[m];
            Learn_Flag[m] = 10;
        }
        else
        {
            Learn_Flag[m] = 0;
        }
    }
    for (m = 0; m < black_OBJ_L; m++)
    {
        DrawFocus(coordinate_transfer_x(black_OBJ_A[m] + 90, -black_OBJ_R[m]) + 320,
                  coordinate_transfer_y(black_OBJ_A[m] + 90, -black_OBJ_R[m]) + 240, &ball1);
        //		black_OBJ_sequence[m]=m;
    }

    for (i = 0; i < black_OBJ_L; i++)
    {
        black_OBJ_fx[i] = l2f_x(f_x, f_y, f_a, -black_OBJ_A[i],
                                quxian[(int)black_OBJ_R[i]] / cdbl * 2.7); // 输入本机测量得到的障碍物数组 全局变量
        black_OBJ_fy[i] = l2f_y(f_x, f_y, f_a, -black_OBJ_A[i], quxian[(int)black_OBJ_R[i]] / cdbl * 2.7);
    }
    for (i = 0; i < in_obj_ll; i++)
    {
        float tla = -in_obj_la[i];
        float tlr = in_obj_lr[i];
        if (fabs(tla) < 20)
        {
            if (fabs(tlr) < 110) // 85.7647)//2015加入如果前方障碍物太近 带球时因为球在前方 且以球为路径规划起点
                                 // 会造成起点穿过障碍物造成不规避前方障碍物
                tlr = 110;
        }
        in_obj_x[i] = l2f_x(f_x, f_y, f_a, tla, tlr); // 输入本机测量得到的障碍物数组 全局变量
        in_obj_y[i] = l2f_y(f_x, f_y, f_a, tla, tlr);
        in_obj_w[i] = 1;

        //	in_obj_x[i]=l2f_x(f_x,f_y,f_a, -in_obj_la[i],in_obj_lr[i]);//输入本机测量得到的障碍物数组 全局变量
        //	in_obj_y[i]=l2f_y(f_x,f_y,f_a, -in_obj_la[i],in_obj_lr[i]);
        //	in_obj_w[i]=1;
    }
    in_obj_l = in_obj_ll;
    /*	for (m=1 ; m<black_OBJ_L ; m++)
        {
        for (n=black_OBJ_L-1 ; n>=m ; n--)
        {
            if (black_OBJ_R[black_OBJ_sequence[n]]>black_OBJ_R[black_OBJ_sequence[n-1]])
            {	o=black_OBJ_sequence[n-1];
                black_OBJ_sequence[n-1]=black_OBJ_sequence[n];
                black_OBJ_sequence[n]=o;
            }

        }
        }
    */
}
