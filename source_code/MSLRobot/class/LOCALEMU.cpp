// 本地 仿真
// LOCALEMU.cpp

#include "stdafx.h"

#ifndef LOCALEMU_cpp
#define LOCALEMU_cpp

#include "LOCALEMU.h"

void Emu_Robot::update_robot(float lm, float bm, float rm)
{
    //	obj_properties_copy_globe(robot_in,robot_emu);
}
void Emu_Robot::update_robot_location(obj_properties robot_in)
{
    float temp_robot_f_x = robot_in.f_x;
    float temp_robot_f_y = robot_in.f_y;

    robot.f_x = temp_robot_f_x + (rand() / 16383.5 - 1.0) * ALL_Robot_rand_p; // 加噪部分
    robot.f_y = temp_robot_f_y + (rand() / 16383.5 - 1.0) * ALL_Robot_rand_p;
    robot.f_a = robot_in.f_a + (rand() / 16383.5 - 1.0) * ALL_Robot_rand_p * 0.01;
}

// 更新机器人看到的球的位置
void Emu_Robot::update_ball_door(obj_properties ball_in)
{
    float temp_ball_in_f_x = ball_in.f_x;
    float temp_ball_in_f_y = ball_in.f_y;

    ball.angle = f2l_angle(robot.f_x, robot.f_y, robot.f_a, temp_ball_in_f_x, temp_ball_in_f_y) +
                 (rand() / 16383.5 - 1.0) * 0.1; // 球对本机的位置 在本机极坐标系里
    ball.radius = f2l_radius(robot.f_x, robot.f_y, robot.f_a, temp_ball_in_f_x, temp_ball_in_f_y);
    float rand_p = ball.radius * 0.002 * ball.radius * 0.002 + 0.3;
    ball.radius = ball.radius + rand_p * (rand() / 16383.5 - 1.0) * ALL_Robot_rand_p;

    ball.f_x = l2f_x(robot.f_x, robot.f_y, robot.f_a, ball.angle, ball.radius); // 估算后 球对地面的位置 在地面坐标系里
    ball.f_y = l2f_y(robot.f_x, robot.f_y, robot.f_a, ball.angle, ball.radius);

    // 给出 本机极坐标 中球的位置
    //	ball.angle	= f2l_angle (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a, ballC_d.f_x,ballC_d.f_y);//球对本机的位置
    // 在本机极坐标系里 	ball.radius	= f2l_radius(robotA_d.f_x,robotA_d.f_y,robotA_d.f_a, ballC_d.f_x,ballC_d.f_y);
    ball.weight = 100;

    // robot_SIDE; //代表机器所属队伍 1左往右攻 0右往左攻

    if (robot_SIDE == 1)
    {
        jdoor.f_x = 0;
        jdoor.f_y = 600;
        ddoor.f_x = 1800;
        ddoor.f_y = 600;
    }
    else
    {
        jdoor.f_x = 1800;
        jdoor.f_y = 600;
        ddoor.f_x = 0;
        ddoor.f_y = 600;
    }

    jdoor.angle = f2l_angle(robot.f_x, robot.f_y, robot.f_a, jdoor.f_x, jdoor.f_y);
    jdoor.radius = f2l_radius(robot.f_x, robot.f_y, robot.f_a, jdoor.f_x, jdoor.f_y);
    jdoor.weight = 1000;
    ddoor.angle = f2l_angle(robot.f_x, robot.f_y, robot.f_a, ddoor.f_x, ddoor.f_y);
    ddoor.radius = f2l_radius(robot.f_x, robot.f_y, robot.f_a, ddoor.f_x, ddoor.f_y);
    ddoor.weight = 1000;

    // cs畸变校正过的变量命名
    //	ball_Rradius = f2l_radius(robotA_d.f_x,robotA_d.f_y,robotA_d.f_a,
    // ballC_d.f_x,ballC_d.f_y)*100.0;//源坐标系是m（米）制还要换成厘米制*100.0
}

