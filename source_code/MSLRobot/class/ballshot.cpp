// 球起飞判定
// ballshot.cpp

#include "stdafx.h"

#ifndef ballshot_cpp
#define ballshot_cpp

#include "ballshot.h"

BallShot::BallShot(void)
{
    int i, j;
    for (i = 0; i < 100; i++)
    {
        quanball_h[i][0] = 0;
        quanball_h[i][1] = 0;
        quanball_h[i][2] = 0;
    }
    Shot_ball_point_X = 0; // 球起飞全场定位点x
    Shot_ball_point_Y = 0; // 球起飞全场定位点y
    min_dis_ball_time = 0; // 此点持续为落地点的周期
    min_dis_ball_x = 0;    // 存储从起飞判定成立开始距机体最近的球的全场坐标 球没起飞时为球的当前场地坐标
    min_dis_ball_y = 0;
    Shot_ball = 0; // 球起飞判定
}
BallShot::~BallShot(void)
{
    ;
}
char BallShot::Ball_Shot_Determine(float benji_x, float benji_y, float ball_x, float ball_y, float ball_w)
{
    int i;
    // 存储上100周期球的全场位置信息
    for (i = 100 - 1; i > 0; i--)
    {
        quanball_h[i][0] = quanball_h[i - 1][0]; //[0]y,[1]x,[2]w //全场球信息历史记录
        quanball_h[i][1] = quanball_h[i - 1][1];
        quanball_h[i][2] = quanball_h[i - 1][2];
    }
    quanball_h[0][0] = ball_y;
    quanball_h[0][1] = ball_x;
    quanball_h[0][2] = ball_w;
    // 计算球在T时间内行进路程 0为当前帧 1为上一帧 2上上帧 ......
    float ball_distance = 0; // 球在视觉中 走过的场地上路程 单位(厘米)
    float startx, starty, startT, endT, ball_velocity;
    const int testT = 20;   // 设置检测是否飞行 的帧数
    const int seeBall = 10; // 判定看到球的最小像素点门限
    startT = -1;            // 设置默认无效数据
    endT = -1;
    for (i = 0; i < testT; i++)
    {
        if (quanball_h[i][2] > seeBall) // 找到开始位置
        {
            starty = quanball_h[i][0];
            startx = quanball_h[i][1];
            startT = i;
            break;
        }
    }
    for (i++; i < testT; i++)
    {
        if (quanball_h[i][2] > seeBall)
        {
            ball_distance += sqrt((starty - quanball_h[i][0]) * (starty - quanball_h[i][0]) +
                                  (startx - quanball_h[i][1]) * (startx - quanball_h[i][1])) *
                             field_zoom_time;
            starty = quanball_h[i][0];
            startx = quanball_h[i][1];
            endT = i;
        }
    }
    // 判断 起止点 有效性至少5帧以上有效数据才算 数据有效
    if ((startT >= 0) && (endT > (startT + 5))) // 如果数据一直没更新就 保持原来判定不变
    {
        ball_velocity = ball_distance * 45.0 / (endT - startT); // 45.0每秒帧数 计算 球视觉中走过的场地上路程的平均速度
        if (ball_velocity > 1000)                               // 10m/s 10 00
        {
            if (Shot_ball == 0)
            {
                Shot_ball_point_Y = quanball_h[int(endT)][0]; // 球起飞全场定位点y
                Shot_ball_point_X = quanball_h[int(endT)][1]; // 球起飞全场定位点x
                min_dis_ball_y =
                    Shot_ball_point_Y; // 存储从起飞判定成立开始距机体最近的球的全场坐标 球没起飞时为球的当前场地坐标
                min_dis_ball_x = Shot_ball_point_X;
            }
            Shot_ball += 21; // 球起飞判定 在21帧以内有1帧球速判定超过(ball_velocity>1000)==10m/s
            if (Shot_ball > 21)
                Shot_ball = 21;
        }
        else
        {
            Shot_ball--; // 球起飞判定
            if (Shot_ball < 0)
                Shot_ball = 0;
            if (Shot_ball == 0)
            {
                Shot_ball_point_X = 0; // 球起飞全场定位点x
                Shot_ball_point_Y = 0; // 球起飞全场定位点y
            }
        }
    }
    if (Shot_ball > 21)
        Shot_ball = 21;
    if (Shot_ball < 0)
        Shot_ball = 0;

    char returnvalue = 0;
    if (Shot_ball > 0)
    {
        // 存储从起飞判定成立开始距机体最近的球的全场坐标 球没起飞时为球的当前场地坐标
        if (sqrt((min_dis_ball_y - benji_y) * (min_dis_ball_y - benji_y) +
                 (min_dis_ball_x - benji_x) *
                     (min_dis_ball_x -
                      benji_x)) // coordinate_transfer_radius(min_dis_ball_y-benji_y,min_dis_ball_x-benji_x)
            > sqrt((ball_y - benji_y) * (ball_y - benji_y) +
                   (ball_x - benji_x) *
                       (ball_x - benji_x)) //>coordinate_transfer_radius(ball_y       -benji_y,ball_x        -benji_x)
        )
        {
            min_dis_ball_y = ball_y;
            min_dis_ball_x = ball_x;
            returnvalue = 2; // 此点是中间落地点
            min_dis_ball_time = 0;
        }
        else
        {
            min_dis_ball_time++;
        }
        returnvalue = 1; // 此点是空中飞行中点
    }
    else
    {
        min_dis_ball_x = ball_x; // 存储从起飞判定成立开始距机体最近的球的全场坐标 球没起飞时为球的当前场地坐标
        min_dis_ball_y = ball_y;
        returnvalue = 0; // 球没有起飞
    }

    if (Shot_ball > 0)
    {
        if ((min_dis_ball_y == Shot_ball_point_Y) && (min_dis_ball_x == Shot_ball_point_X))
            min_dis_ball_time = 0;
    }
    else
    {
        min_dis_ball_time = 0;
        step_ball_ok = 0;
    }
    if (min_dis_ball_time > 5)
    {
        step_ball_ok = 1;             //
        step_ball_x = min_dis_ball_x; // 存储从起飞判定成立开始距机体最近的球的全场坐标 球没起飞时为球的当前场地坐标
        step_ball_y = min_dis_ball_y;
    }

    return (returnvalue);
}

#endif