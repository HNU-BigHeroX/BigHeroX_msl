// path plan路径规划
// Pp.cpp

// path_l注意 这里为兼容老程序 path_l取值范围 >=1 路径点[0]为目标点位置[path_l-1]为临近起点最近的路径点  需要的话
// 可以读取[path_l]点其中存储起点
#include "stdafx.h"

#ifndef Pp_cpp
#define Pp_cpp

#include "Pp.h"
float in_obj_la[10000];     // 输入本机测量得到障碍物数组 全局变量
float in_obj_lr[10000];     // 畸变校正过的
float in_obj_org_lr[10000]; // 无畸变校正的
int in_obj_ll;

float in_obj_x[10000]; // 输入生成花费地图用 障碍物数组 全局变量
float in_obj_y[10000];
float in_obj_w[10000]; // 障碍点范围
int in_obj_l;

char temp[100][100]; // 生成实心圆掩模用函数 用临时变量

int cmask_l = 0; // 生长用四连通域掩模
int cmask_x[100];
int cmask_y[100];

int cmask_l1 = 0; // X1倍的分辨率
int cmask_x1[100];
int cmask_y1[100];
int cmask_lf1 = 0; // X1倍的分辨率 亚像素
float cmask_xf1[100];
float cmask_yf1[100];
int cmask_l2 = 0; // X1/2/2倍的分辨率
int cmask_x2[100];
int cmask_y2[100];
int cmask_l4 = 0; // X1/4/4倍的分辨率
int cmask_x4[100];
int cmask_y4[100];
int omask_l0[80]; // 2~80 直径障碍物掩模
int omask_x0[80][80 * 80 * 4];
int omask_y0[80][80 * 80 * 4];
float omask_w0[80][80 * 80 * 4];

// 生成有方向 圆掩模用函数 x坐标数组 y 坐标数组 当前点与基准点偏差角度(0~180 对应 0~1) 数组长度
const int anglemask_num = 144;  // 144 个角度 方向掩模
int anglemask_l[anglemask_num]; // 144 个角度 方向掩模
int anglemask_x[anglemask_num][80 * 80 * 4];
int anglemask_y[anglemask_num][80 * 80 * 4];
float anglemask_w[anglemask_num][80 * 80 * 4];

char zoomR[80][80];
// 可选数值4 2 1 如果需要新的数值  在生长后回程的程序中增加
const unsigned char zoomR_max = 4; // 1;//1;//最大缩放倍数
const unsigned char zoomR_mid = 2; // 1;//1;//第二缩放倍数
const unsigned char zoomR_min = 2; // 1;//1;//2;//最小缩放倍数
const unsigned char zoomR_1 = 1;   // 1;//一般是不缩放为定值1

float tMapzoom[80 * 4][80 * 4]; // 存储本机周围 局部 近似值 放大 后的 时间花费

// 生长用变量
unsigned int marea_grow_data_arex[MAP_Y * MAP_X * 10]; //[640*480];//currently当前生长的一圈的纪录
unsigned int marea_grow_data_arey[MAP_Y * MAP_X * 10]; //[640*480];//currently当前生长的一圈的纪录
// 注意初始化后一定要用memory_space_init()传入存储空间
// 为生长用变量 从外部获取存储空间 如果要求方便就用 new申请得到 如果要速度就申请全局的静态空间
// void memory_space_init(unsigned int *dataX,unsigned int *dataY);
// unsigned int  *marea_grow_data_Parex;//[640*480];//currently当前生长的一圈的纪录
// unsigned int  *marea_grow_data_Parey;//[640*480];//currently当前生长的一圈的纪录

float mmap_tc_data[MAP_Y][MAP_X]; // 出发点达到每个格的花费时间
float oMap[MAP_Y][MAP_X];         // 每个格的花费
// float tMap[480][640];//出发点达到每个格的花费时间
float opMap[MAP_Y][MAP_X][3]; // 存储每个点的 父点的影射出发点 和向量方向 [父点y] [父点x] [方向]
// float aMap[MAP_Y][MAP_X];//存储每个点的 加减速参数 [父点y] [父点x] 影响类似 oMap[][]
// 1 生成oMap
// 2 先opMap第一点
// 3 父点存自己的

float foul_line_Map[MAP_Y][MAP_X];  // 边线的花费
float foul_line_2Map[MAP_Y][MAP_X]; // 边线的花费2
float foul_line_3Map[MAP_Y][MAP_X]; // 边线的花费2
float foul_line_4Map[MAP_Y][MAP_X]; // 边线的花费2

int path_l; // 存储路径点用
float path_x[10000];
float path_y[10000];

inline double Sinxx(double value)
{
    //	value*=1.5707963267948966192313216916398;
    //	value*=1.0;
    if (value < 0)
        value = -value;

    if (value < 1.0)
    {
        double temp = value * value;
        return 0.5 * temp * value - temp + 2.0 / 3.0;
    }
    else if (value < 2.0)
    {
        value = 2.0 - value;
        value *= value * value;
        return value / 6.0;
    }
    else
    {
        return 0.0;
    }
}
//  这是一个三次样条插值函数,获得每个像素点的值,一个800*600的话,采用循环即可实现
inline double Interpolate(double fx, double fy)
// inline double Bilinear02(float fx,float fy)
{
    //  返回值
    double doubleReturn;
    //  循环变量
    int i, j;
    int x = int(fx);
    int y = int(fy);
    fx = fx - x;
    fy = fy - y;

    int nScanWidth = MAP_X - 4;
    int nScanHeight = MAP_Y - 4;

    // 像素坐标
    int xx[4], yy[4];
    // 相邻四个像素的像素值
    double abyRed[4][4];

    xx[0] = -1;
    xx[1] = 0;
    xx[2] = 1;
    xx[3] = 2;
    yy[0] = -1;
    yy[1] = 0;
    yy[2] = 1;
    yy[3] = 2;

    // 保证合法
    if ((x - 1) < 0)
        xx[0] = 0;
    if ((x + 1) > (nScanWidth - 1))
        xx[2] = 0;
    if ((x + 2) > (nScanWidth - 1))
        xx[3] = ((xx[2] == 0) ? 0 : 1);

    if ((y - 1) < 0)
        yy[0] = 0;
    if ((y + 1) > (nScanHeight - 1))
        yy[2] = 0;
    if ((y + 2) > (nScanHeight - 1))
        yy[3] = ((yy[2] == 0) ? 0 : 1);

    // 像素点(x, y)的数据位置
    // 获取数据
    for (i = 0; i < 4; i++)
    {
        // 像素点(x, y)的数据位置
        //	BYTE* pbySrcBase = lpbySrcXY + yy[i] * nScanWidth * nBytesPerPixel;

        for (j = 0; j < 4; j++)
        {
            //	BYTE* pbySrc = pbySrcBase +  xx[j]*nBytesPerPixel;
            abyRed[i][j] = mmap_tc_data[y + yy[i]][x + xx[j]]; //*pbySrc;
        }
    }

    // u, v向量
    double afu[4], afv[4];

    afu[0] = Sinxx(1.0f + fx);
    afu[1] = Sinxx(fx);
    afu[2] = Sinxx(1.0f - fx);
    afu[3] = Sinxx(2.0f - fx);

    afv[0] = Sinxx(1.0f + fy);
    afv[1] = Sinxx(fy);
    afv[2] = Sinxx(1.0f - fy);
    afv[3] = Sinxx(2.0f - fy);

    // 矩阵乘向量的中间值
    double afRed[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            afRed[i] += afv[j] * abyRed[j][i];
        }
    }

    doubleReturn = afu[0] * afRed[0] + afu[1] * afRed[1] + afu[2] * afRed[2] + afu[3] * afRed[3];

    return doubleReturn;
}

// 生成浮点圆环掩模用函数
void CPathPlan::circle_maskGf(float radius, float *Gcmask_x, float *Gcmask_y, int Gcmask_lo)
{
    int i;
    float PI = 3.1415926535897932384626433832795;
    for (i = 0; i < Gcmask_lo; i++)
    {
        Gcmask_x[i] = cos(i * 2.0 * PI / float(Gcmask_lo)) * radius;
        Gcmask_y[i] = -sin(i * 2.0 * PI / float(Gcmask_lo)) * radius;
    }
}

#ifdef USE_OpenCv // 如果使用Open_Cv

int MouseEvent_s_x = 0;
int MouseEvent_s_y = 0;
int MouseEvent_t_x = 0;
int MouseEvent_t_y = 0;
int MouseEvent_L_R = 'L'; // 按下过左键还是右键 //默认是按下过左键

// 主显示窗口上的鼠标事件 用于测试用
void OpenCv_MouseEvent(int event, int x, int y, int flags, void *param)
{
    IplImage *img = (IplImage *)param;
    switch (event)
    {
    case CV_EVENT_MOUSEMOVE: {
        if (MouseEvent_L_R == 'L')
        {
            MouseEvent_s_x = x;
            MouseEvent_s_y = y;
        }
        if (MouseEvent_L_R == 'R')
        {
            MouseEvent_t_x = x;
            MouseEvent_t_y = y;
        }
    }
    break;
    case CV_EVENT_LBUTTONDOWN: {
        MouseEvent_L_R = 'L';
    }
    break;
    case CV_EVENT_LBUTTONUP:
        break;
    case CV_EVENT_RBUTTONDOWN: {
        MouseEvent_L_R = 'R';
    }
    break;
    }
}

// 配合本类构造函数 进行OpenCV兼容部分初始化
void CPathPlan::OpenCv_CPathPlan(void)
{
    // 用来显示文字,初始化字体
    cvInitFont(&out_txt, CV_FONT_VECTOR0, 1, 1, 0, 1, 8);

    int x, y, i, j;
    char *data, *data2, *data3;

    src_S = 0; // 定义源图像指针

    src = cvLoadImage("./Config/rice.bmp", 1); // 定义输出和运算用图像指针
    src->origin = 1;
    src_zoomR = cvLoadImage("./Config/zoomR.bmp", 1);       // 定义变分辨率源图像指针
    IplImage *mask2 = cvLoadImage("./Config/mask2.bmp", 1); // 不处理的 （机体）部分图像
    IplImage *mask3 = cvLoadImage("./Config/mask3.bmp", 1); // 场地边界花费掩膜 图像
    IplImage *mask4 = cvLoadImage("./Config/mask4.bmp", 1); // 场地边界花费掩膜 图像
    IplImage *mask5 = cvLoadImage("./Config/mask5.bmp", 1); // 场地边界花费掩膜 图像
    IplImage *mask6 = cvLoadImage("./Config/mask6.bmp", 1); // 场地边界花费掩膜 图像
    data = mask2->imageData;                                // 生成 需要 处理图像区域的最小半径
    for (i = 0; i < 144; i++)
        robot_min_r[i] = 0;
    for (x = 0; x < 640; x++)
        for (y = 0; y < 480; y++)
        {
            if ((data[(y * mask2->width + x) * 3 + 0] != -1) || (data[(y * mask2->width + x) * 3 + 1] != -1) ||
                (data[(y * mask2->width + x) * 3 + 2] != -1))
            {
                i = -atan2(x - 320, y - 240) * 144 / 6.28;
                if (i >= 144)
                    i -= 144;
                if (i < 0)
                    i += 144;
                j = sqrt((x - 320) * (x - 320) + (y - 240) * (y - 240));
                if (robot_min_r[i] < j)
                    robot_min_r[i] = j;
            }
        }
    data = mask3->imageData; // 场地边界花费掩膜 图像
    for (x = 0; x < MAP_X; x++)
    {
        for (y = 0; y < MAP_Y; y++)
        {
            foul_line_Map[y][x] =
                255.0 / ((float)((unsigned char)(data[(y * mask3->width + x) * 3 + 0])) + 10); // 边线的花费
        }
    }
    data = mask4->imageData; // 场地边界花费掩膜 图像
    for (x = 0; x < MAP_X; x++)
    {
        for (y = 0; y < MAP_Y; y++)
        {
            foul_line_2Map[y][x] =
                255.0 / ((float)((unsigned char)(data[(y * mask4->width + x) * 3 + 0])) + 10); // 边线的花费
        }
    }
    data = mask5->imageData; // 场地边界花费掩膜 图像
    for (x = 0; x < MAP_X; x++)
    {
        for (y = 0; y < MAP_Y; y++)
        {
            foul_line_3Map[y][x] =
                255.0 / ((float)((unsigned char)(data[(y * mask5->width + x) * 3 + 0])) + 10); // 边线的花费
        }
    }
    data = mask6->imageData; // 场地边界花费掩膜 图像
    for (x = 0; x < MAP_X; x++)
    {
        for (y = 0; y < MAP_Y; y++)
        {
            foul_line_4Map[y][x] =
                255.0 / ((float)((unsigned char)(data[(y * mask6->width + x) * 3 + 0])) + 10); // 边线的花费
        }
    }
    cvNamedWindow("src_Pp", CV_WINDOW_AUTOSIZE); // 开主显示窗口
    cvSetMouseCallback("src_Pp", OpenCv_MouseEvent,
                       (void *)src); // 设置主显示窗口上的鼠标事件 用于测试用 Set the callback function of mouse event
    src_S = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 3);
    data = src->imageData;
    data2 = src_S->imageData;
    i = 0;
    for (x = 0; x < src->width; x++)
    {
        for (y = 0; y < src->height; y++)
        {
            data2[i] = data[i];
            i++;
            data2[i] = data[i];
            i++;
            data2[i] = data[i];
            i++;
        }
    }
    data = src->imageData;
    data2 = src_S->imageData;

    data3 = src_zoomR->imageData;
    unsigned char R, G, B;
    for (x = 0; x < src_zoomR->width; x++)
    {
        for (y = 0; y < src_zoomR->height; y++)
        {
            R = data3[(y * src_zoomR->width + x) * 3 + 0];
            G = data3[(y * src_zoomR->width + x) * 3 + 1];
            B = data3[(y * src_zoomR->width + x) * 3 + 2];
            if ((R == 0) && (G == 0) && (B == 0))
                zoomR[y][x] = zoomR_max; // zoomR_max;//8;
            if ((R > 1))
                zoomR[y][x] = zoomR_mid; // 4;//4;
            if ((B > 1))
                zoomR[y][x] = zoomR_min; // 2;//2;
            if ((G > 1))
                zoomR[y][x] = zoomR_1; // 1;
        }
    }
}

void CPathPlan::OpenCv_Del_CPathPlan(void)
{
    ;
}

void CPathPlan::OpenCv_DrawPoint(int x, int y, unsigned char R, unsigned char G, unsigned char B, IplImage *img)
{
    //	y=img->height-y;
    if (x > img->width)
        x = img->width;
    if (x < 0)
        x = 0;
    if (y > img->height)
        y = img->height;
    if (y < 0)
        y = 0;
    char *data = img->imageData + (img->width * y + x) * 3;
    *data++ = B;
    *data++ = G;
    *data++ = R;
}