void Emu_Robot::obj_properties_copy_globe(
    obj_properties data_in,
    obj_properties &data_to) // 拷贝data_in=>data_to obj_properties 中的全局坐标部分 拷贝地面坐标 不拷贝本机极坐标
{

    // 物体面积
    data_to.weight = data_in.weight; // 图象重量

    // 物体物理质量
    data_in.mass = data_in.mass;

    // 对地坐标系
    data_to.f_x = data_in.f_x;   // field 场地坐标x
    data_to.f_y = data_in.f_y;   // field 场地坐标y
    data_to.f_a = data_in.f_a;   // field 场地坐标a
    data_to.f_vx = data_in.f_vx; // field 场地坐标x 方向上的速度
    data_to.f_vy = data_in.f_vy; // field 场地坐标y 方向上的速度
    data_to.f_w = data_in.f_w;   // field 场地坐标旋转量
}

Emu_Robot::Emu_Robot()
{
    front_v_angle_p = 0; // 注意这里现在只能仿真一个虚拟机器人 只有一个历史记录
    front_v_p = 0;
    turn_w_p = 0;
}
Emu_Robot::~Emu_Robot()
{
}

// 仿真机体加速特性包括线加速度 角加速度  T一个周期时间  front_v_c机体目标平移速度 front_v_angle_c机体目标速度角度
// turn_w机体目标转动速度 obj机体物理状态存储结构体
void Emu_Robot::SetMovement(float T, float front_v_c, float front_v_angle_c, float turn_w)
{
    front_v_c = front_v_c * 100;               // 米制 转 厘米制
    float acceleration_limit_dis = 380.0 * T;  // 1.8m/ss
    float acceleration_limit_turn = 200.0 * T; // 20度/ss

    // double front_v_angle_p = coordinate_transfer_angle (obj->f_vx,obj->f_vy);
    //        front_v_angle_p = angle_add_transfer        (front_v_angle_p,-obj->f_a);
    // double front_v_p       = coordinate_transfer_radius(obj->f_vx,obj->f_vy);

    // 限定平移[功率]加速度最大值 CMovement2Control::front_a_maxlimit(void)
    double front_vx_c, front_vy_c;
    double front_vx_p, front_vy_p;
    double front_vx_d, front_vy_d;
    double front_va_d, front_vr_d;
    front_vx_c = coordinate_transfer_x(front_v_angle_c, front_v_c);
    front_vy_c = coordinate_transfer_y(front_v_angle_c, front_v_c);
    front_vx_p = coordinate_transfer_x(front_v_angle_p, front_v_p);
    front_vy_p = coordinate_transfer_y(front_v_angle_p, front_v_p);
    front_vx_d = front_vx_c - front_vx_p;
    front_vy_d = front_vy_c - front_vy_p;
    front_va_d = coordinate_transfer_angle(front_vx_d, front_vy_d);
    front_vr_d = coordinate_transfer_radius(front_vx_d, front_vy_d);
    /*	if (front_v_c > (front_v_p+acceleration_limit_dis))	//acceleration_limit_dis;//加速度最大值控制
        {
            front_v_c =  front_v_p+acceleration_limit_dis;	//acceleration_limit_dis;//加速度最大值控制
        }
        if (front_v_c < (front_v_p-acceleration_limit_dis))	//acceleration_limit_dis;//加速度最大值控制
        {
            front_v_c =  front_v_p-acceleration_limit_dis;	//acceleration_limit_dis;//加速度最大值控制
        }
    */
    if (front_vr_d > acceleration_limit_dis) // front_vr_d是向量相减所得向量的 模    要求小于加速度限定最大值
    {
        front_vr_d = acceleration_limit_dis; // 加速度最大值控制
    }
    front_vx_c = front_vx_p + coordinate_transfer_x(front_va_d, front_vr_d);
    front_vy_c = front_vy_p + coordinate_transfer_y(front_va_d, front_vr_d);
    front_v_angle_c = coordinate_transfer_angle(front_vx_c, front_vy_c);
    front_v_c = coordinate_transfer_radius(front_vx_c, front_vy_c);
    //	注意：因为在后面运动合成中还会变动front_v_c所以改在最后输出movement_output时存储front_v_p

    // 2009032改错误
    // if (((turn_w - obj->f_w)<=acceleration_limit_turn)&&((-turn_w+ obj->f_w)<=acceleration_limit_turn))
    // obj->f_w=turn_w;
    // 20090306改错误

    float turn_w_c = turn_w;

    if (turn_w_c > turn_w_p + acceleration_limit_turn)
        turn_w_c = turn_w_p + acceleration_limit_turn;

    if (turn_w_c < turn_w_p - acceleration_limit_turn)
        turn_w_c = turn_w_p - acceleration_limit_turn;

    robot_emu.f_vx = coordinate_transfer_x(robot_emu.f_a + front_v_angle_c, front_v_c) * 0.01;
    robot_emu.f_vy = coordinate_transfer_y(robot_emu.f_a + front_v_angle_c, front_v_c) * 0.01;

    front_v_angle_p = front_v_angle_c; // 注意这里现在只能仿真一个虚拟机器人 只有一个历史记录
    front_v_p = front_v_c;
    turn_w_p = turn_w_c;

    Movement(T, front_v_c, front_v_angle_c, turn_w);
}

