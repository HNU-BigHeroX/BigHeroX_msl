// GetImage.h: interface for the CGetImage class.
//
//////////////////////////////////////////////////////////////////////
/*************************************************
  http://robot.up-tech.com
  Author: 徐鑫鑫      Version: 1.1       Date: 2007/10/19
  Email:  zwj_uptech@126.com
  Description:    // 调试版行为
  Others:         //
*************************************************/

#if !defined(AFX_GETIMAGE_H__89E0E158_A6BB_4E5B_9BC7_67E7D82FB2E2__INCLUDED_)
#define AFX_GETIMAGE_H__89E0E158_A6BB_4E5B_9BC7_67E7D82FB2E2__INCLUDED_
#include "IBehavior.h" //行为基类头文件
#include "Vision.h"
#include "UPRColor.h"
#include "path.h"
#include "Yuzhi.h"
// #include "SerialCom.h"
#include "VoyCmd.h"
#include "AgentUDP.h"
#include "VoyHSI.h"
#include "RGB2HSI.h"
#include "./../class/Pp.h"
#include "..\.\class\Debug_Display.h" //调试信息图形显示用类
#include "..\.\class\Movement2Control.h"
#include "..\.\class\Dribble.h"  //带球类
#include "..\.\class\ballshot.h" //球起飞判定
#include "RobotPlayerAMC.h"
#include "..\WaterMulticast.h"
#include "..\GKRecerive.h"

// #ifdef  RUN_WITH_LOCAL_EMU
//	#include "..\.\class\LOCALEMU.h"//本地仿真类
// #endif
#include "..\.\class\LOCALEMU.h" //本地仿真类

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// #define NO_MOVE //开机正常运行但机体不移动
// #define MOUSE_SET_BALL//用鼠标设置球的位置
#define Obstacle_Parameter 8.2 // 障碍物基本大小
// #define CODED_EMU //使用码盘模拟  在机体码盘回传数据不可用的情况下 使用上周期电机值作为当前码盘测量转速
#define USE_VISION_LOCATION 0 // 设置使用视觉定位做接球所有数据基础 1使用视觉 0使用码盘
#define USE_KALMAN_BALL 1     // 设置使用KALMAN做接球所有数据 1使用 0 no使用
class CGetImage : public IBehavior
{
  public: // 2014
    CWaterMulticast m_zubo;

  public: // 2013EMU
#ifdef RUN_WITH_LOCAL_EMU
    void AfterUpdateOverlookEMU(BYTE *pBuffer, long lWidth, long lHeight, double dbTime, UINT nState);
#endif
  public:                  // 2014
    float first_touch_bfx; // 首次触球 场地坐标系x
    float first_touch_bfy; // 首次触球 场地坐标系y
    int first_touch_bt;    // 首次触球 有效计数器0 非触球 >0触球过程中
    int field_p_num_P;     // 记录上周期（进攻 传球 跑位）目的路点编号
    int nVisionFps;        // 视觉帧率，返送到教练机
  public:                  // 2012
    int mtl1;
    int mtr1;
    int mtb1;
    float move_Emu_ballX;
    float move_Emu_ballY;
    double move_Emu_robot_f_x;
    double move_Emu_robot_f_y;
    double move_Emu_robot_f_a;
    float move_Emu_ballSX;
    float move_Emu_ballSY;
    float move_Emu_robot_Sf_x;
    float move_Emu_robot_Sf_y;
    float move_Emu_robot_Sf_a;
    int catch_start_wait_loop; // 接球 队员 切接球后 等待球速度方向稳定 等待周期计数
    int xingwei_catch_timer;   // 接球 队员保持行为周期计数
    int catch_xingwei_P;       // 接球队员最后执行的是接球 还是接球跑位 默认1002接球跑位
    float move_Net_ballf_SX;
    float move_Net_ballf_SY;
    int jieqiuqs;
    double jieqiux;
    double jieqiuy;
    double jdx;
    double jdy;
    double jieqiusudu_s;
    double jieqiuc_s;
    double jieqiujf;
    float catch_first_view_ball_X;
    float catch_first_view_ball_Y;
    float catch_first_view_ball_W;

