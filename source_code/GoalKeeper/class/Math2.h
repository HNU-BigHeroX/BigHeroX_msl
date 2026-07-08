// Math2.h
// 注意：当运动跟踪目标切换时 原来的积分量速度记录 等还在保存 会引起初运动不正常

#ifndef Math2_h
#define Math2_h

#include "class\ObjPropertiesStruct.h" //cs

// 直角坐标到极坐标转换函数 角度
extern double coordinate_transfer_angle(double x, double y);
// 直角坐标到极坐标转换函数 半径
extern double coordinate_transfer_radius(double x, double y);
// 极坐标到直角坐标转换函数 角度
extern double coordinate_transfer_x(double angle, double radius);
// 极坐标到直角坐标转换函数 半径
extern double coordinate_transfer_y(double angle, double radius);
// 坐标旋转n度 转换函数 极坐标
extern float angle_add_transfer(float angle, float n);
// 坐标旋转n度 转换函数 极坐标 >+-360
extern float angle_add_transfer2(float angle, float n);
extern float least_square_method(float *posX, float *posY, float *posW, int num);

extern double f2l_radius(double fx, double fy, double fa, double ftx, double fty);
extern double f2l_angle(double fx, double fy, double fa, double ftx, double fty);
extern double l2f_x(double fx, double fy, double fa, double lta, double ltr);
extern double l2f_y(double fx, double fy, double fa, double lta, double ltr);

extern char delay_compute(float delay_time, float front_a, float front_v, float turn_v, obj_properties input,
                          obj_properties *output);

extern void track(float *currently, float target, float acceleration_limit);
extern void track_angle(float *currently, float target, float acceleration_limit);

extern float simple_filter(float a1, float a2, float a3, float a4, float a5);
// 线性函数  在==top时为1 ==bottom时为0
extern float lf(float top, float bottom, float in); // linearity function

#endif Math2_h