// T一个周期时间  v机体 平移速度 v_a机体速度角度 w机体转动速度 local机体物理状态存储结构体
void Emu_Robot::Movement(float T, float v, float v_a, float w)
{
    // 开始 通过电机输出值运动位置累加  //以后每一帧做累加
    if (fabs(w) > 0.0001)
    {
        float Rc = (v * 180) / (w * 3.14); // v/(w*3.14/????128????);			//机体旋转中心的极坐标	半径w*pi*r/180=v
                                           // Rc w*(2*pi*r/360) =v  w(度/s)  v,r(米) 或(像素)
        double Rx, Ry;
        if (w >
            0) // 算出旋转中心 按照旋转中心,起点(local.f_x,local.f_y)经过的弧度T*w 找出弧形终点存入(local.f_x,local.f_y)
        {
            Rx = robot_emu.f_x + coordinate_transfer_x(robot_emu.f_a + 90 + v_a, Rc); // 算出旋转中心 地面坐标
            Ry = robot_emu.f_y + coordinate_transfer_y(robot_emu.f_a + 90 + v_a, Rc);
            robot_emu.f_x = Rx - coordinate_transfer_x(robot_emu.f_a + 90 + v_a + T * w, Rc); // 本机 新 场地x
            robot_emu.f_y = Ry - coordinate_transfer_y(robot_emu.f_a + 90 + v_a + T * w, Rc); // 本机 新 场地y
        }
        else
        {
            Rx = robot_emu.f_x + coordinate_transfer_x(robot_emu.f_a - 90 + v_a, Rc); // 算出旋转中心 地面坐标
            Ry = robot_emu.f_y + coordinate_transfer_y(robot_emu.f_a - 90 + v_a, Rc);
            robot_emu.f_x = Rx - coordinate_transfer_x(robot_emu.f_a - 90 + v_a - T * w, Rc); // 本机 新 场地x
            robot_emu.f_y = Ry - coordinate_transfer_y(robot_emu.f_a - 90 + v_a - T * w, Rc); // 本机 新 场地y
        }
        robot_emu.f_a = angle_add_transfer(T * w, robot_emu.f_a); // 本机场地angle = 时间*角速度 + 上周期本机场地angle
    }
    else
    {
        robot_emu.f_x += coordinate_transfer_x(robot_emu.f_a + v_a, v) * T; // 本机 新 场地x
        robot_emu.f_y += coordinate_transfer_y(robot_emu.f_a + v_a, v) * T; // 本机 新 场地y
    }
    // 位置估测完毕
}

void Emu_Robot::BallMovement2(float a, float T, float vx, float vy, obj_properties *obj)
{
    // 假设 物体阻力为f=m*u*g a=f/m=u*g dv=a*T=T*u*g
    //                dv/v = dxv/xv = dyv/yv
    //	float dv=T*u*9.8;
    float dv = T * a;
    float v = coordinate_transfer_radius(vx, vy);
    if (v > 0)
    {
        if (fabs(obj->f_vx) > fabs(dv * obj->f_vx / v))
        {
            obj->f_vx = obj->f_vx - dv * obj->f_vx / v;
        }
        else
        {
            obj->f_vx = 0;
        }
        if (fabs(obj->f_vy) > fabs(dv * obj->f_vy / v))
        {
            obj->f_vy = obj->f_vy - dv * obj->f_vy / v;
        }
        else
        {
            obj->f_vy = 0;
        }
    }
    obj->f_x += obj->f_vx * T;
    obj->f_y += obj->f_vy * T;
}

