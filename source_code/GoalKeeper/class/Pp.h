// path plan路径规划
// Ppg.h

// #include "stdafx.h"
#ifndef Pp_h
#define Pp_h
#include <math.h>
#include "Math2.h"
#include <iostream.h>

#define USE_OpenCv // 设置使用Open_Cv
#ifdef USE_OpenCv  // 使用Open_Cv
#include <cv.h>
#include <highgui.h>
//	#define    FUNCTION_TEST//设置使用Open_Cv 并开启测试模式
//	#define    FUNCTION_POINT_GROW_TEST//设置使用Open_Cv 并开启生长时的逐点测试模式
//	#define    FUNCTION_DSPLAY//设置使用显示图像信息
//	#define    FUNCTION_USE_MOUSE//设置使用鼠标调试
extern int MouseEvent_s_x;
extern int MouseEvent_s_y;
extern int MouseEvent_t_x;
extern int MouseEvent_t_y;
extern int MouseEvent_L_R;                                                      // 左键还是右键 //默认是按下过左键
extern void OpenCv_MouseEvent(int event, int x, int y, int flags, void *param); // 主显示窗口上的鼠标事件 用于测试用
#endif

#define MAP_X 208 // 160//320//640//
#define MAP_Y 148 // 120//240//480//

#define MAP_Xmm 2080 // 640//本地图宽度代表距离 单位cm
#define MAP_Ymm 1480 // 480//
#define MAP_oX 140   // 0//本地图直角坐标系原点坐标 单位cm
#define MAP_oY 140   // 0//既：场地每个边留出1.0米的移动空间

#define OBSTACLE_SIZE_PERSET 20; // 设置每个障碍点的默认影响范围

extern float in_obj_la[10000]; // 输入本机测量得到障碍物数组 全局变量
extern float in_obj_lr[10000];
extern float in_obj_org_lr[10000];
extern int in_obj_ll;

extern float in_obj_x[10000]; // 输入生成花费地图用 障碍物数组 全局变量
extern float in_obj_y[10000];
extern float in_obj_w[10000]; // 障碍点范围
extern int in_obj_l;

extern int cmask_l; // 生长用四连通域掩模
extern int cmask_x[100];
extern int cmask_y[100];
extern int cmask_l1; // X1倍的分辨率
extern int cmask_x1[100];
extern int cmask_y1[100];
extern int cmask_l2; // X1/2/2倍的分辨率
extern int cmask_x2[100];
extern int cmask_y2[100];
extern int cmask_l4; // X1/4/4倍的分辨率
extern int cmask_x4[100];
extern int cmask_y4[100];

extern char zoomR[80][80];
extern const unsigned char zoomR_max;

// 生长用变量
extern unsigned int marea_grow_data_arex[MAP_Y * MAP_X * 10]; //[640*480];//currently当前生长的一圈的纪录
extern unsigned int marea_grow_data_arey[MAP_Y * MAP_X * 10]; //[640*480];//currently当前生长的一圈的纪录

extern float mmap_tc_data[MAP_Y][MAP_X]; // 出发点达到每个格的花费时间
extern float oMap[MAP_Y][MAP_X];         // 每个格的花费
// float tMap[480][640];//出发点达到每个格的花费时间
extern float opMap[MAP_Y][MAP_X][3]; // 存储每个点的 父点的影射出发点 和向量方向 [父点y] [父点x] [方向]
extern int path_l;                   // 存储路径点用
extern float path_x[10000];
extern float path_y[10000];

// 存储过生长点的结构体
struct Over_Grow_Nod
{
    int x; // 当前节点x y坐标
    int y;
    float mmap_tc_data; // 当前节点花费
    //	Over_Grow_Nod *previous;//上一个节点 比自己花费小的
    Over_Grow_Nod *next; // 下一个节点 比自己花费大的
};

class CPathPlan
{
  protected:
  public:
    CPathPlan(void);
    ~CPathPlan(void);

    void Ppgrow(void);       // 从生长起始点 开始生长
    float path_plan_angle;   // 路径规划输出方向path plan angle//在场地直角坐标系下机器要行进的方向 角度制
    float path_plan_angle_P; // 上周期路径规划方向 //如果上周期没进行路径规划 也可以使用机体行进方向 或带球方向 为此值
                             // 影响防震荡方向掩膜
    float Close_target_direction;     // 设置接近目标的方向
    float path_plan_start_angle_cost; // 起始方向花费
    // void datain(char *data,int xlong,int ylong);//障碍地图数据输入部分
    void Close_target_direction_01(float ttx, float tty, float directx,
                                   float Direct_Cost);       // 会把需要的目标方向用方向掩膜加进花费地图
    void dataInPerset(void);                                 // 将路径规划设置和花费地图设置 变为预设定值
    char use_film_line;                                      // 设置使用场线 默认0不使用  1为使用
    void add_obstacle_point(float x, float y, float weight); // evade设置躲避指定点  默认x y场地坐标  weight大小
    void datain(float ssx, float ssy, float ttx, float tty); // 障碍地图数据输入部分 且输入目标位置[场地直角坐标系]
    void dataout(char *data, int xlong, int ylong);          // 障碍地图数据输出部分

