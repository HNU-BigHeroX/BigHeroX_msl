// Math2.cpp

#include "stdafx.h"
#include "math.h"

// 直角坐标到极坐标转换函数 角度
double coordinate_transfer_angle(double x, double y)
{
    double angle;
    angle = 180 * atan2(-y, x) / 3.14; // 180*atan2(y-120,x-122)/3.14 ;
    return (angle);
}
// 直角坐标到极坐标转换函数 半径
double coordinate_transfer_radius(double x, double y)
{
    double radiu;
    radiu = sqrt(x * x + y * y); // sqrt((x-122)*(x-122)+(y-120)*(y-120)) ;
    return (radiu);
}

// 极坐标到直角坐标转换函数 角度
double coordinate_transfer_x(double angle, double radius)
{
    double x;
    x = cos(angle * 3.14 / 180) * radius;
    return (x);
}

// 极坐标到直角坐标转换函数 半径
double coordinate_transfer_y(double angle, double radius)
{
    double y;
    y = -sin(angle * 3.14 / 180) * radius;
    return (y);
}

// 坐标旋转n度 转换函数 极坐标
float angle_add_transfer(float angle, float n)
{ // 简单的思路 想的方法 一个角度+-360度绝对方向不变
    float aa;
    aa = angle + n;
    if (aa > 180)
        aa = aa - 360;
    if (aa < -180)
        aa = aa + 360;
    return (aa);
}

// 坐标旋转n度 转换函数 极坐标 可以处理-360<a a>360的情况 并将结果转换到 -180~+180之间
float angle_add_transfer2(float angle, float n)
{ // 简单的思路 想的方法 一个角度+-360度绝对方向不变
    float angle_in;
    angle_in = angle + n;

    // 将角度绝对值大于360度的角度 转换到-360~+360区间内
    int angle_temp = int(angle_in);
    angle_temp = (angle_temp / 360) * 360;
    angle_in = angle_in - angle_temp;
    //	//如果在-360~0区间 就转换到0~+360区间
    //	if (angle_in < 0    )
    //		angle_in +=360.0;

    // 如果在-540~-180 或 180~540区间 就转换到-180~+180区间
    if (angle_in > 180)
        angle_in = angle_in - 360;
    if (angle_in < -180)
        angle_in = angle_in + 360;
    return (angle_in);
}

float least_square_method(float *posX, float *posY, float *posW, int num)
{
    double Exy = 0.0, Ex = 0.0, Ey = 0.0, Ex2 = 0.0, totalnum = 0.0;
    double a, b; // ax+by+c=0
    if (num <= 1)
    {
        return (0);
    }
    for (int ct = 0; ct < num; ct++)
    {
        Exy = Exy + posX[ct] * posY[ct] * posW[ct];
        Ex = Ex + posX[ct] * posW[ct];
        Ey = Ey + posY[ct] * posW[ct];
        Ex2 = Ex2 + posX[ct] * posX[ct] * posW[ct];
        totalnum = totalnum + posW[ct];
    }
    a = totalnum * Exy - Ex * Ey;
    b = totalnum * Ex2 - Ex * Ex;

    //	c=-abc[0]*Ex/totalnum-abc[1]*Ey/totalnum;
    // a=(samnum*Exy-Ex*Ey)/(samnum*Ex2-Ex*Ex);
    // b=Ey/samnum-a*(Ex/samnum);
    return (-atan2(a, b) * 180.0 / 3.1415926);
}

double f2l_radius(double fx, double fy, double fa, double ftx, double fty)
{
    return (coordinate_transfer_radius(ftx - fx, fty - fy));
}
double f2l_angle(double fx, double fy, double fa, double ftx, double fty)
{
    return (angle_add_transfer(coordinate_transfer_angle(ftx - fx, fty - fy), -fa));
}
double l2f_x(double fx, double fy, double fa, double lta, double ltr)
{
    return (coordinate_transfer_x(lta + fa, ltr) + fx);
}
double l2f_y(double fx, double fy, double fa, double lta, double ltr)
{
    return (coordinate_transfer_y(lta + fa, ltr) + fy);
}