    float fSoftwareVer;
    CGKRecerive m_GKRecv; // 接收Kinect程序发来的球坐标

  public: // 2011
    CMovement2Control move_test_C;

  public:
    // CRobotPlayerAMC* pRobot;
    CVoyCmd *pRobot;
    LARGE_INTEGER Loop_Time_Count_Start;
    LARGE_INTEGER Loop_Time_Count_End;
    double LOOPTIME; // 记录视觉周期花费时间
    void DebugInfoS(CString inStr);
    void DebugInfoM(CString inStr);
    void xiqiujiexi(int motol, int motor, int motob, double time, short *l, short *r);
    void Kalman(double *x_p, double *v_p, double xm, double a, double b);
    void AfterUpdateAttitude(FLOAT inAngle, int inXMagn, int inYMagn, FLOAT inXRoll, FLOAT inYRoll);
    void dianjijiexi(double motol, double motor, double motob, double motol_s, double motor_s, double motob_s,
                     double *jiao, double *chang, double *zhuan);
    void guolvzaodian();
    void yanjiao(double x1, double y1, double x2, double y2, double chang, double jiao, int *x, int *y);
    void daiqiu(int qiansudu1, double mubiao);
    void zhuiqiu(double mujiao, double muchang, double c_shang);
    void yanchangxian(double x1, double y1, double x2, double y2, double chang, int *x, int *y);
    void vhecheng(double jiao1, double sudu1, double top1, double jiao2, double sudu2, double top2, double *jiao,
                  double *sudu);
    void jiaodujiaozhun(int jiao, Yuzhi *mubiao);
    void lujing(double jiaom, double changm, double *jiao, double *chang);
    void xianzhi(int *motol, int *motor, int *motob, int zhi);
    void jiasudu(int *motol_x, int *motor_x, int *motob_x, int *motol_y, int *motor_y, int *motob_y, int zhi);
    int duizhun(double jiao, double jizhun, int top);
    void Controlmoto(int jiao, int sudu, int zhuan, int *motol, int *motor, int *motob);
    void hecheng(BYTE *pBuffer);
    void AfterUpdateOverlook(BYTE *pBuffer, long lWidth, long lHeight, double dbTime, UINT nState);
    int jiajiao(int jizhun, int jiao);
    void jiaoduizhun(int *motol, int *motor, int jiao, int jizhun, double zhuan, double qian);
    void zhuanhuan(int x);
    float dbx_s;
    float dby_s;
    CAgentUDP net;
    //	UPRColor *m_find2;
    UPRColor m_find;
    //	UPRColor m_find2;
    RGBTRIPLE prgb;
    //	HSIParam  m_HSI ;
    BOOL num;
    int smjishu;
    int L_moto1;
    int R_moto1;
    Yuzhi y;
    //	HSIParam m_HSI;
    YUVParam m_para;
    void AfterUpdateVideoSample(BYTE *pBuffer, long lWidth, long lHeight, double dbTime, UINT nState);
    VoyHSI wenjian;
    HSISet m_hsiset;
    int dzhenlv;
    CEdit *m_pShow;
    CGetImage();
    virtual ~CGetImage();
    //	byte hecheng[230400];
    byte tuxiang2[230400];
    int qiansudu;
    int jiasudu1;

    int motol_as;
    int motor_as;
    int motob_as;
    int chongqiu;
    double pingjun;
    // 带球半径
    double holdball;
    int ballzdj;
    unsigned char *m_pMPU6050Data;
    int *m_pMotorPos;
    short *m_pMotorCurrent;
    bool *m_pIO;
    WORD *m_pAD;

    bool m_bNetSend;
    int xqpanding;
    int xqjishu;
    int djkaiqiu;
    int djkaiqiujs;
    int puqiulj;

