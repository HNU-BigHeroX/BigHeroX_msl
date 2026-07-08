// stdafx.cpp : source file that includes just the standard includes
//	VoyVideo.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef RUN_WITH_EMU
serverSendStruct ReceiveServerData;   // 从服务器发来的环境数据
serverReceiveStruct sendToServerData; // 发往服务器的本机运动数据
int sendToServerData_kickTimelate = 0;
int m_nAgentID_g_set = 0;
#endif

float field_zoom_time = MK / 2.6; // 1.7   //   1/1.67=0.5833[体育馆]  [1/2.42]=0.41[实验室]
int io_cont = 0;                  // io 防抖
float lujingchang = 0;            // 路径 长度 加速度限制
float jg_wending;

int yu_bh, yu_bf, yu_bs, yu_jh, yu_jf, yu_js, yu_dh, yu_df, yu_ds;

unsigned long int SCDataCount = 0; // 串口自开机收到多少包数据
unsigned long int
    SCDataCount_P[100] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // 视频处理线程上贞记录
                                                                                             // 串口自开机收到多少包数据
                                                                                             // 在AfterUpdateOverlook里的记录传感器数据处更新
unsigned int erzhi_b[640][480];
unsigned int erzhi_black[640][480];
unsigned int erzhi_bai[640][480];
unsigned int erzhi_di[640][480];
unsigned int erzhi_yuandi[640][480];
unsigned int erzhi_bian[640][480];
bool linshi[640][480];
UINT bianliang1; // 存放开始的变量
byte qian_r[320][240];
byte qian_g[320][240];
byte qian_b[320][240];
int xingwei;
int xingweiP = 0;
int obstacle_shan_xingx[37][6000]; // 扇形一 每个扇形10度 一个扇形1600个点 1600 = (240*240)/36
int obstacle_shan_xingy[37][6000]; // 扇形一 每个扇形10度 一个扇形1600个点 1600 = (240*240)/36
int shandian[36];
int zhi_yichu;
int shan_xingx[10][6000]; // 扇形一 每个扇形10度 一个扇形1600个点 1600 = (240*240)/36
int shan_xingy[10][6000];
int C1_Bool_Data[307200];
int lmoto;
int rmoto;
int moto_b;
int moto_l;
int moto_r;
int tiqiu;
int valxx;
int netnum;
float netmove;
int banjing[640][480];    // 存放半径矩阵
BOOL weitu[640][480];     // 存放二值化的位图
int shendubiao[640][480]; // 存放深度图
BOOL changxian[240][240]; // 场线数组
float changxianx[5000];
float changxiany[5000];
int xiandianshu;
float changxianxT[5000];
float changxianyT[5000];
int xiandianshuT;
int shexianx[144][240];
int shexiany[144][240];
float quxian[241];
float quxian11[241];
float fanquxian[3000];

double benjijiao;
double menjiao;
BOOL lianxu[645][485];
int a_1[640][480];
int ax1[60000];
int ay1[60000];
int aa1[6000];
byte niuqu[921600];
long double juli_shang;
int bishe[144];
int tongxing[144];
double cdbl;
CString kongzhitai;
double ycjiao;
double ycchang;
double yczhuan;
int dianjix;
int dianjiy;
double dianjijiao;
int dianjipanding;
int xianshix[10];
int xianshiy[10];
short motospeed[5];
short motospeedP[5] = {0, 0, 0, 0, 0}; // 存储上周期电机输出
float stopvalxx = 1;                   // 存储软停止系数
int qulvb[240];
int luodianb[2500];
int shemenb[2500];
// byte rgb[][]

float adjust_table[241];   // cs 系统用无缩放畸变矫正表
int robot_min_radius[144]; ////存储 机体处理视觉信息 的 144个扇区的最小半径 来自mask2.bmp  来自Cpathplan的构造函数

int Learn_num = 10;
int Learn_Flag[10]; // 存储 学习用像量在最后一次有没有学习过的标志位  如果在最后一次学习中没用到的像量就将它舍掉
float Learn_A[10];  // 存储 学习用像量  最好有组表示每个像量有没有学习过的标志位
float Learn_R[10];

//	float in_obj_la[10000];//输入本机测量得到障碍物数组 全局变量
//	float in_obj_lr[10000];//畸变校正过的
//	float in_obj_org_lr[10000];//无畸变校正的
//	int   in_obj_ll;

//	float in_obj_x[10000];//输入生成花费地图用 障碍物数组 全局变量
//	float in_obj_y[10000];
//	int   in_obj_l;

// 存放 扇区 半径 对照掩膜
//	const int Sector_Num=144;
//	const int OMNI_VISION_CAM_WIDTH=640;
//	const int OMNI_VISION_CAM_HEIGHT=480;
//	int sector_mask[144][(int)(640*640/144*1.414)][2];//存放每个点的[0]y[1]x
//	int sector_mask_R[144][480/2];//存放每个扇区的每个半径在掩模中的起始位置
//	int sector_maskA[144][(int)(640*640/144*1.414*4)][2];//存放每个点的[0]y[1]x
//	int sector_maskA_R[144][480/2];//存放每个扇区的每个半径在掩模中的起始位置

int history_save[800][640]; //[0] long [1] start [2] dingwei x [3] dingwei y [4] dingwei a [5]
int JOY_Xpos = 32767;
int JOY_Ypos = 32767;
int JOY_Zpos = 32767;
float JOY_XposF = 0;
float JOY_YposF = 0;
float JOY_ZposF = 0;
int JOY_Buttons = 32767;
int JOY_Button[16] = {0};