char delay_compute(float delay_time, float front_a, float front_v, float turn_v, obj_properties input,
                   obj_properties *output)
{
    double w, v, //
        X, Y, Rc, Ac, Xc, Yc, R1, A1, X1, Y1, R2, A2, X2, Y2, R3, A3, X3, Y3, T;
    w = 0;
    v = 0; //
    X = 0;
    Y = 0;
    Rc = 0;
    Ac = 0;
    Xc = 0;
    Yc = 0;
    R1 = 0;
    A1 = 0;
    X1 = 0;
    Y1 = 0;
    R2 = 0;
    A2 = 0;
    X2 = 0;
    Y2 = 0;
    R3 = 0;
    A3 = 0;
    X3 = 0;
    Y3 = 0;
    T = 0;

    T = delay_time; // 0.66;//0.12;	//延时 设置 单位s
    v = front_v;    // k;	//front_rate;
    w = turn_v;     // 45.001;	//turn_rate;

    // 对输入数据的有效性进行检查
    if ((abs(input.angle) <= 180) && (abs(input.radius) < 10000)) // 查输出超范围	先查极坐标系有无有效数据
    {
        X = coordinate_transfer_x(input.angle, input.radius); //(-90,20);	//物体的极坐标 到 直角坐标  X
        Y = coordinate_transfer_y(input.angle, input.radius); //(-90,20);	//							Y
    }
    else
    {
        if ((abs(input.x) < 10000) && (abs(input.y) < 10000)) // 查输出超范围   再查直角标系有无有效数据
        {
            X = input.x; // 物体的直角坐标  X
            Y = input.y; // 物体的直角坐标  Y
        }
        else // 如果两个坐标系输入都超范围 就 报错
        {
            return (0);
        }
    }

    // 对w=0这种特殊情况的处理
    if ((fabs(w) < 0.0001))
    {
        // 第一种情况计算 角速度为零时
        X3 = X - coordinate_transfer_x(front_a, v) * T;
        Y3 = Y - coordinate_transfer_y(front_a, v) * T;
        R3 = coordinate_transfer_radius(X3, Y3); // 目标的直角坐标 到  极坐标 R3
        A3 = coordinate_transfer_angle(X3, Y3);  //							A3
        // 第一种情况计算完成
    }
    else
    {
        // 第二种情况计算 角速度不为零时
        Rc = (v * 180) / (w * 3.14); // v/(w*3.14/????128????);			//机体旋转中心的极坐标	半径w*pi*r/180=v Rc
                                     // w*(2*pi*r/360) =v  w(度/s)  v,r(米) 或(像素)
        //?????????????????????
        Ac = front_a + 90;                  // front_angle+90;//Angle - 90;	//						角度	Ac
        Xc = coordinate_transfer_x(Ac, Rc); // 机体旋转中心的极坐标 到 直角坐标  Xc
        Yc = coordinate_transfer_y(Ac, Rc); //									Yc
        //?????????????????????

        X1 = X - Xc; // 平移过后的  目标的直角坐标
        Y1 = Y - Yc;
        R1 = coordinate_transfer_radius(X1, Y1); // 目标的直角坐标 到  极坐标 R1
        A1 = coordinate_transfer_angle(X1, Y1);  //							A1

        R2 = R1;                            // 旋转过后的  目标的直角坐标
        A2 = A1 - w * T;                    //
        X2 = coordinate_transfer_x(A2, R2); // 目标的极坐标 到 直角坐标  X2
        Y2 = coordinate_transfer_y(A2, R2); //							Y2

        X3 = X2 + Xc;
        Y3 = Y2 + Yc;
        R3 = coordinate_transfer_radius(X3, Y3); // 目标的直角坐标 到  极坐标 R3
        A3 = coordinate_transfer_angle(X3, Y3);  //							A3
        // 第二种情况计算完成
    }
    // 延时估算完成
    output->angle = A3;  //(int)
    output->radius = R3; //(int)
    output->x = X3;      //(int)
    output->y = Y3;      //(int)
    return (1);
}

// 限制加速度 的跟踪函数
void track(float *currently, float target, float acceleration_limit)
{
    if (fabs(*currently - target) > acceleration_limit)
        if ((*currently - target) > 0)
            *currently = *currently - acceleration_limit;
        else
            *currently = *currently + acceleration_limit;
    else
        *currently = target;
}

// 限制加速度 的角度跟踪函数
void track_angle(float *currently, float target, float acceleration_limit)
{
    *currently = angle_add_transfer(*currently, 0);
    target = angle_add_transfer(target, 0);
    if (fabs(*currently - target) < 180)
    {
        if (fabs(*currently - target) > acceleration_limit)
            if ((*currently - target) > 0)
                *currently = *currently - acceleration_limit;
            else
                *currently = *currently + acceleration_limit;
        else
            *currently = target;
    }
    else
    {
        if (fabs(angle_add_transfer(*currently, -target)) > acceleration_limit)
            if (angle_add_transfer(*currently, -target) > 0)
                *currently = angle_add_transfer(*currently, -acceleration_limit);
            else
                *currently = angle_add_transfer(*currently, +acceleration_limit);
        else
            *currently = target;
    }
}

// 简易三角卷积滤波器
float simple_filter(float a1, float a2, float a3, float a4, float a5)
{
    return ((a1 * 1 + a2 * 0.5 + a3 * 0.25 + a4 * 0.125 + a5 * 0.125) / 2.0);
}

// 线性函数  在==top时为1 ==bottom时为0
float lf(float top, float bottom, float in) // linearity function
{
    float xs = (in - bottom) / (top - bottom);
    if (xs < 0)
        xs = 0;
    if (xs > 1)
        xs = 1;
    return (xs);
}