    float Obstacle_size_set; // 10;体育馆//15;//22;//使用 多少半径进行标识 障碍点
    double s_x;              // 开始坐标
    double s_y;
    double t_x; // 目标坐标
    double t_y;

    // 过生长点存储用节点
    Over_Grow_Nod *s_Nod;      // 开始节点
    Over_Grow_Nod *r_Nod;      // 正在读取节点
    Over_Grow_Nod *r_PNod;     // 上周期读取节点
    Over_Grow_Nod *temp_r_Nod; // 正在读取节点 暂存
    Over_Grow_Nod *e_Nod;      // 结束节点  //注意此节点一般只有 *previous这个回溯指针有有效数据
    // 过生长点存储用节点操作
    inline void C_nod(int x, int y, float mmap_tc_data); // 创建 插入一个节点//添加一个节点
    inline Over_Grow_Nod *D_nod(Over_Grow_Nod *Pnod,
                                Over_Grow_Nod *nod); // 删除 去掉一个节点 返回*nod的下一个节点 *Pnod是*nod的上一个节点

    float LayerMin;                                          // 当前层 花费最小的
    float GLayerMax;                                         // 当前层 可达到最大花费
    inline float refraction02(int fx, int fy, int x, int y); // 靠类似折射反射 原理 生成花费

    // 生成浮点圆环掩模用函数
    void circle_maskGf(float radius, float *Gcmask_x, float *Gcmask_y, int Gcmask_lo);
    void circle_maskG(float R, int *Gcmask_x, int *Gcmask_y, int *Gcmask_lo);                    // 生成圆环掩模用函数
    void circle_maskGA(float R, int *Gcmask_x, int *Gcmask_y, float *Gcmask_w, int *Gcmask_lo);  // 生成实心圆掩模用函数
    void circle_maskGA_(float R, int *Gcmask_x, int *Gcmask_y, float *Gcmask_w, int *Gcmask_lo); // 生成实心圆掩模用函数
    void circle_maskGAA(
        float R, float angle, int *Gcmask_x, int *Gcmask_y, float *Gcmask_w,
        int *Gcmask_lo); // 生成有方向 圆掩模用函数 x坐标数组 y 坐标数组 当前点与基准点偏差角度(0~180 对应 0~1) 数组长度
    void circle_mask(void); // 生成此类需要使用所有圆环掩模

    void add_oMap_start_driect(int startX, int startY, float startA); // 在花费图中加入机体起始方向的因素

    // void LeastSquaresFitting();
    float LeastSquaresFitting(float *m_points_x, float *m_points_y, int m_points_l, float *cx, float *cy, float *cr);
    float LeastSquaresFitting2(float *m_points_x, float *m_points_y, int m_points_l, float *cx, float *cy, float *cr);
    float LeastSquaresFitting3(float *m_points_x, float *m_points_y, int m_points_l, float *lx, float *ly,
                               float *slope);
    float LeastSquaresFitting4(float *m_points_x, float *m_points_y, int m_points_l, float *lx, float *ly, float *la);

#ifdef USE_OpenCv
    int robot_min_r[144];        // 存储 机体处理视觉信息 的 144个扇区的最小半径 来自mask2.bmp
    IplImage *src;               // 定义输出和运算用图像指针
    IplImage *src_S;             // 定义源图像指针
    IplImage *src_zoomR;         // 定义变分辨率源图像掩模指针
    void OpenCv_CPathPlan(void); // 配合本类构造函数 进行OpenCV兼容部分初始化
    void OpenCv_Del_CPathPlan(void);
    void OpenCv_DrawPoint(int x, int y, unsigned char R, unsigned char G, unsigned char B, IplImage *img);
    BOOL OpenCv_Draw_line(int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b,
                          IplImage *img);
    void OpenCv_circle(int x, int y, float R, int r, int g, int b, IplImage *img);
    void datainT(void); // 障碍地图数据输入部分 使用Open_Cv输入图片测试
    // 用来显示文字,初始化字体
    CvFont out_txt;
#endif

    void film_line2014_3mcircle(int use_line_num); // 三米圆掩膜
    float ftb_circle_ox;                           // 三米圆中心
    float ftb_circle_oy;
};

class CTimer
{
    float time;
    void reset();
    float readtime();
};
#endif