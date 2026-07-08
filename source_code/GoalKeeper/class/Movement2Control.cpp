// MovementControl.cpp

#include "stdafx.h"
#include "Movement2Control.h"
#include "math.h"

CMovement2Control::CMovement2Control()
{
    int i;
    for (i = 0; i < 20; i++) // 清空目标角度历史记录
    {
        save_target_history(0, 0, 0); // 更新历史记录函数  要求调用程序每周期更新
        translate_radius_h[i] = 0;    // 目标距离本机定点 矢量差分 距离的历史记录
        translate_angle_h[i] = 0;     // 目标距离本机定点 矢量差分 角度的历史记录
        // t_a_h[i]=0
        // t_d_h[i]=0;//目标距离历史记录
        // translate_radius_h[i]=0;//目标距离本机定点距离的历史记录
    }
    p_quotiety = 0;         // 比例项系数proportion
    i_quotiety = 0;         // 积分项系数integral
    integral = 0;           // 积分项
    d_quotiety = 0;         // 微分项系数differential coefficient
    diff_inte = 0;          // 微分项积分量 这里的微分是把微分项累加出来的
    dead_zone = 0;          // 电机功率死区
    acceleration_limit = 0; // 角加速度最大值控制
    velocity_limit = 0;     // 角速度最大值控制
    //	float velocity;				//角计算速度
    p_result = 0;
    i_result = 0;
    d_result = 0;
    turn_v_c = 0; // 当前次角度 跟踪 输出量大小
    turn_v_p = 0; // 前一次角度 跟踪 输出量大小

    p_quotiety_dis = 0;         // 比例项系数proportion
    i_quotiety_dis = 0;         // 积分项系数integral
    integral_dis = 0;           // 积分项
    d_quotiety_dis = 0;         // 微分项系数differential coefficient
    diff_inte_dis = 0;          // 微分项积分量 这里的微分是把微分项累加出来的
    dead_zone_dis = 0;          // 电机功率死区
    acceleration_limit_dis = 0; // 加速度最大值控制
    velocity_limit_dis = 0;     // 速度最大值控制
    //	float velocity_dis;//计算速度
    p_result_dis = 0;
    i_result_dis = 0;
    d_result_dis = 0;
    front_v_c = 0;       // 当前次平移 跟踪 输出量大小
    front_v_p = 0;       // 前一次平移 跟踪 输出量大小
    front_v_angle_c = 0; // 当前次平移角度      输出量方向
    front_v_angle_p = 0; // 前一次平移角度      输出量方向

    mtl_p = 0;
    mtr_p = 0;
    mtb_p = 0;
}

void CMovement2Control::save_target_history(float angle, float radius,
                                            float weight) // 更新历史记录函数  要求调用程序每周期更新
{
    int i;
    for (i = 19; i > 0; i--) // 目标位置的历史纪录 用于求微分 积分用
    {
        target_h[i].angle = target_h[i - 1].angle;
    }
    target_h[0].angle = angle;

    for (i = 19; i > 0; i--) // 目标位置的历史纪录 用于求微分 积分用
    {
        target_h[i].radius = target_h[i - 1].radius;
    }
    target_h[0].radius = radius;

    for (i = 19; i > 0; i--) // 目标位置的历史纪录 用于求微分 积分用
    {
        target_h[i].weight = target_h[i - 1].weight;
    }
    target_h[0].weight = weight;
    // 说明	obj_properties traged_h[20];//跟踪目标的历史记录  要求调用程序每周期更新
}

float CMovement2Control::zs_angle_track_001(float target_angle, float benchmark, char *LorRout)
{
    float t_a_h_[20]; // 目标角度历史记录
    for (int i = 0; i < 20; i++)
    {
        t_a_h_[i] = target_h[i].angle;
    }

    return (zs_angle_track_001(target_angle, benchmark, t_a_h_, LorRout));
}

// 配套函数:
// 参数设置						parameter_set_angle(float p_quotiety_,float i_quotiety_,float d_quotiety_,float
// dead_zone_) 参数和内部积分量初始化		initializing_angle(void) 极限输出限定参数设置
// limit_parameter_set_angle(float velocity_limit_,float acceleration_limit_) 限定转动[功率]速度输出最大值
// turn_v_maxlimit(void) 限定转动[功率]加速度最大值	turn_a_maxlimit(void)
// 函数使用说明:
// float target_angle	跟踪目标的 本机坐标 角度
// float benchmark		跟踪目标的 本机基准
// int *DD				跟踪目标的 本机坐标 角度 历史记录
// char *LorRout			函数决策	结果的文本形式
// 返回值为				计算结果角速度[准确说是修正量 无单位]
float CMovement2Control::zs_angle_track_001(float target_angle, float benchmark, float *DD, char *LorRout)
{
    // 类内部 保护成员变量
    //	float p_quotiety;//比例项系数proportion
    //	float i_quotiety;//积分项系数integral
    //	float integral	;//积分项
    //	float d_quotiety;//微分项系数differential coefficient
    //	float dead_zone	;//电机功率死区
    float angular_velocity; // 返回量计算结果角速度

    // 增量调节正方向如果基准0 物体在基准右 往右转target_angle - benchmark >0  即正数增量往右转
    int i;
    float target_angle_t, it, yt, zt; //,angle_increment,xt,
    float DDiff[20];
    float DDiff_t;
    // 差分部分正方向目标向左[逆时针]DDiff_t为正
    DDiff[0] = angle_add_transfer(target_angle, (float)(-DD[0]));
    for (i = 1; i < 20; i++) // 电机功率差的历史纪录 用于求微分 积分用
    {
        DDiff[i] = angle_add_transfer((float)(DD[i]), (float)(-DD[i - 1]));
    }
    DDiff_t = 10 * DDiff[0] + 8 * DDiff[1] + 6 * DDiff[2] + 5 * DDiff[3] + 4 * DDiff[4] + 3 * DDiff[5] + 2 * DDiff[6] +
              2 * DDiff[7] + DDiff[8] + DDiff[9] +
              DDiff[10]; //+DDiff[11]+DDiff[12]+DDiff[13]+DDiff[14]+DDiff[15]+DDiff[16]+DDiff[17]+DDiff[18]+DDiff[19];
    // DDiff_t=10*DDiff[0]+9*DDiff[1]+8*DDiff[2]+7*DDiff[3]+6*DDiff[4]+5*DDiff[5]+4*DDiff[6]+3*DDiff[7]+2*DDiff[8]+DDiff[9]+DDiff[10]+DDiff[11]+DDiff[12]+DDiff[13]+DDiff[14]+DDiff[15]+DDiff[16]+DDiff[17]+DDiff[18]+DDiff[19];
    diff_inte = diff_inte + DDiff_t * 0.1;
    // 完成差分运算

    // 比例项计算
    target_angle_t = angle_add_transfer(target_angle, (-benchmark));

    // 积分项还没加 的代码
    // 积分项如果 增加超调 则归零 如果减小超调则增加
    // 判断方法   积分项符号和比例项相同 则说明减小超调[误差] 相反说明增大[误差]
    // 积分项在   误差[target_angle_t]很大时 增长速度过快 所以 限定增长速度 既每次增长的最大值
    // 当然也可以设定误差在x以上积分项就不增长了
    it = target_angle_t;
    it = it * i_quotiety;                    // 积分项系数乘这里
    if (it > INTEGRAL_ANGLE_s_periods_limit) // 限定累加[积分] 每一次的大小
    {
        it = INTEGRAL_ANGLE_s_periods_limit;
    }
    if (it < -INTEGRAL_ANGLE_s_periods_limit)
    {
        it = -INTEGRAL_ANGLE_s_periods_limit;
    }

    // 判断减小超调[误差] 还是增大[误差]	//注意这里可以和微分量关联 积分量清零过慢
    //		if (((target_angle_t>0)&&(integral<0))||((target_angle_t<0)&&(integral>0)))
    // if (((target_angle_t>-5)&&(integral<0))||((target_angle_t<5)&&(integral>0)))
    //		{	integral=0;
    //		}
    //		if(((d_quotiety>0)&&(it<0))||((d_quotiety<0)&&(it>0)))
    //			it=0;
    integral = integral + it;
    // 设置积分上限 饱和值
    if (integral > INTEGRAL_ANGLE_SATURATION)
    {
        integral = INTEGRAL_ANGLE_SATURATION;
    }
    if (integral < -INTEGRAL_ANGLE_SATURATION)
    {
        integral = -INTEGRAL_ANGLE_SATURATION;
    }

    p_result = p_quotiety * target_angle_t;
    i_result = integral; // i_result=i_quotiety * integral;
    d_result = d_quotiety * (float)DDiff_t;
    yt = p_result + i_result + d_result;

    // 死区的设置 将0~128的空间 转到 [压缩到]40~128  或dead_zone~128
    zt = dead_zone + (128.0 - dead_zone) * (fabs(yt)) / 128.0;
    // x/(128-dead_zone)=(128-abs(y))/128
    if (yt < 0)
    {
        yt = -zt;
    }
    else
    {
        yt = zt;
    }

    // 三个电机 正转 机体逆时针转动   既 y为正 机体顺时针转动 电机功率负
    if (target_angle_t < 0)
    {
        *LorRout = 'L'; // 纪录上次是从哪个方向出去的
    }
    else
    {
        *LorRout = 'R'; // 纪录上次是从哪个方向出去的
    }
    turn_v_c += yt; // 保存当前次角度 跟踪 输出量大小	便于后面运动合成用
    return (yt);    // 输出计算结果角速度
}

