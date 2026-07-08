// 本地 仿真
// LOCALEMU.h

// #include "stdafx.h"
#ifndef LOCALEMU_h
#define LOCALEMU_h
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

#define ALL_Robot_rand_p 10.0

class Emu_Robot
{
  public:
    obj_properties robot_emu;
    obj_properties robot;
    obj_properties ball;
    obj_properties ddoor;
    obj_properties ddoorL;
    obj_properties ddoorR;
    obj_properties jdoor;
    obj_properties jdoorL;
    obj_properties jdoorR;

    int robot_ID;   // 代表机器号码
    int robot_SIDE; // 代表机器所属队伍 1左往右攻 0右往左攻
    Emu_Robot();
    ~Emu_Robot();

    void update_ball_door(obj_properties ball_in);
    void update_robot(float lm, float bm, float rm);
    void update_robot_location(obj_properties robot_in);
    void obj_properties_copy_globe(
        obj_properties data_in,
        obj_properties &data_to); // 拷贝data_in=>data_to obj_properties 中的全局坐标部分 拷贝地面坐标 不拷贝本机极坐标

    double front_v_angle_p; // 注意这里现在只能仿真一个虚拟机器人 只有一个历史记录
    double front_v_p;
    double turn_w_p;
    void SetMovement(float T, float front_v_c, float front_v_angle_c, float turn_w);
    void Movement(float T, float v, float v_a, float w);

    void BallMovement2(float a, float T, float vx, float vy, obj_properties *obj);
    void hit3_r(float r, float CR, obj_properties *objA, obj_properties *objB);
    void hit_film_line(float CR, obj_properties *obj);
};

#endif