  protected:
    int xiqiuz_s;
    int xiqiuy_s;
    double mujiao_s;
    int daduobi;
    int pxqiehuanbj;
    int pxqiehuan;
    int jifen_s;
    int jieqiuys;
    int jieqiuys_2; // 接球累计时间
    double benjix_s;
    double benjiy_s;
    double benjijiao_s;
    int daiqiumb;
    int daiqiumb_change_timer;
    int pass_move_stick_timer;
    int kaijipd;
    int pqzy1;
    int pqzy;
    int kaiqiudj;
    int kaiqiujfx;
    int kaiqiujfy;
    int kaiqiusj;
    int kaiqiubx;
    int kaiqiuby;
    int kaiqiubk;
    int chiqiupd;
    int mtl_s1;
    int mtr_s1;
    int mtb_s1;
    int gbpanding;
    float Path_LA;   // 当前周期 路径规划本机角度
    float Path_LA_P; // 当前周期 路径上个周期规划本机角度
    float Path_FA;   // 当前周期 路径规划全场角度
    float Path_FA_P; // 当前周期 路径上个周期全场结果
    double kaiqiuyidongchang;
    double kaiqiux;
    double kaiqiuy;
    double kaiqiujiao;
    double kaiqiuchang;
    int meny;
    int ballx[20];
    int bally[20];
    int pandingdian[5];
    int xing13;
    int kaiqiuting;
    int gbxiang;
    int kaifuzhu;
    double ballqx;
    double ballqy;
    int ballqz;
    int qishipanding;
    int qishipanding1;
    int tingzhipanding;
    int tingzhipanding1;
    int guibizhuangtai;
    int movepanding;
    int movesudu;
    LARGE_INTEGER Frequency1, CountEnd1, CountStart1;
    double dfElapseMS4;
    double ms_shang[10];
    double ms_shang1;
    int leijiaqi;
    double jifen;
    double zhuan_shang;
    int shemenleijia;
    int smycy[100];
    UPRColor *m_find2;
    HICON m_hIcon;
    int guibileijia;
    Cpath write;
    CPoint m_pt;
    double chang_shang;
    long zhenshu;

    //////////////////////////////////////////////////////////////////////////
    // 从AfterUpdateVideoSample函数里移出来的
    int zhuan;       // 转动量
    int mtl;         // 左电机转速
    int mtr;         // 右电机转速
    int mtb;         // 后电机转速
    int kick;        // 射门
    int zhangaijiao; // 有障碍物时门的角度
    int zhangaiyuan; // 有障碍物时门的角度
    int zhangaiyuanh;
    double dingweijiao;
    double dingweichang;
    CString zhuangtai; // 机器人的策略状态
    CString strShow;   // 输出运动过程中的调试参数
    // HSIParam  *m_HSI= new HSIParam; //生成HSI结构体
    RGBTRIPLE ball;
    CRGB2HSI m_r2h;
    int paowei;
    int paoweileiji;
    double xp, xvp, yp, yvp;

    // 路程定位 检错机制 cs
    void zijian(float mtb, float mtl, float mtr, float f_x, float f_y, float f_a, float *SQd, float *SQv, float *SQ,
                float *SQa, float *SQc);
    void int_zijian(float f_x, float f_y, float f_a);
    int i;
    obj_properties robot_L_c;   // 存储机器 路程定位当前位置
    obj_properties robot_h[20]; // 存储机器 在20周期内的 运动位置等信息历史记录
    float robot_L_vd_h[20];     // 存储机器 视觉定位每贞路程
    float robot_L_dd_h[20];     // 存储机器 路程定位每贞路程
    // T一个周期时间  v机体 平移速度 v_a机体速度角度 w机体转动速度 local机体物理状态存储结构体
    void Movement(float T, float v, float v_a, float w, obj_properties *local);
    bool O_movement_T_analyze(float *move_v_angle, float *move_v_speed, float *turn_v, float motor1, float motor2,
                              float motor3);             // 全向解析
    void motor_a_limit(float mtb, float mtl, float mtr); // 使用加速度限制 预测 机体电机轴速
    // 运动反解
    // 对球的对地运动矢量计算
    float mbh[20]; // 电机的历史记录 [0]为上个周期的
    float mlh[20];
    float mrh[20];
    // 电机速度真值 估算值  (加入硬件单轴加速度限制 考虑)
    float mbR;
    float mlR;
    float mrR;
    long double shijian;
    int shijiandian;
    int mapan_s[3];
    float leijia;
    float leijia1;
    float benjix1;
    float benjiy1;
    float benjijiao1;
    double bxyc[1000];
    double byyc[1000];
    float quanball_h[1000][3];            //[0]y,[1]x,[2]w //全场球信息历史记录
    float Shot_ball_point_X;              // 球起飞全场定位点x
    float Shot_ball_point_Y;              // 球起飞全场定位点y
    int Shot_ball;                        // 球起飞判定
    float set_speed_x, set_speed_y;       // 存储机体在场地上的希望行进速度
    float min_dis_ball_x, min_dis_ball_y; // 存储从起飞判定成立开始距机体最近的球的全场坐标 球没起飞时为球的当前场地坐标

