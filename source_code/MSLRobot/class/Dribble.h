// Dribble.h
// 注意：当运动跟踪目标切换时 原来的积分量速度记录 等还在保存 会引起初运动不正常

#ifndef Dribble_h
#define Dribble_h

// #include "ObjPropertiesStruct.h"
#include ".././ConstantFile/CamConstantSet.h"
#include "..\.\class\Movement2Control.h"
#include "..\.\class\Debug_Display.h" //调试信息图形显示用类
#include "..\.\class\ObjPropertiesStruct.h"

#define def_display_value_h 100 // 默认显示数值区域标准高度
// 并不是显示区域之在上述 +-100 条框里  只是这个值与Draw_vp(,,,)函数里的 int Value值域参数 按照y=Value=1000 out=100  out
// = y*100/value  out = y*def_display_value_h/value
#define def_display_height OMNI_VISION_CAM_HEIGHT // 默认图像存储区域高度
#define def_display_width OMNI_VISION_CAM_WIDTH   // 默认图像存储区域宽度
#define def_display_widthM3 def_display_width * 3
#define mirror_center_y OMNI_VISION_CAM_HEIGHT / 2
#define mirror_center_x OMNI_VISION_CAM_WIDTH / 2
// FieldZoomTime FieldZoomTime往大改 在地图中走的距离会减小
#define FieldZoomTime                                                                                                  \
    field_zoom_time // 0.5833//体育馆  1/2.42//实验室 1/1.67//  1  //  1/1.67    // 7.0/12.0==0.58	  5.0/12.0==1/2.42
                    // //场地比例参数  实际场地y长度/(定值12)
#define TargetGoalLX 18.0        // 对方球门左门柱 x坐标
#define TargetGoalLY (6.0 + 1.0) // 对方球门左门柱 y坐标
#define TargetGoalRX 18.0        // 对方球门右门柱 x坐标
#define TargetGoalRY (6.0 - 1.0) // 对方球门右门柱 y坐标
#define FHaveBallWeight 3333     // 前景有球判定重量

class Difference
{
  public:
    Difference();
    void New_Cycle(void);        // 一周期完成 差分过标志 置1       在 一个周期完结 或周期刚开始 运行
    float diff(float a_c);       // 差分函数
    float diff_angle(float a_c); // 角度差分函数
  protected:
    float a_p;
    char diff_flag;  // 差分过标志
    char new_set_up; // 建立标志 保证在a_p未赋值时第一次运行输出为0
};

class Dribble
{
  protected:
  public:
    float fieldzoomtime;         // 缩放比例对外接口
    float RMfv, RMfa, RMw;       // 实测码盘 得到的底盘 线速度(m/s) 速度方向(度) 转动速度(度/s)
    float RMfv_m, RMfa_m, RMw_m; // 实测码盘 得到的底盘 线速度(码盘线/s) 速度方向(码盘线/s) 转动速度(码盘线/s)
    float ball_op_Fx;            // 前景球的x
    float ball_op_Fy;            // 前景球的y
    float ball_op_Fa;            // 前景球的angle
    float ball_op_Fr;            // 前景球的radius
    float ball_op_Fweight;       // 准备把象素重量传给主逻辑函数
    char FHaveBall;              // 前景有球判定 1有球0无球

    float ball_org_angle; // 运动估算前的输出
    float ball_org_radius;

    float ball_estimate_angle; // 运动估算后的输出
    float ball_estimate_radius;
    float ball_estimate_F_angle; // 前景运动估算后的输出
    float ball_estimate_F_radius;

    float ddoor_left_e, ddoor_right_e;
    float ddoor_left_o, ddoor_right_o;

    char display; // 在前景是否显示 坐标线 默认1 显示 否则不显示
    Dribble();
    //   ~Dribble();
    // virtual 虚函数字头

    int GetMouseX(void); // 得到 鼠标x坐标
    int GetMouseY(void); // 得到 鼠标y坐标

    // 前景摄像机 处理增加函数
    void AfterUpdateVideoSample_add(BYTE *pBuffer, long lWidth, long lHeight, int bhs, int bhx, int bss, int bsx,
                                    int bvs, int bvx);