BOOL CPathPlan::OpenCv_Draw_line(int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b,
                                 IplImage *img)
{
    //	if (x1>m_nWidth-1 || y1>m_nHeight-1 || x1<0 || y1<0 || x2>m_nWidth-1 || y2>m_nHeight-1 || x2<0 || y2<0)
    //		return FALSE;;
    float i, j;
    j = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    if (j <= 0)
        return FALSE;
    float x = x1;
    float y = y1;
    float xadd = (x2 - x1) / j;
    float yadd = (y2 - y1) / j;
    for (i = 0; i <= j; i++)
    {
        OpenCv_DrawPoint(x, y, r, g, b, img); // 画线段
        x += xadd;
        y += yadd;
    }
    return TRUE;
}

// 生成圆环掩模用函数
void CPathPlan::OpenCv_circle(int x, int y, float R, int rr, int g, int b, IplImage *img)
{ // int R=3;//10;//5;
    int Gcmask_l = 0;
    // 半径2 和2以上的 圆都正确   注意使用[方法2：尾点]结尾得到的半径2的圆正好和 四连通的半径1的圆配套吻合
    // 注意使用[方法1：尾点]结尾得到的半径2的圆正好是八连通的半径1的圆 半径2的圆正好是16连通的方块
    float r;
    int i;
    r = R / 1.414;
    //	Gcmask_x[Gcmask_l]=r;
    //	Gcmask_y[Gcmask_l++]=sqrt(R*R-r*r);
    //	OpenCv_DrawPoint(x+r,y+sqrt(R*R-r*r), rr,g,b , img);//画线段
    for (r = R / 1.414 - 1; r > 1; r--)
    {
        //		 Gcmask_x[Gcmask_l]=r;
        //		 Gcmask_y[Gcmask_l++]=sqrt(R*R-r*r);
        OpenCv_DrawPoint(x + r, y + sqrt(R * R - r * r), rr, g, b, img); // 画线段
        OpenCv_DrawPoint(x - sqrt(R * R - r * r), y + r, rr, g, b, img); // 画线段
        OpenCv_DrawPoint(x - r, y - sqrt(R * R - r * r), rr, g, b, img); // 画线段
        OpenCv_DrawPoint(x + sqrt(R * R - r * r), y - r, rr, g, b, img); // 画线段

        //		 Gcmask_y[Gcmask_l]=r;
        //		 Gcmask_x[Gcmask_l++]=sqrt(R*R-r*r);
        OpenCv_DrawPoint(x + sqrt(R * R - r * r), y + r, rr, g, b, img); // 画线段
        OpenCv_DrawPoint(x + r, y - sqrt(R * R - r * r), rr, g, b, img); // 画线段
        OpenCv_DrawPoint(x - sqrt(R * R - r * r), y - r, rr, g, b, img); // 画线段
        OpenCv_DrawPoint(x - r, y + sqrt(R * R - r * r), rr, g, b, img); // 画线段
    }
    // 方法1：尾点
    //	Gcmask_x[Gcmask_l]=r;
    //	Gcmask_y[Gcmask_l++]=sqrt(R*R-r*r);
    // 方法2：尾点
    //	Gcmask_x[Gcmask_l]=R;
    //	Gcmask_y[Gcmask_l++]=0;
}
#endif

long map_fc_data[MAP_Y][MAP_X];  // 场地每个格的行走花费 cost数据
float map_tc_data[MAP_Y][MAP_X]; // 生长到此格花费的时间 cost数据
float map_gd_data[MAP_Y][MAP_X]; // 梯度 方向数据 grads direction

CPathPlan::CPathPlan(void)
{
    dataInPerset();                           // 将路径规划设置和花费地图设置 变为预设定值
    Obstacle_size_set = OBSTACLE_SIZE_PERSET; // 10;体育馆//15;//22;//使用 多少半径进行标识 障碍点
    s_x = 0;                                  // 路径启始坐标
    s_y = 0;
    t_x = 0; // 路径目的地标坐标
    t_y = 0;

    LayerMin = 0;  // 当前层 花费最小的
    GLayerMax = 0; // 当前层 可达到最大花费
    path_plan_angle_P = 0;
    Close_target_direction = 0;
    path_plan_start_angle_cost = 0;

    circle_mask(); // 生成此类需要使用所有圆环掩模
    for (int nm = 0; nm < anglemask_num; nm++)
    {
        circle_maskGAA(30, 360.0 * (float)(nm) / (float)(anglemask_num), anglemask_x[nm], anglemask_y[nm],
                       anglemask_w[nm], &anglemask_l[nm]);
    }

#ifdef USE_OpenCv       // 如果使用Open_Cv
    OpenCv_CPathPlan(); // 配合本类构造函数 进行OpenCV兼容部分初始化
#endif

    int x, y;
    for (x = 0; x < MAP_X; x++)
    {
        for (y = 0; y < MAP_Y; y++)
        {
            map_fc_data[y][x] = 0; // 场地每个格的行走花费 cost数据
            map_tc_data[y][x] = 0; // 生长到此格花费的时间 cost数据
            map_gd_data[y][x] = 0; // 梯度 方向数据 grads direction
        }
    }

    ftb_circle_ox = 0; // 三米圆中心
    ftb_circle_oy = 0; // 三米圆中心
}

CPathPlan::~CPathPlan(void)
{
#ifdef USE_OpenCv           // 如果使用Open_Cv
    OpenCv_Del_CPathPlan(); // 配合本类反构造函数 进行OpenCV兼容部分初始化
#endif
    //	cvDestroyWindow("src_Pp");//
}

// 会把需要的目标方向用方向掩膜加进花费地图
// 需要使用 在执行完 datain()后执行
void CPathPlan::Close_target_direction_01(float ttx, float tty, float directx, float Direct_Cost)
{
    int txx, tyy;
    float zoomX = (float)MAP_X / (float)MAP_Xmm;
    float zoomY = (float)MAP_Y / (float)MAP_Ymm;
    txx = (ttx + MAP_oX) * zoomX;
    tyy = (tty + MAP_oY) * zoomY;

    Close_target_direction = directx;
    int Pangle = Close_target_direction * anglemask_num / 360; // 显示方向花费掩膜
    if (Pangle >= anglemask_num)
        Pangle -= anglemask_num;
    if (Pangle < 0)
        Pangle += anglemask_num;
    int tx, ty;
    for (int nm = 0; nm < anglemask_l[Pangle]; nm++)
    {
        tx = anglemask_x[Pangle][nm] + txx;
        ty = anglemask_y[Pangle][nm] + tyy;
        if ((tx >= 0) && (tx < MAP_X) && (ty >= 0) && (ty < MAP_Y))
        {
            oMap[ty][tx] += Direct_Cost * anglemask_w[Pangle][nm];
            if (oMap[ty][tx] < 0)
                oMap[ty][tx] = 0;
        }
    }
}
void CPathPlan::add_obstacle_point(float x, float y, float weight) // evade设置躲避指定点  默认x y场地坐标 weight大小
{
    //	if (use_evade_ball==1)//设置躲避球  默认0不使用  1为使用
    in_obj_w[in_obj_l] = weight;
    in_obj_x[in_obj_l] = x;
    in_obj_y[in_obj_l++] = y;
}
// 注意 做一个 缩放系数 在离目标足够近时 使用放大过的花费地图【范围缩小】 进行同数量网格路径规划 使路径分辨率提高
void CPathPlan::datain(float ssx, float ssy, float ttx, float tty) // 障碍地图数据输入部分
                                                                   // 且输入目标位置[场地直角坐标系]
{
    register int x, y, i, j, k, tx, ty;
    unsigned char temp;
    float zoomX = (float)MAP_X / (float)MAP_Xmm;
    float zoomY = (float)MAP_Y / (float)MAP_Ymm;
    // #define MAP_oX 100//本地图直角坐标系原点坐标
    // #define MAP_oY 100//
    s_x = (ssx + MAP_oX) * zoomX;
    s_y = (ssy + MAP_oY) * zoomY;
    t_x = (ttx + MAP_oX) * zoomX;
    t_y = (tty + MAP_oY) * zoomY;

    for (x = 0; x < MAP_X; x++)
    {
        for (y = 0; y < MAP_Y; y++)
        {
            oMap[y][x] = 1;
            mmap_tc_data[y][x] = 1000000; // 场地每个格的行走花费 cost数据
            //			aMap[y][x]=1;//存储每个点的 加减速参数 [父点y] [父点x] 影响类似 oMap[][]
        }
    }
    if (use_film_line == 5)
        film_line2014_3mcircle(2);
    if (use_film_line == 6)
        film_line2014_3mcircle(3);
    // 障碍物地图生成
    for (k = 0; k < in_obj_l; k++) // 为了 加速可以跳过一半点
    {
        if (fabs(in_obj_w[k]) > 20)
            continue;
        i = sqrt(sqrt(in_obj_w[k])) * Obstacle_size_set; // 10;体育馆//15;//22;//使用 多少半径进行标识 障碍点
        j = 0;
        while (j < omask_l0[i])
        {
            ty = omask_y0[i][j] + in_obj_y[k] * zoomY + MAP_oY * zoomY; // 输入生成花费地图用 障碍物数组 全局变量
            tx = omask_x0[i][j] + in_obj_x[k] * zoomX + MAP_oX * zoomX;
            if ((tx >= 0) && (tx < MAP_X) && (ty >= 0) && (ty < MAP_Y))
            {                                 //   oMap[ty][tx]=0;
                if (omask_w0[i][j] < i * 0.2) // 在0.2倍直径内 都是可行走度为0
                    oMap[ty][tx] = 0;
                else if (oMap[ty][tx] >
                         (omask_w0[i][j] - i * 0.2) / i / (1 - 0.2)) // 在0.2倍直径到1倍直径 可行走度为0到1
                    oMap[ty][tx] = (omask_w0[i][j] - i * 0.2) / i / (1 - 0.2);
            }
            j++;
        }
    }

    for (x = 0; x < MAP_X; x++)
    {
        for (y = 0; y < MAP_Y; y++)
        //	{	oMap[y][x]=1000/(oMap[y][x]*1000+0.1);//2011
        {
            oMap[y][x] = 1 / (oMap[y][x] + 0.1); // 2010原代码
        }
    }
    // 场地边界花费掩膜 图像mask3.bmp
    if (use_film_line == 1)
    {
        for (x = 0; x < MAP_X; x++)
        {
            for (y = 0; y < MAP_Y; y++)
            {
                oMap[y][x] += foul_line_Map[y][x]; // 边线的花费
            }
        }
    }
    // 场地边界花费掩膜 图像mask4.bmp 防守队员用边界掩膜
    if (use_film_line == 2)
    {
        for (x = 0; x < MAP_X; x++)
        {
            for (y = 0; y < MAP_Y; y++)
            {
                oMap[y][x] += foul_line_2Map[y][x]; // 边线的花费
            }
        }
    }
    // 场地边界花费掩膜 图像mask4.bmp 防守队员用边界掩膜
    if (use_film_line == 3)
    {
        for (x = 0; x < MAP_X; x++)
        {
            for (y = 0; y < MAP_Y; y++)
            {
                oMap[y][x] += foul_line_3Map[y][x]; // 边线的花费
            }
        }
    }
    if (use_film_line == 4)
    {
        for (x = 0; x < MAP_X; x++)
        {
            for (y = 0; y < MAP_Y; y++)
            {
                oMap[y][x] += foul_line_4Map[y][x]; // 边线的花费
            }
        }
    }
    /*测试
        char *data2=src_S->imageData;
        int x,y,i,j,tx,ty;
        unsigned char temp;
        for (x=0;x<MAP_X;x++)
        {	for (y=0;y<MAP_Y;y++)
            {	oMap[y][x]=1;
        }	}
        for (x=0;x<MAP_X;x++)
        {	for (y=0;y<MAP_Y;y++)
            {	mmap_tc_data[y][x]=1000000; //场地每个格的行走花费 cost数据
                temp=(unsigned char) (data2[(src_S->width*y+x)*3]);
                oMap[y][x]=temp/256.0;
    //			aMap[y][x]=1;//存储每个点的 加减速参数 [父点y] [父点x] 影响类似 oMap[][]
            }
        }
        for (x=0;x<MAP_X;x++)
        {	for (y=0;y<MAP_Y;y++)
            {	oMap[y][x]=1/(oMap[y][x]+0.1);
        }	}
    */
    dataInPerset(); // 将路径规划设置和花费地图设置 变为预设定值
}

#ifdef USE_OpenCv             // 如果使用Open_Cv
void CPathPlan::datainT(void) // 障碍地图数据输入部分 使用Open_Cv输入图片测试
{
    char *data2 = src_S->imageData;
    int x, y, i, j, tx, ty;
    unsigned char temp;
    for (x = 0; x < MAP_X; x++)
    {
        for (y = 0; y < MAP_Y; y++)
        {
            oMap[y][x] = 1;
        }
    }
    for (x = 0; x < MAP_X; x++)
    {
        for (y = 0; y < MAP_Y; y++)
        {
            mmap_tc_data[y][x] = 1000000; // 场地每个格的行走花费 cost数据
            temp = (unsigned char)(data2[(src_S->width * y + x) * 3]);
            //	oMap[y][x]=1-temp/256.0 + 0.5;
            //	oMap[y][x]=(256.0+0.1)/(temp + 0.10);
            oMap[y][x] = temp / 256.0;
            //			aMap[y][x]=1;//存储每个点的 加减速参数 [父点y] [父点x] 影响类似 oMap[][]
            /*
                    if (temp<=20)
                    {	//障碍物地图生成
                        i=0;while(i<10)
                        {	j=0;while(j<omask_l0[i])
                            {
                                     oMap[omask_y0[i][j]+y][omask_x0[i][j]+x]=0;
                                j++;
                            }
                            i++;
                        }
                        while(i<20)
                        {	j=0;while(j<omask_l0[i])
                            {	if (oMap[omask_y0[i][j]+y][omask_x0[i][j]+x]>((i-10)*0.1))
                                    oMap[omask_y0[i][j]+y][omask_x0[i][j]+x]= (i-10)*0.1;
                                j++;
                            }
                            i++;
                        }
                    }
                */
            /*
                        if (temp<=20)
                        {	//障碍物地图生成
                            i=20;
                            j=0;while(j<omask_l0[i])
                            {
                                ty=	omask_y0[i][j]+y;
                                tx=	omask_x0[i][j]+x;
                                if ((tx>=0)&&(tx<MAP_X)&&(ty>=0)&&(ty<MAP_Y))
                                {//   oMap[ty][tx]=0;
                                    if(omask_w0[i][j]<i*0.2)//在0.2倍直径内 都是可行走度为0
                                       oMap[ty][tx]=0;
                                    else
                                    if(oMap[ty][tx]>(omask_w0[i][j]-i*0.2)/i/(1-0.2))//在0.2倍直径到1倍直径
               可行走度为0到1 oMap[ty][tx]=(omask_w0[i][j]-i*0.2)/i/(1-0.2);
                                }
                                j++;
                            }
                        }
            */
        }
    }

    //	for (x=0;x<MAP_X;x++)//显示场地花费地图
    //		for (y=0;y<MAP_Y;y++)
    //			OpenCv_DrawPoint(x,y,oMap[y][x]*255,0,0,src);
    //    cvShowImage( "src_Pp", src );
    //	cvWaitKey(0);//if (cvWaitKey(0) == 'q')		return;

    for (x = 0; x < MAP_X; x++)
    {
        for (y = 0; y < MAP_Y; y++)
        {
            oMap[y][x] = 1 / (oMap[y][x] + 0.1);
        }
    }

    //	for (x=0;x<MAP_X;x++)//显示场地花费地图
    //		for (y=0;y<MAP_Y;y++)
    //			OpenCv_DrawPoint(x,y,0,oMap[y][x]*10,0,src);
    //    cvShowImage( "src_Pp", src );
    //	if (cvWaitKey(0) == 'q')		return;
}
#endif

