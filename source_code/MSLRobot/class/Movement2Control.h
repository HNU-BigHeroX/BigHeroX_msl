// Movement2Control.h
// 注意：当运动跟踪目标切换时 原来的积分量速度记录 等还在保存 会引起初运动不正常

#ifndef Movement2Control_h
#define Movement2Control_h

#include "ObjPropertiesStruct.h"

//	#define mirror_center_x 160//镜头中心位置
//	#define mirror_center_y 120//镜头中心位置

// 线速度[米/秒]
// 转速	[转/秒]	值2000=1[转/秒]
// PI=3.1415926
// r		[米]	轮子直径0.05[米]
//	const float wheel_linear_speed_ratio=2000*(1/(2*3.1415926*0.05));//线速度=转速*2*PI*r	转速=线速度/(2*PI*r)
// 轮子到机体中心距离0.22米
// 旋转1圈/秒	2*PI*r=单轮子线速度
//	const float wheel_rotate_speed_ratio=(2*3.1415926*0.22)*wheel_linear_speed_ratio;
//	const float max_wheel_linear_speed = 12.5 *2000.0 / wheel_linear_speed_ratio ;// (m/s)最大单轮线速度
// 注:对应128输入 线速度2m/s   128/2=motor1/out

class CMovement2Control
{
  public:
    obj_properties target_h[20];                                       // 跟踪目标的历史记录  要求调用程序每周期更新
    void save_target_history(float angle, float radius, float weight); // 更新历史记录函数  要求调用程序每周期更新

  public:
    CMovement2Control(); // 创建构造函数

  protected:
    // public:
  public:
    //	float t_a_h[20];//目标角度历史记录
#define INTEGRAL_ANGLE_s_periods_limit 2 // 10//30
#define INTEGRAL_ANGLE_SATURATION 14     // 56//128
    float p_quotiety;                    // 比例项系数proportion
    float i_quotiety;                    // 积分项系数integral
    float integral;                      // 积分项
    float d_quotiety;                    // 微分项系数differential coefficient
    float diff_inte;                     // 微分项积分量 这里的微分是把微分项累加出来的
    float dead_zone;                     // 电机功率死区

    float acceleration_limit; // 角加速度最大值控制
    float velocity_limit;     // 角速度最大值控制
    //	float velocity;				//角计算速度
    // float benchmark_increase_limit;//因为机器有转动惯量等问题 对准同一个目标时突然变换不同的对准基准会出现
    // 电机瞬时加减速 不利于机体运行平稳硬件保护 设置此项 用于限定benchmark值每次调用增长量最大值
    // float benchmark_t;//benchmark上次进入函数基准的

    float p_result, i_result, d_result;
    void initializing_angle(void);
    void parameter_set_angle(float p_quotiety_, float i_quotiety_, float d_quotiety_, float dead_zone_);
    void limit_parameter_set_angle(float velocity_limit_, float acceleration_limit_);
    float zs_angle_track_001(float target_angle, float benchmark, float *DD, char *LorRout);
    float zs_angle_track_001(float target_angle, float benchmark, char *LorRout);
    float zs_angle_track_002(float target_angle, float benchmark);
    float turn_v_c;             // 当前次角度 跟踪 输出量大小
    float turn_v_p;             // 前一次角度 跟踪 输出量大小
    void turn_v_maxlimit(void); // 限定角  [功率]速度输出最大值
    void turn_a_maxlimit(void); // 限定角  [功率]加速度最大值

  public:
    //	float t_d_h[20];//目标距离历史记录
    float translate_radius_h[20];       // 目标距离本机定点 矢量差分 距离的历史记录
    float translate_angle_h[20];        // 目标距离本机定点 矢量差分 角度的历史记录
#define INTEGRAL_DIS_s_periods_limit 20 // 30
#define INTEGRAL_DIS_SATURATION 90      // 64//128
    float p_quotiety_dis;               // 比例项系数proportion
    float i_quotiety_dis;               // 积分项系数integral
    float integral_dis;                 // 积分项
    float d_quotiety_dis;               // 微分项系数differential coefficient
    float diff_inte_dis;                // 微分项积分量 这里的微分是把微分项累加出来的
    float dead_zone_dis;                // 电机功率死区
    float acceleration_limit_dis;       // 加速度最大值控制
    float velocity_limit_dis;           // 速度最大值控制
    //	float velocity_dis;//计算速度

    float p_result_dis, i_result_dis, d_result_dis;
    float p_result_dis_a, i_result_dis_a, d_result_dis_a;
    float p_result_dis_r, i_result_dis_r, d_result_dis_r;
    void initializing_dis(void);
    void parameter_set_dis(float p_quotiety_, float i_quotiety_, float d_quotiety_, float dead_zone_);
    void limit_parameter_set_dis(float velocity_limit_dis_, float acceleration_limit_dis_);
    // float zs_genzongjuli_001(float r,float endr,float a);//跟踪目标的 本机坐标  半径距离
    //	float zs_point_track_001(float target_angle,float target_radius,float benchmark_angle,float benchmark_radius,int
    //*DD,char *LorRout); 有错误 现在计算的不是差分 而是差分的差分  [加速度]!!!!!!!!!!
    float zs_point_track_01_001(float target_angle, float target_radius, float benchmark_angle,
                                float benchmark_radius); // 内部保存历史记录 内部操作全部改为向量操作版本