    double bjxyc;
    double bjyyc;
    double bjjyc;
    int leijiayc;
    int leijiaycx;
    int leijiaycy;
    double ballx_s1;
    double bally_s1;
    double ballx_shang;
    double bally_shang;
    double ballj_shang;
    double ballc_shang;
    double qbx_shang;
    double qby_shang;
    int diuqiuleijia;
    double fangshoujiao;
    double daiqiuz_s;

    int gankzt;
    int ganksx;
    int gankjs;
    int gankpq;
    int gankx[30];
    int ganky[30];
    int zwxiqiu;
    int xiqiuzhi;
    //////////////////////////////////////////////////////////////////////////
  public:
    int duizhun1(double jiao, double jizhun, int top);
    void zhuiqiu1(double mujiao, double muchang, double c_shang);
    void zhuiqiu_(double mujiao, double muchang, double c_shang);
    void adxiqiu();
    void balldiushi(Yuzhi *changdi);
    Dribble Dribble_ball;           // 带球程序
    CDebug_Display Debug_D[38];     // 调试信息图形显示用类
    CMovement2Control move2Control; //
                                    // cs变量命名
    double ball_angle, ball_radius, ball_weight;
    double jdoor_angle, jdoor_radius, jdoor_weight, jdoor_left, jdoor_right, jdoor_width; // 自己们的x,y,角度，长度
    double ddoor_angle, ddoor_radius, ddoor_weight, ddoor_left, ddoor_right, ddoor_width; // 对方门的x,y,角度，长度
    // cs畸变校正过的变量命名
    double ball_Rradius;
    double jdoor_Rradius;
    double ddoor_Rradius;

    // cs判定函数
    // 追球判定  区分规避和追球  既球与机体间有无障碍物
    int Trace_ball_determinant(float target_angle, float forward_direction_);
    // 带球判定  生成带球旋转量
    int Call_Dribble_ball(float forward_direction_);

    float head_direction;    // 存储 机器朝向什么方向 本机坐标
    float forward_speed;     // 存储 规避行进速度
    float forward_direction; // 存储 规避行进方向 本机坐标

    // cs 带球对外接口
    // 运动反解 验证调试程序
    bool update;        // 带球用 更新标志
    float daiqiu_zhuan; // 带球更新 旋转量
    float G_turn_w;     // 带球 旋转量

    // cs 路径规划设置变量
    int target_num;        // 设置路径规划目的地为 球	约定 traget_numC号 初始化值0   1球 2敌方门 3己方门  4以上其他点
    int use_side_line;     //	约定use_side_line号 为1 路径规划中加入边线信息  !=1时无边线信息
    float add_oppo_radius; // 规避半径初始化6.5 8.0

    void call_machine_location(double jiaoa, double ball_Rradius, double *tf_x, double *tf_y, double fx, double fy,
                               double fa, double br, double ba);
    CPathPlan pp1;

    //////////////////////////////////////////////////////////////////////////
    float ball_fxp; // 存上周期球的全场位置 折算 [1200*1800]坐标系下
    float ball_fyp;

    BallShot bshot; // 球起飞判定

    float xingwei30ballXh[900]; // 接球用 开始接球后球历史记录 2014
    float xingwei30ballYh[900];
    int xingwei30ballLh; // 接球用数组记录到的位置

    int xingwei_1001_shoot_end_timer; // 球传出修正接球位置  保持时间标志
};

#endif // !defined(AFX_GETIMAGE_H__89E0E158_A6BB_4E5B_9BC7_67E7D82FB2E2__INCLUDED_)
