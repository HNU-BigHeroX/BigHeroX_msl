// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B1B1C910_3AF4_4BD9_9DCF_949CF77FCE1B__INCLUDED_)
#define AFX_STDAFX_H__B1B1C910_3AF4_4BD9_9DCF_949CF77FCE1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>   // MFC core and standard components
#include <afxext.h>   // MFC extensions
#include <afxdisp.h>  // MFC Automation classes
#include <afxdtctl.h> // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h> // MFC support for Windows Common Controls
#endif              // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h> // MFC socket extensions

#include <atlbase.h>
// #include <streams.h>

#include "math.h"                      //cs
#include "class\math2.h"               //cs
#include "class\ObjPropertiesStruct.h" //cs
#include "class\MomentSetFile.h"       //cs
#include "ConstantFile\CamConstantSet.h"

#define MK 1.35 // 2.7//2.7//     3.2//2.9//1.89 //2.8//1.25//实验室2.7//1.7//1.2//2.7 //体育馆  1.25
#define DIANJI                                                                                                         \
    5.5 // 4.5//6//4.5//3.6          //6.0//3//4.0//1.8416326530612244897959183673469  //22.56/12.25
        // //体育馆  1.25  1.25//实验室

// #define RUN_WITH_JOY	//设置使用手柄
#define USE_GYRO_MPU6050 // 设置使用陀螺仪
// #define TECHCHALLENGE2013_1	//设置使用2013技术挑战赛 反向传球模式 _1
// #define RUN_WITH_EMU	//设置使用仿真
#ifdef RUN_WITH_EMU
// #define RUN_WITH_EMU_LINK_MSLCoach2 //设置使用本机上的2号教练机 用于测试双队对抗
// #define RUN_WITH_LOCAL_EMU//设置使用本地单机仿真
struct serverReceiveStruct
{
    short serverReceiveEMU_motospeed[5];
    short kickTime;
    int holdball;
};
struct serverSendStruct
{
    float robotLocal_x[12];
    float robotLocal_y[12];
    float robotLocal_a[12];
    float ballLocal_x;
    float ballLocal_y;
};
extern serverSendStruct ReceiveServerData;   // 从服务器发来的环境数据
extern serverReceiveStruct sendToServerData; // 发往服务器的本机运动数据
extern int sendToServerData_kickTimelate;
extern int m_nAgentID_g_set;
#endif

extern float field_zoom_time;
extern float jg_wending;
extern int io_cont;       // io 防抖
extern float lujingchang; // 路径 长度  加速度限制

extern unsigned long int SCDataCount; // 串口自开机收到多少包数据
extern unsigned long int
    SCDataCount_P[100]; // 视频处理线程上贞记录 串口自开机收到多少包数据 在AfterUpdateOverlook里的记录传感器数据处更新
extern int yu_bh, yu_bf, yu_bs, yu_jh, yu_jf, yu_js, yu_dh, yu_df, yu_ds;
extern unsigned int erzhi_b[640][480];
extern unsigned int erzhi_black[640][480];
extern unsigned int erzhi_bai[640][480];
extern unsigned int erzhi_di[640][480];
extern unsigned int erzhi_yuandi[640][480];
extern unsigned int erzhi_bian[640][480];
extern bool linshi[640][480];
extern byte qian_r[320][240];
extern byte qian_g[320][240];
extern byte qian_b[320][240];

extern int xingwei;
extern int xingweiP;
extern int obstacle_shan_xingx[37][6000]; // 扇形一 每个扇形10度 一个扇形1600个点 1600 = (240*240)/36
extern int obstacle_shan_xingy[37][6000]; // 扇形一 每个扇形10度 一个扇形1600个点 1600 = (240*240)/36
extern int shandian[36];                  // 扇形一 每个扇形10度 一个扇形1600个点 1600 = (240*240)/36
extern int shan_xingx[10][6000];          // 扇形一 每个扇形10度 一个扇形1600个点 1600 = (240*240)/36
extern int shan_xingy[10][6000];
extern int C1_Bool_Data[307200];
extern UINT bianliang1;
extern int lmoto;
extern int rmoto;
extern int moto_b;
extern int moto_l;
extern int moto_r;
extern int tiqiu;
extern int valxx;
extern int zhi_yichu;
extern int netnum;
extern float netmove;
extern int banjing[640][480];
extern BOOL weitu[640][480];
extern int shendubiao[640][480];
extern BOOL changxian[240][240];
extern float changxianx[5000];
extern float changxiany[5000];
extern int xiandianshu;
extern float changxianxT[5000];
extern float changxianyT[5000];
extern int xiandianshuT;
extern int shexianx[144][240];
extern int shexiany[144][240];
extern float quxian[241];
extern float quxian11[241];
extern float fanquxian[3000];
extern double benjijiao;
extern double menjiao;
extern BOOL lianxu[645][485];
extern int a_1[640][480];
extern int ax1[60000];
extern int ay1[60000];
extern int aa1[6000];
extern byte niuqu[921600];
extern long double juli_shang;
extern int bishe[144];
extern int tongxing[144];
extern double cdbl;
extern CString kongzhitai;
extern double ycjiao;
extern double ycchang;
extern double yczhuan;
extern int dianjix;
extern int dianjiy;
extern double dianjijiao;
extern int dianjipanding;
extern int xianshix[10];
extern int xianshiy[10];
extern short motospeed[5];
extern short motospeedP[5]; // 存储上周期电机输出
extern float stopvalxx;     // 存储软停止系数
extern int qulvb[240];
extern int luodianb[2500];
extern int shemenb[2500];

extern float adjust_table[241];   // cs 系统用无缩放畸变矫正表
extern int robot_min_radius[144]; ////存储 机体处理视觉信息 的 144个扇区的最小半径 来自mask2.bmp 来自Cpathplan的构造函数

extern int Learn_num;
extern int Learn_Flag[10]; // 存储 学习用像量在最后一次有没有学习过的标志位  如果在最后一次学习中没用到的像量就将它舍掉
extern float Learn_A[10];  // 存储 学习用像量  最好有组表示每个像量有没有学习过的标志位
extern float Learn_R[10];

extern int history_save[800][640];
extern int JOY_Xpos;
extern int JOY_Ypos;
extern int JOY_Zpos;
extern float JOY_XposF;
extern float JOY_YposF;
extern float JOY_ZposF;
extern int JOY_Buttons;
extern int JOY_Button[16];
// extern	float in_obj_la[10000];//输入本机测量得到障碍物数组 全局变量
// extern	float in_obj_lr[10000];//畸变校正过的
// extern	float in_obj_org_lr[10000];//无畸变校正的
// extern	int   in_obj_ll;

// extern	float in_obj_x[10000];//输入生成花费地图用 障碍物数组 全局变量
// extern	float in_obj_y[10000];
// extern	int   in_obj_l;

// extern	int sector_mask[144][(int)(640*640/144*1.414)][2];//存放每个点的[0]y[1]x
// extern	int sector_mask_R[144][480/2];//存放每个扇区的每个半径在掩模中的起始位置
// extern	int sector_maskA[144][(int)(640*640/144*1.414*4)][2];//存放每个点的[0]y[1]x
// extern	int sector_maskA_R[144][480/2];//存放每个扇区的每个半径在掩模中的起始位置

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B1B1C910_3AF4_4BD9_9DCF_949CF77FCE1B__INCLUDED_)