    float zs_point_track_01_002(float target_angle, float target_radius, float benchmark_angle,
                                float benchmark_radius); // 外部保存历史记录
    float front_v_c;                                     // 当前次平移 跟踪 输出量大小
    float front_v_p;                                     // 前一次平移 跟踪 输出量大小
    float front_v_angle_c;                               // 当前次平移      输出量方向
    float front_v_angle_p;                               // 前一次平移      输出量方向
    void front_v_maxlimit(void);                         // 限定平移[功率]速度输出最大值
    void front_a_maxlimit(void);                         // 限定平移[功率]加速度最大值

  public:
    void initializing_movement(float turn, float front, float angle); // 公用的数值初始化
    void translate_v_vector_O(float *angle, float *speed);            // 输出平移量向量 极坐标系
    void turn_w_O(float *w);                                          // 输出旋转量

    void zs_movement_compound(void); // 角速度 优先合成角速度平移速度两个分量

    // 还没做
    // 将当前运动向量 在输入方向上的投影分量输出
    //	float vector_projective(float angle,float v);

    // 还没做	void zs_movement_compound(float);//比例合成角速度平移速度两个分量
    //	void movement_output(float *Lmoto,float *Rmoto);//最后输出经过 合成处理的 左右轮速

    void zs_translate_compound_O(float angle, float speed); // 将当前运动向量与  另一个平移分量相加
    // 将两个向量	相加	得到结果向量的角度
    float vector_add_angle(float angle1, float length1, float angle2, float length2);
    // 将两个向量	相加	得到结果向量的长度
    float vector_add_length(float angle1, float length1, float angle2, float length2);
    char zs_movement_compound_O(); //
    char zs_movement_compound_O(float t_v, float f_v, float m_v);
    void movement_output_O(float *motor1, float *motor2, float *motor3); // 最后输出经过 合成处理的 三电机轮速 全向底盘
    void uptech_output_O(float *mtb, float *mtl, float *mtr);
    float mtb_p; // 上一周期 电机速度
    float mtl_p;
    float mtr_p;

    double coordinate_transfer_angle(double x, double y);
    double coordinate_transfer_radius(double x, double y);
    double coordinate_transfer_x(double angle, double radius);
    double coordinate_transfer_y(double angle, double radius);
    float angle_add_transfer(float angle, float n); // 两个角度 相加
    void O_movement_analyze(float move_r_angle, float move_r_speed, float *motor1, float *motor2,
                            float *motor3);                              // 全向解析
    float O_movement_analyze_m1(float move_r_angle, float move_r_speed); // 1电机分速度
    float O_movement_analyze_m2(float move_r_angle, float move_r_speed); // 2电机分速度
    float O_movement_analyze_m3(float move_r_angle, float move_r_speed); // 3电机分速度
    // 全向反解析
    float O_movement_analyze_m1_T(float move_r_angle, float motor1);
    float O_movement_analyze_m2_T(float move_r_angle, float motor2);
    float O_movement_analyze_m3_T(float move_r_angle, float motor3);

    bool O_movement_T_analyze(float *move_v_angle, float *move_v_speed, float *turn_v, float motor1, float motor2,
                              float motor3); // 全向解析

    float Moto_movement_T_analyze_front_v(float motorL, float motorB,
                                          float motorR); // 全向解析 输入上周期电机速度 得到上周期平速度
    float Moto_movement_T_analyze_front_v_angle(float motorL, float motorB,
                                                float motorR); // 全向解析 输入上周期电机速度 得到上周期平移角度
    float Moto_movement_T_analyze_turn_v(float motorL, float motorB,
                                         float motorR); // 全向解析  输入上周期电机速度 得到上周期转动速度
    bool Moto_to_P_movement(float motorL, float motorB,
                            float motorR); // 全向解析 输入上周期电机速度 得到上周期平移三分量
};

#endif Movement2Control_h

/*
        float benchmark_angle=0;
        lpLogic->moveControl.initializing_movement(0,0,0);//(float turn,float front,float angle)
设front_v_c	front_v_angle	turn_v_c初值
        lpLogic->moveControl.zs_angle_track_001(angle_ball,benchmark_angle,h_ball,black_white_ball);
设turn_v_c重设
        lpLogic->moveControl.zs_distance_track_001(angle_ball,radius_ball,60,h_disball,black_white_ball);
设front_v_c front_v_angle重设
        lpLogic->moveControl.zs_translate_compound_O(90,40);//将当前运动向量与  另一个平移分量相加
(front_v_c front_v_angle) 与 (90度,40平移量) 求和  (front_v_c front_v_angle)重设
        lpLogic->moveControl.front_v_maxlimit();//限定平移[功率]速度输出最大值
front_v_c 限定最大值 front_v_c重设
        lpLogic->moveControl.front_a_maxlimit();//限定平移[功率]加速度最大值
front_v_c 限定加速度 front_v_c重设
        lpLogic->moveControl.turn_v_maxlimit();//限定平移[功率]速度输出最大值
turn_v_c 限定最大值 front_v_c重设
        lpLogic->moveControl.turn_a_maxlimit();//限定平移[功率]加速度最大值
turn_v_c 限定加速度 front_v_c重设
        lpLogic->moveControl.zs_movement_compound_O();//运动叠加
front_v_c	front_v_angle	turn_v_c合并
front_v_c	turn_v_c 重设
        lpLogic->moveControl.movement_output_O(&motor1,&motor2,&motor3);//最后输出经过 合成处理的 三电机轮速 全向底盘
*/
