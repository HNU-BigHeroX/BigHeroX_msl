// 球起飞判定
// ballshot.h

// #include "stdafx.h"
#ifndef ballshot_h
#define ballshot_h
#include <math.h>
// #include "Math2.h"
#include <iostream.h>

class BallShot
{

  protected:
  public:
    float quanball_h[100][3];
    float Shot_ball_point_X; // 球起飞全场定位点x
    float Shot_ball_point_Y; // 球起飞全场定位点y
    int min_dis_ball_time;   // 此点持续为落地点的周期
    float min_dis_ball_x;    // 存储从起飞判定成立开始距机体最近的球的全场坐标 球没起飞时为球的当前场地坐标
    float min_dis_ball_y;
    int step_ball_ok;  // 存储从起飞判定成立开始距机体最近的球的全场坐标 球没起飞时为球的当前场地坐标
    float step_ball_x; //
    float step_ball_y;

    int Shot_ball; // 球起飞判定
    BallShot(void);
    ~BallShot(void);

    char Ball_Shot_Determine(float benji_x, float benji_y, float ball_x, float ball_y, float ball_w);
    // 返回值
    // return(2);//此点是中间落地点
    // return(1);//此点是空中飞行中点
    // return(0);//球没有起飞
};
#endif