void CPathPlan::dataout(char *data, int xlong, int ylong) // 障碍地图数据输入部分
{
    int x, y;
    for (x = 0; x < xlong; x++)
    {
        for (y = 0; y < ylong; y++)
        {
            if (map_tc_data[y][x] == 1)
                data[(xlong * y + x) * 3] = 255; // 场地每个格的行走花费 cost数据
            else
                data[(xlong * y + x) * 3] = 0; // 场地每个格的行走花费 cost数据

            data[(xlong * y + x) * 3 + 1] = (int)map_tc_data[y][x]; // 场地每个格的行走花费 cost数据
        }
    }
}

// 添加一个节点
inline void CPathPlan::C_nod(int x, int y, float mmap_tc_data) // 创建 插入一个节点
{
    e_Nod->x = x;
    e_Nod->y = y;
    e_Nod->mmap_tc_data = mmap_tc_data;
    //	e_Nod->previous=e_Nod;
    //	e_Nod->next=NULL;
    e_Nod->next = new Over_Grow_Nod;
    //	e_Nod->next->previous = e_Nod;

    e_Nod = e_Nod->next;
}

// 删除 去掉一个节点 返回*nod的下一个节点 *Pnod是*nod的上一个节点
inline Over_Grow_Nod *CPathPlan::D_nod(Over_Grow_Nod *Pnod, Over_Grow_Nod *nod)
{
    Over_Grow_Nod *temp = nod->next;
    //	nod->previous->next = nod->next;
    //	nod->next->previous = nod->previous;
    Pnod->next = nod->next;
    delete nod;
    return (temp);
}

// 靠类似折射反射 原理 生成花费

// 未完成: 加入 速度对变化角度的影响
//         应该给个 介质中传输速度上限和下限
//         加入 当前点速度问题 花费是加减速
inline float CPathPlan::refraction02(int fx, int fy, int x, int y)
{
    // aMap[y][x]=aMap[fy][fx];
    if (mmap_tc_data[fy][fx] >= mmap_tc_data[y][x])
        return (0);

    float xinS = opMap[fy][fx][1];
    float yinS = opMap[fy][fx][0];
    float xin = xinS - x;
    float yin = yinS - y;
    // float in_A   =atan2(xin,yin);//  入射角度与x轴的弧度
    float in_R = sqrt(xin * xin + yin * yin); //  入射角度与x轴的弧度 可以把这个值保存下来

    float temp;
    // if((oMap[Bx][By]-grads_R)!=0)

    if (in_R == 0) //?????????????
        return (0);

    // float oMap[640][480];//每个格的花费
    // float tMap[640][480];//出发点达到每个格的花费时间
    // float opMap[640][480][3];//存储每个点的 父点的影射出发点 和向量方向 [父点y] [父点x] [方向]
    /*
    float temp2=
        ((oMap[y+1][x-1])
        +(oMap[y+0][x-1])
        +(oMap[y-1][x-1])
        +(oMap[y+1][x])
    //	+(oMap[y+0][x])
        +(oMap[y-1][x])
        +(oMap[y+1][x+1])
        +(oMap[y+0][x+1])
        +(oMap[y-1][x+1]))*0.125;//0.11111111111111111111111111111111;
    */
    float Dadd = in_R - sqrt((fx - xinS) * (fx - xinS) + (fy - yinS) * (fy - yinS)); //+0.01
    if (Dadd < 0)
        //	return(0);
        Dadd = sqrt((fx - x) * (fx - x) + (fy - y) * (fy - y)); //;//-Dadd;//
    // temp =(mmap_tc_data[fy][fx]+Dadd*oMap[y][x]*aMap[y][x]) ;
    temp = (mmap_tc_data[fy][fx] + Dadd * oMap[y][x]);
    // temp =(mmap_tc_data[fy][fx]+Dadd*temp2) ;
    if (temp < mmap_tc_data[y][x])
    {
        mmap_tc_data[y][x] = temp;

        int Bx = x;
        int By = y;
        float dx = (oMap[By + 1][Bx - 1]) * (-0.707) + (oMap[By + 0][Bx - 1]) * (-1) +
                   (oMap[By - 1][Bx - 1]) * (-0.707) + (oMap[By + 1][Bx + 1]) * 0.707 + (oMap[By + 0][Bx + 1]) * 1 +
                   (oMap[By - 1][Bx + 1]) * 0.707; //-oMap[Bx][By]

        float dy = (oMap[By + 1][Bx - 1]) * 0.707 + (oMap[By + 1][Bx + 0]) * 1 + (oMap[By + 1][Bx + 1]) * 0.707 +
                   (oMap[By - 1][Bx - 1]) * (-0.707) + (oMap[By - 1][Bx + 0]) * (-1) +
                   (oMap[By - 1][Bx + 1]) * (-0.707); //-oMap[Bx][By]
        // float grads_A=atan2(-dx,-dy);//test_angle;     //注：  梯度与x轴的弧度
        float grads_R = sqrt(dx * dx + dy * dy); // 注：  梯度大小
        // float sin_out_aa ;
        float out_A;
        float out_sx, out_sy;
        out_sy = (-dy * 0.11 + yin / in_R); //*in_R +y;
        out_sx = (-dx * 0.11 + xin / in_R); //*in_R +x;
        out_A = atan2(out_sx, out_sy);
        float out_R = in_R / (grads_R + 1); // 能用的1简化方法 未经过介质断面检测法验证
        //	  float   out_R =in_R*sqrt(out_sx*out_sx+out_sy*out_sy)/(grads_R+1);//能用的1方法
        // float   out_R =in_R*(out_sx*out_sx+out_sy*out_sy)/((grads_R+1)*(grads_R+1));//sqrt
        // float   out_R =in_R*pow((out_sx*out_sx+out_sy*out_sy),0.9)/(grads_R+1);
        // float   out_R =in_R-(in_R*(1-(out_sx*out_sx+out_sy*out_sy)/((grads_R+1)*(grads_R+1))));
        // aMap[y][x]+=(grads_R+1)/sqrt(out_sx*out_sx+out_sy*out_sy);//fabs(1.002-sqrt(out_sx*out_sx+out_sy*out_sy)/(grads_R*0.11+1));
        //	if(aMap[y][x]<(grads_R+1)/sqrt(out_sx*out_sx+out_sy*out_sy))//(fabs(1.002-sqrt(out_sx*out_sx+out_sy*out_sy)/(grads_R*0.11+1))+1))
        //		aMap[y][x]+=0.2;//((grads_R+1)*(grads_R+1))/(out_sx*out_sx+out_sy*out_sy);
        //	else
        //		aMap[y][x]-=0.1;//((grads_R+1)*(grads_R+1))/(out_sx*out_sx+out_sy*out_sy);
        //	if (aMap[y][x]<1)
        //		aMap[y][x]=1;
        out_sy = cos(out_A) * out_R + y;
        out_sx = sin(out_A) * out_R + x;

        opMap[y][x][0] = out_sy;
        opMap[y][x][1] = out_sx;

#ifdef USE_OpenCv // 测试输出 每个点的父向量
//	Draw_line		(x,y,out_sx             ,out_sy            ,255,0,255, img);
#endif

        if (LayerMin > temp)
            LayerMin = temp;
        return (temp);
    }
    else
    {
        return (0);
    }
}

// 生成圆环掩模用函数
void CPathPlan::circle_maskG(float R, int *Gcmask_x, int *Gcmask_y, int *Gcmask_lo)
{ // int R=3;//10;//5;
    int Gcmask_l = 0;
    // 半径2 和2以上的 圆都正确   注意使用[方法2：尾点]结尾得到的半径2的圆正好和 四连通的半径1的圆配套吻合
    // 注意使用[方法1：尾点]结尾得到的半径2的圆正好是八连通的半径1的圆 半径2的圆正好是16连通的方块
    float r;
    int i;
    r = R / 1.414;
    Gcmask_x[Gcmask_l] = r;
    Gcmask_y[Gcmask_l++] = sqrt(R * R - r * r);
    for (r = R / 1.414 - 1; r > 1; r--)
    {
        Gcmask_x[Gcmask_l] = r;
        Gcmask_y[Gcmask_l++] = sqrt(R * R - r * r);

        Gcmask_y[Gcmask_l] = r;
        Gcmask_x[Gcmask_l++] = sqrt(R * R - r * r);
    }
    // 方法1：尾点
    //	Gcmask_x[Gcmask_l]=r;
    //	Gcmask_y[Gcmask_l++]=sqrt(R*R-r*r);
    // 方法2：尾点
    Gcmask_x[Gcmask_l] = R;
    Gcmask_y[Gcmask_l++] = 0;

    int Gcmask_l_T = Gcmask_l;
    for (i = 0; i < Gcmask_l_T; i++)
    {
        Gcmask_x[Gcmask_l] = -Gcmask_y[i];
        Gcmask_y[Gcmask_l++] = Gcmask_x[i];

        Gcmask_x[Gcmask_l] = -Gcmask_x[i];
        Gcmask_y[Gcmask_l++] = -Gcmask_y[i];

        Gcmask_x[Gcmask_l] = Gcmask_y[i];
        Gcmask_y[Gcmask_l++] = -Gcmask_x[i];
    }
    *Gcmask_lo = Gcmask_l;
}

// 生成实心圆掩模用函数
void CPathPlan::circle_maskGA(float R, int *Gcmask_x, int *Gcmask_y, float *Gcmask_w, int *Gcmask_lo)
{ // int R=3;//10;//5;
    int Gcmask_l = 0;
    int x, y;
    for (x = 0; x < 100; x++)
        for (y = 0; y < 100; y++)
            temp[y][x] = 0;
    // 半径2 和2以上的 圆都正确   注意使用[方法2：尾点]结尾得到的半径2的圆正好和 四连通的半径1的圆配套吻合
    // 注意使用[方法1：尾点]结尾得到的半径2的圆正好是八连通的半径1的圆 半径2的圆正好是16连通的方块
    float r;
    int i;
    Gcmask_x[Gcmask_l] = 0;
    Gcmask_y[Gcmask_l] = 0;
    Gcmask_w[Gcmask_l++] = 0;

    r = R / 1.414;
    //	Gcmask_x[Gcmask_l]=r;
    //	Gcmask_y[Gcmask_l++]=r;//sqrt(R*R-r*r);
    //	for (x=1;x<=r;x++)
    //	{
    temp[(int)r][(int)r] = 1;
    //	}
    for (r = R / 1.414 - 1; r > 1; r--)
    {
        //		 Gcmask_y[Gcmask_l]=r;
        //		 Gcmask_x[Gcmask_l++]=sqrt(R*R-r*r);
        for (x = 1; x <= sqrt(R * R - r * r); x++)
        {
            temp[(int)r][x] = 1;
            temp[x][(int)r] = 1;
        }
        //		 Gcmask_x[Gcmask_l]=r;
        //		 Gcmask_y[Gcmask_l++]=sqrt(R*R-r*r);
    }
    // 方法1：尾点
    //	Gcmask_x[Gcmask_l]=r;
    //	Gcmask_y[Gcmask_l++]=sqrt(R*R-r*r);
    // 方法2：尾点
    //	Gcmask_x[Gcmask_l]=R;
    //	Gcmask_y[Gcmask_l++]=0;
    ///*
    for (x = 1; x <= R; x++)
        temp[0][x] = 1;

    for (x = 0; x < 100; x++)
        for (y = 0; y < 100; y++)
            if (temp[y][x] == 1)
            {
                Gcmask_x[Gcmask_l] = x;
                Gcmask_y[Gcmask_l] = y;
                Gcmask_w[Gcmask_l++] = sqrt(y * y + x * x);
            }
    //*/
    int Gcmask_l_T = Gcmask_l;
    for (i = 0; i < Gcmask_l_T; i++)
    {
        Gcmask_x[Gcmask_l] = -Gcmask_y[i];
        Gcmask_y[Gcmask_l] = Gcmask_x[i];
        Gcmask_w[Gcmask_l++] = Gcmask_w[i];

        Gcmask_x[Gcmask_l] = -Gcmask_x[i];
        Gcmask_y[Gcmask_l] = -Gcmask_y[i];
        Gcmask_w[Gcmask_l++] = Gcmask_w[i];

        Gcmask_x[Gcmask_l] = Gcmask_y[i];
        Gcmask_y[Gcmask_l] = -Gcmask_x[i];
        Gcmask_w[Gcmask_l++] = Gcmask_w[i];
    }
    *Gcmask_lo = Gcmask_l;
}

// 生成实心圆掩模用函数
void CPathPlan::circle_maskGA_(float R, int *Gcmask_x, int *Gcmask_y, float *Gcmask_w, int *Gcmask_lo)
{ // int R=3;//10;//5;
    int Gcmask_l = 0;
    int x, y;
    float temp_r;
    for (x = -R; x <= R; x++)
        for (y = -R; y <= R; y++)
        {
            temp_r = sqrt(y * y + x * x);
            if (temp_r <= R)
            {
                Gcmask_x[Gcmask_l] = x;
                Gcmask_y[Gcmask_l] = y;
                Gcmask_w[Gcmask_l++] = temp_r;
            }
        }

    *Gcmask_lo = Gcmask_l;
}