// 配套函数:
// 参数设置						parameter_set_angle(float p_quotiety_,float i_quotiety_,float d_quotiety_,float
// dead_zone_) 参数和内部积分量初始化		initializing_angle(void) 极限输出限定参数设置
// limit_parameter_set_angle(float velocity_limit_,float acceleration_limit_) 限定转动[功率]速度输出最大值
// turn_v_maxlimit(void) 限定转动[功率]加速度最大值	turn_a_maxlimit(void)
// 函数使用说明:
// float target_angle	跟踪目标的 本机坐标 角度
// float benchmark		跟踪目标的 本机基准
// int *DD				跟踪目标的 本机坐标 角度 历史记录
// char *LorRout			函数决策	结果的文本形式
// 返回值为				计算结果角速度[准确说是修正量 无单位]
float CMovement2Control::zs_angle_track_002(float target_angle, float benchmark)
{
    int i;
    float DD[20]; // 目标角度历史记录
    for (i = 0; i < 20; i++)
    {
        DD[i] = target_h[i].angle;
    }
    // 类内部 保护成员变量
    //	float p_quotiety;//比例项系数proportion
    //	float i_quotiety;//积分项系数integral
    //	float integral	;//积分项
    //	float d_quotiety;//微分项系数differential coefficient
    //	float dead_zone	;//电机功率死区
    float angular_velocity; // 返回量计算结果角速度

    // 增量调节正方向如果基准0 物体在基准右 往右转target_angle - benchmark >0  即正数增量往右转
    float target_angle_t, it, yt, zt; //,angle_increment,xt,
    float DDiff[20];
    float DDiff_t;
    // 差分部分正方向目标向左[逆时针]DDiff_t为正
    DDiff[0] = angle_add_transfer(target_angle, (float)(-DD[0]));
    for (i = 1; i < 20; i++) // 电机功率差的历史纪录 用于求微分 积分用
    {
        DDiff[i] = angle_add_transfer(DD[i], -DD[i - 1]);
    }
    DDiff_t =
        (1 * DDiff[0] + 0.5 * DDiff[1] + 0.25 * DDiff[2] + 0.125 * DDiff[3] + 0.125 * DDiff[4]) *
        0.5; //+0.25*DDiff[5]+0.125*DDiff[6]+0.125*DDiff[7]+0.125*DDiff[8]+0.125*DDiff[9]+0.125*DDiff[10];//+DDiff[11]+DDiff[12]+DDiff[13]+DDiff[14]+DDiff[15]+DDiff[16]+DDiff[17]+DDiff[18]+DDiff[19];
    // DDiff_t=10*DDiff[0]+9*DDiff[1]+8*DDiff[2]+7*DDiff[3]+6*DDiff[4]+5*DDiff[5]+4*DDiff[6]+3*DDiff[7]+2*DDiff[8]+DDiff[9]+DDiff[10]+DDiff[11]+DDiff[12]+DDiff[13]+DDiff[14]+DDiff[15]+DDiff[16]+DDiff[17]+DDiff[18]+DDiff[19];
    // diff_inte = diff_inte + DDiff_t*0.1;
    // 完成差分运算

    // 比例项计算
    target_angle_t = angle_add_transfer(target_angle, -benchmark);
    p_result = p_quotiety * target_angle_t; // 计算比例量
    d_result = d_quotiety * DDiff_t;        // 计算微分量

    // 积分项还没加 的代码
    // 积分项如果 增加超调 则归零 如果减小超调则增加
    // 判断方法   积分项符号和比例项相同 则说明减小超调[误差] 相反说明增大[误差]
    // 积分项在   误差[target_angle_t]很大时 增长速度过快 所以 限定增长速度 既每次增长的最大值
    // 当然也可以设定误差在x以上积分项就不增长了
    it = target_angle_t;
    //		it = (angle_add_transfer(target_angle,-target_h[0].angle) + p_result)* i_quotiety;//积分项系数乘这里

    if (it > INTEGRAL_ANGLE_s_periods_limit) // 限定累加[积分] 每一次的大小
    {
        it = INTEGRAL_ANGLE_s_periods_limit;
    }
    if (it < -INTEGRAL_ANGLE_s_periods_limit)
    {
        it = -INTEGRAL_ANGLE_s_periods_limit;
    }

    // 判断减小超调[误差] 还是增大[误差]	//注意这里和微分量关联
    // if (((d_result>0)&&(it<0))||((d_result<0)&&(it>0))
    //   ||((target_angle_t>0)&&(it<0))||((target_angle_t<0)&&(it>0)))
    //		if (((target_angle_t>0)&&(it<0))||((target_angle_t<0)&&(it>0)))
    //{	it=0;
    //}
    integral = integral + it;
    // 设置积分上限 饱和值
    if (integral > INTEGRAL_ANGLE_SATURATION)
    {
        integral = INTEGRAL_ANGLE_SATURATION;
    }
    if (integral < -INTEGRAL_ANGLE_SATURATION)
    {
        integral = -INTEGRAL_ANGLE_SATURATION;
    }

    i_result = integral;      // i_result=i_quotiety * integral;
    yt = p_result + d_result; //+  i_result
                              // yt =   i_result  ;//+p_result  +  d_result;
    // 死区的设置 代码去掉将0~128的空间 转到 [压缩到]40~128  或dead_zone~128
    // 三个电机 正转 机体逆时针转动   既 y为正 机体顺时针转动 电机功率负
    turn_v_c += yt; // 保存当前次角度 跟踪 输出量大小	便于后面运动合成用
    return (yt);    // 输出计算结果角速度
}

// 参数设置//p_quotiety_比例项//i_quotiety_积分项//d_quotiety_微分项//死区dead_zone_
void CMovement2Control::parameter_set_angle(float p_quotiety_, float i_quotiety_, float d_quotiety_, float dead_zone_)
{
    p_quotiety = p_quotiety_;
    i_quotiety = i_quotiety_;
    d_quotiety = d_quotiety_;
    dead_zone = dead_zone_;
    //	integral  =0;
}

// 参数和内部积分量初始化
void CMovement2Control::initializing_angle(void)
{ //	p_quotiety=0	;
    //	i_quotiety=0	;
    //	d_quotiety=0	;
    //	dead_zone =0	;
    integral = 0;
    p_result = 0;
    i_result = 0;
    d_result = 0;
    turn_v_c = 0; // 当前次角度 跟踪 输出量大小
    turn_v_p = 0; // 前一次角度 跟踪 输出量大小
}

// 极限输出限定参数设置//float velocity_limit_速度最大值 //acceleration_limit_加速度最大值
void CMovement2Control::limit_parameter_set_angle(float velocity_limit_, float acceleration_limit_)
{
    velocity_limit = velocity_limit_;
    acceleration_limit = acceleration_limit_;
}

// 限定转动[功率]速度输出最大值	限定角[功率]速度输出最大值
void CMovement2Control::turn_v_maxlimit(void)
{
    if (turn_v_c > velocity_limit) // 对转弯角速度 单边功率限制 velocity_limit_dis;//速度最大值控制
        turn_v_c = velocity_limit;
    if (turn_v_c < -velocity_limit)
        turn_v_c = -velocity_limit;
}

// 限定转动[功率]加速度最大值	限定角[功率]加速度最大值
void CMovement2Control::turn_a_maxlimit(void)
{
    if (turn_v_c > (turn_v_p + acceleration_limit)) // acceleration_limit_dis;//加速度最大值控制
    {
        turn_v_c = turn_v_p + acceleration_limit; // acceleration_limit_dis;//加速度最大值控制
    }
    if (turn_v_c < (turn_v_p - acceleration_limit)) // acceleration_limit_dis;//加速度最大值控制
    {
        turn_v_c = turn_v_p - acceleration_limit; // acceleration_limit_dis;//加速度最大值控制
    }
    //	turn_v_p = turn_v_c;		 //存上个周期的值
    //	注意：因为在后面运动合成中还会变动turn_v_c所以改在最后输出movement_output时存储turn_v_p
}