// CR恢复系数取值1~0   1为完全弹性碰撞 0为完全非弹性碰撞
void Emu_Robot::hit3_r(float r, float CR, obj_properties *objA, obj_properties *objB)
{
    if (objA == objB)
        return;

    if (coordinate_transfer_radius((objA->f_x - objB->f_x), (objA->f_y - objB->f_y)) < r)
    {
        double vAa = coordinate_transfer_angle(objA->f_vx, objA->f_vy);
        double vBa = coordinate_transfer_angle(objB->f_vx, objB->f_vy);
        double vAm = coordinate_transfer_radius(objA->f_vx, objA->f_vy);
        double vBm = coordinate_transfer_radius(objB->f_vx, objB->f_vy);
        double ha = coordinate_transfer_angle((objB->f_x - objA->f_x),
                                              (objB->f_y - objA->f_y)); // 存储碰撞方向 (objA 指向 objB)
        double vAha = vAm * cos((vAa - ha) * 3.14 / 180.0);             // 存储碰撞方向上的速度分量1 和2
        double vBha = vBm * cos((vBa - ha) * 3.14 / 180.0);
        double vAtha = vAm * sin((vAa - ha) * 3.14 / 180.0); // 存储碰撞垂直方向上的速度分量1 和2
        double vBtha = vBm * sin((vBa - ha) * 3.14 / 180.0);

        // 拉开距离
        double open_dis =
            r - coordinate_transfer_radius((objA->f_x - objB->f_x), (objA->f_y - objB->f_y)); // 要拉开的距离
        //		objA->f_x += coordinate_transfer_x(ha,-open_dis*fabs(vAha)/(fabs(vAha)+fabs(vBha)));
        //		objA->f_y += coordinate_transfer_y(ha,-open_dis*fabs(vAha)/(fabs(vAha)+fabs(vBha)));
        //		objB->f_x += coordinate_transfer_x(ha,open_dis*fabs(vBha)/(fabs(vAha)+fabs(vBha)));
        //		objB->f_y += coordinate_transfer_y(ha,open_dis*fabs(vBha)/(fabs(vAha)+fabs(vBha)));

        objA->f_x += coordinate_transfer_x(ha, -(open_dis + 0.1));
        objA->f_y += coordinate_transfer_y(ha, -(open_dis + 0.1));
        objB->f_x += coordinate_transfer_x(ha, (open_dis + 0.1));
        objB->f_y += coordinate_transfer_y(ha, (open_dis + 0.1));

        if (vAha > vBha) // 碰撞检测
        {

            // 碰撞速度计算
            double vxA =
                (vAha * (objA->mass - CR * objB->mass) + (CR + 1) * objB->mass * vBha) / (objA->mass + objB->mass);
            double vxB =
                (vBha * (objB->mass - CR * objA->mass) + (CR + 1) * objA->mass * vAha) / (objA->mass + objB->mass);
            vAha = vxA;
            vBha = vxB;
            double tAx = vAha * cos(ha * 3.14 / 180.0) + -vAtha * sin(ha * 3.14 / 180.0);
            double tBx = vBha * cos(ha * 3.14 / 180.0) + -vBtha * sin(ha * 3.14 / 180.0);
            double tAy = -vAha * sin(ha * 3.14 / 180.0) + -vAtha * cos(ha * 3.14 / 180.0);
            double tBy = -vBha * sin(ha * 3.14 / 180.0) + -vBtha * cos(ha * 3.14 / 180.0);
            objA->f_vx = tAx;
            objB->f_vx = tBx;
            objA->f_vy = tAy;
            objB->f_vy = tBy;
        }
    }
}

void Emu_Robot::hit_film_line(float CR, obj_properties *obj)
{
    if (obj->f_x < 0) // 碰左底线
    {
        obj->f_vx = fabs(obj->f_vx);
    }
    if (obj->f_x > 18) // 碰右底线
    {
        obj->f_vx = -fabs(obj->f_vx);
    }
    if (obj->f_y < 0) // 碰左底线
    {
        obj->f_vy = fabs(obj->f_vy);
    }
    if (obj->f_y > 12) // 碰右底线
    {
        obj->f_vy = -fabs(obj->f_vy);
    }
}
#endif