// 生成实心圆掩模用函数
// 生成有方向 圆掩模用函数 x坐标数组 y 坐标数组 当前点与基准点偏差角度(0~180 对应 0~1) 数组长度
void CPathPlan::circle_maskGAA(float R, float angle, int *Gcmask_x, int *Gcmask_y, float *Gcmask_w, int *Gcmask_lo)
{                                    // int R=3;//10;//5;
    angle = angle * 3.14159 / 180.0; // 转成弧度制
    int Gcmask_l = 0;
    int x, y;
    float ta;
    const float pi = 3.14159;
    for (x = 0; x < 100; x++)
        for (y = 0; y < 100; y++)
            temp[y][x] = 0;
    // 半径2 和2以上的 圆都正确   注意使用[方法2：尾点]结尾得到的半径2的圆正好和 四连通的半径1的圆配套吻合
    // 注意使用[方法1：尾点]结尾得到的半径2的圆正好是八连通的半径1的圆 半径2的圆正好是16连通的方块
    float r;
    int i;
    Gcmask_x[Gcmask_l] = 0;
    Gcmask_y[Gcmask_l] = 0;
    Gcmask_w[Gcmask_l++] = 0;

    r = R / 1.414;
    //	Gcmask_x[Gcmask_l]=r;
    //	Gcmask_y[Gcmask_l++]=r;//sqrt(R*R-r*r);
    //	for (x=1;x<=r;x++)
    //	{
    temp[(int)r][(int)r] = 1;
    //	}
    for (r = R / 1.414 - 1; r > 1; r--)
    {
        //		 Gcmask_y[Gcmask_l]=r;
        //		 Gcmask_x[Gcmask_l++]=sqrt(R*R-r*r);
        for (x = 1; x <= sqrt(R * R - r * r); x++)
        {
            temp[(int)r][x] = 1;
            temp[x][(int)r] = 1;
        }
        //		 Gcmask_x[Gcmask_l]=r;
        //		 Gcmask_y[Gcmask_l++]=sqrt(R*R-r*r);
    }
    // 方法1：尾点
    //	Gcmask_x[Gcmask_l]=r;
    //	Gcmask_y[Gcmask_l++]=sqrt(R*R-r*r);
    // 方法2：尾点
    //	Gcmask_x[Gcmask_l]=R;
    //	Gcmask_y[Gcmask_l++]=0;
    ///*
    for (x = 1; x <= R; x++)
        temp[0][x] = 1;

    for (x = 0; x < 100; x++)
        for (y = 0; y < 100; y++)
            if (temp[y][x] == 1)
            {
                Gcmask_x[Gcmask_l] = x;
                Gcmask_y[Gcmask_l] = y;
                ta = angle - atan2(Gcmask_y[Gcmask_l], Gcmask_x[Gcmask_l]);
                if (ta > pi)
                    ta -= 2 * pi;
                if (ta < -pi)
                    ta += 2 * pi;
                Gcmask_w[Gcmask_l] =
                    fabs(ta * ta / pi / pi * (1 - sqrt(x * x + y * y) / R)); // fabs(ta/pi*(1-sqrt(x*x+y*y)/R));//
                Gcmask_l++;
            }
    //*/
    /**/
    int Gcmask_l_T = Gcmask_l;
    for (i = 0; i < Gcmask_l_T; i++)
    {
        Gcmask_x[Gcmask_l] = -Gcmask_y[i];
        Gcmask_y[Gcmask_l] = Gcmask_x[i];
        ta = angle - atan2(Gcmask_y[Gcmask_l], Gcmask_x[Gcmask_l]);
        if (ta > pi)
            ta -= 2 * pi;
        if (ta < -pi)
            ta += 2 * pi;
        Gcmask_w[Gcmask_l] =
            fabs(ta * ta / pi / pi *
                 (1 - sqrt(Gcmask_x[Gcmask_l] * Gcmask_x[Gcmask_l] + Gcmask_y[Gcmask_l] * Gcmask_y[Gcmask_l]) / R));
        Gcmask_l++;

        Gcmask_x[Gcmask_l] = -Gcmask_x[i];
        Gcmask_y[Gcmask_l] = -Gcmask_y[i];
        ta = angle - atan2(Gcmask_y[Gcmask_l], Gcmask_x[Gcmask_l]);
        if (ta > pi)
            ta -= 2 * pi;
        if (ta < -pi)
            ta += 2 * pi;
        Gcmask_w[Gcmask_l] =
            fabs(ta * ta / pi / pi *
                 (1 - sqrt(Gcmask_x[Gcmask_l] * Gcmask_x[Gcmask_l] + Gcmask_y[Gcmask_l] * Gcmask_y[Gcmask_l]) / R));
        Gcmask_l++;

        Gcmask_x[Gcmask_l] = Gcmask_y[i];
        Gcmask_y[Gcmask_l] = -Gcmask_x[i];
        ta = angle - atan2(Gcmask_y[Gcmask_l], Gcmask_x[Gcmask_l]);
        if (ta > pi)
            ta -= 2 * pi;
        if (ta < -pi)
            ta += 2 * pi;
        Gcmask_w[Gcmask_l] =
            fabs(ta * ta / pi / pi *
                 (1 - sqrt(Gcmask_x[Gcmask_l] * Gcmask_x[Gcmask_l] + Gcmask_y[Gcmask_l] * Gcmask_y[Gcmask_l]) / R));
        Gcmask_l++;
    }
    *Gcmask_lo = Gcmask_l;
}

// 生成此类需要使用所有圆环掩模
void CPathPlan::circle_mask(void)
{
    cmask_l = 0;
    cmask_x[cmask_l] = +1;
    cmask_y[cmask_l++] = 0;
    cmask_x[cmask_l] = -1;
    cmask_y[cmask_l++] = 0;
    cmask_x[cmask_l] = 0;
    cmask_y[cmask_l++] = +1;
    cmask_x[cmask_l] = 0;
    cmask_y[cmask_l++] = -1;
    cmask_lf1 = 100;                                                // X1倍的分辨率 亚像素
    circle_maskGf(5 /*3.5 15 2*/, cmask_xf1, cmask_yf1, cmask_lf1); // X1倍的分辨率 亚像素
    circle_maskG(18 /*15 2*/, cmask_x1, cmask_y1, &cmask_l1);       // X1倍的分辨率
    circle_maskG(6 /*4*/, cmask_x2, cmask_y2, &cmask_l2);           // X1/2/2倍的分辨率
    circle_maskG(9, cmask_x4, cmask_y4, &cmask_l4);                 // X1/4/4倍的分辨率
    for (int i = 2; i < 80; i++)
    {
        //	circle_maskGA(i/*/1.12*/,&omask_x0[i][0],&omask_y0[i][0],&omask_w0[i][0],&omask_l0[i]);//2~80 直径障碍物掩模
        circle_maskGA_(i /*/1.12*/, &omask_x0[i][0], &omask_y0[i][0], &omask_w0[i][0],
                       &omask_l0[i]); // 2~80 直径障碍物掩模 2013改
    }
    omask_l0[0] = 1; // 0 直径障碍物掩模
    omask_x0[0][0] = 0;
    omask_y0[0][0] = 0;
    omask_l0[1] = 1; // 1 直径障碍物掩模
    omask_x0[1][0] = 0;
    omask_y0[1][0] = 0;
}

void CPathPlan::Ppgrow(void) // 从生长起始点 开始生长
{
    path_plan_angle = 0; // 在场地直角坐标系下机器路径规划要行进的方向
#ifdef FUNCTION_DSPLAY   // FUNCTION_TEST//使用Open_Cv 并开启测试模式
                         //********************
    //	CString strShow;
    char strShow[100]; // 显示调试信息用缓冲区
    LARGE_INTEGER Frequency, CountEnd, CountStart;
    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&CountStart);
    double dfElapseMS0 = 0;
    double dfElapseMS1 = 0;
    double dfElapseMS2 = 0;
    double dfElapseMS3 = 0;
    double pingjun;
    //********************测试时间的起始点
#endif

    // if(1)
    //{

    int sy = 20; // 240;//254;
    int sx = 20; // 244;
    register int x, y, j, i, k, l;
    register float temp;
    // register char  temp_num=0;
    // register long  lay_num=0;

    register int x_add, x_reduce, y_add, y_reduce, xg, yg, ttx, tty, zoomR_t, zoomR_t1, zoomR_t2;
    register unsigned long int area_grow_data_arel = 0; // 纪录currently当前这一圈有几个像素
    register int connect_area_num =
        1; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点 2连通域的第一个编号 3 4 5....
    register int connect_area_layer_l = 0;  // 存储连通域 层的 点数 长度
    register int connect_area_layer_s = 0;  // 存储连通域 层的 首
    register int connect_area_layer_e = 0;  // 存储连通域 层的 尾
    register int connect_area_layer_pl = 0; // 存储连通域 上层的 点数 长度
    register int connect_area_layer_ps = 0; // 存储连通域 上层的 首
    register int connect_area_layer_pe = 0; // 存储连通域 上层的 尾

    register int rcmask_l = cmask_l;
    register int rcmask_x[100];
    register int rcmask_y[100];
    for (i = 0; i < rcmask_l; i++)
    {
        rcmask_x[i] = cmask_x[i];
        rcmask_y[i] = cmask_y[i];
    }