    // 带球函数 及其附属变量
    CMovement2Control move2Control;
    void ni_uptech_output_O(float *va, float *v, float *w, float ml, float mb,
                            float mr); // 逆转换 将博创底盘 电机输出转换成 128体系输出的运动矢量
    void Save_Moment_State(float motor1, float motor2,
                           float motor3); // 通过电机参数 保存本周期最后输出的运动矢量 必须在本周期最后电机输出处使用
    void G_ball_int_02(void);             // 清Front_aC
    int G_ball_02(float front_v, float turn_speed, float ball_angle, float ball_radius, float *m1, float *m2,
                  float *m3);
    int G_ball_03(float front_v, float turn_speed, float ball_angle, float ball_radius, float *m1, float *m2,
                  float *m3);
    void Angle_add(float turnW_, float *Front_aC_); // 一个周期转动量 造成的转动角度
    //	float G_ball_fv,G_ball_fva,G_ball_w;//带球 程序里球进入持球器的积分量
    float Dribble_Angle(float end_dbangle, float turn_w,
                        float front_v); // 生成带球 动作中 带球轨迹圆弧切线方向 和机体0度方向的夹角
                                        // end_dbangle为生成最大夹角 turn_w旋转角速度 front_v球前进速度
    float Angle_Speed(float turn_w);    // 换算从128制角速度 换算为 度/秒 单位的角速度
    float V_Speed(float V);             // 换算从128制速度 换算为 米/秒 单位的速度
    char Dribble_Angle_Track(
        float turn_w, float af,
        float *F_AC); // 带球角度跟踪  用Front_aC 跟踪 Dribble_Angle  并判断是否需要做 带球直线旋转动作 如果是返回1

    float G_turn_wMAX;        // 带球旋转角速度最大值
    float Gb_loop_time;       // 单位 毫秒   程序一个运行周期时间 用来估算旋转角度
    float ra_in_w_angle_P_ms; // 带球夹角超过 预定夹角 回收速度 单位  度/ms
    float Dribble_WP;
    float end_dbangle; //[经验值]front a c角度横90度不变//末状态前进量和0度的夹角	越接近90越稳准备时间越长

    float Front_aC; // 带球程序用变量存储带球方向 会累加(全局)
    float Front_C;
    double turn_fv; // 平移速度大小		临时存储 要达到要求的旋转中心位置 需要附加的平移速度
    float turn_afv; // 平移速度的方向    临时存储 要达到要求的旋转中心位置 需要附加的平移速度的方向

    void Dribble_ball(float *G_turn_w, float *motor1, float *motor2, float *motor3); // 可以生成带球  球速度 的带球函数
    char Dribble_ball_other_moment_flag; // 先运行了其他程序再运行带球函数标志
    float Gb_v_lest;                     // 剩下的前进量速度 占总速度的百分数
    float Gb_w_m;                        // 最小转动速度 以下不需要减速 占总角速度的百分数
    float Gb_Vavg;      // 24;//12.0 ;//直线带球的平均速度 大于时要减小(在持球器中) 小于时要增大(进入带球程序)
    float Gb_Vadd;      // 带球到平均速度的加速度 小于时要增大(进入带球程序)
    float Gb_Vred;      // 0.8;//0.4;//带球到平均速度的加速度 大于时要减小(在持球器中)
    float holdballFmin; // 12.7;//13.6;//13.9+0.2; //12.65;//299;//12.95;        //前景 球进入持球器底端最小值

    // 带球变量
    void G_ball_int_03(void);

    // 追球
    // 计算运动累加用的 速度差
    void Ball_Into_Holder(float ball_angle, float ball_Rradius);
    void Ball_Into_Holder2(float ball_angle, float ball_Rradius, float ball_Fangle, float ball_FRradius);
    float jifen; // 当球块进持球器时 让机体加速向前
    void Ball_Into_Holder3(float ball_angle, float ball_Rradius, float ball_Fangle, float ball_FRradius);
    void Ball_Into_Holder4(float ball_angle, float ball_Rradius, float ball_Fangle, float ball_FRradius);
    void Ball_Into_Holder4_(void);
    CMovement2Control move_test_C;
    void Ball_Track(float ball_angle, float ball_Rradius, float *motoB, float *motoL, float *motoR);
    void Ball_Track2(float *motoB, float *motoL, float *motoR);
    CMovement2Control move_test_D;
    float Ball_Track_x_diff(float x, float max);
    float Ball_Track_y_diff(float y, float max);
    float Ball_Track_a_diff(float a, float max);
    Difference dis_x; // 差分速度
    Difference dis_y;
    Difference dis_a;
    void Ball_Track3(float *motoB, float *motoL, float *motoR);
    Difference v_x; // 差分加速度
    Difference v_y;
    // 重要全局
    float robot_fv;    // 机器的上周期的运动参数 （累加）(全局)本机
    float robot_fva;   // 机器的上周期的运动参数 （累加）(全局)本机
    float robot_w;     // 机器的上周期的运动参数 （累加）(全局)本机
    float fount_xVset; // 机器的上周期的运动参数 （累加）(全局)对地
    float fount_yVset; // 机器的上周期的运动参数 （累加）(全局)对地
    float fv;          // 带球时 球的线速度(全局)
    float fa;
    // 显示用变量
    float view_add_x; // 追球每周期对地增加量大小
    float view_add_y;