// 有错误 现在计算的不是差分 而是差分的差分  [加速度]!!!!!!!!!!
// 自我保存历史记录 保存数据位置	 translate_radius_h[i]    translate_angle_h[i]
// 保存类别   每次计算的  矢量差 20周期

// 配套函数:
// 参数设置						parameter_set_dis(float p_quotiety_,float i_quotiety_,float d_quotiety_,float
// dead_zone_) 参数和内部积分量初始化		initializing_dis(void)
// 极限输出限定参数设置			limit_parameter_set_dis(float velocity_limit_,float acceleration_limit_)
// 限定平移[功率]速度输出最大值	front_v_maxlimit(void)
// 限定平移[功率]加速度最大值	front_a_maxlimit(void)
// 函数使用说明:
// 距离对准 PID
// 注意 目标距离 基准远误差为正+  近误差为负-
// float target_radius	跟踪目标的 本机坐标 距离
// float benchmark		跟踪目标的 本机基准 要对准的距离
// int *DD				跟踪目标的 本机坐标 角度 历史记录
// char *LorRout			函数决策	结果的文本形式
// 返回值为				计算结果平移速度[准确说是修正量 无单位]
float CMovement2Control::zs_point_track_01_001(float target_angle, float target_radius, float benchmark_angle,
                                               float benchmark_radius)
{
    // 类内部 保护成员变量
    //	float p_quotiety;//比例项系数proportion
    //	float i_quotiety;//积分项系数integral
    //	float integral	;//积分项
    //	float d_quotiety;//微分项系数differential coefficient
    //	float dead_zone	;//电机功率死区
    /*
    double coordinate_transfer_angle(double x,double y);
    double coordinate_transfer_radius(double x,double y);
    double coordinate_transfer_x(double angle,double radius);
    double coordinate_transfer_y(double angle,double radius);
        float angle_add_transfer(float angle,float n);//两个角度 相加
    */
    // 增量调节正方向如果基准0 物体在基准右 往右转target_angle - benchmark >0  即正数增量往右转
    int i;
    float it, yt, zt;         //,angle_increment,xt,
    float outAngle, outSpeed; // 输出矢量
    float target_radiu_t;
    float target_angle_t;
    // float DDiff[20];
    float DDiffangle[20];
    float DDiffradiu[20];
    float DDiff_t_angle;
    float DDiff_t_radiu;
    float temp_angle = 0;
    float temp_radiu = 0;

    double target_x = coordinate_transfer_x(target_angle, target_radius);
    double target_y = coordinate_transfer_y(target_angle, target_radius);
    double benchmark_x = coordinate_transfer_x(benchmark_angle, benchmark_radius);
    double benchmark_y = coordinate_transfer_y(benchmark_angle, benchmark_radius);

    double translate_angle = coordinate_transfer_angle(
        target_x - benchmark_x,
        target_y -
            benchmark_y); //(benchmark_x-target_x,benchmark_y-target_y);//(target_x-benchmark_x,target_y-benchmark_y);
    double translate_radius = coordinate_transfer_radius(
        target_x - benchmark_x, target_y - benchmark_y); //(benchmark_x-target_x,benchmark_y-target_y);
    // translate_angle = angle_add_transfer(translate_angle,0);//两个角度 相加
    // 差分部分正方向目标向左[逆时针]DDiff_t为正
    // 完成差分运算	记录历史值
    for (i = 19; i > 0; i--) // 球位置的历史纪录 用于求微分 积分用
    {
        translate_radius_h[i] = translate_radius_h[i - 1];
    }
    translate_radius_h[0] = translate_radius; // 目标距离本机定点距离的历史记录

    for (i = 19; i > 0; i--) // 球位置的历史纪录 用于求微分 积分用
    {
        translate_angle_h[i] = translate_angle_h[i - 1];
    }
    translate_angle_h[0] = translate_angle; // 目标距离本机定点距离的历史记录
    // 行进方向 速度 是一个向量 现在按向量计算
    //	DDiff[0] = translate_radius - translate_radius_h[0];
    DDiffangle[0] = vector_add_angle(translate_angle, translate_radius, translate_angle_h[0], -translate_radius_h[0]);
    DDiffradiu[0] = vector_add_length(translate_angle, translate_radius, translate_angle_h[0], -translate_radius_h[0]);

    for (i = 1; i < 20; i++) // 电机功率差的历史纪录 用于求微分 积分用
    {
        // DDiff[i]=translate_radius_h[i] - translate_radius_h[i-1];
        DDiffangle[i] = vector_add_angle(translate_angle_h[i], translate_radius_h[i], translate_angle_h[i - 1],
                                         -translate_radius_h[i - 1]);
        DDiffradiu[i] = vector_add_length(translate_angle_h[i], translate_radius_h[i], translate_angle_h[i - 1],
                                          -translate_radius_h[i - 1]);
    }
    //	DDiff_t=10*DDiff[0]+8*DDiff[1]+6*DDiff[2]+5*DDiff[3]+4*DDiff[4]+3*DDiff[5]+2*DDiff[6]+2*DDiff[7]+DDiff[8]+DDiff[9]+DDiff[10];//+DDiff[11]+DDiff[12]+DDiff[13]+DDiff[14]+DDiff[15]+DDiff[16]+DDiff[17]+DDiff[18]+DDiff[19];
    float DDiff_quotiety[20] = {10, 8, 6, 5, 4, 3, 2, 2, 1, 1, 1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
    float DDiff_quotiety_add = 0;
    DDiff_t_angle = DDiffangle[0];
    DDiff_t_radiu = DDiffradiu[0] * DDiff_quotiety[0];
    for (i = 1; i < 20; i++) // 电机功率差的历史纪录 用于求微分 积分用
    {
        temp_angle = vector_add_angle(DDiff_t_angle, DDiff_t_radiu, DDiffangle[i], DDiffradiu[i]);
        temp_radiu = vector_add_length(DDiff_t_angle, DDiff_t_radiu, DDiffangle[i], DDiffradiu[i] * DDiff_quotiety[i]);
        DDiff_t_angle = temp_angle;
        DDiff_t_radiu = temp_radiu;
        DDiff_quotiety_add += DDiff_quotiety[i];
    }
    DDiff_t_radiu = DDiff_t_radiu / DDiff_quotiety_add;
    //	diff_inte_dis = diff_inte_dis + DDiff_t*0.1;
    //	diff_inte_dis = DDiff_t/(float)(10+8+6+5+4+3+2+2+1+1+1);//20081021

    // 比例项计算
    target_radiu_t = translate_radius; // target_radius - (float)(benchmark);
    target_angle_t = translate_angle;  // target_radius - (float)(benchmark);

    // 积分项还没加 的代码
    // 积分项如果 增加超调 则归零 如果减小超调则增加
    // 判断方法   积分项符号和比例项相同 则说明减小超调[误差] 相反说明增大[误差]
    // 积分项在   误差[target_angle_t]很大时 增长速度过快 所以 限定增长速度 既每次增长的最大值
    // 当然也可以设定误差在x以上积分项就不增长了
    /*		it = target_radius_t;
            it = it * i_quotiety_dis;//积分项系数乘这里
            if (it>INTEGRAL_DIS_s_periods_limit)//限定累加[积分] 每一次的大小
            {	it=INTEGRAL_DIS_s_periods_limit;
            }
            if (it<-INTEGRAL_DIS_s_periods_limit)
            {	it=-INTEGRAL_DIS_s_periods_limit;
            }

            //判断减小超调[误差] 还是增大[误差]	//注意这里可以和微分量关联 积分量清零过慢
            if (((target_radius_t>0)&&(integral_dis<0))||((target_radius_t<0)&&(integral_dis>0)))
            //if (((target_angle_t>-5)&&(integral<0))||((target_angle_t<5)&&(integral>0)))
            {	integral_dis=0;
            }
            integral_dis = integral_dis + it;
            //设置积分上限 饱和值
            if (integral_dis>INTEGRAL_DIS_SATURATION)
            {	integral_dis=INTEGRAL_DIS_SATURATION;
            }
            if (integral_dis<-INTEGRAL_DIS_SATURATION)
            {	integral_dis=-INTEGRAL_DIS_SATURATION;
            }
    */

    //	p_result_dis = p_quotiety_dis * target_radius_t;
    //	i_result_dis = integral_dis;//i_quotiety_dis * integral_dis;
    //	d_result_dis = d_quotiety_dis * (float)DDiff_t;
    //	yt = p_result_dis  +  i_result_dis  +  d_result_dis;
    p_result_dis_a = target_angle_t;
    p_result_dis_r = p_quotiety_dis * target_radiu_t;
    i_result_dis_a = 0;
    i_result_dis_r = 0;
    d_result_dis_a = DDiff_t_angle;
    d_result_dis_r = d_quotiety_dis * DDiff_t_radiu;

    outAngle = p_result_dis_a;
    outSpeed = p_result_dis_r;
    temp_angle = vector_add_angle(outAngle, outSpeed, d_result_dis_a, d_result_dis_r);
    temp_radiu = vector_add_length(outAngle, outSpeed, d_result_dis_a, d_result_dis_r);
    outAngle = temp_angle;
    outSpeed = temp_radiu;
    //	outAngle= vector_add_angle (outAngle,outSpeed   ,DDiff_t_angle,d_quotiety_dis*DDiff_t_radiu);
    //	outSpeed= vector_add_length(outAngle,outSpeed   ,DDiff_t_angle,d_quotiety_dis*DDiff_t_radiu);

    // 死区的设置 将0~128的空间 转到 [压缩到]40~128  或dead_zone~128
    //	zt=	dead_zone_dis+(128-dead_zone_dis)*(abs((int)yt))/128;
    //		//x/(128-dead_zone)=(128-abs(y))/128
    //	if (yt<0)
    //	{	yt=-zt;
    //	}
    //	else
    //	{	yt=zt;
    //	}
    // extro=(Distance_t*1.2);//再加了一个 小小的10有助于对准目标精度 （不用积分项）
    // 三个电机 正转 机体逆时针转动   既 y为正 机体顺时针转动 电机功率负
    //	if(target_radius_t<0)
    //	{
    //		*LorRout='F';//纪录上次是从哪个方向出去的
    //	}
    //	else
    //	{
    //		*LorRout='B';//纪录上次是从哪个方向出去的
    //	}

    //	front_v_c = outSpeed;//保存当前次平移 跟踪 输出量大小	便于后面运动合成用
    // 距离跟踪平移标量  矢量化 设置平移方向
    //	front_v_angle_c=outAngle;//target_angle;//当前次平移方向

    // 将当前运动向量与  计算结果平移分量相加
    zs_translate_compound_O(outAngle, outSpeed);
    return (yt);
}

// 外部保存历史记录 保存数据位置	 obj_properties target_h[20];//跟踪目标的历史记录  要求调用程序每周期更新
// 保存类别   每次目标所有位置参数   20周期

// 配套函数:
// 参数设置						parameter_set_dis(float p_quotiety_,float i_quotiety_,float d_quotiety_,float
// dead_zone_) 参数和内部积分量初始化		initializing_dis(void)
// 极限输出限定参数设置			limit_parameter_set_dis(float velocity_limit_,float acceleration_limit_)
// 限定平移[功率]速度输出最大值	front_v_maxlimit(void)
// 限定平移[功率]加速度最大值	front_a_maxlimit(void)
// 函数使用说明:
// 距离对准 PID
// 注意 目标距离 基准远误差为正+  近误差为负-
// float target_radius	跟踪目标的 本机坐标 距离
// float benchmark		跟踪目标的 本机基准 要对准的距离
// int *DD				跟踪目标的 本机坐标 角度 历史记录
// char *LorRout			函数决策	结果的文本形式
// 返回值为				计算结果平移速度[准确说是修正量 无单位]
float CMovement2Control::zs_point_track_01_002(float target_angle, float target_radius, float benchmark_angle,
                                               float benchmark_radius)
{
    // 类内部 保护成员变量
    //	float p_quotiety;//比例项系数proportion
    //	float i_quotiety;//积分项系数integral
    //	float integral	;//积分项
    //	float d_quotiety;//微分项系数differential coefficient
    //	float dead_zone	;//电机功率死区
    // 增量调节正方向如果基准0 物体在基准右 往右转target_angle - benchmark >0  即正数增量往右转
    int i = 0;
    float it = 0;
    float yt = 0;
    float zt = 0;       //,angle_increment,xt,
    float outAngle = 0; // 输出矢量
    float outSpeed = 0; // 输出矢量
    float target_radiu_t = 0;
    float target_angle_t = 0;
    float temp_angle = 0;
    float temp_radiu = 0;

    // float DDiff[20];
    float DDiffangle[20] = {0};
    float DDiffradiu[20] = {0};
    float DDiff_t_angle = 0;
    float DDiff_t_radiu = 0;

    double target_x = coordinate_transfer_x(target_angle, target_radius);
    double target_y = coordinate_transfer_y(target_angle, target_radius);
    double benchmark_x = coordinate_transfer_x(benchmark_angle, benchmark_radius);
    double benchmark_y = coordinate_transfer_y(benchmark_angle, benchmark_radius);

    double translate_angle = coordinate_transfer_angle(
        target_x - benchmark_x,
        target_y -
            benchmark_y); //(benchmark_x-target_x,benchmark_y-target_y);//(target_x-benchmark_x,target_y-benchmark_y);
    double translate_radius = coordinate_transfer_radius(
        target_x - benchmark_x, target_y - benchmark_y); //(benchmark_x-target_x,benchmark_y-target_y);
    // translate_angle = angle_add_transfer(translate_angle,0);//两个角度 相加
    // 差分部分正方向目标向左[逆时针]DDiff_t为正

    // 行进方向 速度 是一个向量 现在按向量计算
    //	DDiff[0] = translate_radius - {translate_radius_h}[0];
    // float target_angle,float target_radius
    // DDiffangle[0]= vector_add_angle (translate_angle,translate_radius,target_h[0].angle,-target_h[0].radius);
    // DDiffradiu[0]= vector_add_length(translate_angle,translate_radius,target_h[0].angle,-target_h[0].radius);
    DDiffangle[0] = vector_add_angle(target_angle, target_radius, target_h[0].angle, -target_h[0].radius);
    DDiffradiu[0] = vector_add_length(target_angle, target_radius, target_h[0].angle, -target_h[0].radius);

    for (i = 1; i < 20; i++) // 电机功率差的历史纪录 用于求微分 积分用
    {
        // DDiff[i]={translate_radius_h}[i] - {translate_radius_h}[i-1];
        DDiffangle[i] =
            vector_add_angle(target_h[i].angle, target_h[i].radius, target_h[i - 1].angle, -target_h[i - 1].radius);
        DDiffradiu[i] =
            vector_add_length(target_h[i].angle, target_h[i].radius, target_h[i - 1].angle, -target_h[i - 1].radius);
    }
    //	DDiff_t=10*DDiff[0]+8*DDiff[1]+6*DDiff[2]+5*DDiff[3]+4*DDiff[4]+3*DDiff[5]+2*DDiff[6]+2*DDiff[7]+DDiff[8]+DDiff[9]+DDiff[10];//+DDiff[11]+DDiff[12]+DDiff[13]+DDiff[14]+DDiff[15]+DDiff[16]+DDiff[17]+DDiff[18]+DDiff[19];
    float DDiff_quotiety[20] = {10, 5, 3, 2, 1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float DDiff_quotiety_add = 0;
    DDiff_t_angle = DDiffangle[0];
    DDiff_t_radiu = DDiffradiu[0] * DDiff_quotiety[0];
    for (i = 1; i < 20; i++) // 电机功率差的历史纪录 用于求微分 积分用
    {
        temp_angle = vector_add_angle(DDiff_t_angle, DDiff_t_radiu, DDiffangle[i], DDiffradiu[i]);
        temp_radiu = vector_add_length(DDiff_t_angle, DDiff_t_radiu, DDiffangle[i], DDiffradiu[i] * DDiff_quotiety[i]);
        DDiff_t_angle = temp_angle;
        DDiff_t_radiu = temp_radiu;
        DDiff_quotiety_add += DDiff_quotiety[i];
    }
    DDiff_t_radiu = DDiff_t_radiu / DDiff_quotiety_add;
    // 比例项计算
    target_radiu_t = translate_radius; // target_radius - (float)(benchmark);
    target_angle_t = translate_angle;  // target_radius - (float)(benchmark);
    // 积分项还没加 的代码
    // 积分项如果 增加超调 则归零 如果减小超调则增加
    // 判断方法   积分项符号和比例项相同 则说明减小超调[误差] 相反说明增大[误差]
    // 积分项在   误差[target_angle_t]很大时 增长速度过快 所以 限定增长速度 既每次增长的最大值
    // 当然也可以设定误差在x以上积分项就不增长了

    //	p_result_dis = p_quotiety_dis * target_radius_t;
    //	i_result_dis = integral_dis;//i_quotiety_dis * integral_dis;
    //	d_result_dis = d_quotiety_dis * (float)DDiff_t;
    //	yt = p_result_dis  +  i_result_dis  +  d_result_dis;
    p_result_dis_a = target_angle_t;
    p_result_dis_r = p_quotiety_dis * target_radiu_t;
    i_result_dis_a = 0;
    i_result_dis_r = 0;
    d_result_dis_a = DDiff_t_angle;
    d_result_dis_r = d_quotiety_dis * DDiff_t_radiu;

    outAngle = p_result_dis_a;
    outSpeed = p_result_dis_r;
    temp_angle = vector_add_angle(outAngle, outSpeed, d_result_dis_a, d_result_dis_r);
    temp_radiu = vector_add_length(outAngle, outSpeed, d_result_dis_a, d_result_dis_r);
    outAngle = temp_angle;
    outSpeed = temp_radiu;
    //	outAngle= vector_add_angle (outAngle,outSpeed   ,DDiff_t_angle,d_quotiety_dis*DDiff_t_radiu);
    //	outSpeed= vector_add_length(outAngle,outSpeed   ,DDiff_t_angle,d_quotiety_dis*DDiff_t_radiu);

    // 将当前运动向量与  计算结果平移分量相加
    zs_translate_compound_O(outAngle, outSpeed);
    return (yt);
}

// 参数设置
void CMovement2Control::parameter_set_dis(float p_quotiety_, float i_quotiety_, float d_quotiety_, float dead_zone_)
{
    p_quotiety_dis = p_quotiety_;
    i_quotiety_dis = i_quotiety_;
    d_quotiety_dis = d_quotiety_;
    dead_zone_dis = dead_zone_;
    //	integral_dis  =0;
    //	velocity_dis  =0;
}

// 参数和内部积分量初始化
void CMovement2Control::initializing_dis(void)
{
    //	p_quotiety_dis=0	;
    //	i_quotiety_dis=0	;
    //	d_quotiety_dis=0	;
    //	dead_zone_dis =0	;
    integral_dis = 0;
    p_result_dis = 0;
    i_result_dis = 0;
    d_result_dis = 0;
    front_v_c = 0;       // 当前次平移 跟踪 输出量大小
    front_v_p = 0;       // 前一次平移 跟踪 输出量大小
    front_v_angle_c = 0; // 当前次平移角度 跟踪 输出量大小
    front_v_angle_p = 0; // 前一次平移角度 跟踪 输出量大小
    for (int i = 0; i < 20; i++)
    {
        translate_radius_h[i] = 0; // 目标距离本机定点距离的历史记录
    }
    for (i = 0; i < 20; i++)
    {
        translate_angle_h[i] = 0; // 目标距离本机定点距离的历史记录
    }
}

// 极限输出限定参数设置
void CMovement2Control::limit_parameter_set_dis(float velocity_limit_dis_, float acceleration_limit_dis_)
{
    velocity_limit_dis = velocity_limit_dis_;
    acceleration_limit_dis = acceleration_limit_dis_;
}

// 限定平移[功率]速度输出最大值
void CMovement2Control::front_v_maxlimit(void)
{
    if (front_v_c > velocity_limit_dis) // 对转弯角速度 单边功率限制 velocity_limit_dis;//速度最大值控制
        front_v_c = velocity_limit_dis;
    if (front_v_c < -velocity_limit_dis)
        front_v_c = -velocity_limit_dis;
}

// 限定平移[功率]加速度最大值
void CMovement2Control::front_a_maxlimit(void)
{
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
}

// void CMovement2Control::zs_movement_compound(void)//角速度 优先合成角速度平移速度两个分量
// void CMovement2Control::zs_movement_compound(float)//比例合成角速度平移速度两个分量

// 用来初始化 预制默认的 平动,转动分量  既;如果后面没有变动[如;角度对准  距离对准]那么就用默认值
void CMovement2Control::initializing_movement(float turn, float front, float angle)
{
    front_v_c = front;
    turn_v_c = turn;
    front_v_angle_c = angle;
}

// 角速度 优先合成角速度平移速度两个分量		既:如果输出饱和 就靠削减平移分量保证角速度
void CMovement2Control::zs_movement_compound(void)
{
    if ((fabs((double)front_v_c) + fabs((double)turn_v_c)) > 128)
    {
        if (front_v_c > 0)
            front_v_c = 128 - fabs((double)turn_v_c);
        if (front_v_c < 0)
            front_v_c = -128 + fabs((double)turn_v_c);
    }
}

// 将当前运动向量与  另一个平移分量相加
void CMovement2Control::zs_translate_compound_O(float angle, float speed)
{
    double x1, y1, x2, y2;
    x1 = 0;
    y1 = 0;
    x2 = 0;
    y2 = 0;

    x1 = coordinate_transfer_x(front_v_angle_c, front_v_c); // 极坐标到直角坐标转换函数 角度
    y1 = coordinate_transfer_y(front_v_angle_c, front_v_c); // 极坐标到直角坐标转换函数 半径
    x2 = coordinate_transfer_x(angle, speed);               // 极坐标到直角坐标转换函数 角度
    y2 = coordinate_transfer_y(angle, speed);               // 极坐标到直角坐标转换函数 半径
    front_v_angle_c = coordinate_transfer_angle((x1 + x2), (y1 + y2));
    front_v_c = coordinate_transfer_radius((x1 + x2), (y1 + y2));
    /*	if (front_v_c>0)	//注意因为如果出现front_v_c为负值的情况 会出现经过坐标系转换后的结果
        {					//front_v_c变为正 角度转180度的现象  已知会引起差分量的严重错误
            front_v_angle_c=coordinate_transfer_angle((x1+x2),(y1+y2));
            front_v_c=coordinate_transfer_radius((x1+x2),(y1+y2));
        }
        else
        {	front_v_angle_c=   angle_add_transfer(coordinate_transfer_angle((x1+x2),(y1+y2)),180) ;
            front_v_c	 =  -coordinate_transfer_radius((x1+x2),(y1+y2));
        }
    */
}

// 将两个向量	相加	得到结果向量的角度
float CMovement2Control::vector_add_angle(float angle1, float length1, float angle2, float length2)
{
    double x1, y1, x2, y2, angleOut;
    x1 = 0;
    y1 = 0;
    x2 = 0;
    y2 = 0;
    angleOut = 0;
    x1 = coordinate_transfer_x(angle1, length1); // 极坐标到直角坐标转换函数 角度
    y1 = coordinate_transfer_y(angle1, length1); // 极坐标到直角坐标转换函数 半径
    x2 = coordinate_transfer_x(angle2, length2); // 极坐标到直角坐标转换函数 角度
    y2 = coordinate_transfer_y(angle2, length2); // 极坐标到直角坐标转换函数 半径
    angleOut = coordinate_transfer_angle((x1 + x2), (y1 + y2));
    return (angleOut);
}
// 将两个向量	相加	得到结果向量的长度
float CMovement2Control::vector_add_length(float angle1, float length1, float angle2, float length2)
{
    double x1, y1, x2, y2, lengthOut;
    x1 = 0;
    y1 = 0;
    x2 = 0;
    y2 = 0;
    lengthOut = 0;
    x1 = coordinate_transfer_x(angle1, length1); // 极坐标到直角坐标转换函数 角度
    y1 = coordinate_transfer_y(angle1, length1); // 极坐标到直角坐标转换函数 半径
    x2 = coordinate_transfer_x(angle2, length2); // 极坐标到直角坐标转换函数 角度
    y2 = coordinate_transfer_y(angle2, length2); // 极坐标到直角坐标转换函数 半径
    lengthOut = coordinate_transfer_radius((x1 + x2), (y1 + y2));
    return (lengthOut);
}

// 角速度优先合成    角速度平移速度两个分量		既:如果输出饱和 就靠削减平移分量保证角速度
char CMovement2Control::zs_movement_compound_O(void)
{
    // 应该先限速	最后一次把关(程序结识耐用)
    float max_motor_speed = 0;
    float max_speed_motor_num = 0;
    float motor1, motor2, motor3;
    //	motor1=O_movement_analyze_m1(front_v_angle_c,front_v_c)-turn_v_c;//1电机分速度
    //	motor2=O_movement_analyze_m2(front_v_angle_c,front_v_c)-turn_v_c;//2电机分速度
    //	motor3=O_movement_analyze_m3(front_v_angle_c,front_v_c)-turn_v_c;//3电机分速度
    O_movement_analyze(front_v_angle_c, front_v_c, &motor1, &motor2, &motor3); // 全向解析
    if (abs(motor1 - turn_v_c) > abs(motor2 - turn_v_c))
    {
        max_motor_speed = motor1 - turn_v_c;
        max_speed_motor_num = 1;
    }
    else
    {
        max_motor_speed = motor2 - turn_v_c;
        max_speed_motor_num = 2;
    }

    if (abs(motor3 - turn_v_c) > abs(max_motor_speed))
    {
        max_motor_speed = motor3 - turn_v_c;
        max_speed_motor_num = 3;
    }

    if (max_motor_speed > 128) //( (fabs((double)max_motor_speed)+fabs((double)turn_v_c))>128  )
    {
        /*		if (max_motor_speed>0)
                    max_motor_speed = 128- fabs((double)turn_v_c);
                if (max_motor_speed<=0)
                    max_motor_speed =-128+ fabs((double)turn_v_c);
        */
        max_motor_speed = 128;
    }
    if (max_motor_speed < -128)
    {
        max_motor_speed = -128;
    }

    if (max_speed_motor_num == 1)
        front_v_c = O_movement_analyze_m1_T(front_v_angle_c, max_motor_speed + turn_v_c);
    if (max_speed_motor_num == 2)
        front_v_c = O_movement_analyze_m2_T(front_v_angle_c, max_motor_speed + turn_v_c);
    if (max_speed_motor_num == 3)
        front_v_c = O_movement_analyze_m3_T(front_v_angle_c, max_motor_speed + turn_v_c);

    return (max_speed_motor_num); // 知道返回0是错误
}

// 饱和后平移单电机速度分量 占单电机总速度的f_v
// 饱和后转动单电机速度分量 占单电机总速度的t_v
// 单电机速度上限限制m_v
char CMovement2Control::zs_movement_compound_O(float t_v, float f_v, float m_v)
{
    // 应该先限速	最后一次把关(程序结识耐用)
    float max_motor_speed = 0;
    float max_speed_motor_num = 0;
    float motor1, motor2, motor3;
    //	motor1=O_movement_analyze_m1(front_v_angle_c,front_v_c)-turn_v_c;//1电机分速度
    //	motor2=O_movement_analyze_m2(front_v_angle_c,front_v_c)-turn_v_c;//2电机分速度
    //	motor3=O_movement_analyze_m3(front_v_angle_c,front_v_c)-turn_v_c;//3电机分速度
    O_movement_analyze(front_v_angle_c, front_v_c, &motor1, &motor2, &motor3); // 全向解析
    if (abs(motor1 - turn_v_c) > abs(motor2 - turn_v_c))
    {
        max_motor_speed = motor1 - turn_v_c;
        max_speed_motor_num = 1;
    }
    else
    {
        max_motor_speed = motor2 - turn_v_c;
        max_speed_motor_num = 2;
    }

    if (abs(motor3 - turn_v_c) > abs(max_motor_speed))
    {
        max_motor_speed = motor3 - turn_v_c;
        max_speed_motor_num = 3;
    }

    if (0) //(abs(max_motor_speed)>m_v)//( (fabs((double)max_motor_speed)+fabs((double)turn_v_c))>128  )
    {
        if (max_motor_speed > m_v) //( (fabs((double)max_motor_speed)+fabs((double)turn_v_c))>128  )
        {
            /*		if (max_motor_speed>0)
                        max_motor_speed = 128- fabs((double)turn_v_c);
                    if (max_motor_speed<=0)
                        max_motor_speed =-128+ fabs((double)turn_v_c);
            */
            max_motor_speed = m_v;
        }
        if (max_motor_speed < (-m_v))
        {
            max_motor_speed = (-m_v);
        }
        if (turn_v_c > (m_v * t_v))
        {
            turn_v_c = m_v * t_v;
        }
        if (turn_v_c < (-m_v * t_v))
        {
            turn_v_c = -m_v * t_v;
        }
        if ((max_motor_speed + turn_v_c) > (m_v * f_v))
        {
            max_motor_speed = m_v * f_v - turn_v_c;
        }
        if ((max_motor_speed + turn_v_c) < (-m_v * f_v))
        {
            max_motor_speed = -m_v * f_v - turn_v_c;
        }

        if (max_speed_motor_num == 1)
            front_v_c = O_movement_analyze_m1_T(front_v_angle_c, max_motor_speed + turn_v_c);
        if (max_speed_motor_num == 2)
            front_v_c = O_movement_analyze_m2_T(front_v_angle_c, max_motor_speed + turn_v_c);
        if (max_speed_motor_num == 3)
            front_v_c = O_movement_analyze_m3_T(front_v_angle_c, max_motor_speed + turn_v_c);
    }
    // 合成的主逻辑
    // 分给t (t_v%)*m_v 分给f (f_v%)*m_v
    // 如果abs(t)<(t_v%)*m_v 就把剩下的(t_v%)*m_v-abs(t) 分给f  即当前的f可使用速度(f_v%)*m_v+((t_v%)*m_v-abs(t))
    // 如果abs(t)>(t_v%)*m_v									   即当前的f可使用速度(f_v%)*m_v+0
    // 如果abs(f)<(f_v%)*m_v 就把剩下的(f_v%)*m_v-abs(f) 分给t  即当前的t可使用速度(t_v%)*m_v+((f_v%)*m_v-abs(f))
    // 如果abs(f)>(f_v%)*m_v									   即当前的t可使用速度(t_v%)*m_v+0
    float t_add = 0; // 转动分量 附加 平移剩余功率
    float f_add = 0; // 平移分量 附加 转动剩余功率
    if (turn_v_c < (t_v * m_v))
    {
        f_add = t_v * m_v - abs(turn_v_c);
    }
    else
    {
        f_add = 0;
    }
    if ((max_motor_speed + turn_v_c) < (m_v * f_v))
    {
        t_add = t_v * m_v - abs(max_motor_speed + turn_v_c);
    }
    else
    {
        t_add = 0;
    }

    if (turn_v_c > (m_v * t_v + t_add))
    {
        turn_v_c = m_v * t_v + t_add;
    }
    if (turn_v_c < (-(m_v * t_v + t_add)))
    {
        turn_v_c = -(m_v * t_v + t_add);
    }
    if ((max_motor_speed + turn_v_c) > (m_v * f_v + f_add))
    {
        max_motor_speed = (m_v * f_v + f_add) - turn_v_c;
    }
    if ((max_motor_speed + turn_v_c) < (-(m_v * f_v + f_add)))
    {
        max_motor_speed = -(m_v * f_v + f_add) - turn_v_c;
    }
    if (max_speed_motor_num == 1)
        front_v_c = O_movement_analyze_m1_T(front_v_angle_c, max_motor_speed + turn_v_c);
    if (max_speed_motor_num == 2)
        front_v_c = O_movement_analyze_m2_T(front_v_angle_c, max_motor_speed + turn_v_c);
    if (max_speed_motor_num == 3)
        front_v_c = O_movement_analyze_m3_T(front_v_angle_c, max_motor_speed + turn_v_c);

    /*	if (front_v_c>(m_v*f_v))
        {	front_v_c= m_v*f_v;
        }
        if (front_v_c<(-m_v*f_v))
        {	front_v_c= -m_v*f_v;
        }
    */
    return (max_speed_motor_num); // 知道返回0是错误
}

// 最后输出经过 合成处理的 三电机轮速 全向底盘
void CMovement2Control::movement_output_O(float *motor1, float *motor2, float *motor3)
{
    O_movement_analyze(front_v_angle_c, front_v_c, motor1, motor2, motor3); // 全向解析
    *motor1 = *motor1 - turn_v_c;
    *motor2 = *motor2 - turn_v_c;
    *motor3 = *motor3 - turn_v_c;
    turn_v_p = turn_v_c;               // 存上个周期的值
    front_v_p = front_v_c;             // 存上个周期的值
    front_v_angle_p = front_v_angle_c; // 存上个周期的值
}

// 直角坐标到极坐标转换函数 角度
double CMovement2Control::coordinate_transfer_angle(double x, double y)
{
    double angle;
    angle = 180 * atan2(-y, x) / 3.14;
    return (angle);
}
// 直角坐标到极坐标转换函数 半径
double CMovement2Control::coordinate_transfer_radius(double x, double y)
{
    double radiu;
    radiu = sqrt(x * x + y * y);
    return (radiu);
}
// 极坐标到直角坐标转换函数 角度
double CMovement2Control::coordinate_transfer_x(double angle, double radius)
{
    double x;
    x = cos(angle * 3.14 / 180) * radius;
    return (x);
}
// 极坐标到直角坐标转换函数 半径
double CMovement2Control::coordinate_transfer_y(double angle, double radius)
{
    double y;
    y = -sin(angle * 3.14 / 180) * radius;
    return (y);
}
// 坐标旋转n度 转换函数 极坐标
float CMovement2Control::angle_add_transfer(float angle, float n)
{ // 简单的思路 想的方法 一个角度+-360度绝对方向不变
    float aa;
    aa = angle + n;
    if (aa > 180)
        aa = aa - 360;
    if (aa < -180)
        aa = aa + 360;
    return (aa);
}

// 全向正解析
void CMovement2Control::O_movement_analyze(float move_r_angle, float move_r_speed, float *motor1, float *motor2,
                                           float *motor3)
{
    *motor1 = (move_r_speed)*cos((move_r_angle + 150.0) / 180 * 3.14);
    *motor2 = (move_r_speed)*cos((move_r_angle - 90.0) / 180 * 3.14);
    *motor3 = (move_r_speed)*cos((move_r_angle + 30.0) / 180 * 3.14);
}
float CMovement2Control::O_movement_analyze_m1(float move_r_angle, float move_r_speed)
{
    float motor1 = (move_r_speed)*cos((move_r_angle + 150.0) / 180 * 3.14);
    return (motor1);
}
float CMovement2Control::O_movement_analyze_m2(float move_r_angle, float move_r_speed)
{
    float motor2 = (move_r_speed)*cos((move_r_angle - 90.0) / 180 * 3.14);
    return (motor2);
}
float CMovement2Control::O_movement_analyze_m3(float move_r_angle, float move_r_speed)
{
    float motor3 = (move_r_speed)*cos((move_r_angle + 30.0) / 180 * 3.14);
    return (motor3);
}
// 全向反解析
float CMovement2Control::O_movement_analyze_m1_T(float move_r_angle, float motor1)
{
    return (motor1 / (cos((move_r_angle + 150.0) / 180 * 3.14)));
}
float CMovement2Control::O_movement_analyze_m2_T(float move_r_angle, float motor2)
{
    return (motor2 / (cos((move_r_angle - 90.0) / 180 * 3.14)));
}
float CMovement2Control::O_movement_analyze_m3_T(float move_r_angle, float motor3)
{
    return (motor3 / (cos((move_r_angle + 30.0) / 180 * 3.14)));
}

// 输出平移量向量 极坐标系
void CMovement2Control::translate_v_vector_O(float *angle, float *speed)
{
    *angle = front_v_angle_c;
    *speed = front_v_c;
}

// 输出旋转量
void CMovement2Control::turn_w_O(float *w) // 输出旋转量
{
    *w = turn_v_c;
}

// 将 cs板全向运动输出数值 换算到博创 机器的 输出格式 和范围
void CMovement2Control::uptech_output_O(float *mtb, float *mtl, float *mtr)
{
    float motor1, motor2, motor3;
    movement_output_O(&motor1, &motor2, &motor3);
    float ma, mv, mva;
    // 运动反解 验证调试程序
    // O_movement_T_analyze(&ma,&mv,&mva,motor1,motor2,motor3);
    // front_v_angle_c=front_v_angle_c;
    // front_v_c=front_v_c;
    // turn_v_c=turn_v_c;

    // 下部分常量代码 定义放在Main_Logic.h关于USE_UP_BASE的定义部分
    // 线速度[米/秒]
    // 转速	[转/秒]	值1000=1[转/秒]
    // PI=3.1415926
    // r		[米]	轮子直径0.05[米]

    // 线速度[米/秒]
    // 转速	[转/秒]	值1000=1[转/秒]
    // PI=3.1415926
    // r		[米]	轮子直径0.05[米]
    //	const float wheel_linear_speed_ratio=1000*(1/(2*3.1415926*0.05));//线速度=转速*2*PI*r	转速=线速度/(2*PI*r)
    // 轮子到机体中心距离0.22米
    // 旋转1圈/秒	2*PI*r=单轮子线速度
    //	const float wheel_rotate_speed_ratio=(2*3.1415926*0.22)*wheel_linear_speed_ratio;
    //	const float max_wheel_linear_speed =3;// (m/s)最大单轮线速度	注:对应128输入 线速度2m/s   128/2=motor1/out

    //	float wheel_linear_speed_ratio=1000*(1/(2*3.1415926*0.05));//线速度=转速*2*PI*r	转速=线速度/(2*PI*r)
    // 轮子到机体中心距离0.22米
    // 旋转1圈/秒	2*PI*r=单轮子线速度
    //	float wheel_rotate_speed_ratio=(2*3.1415926*0.22)*wheel_linear_speed_ratio;
    //	float max_wheel_linear_speed =3;// (m/s)最大单轮线速度	注:对应128输入 线速度2m/s   128/2=motor1/out
    float m1_linear_speed, m2_linear_speed, m3_linear_speed;
    motor1 = -motor1;
    motor2 = -motor2;
    motor3 = -motor3;
    m1_linear_speed = (motor1 / 128 * max_wheel_linear_speed) * wheel_linear_speed_ratio;
    m2_linear_speed = (motor2 / 128 * max_wheel_linear_speed) * wheel_linear_speed_ratio;
    m3_linear_speed = (motor3 / 128 * max_wheel_linear_speed) * wheel_linear_speed_ratio;
    // lpLogic->SetFourMotorsSpeed(motor2,motor1,motor3,0);
    // lpLogic->SetFourMotorsSpeed(m2_linear_speed,m1_linear_speed,m3_linear_speed,0);
    *mtb = m2_linear_speed;
    *mtl = m1_linear_speed;
    *mtr = m3_linear_speed;
    //		m_Cmd.SetFourMotorsSpeed(mtb,mtl,mtr,0);

    // 运动反解 验证调试程序
    // O_movement_T_analyze(&ma,&mv,&mva,m1_linear_speed,m2_linear_speed,m3_linear_speed);
    // front_v_angle_c=front_v_angle_c;
    // front_v_c=front_v_c;
    // turn_v_c=turn_v_c;

    float one_o_maxA = 400;
    if (0)
    {
        ///*
        float mtb_a; // 此周期 电机加速度
        float mtl_a;
        float mtr_a;

        mtb_a = *mtb - mtb_p;
        mtl_a = *mtl - mtl_p;
        mtr_a = *mtr - mtr_p;

        float maxA;    // 绝对值最大的加速度
        float maxA_m;  // 最大加速度的电机  的速度
        float maxA_mp; // 最大加速度的电机  的上次的速度

        if (abs(mtb_a) > abs(mtl_a))
        {
            maxA = mtb_a;
            maxA_m = *mtb;
            maxA_mp = mtb_p;
        }
        else
        {
            maxA = mtl_a;
            maxA_m = *mtl;
            maxA_mp = mtl_p;
        }
        if (abs(maxA) < abs(mtr_a))
        {
            maxA = mtr_a;
            maxA_m = *mtr;
            maxA_mp = mtr_p;
        }
        // 100 200 300
        // 150 260 370
        //  50  60  70
        //			+60
        // xxx	xxx 360
        float tempp;
        tempp = maxA;
        if (abs(maxA) > one_o_maxA) // 限制最大加速度为one_o_maxA
        {
            if (tempp > one_o_maxA)
                tempp = one_o_maxA;
            if (tempp < -one_o_maxA)
                tempp = -one_o_maxA;

            mtb_a = abs(tempp) * mtb_a / abs(maxA);
            mtl_a = abs(tempp) * mtl_a / abs(maxA);
            mtr_a = abs(tempp) * mtr_a / abs(maxA);
        }
        *mtl = float(mtl_p + mtl_a);
        *mtr = float(mtr_p + mtr_a);
        *mtb = float(mtb_p + mtb_a);
        mtl_p = *mtl;
        mtr_p = *mtr;
        mtb_p = *mtb;
        //*/
    }
}

bool CMovement2Control::O_movement_T_analyze(float *move_v_angle, float *move_v_speed, float *turn_v, float motor1,
                                             float motor2, float motor3) // 全向解析
{                                                                        // l,b,r=>123
    /**/
    float temp_v_a, temp_v_s;
    temp_v_a = vector_add_angle(-30, motor1, -150, motor3);
    temp_v_s = vector_add_length(-30, motor1, -150, motor3);
    *move_v_angle = vector_add_angle(temp_v_a, temp_v_s, 90, motor2); //-270-270
    *move_v_speed = vector_add_length(temp_v_a, temp_v_s, 90, motor2);

    *turn_v = -(sin((float(angle_add_transfer((*move_v_angle), -60))) * 3.1416 / 180) * (*move_v_speed) * 0.66667);
    *turn_v = (*turn_v) - motor1;
    *move_v_speed = *move_v_speed * 0.66667;
    *move_v_angle = angle_add_transfer(-*move_v_angle, -180);
    return (1);

    // 运动反解
    // ———————————————————————————————————————————————————————
    // 会出现 平移角度反解输出无效的 特殊情况
    //	(m2-m1)*0.5 -(m3-m2)*0.5  -(m1-m3)*1 =0
    //   m2/2 -m1/2  -m3/2 +m2/2   -m1 +m3    =0
    //   m2/2 +m2/2  -m1/2 -m1     -m3/2 +m3  =0
    //   m2          -m1*1.5       +m3*0.5    =0
    // 可以看出上式 m1=m2=m3 时即只有旋转 无平移 属于特殊情况
    //			   m1=0  m2=-m3*0.5  时此动作无效 属于运动正解时不会出现的状态（一个轴 0  一个电机1  一个电机2）

    double m1, m2, m3;
    double move_r_speed = 11.111;
    double move_r_angle = 9.1111;
    double turn_v_c = 8.111;
    double move_angle_O; // 平移角度反解输出
    double move_speed_O; // 平移速度反解输出
    double turn_speed_O; // 旋转速度反解输出
    m1 = motor1;         //(move_r_speed)*cos((move_r_angle+ 150.0)/180*3.14);
    m2 = motor2;         //(move_r_speed)*cos((move_r_angle-  90.0)/180*3.14);
    m3 = motor3;         //(move_r_speed)*cos((move_r_angle+  30.0)/180*3.14);
    //	m1= m1-turn_v_c;
    //	m2= m2-turn_v_c;
    //	m3= m3-turn_v_c;
    if ((m1 == m2) && (m1 == m3)) // 注意特殊情况无需平移反解    推导： 当m2-m1=m3-m2时  ((m2-m1)*sin( 30.0/180*3.14)
                                  // +(m3-m2)*sin(150.0/180*3.14)==0   当m1=m3时(m1-m3)*sin(-  90.0/180*3.14))==0
    {
        move_angle_O = 0;   // 平移角度反解输出 清零
        move_speed_O = 0;   // 平移速度反解输出 清零
        turn_speed_O = -m1; // 旋转速度反解输出 清零
        *move_v_angle = move_angle_O;
        *move_v_speed = move_speed_O;
        *turn_v = turn_speed_O;
        return (1); // return false;
    }

    double tempsin = ((m2 - m1) * sin(30.0 / 180 * 3.14) + (m3 - m2) * sin(150.0 / 180 * 3.14) +
                      (m1 - m3) * sin(-90.0 / 180 * 3.14));
    double temptan;
    if (tempsin != 0)
    {
        temptan = ((m2 - m1) * cos(30.0 / 180 * 3.14) + (m3 - m2) * cos(150.0 / 180 * 3.14) +
                   (m1 - m3) * cos(-90.0 / 180 * 3.14)) /
                  tempsin;
    }
    else
    {
        temptan = 0;
        move_angle_O = 0; // 平移角度反解输出 清零
        move_speed_O = 0; // 平移速度反解输出 清零
        turn_speed_O = 0; // 旋转速度反解输出 清零
        *move_v_angle = move_angle_O;
        *move_v_speed = move_speed_O;
        *turn_v = turn_speed_O;
        return false;
    }
    move_angle_O = atan(temptan) / 3.14 * 180; // 平移角度的反解
    // m1=move_speed_O*cos((move_angle_O+ 150.0)/180*3.14)-turn_v_O;
    // m2=move_speed_O*cos((move_angle_O-  90.0)/180*3.14)-turn_v_O;
    // m3=move_speed_O*cos((move_angle_O+  30.0)/180*3.14)-turn_v_O;

    // m1-m2 = move_speed_O*cos((move_angle_O+ 150.0)/180*3.14)-move_speed_O*cos((move_angle_O-  90.0)/180*3.14);
    // 平移速度的反解做 分母 防止为0的 验证如果为零就换另一组
    if ((cos((move_angle_O + 150.0) / 180 * 3.14) - cos((move_angle_O - 90.0) / 180 * 3.14)) != 0)
    {
        move_speed_O = (m1 - m2) / (cos((move_angle_O + 150.0) / 180 * 3.14) - cos((move_angle_O - 90.0) / 180 * 3.14));
        turn_speed_O = (m1 * cos((move_angle_O - 90.0) / 180 * 3.14) - m2 * cos((move_angle_O + 150.0) / 180 * 3.14)) /
                       (cos((move_angle_O + 150.0) / 180 * 3.14) - cos((move_angle_O - 90.0) / 180 * 3.14));
    }
    else if ((cos((move_angle_O - 90.0) / 180 * 3.14) - cos((move_angle_O + 30.0) / 180 * 3.14)) != 0)
    {
        move_speed_O = (m2 - m3) / (cos((move_angle_O - 90.0) / 180 * 3.14) - cos((move_angle_O + 30.0) / 180 * 3.14));
        turn_speed_O = (m2 * cos((move_angle_O + 30.0) / 180 * 3.14) - m3 * cos((move_angle_O - 90.0) / 180 * 3.14)) /
                       (cos((move_angle_O - 90.0) / 180 * 3.14) - cos((move_angle_O + 30.0) / 180 * 3.14));
    }
    else if ((cos((move_angle_O + 30.0) / 180 * 3.14) - cos((move_angle_O + 150.0) / 180 * 3.14)) != 0)
    {
        move_speed_O = (m3 - m1) / (cos((move_angle_O + 30.0) / 180 * 3.14) - cos((move_angle_O + 150.0) / 180 * 3.14));
        turn_speed_O = (m3 * cos((move_angle_O + 150.0) / 180 * 3.14) - m1 * cos((move_angle_O + 30.0) / 180 * 3.14)) /
                       (cos((move_angle_O + 30.0) / 180 * 3.14) - cos((move_angle_O + 150.0) / 180 * 3.14));
    }
    else
    {
        move_angle_O = 0; // 平移角度反解输出 清零
        move_speed_O = 0; // 平移速度反解输出 清零
        turn_speed_O = 0; // 旋转速度反解输出 清零
        *move_v_angle = move_angle_O;
        *move_v_speed = move_speed_O;
        *turn_v = turn_speed_O;
        return false;
    }
    move_speed_O = move_speed_O;
    turn_speed_O = turn_speed_O;
    *move_v_angle = move_angle_O;
    *move_v_speed = move_speed_O;
    *turn_v = turn_speed_O;
    // m1=move_speed_O*cos((move_angle_O+ 150.0)/180*3.14)-turn_v_O;
    // m2=move_speed_O*cos((move_angle_O-  90.0)/180*3.14)-turn_v_O;
    // m3=move_speed_O*cos((move_angle_O+  30.0)/180*3.14)-turn_v_O;

    //(m1+turn_v_O)/cos((move_angle_O+ 150.0)/180*3.14)=move_speed_O;
    // m2+turn_v_O=move_speed_O*cos((move_angle_O-  90.0)/180*3.14);
    // m3+turn_v_O=move_speed_O*cos((move_angle_O+  30.0)/180*3.14);
    //(m1+turn_v_O)*cos((move_angle_O-  90.0)/180*3.14)=(m2+turn_v_O)*cos((move_angle_O+ 150.0)/180*3.14)
    // turn_v_O*cos((move_angle_O-  90.0)/180*3.14) +m1*cos((move_angle_O-  90.0)/180*3.14) =m2*cos((move_angle_O+
    // 150.0)/180*3.14)  +turn_v_O*cos((move_angle_O+ 150.0)/180*3.14)
    // turn_v_O*(cos((move_angle_O-  90.0)/180*3.14)-cos((move_angle_O+ 150.0)/180*3.14))   =m2*cos((move_angle_O+
    // 150.0)/180*3.14)-m1*cos((move_angle_O-  90.0)/180*3.14) 	turn_speed_O = (m1*cos((move_angle_O-  90.0)/180*3.14)
    //-m2*cos((move_angle_O+ 150.0)/180*3.14))  /  (cos((move_angle_O+
    // 150.0)/180*3.14)-cos((move_angle_O-  90.0)/180*3.14));

    // 运动反解完成
    // ———————————————————————————————————————————————————————
}

float CMovement2Control::Moto_movement_T_analyze_front_v(float motorL, float motorB, float motorR) // 全向解析
{
    float front_a, front_v, turn_w;
    O_movement_T_analyze(&front_a, &front_v, &turn_w, motorL, motorB,
                         motorR); //    *move_v_angle, *move_v_speed, *turn_v
    return (front_v * 0.005126);
}

float CMovement2Control::Moto_movement_T_analyze_front_v_angle(float motorL, float motorB, float motorR) // 全向解析
{
    float front_a, front_v, turn_w;
    O_movement_T_analyze(&front_a, &front_v, &turn_w, motorL, motorB,
                         motorR); //    *move_v_angle, *move_v_speed, *turn_v
    return (angle_add_transfer(front_a, -180));
}

float CMovement2Control::Moto_movement_T_analyze_turn_v(float motorL, float motorB, float motorR) // 全向解析
{
    float front_a, front_v, turn_w;
    O_movement_T_analyze(&front_a, &front_v, &turn_w, motorL, motorB,
                         motorR); //    *move_v_angle, *move_v_speed, *turn_v
    return (-turn_w * 0.005126);
}

bool CMovement2Control::Moto_to_P_movement(float motorL, float motorB, float motorR) // 全向解析
{
    float front_a, front_v, turn_w;
    O_movement_T_analyze(&front_a, &front_v, &turn_w, motorL, motorB,
                         motorR); //    *move_v_angle, *move_v_speed, *turn_v
    front_v_p = front_v * 0.005126;
    front_v_angle_p = angle_add_transfer(front_a, -180);
    turn_v_p = -turn_w * 0.005126;
    return (1);
}