#ifdef FUNCTION_TEST // 使用Open_Cv 并开启测试模式
    while (1)
    {
        //********************
        QueryPerformanceCounter(&CountEnd);
        dfElapseMS0 =
            (double)((double)(CountEnd.QuadPart - CountStart.QuadPart + 10) / (double)Frequency.QuadPart) * 1000.0;
        //********************第一个时间节点
        datainT();
#endif

#ifdef FUNCTION_DSPLAY // FUNCTION_TEST//使用Open_Cv 并开启测试模式
//********************
// QueryPerformanceCounter(&CountEnd);
// dfElapseMS1=(double)((double)(CountEnd.QuadPart-CountStart.QuadPart+10)/(double)Frequency.QuadPart)*1000.0;
//********************第一个时间节点
#endif

#ifdef FUNCTION_USE_MOUSE
        s_x = MouseEvent_s_x; // 开始坐标
        s_y = MouseEvent_s_y;
        t_x = MouseEvent_t_x; // 目标坐标
        t_y = MouseEvent_t_y;
#endif

#ifndef FUNCTION_TEST // 使用Open_Cv 并开启测试模式
// in_obj_l=0;
// datain();
#endif
        if (s_x >= MAP_X - 4 - 1)
            s_x = MAP_X - 4 - 1; // 注意这里 不能 解决目标点和起始点都出界的问题
        if (s_x < 0)
            s_x = 0; // 注意 这里同时也可以输出路径规划失败信息 同时推出函数
        if (s_y >= MAP_Y - 4 - 1)
            s_y = MAP_Y - 4 - 1;
        if (s_y < 0)
            s_y = 0;

        if (t_x >= MAP_X - 4)
            t_x = MAP_X - 4;
        if (t_x < 0)
            t_x = 0;
        if (t_y >= MAP_Y - 4)
            t_y = MAP_Y - 4;
        if (t_y < 0)
            t_y = 0;

        int Pangle = angle_add_transfer2(path_plan_angle_P, 0) * anglemask_num / 360; // 显示方向花费掩膜
        if (Pangle >= anglemask_num)
            Pangle -= anglemask_num;
        if (Pangle < 0)
            Pangle += anglemask_num;
        int tx, ty; // temp_x temp_y
        for (int nm = 0; nm < anglemask_l[Pangle]; nm++)
        {
            tx = anglemask_x[Pangle][nm] + s_x;
            ty = anglemask_y[Pangle][nm] + s_y;
            if ((tx >= 0) && (tx < MAP_X) && (ty >= 0) && (ty < MAP_Y))
            {
                oMap[ty][tx] += path_plan_start_angle_cost * anglemask_w[Pangle][nm];
                if (oMap[ty][tx] < 0.0001)
                    oMap[ty][tx] = 0.0001;
            }
        }
        path_plan_start_angle_cost = 2.4; // 1.5 1.8 2.4  //在起始方向花费当前周期已经使用完后将 其赋定值
                                          //********************
        //	QueryPerformanceCounter(&CountEnd);
        //	dfElapseMS0=(double)((double)(CountEnd.QuadPart-CountStart.QuadPart+10)/(double)Frequency.QuadPart)*1000.0;
        //********************第一个时间节点
        char end_flog = 0;
        e_Nod = new Over_Grow_Nod;
        s_Nod = e_Nod;
        s_Nod->next = e_Nod;
        // e_Nod->previous = NULL;
        C_nod(0, 0, 100000000);
        // e_Nod->previous = s_Nod;

        area_grow_data_arel = 0; // 纪录currently当前这一圈有几个像素
        connect_area_num =
            1; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点 2连通域的第一个编号 3 4 5....
        connect_area_layer_l = 0;  // 存储连通域 层的 点数 长度
        connect_area_layer_s = 0;  // 存储连通域 层的 首
        connect_area_layer_e = 0;  // 存储连通域 层的 尾
        connect_area_layer_pl = 0; // 存储连通域 上层的 点数 长度
        connect_area_layer_ps = 0; // 存储连通域 上层的 首
        connect_area_layer_pe = 0; // 存储连通域 上层的 尾

        GLayerMax = 2;      // 当前层 可达到最大花费          LayerMin+
        LayerMin = 1000000; //
        float mmap_tc_data_t;
        // over_grow_data_s=0;//超出生长花费队列 开始
        // over_grow_data_e=0;

        // 设置启始点
        sy = s_y; //
        sx = s_x; //

        if (mmap_tc_data[sy][sx] == 1000000) // 没生长过 且是有效点
        {                                    // seedx=x;
            // seedy=y;
            // 2开始生长
            // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
            // 1设置这次生长的起点种子
            marea_grow_data_arex[area_grow_data_arel] = sx; // 当前次生长的一圈的纪录
            marea_grow_data_arey[area_grow_data_arel] = sy; // 当前次生长的一圈的纪录
            //			marea_grow_data_Parex[area_grow_data_arel]=marea_grow_data_arex[area_grow_data_arel];//[640*480];//currently当前生长的一圈的纪录
            //			marea_grow_data_Parey[area_grow_data_arel]=marea_grow_data_arey[area_grow_data_arel];//[640*480];//currently当前生长的一圈的纪录
            area_grow_data_arel++;
            mmap_tc_data[sy][sx] = 0;
            connect_area_layer_l = 1;
            connect_area_layer_pe = connect_area_layer_ps + 1;

            opMap[sy][sx][1] = sx; //+5
            opMap[sy][sx][0] = sy; //+3

            // 开始生长
            while (1) // for (i=0;i<1500;i++)//生成生长区域 的生长次数
            {
                connect_area_layer_s = area_grow_data_arel;
                j = connect_area_layer_ps;
                while (j < connect_area_layer_pe)
                {                                 // 周围不是A中的有效点且符合生长规则
                    xg = marea_grow_data_arex[j]; // 当前次生长的一圈的纪录
                    yg = marea_grow_data_arey[j]; // 当前次生长的一圈的纪录

                    i = 0;
                    while (i < cmask_l)
                    {
                        int rcmask_x_t = rcmask_x[i];
                        int rcmask_y_t = rcmask_y[i];
                        //			if
                        //(((yg-sy)>=-40)&&((yg-sy)<40)&&((xg-sx)>=-40)&&((xg-sx)<40))//((abs(yg-sy)<40)&&(abs(xg-sx)<40))//((abs(yg-sy)<40)&&(abs(xg-sx)<40))//
                        //			if
                        //(((yg-sy)>=-36)&&((yg-sy)<36)&&((xg-sx)>=-36)&&((xg-sx)<36))//((abs(yg-sy)<40)&&(abs(xg-sx)<40))//((abs(yg-sy)<40)&&(abs(xg-sx)<40))//
                        if (((yg - sy) >= -36) && ((yg - sy) < 34) && ((xg - sx) >= -36) &&
                            ((xg - sx) < 34)) //((abs(yg-sy)<40)&&(abs(xg-sx)<40))//((abs(yg-sy)<40)&&(abs(xg-sx)<40))//
                        {
                            zoomR_t1 = zoomR[yg - sy + 40][xg - sx + 40];
                            zoomR_t2 =
                                zoomR[rcmask_y_t * zoomR_t1 + yg - sy + 40][rcmask_x_t * zoomR_t1 + xg - sx + 40];
                            if (zoomR_t1 > zoomR_t2)
                                zoomR_t = zoomR_t1;
                            else
                                zoomR_t = zoomR_t2;

                            ttx = xg - sx;
                            ttx = ttx / zoomR_t;
                            ttx = ttx * zoomR_t;
                            tty = yg - sy;
                            tty = tty / zoomR_t;
                            tty = tty * zoomR_t;

                            x_add = rcmask_x_t * zoomR_t + ttx +
                                    sx; /// zoomR[yg-sy+40][xg-sx+40]*zoomR[yg-sy+40][xg-sx+40];
                            y_add = rcmask_y_t * zoomR_t + tty +
                                    sy; /// zoomR[yg-sy+40][xg-sx+40]*zoomR[yg-sy+40][xg-sx+40];
                        }
                        else
                        {
                            zoomR_t = zoomR_max; // 0;
                            //	 ttx=xg /zoomR_max;
                            //	 ttx=ttx*zoomR_max;
                            //	 tty=yg /zoomR_max;
                            //	 tty=tty*zoomR_max;
                            ttx = xg - sx;
                            ttx = ttx / zoomR_max;
                            ttx = ttx * zoomR_max;
                            tty = yg - sy;
                            tty = tty / zoomR_max;
                            tty = tty * zoomR_max;
                            //	ttx=xg;tty=yg;
                            x_add = rcmask_x[i] * zoomR_max + ttx + sx;
                            y_add = rcmask_y[i] * zoomR_max + tty + sy;
                        }

                        //						if ((x_add>=0)&&(x_add<640)&&(y_add>=0)&&(y_add<480))
                        //						if ((x_add>=2)&&(x_add<320)&&(y_add>=2)&&(y_add<240))
                        //						if ((x_add>=0)&&(x_add<160)&&(y_add>=0)&&(y_add<120))
                        if ((x_add >= 0) && (x_add < MAP_X - 4) && (y_add >= 0) && (y_add < MAP_Y - 4))
                        {
                            mmap_tc_data_t = refraction02(xg, yg, x_add, y_add);
                            if (mmap_tc_data_t != 0)
                            {
                                /*										if (zoomR_t!=0)
                                                                        {	for(k=0;k<zoomR_t;k++)
                                                                            {	for(l=0;l<zoomR_t;l++)
                                                                                {
                                                                                    mmap_tc_data[y_add+l][x_add+k]=mmap_tc_data_t;
                                                                        }	}	}
                                                                        else
                                                                        {	for(k=0;k<zoomR_max;k++)
                                                                            {	for(l=0;l<zoomR_max;l++)
                                                                                {
                                                                                    mmap_tc_data[y_add+l][x_add+k]=mmap_tc_data_t;
                                                                        }	}	}
                                */
                                for (k = 0; k < zoomR_t; k++)
                                {
                                    for (l = 0; l < zoomR_t; l++)
                                    {
                                        mmap_tc_data[y_add + l][x_add + k] = mmap_tc_data_t;
                                    }
                                }

                                if (mmap_tc_data_t > GLayerMax)
                                {
                                    C_nod(x_add, y_add, mmap_tc_data[y_add][x_add]);
                                }
                                else
                                {
                                    marea_grow_data_arey[area_grow_data_arel] = y_add;
                                    marea_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                                }
                            }
                        }

                        i++;
                    }
                    j++;
                }

                if (LayerMin < 100000000000)
                {
                    GLayerMax = LayerMin + 2.01; // 当前层 可达到最大花费   注意这里要在考虑考虑 因为此量是可变的
                    LayerMin = 100000000000;
                }
                else
                {
                    GLayerMax += 2.01; // 3;//当前层 可达到最大花费
                }
                /*				end_flog=0;
                                for (k=0;k<over_grow_data_e;k++)
                                {
                                    if (over_grow_data_x[k]!=100000)
                                    {	end_flog=1;
                                        if (mmap_tc_data[over_grow_data_y[k]][over_grow_data_x[k]]<GLayerMax)
                                        {
                                            marea_grow_data_arex[area_grow_data_arel]=over_grow_data_x[k];//[640*480];//currently当前生长的一圈的纪录
                                            marea_grow_data_arey[area_grow_data_arel]=over_grow_data_y[k];//[640*480];//currently当前生长的一圈的纪录
                                            over_grow_data_x[k]=100000;//标志此存储位现在为空
                                        //	over_grow_data_y[k]=100000;
                                            area_grow_data_arel++;//
                                        //	connect_area_layer_pe++;
                                        }
                                    }
                                }
                */

                float min_over_grow_v = 10000000000000;

                r_Nod = s_Nod;
                if (s_Nod->next == e_Nod)
                    end_flog = 0;
                else
                    end_flog = 1;
                while (1)
                {
                    if (r_Nod->mmap_tc_data < GLayerMax)
                    {
                        marea_grow_data_arex[area_grow_data_arel] =
                            r_Nod->x; //[640*480];//currently当前生长的一圈的纪录
                        marea_grow_data_arey[area_grow_data_arel++] =
                            r_Nod->y; //[640*480];//currently当前生长的一圈的纪录
                                      //						r_Nod=D_nod(r_Nod);
                        temp_r_Nod = r_Nod->next;
                        r_PNod->next = temp_r_Nod;
                        delete r_Nod;
                        r_Nod = temp_r_Nod;
                        if (r_Nod == e_Nod)
                            break;
                    }
                    else
                    {
                        r_PNod = r_Nod;
                        r_Nod = r_Nod->next;
                        if (r_Nod == e_Nod)
                            break;
                        if (min_over_grow_v > r_Nod->mmap_tc_data)
                            min_over_grow_v = r_Nod->mmap_tc_data;
                    }
                }
                //				area_grow_data_arel=0;
                connect_area_layer_e = area_grow_data_arel;
                connect_area_layer_l = connect_area_layer_e - connect_area_layer_s; // 连通域重量+当前一圈的点数
                connect_area_layer_ps = connect_area_layer_s;                       // 存储连通域 上层的 首
                connect_area_layer_pe = connect_area_layer_e;                       // 存储连通域 上层的 尾

                if (connect_area_layer_l == 0)
                {
                    if (min_over_grow_v < 10000000000000)
                        GLayerMax = min_over_grow_v;
                }
                if ((connect_area_layer_l == 0) && (end_flog == 0))
                    break;
                if (mmap_tc_data[int(t_y)][int(t_x)] < 1000000)
                    break;

#ifdef FUNCTION_POINT_GROW_TEST                  ////设置使用Open_Cv 并开启生长时的逐点测试模式
                for (x = 0; x < src->width; x++) // 清屏
                {
                    for (y = 0; y < src->height; y++)
                    {
                        OpenCv_DrawPoint(x, y, 255, 255, 255, src);
                    }
                }
                for (j = connect_area_layer_ps; j < connect_area_layer_pe; j += 1)
                {
                    xg = marea_grow_data_arex[j]; // marea_grow_data_arex[j];
                    yg = marea_grow_data_arey[j]; // marea_grow_data_arey[j];
                    OpenCv_DrawPoint(xg, yg, mmap_tc_data[yg][xg] * 10, 0, 0, src);
                    //	OpenCv_Draw_line(xg,yg,opMap[yg][xg][1],opMap[yg][xg][0],128,128,255, src);
                }

                if (cvWaitKey(100) == 'q')
                    break;
                cvShowImage("src_Pp", src);
#endif
            }
            // 生长完成
        }

        // 链表用内存回收
        /*
        r_Nod=s_Nod;
        while(1)
        {	temp_r_Nod   = r_Nod->next;
            delete  r_Nod;
            r_Nod=temp_r_Nod;
            if (r_Nod==e_Nod)
                break;

        }
        delete e_Nod;
        */

        // 链表用内存回收
        r_Nod = s_Nod;
        while (1)
        {
            temp_r_Nod = r_Nod->next;
            delete r_Nod;
            if (temp_r_Nod == e_Nod)
                break;

            r_Nod = temp_r_Nod->next;
            delete temp_r_Nod;
            if (r_Nod == e_Nod)
                break;
        }
        delete e_Nod;

        //********************
        //	QueryPerformanceCounter(&CountEnd);
        //	dfElapseMS1=(double)((double)(CountEnd.QuadPart-CountStart.QuadPart+10)/(double)Frequency.QuadPart)*1000.0;
        //	dfElapseMS1=dfElapseMS1-dfElapseMS0;
        //	dfElapseMS1=dfElapseMS1;
        //********************第一个时间节点
        //		 CString str;
        //		 str.Format("%f ",dfElapseMS1);
        //		 AfxMessageBox(str);

        /*
        for (x=sx-40;x<sx+40;x++)
        {	for (y=sy-40;y<sy+40;y++)
            {
                for (i=-2;i<2;i++)
                {	for (j=-2;j<2;j++)
                    {
                        if ((((y-sy+40)*4+i)>=0)&&(((y-sy+40)*4+i)<80*4)&&(((x-sx+40)*4+j)>=0)&&(((x-sx+40)*4+j)<80*4))
                            tMapzoom[(y-sy+40)*4+i][(x-sx+40)*4+j]=mmap_tc_data[y][x];//存储本机周围 局部 近似值 放大
        后的 时间花费 else tMapzoom[(y-sy+40)*4+i][(x-sx+40)*4+j]=1000000000;
        //			OpenCv_DrawPoint(x-sx+40+j,y-sy+40+i,0,0,mmap_tc_data[y][x],src);
                }	}
        //			OpenCv_DrawPoint(x,y+100,0,0,255,src);
        }	}
        */
        float tempx, tempy, min;
        float x_addf, y_addf, xgf, ygf;
        double temp_min;
        // int xg,yg;
        // int x_add,y_add;

        xg = t_x;  // tempx;
        yg = t_y;  // tempy;
        xgf = t_x; // tempx;
        ygf = t_y; // tempy;
        min = mmap_tc_data[yg][xg];
        float min_P;
        path_l = 0;          // 存储路径点用长度计数器清零
        path_x[path_l] = xg; // 存储路径点
        path_y[path_l++] = yg;

        // 路径规划停止规划半径 （小于此半径直接规划方向朝向目的地）
        float stop_path_plan_radius = 7.5;
        for (j = 0; j < 10000; j++)
        {
            min_P = min;

            if ((abs(yg - sy) < 40) && (abs(xg - sx) < 40))
            {
                zoomR_t = zoomR[yg - sy + 40][xg - sx + 40];
            }
            else
            {
                zoomR_t = zoomR_max;
            }
            if (zoomR_t == 1)
            {
                /*
                                   i=0;while(i<cmask_l1)
                                   {
                                        x_add=cmask_x1[i]+(xg-sx+40)*4;
                                        y_add=cmask_y1[i]+(yg-sy+40)*4;
                                        if ((x_add>=0)&&(x_add<80*4)&&(y_add>=0)&&(y_add<80*4))
                                        if (min>tMapzoom[y_add][x_add])
                                        {	min=tMapzoom[y_add][x_add];
                                            if ((tempx>=0)&&(tempx<MAP_X)&&(tempy>=0)&&(tempy<MAP_Y))
                                            {tempx=x_add/4+sx-40;
                                            tempy=y_add/4+sy-40;}
                                        }
                                        i++;
                                    }
                */
                /*
                                   i=0;while(i<cmask_l1)
                                   {
                                        x_addf=(float)(cmask_x1[i])/4.0+xgf; //  /2=><=15  /4=><=18
                                        y_addf=(float)(cmask_y1[i])/4.0+ygf;
                                        if ((x_addf>=0)&&(x_addf<MAP_X)&&(y_addf>=0)&&(y_addf<MAP_Y))
                                        if (min>mmap_tc_data[(int)y_addf][(int)x_addf])
                                        {	min=mmap_tc_data[(int)y_addf][(int)x_addf];
                                            tempx=x_addf;
                                            tempy=y_addf;
                                        }
                                        i++;
                                    }
                */
                i = 0;
                while (i < cmask_lf1)
                {
                    x_addf = (float)(cmask_xf1[i]) + float(xgf); //  /2=><=15  /4=><=18
                    y_addf = (float)(cmask_yf1[i]) + float(ygf);
                    if ((x_addf >= 0) && (x_addf < MAP_X) && (y_addf >= 0) && (y_addf < MAP_Y))
                    {
                        temp_min = Interpolate(x_addf, y_addf); // mmap_tc_data[(int)y_addf][(int)x_addf];//
                        if (min > temp_min)
                        {
                            min = temp_min;
                            tempx = x_addf;
                            tempy = y_addf;
                        }
                    }
                    i++;
                }
            }
            if (zoomR_t == 2)
            {
                i = 0;
                while (i < cmask_l2)
                {
                    x_add = cmask_x2[i] + xg;
                    y_add = cmask_y2[i] + yg;
                    if ((x_add >= 0) && (x_add < MAP_X) && (y_add >= 0) && (y_add < MAP_Y))
                        if (min > mmap_tc_data[y_add][x_add])
                        {
                            min = mmap_tc_data[y_add][x_add];
                            tempx = x_add;
                            tempy = y_add;
                        }
                    i++;
                }
            }
            if (zoomR_t == 4)
            {
                i = 0;
                while (i < cmask_l4)
                {
                    x_add = cmask_x4[i] + xg;
                    y_add = cmask_y4[i] + yg;
                    if ((x_add >= 0) && (x_add < MAP_X) && (y_add >= 0) && (y_add < MAP_Y))
                        if (min > mmap_tc_data[y_add][x_add])
                        {
                            min = mmap_tc_data[y_add][x_add];
                            tempx = x_add;
                            tempy = y_add;
                        }
                    i++;
                }
            }

            path_x[path_l] = tempx; // 存储路径点
            path_y[path_l++] = tempy;
#ifdef USE_OpenCv // 使用Open_Cv
//	OpenCv_Draw_line(tempx,tempy,xg,yg,255,128,255, src);
//	OpenCv_DrawPoint(xg,yg,128,255,255,src);
//	OpenCv_DrawPoint(tempx,tempy,128,255,255,src);
//	cvShowImage("src_Pp", src);
//	cvWaitKey(2000);
#endif

            xg = tempx;
            yg = tempy;
            xgf = tempx;
            ygf = tempy;
            if (min >= min_P)
                break;
            if (sqrt((tempx - sx) * (tempx - sx) + (tempy - sy) * (tempy - sy)) <= stop_path_plan_radius)
                break;
        }
        //				path_l--;
        //				path_x[path_l-1  ]=sx;//存储路径点
        //				path_y[path_l-1  ]=sy;
        /*
        if (path_l>2)
        {//	if(path_l==3)
            //	 path_l=path_l;

            path_l--;
            path_x[path_l-1  ]=s_x;//存储路径点
            path_y[path_l-1  ]=s_y;

        }
        else
        {
            path_l=2;
            path_x[1  ]=s_x;//存储路径点
            path_y[1  ]=s_y;
            path_x[0  ]=t_x;//存储路径点
            path_y[0  ]=t_y;
        }
        */
        if (sqrt((t_x - sx) * (t_x - sx) + (t_y - sy) * (t_y - sy)) <= stop_path_plan_radius)
        {
            path_l = 2;
            path_x[1] = s_x; // 存储路径点
            path_y[1] = s_y;
            path_x[0] = t_x; // 存储路径点
            path_y[0] = t_y;
        }
        else // if (path_l>=2)
        {
            path_x[path_l] = sx; // 存储路径点
            path_y[path_l] = sy;
            path_l++;
        }

        //				path_x[path_l  ]=sx;//存储路径点
        //				path_y[path_l++]=sy;

        // point grey mv 工业摄像机

#ifdef FUNCTION_DSPLAY // FUNCTION_TEST//使用Open_Cv 并开启测试模式
        //********************
        QueryPerformanceCounter(&CountEnd);
        dfElapseMS3 =
            (double)((double)(CountEnd.QuadPart - CountStart.QuadPart + 10) / (double)Frequency.QuadPart) * 1000.0;
//********************第一个时间节点
#endif

#ifdef FUNCTION_TEST // 使用Open_Cv 并开启测试模式
        //********************
        QueryPerformanceCounter(&CountEnd);
        dfElapseMS1 =
            (double)((double)(CountEnd.QuadPart - CountStart.QuadPart + 10) / (double)Frequency.QuadPart) * 1000.0;
        dfElapseMS1 = dfElapseMS1 - dfElapseMS0;
        dfElapseMS1 = dfElapseMS1;
        //********************第一个时间节点
#endif

#ifdef FUNCTION_DSPLAY
        char *data = src->imageData;
        for (x = 0; x < src->width; x++)
        {
            for (y = 0; y < src->height; y++)
            {
                data[(src->width * y + x) * 3 + 0] =
                    0; //(float)(unsigned char) (data2[(src_S->width*y+x)*3+0]);//(int)mmap_tc_data[y][x]*30;
                       ////场地每个格的行走花费 cost数据
                data[(src->width * y + x) * 3 + 1] = (int)mmap_tc_data[y][x] * 3; // 场地每个格的行走花费 cost数据
                data[(src->width * y + x) * 3 + 2] =
                    0; //(float)(unsigned char) (data2[(src_S->width*y+x)*3+2]);//(int)mmap_tc_data[y][x]*30;
                       ////场地每个格的行走花费 cost数据
            }
        }
        for (i = 1; i < path_l; i++)
        {
            //		OpenCv_Draw_line(path_x[i],path_y[i],path_x[i-1],path_y[i-1],255,128,255, src);
            OpenCv_DrawPoint(path_x[i], path_y[i], 255, 0, 0, src);
            //			cvWaitKey(100);
            //			cvShowImage( "src_Pp", src );
        }
#endif
#ifdef USE_OpenCv // 使用Open_Cv
        float circle_x, circle_y, circle_r;
        int need_test_C = 5;      // 4;//8;//最初希望以多少点来拟合路径
        int test_C = need_test_C; // 测试 多少点来拟合圆
        float min_err, min_err_t, circle_x_minerr, circle_y_minerr, circle_r_minerr;
        int min_err_pn;                    // 误差最小的拟合圆 用了几个点
        float min_err_c, mecX, mecY, mecR; // 最小二乘 得到圆 的方程   圆上点误差,圆心x,圆心y,圆半径
        float min_err_l, melX, melY,
            melA; // 最小二乘 得到 直线方程   直线上点误差,直线基点x,直线基点y,方向  [直线基点 为路径出发点]
        // float path_plan_angle;//路径规划输出方向
        if (path_l <= 2)
        { //	if(path_l==3)
            //		path_l=path_l;
            //	if(path_l< 3)
            //		path_l=path_l;
            path_plan_angle = atan2(
                path_x[path_l - 2] - path_x[path_l - 1],
                path_y[path_l - 2] -
                    path_y[path_l -
                           1]); // atan2(path_x[path_l-2]-path_x[path_l-1],path_y[path_l-2]-path_y[path_l-1]);//输出角度
                                // 在场地直角坐标系下机器路径规划要行进的方向
            OpenCv_Draw_line(path_x[path_l - 1], path_y[path_l - 1], sin(path_plan_angle) * 100 + path_x[path_l - 1],
                             cos(path_plan_angle) * 100 + path_y[path_l - 1], 0, 255, 0, src);

            //		path_plan_angle=atan2(path_x[path_l-2]-path_x[path_l-1],path_y[path_l-2]-path_y[path_l-1]);//*180/3.14
            // atan2(path_x[path_l-2]-path_x[path_l-1],path_y[path_l-2]-path_y[path_l-1]);//输出角度
            // 在场地直角坐标系下机器路径规划要行进的方向
            //		OpenCv_Draw_line(path_x[path_l-1],path_y[path_l-1]+10,sin(path_plan_angle)*100+path_x[path_l-1],cos(path_plan_angle)*100+path_y[path_l-1]+10,0,255,0,
            // src);
        }
        else
        {
            if (test_C > path_l)
                test_C = path_l;
            /*
            //??????????????????????
                    for (i=test_C-1;i>=6;i--)//最好最少拟合4个以上的点 因为3个点总是无误差拟合
            与其他多点有误差拟合无可比性 {
            min_err_t=LeastSquaresFitting(&path_x[path_l-i],&path_y[path_l-i],i,&circle_x,&circle_y,&circle_r);
                        if(min_err_t>0)
                        {	min_err=min_err_t;
                            circle_x_minerr=circle_x;
                            circle_y_minerr=circle_y;
                            circle_r_minerr=circle_r;
                            min_err_pn=i;
                            break;
                        }
                    }
                    for (i=i;i>=6;i--)//最好最少拟合4个以上的点 因为3个点总是无误差拟合 与其他多点有误差拟合无可比性
                    {
            min_err_t=LeastSquaresFitting(&path_x[path_l-i],&path_y[path_l-i],i,&circle_x,&circle_y,&circle_r);
                        if((min_err>min_err_t)&&(min_err_t>0))
                        {	min_err=min_err_t;
                            circle_x_minerr=circle_x;
                            circle_y_minerr=circle_y;
                            circle_r_minerr=circle_r;
                            min_err_pn=i;
                        }
                    }
                    OpenCv_Draw_line(path_x[path_l-1],path_y[path_l-1],circle_x_minerr,circle_y_minerr,255,128,0, src);
                    for (i=path_l-min_err_pn;i<path_l;i++)
                    {
                            OpenCv_Draw_line(path_x[i],path_y[i],path_x[i-1],path_y[i-1],255,128,255, src);
                    }
            */
            // 测试 最小二乘
            min_err_pn = test_C;
            i = test_C;
            min_err_c = LeastSquaresFitting(&path_x[path_l - i], &path_y[path_l - i], i, &mecX, &mecY, &mecR);
            min_err_l = LeastSquaresFitting4(&path_x[path_l - i], &path_y[path_l - i], i, &melX, &melY, &melA);

            // 因为经常点少的情况下圆拟合精度比较高 但小于4点就可能因为地图分辨率问题 路径产生 锯齿尽量需要用直线你和
            // 所以当点小于8个点逐渐 增加圆的误差 使结果偏向使用直线拟合
            float tpn =
                (float)(need_test_C - min_err_pn) /
                (float)need_test_C; // 最初希望以多少点来拟合路径 与实际使用多少点来拟合路径的差  差的越多越趋近1.0
            min_err_c += tpn * 1.5;
            if (path_l < 30)
                float tpn = (float)(30 - path_l) /
                            (float)30; // 最初希望以多少点来拟合路径 与实际使用多少点来拟合路径的差  差的越多越趋近1.0
            min_err_c += tpn * 1.5;

            // 测试
            char caseCL = 0;
            if ((min_err_c == 0) && (min_err_l == 0))
            {
                if (path_y[path_l - test_C] < path_y[path_l - 1])
                    path_plan_angle = 3.14;
                else
                    path_plan_angle = 0.0;
            }
            if ((min_err_c == 0) && (min_err_l > 0))
            {
                caseCL = 'L';
            }
            if ((min_err_c > 0) && (min_err_l == 0))
            {
                caseCL = 'C';
            }
            if ((min_err_c > 0) && (min_err_l > 0))
            {
                if (min_err_c < min_err_l)
                    caseCL = 'C';
                else
                    caseCL = 'L';
            }
            float direct_A = atan2(path_x[path_l - 1] - mecX, path_y[path_l - 1] - mecY);
            //	if (direct_A<(-3.1415926))		direct_A+=2*3.1415926;
            //	if (direct_A>  3.1415926)		direct_A-=2*3.1415926;
            float dxt = 0;
            float dyt = 0;
            float dat, datC, datL;
            // 求参加拟合的路径点的重心
            for (i = path_l - min_err_pn; i < path_l; i++)
            {
                dxt += path_x[i];
                dyt += path_y[i];
            }
            dxt = dxt / min_err_pn;
            dyt = dyt / min_err_pn;
            dat = atan2(mecX - dxt, mecY - dyt);
            //	if (dat<(-3.1415926))		dat+=2*3.1415926;
            //	if (dat>  3.1415926)		dat-=2*3.1415926;
            //	OpenCv_Draw_line(mecX,mecY,dxt,dyt,255,255,255, src);

            if (caseCL == 'C')
            {
                // 求 圆拟合的 出发方向
                datC = dat - direct_A;
                if (datC < (-3.1415926))
                    datC += 2 * 3.1415926;
                if (datC > 3.1415926)
                    datC -= 2 * 3.1415926;
                if (datC < 0)
                    direct_A += 3.14 / 2;
                else
                    direct_A -= 3.14 / 2;
                if (direct_A < (-3.1415926))
                    direct_A += 2 * 3.1415926;
                if (direct_A > 3.1415926)
                    direct_A -= 2 * 3.1415926;
#ifdef FUNCTION_DSPLAY
                OpenCv_Draw_line(path_x[path_l - 1], path_y[path_l - 1], path_x[path_l - 1] + 100 * sin(direct_A),
                                 path_y[path_l - 1] + 100 * cos(direct_A), 0, 0, 255, src);
//	OpenCv_circle   (mecX,mecY,mecR,255,255,255,src);
#endif
                path_plan_angle = direct_A; // 输出角度 在场地直角坐标系下机器路径规划要行进的方向
            }
            if (caseCL == 'L')
            {
                ////求 直线拟合的 出发方向
                dat = atan2(path_x[path_l - 1] - dxt, path_y[path_l - 1] - dyt);
                datL = dat - melA;
                if (datL < (-3.1415926))
                    datL += 2 * 3.1415926;
                if (datL > 3.1415926)
                    datL -= 2 * 3.1415926;
                if (datL < 0)
                    melA += 3.14 / 2;
                else
                    melA -= 3.14 / 2;
                if (direct_A < (-3.1415926))
                    direct_A += 2 * 3.1415926;
                if (direct_A > 3.1415926)
                    direct_A -= 2 * 3.1415926;
#ifdef FUNCTION_DSPLAY
                OpenCv_Draw_line(melX, melY, sin(melA) * 100 + melX, cos(melA) * 100 + melY, 255, 0, 255, src);
//	OpenCv_Draw_line(melX,melY,sin(melA)*100+melX,cos(melA)*100+melY,255,255,255, src);
#endif
                path_plan_angle = melA; // 输出角度 在场地直角坐标系下机器路径规划要行进的方向
            }
        }
#endif

#ifdef FUNCTION_DSPLAY
        /*
            int x_ad=160;
            for (x=0;x<160;x++)
            {	for (y=0;y<120;y++)
                {	data[(src->width*y+x)*6+0+x_ad*3] =data[(src->width*y+x)*3+0];//(int)mmap_tc_data[y][x]*30;
           //场地每个格的行走花费 cost数据 data[(src->width*y+x)*6+1+x_ad*3]
           =data[(src->width*y+x)*3+1];//场地每个格的行走花费 cost数据 data[(src->width*y+x)*6+2+x_ad*3]
           =data[(src->width*y+x)*3+2];//(int)mmap_tc_data[y][x]*30; //场地每个格的行走花费 cost数据
                    data[(src->width*y+x)*6+3+x_ad*3] =data[(src->width*y+x)*3+0];//(int)mmap_tc_data[y][x]*30;
           //场地每个格的行走花费 cost数据 data[(src->width*y+x)*6+4+x_ad*3]
           =data[(src->width*y+x)*3+1];//场地每个格的行走花费 cost数据 data[(src->width*y+x)*6+5+x_ad*3]
           =data[(src->width*y+x)*3+2];//(int)mmap_tc_data[y][x]*30; //场地每个格的行走花费 cost数据
                    data[(src->width*y+x)*6+0+(x_ad+src->width)*3]=data[(src->width*y+x)*3+0];//(int)mmap_tc_data[y][x]*30;
           //场地每个格的行走花费 cost数据
                    data[(src->width*y+x)*6+1+(x_ad+src->width)*3]=data[(src->width*y+x)*3+1];//场地每个格的行走花费
           cost数据
                    data[(src->width*y+x)*6+2+(x_ad+src->width)*3]=data[(src->width*y+x)*3+2];//(int)mmap_tc_data[y][x]*30;
           //场地每个格的行走花费 cost数据
                    data[(src->width*y+x)*6+3+(x_ad+src->width)*3]=data[(src->width*y+x)*3+0];//(int)mmap_tc_data[y][x]*30;
           //场地每个格的行走花费 cost数据
                    data[(src->width*y+x)*6+4+(x_ad+src->width)*3]=data[(src->width*y+x)*3+1];//场地每个格的行走花费
           cost数据
                    data[(src->width*y+x)*6+5+(x_ad+src->width)*3]=data[(src->width*y+x)*3+2];//(int)mmap_tc_data[y][x]*30;
           //场地每个格的行走花费 cost数据
                }
            }

        */
        /*//测试障碍物生成掩模
        i=2;while(cvWaitKey(1000) != '1')
        {
            if (i>=80)
                i=2;
            cvShowImage( "src_Pp", src );
            for (j=0;j<omask_l0[i];j++)
            {	OpenCv_DrawPoint(omask_x0[i][j]+100,omask_y0[i][j]+100,255,255,0,src);
            }
            i++;
        }
        */
        for (x = 0; x < MAP_X; x++) // 显示花费地图  越亮的地方 花费越大
        {
            for (y = 0; y < MAP_Y; y++)
            {
                OpenCv_DrawPoint(x, y + MAP_Y, 0, oMap[y][x] * 10, 0, src);
            }
        }
        Pangle = path_plan_angle_P * anglemask_num / 360; // 显示方向花费掩膜
        if (Pangle >= anglemask_num)
            Pangle -= anglemask_num;
        if (Pangle < 0)
            Pangle += anglemask_num;
        for (nm = 0; nm < anglemask_l[Pangle]; nm++)
            OpenCv_DrawPoint(anglemask_x[Pangle][nm] + 320, anglemask_y[Pangle][nm] + 400,
                             anglemask_w[Pangle][nm] * 255, 0, 0, src);
        Pangle = Close_target_direction * anglemask_num / 360; // 显示方向花费掩膜
        if (Pangle >= anglemask_num)
            Pangle -= anglemask_num;
        if (Pangle < 0)
            Pangle += anglemask_num;
        for (nm = 0; nm < anglemask_l[Pangle]; nm++)
            OpenCv_DrawPoint(anglemask_x[Pangle][nm] + 320, anglemask_y[Pangle][nm] + 450,
                             anglemask_w[Pangle][nm] * 255, 0, 0, src);

        if (cvWaitKey(10) == 'q')
            return;
        // 在图像中显示文本字符串

        // strShow.Format("0=:%9.4f ms,1=:%9.4f "
        //			   ,dfElapseMS0,dfElapseMS1);
        // cvPutText(src, "HELLO!" , cvPoint(0, 40), &out_txt, CV_RGB(255,255,255));
        // cvPutText(src, strShow , cvPoint(0, 40), &out_txt, CV_RGB(255,255,255));
        // strShow.Format("2=:%7.4f ms,3=:%7.4f ms,"
        //			   ,dfElapseMS2,dfElapseMS3);
        // cvPutText(src, strShow , cvPoint(0, 80), &out_txt, CV_RGB(255,255,255));
        // strShow.Format("%d"   ,path_l);
        // cvPutText(src, strShow , cvPoint(0, 120), &out_txt, CV_RGB(255,255,255));
        /**/
        sprintf(strShow, "0=:%7.4f ms,1=:%7.4f ms,", dfElapseMS0, dfElapseMS1);
        cvPutText(src, strShow, cvPoint(0, 480 - 40), &out_txt, CV_RGB(255, 255, 255));
        sprintf(strShow, "2=:%7.4f ms,3=:%7.4f ms,", dfElapseMS2, dfElapseMS3);
        cvPutText(src, strShow, cvPoint(0, 480 - 80), &out_txt, CV_RGB(255, 255, 255));
        sprintf(strShow, "%d", path_l);
        cvPutText(src, strShow, cvPoint(0, 480 - 120), &out_txt, CV_RGB(255, 255, 255));
        // cvWaitKey(1000);
        cvShowImage("src_Pp", src);
#endif

#ifdef FUNCTION_TEST // 使用Open_Cv 并开启测试模式

        cout << path_plan_angle / 3.14 * 180 << "		" << min_err_c << "		" << min_err_l << "		" << dfElapseMS1
             << endl;                                              // 显示时间dfElapseMS1
        path_plan_angle = -path_plan_angle * 180.0 / 3.14159 + 90; // 输出角度角度转弧度
        if (path_plan_angle > 180)
            path_plan_angle -= 360;
        if (path_plan_angle < -180)
            path_plan_angle += 360;
        path_plan_angle_P = path_plan_angle; // 存储历史记录
    }
