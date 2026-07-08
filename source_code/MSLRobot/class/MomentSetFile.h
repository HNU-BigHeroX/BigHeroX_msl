// MomentSetFile.h
// 注意：当运动跟踪目标切换时 原来的积分量速度记录 等还在保存 会引起初运动不正常

#ifndef MomentSetFile_h
#define MomentSetFile_h

// 线速度[米/秒]
// 转速	[转/秒]	值2000=1[转/秒]
// PI=3.1415926
// r		[米]	轮子直径0.05[米]
const float wheel_linear_speed_ratio = 2000 * (1 / (2 * 3.1415926 * 0.05)); // 线速度=转速*2*PI*r	转速=线速度/(2*PI*r)
// 轮子到机体中心距离0.22米
// 旋转1圈/秒	2*PI*r=单轮子线速度
const float wheel_rotate_speed_ratio = (2 * 3.1415926 * 0.22) * wheel_linear_speed_ratio;
const float max_wheel_linear_speed =
    12.5 * 2000.0 / wheel_linear_speed_ratio; // (m/s)最大单轮线速度	注:对应128输入 线速度2m/s   128/2=motor1/out

#endif MomentSetFile_h