    // 路程定位
    float Dis_Location; // 路程定位在 定位中占的影响 数值0~2 浮点数 0为完全不使用路程定位 2为完全使用路程定位  默认为0
                        // 比较合适的使用系数1.8
    void Distance_Location_init(void);
    void Distance_Location(float ball_angle, float ball_Rradius, float ball_weight, float f_x, float f_y, float f_a,
                           char enable);
    void Distance_Location_2(float ball_angle, float ball_Rradius, float ball_weight, float f_x, float f_y, float f_a,
                             char enable);
    void Movement_estimate_delay_compute3(
        float *angle, float *radius, float fvx, float fvy,
        float T); // 配套 Distance_Location_2 使用  angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
    obj_properties robot_L_test;         // 存储机器 估算位置_测试用
    obj_properties robot_L;              // 存储机器 估算位置
    obj_properties ball_d_h[20];         // 存储球 历史位置  [0]是当前周期的结果
    obj_properties obstacle_L_edgeH[20]; // 存障碍左边缘 历史位置  [0]是当前周期的结果
    obj_properties obstacle_R_edgeH[20]; // 存障碍右边缘 历史位置  [0]是当前周期的结果

    void Movement_estimate_delay_compute(float *angle, float *radius);      // 运动估算 用于消除延时
    void Movement_estimate_delay_compute2ball(float *angle, float *radius); // 运动估算 用于消除球的延时
                                                                            // 估算球有对地速度
    void Movement_estimate_delay_compute2(float *angle, float *radius, float fvx, float fvy,
                                          float T); // angle单位[度] radius 单位[米]  主要对对地面移动的物体有效
    void Movement(float T, float v, float v_a, float w, obj_properties *local);
    bool O_movement_T_analyze(float *move_v_angle, float *move_v_speed, float *turn_v, float motor1, float motor2,
                              float motor3); // 全向解析 修正输出负速度的情况
    bool O_movement_T_analyze_(float *move_v_angle, float *move_v_speed, float *turn_v, float motor1, float motor2,
                               float motor3); // 全向解析
    void motor_a_limit_init(void);
    void motor_a_limit(float mtb, float mtl, float mtr); // 使用加速度限制 预测 机体电机轴速  在周期末电机输出的位置使用
    void motor_history(float mtb, float mtl, float mtr); // 使用加速度限制 预测 机体电机轴速  在周期末电机输出的位置使用
    float mlR, mbR, mrR;                                 // 存储上个周期电机的真实转速
    float mhLR[20];                                      // 存储上20周期电机的真实转速  [0]是上个周期的结果
    float mhBR[20];
    float mhRR[20];
    float mhL[20]; // 存储上20周期电机的发送转速	[0]是上个周期的结果
    float mhB[20];
    float mhR[20];

    // 行为102规避使用 和行为 跑定点使用
    void move_and_target(float forward_direction_a, float forward_V, float target_a, float *motorB, float *motorL,
                         float *motorR); // 用于向指定方向平移(本机极坐标)并 对准指定目标(本机极坐标)

    //
    void obstacle_edge_save(float Langle, float LRradius, float Rangle,
                            float RRradius); // angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
    float revise_obstacle_R_edge(float guibiquyu);
    float revise_obstacle_L_edge(float guibiquyu);
    float view_R_edge_x; // 用于在全场定位上看边沿信息
    float view_R_edge_y; // 用于在全场定位上看边沿信息
    float view_L_edge_x;
    float view_L_edge_y;

    // 行为103拼抢使用
    void Grab_And_Breakage_Ball(float direct, float *motorB, float *motorL,
                                float *motorR); // 用于向指定方向平移(本机极坐标)并 对准指定目标(本机极坐标)

    // 射门判定用左右边缘角度校正
    float revise_gool_edge_A(float angle, float radius);
    float revise_gool_L_edge_A(void);
    float revise_gool_R_edge_A(void);
    float revise_goal_L_time_test(void); // 计算机器在当前运动状态下 多长时间可以射击左门柱
    float revise_goal_R_time_test(void); // 计算机器在当前运动状态下 多长时间可以射击右门柱

    // 存储上个周期的行为号
    int xingwei_P;
};

#endif Dribble_h