#endif

    path_plan_angle = -path_plan_angle * 180.0 / 3.14159 + 90; // 输出角度角度转弧度
    if (path_plan_angle > 180)
        path_plan_angle -= 360;
    if (path_plan_angle < -180)
        path_plan_angle += 360;
    path_plan_angle_P = path_plan_angle; // 存储历史记录
    //}

    path_l--; // 注意 这里为兼容老程序 path_l取值范围 >=1 路径点[0]为目标点位置[path_l-1]为临近起点最近的路径点 需要的话
              // 可以读取[path_l]点其中存储起点
}

// void CPathPlan::grow(int sx,int sy)//从生长起始点 开始生长
//{
// }

// 为生长用变量 从外部获取存储空间 如果要求方便就用 new申请得到 如果要速度就申请全局的静态空间
// void CPathPlan::memory_space_init(unsigned int *marea_grow_data_arex_space,unsigned int *marea_grow_data_arey_space)
//{
//	marea_grow_data_arex=marea_grow_data_arex_space;//[640*480];//currently当前生长的一圈的纪录
//	marea_grow_data_arey=marea_grow_data_arey_space;//[640*480];//currently当前生长的一圈的纪录
// }

float CPathPlan::LeastSquaresFitting(float *m_points_x, float *m_points_y, int m_points_l, float *cx, float *cy,
                                     float *cr)
{
    int m_nNum = m_points_l;
    if (m_nNum < 3)
    {
        return (0);
    }

    int i = 0;

    double X1 = 0;
    double Y1 = 0;
    double X2 = 0;
    double Y2 = 0;
    double X3 = 0;
    double Y3 = 0;
    double X1Y1 = 0;
    double X1Y2 = 0;
    double X2Y1 = 0;

    for (i = 0; i < m_nNum; i++)
    {
        X1 = X1 + m_points_x[i];
        Y1 = Y1 + m_points_y[i];
        X2 = X2 + m_points_x[i] * m_points_x[i];
        Y2 = Y2 + m_points_y[i] * m_points_y[i];
        X3 = X3 + m_points_x[i] * m_points_x[i] * m_points_x[i];
        Y3 = Y3 + m_points_y[i] * m_points_y[i] * m_points_y[i];
        X1Y1 = X1Y1 + m_points_x[i] * m_points_y[i];
        X1Y2 = X1Y2 + m_points_x[i] * m_points_y[i] * m_points_y[i];
        X2Y1 = X2Y1 + m_points_x[i] * m_points_x[i] * m_points_y[i];
    }

    double C, D, E, G, H, N;
    double a, b, c;
    N = m_nNum;
    C = N * X2 - X1 * X1;
    D = N * X1Y1 - X1 * Y1;
    E = N * X3 + N * X1Y2 - (X2 + Y2) * X1;
    G = N * Y2 - Y1 * Y1;
    H = N * X2Y1 + N * Y3 - (X2 + Y2) * Y1;
    if ((C * G - D * D) == 0)
    {
        return (0);
    }
    a = (H * D - E * G) / (C * G - D * D);
    b = (H * C - E * D) / (D * D - G * C);
    c = -(a * X1 + b * Y1 + X2 + Y2) / N;

    double A, B, R;
    A = a / (-2);
    B = b / (-2);
    R = sqrt(a * a + b * b - 4 * c) / 2;

    *cx = (float)A; // m_fCenterX = A;
    *cy = (float)B; // m_fCenterY = B;
    *cr = (float)R; // m_fRadius = R;
    float t, err = 0;
    for (i = 0; i < m_nNum; i++)
    {
        t = sqrt((m_points_x[i] - A) * (m_points_x[i] - A) + (m_points_y[i] - B) * (m_points_y[i] - B)) - R;
        err += t * t;
    }
    err = err / (float)(m_nNum);
    if (err == 0)
        err = 0.000000000001;
    return (err);
}
float CPathPlan::LeastSquaresFitting2(float *m_points_x, float *m_points_y, int m_points_l, float *cx, float *cy,
                                      float *cr)
{
    int m_nNum = m_points_l;
    if (m_nNum < 3)
    {
        return (0);
    }
    int i = 0;
    double X1 = 0;
    double Y1 = 0;
    double X2 = 0;
    double Y2 = 0;
    double X3 = 0;
    double Y3 = 0;
    double X1Y1 = 0;
    double X1Y2 = 0;
    double X2Y1 = 0;

    for (i = 0; i < m_nNum; i++)
    {
        X1 = X1 + m_points_x[i];
        Y1 = Y1 + m_points_y[i];
        X2 = X2 + m_points_x[i] * m_points_x[i];
        Y2 = Y2 + m_points_y[i] * m_points_y[i];
        X3 = X3 + m_points_x[i] * m_points_x[i] * m_points_x[i];
        Y3 = Y3 + m_points_y[i] * m_points_y[i] * m_points_y[i];
        X1Y1 = X1Y1 + m_points_x[i] * m_points_y[i];
        X1Y2 = X1Y2 + m_points_x[i] * m_points_y[i] * m_points_y[i];
        X2Y1 = X2Y1 + m_points_x[i] * m_points_x[i] * m_points_y[i];
    }

    double C, D, E, G, H, N;
    double a, b, c;
    N = m_nNum;
    C = N * X2 - X1 * X1;
    D = N * X1Y1 - X1 * Y1;
    E = N * X3 + N * X1Y2 - (X2 + Y2) * X1;
    G = N * Y2 - Y1 * Y1;
    H = N * X2Y1 + N * Y3 - (X2 + Y2) * Y1;
    if ((C * G - D * D) == 0)
    {
        return (0);
    }
    a = (H * D - E * G) / (C * G - D * D);
    b = (H * C - E * D) / (D * D - G * C);
    c = -(a * X1 + b * Y1 + X2 + Y2) / N;

    double A, B, R;
    A = a / (-2);
    B = b / (-2);
    R = sqrt(a * a + b * b - 4 * c) / 2;

    *cx = (float)A; // m_fCenterX = A;
    *cy = (float)B; // m_fCenterY = B;
    *cr = (float)R; // m_fRadius = R;
    float err = 0;
    float min_mis_x, min_mis_y, min_mis_r, try_x, try_y, try_r, temp_min, temp;
    float delta_temp_r;  // 德耳塔 △ temp的变量
    float delta_temp_x;  // 德耳塔 △ temp的变量
    float delta_temp_y;  // 德耳塔 △ temp的变量
    float step_r = 0.01; // 为测试偏导数的试验步长变量 角度
    float step_x = 0.01; // 为测试偏导数的试验步长变量 x
    float step_y = 0.01; // 为测试偏导数的试验步长变量 y
    double speed_abs;    // 梯度的倒数 用来控制 下降步长   梯度越大越陡 步长越小
    float r_change_Gdown2_quotiety = 1;
    float x_change_Gdown2_quotiety = 1;
    float y_change_Gdown2_quotiety = 1;
    min_mis_r = R;
    min_mis_x = A;
    min_mis_y = B; // 入口
    temp = 0;
    try_r = min_mis_r;
    try_x = min_mis_x;
    try_y = min_mis_y;
    for (i = 0; i < m_nNum; i++)
    {
        temp += fabs((m_points_x[i] - try_x) * (m_points_x[i] - try_x) +
                     (m_points_y[i] - try_y) * (m_points_y[i] - try_y) - try_r * try_r);
    }
    temp_min = temp;

    for (int j = 0; j < 100; j++)
    {
        temp = 0;
        try_r = min_mis_r + step_r;
        try_x = min_mis_x;
        try_y = min_mis_y;
        for (i = 0; i < m_nNum; i++)
        {
            temp += fabs((m_points_x[i] - try_x) * (m_points_x[i] - try_x) +
                         (m_points_y[i] - try_y) * (m_points_y[i] - try_y) - try_r * try_r);
        }
        delta_temp_r = temp_min - temp;
        temp = 0;
        try_r = min_mis_r;
        try_x = min_mis_x + step_x;
        try_y = min_mis_y;
        for (i = 0; i < m_nNum; i++)
        {
            temp += fabs((m_points_x[i] - try_x) * (m_points_x[i] - try_x) +
                         (m_points_y[i] - try_y) * (m_points_y[i] - try_y) - try_r * try_r);
        }
        delta_temp_x = temp_min - temp;
        temp = 0;
        try_r = min_mis_r;
        try_x = min_mis_x;
        try_y = min_mis_y + step_y;
        for (i = 0; i < m_nNum; i++)
        {
            temp += fabs((m_points_x[i] - try_x) * (m_points_x[i] - try_x) +
                         (m_points_y[i] - try_y) * (m_points_y[i] - try_y) - try_r * try_r);
        }
        delta_temp_y = temp_min - temp;
        if ((delta_temp_r == 0) && (delta_temp_x == 0) &&
            (delta_temp_y ==
             0))             // 注意这个判定错误三项不为0时仍然和可为0 ((delta_temp_a + delta_temp_x + delta_temp_y)!=0)
            speed_abs = 0.1; // 如果有最极端的现象出现 各偏导数都为零 就用一个比较大的增长速率0.1
        else
            speed_abs =
                1 / sqrt(delta_temp_r * delta_temp_r + delta_temp_x * delta_temp_x + delta_temp_y * delta_temp_y);
        double speed = 1; // 0.6*(1-(float)(l)/(float)(MAX_Gdown2_Loop_time))+0.0001;//
        double tempR = delta_temp_r * speed_abs * r_change_Gdown2_quotiety * speed; // 6;//3;//*10;
        double tempX = delta_temp_x * speed_abs * x_change_Gdown2_quotiety * speed; // 2;
        double tempY = delta_temp_y * speed_abs * y_change_Gdown2_quotiety * speed; // 2;
        min_mis_r += tempR;
        min_mis_x += tempX;
        min_mis_y += tempY;
        temp = 0;
        try_r = min_mis_r;
        try_x = min_mis_x;
        try_y = min_mis_y;
        for (i = 0; i < m_nNum; i++)
        {
            temp += fabs((m_points_x[i] - try_x) * (m_points_x[i] - try_x) +
                         (m_points_y[i] - try_y) * (m_points_y[i] - try_y) - try_r * try_r);
        }
        temp_min = temp;
    }
    *cx = min_mis_x; // m_fCenterX = A;
    *cy = min_mis_y; // m_fCenterY = B;
    *cr = min_mis_r; // m_fRadius = R;

    err = err / (float)(m_nNum);
    if (err == 0)
        err = 0.000000000001;
    return (err);
}

// 最小方差评价直线拟合
float CPathPlan::LeastSquaresFitting3(float *m_points_x, float *m_points_y, int m_points_l, float *lx, float *ly,
                                      float *la)
{
    int m_nNum = m_points_l;
    if (m_nNum < 2)
    {
        return (0);
    }
    const double pi = 3.1415926535897932384626433832795;
    const double pi_2 = pi * 0.5;
    int i = 0;
    float err = 0;
    float min_mis_x, min_mis_y, min_mis_a, try_x, try_y, try_a, temp_min, temp, tempa, tempb, tan_mmA;
    float delta_temp_a;  // 德耳塔 △ temp的变量
    float delta_temp_x;  // 德耳塔 △ temp的变量
    float delta_temp_y;  // 德耳塔 △ temp的变量
    float step_a = 0.01; // 为测试偏导数的试验步长变量 角度
    float step_x = 0.01; // 为测试偏导数的试验步长变量 x
    float step_y = 0.01; // 为测试偏导数的试验步长变量 y
    double speed_abs;    // 梯度的倒数 用来控制 下降步长   梯度越大越陡 步长越小
    float a_change_Gdown2_quotiety = 0.01;
    float x_change_Gdown2_quotiety = 1;
    float y_change_Gdown2_quotiety = 1;
    int MAX_Gdown2_Loop_time = 100; // 最大下降叠代次数
    min_mis_a = atan2(m_points_x[0] - m_points_x[m_points_l - 1], m_points_y[0] - m_points_y[m_points_l - 1]);
    min_mis_x = m_points_x[m_points_l - 1];
    min_mis_y = m_points_y[m_points_l - 1]; // 入口

    temp = 0;
    try_a = min_mis_a;
    try_x = min_mis_x;
    try_y = min_mis_y;
    //	y=(x-min_mis_x)*tan(min_mis_a)+min_mis_y;
    //	x=(y-min_mis_y)/tan(min_mis_a)+min_mis_x;
    //	0=kx-y+b  Ax+By+C=0;  0=(x-x1)*tan(A)-y+y1         0 = x*tan(A) -y -x1*tan(A)+y1 ;
    //	d^2=(A*x0+B*y0+C)*(A*x0+B*y0+C)/(A^2+B^2);
    // 点斜式(斜率=k,x=a,y=b)    y=kx-ak+b  Ax+By+C=0  (A=k , B=-1 , C=-ak+b)
    // 点(x0,y0)到直线 Ax+By+C=0距离  = (A*x0+B*y0+C)*(A*x0+B*y0+C)/(A^2+B^2)  =(k*x0-y0-ak+b)*(k*x0-y0-ak+b)/(k*k+1)
    if (try_a > 3.14)
        try_a -= 6.28;
    if (try_a < -3.14)
        try_a += 6.28;
    tan_mmA = tan(pi_2 - try_a);
    for (i = 0; i < m_nNum; i++)
    { //	tempa=(m_points_x[i]-min_mis_x)*tan(min_mis_a)-min_mis_y;
        //	tempb=(m_points_y[i]-min_mis_y)/tan(min_mis_a)-min_mis_x;
        //	temp+=fabs(tempa*tempb*tempa*tempb/(tempa*tempa+tempb*tempb));
        //	temp+=(tan_mmA*m_points_x[i]-m_points_y[i]-min_mis_x*tan_mmA+min_mis_y)*(tan_mmA*m_points_x[i]-m_points_y[i]-min_mis_x*tan_mmA+min_mis_y)/(tan_mmA*tan_mmA+1);
        temp += fabs(m_points_y[i] - tan_mmA * (m_points_x[i] - min_mis_x) - min_mis_y) / sqrt(tan_mmA * tan_mmA + 1);
    }
    temp_min = temp;

    for (int j = 0; j < MAX_Gdown2_Loop_time; j++)
    {
        temp = 0;
        try_a = min_mis_a + step_a;
        try_x = min_mis_x;
        try_y = min_mis_y;
        if (try_a > 3.14)
            try_a -= 6.28;
        if (try_a < -3.14)
            try_a += 6.28;
        tan_mmA = tan(pi_2 - try_a);
        for (i = 0; i < m_nNum; i++)
        { //	tempa=(m_points_x[i]-min_mis_x)*tan(min_mis_a)-min_mis_y;
            //	tempb=(m_points_y[i]-min_mis_y)/tan(min_mis_a)-min_mis_x;
            //	temp+=fabs(tempa*tempb*tempa*tempb/(tempa*tempa+tempb*tempb));
            //	temp+=(tan_mmA*m_points_x[i]-m_points_y[i]-min_mis_x*tan_mmA+min_mis_y)*(tan_mmA*m_points_x[i]-m_points_y[i]-min_mis_x*tan_mmA+min_mis_y)/(tan_mmA*tan_mmA+1);
            temp +=
                fabs(m_points_y[i] - tan_mmA * (m_points_x[i] - min_mis_x) - min_mis_y) / sqrt(tan_mmA * tan_mmA + 1);
        }
        delta_temp_a = temp_min - temp;
        if (delta_temp_a ==
            0)               // 注意这个判定错误三项不为0时仍然和可为0 ((delta_temp_a + delta_temp_x + delta_temp_y)!=0)
            speed_abs = 0.1; // 如果有最极端的现象出现 各偏导数都为零 就用一个比较大的增长速率0.1
        else
            speed_abs = fabs(1 / delta_temp_a);
        double speed = 0.6 * (1 - (float)(j) / (float)(MAX_Gdown2_Loop_time)) + 0.0001; // 1;//
        double tempA = delta_temp_a * speed_abs * a_change_Gdown2_quotiety * speed;     // 6;//3;//*10;
        if (tempA > 0.017)                                                              // 如果角度增量上限大于1度
            tempA = 0.017;
        if (tempA < -0.017) // 如果角度增量上限小于-1度
            tempA = -0.017;
        min_mis_a += tempA;
        if (min_mis_a > 3.14)
            min_mis_a -= 6.28;
        if (min_mis_a < -3.14)
            min_mis_a += 6.28;
        temp = 0;
        try_a = min_mis_a;
        try_x = min_mis_x;
        try_y = min_mis_y;
        tan_mmA = tan(pi_2 - try_a);
        for (i = 0; i < m_nNum; i++)
        { //	tempa=(m_points_x[i]-min_mis_x)*tan(min_mis_a)-min_mis_y;
            //	tempb=(m_points_y[i]-min_mis_y)/tan(min_mis_a)-min_mis_x;
            //	temp+=fabs(tempa*tempb*tempa*tempb/(tempa*tempa+tempb*tempb));
            //	temp+=(tan_mmA*m_points_x[i]-m_points_y[i]-min_mis_x*tan_mmA+min_mis_y)*(tan_mmA*m_points_x[i]-m_points_y[i]-min_mis_x*tan_mmA+min_mis_y)/(tan_mmA*tan_mmA+1);
            temp +=
                fabs(m_points_y[i] - tan_mmA * (m_points_x[i] - min_mis_x) - min_mis_y) / sqrt(tan_mmA * tan_mmA + 1);
        }
        temp_min = temp;
    }
    *lx = min_mis_x; // m_fCenterX = A;
    *ly = min_mis_y; // m_fCenterY = B;
    *la = min_mis_a; // m_fRadius = R;

    temp = 0;
    try_a = min_mis_a;
    try_x = min_mis_x;
    try_y = min_mis_y;
    tan_mmA = tan(pi_2 - try_a); //
    float t;
    for (i = 0; i < m_nNum; i++)
    { //	tempa=(m_points_x[i]-min_mis_x)/tan(min_mis_a)-min_mis_y;
        //	tempb=(m_points_y[i]-min_mis_y)*tan(min_mis_a)-min_mis_x;
        //	t=fabs(tempa*tempb*tempa*tempb/(tempa*tempa+tempb*tempb));
        //	t=fabs((tan_mmA*m_points_x[i]-m_points_y[i]-min_mis_x*tan_mmA+min_mis_y)*(tan_mmA*m_points_x[i]-m_points_y[i]-min_mis_x*tan_mmA+min_mis_y)/(tan_mmA*tan_mmA+1));
        t = fabs(m_points_y[i] - tan_mmA * (m_points_x[i] - min_mis_x) - min_mis_y) / sqrt(tan_mmA * tan_mmA + 1);
        temp += t * t;
    }
    temp_min = temp;

    temp_min = temp_min / (float)(m_nNum);
    if (temp_min == 0)
        temp_min = 0.000000000001;
    return (temp_min);
}

// 最小方差评价直线拟合
float CPathPlan::LeastSquaresFitting4(float *m_points_x, float *m_points_y, int m_points_l, float *lx, float *ly,
                                      float *la)
{
    const double pi = 3.1415926535897932384626433832795;
    const double pi_2 = pi * 0.5;
    int m_nNum = m_points_l;
    if (m_nNum < 2)
    {
        return (0);
    }
    int i = 0;
    // 计算系数
    double A = 0.0;
    double B = 0.0;
    double C = 0.0;
    double D = 0.0;

    for (i = 0; i < m_nNum; i++)
    {
        A += m_points_y[i];
        B += m_points_x[i];
        C += (m_points_x[i] * m_points_y[i]);
        D += (m_points_x[i] * m_points_x[i]);
    }

    if ((B * B - D * m_nNum) == 0)
        return (0);
    double k = (A * B - m_nNum * C) / (B * B - m_nNum * D);
    double b = (B * C - A * D) / (B * B - m_nNum * D);

    float temp_min, temp = 0;
    for (i = 0; i < m_nNum; i++)
    {
        //	tempa=(m_points_x[i]-min_mis_x)*tan(min_mis_a)+min_mis_y;
        //	tempb=(m_points_y[i]-min_mis_y)/tan(min_mis_a)+min_mis_x;
        //	temp+=fabs(tempa*tempb*tempa*tempb/(tempa*tempa+tempb*tempb));
        //	0=kx-y+b  Ax+By+C=0;
        //	d^2=(A*x0+B*y0+C)*(A*x0+B*y0+C)/(A^2+B^2);
        temp += (k * m_points_x[i] - m_points_y[i] + b) * (k * m_points_x[i] - m_points_y[i] + b) / (k * k + 1);
        //		t=fabs(m_points_y[i]-tan_mmA*(m_points_x[i]-min_mis_x)-min_mis_y)/sqrt(tan_mmA*tan_mmA+1);
        //			temp+=t*t;
    }
    temp_min = temp;

    *lx = m_points_x[m_nNum - 1]; // m_fCenterX = A;
    *ly = m_points_y[m_nNum - 1]; // m_fCenterY = B;
    temp = atan(-k);              //-m_fRadius = R;
                                  /*	temp+=pi_2;
                                      if (temp>pi)
                                          temp-=pi*2;
                                      if (temp<-pi)
                                          temp+=pi*2;
                                  */
    *la = temp;

    temp_min = temp_min / (float)(m_nNum);
    if (temp_min == 0)
        temp_min = 0.000000000001;
    return (temp_min);
}

void CPathPlan::dataInPerset(void) // 将路径规划设置和花费地图设置 变为预设定值
{
    use_film_line = 0;                        // 设置使用场线 默认0不使用  1为使用
    Obstacle_size_set = OBSTACLE_SIZE_PERSET; // 设置每个障碍点的影响范围
}

void CTimer::reset()
{
    time = 0;
}
float CTimer::readtime()
{
    return (time++);
}

void CPathPlan::film_line2014_3mcircle(int use_line_num)
{ // 障碍物地图生成
    int i, j, x, y, tx, ty;
    float zoomX = (float)MAP_X / (float)MAP_Xmm;
    float zoomY = (float)MAP_Y / (float)MAP_Ymm;
    for (x = 0; x < MAP_X; x++)
    {
        for (y = 0; y < MAP_Y; y++)
        {
            oMap[y][x] = 0; // 边线的花费
        }
    }
    i = 300 * zoomX; // 10;体育馆//15;//22;//使用 多少半径进行标识 障碍点
    j = 0;
    while (j < omask_l0[i])
    {
        ty = omask_y0[i][j] + ftb_circle_oy * zoomY + MAP_oY * zoomY; // 输入生成花费地图用 障碍物数组 全局变量
        tx = omask_x0[i][j] + ftb_circle_ox * zoomX + MAP_oX * zoomX;
        if ((tx >= 0) && (tx < MAP_X) && (ty >= 0) && (ty < MAP_Y))
        {                                 //   oMap[ty][tx]=0;
            if (omask_w0[i][j] < i * 0.5) // 在0.2倍直径内 都是可行走度为0
                oMap[ty][tx] = 1;
            else
                oMap[ty][tx] = (i - omask_w0[i][j]) / i / (1 - 0.5); // 在0.2倍直径到1倍直径 可行走度为0到1
        }
        j++;
    }
    if (use_line_num == 2)
        for (x = 0; x < MAP_X; x++)
        {
            for (y = 0; y < MAP_Y; y++) // 255.0/((float)((unsigned char)(data[(y*mask4->width+x)*3+0]))+10) 0.96~25.5
            {
                if (oMap[y][x] > (11.5909 - foul_line_2Map[y][x]) / 11.5909)
                    oMap[y][x] = (11.5909 - foul_line_2Map[y][x]) / 11.5909; // 边线的花费
            }
        }
    if (use_line_num == 3)
        for (x = 0; x < MAP_X; x++)
        {
            for (y = 0; y < MAP_Y; y++) // 255.0/((float)((unsigned char)(data[(y*mask4->width+x)*3+0]))+10) 0.96~25.5
            {
                if (oMap[y][x] > (11.5909 - foul_line_3Map[y][x]) / 11.5909)
                    oMap[y][x] = (11.5909 - foul_line_3Map[y][x]) / 11.5909; // 边线的花费
            }
        }
    if (use_line_num == 4)
        for (x = 0; x < MAP_X; x++)
        {
            for (y = 0; y < MAP_Y; y++) // 255.0/((float)((unsigned char)(data[(y*mask4->width+x)*3+0]))+10) 0.96~25.5
            {
                if (oMap[y][x] > (11.5909 - foul_line_4Map[y][x]) / 11.5909)
                    oMap[y][x] = (11.5909 - foul_line_4Map[y][x]) / 11.5909; // 边线的花费
            }
        }
    if (use_line_num == 1)
        for (x = 0; x < MAP_X; x++)
        {
            for (y = 0; y < MAP_Y; y++) // 255.0/((float)((unsigned char)(data[(y*mask4->width+x)*3+0]))+10) 0.96~25.5
            {
                if (oMap[y][x] > (11.5909 - foul_line_Map[y][x]) / 11.5909)
                    oMap[y][x] = (11.5909 - foul_line_Map[y][x]) / 11.5909; // 边线的花费
            }
        }
}
#endif