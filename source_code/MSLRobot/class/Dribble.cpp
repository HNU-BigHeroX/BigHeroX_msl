// Dribble.cpp

#include "stdafx.h"
#include "Dribble.h"
#include "math.h"
#include "math2.h"

Dribble::Dribble()
{
    Dis_Location = 0; // 路程定位在 定位中占的影响 数值0~2 浮点数 0为完全不使用路程定位 2为完全使用路程定位  默认为0
                      // 比较合适的使用系数1.8
    robot_L_test.f_x = 0;
    robot_L_test.f_y = 0;
    robot_L_test.f_a = 0;
    fieldzoomtime = FieldZoomTime;
    display = 1; // 默认显示所有附加信息
    G_ball_int_02();
    G_ball_int_03(); // 带球前进量的角度
    Distance_Location_init();
    motor_a_limit_init();
}

// 前景摄像机 处理增加函数 做 rgb2hsv 球的二值化 重心计算
void Dribble::AfterUpdateVideoSample_add(BYTE *pBuffer, long lWidth, long lHeight, int bhs, int bhx, int bss, int bsx,
                                         int bvs, int bvx)
{ // return;
  // m_find2->ImageBuf(lWidth,lHeight,pBuffer);
    //	int bhs,int bhx,int bss,int bsx,int bvs,int bvx
    register int y_bhs = bhs; // h上限
    register int y_bhx = bhx; // h下限
    register int y_bs = bsx;  // s下限
    // r,g,b
    // h_u d   s_d     v_
    register float OBJ_temp_b = 0;
    register float OBJ_temp_b_ = 0;
    register float OBJ_temp_c = 0;
    register float H = 0;
    register float S = 0;
    register float V = 0;
    register int r, g, b, Vmin;
    register int hminVI;
    register int hmaxVI;
    register int sminVI;
    register int smaxVI;
    register int vminVI;
    register int vmaxVI;
    register unsigned long int i;
    register unsigned long int data_long = lWidth * lHeight * 3;

    i = 0;
    if (y_bhs > y_bhx) // 切换红色 跨180度问题
    {                  // if (volh<zz->bhs && volh>zz->bhx && vols>zz->bs)

        while (i < data_long)
        {

            b = pBuffer[i];
            g = pBuffer[i + 1];
            r = pBuffer[i + 2];
            if (b > g)
            {
                if (r > b)
                {
                    V = r;
                    Vmin = g; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=g//A=0;B=(g-b);
                    if (V != Vmin)
                    {
                        H = 360 + (g - b) * 60 / (V - Vmin); // 0*60
                    }
                    else
                        H = 0;
                }
                else // V=b   Vmin=?
                {
                    if (r < g)
                    {
                        V = b;
                        Vmin = r; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=r//A=4;B=(r-g);
                        if (V != Vmin)
                            H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                        else
                            H = 0;
                    }
                    else
                    {
                        V = b;
                        Vmin = g; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=g//A=4;B=(r-g);
                        if (V != Vmin)
                            H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                        else
                            H = 0;
                    }
                }
            }
            else
            {
                if (r > g)
                {
                    V = r;
                    Vmin = b; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=b//A=0;B=(g-b);
                    if (V != Vmin)
                    {
                        H = 0 + (g - b) * 60 / (V - Vmin); // 0*60
                        if (H < 0)
                        {
                            H = H + 255; // H=H+360;
                        }
                    }
                    else
                        H = 0;
                }
                else // V=g   Vmin=?
                {
                    if (r < b)
                    {
                        V = g;
                        Vmin = r; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=r//A=2;B=(b-r);
                        if (V != Vmin)
                            H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                        else
                            H = 0;
                    }
                    else
                    {
                        V = g;
                        Vmin = b; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=b//A=2;B=(b-r);
                        if (V != Vmin)
                            H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                        else
                            H = 0;
                    }
                }
            }
            S = V - Vmin; //*255/V;    //这里改了 原来是S=(V-Vmin)*255/V

            if (H < y_bhs && H > y_bhx && S > y_bs)
            {
                //					if (		  (hminVI<H)&&(H<hmaxVI))
                //						{if	(	  (sminVI<S)&&(S<smaxVI))
                //							{if ( (vminVI<V)&&(V<vmaxVI))
                //								{//	Bool_Data[j]=1;
                OBJ_temp_c++;
                OBJ_temp_b += i % 960 / 3;
                OBJ_temp_b_ += i / 960;
                //								}
                //							}
                //						}
            }
            i = i + 3; // j++;
        }
    }
    else
    { // if ((volh>zz->bhx || volh<zz->bhs )&& vols>zz->bs)

        while (i < data_long)
        {

            b = pBuffer[i];
            g = pBuffer[i + 1];
            r = pBuffer[i + 2];
            if (b > g)
            {
                if (r > b)
                {
                    V = r;
                    Vmin = g; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=g//A=0;B=(g-b);
                    if (V != Vmin)
                    {
                        H = 360 + (g - b) * 60 / (V - Vmin); // 0*60
                    }
                    else
                        H = 0;
                }
                else // V=b   Vmin=?
                {
                    if (r < g)
                    {
                        V = b;
                        Vmin = r; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=r//A=4;B=(r-g);
                        if (V != Vmin)
                            H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                        else
                            H = 0;
                    }
                    else
                    {
                        V = b;
                        Vmin = g; //			hsvA=4;hsvB=(r-g);	//V=b   Vmin=g//A=4;B=(r-g);
                        if (V != Vmin)
                            H = 240 + (r - g) * 60 / (V - Vmin); // 4*60
                        else
                            H = 0;
                    }
                }
            }
            else
            {
                if (r > g)
                {
                    V = r;
                    Vmin = b; //			hsvA=0;hsvB=(g-b);	//V=r   Vmin=b//A=0;B=(g-b);
                    if (V != Vmin)
                    {
                        H = 0 + (g - b) * 60 / (V - Vmin); // 0*60
                        if (H < 0)
                        {
                            H = H + 255; // H=H+360;
                        }
                    }
                    else
                        H = 0;
                }
                else // V=g   Vmin=?
                {
                    if (r < b)
                    {
                        V = g;
                        Vmin = r; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=r//A=2;B=(b-r);
                        if (V != Vmin)
                            H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                        else
                            H = 0;
                    }
                    else
                    {
                        V = g;
                        Vmin = b; //			hsvA=2;hsvB=(b-r);	//V=g   Vmin=b//A=2;B=(b-r);
                        if (V != Vmin)
                            H = 120 + (b - r) * 60 / (V - Vmin); // 2*60
                        else
                            H = 0;
                    }
                }
            }
            S = V - Vmin; //*255/V;    //这里改了 原来是S=(V-Vmin)*255/V
            if ((H > y_bhx || H < y_bhs) && S > y_bs)
            // if (H<y_bhs && H>y_bhx && S>y_bs)
            {
                OBJ_temp_c++;
                OBJ_temp_b += i % 960 / 3;
                OBJ_temp_b_ += i / 960;
            }
            i = i + 3; // j++;
        }
    }

    if (OBJ_temp_c != 0) // 保险程序  当同步不佳时  有时Capture不到图象 就会出现所有象素都为零 这时做OBJ_temp_b /
                         // OBJ_temp_c的除法就会产生分母为零的错误
    {
        ball_op_Fx = OBJ_temp_b / OBJ_temp_c;  //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+320
        ball_op_Fy = OBJ_temp_b_ / OBJ_temp_c; //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+240
    }
    ball_op_Fweight = OBJ_temp_c;          // 准备把象素重量传给主逻辑函数
    if (ball_op_Fweight > FHaveBallWeight) // 前景有球判定
        FHaveBall = 1;
    else
        FHaveBall = 0;
    // ball_op_Fx=20;
    // ball_op_Fy=41;

    if (display == 1)
    {
        int y, x;
        if (ball_op_Fweight != 0)
        {
            for (y = 0; y < lHeight; y++)
            {
                pBuffer[(lWidth * y + (int)ball_op_Fx) * 3] = 255;
            }
            for (x = 0; x < lWidth; x++)
            {
                pBuffer[(lWidth * (int)ball_op_Fy + x) * 3] = 255;
            }
        }
    }
    const float Xcenter = lWidth * 0.5;     // 前景中 机体中心的 像素位置
    const float Ycenter = -lHeight * 0.583; // 前景中 机体中心的 像素位置

    // 前景标定线程序
    if (0)
    {
        float r, a, x, y, n;
        for (r = 0; r < 1000; r += 20)
        {
            for (a = -180; a < 0; a++)
            {
                x = coordinate_transfer_x(a, r) + Xcenter;
                y = coordinate_transfer_y(a, r) + Ycenter;
                if ((y > 0) && (y < lHeight) && (x > 0) && (x < lWidth))
                    pBuffer[(lWidth * (int)y + (int)x) * 3] = 255;
            }
        }
    }
    ball_op_Fa = coordinate_transfer_angle(Xcenter - ball_op_Fx, -ball_op_Fy + Ycenter); // 直角坐标系到极坐标系转换
                                                                                         // 角度
    ball_op_Fr =
        coordinate_transfer_radius(Xcenter - ball_op_Fx, -ball_op_Fy + Ycenter); // 直角坐标系到极坐标系转换 半径
    ball_op_Fr = (ball_op_Fr - 14 * 18) * (57 - 38) / (18 * 18 - 14 * 18) + 38;
    ball_op_Fa = angle_add_transfer(ball_op_Fa, -90);
}

int Dribble::GetMouseX(void) // 得到 鼠标x坐标
{
    tagPOINT cursorXY; // 用于存储鼠标点的位置
    GetCursorPos(&cursorXY);
    return (cursorXY.x);
}
int Dribble::GetMouseY(void) // 得到 鼠标y坐标
{
    tagPOINT cursorXY; // 用于存储鼠标点的位置
    GetCursorPos(&cursorXY);
    return (cursorXY.y);
}

void Dribble::G_ball_int_02(void)
{
    Front_aC = 0; // 前进量的角度
    // G_ball_fv=0;
    // G_ball_fva=0;
    // G_ball_w=0;
}

void Dribble::G_ball_int_03(void)
{
    ball_op_Fx = 0;      // 前景球的x
    ball_op_Fy = 0;      // 前景球的y
    ball_op_Fa = 0;      // 前景球的angle
    ball_op_Fr = 0;      // 前景球的radius
    ball_op_Fweight = 0; // 准备把象素重量传给主逻辑函数
    FHaveBall = 0;       //

    // 全局参数声明1
    Gb_loop_time = 30;           // 33;//67;//49.5;//67;//120;//67;//56;//66.6;//单位毫秒 一周期需要的时间
    G_turn_wMAX = 1024 / 2 / 50; // 1024/2/40;//1024/2/30;//带球旋转角速度最大值
    ra_in_w_angle_P_ms = 0.3;    // 带球夹角超过 预定夹角 回收速度 单位  度/ms
    Dribble_WP = 1.826865672;    // 修正 带球转机身 球行进方向不变 的参数
    end_dbangle = 90;            //[经验值]front a c角度横90度不变//末状态前进量和0度的夹角	越接近90越稳准备时间越长
    // 注意还有G_ball_02 最后动作方向修正 （原离心力修正参数）

    // 全局参数声明2
    Dribble_ball_other_moment_flag = 1; // 先运行了其他程序再运行带球函数标志
    Gb_Vavg = 36;                       ////直线带球的平均速度 大于时要减小(在持球器中) 小于时要增大(进入带球程序)
    Gb_Vadd = 1.6;                      // 0.8;//带球到平均速度的加速度 小于时要增大(进入带球程序)
    Gb_Vred = 0.8;                      // 0.4;//带球到平均速度的加速度 大于时要减小(在持球器中)
    Gb_v_lest = 0.4;                    // 0.9;//0.2;//剩下的前进量速度 占总速度的百分数
    Gb_w_m = 0.38;                      // 最小转动速度 以下不需要减速 占总角速度的百分数
    // 注意如果有时间 让机器自己查找适应的减速点 在线调整  每次记录球减速位置平均...
    holdballFmin = 31.5; // 12.95;//12.7;//13.6;//13.9+0.2; //12.65;//299;//12.95;        //前景 球进入持球器底端最小值
    holdballFmin =
        34.0765 + 3; // 12.95;//12.7;//13.6;//13.9+0.2; //12.65;//299;//12.95;        //前景 球进入持球器底端最小值

    /* dis_yp=0;//计算球速度向量用
     dis_xp=0;
     dis_vyp=0;//计算机体平移速度向量用
     dis_vxp=0;

        //电机速度真值 估算值  (加入硬件单轴加速度限制 考虑)
         mbR=0;
         mlR=0;
         mrR=0;
    */
    fount_xVset = 0; // 设置速度
    fount_yVset = 0; // 设置速度
    fv = 0;          // 带球时 球的线速度
    fa = 0;          //
    robot_fv = 0;    // 机器的上周期的运动参数 （累加）
    robot_fva = 0;   // 机器的上周期的运动参数 （累加）
    robot_w = 0;     // 机器的上周期的运动参数 （累加）

    // 存储上个周期的行为号
    xingwei_P = 0;
}

void Dribble::Angle_add(float turnW_, float *Front_aC_) // 一个周期转动量 造成的转动角度
{
    float Ww = Angle_Speed(turnW_); // 换算从128制角速度 换算为 度/秒 单位的角速度
    *Front_aC_ = angle_add_transfer(*Front_aC_, -Ww * 0.001 * 360 * Dribble_WP *
                                                    Gb_loop_time); // 这里要根据 本桢时间 调整偏移量 因为角速度是不变的
}

// 生成带球 动作中 带球轨迹圆弧切线方向 和机体0度方向的夹角   end_dbangle为生成最大夹角 turn_w旋转角速度
// front_v球前进速度 Front_aC机体平移角度
float Dribble::Dribble_Angle(float end_dbangle, float turn_w, float front_v)
{
    // 25大~35小   90-40;//90-30;
    /*测试持球器的代码*/ // float end_dbangle=90;//front a c角度横90度不变//末状态前进量和0度的夹角
                         // 越接近90越稳准备时间越长
    float ap = 0;        // 0.65;//没电0.2;//系数控制姿态角度 和角速度的比值
    ap = fabs(turn_w * front_v / G_turn_wMAX / 24) * 0.8; // 24 0.8[经验值]
    float af = -(turn_w / G_turn_wMAX) * (end_dbangle)*ap;
    if (af > 90)
    {
        af = 90;
    }
    if (af < -90)
    {
        af = -90;
    }
    af = af * 10000.0;
    int aft = (int)af;         // 取小数点后4位
    af = (float)aft / 10000.0; // 取小数点后4位
    if (af > end_dbangle)      // 保险程序 平移量方向 不能超过限定最大值
    {
        af = end_dbangle;
    }
    if (af < -end_dbangle)
    {
        af = -end_dbangle;
    }
    //	float ab= af - Front_aC;
    return (af);
}

// 换算从128制角速度 换算为 度/秒 单位的角速度
float Dribble::Angle_Speed(float turn_w)
{
    double aa = 1 * max_wheel_linear_speed * wheel_linear_speed_ratio / (wheel_rotate_speed_ratio * 128);
    return (turn_w * aa);
}
// 换算从128制速度 换算为 米/秒 单位的速度
float Dribble::V_Speed(float V)
{
    double v = (1 / 128 * max_wheel_linear_speed) * wheel_linear_speed_ratio;
    return (V * v);
}

char Dribble::Dribble_Angle_Track(
    float turn_w, float af,
    float *F_AC) // 带球角度跟踪  用Front_aC 跟踪 Dribble_Angle  并判断是否需要做 带球直线旋转动作 如果是返回1
{
    // 见aa Ww
    float Ww1 = Angle_Speed(turn_w); // 计算转速实际值
    char ac = 0;
    //	float ra_in_w=3.2/10.0  *Gb_loop_time;//[经验值]0.4/10.0  *Gb_loop_time;// 0.4 度 每10ms 的回程前进量变化速度
    // 0.04度/毫秒 ;//前进量向内回的速度
    float ra_in_w = ra_in_w_angle_P_ms * Gb_loop_time; //[经验值]0.4/10.0  *Gb_loop_time;// 0.4 度 每10ms
                                                       // 的回程前进量变化速度 0.04度/毫秒 ;//前进量向内回的速度

    if ((-turn_w > 0) && ((af - (*F_AC)) > 0) && ((*F_AC) > 0))
    {
        ac = 1;
    }
    if ((-turn_w <= 0) && ((af - (*F_AC)) <= 0) && ((*F_AC) <= 0))
    {
        ac = 1;
    }
    ///*
    if ((-turn_w > 0) && ((af - (*F_AC)) < 0) && ((*F_AC) > 0))
    {
        ac = -1;
        (*F_AC) -= ra_in_w; //
        if (af > 0)
        {
            if ((*F_AC) < af)
                (*F_AC) = af;
        }
        else
        {
            if ((*F_AC) > af)
                (*F_AC) = af;
        }
    }
    if ((-turn_w <= 0) && ((af - (*F_AC)) > 0) && ((*F_AC) <= 0))
    {
        ac = -1;
        (*F_AC) += ra_in_w; //
        if (af > 0)
        {
            if ((*F_AC) < af)
                (*F_AC) = af;
        }
        else
        {
            if ((*F_AC) > af)
                (*F_AC) = af;
        }
    }
    if ((-turn_w > 0) && ((*F_AC) <= 0))
    {
        if (ra_in_w < fabs(Ww1 * 0.006 * 360)) //-Ww*0.001*360*1.826865672*Gb_loop_time
        {
            ac = 1;
        }
        else
        {
            ac = -1;
            (*F_AC) += ra_in_w; //-0.4;//40/50
            if ((*F_AC) > 0)
                (*F_AC) = 0.0000001;
        }
    }
    if ((-turn_w <= 0) && ((*F_AC) > 0))
    {
        if (ra_in_w < fabs(Ww1 * 0.006 * 360)) //-Ww*0.001*360*1.826865672*Gb_loop_time
        {
            ac = 1;
        }
        else
        {
            ac = -1;
            (*F_AC) += -ra_in_w; // 0.4;
            if ((*F_AC) < 0)
                (*F_AC) = 0;
        }
    }

    (*F_AC) = angle_add_transfer((*F_AC), 0); // 这里要把角度放到+-180之间
    //*/
    return (ac);
}

// G_ball_02(fv	 ,G_turn_w,ball_op_Fa,ball_op_Fr,&motor1,&motor2,&motor3);//32
int Dribble::G_ball_02(float front_v, float turn_speed, float ball_angle, float ball_radius, float *m1, float *m2,
                       float *m3)
{ // front_v=0;turn_speed=0;

    // 实际使用 前进 参数数值
    double turn_w =
        turn_speed;    // 17;  //-18;//-20;//要求的用于旋转的每个轴的线速度<<<<<===================================
    Front_C = front_v; // 存储带球当中当前前进分量的大小  保证带球当中此量减小速度小于一个定值
    // float Front_aC = 0;	//前进量的角度
    //		float Front_S	= 50;		//前进量的大小
    turn_fv = 0;       // 平移速度大小		临时存储 要达到要求的旋转中心位置 需要附加的平移速度
    turn_afv = -90;    // 平移速度的方向    临时存储 要达到要求的旋转中心位置 需要附加的平移速度的方向
    float sign_fv = 0; // 记录turn_fv的正负号方向
    char black_white_ball[10];

    // float ball_op_weight;
    float ball_op_angle = ball_angle;
    float ball_op_radius = ball_radius;
    float motor1 = 0;
    float motor2 = 0;
    float motor3 = 0;

    move2Control.initializing_movement(0, 0, 0); //(float turn,float front,float angle)

    move2Control.limit_parameter_set_angle(21, 1); // 3(128,328);//float velocity_limit_,float acceleration_limit_
    move2Control.limit_parameter_set_dis(
        10, 5); //(15, 5);//(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
    float ax =
        0.51 * 4.8 * 0.14 * 0; //  0.5;//0.17;//0.51*4.8*0.09;//0.51*1.5;//1.2;//5;//0.45;//0.39;//角速度修正量 缩减系数
    float dx = 2.4 * 0.015 * 32; //*0  2.4*0.015 *8;//1;//0.80;//1;//2.8;			//平移修正量  缩减系数
    float angle_p_1 = 0.35 * ax; //;
    float angle_i_1 = 0;
    float angle_d_1 = -0.028 * ax; // 0
    float dis_p_1 = 5.50 * dx;     // 0;
    float dis_i_1 = 0;             // 0.55*dx;
    float dis_d_1 = 15.00 * dx;    // 0;
    move2Control.parameter_set_angle(angle_p_1, angle_i_1, angle_d_1, 0);
    move2Control.parameter_set_dis(dis_p_1, dis_i_1, dis_d_1, 0); // 0.3

    move2Control.zs_angle_track_001(ball_op_angle, 0 /*-120benchmark_angle/*______ */, black_white_ball);
    move2Control.turn_v_maxlimit(); // 限定平移[功率]速度输出最大值

    float r_hand_ball = 14.2 + 0.25; // 球绕开 持球器顶端的半径
    float r_add_hand_ball = 0.8;     // 为程序稳定运行不震荡  球绕开 持球器顶端的半径 加的一个距离 越大动作越大越快
    float a_hand_ball = 25.0;        // 球绕开 持球器顶端的角度 正数"+"
    float r_ball_in_hand = 11.0;     // 12.66;		//球进入 持球器的底端的半径
    float a_ball_in_hand = 10;       // 6.0;		//球进入 持球器的误差角   正数"+"
    // r_hand_ball=22.6+0.25;	//球绕开 持球器顶端的半径
    // r_add_hand_ball=0.8;		//为程序稳定运行不震荡  球绕开 持球器顶端的半径 加的一个距离 越大动作越大越快
    // a_hand_ball=25.0;			//球绕开 持球器顶端的角度 正数"+"
    // r_ball_in_hand=19.66;		//球进入 持球器的底端的半径
    // a_ball_in_hand=6.0;		//球进入 持球器的误差角   正数"+"
    if ((abs(ball_op_angle) > a_hand_ball) && (ball_op_radius < r_hand_ball))
    {
        if (ball_op_angle > a_hand_ball) // 右边躲持球器 后退部分
            move2Control.zs_point_track_01_002(ball_op_angle, ball_op_radius, a_hand_ball /*benchmark_angle*/,
                                               r_hand_ball + r_add_hand_ball * 2 /*15.5 10.0*/);
        if (ball_op_angle < -a_hand_ball) // 左边躲持球器 后退部分
            move2Control.zs_point_track_01_002(ball_op_angle, ball_op_radius, -a_hand_ball /*benchmark_angle*/,
                                               r_hand_ball + r_add_hand_ball * 2 /*15.5 10.0*/);
    }
    else
    {
        if ((abs(ball_op_angle) > a_ball_in_hand) &&
            (ball_op_radius >= r_hand_ball)) // 角度未进持球器 平移不接近部分	//这部分可以换成旋转接近
            move2Control.zs_point_track_01_002(ball_op_angle, ball_op_radius, 0 /*benchmark_angle*/,
                                               r_hand_ball + r_add_hand_ball /*15.5 10.0*/);
        else // 角度已经进持球器 平移接近部分
        {
            move2Control.zs_point_track_01_002(ball_op_angle, ball_op_radius, 0 /*benchmark_angle*/,
                                               r_ball_in_hand /*12.6+0.25*/);

            //		move2Control.parameter_set_dis  (    6.50*dx*1.9 	, 0.001*5*3*dx*1.5*0  ,	-100*0.805*dx*1.6 ,
            // 0);//为防止球出持球器加大PID增益 		move2Control.zs_point_track_01_002(ball_op_angle,ball_op_radius,
            // ball_op_angle		 , r_ball_in_hand/*12.6+0.25*/);

            //		move2Control.zs_point_track_01_002(ball_op_angle,ball_op_radius, 0/*benchmark_angle*/,
            // ball_op_radius/*12.6+0.25*/);
        }
        //[无球] 变 [持球] 用 无前进量 平移对准 代码
        //	if ((abs(ball_op_angle)>a_ball_in_hand)&&(ball_op_radius>=r_hand_ball))//角度未进持球器 平移不接近部分
        ////这部分可以换成旋转接近 	  move2Control.zs_point_track_01_002(ball_op_angle,ball_op_radius,
        // 0/*benchmark_angle*/, ball_op_radius); 	else
        ////角度已经进持球器 平移接近部分 	  move2Control.zs_point_track_01_002(ball_op_angle,ball_op_radius,
        // 0/*benchmark_angle*/, ball_op_radius);
    }
    // 测试move2Control.zs_point_track_01_002(ball_op_angle,ball_op_radius, 0/*benchmark_angle*/, 15/*15.5 10.0*/);
    move2Control.front_v_maxlimit(); // 限定平移[功率]速度输出最大值
//	move2Control.front_a_maxlimit();//限定平移[功率]加速度最大值

// G_ball_fv=0;
// G_ball_fva=0;
// G_ball_w=0;
// 把现在的速度都变成积分控制
/*
move2Control.front_v_c=move2Control.front_v_c*0.2;
move2Control.zs_translate_compound_O(G_ball_fva,G_ball_fv);
move2Control.front_v_maxlimit();//限定平移[功率]速度输出最大值
G_ball_fv = move2Control.front_v_c;
G_ball_fva= move2Control.front_v_angle_c;
move2Control.turn_v_c=G_ball_w+move2Control.turn_v_c*0.1;
move2Control.turn_v_maxlimit();//限定平移[功率]速度输出最大值
G_ball_w   =move2Control.turn_v_c;
*/
//////////////用鼠标设置带球转动的速度大小/////////////////
#ifdef hand_set // mousex= (cursorXY.x-1024/2)   //mousey=-(cursorXY.y-768/2)
    turn_w = (GetMouseX() - 1024 / 2) /
             30; // 40;//40;//40;//30;//要求的用于旋转的每个轴的线速度<<<<<===================================
#endif
    //////////////用鼠标设置带球转动的速度大小/////////////////

    float Rcenter = 31;      // 32;//24;//5;				//[厘米]要求的旋转中心到本机中心的半径 要求正数
    float RcQuotiety = 2.95; // up底盘    //??底盘2.2*1.5; 			//Rcenter和实际输出的比例系数	 要求正数
    turn_fv = turn_w * 3.14 * Rcenter * RcQuotiety /
              180; // w*pi*r/180=v     v/(w*3.14/????128????);			//机体旋转中心的极坐标	半径w*pi*r/180=v  Rc
                   // w*(2*pi*r/360) =v  w(度/s)  v,r(米) 或(像素)
    if (turn_fv > 0)
        sign_fv = 1;
    else
        sign_fv = -1;

    turn_afv = -90; // ball_op_angle-90;//-90;	//+Acenter;	//平移量的角度

#ifdef hand_set
    Front_C = 20; // 30;		//测试 固定 前进量的大小
#endif

    // 25大~35小   90-40;//90-30;
    /*测试持球器的代码*/
    //	float end_dbangle=90;//[经验值]front a c角度横90度不变//末状态前进量和0度的夹角	越接近90越稳准备时间越长
    /*float ap = 0;//0.65;//没电0.2;//系数控制姿态角度 和角速度的比值
          ap = fabs(turn_w*front_v/G_turn_wMAX/24)*0.8;
    float af = -(turn_w/G_turn_wMAX)*(end_dbangle)*ap;
    if (af>90)
    {	af=90;}
    if (af<-90)
    {	af=-90;}
    af =af*10000.0;
    int   aft= (int)af;				//取小数点后4位
          af = (float)aft/10000.0;	//取小数点后4位
    if (af>end_dbangle)//保险程序 平移量方向 不能超过限定最大值
    {	af=end_dbangle;}
    if (af<-end_dbangle)
    {	af=-end_dbangle;}
    */
    float af =
        Dribble_Angle(end_dbangle, turn_w,
                      front_v); // 生成带球 动作中 带球轨迹圆弧切线方向 和机体0度方向的夹角   end_dbangle为生成最大夹角
                                // turn_w旋转角速度 front_v球前进速度 Front_aC机体平移角度

    if (Dribble_Angle_Track(turn_w, af, &Front_aC) ==
        1) // 带球角度跟踪  用Front_aC 跟踪 Dribble_Angle  并判断是否需要做 带球直线旋转动作 如果是返回1
    {
        // 这里先调 角度变化 速度 和转动速度一致的问题
        // wheel_rotate_speed_ratio=(2*3.1415926*0.22)*wheel_linear_speed_ratio
        // max_wheel_linear_speed
        //	m1_linear_speed=(motor1/128*max_wheel_linear_speed)*wheel_linear_speed_ratio;
        // 轮子转速w1 = W * wheel_rotate_speed_ratio
        // 轮子转速w2 = V * wheel_linear_speed_ratio
        //  w (圈/秒)    =>    轮子转速 Lw (圈/秒) =  w * wheel_rotate_speed_ratio
        //  turn_w    =>    轮子线速度Vw = turn_w/128*max_wheel_linear_speed    =>    机体角速度Ww *
        //  wheel_rotate_speed_ratio= 轮子线速度Vw * wheel_linear_speed_ratio
        //			 =>	   机体角速度Ww = 轮子线速度Vw * wheel_linear_speed_ratio / wheel_rotate_speed_ratio
        //			 =>	   机体角速度Ww = turn_w / 128 * max_wheel_linear_speed * wheel_linear_speed_ratio /
        // wheel_rotate_speed_ratio double aa=1/ 128.0 * max_wheel_linear_speed * wheel_linear_speed_ratio /
        // wheel_rotate_speed_ratio;// 机体角速度
        float Ww = Angle_Speed(turn_w); // 换算从128制角速度 换算为 度/秒 单位的角速度

        Front_aC = angle_add_transfer(Front_aC, -Ww * 0.001 * 360 * Dribble_WP *
                                                    Gb_loop_time); // 这里要根据 本桢时间 调整偏移量 因为角速度是不变的
        if (af > 0)
        {
            if (Front_aC > af)
                Front_aC = af;
        }
        else
        {
            if (Front_aC < af)
                Front_aC = af;
        }
        ///*20090401 换至此位置*/		move2Control.zs_translate_compound_O(turn_afv,turn_fv);//(float angle,float
        /// speed)(90,80);//angle_ball+90(90,40);//将当前运动向量与  另一个平移分量相加
    }
    if (Front_aC > end_dbangle) // 保险程序 平移量方向 不能超过限定最大值
    {
        Front_aC = end_dbangle;
    }
    if (Front_aC < -end_dbangle)
    {
        Front_aC = -end_dbangle;
    }

    ///*20081020*/		move2Control.turn_v_c	+=	turn_w;
    //	turn_fv  = turn_w*3.14*Rcenter*1.8*RcQuotiety/180;//w*pi*r/180=v     v/(w*3.14/????128????);
    ////机体旋转中心的极坐标	半径w*pi*r/180=v  Rc							w*(2*pi*r/360) =v  w(度/s)  v,r(米)
    // 或(像素) 	float R  = Front_C/(turn_w*3.14*RcQuotiety/180); if (Front_C>turn_fv) {
    // move2Control.zs_translate_compound_O(Front_aC,Front_C);//(float angle,float
    // speed)(90,80);//angle_ball+90(90,40);//将当前运动向量与  另一个平移分量相加
    // }
    //  else
    //{	move2Control.zs_translate_compound_O(Front_aC,turn_fv);//(float angle,float
    // speed)(90,80);//angle_ball+90(90,40);//将当前运动向量与  另一个平移分量相加
    // }

    /*20081020*/ move2Control.turn_v_c += turn_w;
    /*20081020*/ move2Control.zs_translate_compound_O(
        turn_afv,
        turn_fv); //(float angle,float speed)(90,80);//angle_ball+90(90,40);//将当前运动向量与  另一个平移分量相加
    /*20081020*/ move2Control.zs_translate_compound_O(
        Front_aC,
        Front_C);      //(float angle,float speed)(90,80);//angle_ball+90(90,40);//将当前运动向量与  另一个平移分量相加
    float curV, curVa; // 当前平移 速度 和方向
    move2Control.translate_v_vector_O(&curVa, &curV);
    // float centripetal_force_add =  fabs(turn_w/G_turn_wMAX)* curV/(93.0)
    // *(61.0);//(41.0);//(35.0);//最大35.0的向心力修正量
    ///*20081022进行离心力抵消修正*/		move2Control.zs_translate_compound_O(0,centripetal_force_add);//(float
    /// angle,float speed)(90,80);//angle_ball+90(90,40);//将当前运动向量与  另一个平移分量相加
    // 姿态重新修正 旋转量越大平移方向越趋近0度 关系为二次
    move2Control.front_v_angle_c = move2Control.front_v_angle_c *
                                   (G_turn_wMAX * G_turn_wMAX - fabs(turn_w * turn_w) * 0.75) / G_turn_wMAX /
                                   G_turn_wMAX;
    move2Control.movement_output_O(&motor1, &motor2, &motor3); // 最后输出经过 合成处理的 三电机轮速 全向底盘

    *m1 = motor1;
    *m2 = motor2;
    *m3 = motor3;

    return (1);
}

int Dribble::G_ball_03(float front_v, float turn_speed, float ball_angle, float ball_radius, float *m1, float *m2,
                       float *m3)
{ //	front_v=0;turn_speed=0;

    // 实际使用 前进 参数数值
    double turn_w =
        turn_speed;    // 17;  //-18;//-20;//要求的用于旋转的每个轴的线速度<<<<<===================================
    Front_C = front_v; // 存储带球当中当前前进分量的大小  保证带球当中此量减小速度小于一个定值
    // float Front_aC = 0;	//前进量的角度
    //		float Front_S	= 50;		//前进量的大小
    turn_fv = 0;       // 平移速度大小		临时存储 要达到要求的旋转中心位置 需要附加的平移速度
    turn_afv = -90;    // 平移速度的方向    临时存储 要达到要求的旋转中心位置 需要附加的平移速度的方向
    float sign_fv = 0; // 记录turn_fv的正负号方向
    char black_white_ball[10];

    // float ball_op_weight;
    float ball_op_angle = ball_angle;
    float ball_op_radius = ball_radius;
    float motor1 = 0;
    float motor2 = 0;
    float motor3 = 0;

    move2Control.initializing_movement(0, 0, 0); //(float turn,float front,float angle)

    move2Control.limit_parameter_set_angle(21, 1); // 3(128,328);//float velocity_limit_,float acceleration_limit_
    move2Control.limit_parameter_set_dis(
        10, 5); //(15, 5);//(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
    float ax =
        0.51 * 4.8 * 0.14; //  0.5;//0.17;//0.51*4.8*0.09;//0.51*1.5;//1.2;//5;//0.45;//0.39;//角速度修正量 缩减系数
    float dx = 1;          //*0  2.4*0.015 *8;//1;//0.80;//1;//2.8;			//平移修正量  缩减系数
    float angle_p_1 = 0.35 * ax;
    float angle_i_1 = 0;
    float angle_d_1 = 0; //-0.028*ax;//0
    float dis_p_1 = 6 * dx;
    float dis_i_1 = 0;
    float dis_d_1 = 0; // 17*dx;
    move2Control.parameter_set_angle(angle_p_1, angle_i_1, angle_d_1, 0);
    move2Control.parameter_set_dis(dis_p_1, dis_i_1, dis_d_1, 0); // 0.3

    move2Control.zs_angle_track_001(ball_op_angle, 0 /*-120benchmark_angle/*______ */, black_white_ball);
    move2Control.turn_v_maxlimit(); // 限定平移[功率]速度输出最大值

    float r_hand_ball = 14.2 + 0.85; // 球绕开 持球器顶端的半径
    float r_add_hand_ball = 0.8;     // 为程序稳定运行不震荡  球绕开 持球器顶端的半径 加的一个距离 越大动作越大越快
    float a_hand_ball = 25.0;        // 球绕开 持球器顶端的角度 正数"+"
    float r_ball_in_hand = 11.0;     // 12.66;		//球进入 持球器的底端的半径
    float a_ball_in_hand = 10;       // 6.0;		//球进入 持球器的误差角   正数"+"
    // r_hand_ball=22.6+0.25;	//球绕开 持球器顶端的半径
    // r_add_hand_ball=0.8;		//为程序稳定运行不震荡  球绕开 持球器顶端的半径 加的一个距离 越大动作越大越快
    // a_hand_ball=25.0;			//球绕开 持球器顶端的角度 正数"+"
    // r_ball_in_hand=19.66;		//球进入 持球器的底端的半径
    // a_ball_in_hand=6.0;		//球进入 持球器的误差角   正数"+"
    if ((abs(ball_op_angle) > a_hand_ball) && (ball_op_radius < r_hand_ball))
    {
        if (ball_op_angle > a_hand_ball) // 右边躲持球器 后退部分
            move2Control.zs_point_track_01_002(ball_op_angle, ball_op_radius, a_hand_ball /*benchmark_angle*/,
                                               r_hand_ball + r_add_hand_ball * 2 /*15.5 10.0*/);
        if (ball_op_angle < -a_hand_ball) // 左边躲持球器 后退部分
            move2Control.zs_point_track_01_002(ball_op_angle, ball_op_radius, -a_hand_ball /*benchmark_angle*/,
                                               r_hand_ball + r_add_hand_ball * 2 /*15.5 10.0*/);
    }
    else
    {
        if ((abs(ball_op_angle) > a_ball_in_hand) &&
            (ball_op_radius >= r_hand_ball)) // 角度未进持球器 平移不接近部分	//这部分可以换成旋转接近
            move2Control.zs_point_track_01_002(ball_op_angle, ball_op_radius, 0 /*benchmark_angle*/,
                                               r_hand_ball + r_add_hand_ball /*15.5 10.0*/);
        else // 角度已经进持球器 平移接近部分
        {
            move2Control.zs_point_track_01_002(ball_op_angle, ball_op_radius, 0 /*benchmark_angle*/,
                                               r_ball_in_hand /*12.6+0.25*/);

            //		move2Control.parameter_set_dis  (    6.50*dx*1.9 	, 0.001*5*3*dx*1.5*0  ,	-100*0.805*dx*1.6 ,
            // 0);//为防止球出持球器加大PID增益 		move2Control.zs_point_track_01_002(ball_op_angle,ball_op_radius,
            // ball_op_angle		 , r_ball_in_hand/*12.6+0.25*/);

            //		move2Control.zs_point_track_01_002(ball_op_angle,ball_op_radius, 0/*benchmark_angle*/,
            // ball_op_radius/*12.6+0.25*/);
        }
        //[无球] 变 [持球] 用 无前进量 平移对准 代码
        //	if ((abs(ball_op_angle)>a_ball_in_hand)&&(ball_op_radius>=r_hand_ball))//角度未进持球器 平移不接近部分
        ////这部分可以换成旋转接近 	  move2Control.zs_point_track_01_002(ball_op_angle,ball_op_radius,
        // 0/*benchmark_angle*/, ball_op_radius); 	else
        ////角度已经进持球器 平移接近部分 	  move2Control.zs_point_track_01_002(ball_op_angle,ball_op_radius,
        // 0/*benchmark_angle*/, ball_op_radius);
    }
    // 测试move2Control.zs_point_track_01_002(ball_op_angle,ball_op_radius, 0/*benchmark_angle*/, 15/*15.5 10.0*/);
    move2Control.front_v_maxlimit(); // 限定平移[功率]速度输出最大值
    //	move2Control.front_a_maxlimit();//限定平移[功率]加速度最大值

    // G_ball_fv=0;
    // G_ball_fva=0;
    // G_ball_w=0;
    // 把现在的速度都变成积分控制
    /*
    move2Control.front_v_c=move2Control.front_v_c*0.2;
    move2Control.zs_translate_compound_O(G_ball_fva,G_ball_fv);
    move2Control.front_v_maxlimit();//限定平移[功率]速度输出最大值
    G_ball_fv = move2Control.front_v_c;
    G_ball_fva= move2Control.front_v_angle_c;
    move2Control.turn_v_c=G_ball_w+move2Control.turn_v_c*0.1;
    move2Control.turn_v_maxlimit();//限定平移[功率]速度输出最大值
    G_ball_w   =move2Control.turn_v_c;
    */
    move2Control.front_v_c = move2Control.front_v_c * 0.2;
    move2Control.zs_translate_compound_O(robot_fva, robot_fv);
    move2Control.front_v_maxlimit(); // 限定平移[功率]速度输出最大值
    robot_fv = move2Control.front_v_c;
    robot_fva = move2Control.front_v_angle_c;

// move2Control.turn_v_c=robot_w+move2Control.turn_v_c*0.02;
// move2Control.turn_v_maxlimit();//限定平移[功率]速度输出最大值
// robot_w  =move2Control.turn_v_c;

//////////////用鼠标设置带球转动的速度大小/////////////////
#ifdef hand_set // mousex= (cursorXY.x-1024/2)   //mousey=-(cursorXY.y-768/2)
    turn_w = (GetMouseX() - 1024 / 2) /
             30; // 40;//40;//40;//30;//要求的用于旋转的每个轴的线速度<<<<<===================================
#endif
    //////////////用鼠标设置带球转动的速度大小/////////////////

    float Rcenter = 31;      // 32;//24;//5;				//[厘米]要求的旋转中心到本机中心的半径 要求正数
    float RcQuotiety = 2.95; // up底盘    //??底盘2.2*1.5; 			//Rcenter和实际输出的比例系数	 要求正数
    turn_fv = turn_w * 3.14 * Rcenter * RcQuotiety /
              180; // w*pi*r/180=v     v/(w*3.14/????128????);			//机体旋转中心的极坐标	半径w*pi*r/180=v  Rc
                   // w*(2*pi*r/360) =v  w(度/s)  v,r(米) 或(像素)
    if (turn_fv > 0)
        sign_fv = 1;
    else
        sign_fv = -1;

    turn_afv = -90; // ball_op_angle-90;//-90;	//+Acenter;	//平移量的角度

#ifdef hand_set
    Front_C = 20; // 30;		//测试 固定 前进量的大小
#endif

    float af =
        Dribble_Angle(end_dbangle, turn_w,
                      front_v); // 生成带球 动作中 带球轨迹圆弧切线方向 和机体0度方向的夹角   end_dbangle为生成最大夹角
                                // turn_w旋转角速度 front_v球前进速度 Front_aC机体平移角度

    if (Dribble_Angle_Track(turn_w, af, &Front_aC) ==
        1) // 带球角度跟踪  用Front_aC 跟踪 Dribble_Angle  并判断是否需要做 带球直线旋转动作 如果是返回1
    {
        // 这里先调 角度变化 速度 和转动速度一致的问题
        // float Ww=Angle_Speed(turn_w);//换算从128制角速度 换算为 度/秒 单位的角速度
        //	Front_aC =angle_add_transfer(Front_aC ,-Ww*0.001*360*Dribble_WP*Gb_loop_time);//这里要根据 本桢时间
        // 调整偏移量 因为角速度是不变的
        Angle_add(turn_w, &Front_aC);
        if (af > 0)
        {
            if (Front_aC > af)
                Front_aC = af;
        }
        else
        {
            if (Front_aC < af)
                Front_aC = af;
        }
    }
    if (Front_aC > end_dbangle) // 保险程序 平移量方向 不能超过限定最大值
    {
        Front_aC = end_dbangle;
    }
    if (Front_aC < -end_dbangle)
    {
        Front_aC = -end_dbangle;
    }

    /*20081020*/ move2Control.turn_v_c += turn_w;
    /*20081020*/ move2Control.zs_translate_compound_O(
        turn_afv,
        turn_fv); //(float angle,float speed)(90,80);//angle_ball+90(90,40);//将当前运动向量与  另一个平移分量相加
    /*20081020*/ move2Control.zs_translate_compound_O(
        Front_aC,
        Front_C);      //(float angle,float speed)(90,80);//angle_ball+90(90,40);//将当前运动向量与  另一个平移分量相加
    float curV, curVa; // 当前平移 速度 和方向
    move2Control.translate_v_vector_O(&curVa, &curV);
    ///*20081022进行离心力抵消修正*/		move2Control.zs_translate_compound_O(0,centripetal_force_add);//(float
    /// angle,float speed)(90,80);//angle_ball+90(90,40);//将当前运动向量与  另一个平移分量相加
    // 姿态重新修正 旋转量越大平移方向越趋近0度 关系为二次
    move2Control.front_v_angle_c = move2Control.front_v_angle_c *
                                   (G_turn_wMAX * G_turn_wMAX - fabs(turn_w * turn_w) * 0.75) / G_turn_wMAX /
                                   G_turn_wMAX;

    move2Control.movement_output_O(&motor1, &motor2, &motor3); // 最后输出经过 合成处理的 三电机轮速 全向底盘

    *m1 = motor1;
    *m2 = motor2;
    *m3 = motor3;

    return (1);
}

void Dribble::Dribble_ball(float *G_turn_w, float *motor1, float *motor2,
                           float *motor3) // 可以生成带球  球速度 的带球函数
{
    if (xingwei_P != 101) // 如果上周期没有在进攻  就认为动作变化了 追球和带球有过度
                          // 也使用Dribble_ball_other_moment_flag  追球和带球都在101里
    {
        Dribble_ball_other_moment_flag = 1; // 所以不能靠else清零
    }
    if (Dribble_ball_other_moment_flag == 1)
    {
        Dribble_ball_other_moment_flag = 0;
        fv = robot_fv;
    }
    Front_aC = robot_fva; //?????????????????????????????????????
    // 主进攻带球

    float Gb_v = Gb_Vavg;
    // 生成目标带球平移速度Gb_v 为分段函数
    if (fabs(*G_turn_w) >= fabs(G_turn_wMAX * Gb_w_m)) // 如果角速度 大于G_turn_wMAX*0.2就减平移速度
    { // G_turn_wMAX 带球旋转角速度最大值//Gb_v_lest 剩下的前进量速度 占总速度的百分数//Gb_w_m 最小转动速度
      // 以下不需要减速 占总角速度的百分数
        Gb_v =
            (Gb_Vavg - Gb_v_lest * Gb_Vavg) * (fabs(*G_turn_w) - G_turn_wMAX) / (G_turn_wMAX * Gb_w_m - G_turn_wMAX) +
            Gb_v_lest * Gb_Vavg;
    }
    if (Gb_v < 0) // 保险
    {
        Gb_v = 0;
    }

    if (fv < Gb_v) // 带球到平均速度的加速部分 小于时要增大(进入带球程序)
    {
        fv += Gb_Vadd;
        if (fv > Gb_v)
        {
            fv = Gb_v;
        }
    }
    ///*调试显示图线用*/	Debug_D[13].Draw_vp (Gb_v,400,2,valxx,255,0,0, pBuffer);//valxx

    if (ball_op_Fr < holdballFmin) // 调节减速位置点 设置在球进入持球器底端+0.1左右的位置
                                   // ball_org_angle,ball_org_radius;没经过估算的数据
    {
        if (fv > Gb_v) // 带球到平均速度的减速部分 大于时要减小(进入带球程序)
        {
            fv -= Gb_Vred;
            if (fv < Gb_v)
            {
                fv = Gb_v;
            }
        }
    }

    if (fabs(*G_turn_w) >= fabs(G_turn_wMAX * Gb_w_m)) // 如果角速度 大于G_turn_wMAX*0.2就减平移速度
    {
        float G_turn_w_t;
        if (*G_turn_w > 0)
        {
            G_turn_w_t =
                (fv - Gb_v_lest * Gb_Vavg) * (G_turn_wMAX * Gb_w_m - G_turn_wMAX) / (Gb_Vavg - Gb_v_lest * Gb_Vavg) +
                G_turn_wMAX;
        }
        else
        {
            G_turn_w_t =
                -(fv - Gb_v_lest * Gb_Vavg) * (G_turn_wMAX * Gb_w_m - G_turn_wMAX) / (Gb_Vavg - Gb_v_lest * Gb_Vavg) -
                G_turn_wMAX;
        }
        if (fabs(*G_turn_w) > fabs(G_turn_w_t))
        {
            *G_turn_w = G_turn_w_t;
        }
    }
    // twt=*G_turn_w;//DEBUG 使用
    ///*调试显示图线用*/Debug_D[13].Draw_vp (*G_turn_w,400,2,valxx,255,0,0, pBuffer);//valxx
    //	use_side_line=0;//应该使用1;	//	约定use_side_line号 为1 路径规划中加入边线信息  !=1时无边线信息
    //*G_turn_w=-17;//
    //      fv=0;//21;//
    G_ball_03(fv, *G_turn_w, ball_op_Fa, ball_op_Fr, motor1, motor2, motor3); // 32
    // 转换成博创底盘的输出
    move2Control.uptech_output_O(motor1, motor2, motor3); // float *mtb,float *mtl,float *mtr);
}
void Dribble::ni_uptech_output_O(float *va, float *v, float *w, float m1, float m2, float m3)
{
    m1 = (m1 * 128) / (wheel_linear_speed_ratio * max_wheel_linear_speed);
    m2 = (m2 * 128) / (wheel_linear_speed_ratio * max_wheel_linear_speed);
    m3 = (m3 * 128) / (wheel_linear_speed_ratio * max_wheel_linear_speed);
    // lpLogic->SetFourMotorsSpeed(motor2,motor1,motor3,0);
    // lpLogic->SetFourMotorsSpeed(m2_linear_speed,m1_linear_speed,m3_linear_speed,0);
    //	*mtb=m2_linear_speed;	*mtl=m1_linear_speed;	*mtr=m3_linear_speed;
    O_movement_T_analyze(va, v, w, -m2, -m1, -m3); // *move_v_angle, *move_v_speed, *turn_v
}

float Dribble::Ball_Track_x_diff(float x, float max)
{
    float diff_x = dis_x.diff(x);
    if (diff_x > max)
        diff_x = max;
    if (diff_x < -max)
        diff_x = -max;
    return (diff_x); // 计算速度矢量完成
}
float Dribble::Ball_Track_y_diff(float y, float max)
{
    float diff_y = dis_y.diff(y);
    if (diff_y > max)
        diff_y = max;
    if (diff_y < -max)
        diff_y = -max;
    return (diff_y); // 计算速度矢量完成
}
float Dribble::Ball_Track_a_diff(float a, float max)
{
    float diff_a = dis_a.diff_angle(a);
    if (diff_a > max)
        diff_a = max;
    if (diff_a < -max)
        diff_a = -max;
    return (diff_a); // 计算速度矢量完成
}

void Dribble::Ball_Into_Holder(float ball_angle, float ball_Rradius)
{
    //	mtl=0;
    //	mtr=0;
    //	mtb=0;

    move_test_C.limit_parameter_set_angle(10,
                                          1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
    move_test_C.limit_parameter_set_dis(
        10, 5);                   //(15, 5);//(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
    float ax = 0.51 * 4.8 * 0.55; // 0.17;//0.51*4.8*0.09;//0.51*1.5;//1.2;//5;//0.45;//0.39;//角速度修正量 缩减系数
    float dx = 0.018;             // 2.4*0.015 ;//1;//0.80;//1;//2.8;			//平移修正量  缩减系数
    float angle_p_1 = 0.140 * ax; // 0.35*ax;//;
    float angle_i_1 = 0 * ax;     // 0.005*ax;  //0.06*ax;//0;
    float angle_d_1 = 0;          //-0.008*ax;//0-0.028*ax;//0
    float dis_p_1 = 5.50 * dx;
    float dis_i_1 = 0;
    float dis_d_1 = 15.00 * dx;
    move_test_C.parameter_set_angle(angle_p_1, angle_i_1, angle_d_1, 0);
    move_test_C.parameter_set_dis(dis_p_1, dis_i_1, dis_d_1, 0); // 0.3

    float holdballmin = 19.3; // 持球的 距离
    float holdballmax = 26;   // 持球的 距离
    float holdballangle = 6;  // 持球 的角度
    char black_white_ball[20];
    float motor1, motor2, motor3;
    float mb, ml, mr;

    // float holdballFmin = 12.7;//12.7;//13.6;//13.9+0.2; //12.65;//299;//12.95;        //前景 球进入持球器底端最小值
    // float holdballFmax = 12.8;//12.8;//13.7;//13.99+0.2;//12.75;//35 ;//12.7; //12.95;//前景 球进入持球器    最大值

    // 主进攻追球
    // if(1)//((ball_op_Fr>holdballFmax)||(abs(ball_org_angle)>holdballangle)) //应该再加一个球的大小前景
    // 如果球的距离太远(0)//
    //{
    //	target_num		=1;	//设置路径规划目的地为 球	约定 traget_numC号 初始化值0   1球 2敌方门 3己方门  4以上其他点
    //	use_side_line	=0;	//	约定use_side_line号 为1 路径规划中加入边线信息  !=1时无边线信息
    //	add_oppo_radius	=8.0;//规避半径初始化6.5 8.0
    //	if (ball_radius<60)
    //	{	add_oppo_radius=sqrt(ball_radius);
    //	}
    /*
        float dis_xc=ball_d_h[0].f_x-robot_L.f_x;//当前 球C~机器人A x 距离
        float dis_yc=ball_d_h[0].f_y-robot_L.f_y;//当前 球C~机器人A y 距离

        float D_dis_x=dis_xc-dis_xp;//计算速度矢量
        float D_dis_y=dis_yc-dis_yp;
        if (D_dis_x> 0.80)	D_dis_x= 0.80;
        if (D_dis_x<-0.80)	D_dis_x=-0.80;
        if (D_dis_y> 0.80)	D_dis_y= 0.80;
        if (D_dis_y<-0.80)	D_dis_y=-0.80;
        //计算速度矢量完成

    /////////////////////////////////////////////////////////////////////////////////
    double front_vx_c,front_vy_c;
    double front_vx_p,front_vy_p;
    double front_vx_d,front_vy_d;
    double front_va_d,front_vr_d;
    float  acceleration_limit_dis=1.2;
    front_vx_c = dis_xc;
    front_vy_c = dis_yc;
    front_vx_p = dis_vxp;
    front_vy_p = dis_vyp;
    front_vx_d = front_vx_c - front_vx_p;
    front_vy_d = front_vy_c - front_vy_p;
    front_va_d = coordinate_transfer_angle (front_vx_d,front_vy_d);
    front_vr_d = coordinate_transfer_radius(front_vx_d,front_vy_d);
        if (front_vr_d > acceleration_limit_dis)//front_vr_d是向量相减所得向量的 模    要求小于加速度限定最大值
        {	front_vr_d = acceleration_limit_dis;//加速度最大值控制
        }
    dis_vxp = front_vx_p + coordinate_transfer_x(front_va_d,front_vr_d);//更新dis_vxp dis_vyp
    dis_vyp = front_vy_p + coordinate_transfer_y(front_va_d,front_vr_d);
    //dis_vyp=dis_yc;//计算机体平移速度向量用
    //dis_vxp=dis_xc;
    /////////////////////////////////////////////////////////////////////////////////
        float dis_xct=dis_vxp;//先限制加速还是先限制最大值是一个问题
        float dis_yct=dis_vyp;
        float dis_act=coordinate_transfer_angle (dis_vxp,dis_vyp);
        float dis_rct=coordinate_transfer_radius(dis_vxp,dis_vyp);
        if (dis_rct>1)//平移最大值限制 （米制）
        {	dis_rct=1;
            dis_xct=coordinate_transfer_x(dis_act,dis_rct);
            dis_yct=coordinate_transfer_y(dis_act,dis_rct);
        }

    //	float dis_ac=coordinate_transfer_angle (dis_xc,dis_yc);
    //	float dis_rc=coordinate_transfer_radius(dis_xc,dis_yc);
    //	if (dis_rc>1)//平移最大值限制 （米制）
    //	{	dis_rc=1;
    //		dis_xc=coordinate_transfer_x(dis_ac,dis_rc);
    //		dis_yc=coordinate_transfer_y(dis_ac,dis_rc);
    //	}
    */
    move_test_C.limit_parameter_set_dis(
        30, 5); //(15, 5);//(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
    move_test_C.initializing_movement(0, 0, 0); //(float turn,float front,float angle)
    float r_hand_ball = 12.9;                   // 14.2+0.25;	//球绕开 持球器顶端的半径
    ///*显示球*/Debug_D[12].Draw_line_	   (320 ,240
    ///,-coordinate_transfer_angle(r_hand_ball,sin(ball_angle*3.14/180.0)*ball_Rradius)
    ///-90,sqrt(r_hand_ball*r_hand_ball+sin(ball_angle*3.14/180.0)*ball_Rradius*sin(ball_angle*3.14/180.0)*ball_Rradius),0,255,255,
    /// pBuffer); *显示球*/Debug_D[12].Draw_line_	   (320 ,240
    ///,-coordinate_transfer_angle(r_hand_ball,sin(ball_op_Fa*3.14/180.0)*ball_op_Fr)
    ///-90,sqrt(r_hand_ball*r_hand_ball+sin(ball_op_Fa*3.14/180.0)*ball_op_Fr*sin(ball_op_Fa*3.14/180.0)*ball_op_Fr),0,255,255,
    /// pBuffer); *显示球*/Debug_D[11].Draw_line_	   (320 ,240 ,ball_angle -90,ball_Rradius,255,0,0,  pBuffer);
    if (FHaveBall == 1) //(ball_op_Fweight>1000)
    {
        //??????????????????????????????????????????????????????????????
        ax = 0.51 * 4.8 * 0.14 *
             0.8; //  0.5;//0.17;//0.51*4.8*0.09;//0.51*1.5;//1.2;//5;//0.45;//0.39;//角速度修正量 缩减系数
        dx = 2.4 * 0.015 * 2.9; // 32;//*0  2.4*0.015 *8;//1;//0.80;//1;//2.8;			//平移修正量  缩减系数
        angle_p_1 = 0.35 * ax;  //;
        angle_i_1 = 0;
        angle_d_1 = 0;            //-0.028*ax;//0
        dis_p_1 = 5.50 * dx;      // 0;
        dis_i_1 = 0;              // 0.55*dx;
        dis_d_1 = 15.00 * dx * 0; // 0;
        move_test_C.parameter_set_angle(angle_p_1, angle_i_1, angle_d_1, 0);
        move_test_C.parameter_set_dis(dis_p_1, dis_i_1, dis_d_1, 0); // 0.3

        float r_hand_ballpd = 14.0 + 2;  // 12.9;//14.2+0.25;	//球绕开 持球器顶端的判定半径
        float r_hand_ball = 14.0 - 1.15; // 12.9;//14.2+0.25;	//球绕开 持球器顶端的半径
        float r_add_hand_ball = -1.15;   // 为程序稳定运行不震荡  球绕开 持球器顶端的半径 加的一个距离 越大动作越大越快
        float a_hand_ball = 15.0 - 5;    // 25.0;			//球绕开 持球器顶端的角度 正数"+"
        float r_ball_in_hand = 9.0;      // 11.0;//12.66;		//球进入 持球器的底端的半径
        float a_ball_in_hand = 15 + 4;   // 6.0;		//球进入 持球器的误差角   正数"+"

        if ((fabs(ball_op_Fa) > a_ball_in_hand) && (ball_op_Fr < r_hand_ballpd))
        {
            move_test_C.parameter_set_angle(angle_p_1 * 0.5, angle_i_1, angle_d_1, 0);
            move_test_C.zs_angle_track_001(ball_op_Fa, 0 /*-120benchmark_angle/*______ */, black_white_ball);
            //	move_test_C.turn_v_maxlimit();//限定平移[功率]速度输出最大值
            if (ball_op_Fa > a_hand_ball) // 右边躲持球器 后退部分
                move_test_C.zs_point_track_01_002(ball_op_Fa, ball_op_Fr, a_hand_ball /* benchmark_angle*/,
                                                  r_hand_ball + r_add_hand_ball * 1 /*15.5 10.0*/);
            if (ball_op_Fa < -a_hand_ball) // 左边躲持球器 后退部分
                move_test_C.zs_point_track_01_002(ball_op_Fa, ball_op_Fr, -a_hand_ball /* benchmark_angle*/,
                                                  r_hand_ball + r_add_hand_ball * 1 /*15.5 10.0*/);
        }
        else
        {
            if ((fabs(ball_op_Fa) > a_ball_in_hand) &&
                (ball_op_Fr < r_hand_ballpd)) // 角度未进持球器 平移不接近部分	//这部分可以换成旋转接近
            {
                ///*正式*/		move_test_C.zs_point_track_01_002(ball_op_Fa,ball_op_Fr, 0/*benchmark_angle*/,
                /// r_hand_ball+r_add_hand_ball/*15.5 10.0*/); *正式2*/		move_test_C  .parameter_set_angle(
                /// angle_p_1*2	, angle_i_1 , angle_d_1 , 0); *正式2*/
                /// move_test_C.zs_angle_track_001(ball_op_Fa,0/*-120benchmark_angle/*______ */,black_white_ball);
                /*正式3*/ move_test_C.parameter_set_angle(angle_p_1 * 2.2, angle_i_1, angle_d_1, 0);
                /*正式3*/ move_test_C.zs_angle_track_001(ball_op_Fa, 0 /*-120benchmark_angle/*______ */,
                                                         black_white_ball);
                /*正式3*/ move_test_C.parameter_set_dis(dis_p_1 * 0.4, dis_i_1, dis_d_1, 0); // 0.3
                /*正式3*/ move_test_C.zs_point_track_01_002(ball_op_Fa, ball_op_Fr, 0 /*benchmark_angle*/,
                                                            ball_op_Fr * cos(ball_op_Fa * 3.14 / 180.0) /*12.6+0.25*/);
                /*正式3*/ move_test_C.parameter_set_dis(dis_p_1 * 1.5, dis_i_1, dis_d_1, 0); // 0.3
                /*正式3*/ move_test_C.zs_point_track_01_002(
                    ball_op_Fa, ball_op_Fr,
                    -coordinate_transfer_angle(r_hand_ball, sin(ball_op_Fa * 3.14 / 180.0) * ball_op_Fr),
                    sqrt(r_hand_ball * r_hand_ball +
                         sin(ball_op_Fa * 3.14 / 180.0) * ball_op_Fr * sin(ball_op_Fa * 3.14 / 180.0) * ball_op_Fr));
                //		move_test_C.turn_v_maxlimit();//限定平移[功率]速度输出最大值
                //		move_test_C.parameter_set_dis    (      dis_p_1*1.2 ,   dis_i_1 ,   dis_d_1 , 0);//0.3
                //		move_test_C.zs_point_track_01_002(ball_op_Fa,ball_op_Fr, 0/*benchmark_angle*/ ,
                // ball_op_Fr*cos(ball_op_Fa*3.14/180.0)/*12.6+0.25*/);
            }
            else // 角度已经进持球器 平移接近部分
            {
                ///*正式*/		move_test_C.parameter_set_dis    (      dis_p_1*1.5 ,   dis_i_1 ,   dis_d_1 , 0);//0.3
                ///*正式*/		move_test_C.zs_point_track_01_002(ball_op_Fa,ball_op_Fr, 0/*benchmark_angle*/,
                /// r_ball_in_hand/*12.6+0.25*/);
                move_test_C.parameter_set_angle(angle_p_1 * 2.2, angle_i_1, angle_d_1, 0);
                move_test_C.zs_angle_track_001(ball_op_Fa, 0 /*-120benchmark_angle/*______ */, black_white_ball);
                move_test_C.parameter_set_dis(dis_p_1 * 0.4, dis_i_1, dis_d_1, 0); // 0.3
                move_test_C.zs_point_track_01_002(ball_op_Fa, ball_op_Fr, 0 /*benchmark_angle*/,
                                                  ball_op_Fr * cos(ball_op_Fa * 3.14 / 180.0) /*12.6+0.25*/);
                move_test_C.parameter_set_dis(dis_p_1 * 1.5, dis_i_1, dis_d_1, 0); // 0.3
                move_test_C.zs_point_track_01_002(
                    ball_op_Fa, ball_op_Fr,
                    -coordinate_transfer_angle(r_ball_in_hand, sin(ball_op_Fa * 3.14 / 180.0) * ball_op_Fr),
                    sqrt(r_ball_in_hand * r_ball_in_hand +
                         sin(ball_op_Fa * 3.14 / 180.0) * ball_op_Fr * sin(ball_op_Fa * 3.14 / 180.0) * ball_op_Fr));

                //	move_test_C.parameter_set_dis  (    6.50*dx*1.9 	, 0.001*5*3*dx*1.5*0  ,	-100*0.805*dx*1.6 ,
                // 0);//为防止球出持球器加大PID增益 	move_test_C.zs_point_track_01_002(ball_op_Fa,ball_op_Fr,
                // 0/*benchmark_angle*/ , ball_op_Fr*cos(ball_op_Fa*3.14/180.0)/*12.6+0.25*/);
                //		move_test_C.parameter_set_dis    (      dis_p_1*2 ,   dis_i_1 ,   dis_d_1 , 0);//0.3
                //		move_test_C.zs_point_track_01_002(ball_op_Fa,ball_op_Fr, ball_op_Fa		 ,
                // r_ball_in_hand/*12.6+0.25*/);
            }
            //[无球] 变 [持球] 用 无前进量 平移对准 代码
            //	if ((abs(ball_op_angle)>a_ball_in_hand)&&(ball_op_radius>=r_hand_ball))//角度未进持球器 平移不接近部分
            ////这部分可以换成旋转接近 	  move_test_C.zs_point_track_01_002(ball_op_angle,ball_op_radius,
            // 0/*benchmark_angle*/, ball_op_radius); 	else
            ////角度已经进持球器 平移接近部分 	  move_test_C.zs_point_track_01_002(ball_op_angle,ball_op_radius,
            // 0/*benchmark_angle*/, ball_op_radius);
        }
        // 测试move_test_C.zs_point_track_01_002(ball_op_angle,ball_op_radius, 0/*benchmark_angle*/, 15/*15.5 10.0*/);
        // move_test_C  .initializing_movement(0,0,0);//(float turn,float front,float angle)
        //	move_test_C.turn_v_maxlimit();//限定平移[功率]速度输出最大值
        move_test_C.front_v_maxlimit(); // 限定平移[功率]速度输出最大值
        //??????????????????????????????????????????????????????????????
    }
    else
    {
        move_test_C.parameter_set_angle(angle_p_1 * 0.6, angle_i_1, angle_d_1, 0);
        ///*t*/ 	move_test_C  .parameter_set_angle(    angle_p_1*0.0	, angle_i_1 , angle_d_1 , 0);
        move_test_C.limit_parameter_set_angle(
            12, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
        move_test_C.zs_angle_track_001(ball_angle, 0 /*-120benchmark_angle/*______ */, black_white_ball);
        move_test_C.turn_v_maxlimit(); //
        //	move_test_C  .parameter_set_dis  (      dis_p_1 *0.3,   dis_i_1 ,   dis_d_1 , 0);//0.3
        move_test_C.zs_point_track_01_002(ball_angle, ball_Rradius, ball_angle, 21); // ball_angle/800
        //	move_test_C.turn_v_maxlimit();//限定平移[功率]速度输出最大值
    }
}

float faTest = 0;
void Dribble::Ball_Track3(float *motoB, float *motoL, float *motoR)
{
    // float   ball_estimate_angle ;ball_estimate_radius//运动估算后的输出
    // float   ball_estimate_F_angle ;ball_estimate_F_radius//前景运动估算后的输出
    float timeXISHU = float(Gb_loop_time) / 25.0; // 0.067;//周期时间单位秒 根据视觉周期时间 确定积分速度
    // 40ms=>1  0ms=>0
    if (timeXISHU > 1.4)
        timeXISHU = 1.4;
    if (timeXISHU < 0.5)
        timeXISHU = 0.5;

    float zqmb, zqml, zqmr;
    //+5是坐边持球好 -5是右边持球好
    // Ball_Into_Holder4(ball_estimate_angle+5 ,ball_estimate_radius,ball_estimate_F_angle,ball_estimate_F_radius);
    Ball_Into_Holder4(ball_estimate_angle, ball_estimate_radius, ball_estimate_F_angle, ball_estimate_F_radius);
    // Ball_Into_Holder4_();
    move_test_C.uptech_output_O(&zqmb, &zqml, &zqmr); // 对Ball_Into_Holder进行输出
    // 如果要加 可以在这里加入 球的平移矢量fv，Front_aC
    // 运动反解
    float zqva, zqv, zqw, zqv_x, zqv_y;
    zqva = move_test_C.front_v_angle_c;
    zqv = move_test_C.front_v_c;
    zqw = move_test_C.turn_v_c;
    zqv_x = coordinate_transfer_x(zqva + robot_L.f_a, zqv);
    zqv_y = coordinate_transfer_y(zqva + robot_L.f_a, zqv);

    // 调试代码
    // zqw   =0; zqv_x =0; zqv_y =0;
    zqw = zqw * 2.55; // 4.0;//4.5;//0;//0.959;//0;//0;//0.6;0.0;//

    float bili = 0.60; // 0.5;//0.8;//0;//1.5;//接近用的比例系数
    zqv_x = zqv_x * 0.001 * bili;
    zqv_y = zqv_y * 0.001 * bili;
    float bili2 = 2 - bili; // 1.5;//跟踪速度用比例系数
    // float err_fx
    // =Ball_Track_x_diff(ball_d_h[0].f_x-robot_L.f_x,400)*bili2+ball_d_h[0].f_vx*0.028*bili2+zqv_x;//速度误差 float
    // err_fy =Ball_Track_y_diff(ball_d_h[0].f_y-robot_L.f_y,400)*bili2+ball_d_h[0].f_vy*0.028*bili2+zqv_y;//
    float err_fx = Ball_Track_x_diff(ball_d_h[0].f_x - robot_L.f_x, 400) * bili2 + ball_d_h[0].f_vx * 0.005 * bili2 +
                   zqv_x; // 速度误差
    float err_fy =
        Ball_Track_y_diff(ball_d_h[0].f_y - robot_L.f_y, 400) * bili2 + ball_d_h[0].f_vy * 0.005 * bili2 + zqv_y; //

    // zqw  =zqw*0.4*bili;//???????????????????????????????????????????????????????????????????????
    // float err_angle =Ball_Track_a_diff(ball_estimate_angle,100)*bili2*0.2+zqw;//
    // robot_w+=err_angle;
    // zqw=robot_w;
    // if (FHaveBall==1)//(ball_op_Fweight>1000)//判断前景有没有球
    // float err_fx =Ball_Track_x_diff(l2f_x(robot_L.f_x,robot_L.f_y,robot_L.f_a,
    // ball_op_Fa,ball_op_Fr/100.0)-robot_L.f_x,400)*bili2+zqv_x;//速度误差 float err_fy
    // =Ball_Track_y_diff(l2f_y(robot_L.f_x,robot_L.f_y,robot_L.f_a,
    // ball_op_Fa,ball_op_Fr/100.0)-robot_L.f_y,400)*bili2+zqv_y;//

    float err_dfx = v_x.diff(err_fx); // 差分加速度
    float err_dfy = v_y.diff(err_fy);

    // 外界显示用
    view_add_x = err_fx; // 追球每周期对地增加量大小
    view_add_y = err_fy;

    float add_bili3 = 2; // 1.2*2;//1.0;//0.7;//1.1;//总比例系数
    // float add_x=((err_fx)*40 +err_dfx*72)*add_bili3;//0.5;
    // float add_y=((err_fy)*40 +err_dfy*72)*add_bili3;//0.5;
    float add_x = ((err_fx) * 30 - err_dfx * 48 * 0.02) * add_bili3; // 0.5;
    float add_y = ((err_fy) * 30 - err_dfy * 48 * 0.02) * add_bili3; // 0.5;
    float add_v =
        coordinate_transfer_radius(add_x, add_y); //(fount_xVset+ dis_xct*bilixishu,fount_yVset+ dis_yct*bilixishu);
    float add_a =
        coordinate_transfer_angle(add_x, add_y); //(fount_xVset+ dis_xct*bilixishu,fount_yVset+ dis_yct*bilixishu);
    //		Angle_add(zqw*0.9,&fa);//(zqw*0.55,&fa);//一个周期转动量 造成的转动角度
    //		Angle_add(zqw*0.9,&add_a);//(zqw*0.55,&fa);//一个周期转动量 造成的转动角度

    float max_A = 35; // 40
    if (add_v > max_A)
        add_v = max_A;
    if (add_v < 0) //(add
        add_x = coordinate_transfer_x(add_a, add_v);
    add_y = coordinate_transfer_y(add_a, add_v);

    //	Movement(T,RMfv,RMfa,RMw,&robot_L);
    //	Movement(T,RMfv,RMfa,RMw,&robot_L_test);
    // fount_xVset=l2f_x(0,0,robot_L.f_a,RMfa,RMfv)*28.31;
    // fount_yVset=l2f_y(0,0,robot_L.f_a,RMfa,RMfv)*28.31;

    // fount_xVset+=add_x*0.55;//0.65;//注意这里也应改成估算后的值
    // fount_yVset+=add_y*0.55;//0.65;
    //////////////////////////////////////////////
    //????????????????????????????????????????????
    ///*
    //	float fvx=fount_xVset;//add_x;//输入向量
    //	float fvy=fount_yVset;//add_y;
    float T = Gb_loop_time * 0.001; // 0.067;//周期时间单位秒
    float fvx = add_x;              // 输入向量
    float fvy = add_y;
    float bvx =
        simple_filter(ball_d_h[0].f_vx, ball_d_h[1].f_vx, ball_d_h[2].f_vx, ball_d_h[3].f_vx, ball_d_h[4].f_vx) / T;
    float bvy =
        simple_filter(ball_d_h[0].f_vy, ball_d_h[1].f_vy, ball_d_h[2].f_vy, ball_d_h[3].f_vy, ball_d_h[4].f_vy) /
        T; // 球对地速度向量
    float aaannn = sqrt(bvx * bvx + bvy * bvy);
    if (aaannn > 160)
        aaannn = aaannn;
    aaannn = aaannn / 160.0;
    if (aaannn > 1)
        aaannn = 1;
    aaannn = 0.1;                    // 0.8;
    float BAangle = atan2(bvy, bvx); //-3.14/2.0
                                     //	float VBV=sqrt;
    // 横向 法相 系数 相差程度 应该和球的总速度 成正比
    float bv_f = (sin(BAangle) * fvy + cos(BAangle) * fvx) * (1 - 0.65 * aaannn); // 球运行正方向 分量  进行0.5倍缩小
    float bv_f_ = (sin(BAangle + 3.14 / 2.0) * fvy + cos(BAangle + 3.14 / 2.0) * fvx) *
                  (1 + 1.6 * aaannn);                                     // 球运行正方向垂直 分量
    float fvx_ = bv_f * cos(BAangle) + bv_f_ * cos(BAangle + 3.14 / 2.0); // 输出向量
    float fvy_ = bv_f * sin(BAangle) + bv_f_ * sin(BAangle + 3.14 / 2.0); //
    //	fount_xVset=fvx_;//0.65;//注意这里也应改成估算后的值
    //	fount_yVset=fvy_;//0.65;
    fount_xVset += fvx_ * 0.85 * timeXISHU; // 0.65;//注意这里也应改成估算后的值
    fount_yVset += fvy_ * 0.85 * timeXISHU; // 0.65;//
    //*/
    //////////////////////////////////////////////

    // v=
    if (0) //(valxx!=1)///*调试球进持球器*/?????????????!!!!!!!!!!!!!!!全局????????????????????????????????????!!!!!!!!!!
    {
        fount_xVset = 0;
        fount_yVset = 0;
    }

    //	float bilixishu=6.0;
    ///*调试球进持球器*/	fv =
    /// coordinate_transfer_radius(fount_xVset+zqv_x*bili,fount_yVset+zqv_y*bili);//(fount_xVset+
    /// dis_xct*bilixishu,fount_yVset+ dis_yct*bilixishu); *调试球进持球器*/	fa = coordinate_transfer_angle
    ///(fount_xVset+zqv_x*bili,fount_yVset+zqv_y*bili);//(fount_xVset+ dis_xct*bilixishu,fount_yVset+
    /// dis_yct*bilixishu);
    fv = coordinate_transfer_radius(fount_xVset,
                                    fount_yVset); //(fount_xVset+ dis_xct*bilixishu,fount_yVset+ dis_yct*bilixishu);
    fa = coordinate_transfer_angle(fount_xVset,
                                   fount_yVset); //(fount_xVset+ dis_xct*bilixishu,fount_yVset+ dis_yct*bilixishu);
    ///*调试*/	fv = coordinate_transfer_radius(zqv_x,zqv_y);//(fount_xVset+ dis_xct*bilixishu,fount_yVset+
    /// dis_yct*bilixishu); *调试*/	fa = coordinate_transfer_angle (zqv_x,zqv_y);//(fount_xVset+
    /// dis_xct*bilixishu,fount_yVset+ dis_yct*bilixishu);
    //	fv-=fabs(zqw)*0.2;//在旋转时减速
    if (fv > 42) //(fv>42)//50可调值 限制最大平移速度
        fv = 42;
    if (fv < 0) //(fv<-50)//可调值 限制最大平移速度
        fv = 0; //-50;
    fount_xVset = coordinate_transfer_x(fa, fv);
    fount_yVset = coordinate_transfer_y(fa, fv);

    fa = angle_add_transfer(fa, -robot_L.f_a);

    move_test_D.limit_parameter_set_angle(30,
                                          1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
    //	move_test_D  .limit_parameter_set_dis(10, 5);//(15, 5);//(128,328);//float velocity_limit_dis_,float
    // acceleration_limit_dis_
    move_test_D.parameter_set_angle(0.2, 0, 0, 0);
    move_test_D.parameter_set_dis(0.2, 0, 0.5, 0); // 0.3

    move_test_D.initializing_movement(0, 0, 0); //(float turn,float front,float angle)

    move_test_D.turn_v_c = zqw;
    /*测试掉的正式代码*/ move_test_D.zs_translate_compound_O(fa, fv); /// 3.0   19float angle,float speed
    /*测试掉的正式代码*/ move_test_D.limit_parameter_set_dis(
        72, 72); // 2.5+2.5*lf(0,45,fabs(RMfv)));//(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
                 ///*测试旋转量在每周期产生角度的代码*/	Angle_add(zqw,&faTest);//一 个周期转动量 造成的转动角度
    ///*测试旋转量在每周期产生角度的代码*/	move_test_D  .zs_translate_compound_O(faTest,10);///3.0   19float
    /// angle,float speed
    //	/*测试代码*/	move_test_D  .initializing_movement(0,0.1,0);//(float turn,float front,float angle)
    //	/*测试代码*/	move_test_D  .zs_angle_track_001(ball_angle,0/*-120benchmark_angle/*______ */,black_white_ball);
    /*测试代码*/ move_test_D.turn_v_maxlimit(); //
    //	/*测试代码*/	move_test_D  .zs_point_track_01_002(ball_angle,ball_Rradius, ball_angle, 200);//ball_angle/800
    move_test_D.front_v_angle_p = robot_fva; // 将上周期的速度分量辅给
    move_test_D.front_v_p = robot_fv;
    move_test_D.front_a_maxlimit();
    move_test_D.front_v_maxlimit();                   //
    move_test_D.uptech_output_O(motoB, motoL, motoR); // ？？？？？？？？？？？？？？
    float mtBt1 = 0;
    float mtLt1 = 0;
    float mtRt1 = 0;
    //	move_test_D  .uptech_output_O(&mtBt1,&mtLt1,&mtRt1);//？？？？？？？？？？？？？？
    float mtBt2 = 0;
    float mtLt2 = 0;
    float mtRt2 = 0;

    if (0) // ball_estimate_radius>800)//(1)//250)//1)//球距离比较远 不使用速度跟踪if(ball_estimate_angle>0
    {
        zqmb = 0;
        zqml = 0;
        zqmr = 0;
        float rin = 150;
        float rout = 220;
        float xs = (ball_estimate_radius - rin) / (rout - rin);
        if (xs > 1)
            xs = 1;
        if (xs < 0)
            xs = 0;
        float v = 10 + xs * 40; // 基础速度10 比例增加最大40
        move_and_target(ball_estimate_angle, v, ball_estimate_angle, motoB, motoL,
                        motoR); // 有问题 在 反解函数里 用于向指定方向平移(本机极坐标)并 对准指定目标(本机极坐标)
        //	move_and_target(ball_estimate_angle,v,ball_estimate_angle,&mtBt2,&mtLt2,&mtRt2);//有问题 在 反解函数里
        // 用于向指定方向平移(本机极坐标)并 对准指定目标(本机极坐标)
        //		ni_uptech_output_O(&zqva,&zqv,&zqw,zqmb,zqml,zqmr);//逆转换 将博创底盘 电机输出转换成
        // 128体系输出的运动矢量
    }
    /*
float a1=0.5;
float a2=1-a1;
motoB=mtBt1*a1+mtBt2*a2;
motoL=mtLt1*a1+mtLt2*a2;
motoR=mtRt1*a1+mtRt2*a2;
*/
}

void Dribble::Ball_Track2(float *motoB, float *motoL, float *motoR)
{
    // float   ball_estimate_angle ;ball_estimate_radius//运动估算后的输出
    // float   ball_estimate_F_angle ;ball_estimate_F_radius//前景运动估算后的输出
    float zqmb, zqml, zqmr;

    // Ball_Into_Holder(ball_angle,ball_Rradius);
    Ball_Into_Holder2(ball_estimate_angle, ball_estimate_radius, ball_estimate_F_angle, ball_estimate_F_radius);
    // Ball_Into_Holder2(ball_angle,ball_Rradius,ball_op_Fa,ball_op_Fr);
    // Ball_Into_Holder2(ball_estimate_angle,ball_estimate_radius,ball_op_Fa,ball_op_Fr);
    move_test_C.uptech_output_O(&zqmb, &zqml, &zqmr); // 对Ball_Into_Holder进行输出
    // 如果要加 可以在这里加入 球的平移矢量fv，Front_aC
    // 运动反解
    float zqva, zqv, zqw, zqv_x, zqv_y;
    zqva = move_test_C.front_v_angle_c;
    zqv = move_test_C.front_v_c;
    zqw = move_test_C.turn_v_c;
    zqv_x = coordinate_transfer_x(zqva + robot_L.f_a, zqv);
    zqv_y = coordinate_transfer_y(zqva + robot_L.f_a, zqv);

    // 调试代码
    // zqw   =0; zqv_x =0; zqv_y =0;
    zqw = zqw * 0.99; // 0.6;

    float bili = 0.001 * 1.5;
    zqv_x = zqv_x * bili;
    zqv_y = zqv_y * bili;
    fount_xVset += (Ball_Track_x_diff(ball_d_h[0].f_x - robot_L.f_x, 10) + zqv_x) * 30; // 注意这里也应改成估算后的值
    fount_yVset += (Ball_Track_y_diff(ball_d_h[0].f_y - robot_L.f_y, 10) + zqv_y) * 30;
    // v=
    if (0) //(valxx!=1)///*调试球进持球器*/?????????????!!!!!!!!!!!!!!!全局????????????????????????????????????!!!!!!!!!!
    {
        fount_xVset = 0;
        fount_yVset = 0;
    }

    //	float bilixishu=6.0;
    ///*调试球进持球器*/	fv =
    /// coordinate_transfer_radius(fount_xVset+zqv_x*bili,fount_yVset+zqv_y*bili);//(fount_xVset+
    /// dis_xct*bilixishu,fount_yVset+ dis_yct*bilixishu); *调试球进持球器*/	fa = coordinate_transfer_angle
    ///(fount_xVset+zqv_x*bili,fount_yVset+zqv_y*bili);//(fount_xVset+ dis_xct*bilixishu,fount_yVset+
    /// dis_yct*bilixishu);
    fv = coordinate_transfer_radius(fount_xVset,
                                    fount_yVset); //(fount_xVset+ dis_xct*bilixishu,fount_yVset+ dis_yct*bilixishu);
    fa = coordinate_transfer_angle(fount_xVset,
                                   fount_yVset); //(fount_xVset+ dis_xct*bilixishu,fount_yVset+ dis_yct*bilixishu);
    ///*调试*/	fv = coordinate_transfer_radius(zqv_x,zqv_y);//(fount_xVset+ dis_xct*bilixishu,fount_yVset+
    /// dis_yct*bilixishu); *调试*/	fa = coordinate_transfer_angle (zqv_x,zqv_y);//(fount_xVset+
    /// dis_xct*bilixishu,fount_yVset+ dis_yct*bilixishu);
    if (fv > 30) // 可调值 限制最大平移速度
        fv = 30;
    fount_xVset = coordinate_transfer_x(fa, fv);
    fount_yVset = coordinate_transfer_y(fa, fv);

    fa = angle_add_transfer(fa, -robot_L.f_a);
    Angle_add(zqw * 0.7, &fa); // 一个周期转动量 造成的转动角度

    move_test_D.limit_parameter_set_angle(10,
                                          1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
    move_test_D.limit_parameter_set_dis(
        10, 5); //(15, 5);//(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
    move_test_D.parameter_set_angle(0.2, 0, 0, 0);
    move_test_D.parameter_set_dis(0.2, 0, 0.5, 0); // 0.3

    move_test_D.initializing_movement(0, 0, 0); //(float turn,float front,float angle)
    move_test_D.turn_v_c = zqw;
    /*测试掉的正式代码*/ move_test_D.zs_translate_compound_O(fa, fv); /// 3.0   19float angle,float speed
    /*测试掉的正式代码*/ move_test_D.limit_parameter_set_dis(
        40, 16); //(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
    ///*测试旋转量在每周期产生角度的代码*/	Angle_add(zqw,&faTest);//一 个周期转动量 造成的转动角度
    ///*测试旋转量在每周期产生角度的代码*/	move_test_D  .zs_translate_compound_O(faTest,10);///3.0   19float
    /// angle,float speed
    //	/*测试代码*/	move_test_D  .initializing_movement(0,0.1,0);//(float turn,float front,float angle)
    //	/*测试代码*/	move_test_D  .zs_angle_track_001(ball_angle,0/*-120benchmark_angle/*______ */,black_white_ball);
    /*测试代码*/ move_test_D.turn_v_maxlimit(); //
    //	/*测试代码*/	move_test_D  .zs_point_track_01_002(ball_angle,ball_Rradius, ball_angle, 200);//ball_angle/800
    /*测试代码*/ move_test_D.front_v_maxlimit();      //
    move_test_D.uptech_output_O(motoB, motoL, motoR); // ？？？？？？？？？？？？？？
}

void Dribble::Ball_Track(float ball_angle, float ball_Rradius, float *motoB, float *motoL, float *motoR)
{
    float zqmb, zqml, zqmr;

    // Ball_Into_Holder(ball_angle,ball_Rradius);
    Ball_Into_Holder2(ball_angle, ball_Rradius, ball_op_Fa, ball_op_Fr);
    move_test_C.uptech_output_O(&zqmb, &zqml, &zqmr); // 对Ball_Into_Holder进行输出
    // 如果要加 可以在这里加入 球的平移矢量fv，Front_aC
    // 运动反解
    float zqva, zqv, zqw, zqv_x, zqv_y;
    zqva = move_test_C.front_v_angle_c;
    zqv = move_test_C.front_v_c;
    zqw = move_test_C.turn_v_c;
    // move_test_C.O_movement_T_analyze(&zqva,&zqv,&zqw,-zqml,-zqmb,-zqmr);// *move_v_angle, *move_v_speed, *turn_v
    // zqv   = zqv*32.6/wheel_linear_speed_ratio;
    // zqw   = zqw*32.6/wheel_linear_speed_ratio;//wheel_rotate_speed_ratio;
    // ni_uptech_output_O(&zqva,&zqv,&zqw,zqmb,zqml,zqmr);
    zqv_x = coordinate_transfer_x(zqva + robot_L.f_a, zqv);
    zqv_y = coordinate_transfer_y(zqva + robot_L.f_a, zqv);

    // 调试代码
    // zqw   =0;
    // zqv_x =0;
    // zqv_y =0;
    float bili = 0.3; //
    zqw = zqw * 0.99; // 0.6;
    // fount_xVset+=Ball_Track_x_diff(ball_d_h[0].f_x-robot_L.f_x,8)*30+zqv_x*bili;
    // fount_yVset+=Ball_Track_y_diff(ball_d_h[0].f_y-robot_L.f_y,8)*30+zqv_y*bili;
    bili = 0.001 * 1.5;
    zqv_x = zqv_x * bili;
    zqv_y = zqv_y * bili;
    fount_xVset += (Ball_Track_x_diff(ball_d_h[0].f_x - robot_L.f_x, 8) + zqv_x) * 30;
    fount_yVset += (Ball_Track_y_diff(ball_d_h[0].f_y - robot_L.f_y, 8) + zqv_y) * 30;
    if (0) //(valxx!=1)///*调试球进持球器*/?????????????!!!!!!!!!!!!!!!全局????????????????????????????????????!!!!!!!!!!
    {
        fount_xVset = 0;
        fount_yVset = 0;
    }
    //	fount_xVset =10;
    //	fount_yVset =0;

    //	float bilixishu=6.0;
    ///*调试球进持球器*/	fv =
    /// coordinate_transfer_radius(fount_xVset+zqv_x*bili,fount_yVset+zqv_y*bili);//(fount_xVset+
    /// dis_xct*bilixishu,fount_yVset+ dis_yct*bilixishu); *调试球进持球器*/	fa = coordinate_transfer_angle
    ///(fount_xVset+zqv_x*bili,fount_yVset+zqv_y*bili);//(fount_xVset+ dis_xct*bilixishu,fount_yVset+
    /// dis_yct*bilixishu);
    fv = coordinate_transfer_radius(fount_xVset,
                                    fount_yVset); //(fount_xVset+ dis_xct*bilixishu,fount_yVset+ dis_yct*bilixishu);
    fa = coordinate_transfer_angle(fount_xVset,
                                   fount_yVset); //(fount_xVset+ dis_xct*bilixishu,fount_yVset+ dis_yct*bilixishu);
                                                 ///*调试球进持球器*/	fa = fa+zqw*2;//angle_add_transfer(fa,zqw*2);
    ///*调试*/	fv = coordinate_transfer_radius(zqv_x,zqv_y);//(fount_xVset+ dis_xct*bilixishu,fount_yVset+
    /// dis_yct*bilixishu); *调试*/	fa = coordinate_transfer_angle (zqv_x,zqv_y);//(fount_xVset+
    /// dis_xct*bilixishu,fount_yVset+ dis_yct*bilixishu);
    if (fv > 30)
        fv = 30;
    fount_xVset = coordinate_transfer_x(fa, fv);
    fount_yVset = coordinate_transfer_y(fa, fv);

    fa = angle_add_transfer(fa, -robot_L.f_a);
    Angle_add(zqw, &fa); // 一个周期转动量 造成的转动角度

    //	fa = angle_add_transfer(fa,-robot_L.f_a-zqw*16);
    //	fa = angle_add_transfer(fa,-robot_L.f_a+zqw*1.5);

    move_test_D.limit_parameter_set_angle(10,
                                          1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
    move_test_D.limit_parameter_set_dis(
        10, 5); //(15, 5);//(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
    move_test_D.parameter_set_angle(0.2, 0, 0, 0);
    move_test_D.parameter_set_dis(0.2, 0, 0.5, 0); // 0.3

    move_test_D.initializing_movement(0, 0, 0); //(float turn,float front,float angle)
    move_test_D.turn_v_c = zqw;
    /*测试掉的正式代码*/ move_test_D.zs_translate_compound_O(fa, fv); /// 3.0   19float angle,float speed
    /*测试掉的正式代码*/ move_test_D.limit_parameter_set_dis(
        40, 16); //(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
    //	/*测试代码*/	move_test_D  .initializing_movement(0,0.1,0);//(float turn,float front,float angle)
    //	/*测试代码*/	move_test_D  .zs_angle_track_001(ball_angle,0/*-120benchmark_angle/*______ */,black_white_ball);
    /*测试代码*/ move_test_D.turn_v_maxlimit(); //
    //	/*测试代码*/	move_test_D  .zs_point_track_01_002(ball_angle,ball_Rradius, ball_angle, 200);//ball_angle/800
    /*测试代码*/ move_test_D.front_v_maxlimit();      //
    move_test_D.uptech_output_O(motoB, motoL, motoR); // ？？？？？？？？？？？？？？

    // 代码转到周期电机输出最后
    //	ni_uptech_output_O(&robot_fva,&robot_fv,&robot_w,*motoB,*motoL,*motoR);//机器的上周期的运动参数 （累加）
}

void Dribble::Ball_Into_Holder2(float ball_angle, float ball_Rradius, float ball_FAngle, float ball_FRradius)
{
    if (1)
    {
        float rin = 60;  // 在前进量60时得到7  的最大转动量
        float rout = 50; // 在前进量15时得到7+9的最大转动量
        float xs = (fabs(robot_fv) - rin) / (rout - rin);
        if (xs < 0)
            xs = 0;
        if (xs > 1)
            xs = 1;
        move_test_C.limit_parameter_set_angle(
            7.0 + xs * 9.0, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
    }
    move_test_C.limit_parameter_set_dis(
        200, 1); //(10, 5);//(15, 5);//(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
    float ax =
        0.85; // 1.75;//2.4;//3.0;//0.17;//0.51*4.8*0.09;//0.51*1.5;//1.2;//5;//0.45;//0.39;//角速度修正量 缩减系数
    float dx = 0.2;                 // 0.056;//2.4*0.015 ;//1;//0.80;//1;//2.8;			//平移修正量  缩减系数
    float angle_p_1 = 0.18 * ax;    // 0.140*ax;  //0.35*ax;//;
    float angle_i_1 = 0;            // 0.0002*ax;//0.005*ax;  //0.06*ax;//0;
    float angle_d_1 = -0.0070 * ax; //-0.008*ax;//0-0.028*ax;//0
    float dis_p_1 = 5.50 * dx;
    float dis_i_1 = 0;
    float dis_d_1 = 0; // 15.00*dx;
    move_test_C.parameter_set_angle(angle_p_1, angle_i_1, angle_d_1, 0);
    move_test_C.parameter_set_dis(dis_p_1, dis_i_1, dis_d_1, 0); // 0.3

    // float holdballmin=19.3;//持球的 距离
    // float holdballmax=26;//持球的 距离
    // float holdballangle=6;//持球 的角度
    char black_white_ball[20];
    // float motor1,motor2,motor3;
    // float mb,ml,mr;

    // 组成1
    // 在前景视野外的大范围里
    float radius_in = 23;  // 最近躲避持球器距离
    float radius_out = 23; // 30;//最远躲避持球器距离
    float angle_in = 20;   // 进入持球器角度
    float angle_out = 60;  // 最大远离持球器角度

    // 组成2 所有pid 都可以相应 上调
    // 在前景视野内小范围里	(FHaveBall==1)//(Dribble_ball.ball_op_Fweight>1000)
    float radiusF_in = 0; // holdballFmin-10;//最远躲避持球器距离
    float radiusF_out =
        39;                // 14;//radius_in	;//最近躲避持球器距离 =组成1 里的radius_in 但要比radius_in 小一点在物理上
    float angleF_in = 1.0; // 进入持球器底部角度
    float angleF_out = 9;  // 最大远离持球器角度

    move_test_C.initializing_movement(0, 0, 0); //(float turn,float front,float angle)
    if (FHaveBall == 0)                         //(ball_op_Fweight<1000)
    {
        // 组成1
        // 在前景视野外的大范围里
        // R-rin/a-ain=rout-rin/aout-ain
        // R-rin=(rout-rin)(a-ain)/(aout-ain)
        float Rbenchmark; //=基准长度
        if (fabs(ball_angle) < angle_in)
            Rbenchmark = radius_in;
        if ((fabs(ball_angle) >= angle_in) && (fabs(ball_angle) < angle_out))
            Rbenchmark = radius_in + (radius_out - radius_in) * (ball_angle - angle_in) / (angle_out - angle_in);
        if (fabs(ball_angle) >= angle_out)
            Rbenchmark = radius_out;
        move_test_C.zs_point_track_01_002(ball_angle, ball_Rradius, ball_angle, Rbenchmark);
        move_test_C.zs_angle_track_001(ball_angle, 0 /*benchmark_angle*/, black_white_ball);
        //	move_test_C.zs_angle_track_002	 (ball_angle,0);

        move_test_C.turn_v_maxlimit();  // 限定平移[功率]速度输出最大值
        move_test_C.front_v_maxlimit(); // 限定平移[功率]速度输出最大值
        //??????????????????????????????????????????????????????????????
    }
    else
    {

        // 组成2 所有pid 都可以相应 上调
        // 在前景视野内小范围里	(FHaveBall==1)//(Dribble_ball.ball_op_Fweight>1000)
        float fa = 1.1;
        float fd = 2.0;
        move_test_C.parameter_set_angle(angle_p_1 * fa, angle_i_1 * fa, angle_d_1 * fa, 0);
        move_test_C.parameter_set_dis(dis_p_1 * fd, dis_i_1 * fd, dis_d_1 * fd, 0); // 0.3
        float RFbenchmark;                                                          //=基准长度
        if (fabs(ball_FAngle) < angleF_in)
            RFbenchmark = radiusF_in;
        if ((fabs(ball_FAngle) >= angleF_in) && (fabs(ball_FAngle) < angleF_out))
            RFbenchmark = radiusF_in + (radiusF_out - radiusF_in) * (ball_op_Fa - angleF_in) / (angleF_out - angleF_in);
        if (fabs(ball_FAngle) >= angleF_out * 2)
            RFbenchmark = radiusF_out;
        move_test_C.zs_point_track_01_002(ball_FAngle, ball_FRradius, ball_FAngle, RFbenchmark);
        move_test_C.zs_angle_track_001(ball_FAngle, 0 /*-120benchmark_angle/*______ */, black_white_ball);
        //	move_test_C .zs_angle_track_002	  (ball_FAngle,0);
        move_test_C.turn_v_maxlimit();  //
        move_test_C.front_v_maxlimit(); // 限定平移[功率]速度输出最大值
    }

    float a_angle = 15; // 加速角 进了这个角度就进行加速
    // 判断进入加速区域 为了方便持球
    if ((FHaveBall == 1) && (fabs(ball_FAngle) < a_angle) && (ball_FRradius < 50) &&
        (ball_FRradius > 30)) //(ball_op_Fweight>1000)
    {                         // 进入加速角 进行加速
        jifen += 4;
        // 进行收角时序
        //	track_angle(&robot_fva,0,4.0);
        //	fount_xVset =coordinate_transfer_x (robot_fva+robot_L.f_a,robot_fv);
        //	fount_yVset =coordinate_transfer_y (robot_fva+robot_L.f_a,robot_fv);
    }
    else
    {
        jifen = 0;
    }
    float rin = 0;      // 40;
    float rout = 10;    // 60;
    if (FHaveBall == 1) //(ball_op_Fweight>1000)
    {
        float xs = (ball_FRradius - rin) / (rout - rin);
        if (xs < 0)
            xs = 0;
        if (xs > 1)
            xs = 1;
        if (fabs(ball_FAngle) < a_angle)
            //			move_test_C .zs_translate_compound_O(ball_FAngle*(xs),54+jifen);//0.8将当前运动向量与
            // 另一个平移分量相加
            move_test_C.zs_translate_compound_O(ball_FAngle * 1.4,
                                                54 + jifen); // 0.8将当前运动向量与  另一个平移分量相加
    }
    else
    {
        float xs = (ball_Rradius - rin) / (rout - rin);
        if (xs < 0)
            xs = 0;
        if (xs > 1)
            xs = 1;
        if (fabs(ball_angle) < a_angle)
            //			move_test_C .zs_translate_compound_O(ball_angle*(xs),54+jifen);//0.8将当前运动向量与
            // 另一个平移分量相加
            move_test_C.zs_translate_compound_O(ball_angle * 1.4, 54 + jifen); // 0.8将当前运动向量与 另一个平移分量相加
    }
    move_test_C.turn_v_maxlimit();  //
    move_test_C.front_v_maxlimit(); // 限定平移[功率]速度输出最大值

    // ball_FRradius R1  R2   (x-r2)*1/(r1-r2)=y   y=(x-r2)*1/(r1-r2)
}
// 10:18  11:16 剩24% 14min
void Dribble::Ball_Into_Holder4(float ball_angle, float ball_Rradius, float ball_FAngle, float ball_FRradius)
{

    char black_white_ball[20];
    // 组成1
    // 在前景视野外的大范围里
    float radius_in = 6;              // 23;//最近躲避持球器距离
    float radius_out = 6;             // 23;//30;//最远躲避持球器距离
    float angle_in = 20;              // 进入持球器角度
    float angle_out = 60;             // 最大远离持球器角度
                                      // 组成2 所有pid 都可以相应 上调
                                      // 在前景视野内小范围里	(FHaveBall==1)//(Dribble_ball.ball_op_Fweight>1000)
    float radiusF_in = holdballFmin;  // 6//30;//holdballFmin-10;//最远躲避持球器距离
    float radiusF_out = holdballFmin; // 6//39;//14;//radius_in	;//最近躲避持球器距离 =组成1 里的radius_in
                                      // 但要比radius_in 小一点在物理上
    float angleF_in = 1.0;            // 进入持球器底部角度
    float angleF_out = 9;             // 最大远离持球器角度

    move_test_C.initializing_movement(0, 0, 0); //(float turn,float front,float angle)
    if (FHaveBall == 1)                         //(1)//(0)//(ball_op_Fweight>1000)
    {
        float rin = 9;  // 12.0;//在前进量60时得到7  的最大转动量
        float rout = 1; // 在前进量15时得到7+9的最大转动量
        float xs = (fabs(robot_fv) - rin) / (rout - rin);
        if (xs < 0)
            xs = 0;
        if (xs > 1)
            xs = 1;
        move_test_C.limit_parameter_set_angle(
            5.0 + xs * 5.0, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
        move_test_C.limit_parameter_set_dis(
            200, 1); //(10, 5);//(15, 5);//(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
                     //    float
        //    ax=0.45+xs*0.35;//0.6..0.2//0.75;//0.85;//1.75;//2.4;//3.0;//0.17;//0.51*4.8*0.09;//0.51*1.5;//1.2;//5;//0.45;//0.39;//角速度修正量
        //    缩减系数
        float ax =
            0.5 +
            xs *
                0.40; // 0.6..0.2//0.75;//0.85;//1.75;//2.4;//3.0;//0.17;//0.51*4.8*0.09;//0.51*1.5;//1.2;//5;//0.45;//0.39;//角速度修正量
                      // 缩减系数
        float dx = 0.15; // 0.056;//2.4*0.015 ;//1;//0.80;//1;//2.8;			//平移修正量  缩减系数
        float fa = 1;
        if (ball_op_Fr < 45) //(0)//(ball_FRradius<45)//(0)//((fabs(ball_FAngle)>15)&&(ball_FRradius<45))
        {
            fa = fa * 1.6;
            move_test_C.limit_parameter_set_angle(
                10, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
        }
        float angle_p_1 = 0.19 * ax * fa; // 0.180*ax;
        float angle_i_1 = 0;              // 0.3*ax;//0.0002*ax;//0.005*ax;  //0.06*ax;//0;
        float angle_d_1 = -0.0070 * ax;   //-0.0070*ax;//
        float dis_p_1 = 5.50 * dx;
        move_test_C.parameter_set_angle(angle_p_1, angle_i_1, angle_d_1, 0);
        move_test_C.parameter_set_dis(dis_p_1, 0, 0, 0); // 0.3
        // 组成2 所有pid 都可以相应 下调
        // 在前景视野内小范围里	(FHaveBall==1)//(Dribble_ball.ball_op_Fweight>1000)
        float RFbenchmark; //=基准长度
        if (fabs(ball_op_Fa) < angleF_in)
            RFbenchmark = radiusF_in;
        if ((fabs(ball_op_Fa) >= angleF_in) && (fabs(ball_op_Fa) < angleF_out))
            RFbenchmark = radiusF_in + (radiusF_out - radiusF_in) * (ball_op_Fa - angleF_in) / (angleF_out - angleF_in);
        if (fabs(ball_op_Fa) >= angleF_out * 2)
            RFbenchmark = radiusF_out;
        move_test_C.zs_point_track_01_002(ball_op_Fa, ball_op_Fr, ball_op_Fa * 0.3, RFbenchmark); // ball_op_Fa
        move_test_C.zs_angle_track_001(ball_op_Fa, 0 /*-120benchmark_angle/*______ */, black_white_ball);
        //	move_test_C .zs_angle_track_002	  (ball_FAngle,0);

        //	float	a_angle=22;//15;
        // 判断进入加速区域 为了方便持球
        // 用前景 控制 在持球器范围内加速部分
        float rin2 = 60;  // 65;//球在65cm以外距离时得到0  的附加平移角度
        float rout2 = 40; // 球在40cm以内距离时得到1  的附加平移角度
        float xs2 = 0;
        xs2 = (ball_op_Fr - rin2) / (rout2 - rin2);
        if (xs2 < 0)
            xs2 = 0;
        if (xs2 > 1)
            xs2 = 1;
        //+xs2*0.00

        if (jifen > 40)
            jifen = 40; //
        if (jifen < 0)
            jifen = 0; ////加速区 进了这个区就进行加速
        if ((FHaveBall == 1) && (fabs(ball_op_Fa) < 20) && (ball_op_Fr < 50) && (ball_op_Fr > 10)) //
        //(0)//	 ||(FHaveBall==1)&&(fabs(ball_op_Fa)<26)&&(ball_op_Fr<50)&&(ball_op_Fr>42))//(ball_op_Fweight>1000)
        { // 进入加速角 进行加速
            jifen += 8.0;
            //		move_test_C .zs_translate_compound_O(ball_op_Fa*(1+0.80*xs2),32+jifen);//25,54,(1+0.9*xs)
            // 0.8将当前运动向量与  另一个平移分量相加 		move_test_C
            //.zs_translate_compound_O(move_test_C.front_v_angle_c*(1+0.00*xs2),25+jifen);//25,54,(1+0.9*xs)
            // 0.8将当前运动向量与  另一个平移分量相加
            move_test_C.zs_translate_compound_O(move_test_C.front_v_angle_c * (1 + 0.00 * xs2),
                                                24 + jifen); // 25,54,(1+0.9*xs)  0.8将当前运动向量与 另一个平移分量相加
        }
        else
        {
            jifen -= 8.0 * 2.0; // 0;//
        }

        move_test_C.turn_v_maxlimit();  //
        move_test_C.front_v_maxlimit(); // 限定平移[功率]速度输出最大值
    }
    else
    {
        float rin = 9;  // 12.0;//在前进量60时得到7  的最大转动量
        float rout = 1; // 在前进量15时得到7+9的最大转动量
        float xs = (fabs(robot_fv) - rin) / (rout - rin);
        if (xs < 0)
            xs = 0;
        if (xs > 1)
            xs = 1;
        move_test_C.limit_parameter_set_angle(
            5.0 + xs * 5.0, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
        move_test_C.limit_parameter_set_dis(
            200, 1); //(0, 1);//(10, 5);//(15, 5);//(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
                     //   float
        //   ax=0.35+xs*0.45;//0.6..0.2//0.75;//0.85;//1.75;//2.4;//3.0;//0.17;//0.51*4.8*0.09;//0.51*1.5;//1.2;//5;//0.45;//0.39;//角速度修正量
        //   缩减系数
        float ax =
            0.6 +
            xs *
                0.4; // 0.25+xs*0.40;//0.6..0.2//0.75;//0.85;//1.75;//2.4;//3.0;//0.17;//0.51*4.8*0.09;//0.51*1.5;//1.2;//5;//0.45;//0.39;//角速度修正量
                     // 缩减系数
        float dx = 0.2;              // 0.056;//2.4*0.015 ;//1;//0.80;//1;//2.8;			//平移修正量  缩减系数
        float angle_p_1 = 0.19 * ax; // 0;// 0.180*ax;
        float angle_i_1 = 0;         //-0.0000120;//0.0000510;//0.3*ax;//0;//0.005*ax;  //0.06*ax;//0;
        float angle_d_1 = 0;         //-0.0450*ax;//-0.0450*ax;//0;//-0.0070*ax;//
        float dis_p_1 = 5.50 * dx;
        move_test_C.parameter_set_angle(angle_p_1, angle_i_1, angle_d_1, 0);
        move_test_C.parameter_set_dis(dis_p_1, 0, 0, 0); // 0.3
        // 组成1
        // 在前景视野外的大范围里
        // R-rin/a-ain=rout-rin/aout-ain
        // R-rin=(rout-rin)(a-ain)/(aout-ain)
        float Rbenchmark; //=基准长度
        if (fabs(ball_angle) < angle_in)
            Rbenchmark = radius_in;
        if ((fabs(ball_angle) >= angle_in) && (fabs(ball_angle) < angle_out))
            Rbenchmark = radius_in + (radius_out - radius_in) * (ball_angle - angle_in) / (angle_out - angle_in);
        if (fabs(ball_angle) >= angle_out)
            Rbenchmark = radius_out;
        move_test_C.zs_point_track_01_002(ball_angle, ball_Rradius, ball_angle * 0.30, Rbenchmark); // ball_angle
        move_test_C.zs_angle_track_001(ball_angle, 0 /*benchmark_angle*/, black_white_ball);
        //	move_test_C.zs_angle_track_002	 (ball_angle,0);

        move_test_C.zs_translate_compound_O(move_test_C.front_v_angle_c,
                                            12); // 4825,54,(1+0.9*xs)  0.8将当前运动向量与  另一个平移分量相加

        move_test_C.turn_v_maxlimit();  // 限定平移[功率]速度输出最大值
        move_test_C.front_v_maxlimit(); // 限定平移[功率]速度输出最大值

        //??????????????????????????????????????????????????????????????
    }

    // if (FHaveBall==1)//(ball_op_Fweight>1000)
    //{
    // }
    // else
    //{//	if(fabs(ball_angle)<a_angle)
    //	{	xs=(ball_Rradius-rin)/(rout-rin);
    //		if (xs<0) xs=0;
    //		if (xs>1) xs=1;
    //		move_test_C .zs_translate_compound_O(ball_angle*(1+0.7*xs),54+jifen);//0.8将当前运动向量与
    // 另一个平移分量相加
    //	}
    // }
    //	move_test_C .turn_v_maxlimit();//
    //	move_test_C	.front_v_maxlimit();//限定平移[功率]速度输出最大值
}
void Dribble::Ball_Into_Holder4_(void)
{                   // ball_estimate_angle,ball_estimate_radius,ball_estimate_F_angle,ball_estimate_F_radius
    float rin = 12; // 11;//12.0;//在前进量60时得到7  的最大转动量
    float rout = 4; // 3;//在前进量15时得到7+9的最大转动量
    float xs = (fabs(robot_fv) - rin) / (rout - rin);
    if (xs < 0)
        xs = 0;
    if (xs > 1)
        xs = 1;

    move_test_C.limit_parameter_set_angle(7, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
    move_test_C.limit_parameter_set_dis(
        200, 1); //(10, 5);//(15, 5);//(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
                 //   float
    //   ax=0.35+xs*0.45;//0.6..0.2//0.75;//0.85;//1.75;//2.4;//3.0;//0.17;//0.51*4.8*0.09;//0.51*1.5;//1.2;//5;//0.45;//0.39;//角速度修正量
    //   缩减系数
    float ax =
        0.25 +
        xs *
            0.40; // 0.3+xs*0.40;//0.6..0.2//0.75;//0.85;//1.75;//2.4;//3.0;//0.17;//0.51*4.8*0.09;//0.51*1.5;//1.2;//5;//0.45;//0.39;//角速度修正量
                  // 缩减系数
    float dx = 0.2;                 // 0.056;//2.4*0.015 ;//1;//0.80;//1;//2.8;			//平移修正量  缩减系数
    float angle_p_1 = 0.19 * ax;    // 0.180*ax;
    float angle_i_1 = 0;            // 0.0002*ax;//0.005*ax;  //0.06*ax;//0;
    float angle_d_1 = -0.0070 * ax; //-0.0070*ax;//
    float dis_p_1 = 5.50 * dx;
    float dis_i_1 = 0;
    float dis_d_1 = 0; // 15.00*dx;
    move_test_C.parameter_set_angle(angle_p_1, angle_i_1, angle_d_1, 0);
    move_test_C.parameter_set_dis(dis_p_1, dis_i_1, dis_d_1, 0); // 0.3
    char black_white_ball[20];
    // 组成1
    // 在前景视野外的大范围里
    float radius_in = 23; // 最近躲避持球器距离
    // float	radius_out 	=23;//30;//最远躲避持球器距离
    // float	angle_in	=20;//进入持球器角度
    // float	angle_out	=60;//最大远离持球器角度
    // 组成2 所有pid 都可以相应 上调
    // 在前景视野内小范围里	(FHaveBall==1)//(Dribble_ball.ball_op_Fweight>1000)
    float radiusF_in = 0; // holdballFmin-10;//最远躲避持球器距离
    float radiusF_out =
        39;                // 14;//radius_in	;//最近躲避持球器距离 =组成1 里的radius_in 但要比radius_in 小一点在物理上
    float angleF_in = 1.0; // 进入持球器底部角度
    float angleF_out = 9;  // 最大远离持球器角度

    move_test_C.initializing_movement(0, 0, 0); //(float turn,float front,float angle)
    if (FHaveBall == 1)                         //(0)//(ball_op_Fweight>1000)
    {
        move_test_C.limit_parameter_set_angle(
            6, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
        move_test_C.limit_parameter_set_dis(
            200, 1); //(10, 5);//(15, 5);//(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
                     // 组成2 所有pid 都可以相应 上调
        // 在前景视野内小范围里	(FHaveBall==1)//(Dribble_ball.ball_op_Fweight>1000)
        float fa = 0.99; // 0.86;
        float fd = 1.5;
        if (ball_op_Fr < 43) //((fabs(ball_op_Fa)>15)&&(ball_op_Fr<45))
        {
            fa = fa * 2.0;
            move_test_C.limit_parameter_set_angle(
                10, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
        }
        move_test_C.parameter_set_angle(angle_p_1 * fa, angle_i_1 * fa, angle_d_1 * fa, 0);
        move_test_C.parameter_set_dis(dis_p_1 * fd, dis_i_1 * fd, dis_d_1 * fd, 0); // 0.3
        float RFbenchmark;                                                          //=基准长度
        if (fabs(ball_op_Fa) < angleF_in)
            RFbenchmark = radiusF_in;
        if ((fabs(ball_op_Fa) >= angleF_in) && (fabs(ball_op_Fa) < angleF_out))
            RFbenchmark = radiusF_in + (radiusF_out - radiusF_in) * (ball_op_Fa - angleF_in) / (angleF_out - angleF_in);
        if (fabs(ball_op_Fa) >= angleF_out * 2)
            RFbenchmark = radiusF_out;
        move_test_C.zs_point_track_01_002(ball_estimate_F_angle, ball_estimate_F_radius, ball_estimate_F_angle,
                                          RFbenchmark);
        move_test_C.zs_angle_track_001(ball_estimate_F_angle, 0 /*-120benchmark_angle/*______ */, black_white_ball);
        //	move_test_C .zs_angle_track_002	  (ball_FAngle,0);

        // 判断进入加速区域 为了方便持球
        // 用前景 控制 在持球器范围内加速部分
        float rin2 = 60;  // 65;//球在65cm以外距离时得到0  的附加平移角度
        float rout2 = 40; // 球在40cm以内距离时得到1  的附加平移角度
        float xs2 = 0;
        xs2 = (ball_op_Fr - rin2) / (rout2 - rin2);
        if (xs2 < 0)
            xs = 0;
        if (xs2 > 1)
            xs = 1;

        if (jifen > 40)
            jifen = 40; //
        if (jifen < 0)
            jifen = 0; ////20  26加速角 进了这个角度就进行加速
        if ((FHaveBall == 1) && (fabs(ball_op_Fa) < 20) && (ball_op_Fr < 50) && (ball_op_Fr > 10) ||
            (FHaveBall == 1) && (fabs(ball_op_Fa) < 26) && (ball_op_Fr < 50) &&
                (ball_op_Fr > 42)) //(ball_op_Fweight>1000)
        {                          // 进入加速角 进行加速
            jifen += 4.0;
            move_test_C.zs_translate_compound_O(ball_op_Fa * (1 + 0.45 * xs2),
                                                32 + jifen); // 25,54,(1+0.9*xs)  0.8将当前运动向量与 另一个平移分量相加
        }
        else
        {
            jifen -= 4.0 * 1.6;
        }

        move_test_C.turn_v_maxlimit();  //
        move_test_C.front_v_maxlimit(); // 限定平移[功率]速度输出最大值
    }
    else
    {
        // 组成1
        // 在前景视野外的大范围里
        // R-rin/a-ain=rout-rin/aout-ain
        // R-rin=(rout-rin)(a-ain)/(aout-ain)
        float Rbenchmark = radius_in; //=基准长度
                                      //	if (fabs(ball_org_angle)<angle_in)
                                      //		Rbenchmark=radius_in;
                                      //	if((fabs(ball_org_angle)>=angle_in)&&(fabs(ball_org_angle)<angle_out))
        //		Rbenchmark = radius_in + (radius_out-radius_in)*(ball_org_angle -angle_in)/(angle_out-angle_in);
        //	if (fabs(ball_org_angle)>=angle_out)
        //		Rbenchmark=radius_out;
        move_test_C.zs_point_track_01_002(ball_estimate_angle, ball_estimate_radius, ball_estimate_angle, Rbenchmark);
        move_test_C.zs_angle_track_001(ball_estimate_angle, 0 /*benchmark_angle*/, black_white_ball);
        //	move_test_C.zs_angle_track_002	 (ball_angle,0);

        move_test_C.turn_v_maxlimit();  // 限定平移[功率]速度输出最大值
        move_test_C.front_v_maxlimit(); // 限定平移[功率]速度输出最大值
        //??????????????????????????????????????????????????????????????
    }
}

void Dribble::Ball_Into_Holder3(float ball_angle, float ball_Rradius, float ball_FAngle, float ball_FRradius)
{

    move_test_C.limit_parameter_set_angle(10,
                                          1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
    move_test_C.limit_parameter_set_dis(
        200, 1);    //(10, 5);//(15, 5);//(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
    float ax = 2.1; // 2.4;//3.0;//0.17;//0.51*4.8*0.09;//0.51*1.5;//1.2;//5;//0.45;//0.39;//角速度修正量 缩减系数
    float dx = 0.2; // 0.056;//2.4*0.015 ;//1;//0.80;//1;//2.8;			//平移修正量  缩减系数
    float angle_p_1 = 0.140 * ax; // 0.35*ax;//;
    float angle_i_1 = 0;          // 0.005*ax;  //0.06*ax;//0;
    float angle_d_1 = 0;          //-0.008*ax;//0-0.028*ax;//0
    float dis_p_1 = 5.50 * dx;
    float dis_i_1 = 0;
    float dis_d_1 = 0; // 15.00*dx;
    move_test_C.parameter_set_angle(angle_p_1, angle_i_1, angle_d_1, 0);
    move_test_C.parameter_set_dis(dis_p_1, dis_i_1, dis_d_1, 0); // 0.3

    // float holdballmin=19.3;//持球的 距离
    // float holdballmax=26;//持球的 距离
    // float holdballangle=6;//持球 的角度
    char black_white_ball[20];
    // float motor1,motor2,motor3;
    // float mb,ml,mr;

    // 组成1
    // 在前景视野外的大范围里
    float radius_in = 23;  // 最近躲避持球器距离
    float radius_out = 23; // 30;//最远躲避持球器距离
    float angle_in = 20;   // 进入持球器角度
    float angle_out = 60;  // 最大远离持球器角度

    // 组成2 所有pid 都可以相应 上调
    // 在前景视野内小范围里	(FHaveBall==1)//(Dribble_ball.ball_op_Fweight>1000)
    float radiusF_in = 0; // holdballFmin-10;//最远躲避持球器距离
    float radiusF_out =
        39;                // 14;//radius_in	;//最近躲避持球器距离 =组成1 里的radius_in 但要比radius_in 小一点在物理上
    float angleF_in = 1.0; // 进入持球器底部角度
    float angleF_out = 9;  // 最大远离持球器角度

    move_test_C.initializing_movement(0, 0, 0); //(float turn,float front,float angle)
    if (FHaveBall == 0)                         //(ball_op_Fweight<1000)
    {
        // 组成1
        // 在前景视野外的大范围里
        // R-rin/a-ain=rout-rin/aout-ain
        // R-rin=(rout-rin)(a-ain)/(aout-ain)
        float Rbenchmark; //=基准长度
        if (fabs(ball_angle) < angle_in)
            Rbenchmark = radius_in;
        if ((fabs(ball_angle) >= angle_in) && (fabs(ball_angle) < angle_out))
            Rbenchmark = radius_in + (radius_out - radius_in) * (ball_angle - angle_in) / (angle_out - angle_in);
        if (fabs(ball_angle) >= angle_out)
            Rbenchmark = radius_out;
        Rbenchmark = 0; //
        move_test_C.zs_point_track_01_002(ball_angle, ball_Rradius, ball_angle, Rbenchmark);
        move_test_C.zs_angle_track_001(ball_angle, 0 /*benchmark_angle*/, black_white_ball);

        move_test_C.turn_v_maxlimit();  // 限定平移[功率]速度输出最大值
        move_test_C.front_v_maxlimit(); // 限定平移[功率]速度输出最大值
        //??????????????????????????????????????????????????????????????
    }
    else
    {

        // 组成2 所有pid 都可以相应 上调
        // 在前景视野内小范围里	(FHaveBall==1)//(Dribble_ball.ball_op_Fweight>1000)
        float fa = 1.1;
        float fd = 2.0;
        move_test_C.parameter_set_angle(angle_p_1 * fa, angle_i_1 * fa, angle_d_1 * fa, 0);
        move_test_C.parameter_set_dis(dis_p_1 * fd, dis_i_1 * fd, dis_d_1 * fd, 0); // 0.3
        float RFbenchmark;                                                          //=基准长度
        if (fabs(ball_FAngle) < angleF_in)
            RFbenchmark = radiusF_in;
        if ((fabs(ball_FAngle) >= angleF_in) && (fabs(ball_FAngle) < angleF_out))
            RFbenchmark = radiusF_in + (radiusF_out - radiusF_in) * (ball_op_Fa - angleF_in) / (angleF_out - angleF_in);
        if (fabs(ball_FAngle) >= angleF_out * 2)
            RFbenchmark = radiusF_out;
        RFbenchmark = 0; //
        move_test_C.zs_point_track_01_002(ball_FAngle, ball_FRradius, ball_FAngle, RFbenchmark);
        move_test_C.zs_angle_track_001(ball_FAngle, 0 /*-120benchmark_angle/*______ */, black_white_ball);
        move_test_C.turn_v_maxlimit();  //
        move_test_C.front_v_maxlimit(); // 限定平移[功率]速度输出最大值
    }

    //	float rin =40;
    //	float rout=60;
    //	float xs=(ball_FRradius-rin)/(rout-rin);
    //	if (xs<0) xs=0;
    //	if (xs>1) xs=1;
    //	if(fabs(ball_FAngle)<8)
    //			move_test_C .zs_translate_compound_O(ball_FAngle*(xs),94);//0.8将当前运动向量与  另一个平移分量相加
    //	move_test_C .turn_v_maxlimit();//
    //	move_test_C	.front_v_maxlimit();//限定平移[功率]速度输出最大值
    // ball_FRradius R1  R2   (x-r2)*1/(r1-r2)=y   y=(x-r2)*1/(r1-r2)
}

float angle_average(float a1, float a2) // 求两个角度 平均角度 有左右哪个小取哪个的作用
{
    float tempA = angle_add_transfer(a1, a2) / 2.0;
    float tempB = angle_add_transfer(tempA, 180);
    if (fabs(angle_add_transfer(tempA, -a1)) < fabs(angle_add_transfer(tempB, -a1)))
        return (tempA);
    else
        return (tempB);
}

void Dribble::Distance_Location_init(void)
{
    robot_L.f_x = 0;
    robot_L.f_y = 0;
    robot_L.f_a = 0;
    for (int i = 0; i < 20; i++) // 有延时的球在20周期内的 信息历史记录
    {
        ball_d_h[i].f_x = 0; // 估算后 球对地面的位置 在地面坐标系里
        ball_d_h[i].f_y = 0;
        ball_d_h[i].f_vx = 0;        // field 场地坐标x 方向上的速度
        ball_d_h[i].f_vy = 0;        // field 场地坐标y 方向上的速度
        obstacle_L_edgeH[i].f_x = 0; // 存障碍左边缘 历史位置  [0]是当前周期的结果
        obstacle_L_edgeH[i].f_y = 0;
        obstacle_L_edgeH[i].f_vx = 0;
        obstacle_L_edgeH[i].f_vy = 0;
        obstacle_R_edgeH[i].f_x = 0; // 存障碍右边缘 历史位置  [0]是当前周期的结果
        obstacle_R_edgeH[i].f_y = 0;
        obstacle_R_edgeH[i].f_vx = 0;
        obstacle_R_edgeH[i].f_vy = 0;
    }
}

// 路程定位 char enable是将机器定位 和 球定位 全部用全场定位决定
void Dribble::Distance_Location(float ball_angle, float ball_Rradius, float ball_weight, float f_x, float f_y,
                                float f_a, char enable)
{
    f_x = f_x * FieldZoomTime;
    f_y = f_y * FieldZoomTime;
    f_a = angle_add_transfer(f_a, 0);
    // Angle_add(robot_w,&f_a);//一个周期转动量 造成的转动角度

    float T = Gb_loop_time * 0.001; // 0.067;//周期时间单位秒
    int i;                          // mlR,mbR,mrR,ball_angle,ball_Rradius;
    // 求球对地绝对速度矢量
    if (0) //	enable!=1)// 停止时作位置清零 记录
    {
        //	Distance_Location_init();
        robot_L.f_x = (robot_L.f_x + f_x) / 2.0;
        robot_L.f_y = (robot_L.f_y + f_y) / 2.0;
        robot_L.f_a = angle_average(robot_L.f_a, f_a);
        for (i = 19; i > 0; i--) // 有延时的球在20周期内的 信息历史记录
        {
            ball_d_h[i].f_x = ball_d_h[i - 1].f_x;
            ball_d_h[i].f_y = ball_d_h[i - 1].f_y;
        }
        ball_d_h[0].f_x = l2f_x(robot_L.f_x, robot_L.f_y, robot_L.f_a, ball_angle,
                                ball_Rradius / 100.0); // 估算后 球对地面的位置 在地面坐标系里
        ball_d_h[0].f_y = l2f_y(robot_L.f_x, robot_L.f_y, robot_L.f_a, ball_angle,
                                ball_Rradius / 100.0); // 估算后 球对地面的位置 在地面坐标系里
        for (i = 19; i > 0; i--)                       // 有延时的球在20周期内的 速度信息历史记录
        {
            ball_d_h[i].f_vx = ball_d_h[i - 1].f_vx;
            ball_d_h[i].f_vy = ball_d_h[i - 1].f_vy;
        }
        ball_d_h[0].f_vx = (ball_d_h[0].f_x - ball_d_h[1].f_x) / T; // field 场地坐标x 方向上的速度
        ball_d_h[0].f_vy = (ball_d_h[0].f_y - ball_d_h[1].f_y) / T; // field 场地坐标y 方向上的速度
    }
    // 注：矫正方法 使用单独 角度对准程序矫正wp 单独平移对准程序矫正vp  矫正结果就是球在场地地图上的位置不动
    const double vp = 3.5; // 2.2 ;//静态2.2 ;//平移矫正系数 常数
    const double wp = 2.3; // 1.2 ;//静态2.0 ;//旋转矫正系数 常数
    if (1)                 // enable==1)
    {
        // 对球的对地运动矢量计算001
        // 运动反解
        float front_v = 0; // 1.99012;//临时存储 反解出的 平移速度 转动速度 平移方向
        float turn_w = 0;  // 61.9297;
        float front_a = 0; //-29.3969;
                           // 电机速度真值 估算
        // float max_a=540;
        /*因调试去掉*/ O_movement_T_analyze(&front_a, &front_v, &turn_w, -mhLR[1], -mhBR[1],
                                            -mhRR[1]); //[0]是上个周期的结果    *move_v_angle, *move_v_speed, *turn_v
        /*因调试去掉*/ front_v = front_v * vp / wheel_linear_speed_ratio;
        /*因调试去掉*/ turn_w = turn_w * wp * 360.0 / wheel_rotate_speed_ratio;
        // SetMovement(T, front_v, front_a, turn_w, &robot_L);//(float front_v_c,float front_v_angle_c,float
        // turn_w,obj_properties *obj);
        Movement(T, front_v, front_a, turn_w, &robot_L);

        float blc = 1.5;     // 比例路程	//blc占的比重大 定位偏向使用路程定位 占的小偏向使用场线匹配定位
        float bcx = 2 - blc; // 比例场线
        robot_L.f_x = (robot_L.f_x * blc + f_x * bcx) / 2.0;
        robot_L.f_y = (robot_L.f_y * blc + f_y * bcx) / 2.0;
        //	robot_L.f_a=angle_average(robot_L.f_a,f_a);//
        robot_L.f_a = f_a; //

        if (ball_weight > 30) // 对球大小的判断  如果没有球 就按上周期球对地速度 估计球当前位置
        {
            for (i = 19; i > 0; i--) // 有延时的球在20周期内的 信息历史记录
            {
                ball_d_h[i].f_x = ball_d_h[i - 1].f_x;
                ball_d_h[i].f_y = ball_d_h[i - 1].f_y;
            }
            ball_d_h[0].f_x = l2f_x(robot_L.f_x, robot_L.f_y, robot_L.f_a, ball_angle,
                                    ball_Rradius / 100.0); // 估算后 球对地面的位置 在地面坐标系里
            ball_d_h[0].f_y = l2f_y(robot_L.f_x, robot_L.f_y, robot_L.f_a, ball_angle,
                                    ball_Rradius / 100.0); // 估算后 球对地面的位置 在地面坐标系里

            for (i = 19; i > 0; i--) // 有延时的球在20周期内的 速度信息历史记录
            {
                ball_d_h[i].f_vx = ball_d_h[i - 1].f_vx;
                ball_d_h[i].f_vy = ball_d_h[i - 1].f_vy;
            }
            ball_d_h[0].f_vx = (ball_d_h[0].f_x - ball_d_h[1].f_x) / T; // field 场地坐标x 方向上的速度
            ball_d_h[0].f_vy = (ball_d_h[0].f_y - ball_d_h[1].f_y) / T; // field 场地坐标y 方向上的速度
        }
        else
        {
            float T = Gb_loop_time * 0.001; // 0.067;//周期时间单位秒
            float bfvx =
                simple_filter(ball_d_h[0].f_vx, ball_d_h[1].f_vx, ball_d_h[2].f_vx, ball_d_h[3].f_vx, ball_d_h[4].f_vx);
            float bfvy =
                simple_filter(ball_d_h[0].f_vy, ball_d_h[1].f_vy, ball_d_h[2].f_vy, ball_d_h[3].f_vy, ball_d_h[4].f_vy);
            for (i = 19; i > 0; i--) // 有延时的球在20周期内的 信息历史记录
            {
                ball_d_h[i].f_x = ball_d_h[i - 1].f_x;
                ball_d_h[i].f_y = ball_d_h[i - 1].f_y;
            }
            ball_d_h[0].f_x += bfvx * T; // 估算后 球对地面的位置 在地面坐标系里
            ball_d_h[0].f_y += bfvy * T; // 估算后 球对地面的位置 在地面坐标系里

            for (i = 19; i > 0; i--) // 有延时的球在20周期内的 速度信息历史记录
            {
                ball_d_h[i].f_vx = ball_d_h[i - 1].f_vx;
                ball_d_h[i].f_vy = ball_d_h[i - 1].f_vy;
            }
            ball_d_h[0].f_vx = 0; //(ball_d_h[0].f_x-ball_d_h[1].f_x)/T;//field 场地坐标x 方向上的速度
            ball_d_h[0].f_vy = 0; //(ball_d_h[0].f_y-ball_d_h[1].f_y)/T;//field 场地坐标y 方向上的速度
        }
        // m_find.pField->SetAgent(11 ,robot_L.f_x*100     ,robot_L.f_y*100    ,-robot_L.f_a);//场子正中向上
        // 逆时针坐标系 m_find.pField->SetBall (11 ,ball_d_h[0].f_x*100 ,ball_d_h[0].f_y*100);
    }
    ///*显示球*/Debug_D[11].Draw_line_	   ( /*因调试去掉*/coordinate_transfer_x
    ///(angle_add_transfer(ball_angle,-90),ball_Rradius/5.0) +320
    //										,/*因调试去掉*/coordinate_transfer_y
    //(angle_add_transfer(ball_angle,-90),ball_Rradius/5.0) +240
    //,angle_add_transfer(angle_add_transfer(coordinate_transfer_angle
    //(ball_d_h[0].f_vx,ball_d_h[0].f_vy),-robot_L.f_a),-90)
    //										,coordinate_transfer_radius(ball_d_h[0].f_vx,ball_d_h[0].f_vy)*40
    //										,0,255,0,  pBuffer);

    // float axdl=	angle_add_transfer(angle_add_transfer(coordinate_transfer_angle
    // (ball_d_h[0].f_vx,ball_d_h[0].f_vy),-robot_L.f_a),0) ; 运动估算前的输出
    ball_org_angle = f2l_angle(robot_L.f_x, robot_L.f_y, robot_L.f_a, ball_d_h[0].f_x,
                               ball_d_h[0].f_y); // ball_angle;//全景球的angle;
    ball_org_radius = f2l_radius(robot_L.f_x, robot_L.f_y, robot_L.f_a, ball_d_h[0].f_x,
                                 ball_d_h[0].f_y); // ball_Rradius/100.0;//全景球的radius;
    // 运动估算
    // 全景运动估算后的输出
    ball_estimate_angle = ball_org_angle;
    ball_estimate_radius = ball_org_radius;
    Movement_estimate_delay_compute2ball(
        &ball_estimate_angle, &ball_estimate_radius); //  angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
    ball_estimate_radius = ball_estimate_radius * 100.0; // 全景球的radius
    // 前景运动估算后的输出
    ball_estimate_F_angle = ball_op_Fa;          // 前景球的angle
    ball_estimate_F_radius = ball_op_Fr / 100.0; // 前景球的radius
    Movement_estimate_delay_compute2ball(
        &ball_estimate_F_angle,
        &ball_estimate_F_radius); // angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
    ball_estimate_F_radius = ball_estimate_F_radius * 100.0; // 前景球的radius
    ball_org_radius = ball_org_radius * 100;
    // 对方门左右边缘的原只值
    ddoor_left_o =
        f2l_angle(robot_L.f_x, robot_L.f_y, robot_L.f_a, TargetGoalLX * FieldZoomTime, TargetGoalLY * FieldZoomTime);
    ddoor_right_o =
        f2l_angle(robot_L.f_x, robot_L.f_y, robot_L.f_a, TargetGoalRX * FieldZoomTime, TargetGoalRY * FieldZoomTime);
    // 对方门左右边缘的估算值
    ddoor_left_e = revise_gool_L_edge_A();
    ddoor_right_e = revise_gool_R_edge_A();
}

void Dribble::obstacle_edge_save(float Langle, float LRradius, float Rangle,
                                 float RRradius) // angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
{
    float T = Gb_loop_time * 0.001; // 0.067;//周期时间单位秒
    int i;
    for (i = 19; i > 0; i--) // 有延时的球在20周期内的 信息历史记录
    {
        obstacle_L_edgeH[i].f_x = obstacle_L_edgeH[i - 1].f_x;
        obstacle_L_edgeH[i].f_y = obstacle_L_edgeH[i - 1].f_y;
        obstacle_R_edgeH[i].f_x = obstacle_R_edgeH[i - 1].f_x;
        obstacle_R_edgeH[i].f_y = obstacle_R_edgeH[i - 1].f_y;
    }
    obstacle_L_edgeH[0].f_x =
        l2f_x(robot_L.f_x, robot_L.f_y, robot_L.f_a, Langle, LRradius / 100.0); // 估算后 球对地面的位置 在地面坐标系里
    obstacle_L_edgeH[0].f_y =
        l2f_y(robot_L.f_x, robot_L.f_y, robot_L.f_a, Langle, LRradius / 100.0); // 估算后 球对地面的位置 在地面坐标系里
    obstacle_R_edgeH[0].f_x =
        l2f_x(robot_L.f_x, robot_L.f_y, robot_L.f_a, Rangle, RRradius / 100.0); // 估算后 球对地面的位置 在地面坐标系里
    obstacle_R_edgeH[0].f_y =
        l2f_y(robot_L.f_x, robot_L.f_y, robot_L.f_a, Rangle, RRradius / 100.0); // 估算后 球对地面的位置 在地面坐标系里

    for (i = 19; i > 0; i--) // 有延时的球在20周期内的 速度信息历史记录
    {
        obstacle_L_edgeH[i].f_vx = obstacle_L_edgeH[i - 1].f_vx;
        obstacle_L_edgeH[i].f_vy = obstacle_L_edgeH[i - 1].f_vy;
        obstacle_R_edgeH[i].f_vx = obstacle_R_edgeH[i - 1].f_vx;
        obstacle_R_edgeH[i].f_vy = obstacle_R_edgeH[i - 1].f_vy;
    }
    obstacle_L_edgeH[0].f_vx = (obstacle_L_edgeH[0].f_x - obstacle_L_edgeH[1].f_x) / T; // field 场地坐标x 方向上的速度
    obstacle_L_edgeH[0].f_vy = (obstacle_L_edgeH[0].f_y - obstacle_L_edgeH[1].f_y) / T; // field 场地坐标y 方向上的速度
    obstacle_R_edgeH[0].f_vx = (obstacle_R_edgeH[0].f_x - obstacle_R_edgeH[1].f_x) / T; // field 场地坐标x 方向上的速度
    obstacle_R_edgeH[0].f_vy = (obstacle_R_edgeH[0].f_y - obstacle_R_edgeH[1].f_y) / T; // field 场地坐标y 方向上的速度
}

float Dribble::revise_obstacle_R_edge(float guibiquyu)
{
    float T = Gb_loop_time * 0.001; // 0.067;//周期时间单位秒
    float bfvx = simple_filter(obstacle_R_edgeH[0].f_vx, obstacle_R_edgeH[1].f_vx, obstacle_R_edgeH[2].f_vx,
                               obstacle_R_edgeH[3].f_vx, obstacle_R_edgeH[4].f_vx);
    float bfvy = simple_filter(obstacle_R_edgeH[0].f_vy, obstacle_R_edgeH[1].f_vy, obstacle_R_edgeH[2].f_vy,
                               obstacle_R_edgeH[3].f_vy, obstacle_R_edgeH[4].f_vy);
    float radius = f2l_radius(robot_L.f_x, robot_L.f_y, robot_L.f_a, obstacle_R_edgeH[0].f_x, obstacle_R_edgeH[0].f_y);
    float angle = f2l_angle(robot_L.f_x, robot_L.f_y, robot_L.f_a, obstacle_R_edgeH[0].f_x, obstacle_R_edgeH[0].f_y);
    view_R_edge_x = l2f_x(robot_L.f_x, robot_L.f_y, robot_L.f_a, angle,
                          radius); // 用于在全场定位上看边沿信息//估算后 球对地面的位置 在地面坐标系里
    view_R_edge_y = l2f_y(robot_L.f_x, robot_L.f_y, robot_L.f_a, angle,
                          radius); // 用于在全场定位上看边沿信息//估算后 球对地面的位置 在地面坐标系里
    Movement_estimate_delay_compute2(&angle, &radius, bfvx, bfvy,
                                     T * 2); // angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
    return (angle_add_transfer(angle, -double(guibiquyu / double(radius)) * 12 * 2.5));
}
float Dribble::revise_obstacle_L_edge(float guibiquyu)
{
    float T = Gb_loop_time * 0.001; // 0.067;//周期时间单位秒
    float bfvx = simple_filter(obstacle_L_edgeH[0].f_vx, obstacle_L_edgeH[1].f_vx, obstacle_L_edgeH[2].f_vx,
                               obstacle_L_edgeH[3].f_vx, obstacle_L_edgeH[4].f_vx);
    float bfvy = simple_filter(obstacle_L_edgeH[0].f_vy, obstacle_L_edgeH[1].f_vy, obstacle_L_edgeH[2].f_vy,
                               obstacle_L_edgeH[3].f_vy, obstacle_L_edgeH[4].f_vy);
    float radius = f2l_radius(robot_L.f_x, robot_L.f_y, robot_L.f_a, obstacle_L_edgeH[0].f_x + bfvx * 4 * T,
                              obstacle_L_edgeH[0].f_y + bfvy * 4 * T);
    float angle = f2l_angle(robot_L.f_x, robot_L.f_y, robot_L.f_a, obstacle_L_edgeH[0].f_x + bfvx * 4 * T,
                            obstacle_L_edgeH[0].f_y + bfvy * 4 * T);
    view_L_edge_x = l2f_x(robot_L.f_x, robot_L.f_y, robot_L.f_a, angle,
                          radius); // 用于在全场定位上看边沿信息//估算后 球对地面的位置 在地面坐标系里
    view_L_edge_y = l2f_y(robot_L.f_x, robot_L.f_y, robot_L.f_a, angle,
                          radius); // 用于在全场定位上看边沿信息//估算后 球对地面的位置 在地面坐标系里
    Movement_estimate_delay_compute2(&angle, &radius, bfvx, bfvy,
                                     T * 2); // angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
    return (angle_add_transfer(angle, +double(guibiquyu / double(radius)) * 12 * 2.5));
}

float Dribble::revise_gool_edge_A(float angle, float radius)
{
    float T = Gb_loop_time * 0.001; // 0.067;//周期时间单位秒
    Movement_estimate_delay_compute2(&angle, &radius, 0, 0,
                                     T); // angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
    return (angle);
}

float Dribble::revise_gool_L_edge_A(void)
{
    float T = Gb_loop_time * 0.001; // 0.067;//周期时间单位秒
    float x = TargetGoalLX * FieldZoomTime;
    float y = TargetGoalLY * FieldZoomTime;
    float radius = f2l_radius(robot_L.f_x, robot_L.f_y, robot_L.f_a, x, y);
    float angle = f2l_angle(robot_L.f_x, robot_L.f_y, robot_L.f_a, x, y);
    Movement_estimate_delay_compute2(&angle, &radius, 0, 0,
                                     T); // angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
    return (angle);
}

float Dribble::revise_gool_R_edge_A(void)
{
    float T = Gb_loop_time * 0.001; // 0.067;//周期时间单位秒
    float x = TargetGoalRX * FieldZoomTime;
    float y = TargetGoalRY * FieldZoomTime;
    float radius = f2l_radius(robot_L.f_x, robot_L.f_y, robot_L.f_a, x, y);
    float angle = f2l_angle(robot_L.f_x, robot_L.f_y, robot_L.f_a, x, y);
    Movement_estimate_delay_compute2(&angle, &radius, 0, 0,
                                     T); // angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
    return (angle);
}
float Dribble::revise_goal_R_time_test(void) // 计算机器在当前运动状态下 多长时间可以射击右门柱
{
    int i;
    float looptime = 3.6;           // 使用几个周期的时间 进行时序运动预测
    float T = Gb_loop_time * 0.001; // 0.067;//周期时间单位秒
    float radiusORG =
        f2l_radius(robot_L.f_x, robot_L.f_y, robot_L.f_a, TargetGoalRX * FieldZoomTime, TargetGoalRY * FieldZoomTime);
    float angleORG =
        f2l_angle(robot_L.f_x, robot_L.f_y, robot_L.f_a, TargetGoalRX * FieldZoomTime, TargetGoalRY * FieldZoomTime);
    float radius = radiusORG;
    float angle = angleORG;
    float time = T * looptime * 0.5;
    if ((-90 < angle) && (angle < 0)) // 初始如果右门拄角度小于零 在机器左边
    {
        Movement_estimate_delay_compute2(
            &angle, &radius, 0, 0, T * looptime); // angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
        if (angle > 0) // 如果经过2周期预测由于机器运行  右门拄将在机器右边 就开始用二分法计算 时序时间
        {
            for (i = 0; i < 10; i++)
            {
                radius = radiusORG;
                angle = angleORG;
                Movement_estimate_delay_compute2(
                    &angle, &radius, 0, 0, time); // angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
                if (angle > 0)
                    time -= T * looptime * pow(0.5, i + 2); // double pow(double x, double y);
                else
                    time += T * looptime * pow(0.5, i + 2);
            }
        }
        else
        {
            return (0);
        }
    }
    else
    {
        return (0);
    }
    return (time);
}
float Dribble::revise_goal_L_time_test(void) // 计算机器在当前运动状态下 多长时间可以射击左门柱
{
    int i;
    float looptime = 3.6;           // 使用几个周期的时间 进行时序运动预测
    float T = Gb_loop_time * 0.001; // 0.067;//周期时间单位秒
    float radiusORG =
        f2l_radius(robot_L.f_x, robot_L.f_y, robot_L.f_a, TargetGoalLX * FieldZoomTime, TargetGoalLY * FieldZoomTime);
    float angleORG =
        f2l_angle(robot_L.f_x, robot_L.f_y, robot_L.f_a, TargetGoalLX * FieldZoomTime, TargetGoalLY * FieldZoomTime);
    float radius = radiusORG;
    float angle = angleORG;
    float time = T * looptime * 0.5;
    if ((90 > angle) && (angle > 0)) // 初始如果左门拄角度小于零 在机器左边
    {
        Movement_estimate_delay_compute2(
            &angle, &radius, 0, 0, T * looptime); // angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
        if (angle < 0) // 如果经过2周期预测由于机器运行  左门拄将在机器右边 就开始用二分法计算 时序时间
        {
            for (i = 0; i < 10; i++)
            {
                radius = radiusORG;
                angle = angleORG;
                Movement_estimate_delay_compute2(
                    &angle, &radius, 0, 0, time); // angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
                if (angle < 0)
                    time -= T * looptime * pow(0.5, i + 2); // double pow(double x, double y);
                else
                    time += T * looptime * pow(0.5, i + 2);
            }
        }
        else
        {
            return (0);
        }
    }
    else
    {
        return (0);
    }
    return (time);
}
void Dribble::Movement_estimate_delay_compute(
    float *angle, float *radius) // angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
{
    float T = Gb_loop_time * 0.001; // 0.067;//周期时间单位秒
    obj_properties robot_L_estimate, ball_estimate;
    robot_L_estimate.f_x = robot_L.f_x; // robot_L;有延时的 机体 直角坐标系位置  通过全场定位得到 其实也可以用任意值
    robot_L_estimate.f_y = robot_L.f_y;
    robot_L_estimate.f_a = robot_L.f_a;
    ball_estimate.f_x =
        l2f_x(robot_L.f_x, robot_L.f_y, robot_L.f_a, *angle, *radius); // 估算后 球对地面的位置 在地面坐标系里
    ball_estimate.f_y =
        l2f_y(robot_L.f_x, robot_L.f_y, robot_L.f_a, *angle, *radius); // 估算后 球对地面的位置 在地面坐标系里

    // 注：矫正方法 使用单独 角度对准程序矫正wp 单独平移对准程序矫正vp  矫正结果就是球在场地地图上的位置不动
    const double vp = 3.5; // 2.2 ;//静态2.2 ;//平移矫正系数 常数
    const double wp = 2.3; // 1.2 ;//静态2.0 ;//旋转矫正系数 常数

    // 对球的对地运动矢量计算001
    // 运动反解
    float front_v = 0; // 临时存储 反解出的 平移速度 转动速度 平移方向
    float turn_w = 0;  //
    float front_a = 0; //
    /*因调试去掉*/ O_movement_T_analyze(&front_a, &front_v, &turn_w, -mhLR[0], -mhBR[0],
                                        -mhRR[0]); //[0]是上个周期的结果    *move_v_angle, *move_v_speed, *turn_v
    /*因调试去掉*/ front_v = front_v * vp / wheel_linear_speed_ratio;
    /*因调试去掉*/ turn_w = turn_w * wp * 360.0 / wheel_rotate_speed_ratio;
    Movement(T, front_v, front_a, turn_w, &robot_L_estimate);

    *radius = f2l_radius(robot_L_estimate.f_x, robot_L_estimate.f_y, robot_L_estimate.f_a, ball_estimate.f_x,
                         ball_estimate.f_y);
    *angle = f2l_angle(robot_L_estimate.f_x, robot_L_estimate.f_y, robot_L_estimate.f_a, ball_estimate.f_x,
                       ball_estimate.f_y);
}

void Dribble::Movement_estimate_delay_compute2ball(
    float *angle, float *radius) // angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
{
    float T = Gb_loop_time * 0.001; // 0.067;//周期时间单位秒
    obj_properties robot_L_estimate, ball_estimate;
    robot_L_estimate.f_x = robot_L.f_x; // robot_L;有延时的 机体 直角坐标系位置  通过全场定位得到 其实也可以用任意值
    robot_L_estimate.f_y = robot_L.f_y;
    robot_L_estimate.f_a = robot_L.f_a;
    //	ball_estimate.f_x   =ball_d_h [0].f_x +ball_d_h[0].f_vx*T;//估算后 球对地面的位置 在地面坐标系里
    //	ball_estimate.f_y   =ball_d_h [0].f_y +ball_d_h[0].f_vy*T;//估算后 球对地面的位置 在地面坐标系里
    float bfvx =
        simple_filter(ball_d_h[0].f_vx, ball_d_h[1].f_vx, ball_d_h[2].f_vx, ball_d_h[3].f_vx, ball_d_h[4].f_vx);
    float bfvy =
        simple_filter(ball_d_h[0].f_vy, ball_d_h[1].f_vy, ball_d_h[2].f_vy, ball_d_h[3].f_vy, ball_d_h[4].f_vy);
    ball_estimate.f_x = l2f_x(robot_L.f_x, robot_L.f_y, robot_L.f_a, *angle, *radius) +
                        bfvx * T; // 估算后 球对地面的位置 在地面坐标系里
    ball_estimate.f_y = l2f_y(robot_L.f_x, robot_L.f_y, robot_L.f_a, *angle, *radius) +
                        bfvy * T; // 估算后 球对地面的位置 在地面坐标系里

    // 注：矫正方法 使用单独 角度对准程序矫正wp 单独平移对准程序矫正vp  矫正结果就是球在场地地图上的位置不动
    const double vp = 3.5; // 2.2 ;//静态2.2 ;//平移矫正系数 常数
    const double wp = 2.3; // 1.2 ;//静态2.0 ;//旋转矫正系数 常数

    // 对球的对地运动矢量计算001
    // 运动反解
    float front_v = 0; // 临时存储 反解出的 平移速度 转动速度 平移方向
    float turn_w = 0;  //
    float front_a = 0; //
    /*因调试去掉*/ O_movement_T_analyze(&front_a, &front_v, &turn_w, -mhLR[0], -mhBR[0],
                                        -mhRR[0]); //[0]是上个周期的结果    *move_v_angle, *move_v_speed, *turn_v
    /*因调试去掉*/ front_v = front_v * vp / wheel_linear_speed_ratio;
    /*因调试去掉*/ turn_w = turn_w * wp * 360.0 / wheel_rotate_speed_ratio;
    Movement(T, front_v, front_a, turn_w, &robot_L_estimate);

    *radius = f2l_radius(robot_L_estimate.f_x, robot_L_estimate.f_y, robot_L_estimate.f_a, ball_estimate.f_x,
                         ball_estimate.f_y);
    *angle = f2l_angle(robot_L_estimate.f_x, robot_L_estimate.f_y, robot_L_estimate.f_a, ball_estimate.f_x,
                       ball_estimate.f_y);
}

void Dribble::Movement_estimate_delay_compute2(
    float *angle, float *radius, float fvx, float fvy,
    float T) // angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
{
    //	float    T =Gb_loop_time*0.001;//0.067;//周期时间单位秒
    obj_properties robot_L_estimate, ball_estimate;
    robot_L_estimate.f_x = robot_L.f_x; // robot_L;有延时的 机体 直角坐标系位置  通过全场定位得到 其实也可以用任意值
    robot_L_estimate.f_y = robot_L.f_y;
    robot_L_estimate.f_a = robot_L.f_a;
    ball_estimate.f_x =
        l2f_x(robot_L.f_x, robot_L.f_y, robot_L.f_a, *angle, *radius) + fvx * T; // 估算后 球对地面的位置 在地面坐标系里
    ball_estimate.f_y =
        l2f_y(robot_L.f_x, robot_L.f_y, robot_L.f_a, *angle, *radius) + fvy * T; // 估算后 球对地面的位置 在地面坐标系里
    // 注：矫正方法 使用单独 角度对准程序矫正wp 单独平移对准程序矫正vp  矫正结果就是球在场地地图上的位置不动
    const double vp = 3.5; // 2.2 ;//静态2.2 ;//平移矫正系数 常数
    const double wp = 2.3; // 1.2 ;//静态2.0 ;//旋转矫正系数 常数
    // 对球的对地运动矢量计算001
    // 运动反解
    float front_v = 0; // 临时存储 反解出的 平移速度 转动速度 平移方向
    float turn_w = 0;  //
    float front_a = 0; //
    /*因调试去掉*/ O_movement_T_analyze(&front_a, &front_v, &turn_w, -mhLR[0], -mhBR[0],
                                        -mhRR[0]); //[0]是上个周期的结果    *move_v_angle, *move_v_speed, *turn_v
    /*因调试去掉*/ front_v = front_v * vp / wheel_linear_speed_ratio;
    /*因调试去掉*/ turn_w = turn_w * wp * 360.0 / wheel_rotate_speed_ratio;
    Movement(T, front_v, front_a, turn_w, &robot_L_estimate);

    *radius = f2l_radius(robot_L_estimate.f_x, robot_L_estimate.f_y, robot_L_estimate.f_a, ball_estimate.f_x,
                         ball_estimate.f_y);
    *angle = f2l_angle(robot_L_estimate.f_x, robot_L_estimate.f_y, robot_L_estimate.f_a, ball_estimate.f_x,
                       ball_estimate.f_y);
}

// T一个周期时间  v机体 平移速度 v_a机体速度角度 w机体转动速度 local机体物理状态存储结构体
void Dribble::Movement(float T, float v, float v_a, float w, obj_properties *local)
{
    // 开始 通过电机输出值运动位置累加  //以后每一帧做累加
    if (fabs(w) > 0.1)
    {
        float Rc = (v * 180) / (w * 3.14); // v/(w*3.14/????128????);			//机体旋转中心的极坐标	半径w*pi*r/180=v
                                           // Rc w*(2*pi*r/360) =v  w(度/s)  v,r(米) 或(像素)
        double Rx, Ry;
        Rx = local->f_x + coordinate_transfer_x(local->f_a + 90 + v_a, Rc); // 算出旋转中心 地面坐标
        Ry = local->f_y + coordinate_transfer_y(local->f_a + 90 + v_a, Rc);
        local->f_x = Rx - coordinate_transfer_x(local->f_a + 90 + v_a + T * w, Rc); // 本机 新 场地x
        local->f_y = Ry - coordinate_transfer_y(local->f_a + 90 + v_a + T * w, Rc); // 本机 新 场地y
        local->f_a = angle_add_transfer(T * w, local->f_a); // 本机场地angle = 时间*角速度 + 上周期本机场地angle
    }
    else
    {
        local->f_x += coordinate_transfer_x(local->f_a + v_a, v) * T; // 本机 新 场地x
        local->f_y += coordinate_transfer_y(local->f_a + v_a, v) * T; // 本机 新 场地y
        local->f_a = angle_add_transfer(T * w, local->f_a); // 本机场地angle = 时间*角速度 + 上周期本机场地angle
    }
    // 位置估测完毕
}
bool Dribble::O_movement_T_analyze(float *move_v_angle, float *move_v_speed, float *turn_v, float motor1, float motor2,
                                   float motor3) // 全向解析
{
    bool temp = O_movement_T_analyze_(move_v_angle, move_v_speed, turn_v, motor1, motor2, motor3); // 全向解析
    if (*move_v_speed < 0)
    {
        *move_v_angle = angle_add_transfer(*move_v_angle, 180);
        *move_v_speed = -*move_v_speed;
    }
    return (temp);
}

bool Dribble::O_movement_T_analyze_(float *move_v_angle, float *move_v_speed, float *turn_v, float motor1, float motor2,
                                    float motor3) // 全向解析
{
    return (move2Control.O_movement_T_analyze(move_v_angle, move_v_speed, turn_v, motor1, motor2, motor3));
}

void Dribble::motor_a_limit_init(void)
{
    mlR = 0;
    mbR = 0;
    mrR = 0; // 存储上个周期电机的真实转速 的变量清零
}
// 使用加速度限制 预测 机体电机轴速
void Dribble::motor_a_limit(float mtb, float mtl, float mtr)
{
    // 电机速度真值 估算
    float max_a = 800;

    if ((mtb - mbR) > max_a)
        mbR += max_a;
    else if ((mtb - mbR) < -max_a)
        mbR -= max_a;
    else
        mbR = mtb;

    if ((mtl - mlR) > max_a)
        mlR += max_a;
    else if ((mtl - mlR) < -max_a)
        mlR -= max_a;
    else
        mlR = mtl;

    if ((mtr - mrR) > max_a)
        mrR += max_a;
    else if ((mtr - mrR) < -max_a)
        mrR -= max_a;
    else
        mrR = mtr;
}
// 包括 motor_a_limit的功能 且作历史记录使用加速度限制 预测 机体电机轴速  在周期末电机输出的位置使用
void Dribble::motor_history(float mtb, float mtl,
                            float mtr) // 使用加速度限制 预测 机体电机轴速  在周期末电机输出的位置使用
{
    int i;
    for (i = 19; i > 0; i--) // 存储上20周期电机的发送转速
    {
        mhL[i] = mhL[i - 1];
        mhB[i] = mhB[i - 1];
        mhR[i] = mhR[i - 1];
    }
    mhL[0] = mtl;
    mhB[0] = mtb;
    mhR[0] = mtr;
    motor_a_limit(mtb, mtl, mtr);
    for (i = 19; i > 0; i--) // 存储上20周期电机的真实转速
    {
        mhLR[i] = mhLR[i - 1];
        mhBR[i] = mhBR[i - 1];
        mhRR[i] = mhRR[i - 1];
    }
    mhLR[0] = mlR;
    mhBR[0] = mbR;
    mhRR[0] = mrR;
}

void Dribble::Save_Moment_State(float motor1, float motor2, float motor3) // 通过电机参数 保存本周期最后输出的运动矢量
{ //	ni_uptech_output_O(&va,&v,&w,*motor1,*motor2,*motor3);

    ni_uptech_output_O(&robot_fva, &robot_fv, &robot_w, motor1, motor2, motor3); // 机器的上周期的运动参数 （累加）
    if (Dribble_ball_other_moment_flag == 1) // 先运行了其他程序再运行带球函数标志  程序运行中标志
    {                                        //	fount_xVset =coordinate_transfer_x (robot_fva+robot_L.f_a,robot_fv);
                                             //	fount_yVset =coordinate_transfer_y (robot_fva+robot_L.f_a,robot_fv);
    }
    else
    {
        robot_fv = fv; // Front_C;//在带球的状态下 平移速度是 球的 不是机体的
        //	fount_xVset =coordinate_transfer_x (robot_fva+robot_L.f_a,robot_fv);
        //	fount_yVset =coordinate_transfer_y (robot_fva+robot_L.f_a,robot_fv);
        fount_xVset = l2f_x(0, 0, robot_L.f_a, RMfa, RMfv) * 22.648 * 1.4; // 1.2;
        fount_yVset = l2f_y(0, 0, robot_L.f_a, RMfa, RMfv) * 22.648 * 1.4; // 1.2;
    }
    dis_x.New_Cycle(); // Ball_Track_x_diff() 差分速度  一周期完成 差分过标志 置1       在 一个周期完结 或周期刚开始
                       // 运行
    dis_y.New_Cycle(); // Ball_Track_x_diff() 差分速度  一周期完成 差分过标志 置1       在 一个周期完结 或周期刚开始
                       // 运行
    dis_a.New_Cycle(); // Ball_Track_x_diff() 差分角速度  一周期完成 差分过标志 置1       在 一个周期完结 或周期刚开始
                       // 运行
    v_x.New_Cycle();   // Ball_Track3()//差分加速度  一周期完成 差分过标志 置1       在 一个周期完结 或周期刚开始 运行
    v_y.New_Cycle();   // Ball_Track3()//差分加速度  一周期完成 差分过标志 置1       在 一个周期完结 或周期刚开始 运行

    if (FHaveBall == 0) //(ball_op_Fweight<1000)
    {
        move_test_C.save_target_history(ball_estimate_angle, ball_estimate_radius,
                                        100); // Ball_Into_Holder2()更新历史记录函数  要求调用程序每周期更新
    }
    else
    {
        move_test_C.save_target_history(ball_estimate_F_angle, ball_estimate_F_radius,
                                        ball_op_Fweight); // Ball_Into_Holder2()更新历史记录函数  要求调用程序每周期更新
    }
    Dribble_ball_other_moment_flag = 0; // 先运行了其他程序再运行带球函数标志  程序运行中标志
}

void Dribble::move_and_target(float forward_direction_a, float forward_V, float target_a, float *motorB, float *motorL,
                              float *motorR) // 用于向指定方向平移(本机极坐标)并 对准指定目标(本机极坐标)
{
    CMovement2Control move_Control;
    char black_white_ball[20];
    move_Control.limit_parameter_set_angle(5,
                                           1);   //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
    move_Control.limit_parameter_set_dis(50, 8); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
    float ax = 0.1;                              // 1;
    float angle_p_1 = 0.090 * ax;                // 0.35*ax;//;
    float angle_i_1 = 0;                         // 0.005*ax;  //0.06*ax;//0;
    float angle_d_1 = 0;                         //-0.008*ax;//0-0.028*ax;//0
    move_Control.parameter_set_angle(angle_p_1, angle_i_1, angle_d_1, 0);
    move_Control.parameter_set_dis(0, 0, 0, 0); // 0.3

    move_Control.initializing_movement(0, 0, 0); //(float turn,float front,float angle)
    move_Control.zs_angle_track_001(target_a, 0 /*-120benchmark_angle/*______ */, black_white_ball);

    Angle_add(move_Control.turn_v_c, &forward_direction_a); // 旋转 修正 前进方向函数

    //	track_angle(&robot_fva,forward_direction_a,1.20); //定速度 角度跟踪函数 速度12.0

    move_Control.zs_translate_compound_O(forward_direction_a, forward_V); // robot_fv);//100robot_fva,robot_fv);
    move_Control.front_v_angle_p = robot_fva;                             // 将上周期的速度分量辅给
    move_Control.front_v_p = robot_fv;
    move_Control.front_a_maxlimit();
    ///*测试旋转量在每周期产生角度的代码*/	Angle_add(zqw,&faTest);//一 个周期转动量 造成的转动角度
    ///*测试旋转量在每周期产生角度的代码*/	move_test_D  .zs_translate_compound_O(faTest,10);///3.0   19float
    /// angle,float speed
    //	/*测试代码*/	move_test_D  .initializing_movement(0,0.1,0);//(float turn,float front,float angle)
    //	/*测试代码*/		/*测试代码*/	move_test_D  .turn_v_maxlimit();//
    //	/*测试代码*/	move_test_D  .zs_point_track_01_002(ball_angle,ball_Rradius, ball_angle, 200);//ball_angle/800
    //	/*测试代码*/	move_test_D  .front_v_maxlimit();//
    move_Control.uptech_output_O(motorB, motorL, motorR); // ？？？？？？？？？？？？？？
}
void Dribble::Grab_And_Breakage_Ball(float direct, float *motorB, float *motorL,
                                     float *motorR) // 用于向指定方向平移(本机极坐标)并 对准指定目标(本机极坐标)
{
    //	ball_org_angle =f2l_angle (robot_L.f_x,robot_L.f_y,robot_L.f_a,ball_d_h [0].f_x,ball_d_h
    //[0].f_y);//ball_angle;//全景球的angle; 	ball_org_radius=f2l_radius(robot_L.f_x,robot_L.f_y,robot_L.f_a,ball_d_h
    //[0].f_x,ball_d_h [0].f_y);//ball_Rradius/100.0;//全景球的radius;
    //  float	ball_op_Fa;//前景球的angle
    //  float	ball_op_Fr;//前景球的radius
    char black_white_ball[20];
    move_test_C.initializing_movement(0, 0, 0); //(float turn,float front,float angle)
    if (FHaveBall == 1)                         //(0)//(1)//(ball_op_Fweight>1000)
    {
        move_test_C.limit_parameter_set_angle(
            6, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
        move_test_C.limit_parameter_set_dis(
            40, 15); //(10, 5);//(15, 5);//(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
        float ax =
            0.8; // 0.6..0.2//0.75;//0.85;//1.75;//2.4;//3.0;//0.17;//0.51*4.8*0.09;//0.51*1.5;//1.2;//5;//0.45;//0.39;//角速度修正量
                 // 缩减系数
        float dx = 0.07; // 0.056;//2.4*0.015 ;//1;//0.80;//1;//2.8;			//平移修正量  缩减系数
        // 组成2 所有pid 都可以相应 上调
        // 在前景视野内小范围里	(FHaveBall==1)//(Dribble_ball.ball_op_Fweight>1000)
        float fa = 1;
        if (ball_op_Fr < 45) //(0)//((fabs(ball_FAngle)>15)&&(ball_FRradius<45))
        {
            fa = fa * 2.0;
            move_test_C.limit_parameter_set_angle(
                10, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
        }
        float angle_p_1 = 0.19 * ax * fa; // 0.180*ax;
        float angle_i_1 = 0;              // 0.0002*ax;//0.005*ax;  //0.06*ax;//0;
        float angle_d_1 = -0.0070 * ax;   //-0.0070*ax;//
        float dis_p_1 = 5.50 * dx;
        move_test_C.parameter_set_angle(angle_p_1, angle_i_1, angle_d_1, 0);
        move_test_C.parameter_set_dis(dis_p_1, 0, 0, 0); // 0.3

        move_test_C.zs_point_track_01_002(ball_op_Fa, ball_op_Fr, 0, holdballFmin * 0.6); // ball_op_Fa
        move_test_C.zs_angle_track_001(ball_op_Fa, 0 /*-120benchmark_angle/*______ */, black_white_ball);
        //	move_test_C .zs_angle_track_002	  (ball_FAngle,0);

        if (jifen > 10)
            jifen = 10; //
        if (jifen < 0)
            jifen = 0; // 加速区 进了这个区就进行加速
        if ((FHaveBall == 1) && (fabs(ball_op_Fa) < 20) && (ball_op_Fr < 50) && (ball_op_Fr > 10)) //
        //(0)//	 ||(FHaveBall==1)&&(fabs(ball_op_Fa)<26)&&(ball_op_Fr<50)&&(ball_op_Fr>42))//(ball_op_Fweight>1000)
        { // 进入加速角 进行加速
            jifen += 0.01;
            move_test_C.zs_translate_compound_O(move_test_C.front_v_angle_c,
                                                48 + jifen); // 25,54,(1+0.9*xs)  0.8将当前运动向量与 另一个平移分量相加
        }
        else
        {
            jifen -= 0.01 * 2.0; // 0;//
        }
        if ((ball_op_Fr < holdballFmin) && (fabs(ball_op_Fa) < 5)) //(0)//进入持球器 后左右震荡时序动作
        {
            jifen = 2; //
            if (jifen >= 40)
                jifen = 0;
            if (direct > 0) //(jifen>20)
            {
                move_test_C.initializing_movement(10, 0, 0); //(float turn,float front,float angle)
                move_test_C.zs_translate_compound_O(60, 24); // 25,54,(1+0.9*xs)  0.8将当前运动向量与 另一个平移分量相加
            }
            else // if (jifen<20)
            {
                move_test_C.initializing_movement(-10, 0, 0); //(float turn,float front,float angle)
                move_test_C.zs_translate_compound_O(-60,
                                                    24); // 25,54,(1+0.9*xs)  0.8将当前运动向量与  另一个平移分量相加
            }
        }

        //	move_test_C .zs_translate_compound_O(move_test_C.front_v_angle_c,12);//4825,54,(1+0.9*xs)
        // 0.8将当前运动向量与  另一个平移分量相加
        move_test_C.turn_v_maxlimit();  //
        move_test_C.front_v_maxlimit(); // 限定平移[功率]速度输出最大值
    }
    else
    {
        move_test_C.limit_parameter_set_angle(
            6, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
        move_test_C.limit_parameter_set_dis(
            40, 15); //(10, 5);//(15, 5);//(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
        float ax =
            0.8; // 0.25+xs*0.40;//0.6..0.2//0.75;//0.85;//1.75;//2.4;//3.0;//0.17;//0.51*4.8*0.09;//0.51*1.5;//1.2;//5;//0.45;//0.39;//角速度修正量
                 // 缩减系数
        float dx = 0.1;              // 0.056;//2.4*0.015 ;//1;//0.80;//1;//2.8;			//平移修正量  缩减系数
        float angle_p_1 = 0.19 * ax; // 0.180*ax;
        float angle_i_1 = 0;         // 0.0002*ax;//0.005*ax;  //0.06*ax;//0;
        float angle_d_1 = 0;         //-0.0070*ax;//-0.0070*ax;//
        float dis_p_1 = 5.50 * dx;
        move_test_C.parameter_set_angle(angle_p_1, angle_i_1, angle_d_1, 0);
        move_test_C.parameter_set_dis(dis_p_1, 0, 0, 0); // 0.3
        // 组成1
        // 在前景视野外的大范围里
        move_test_C.zs_point_track_01_002(ball_org_angle, ball_org_radius, 0,
                                          holdballFmin * 0.6); // ball_org_angle , 0 );
        move_test_C.zs_angle_track_001(ball_org_angle, 0 /*benchmark_angle*/, black_white_ball);
        //	move_test_C.zs_angle_track_002	 (ball_org_angle,0);

        move_test_C.zs_translate_compound_O(move_test_C.front_v_angle_c,
                                            12); // 4825,54,(1+0.9*xs)  0.8将当前运动向量与  另一个平移分量相加

        move_test_C.turn_v_maxlimit();  // 限定平移[功率]速度输出最大值
        move_test_C.front_v_maxlimit(); // 限定平移[功率]速度输出最大值
    }

    //	move_test_C  .limit_parameter_set_dis(40, 15);//(10, 5);//(15, 5);//(128,328);//float velocity_limit_dis_,float
    // acceleration_limit_dis_ 	move_test_C	 .front_v_angle_p	=robot_fva	;//将上周期的速度分量辅给
    //	move_test_C	 .front_v_p			=robot_fv	;
    //	move_test_C	 .front_a_maxlimit();

    move_test_C.uptech_output_O(motorB, motorL, motorR); // ？？？？？？？？？？？？？？
}

Difference::Difference()
{
    diff_flag = 1;  // 注意开始时 因为a_p无数据 所以计算无意义
    new_set_up = 1; // 建立标志 保证在a_p未赋值时第一次运行输出为0
    a_p = 0;
}
void Difference::New_Cycle(void) // 差分过标志 置1  在 一个周期完结 或周期刚开始 运行
{
    diff_flag = 1; // 差分过标志 清零
}
float Difference::diff(float a_c) // 差分函数
{
    if (diff_flag == 1)
    {
        float temp = a_c - a_p;
        a_p = a_c;
        diff_flag = 0;       // 差分过标志 清零
        if (new_set_up == 1) // 建立标志 保证在a_p未赋值时第一次运行输出为0
        {
            new_set_up = 0;
            return (0);
        }
        return (temp);
    }
    else
    {
        return (0);
    }
}

float Difference::diff_angle(float a_c) // 差分函数
{
    if (diff_flag == 1)
    {
        float temp = angle_add_transfer(a_c, -a_p);
        a_p = a_c;
        diff_flag = 0;       // 差分过标志 清零
        if (new_set_up == 1) // 建立标志 保证在a_p未赋值时第一次运行输出为0
        {
            new_set_up = 0;
            return (0);
        }
        return (temp);
    }
    else
    {
        return (0);
    }
}

// 路程定位 char enable是将机器定位 和 球定位 全部用全场定位决定
void Dribble::Distance_Location_2(float ball_angle, float ball_Rradius, float ball_weight, float f_x, float f_y,
                                  float f_a, char enable)
{
    f_x = f_x * FieldZoomTime;
    f_y = f_y * FieldZoomTime;
    f_a = angle_add_transfer(f_a, 0);
    // Angle_add(robot_w,&f_a);//一个周期转动量 造成的转动角度

    float T = Gb_loop_time * 0.001; // 0.067;//周期时间单位秒
    int i;                          // mlR,mbR,mrR,ball_angle,ball_Rradius;
    // 注：矫正方法 使用单独 角度对准程序矫正wp 单独平移对准程序矫正vp  矫正结果就是球在场地地图上的位置不动
    const double vp = 1; // 2.2 ;//静态2.2 ;//平移矫正系数 常数
    const double wp = 1; // 1.2 ;//静态2.0 ;//旋转矫正系数 常数
    if (enable == 1)
    {
        // 对球的对地运动矢量计算001
        // 运动反解
        float front_v = 0; // 1.99012;//临时存储 反解出的 平移速度 转动速度 平移方向
        float turn_w = 0;  // 61.9297;
        float front_a = 0; //-29.3969;
        // 电机速度真值
        // float max_a=540;
        if (1) // 使用不使用仿真 0仿真 1码盘视觉融合//EMU
        {
            Movement(T, RMfv, RMfa, RMw, &robot_L);
            Movement(T, RMfv, RMfa, RMw, &robot_L_test);

            float blc = Dis_Location; // 1.8;//不使用路程信息 1.5;  //比例路程	//blc占的比重大 定位偏向使用路程定位
                                      // 占的小偏向使用场线匹配定位
            float bcx = 2 - blc;      // 比例场线
            robot_L.f_x = (robot_L.f_x * blc + f_x * bcx) / 2.0;
            robot_L.f_y = (robot_L.f_y * blc + f_y * bcx) / 2.0;
            //	robot_L.f_a=angle_average(robot_L.f_a,f_a);//
            robot_L.f_a = f_a; //
        }
        else
        {
            // 电机速度真值 估算
            // float max_a=540;
            /*因调试去掉*/ O_movement_T_analyze(
                &front_a, &front_v, &turn_w, -mhLR[1], -mhBR[1],
                -mhRR[1]); // 0,0,0);//[0]是上个周期的结果    *move_v_angle, *move_v_speed, *turn_v
            /*因调试去掉*/ front_v = front_v * vp / wheel_linear_speed_ratio;
            /*因调试去掉*/ turn_w = turn_w * wp * 360.0 / wheel_rotate_speed_ratio;
            // SetMovement(T, front_v, front_a, turn_w, &robot_L);//(float front_v_c,float front_v_angle_c,float
            // turn_w,obj_properties *obj);
            Movement(T, front_v, front_a, turn_w, &robot_L);
        }
        if (ball_weight > 30) // 对球大小的判断  如果没有球 就按上周期球对地速度 估计球当前位置
        {
            for (i = 19; i > 0; i--) // 有延时的球在20周期内的 信息历史记录
            {
                ball_d_h[i].f_x = ball_d_h[i - 1].f_x;
                ball_d_h[i].f_y = ball_d_h[i - 1].f_y;
            }
            ball_d_h[0].f_x = l2f_x(robot_L.f_x, robot_L.f_y, robot_L.f_a, ball_angle,
                                    ball_Rradius / 100.0); // 估算后 球对地面的位置 在地面坐标系里
            ball_d_h[0].f_y = l2f_y(robot_L.f_x, robot_L.f_y, robot_L.f_a, ball_angle,
                                    ball_Rradius / 100.0); // 估算后 球对地面的位置 在地面坐标系里

            for (i = 19; i > 0; i--) // 有延时的球在20周期内的 速度信息历史记录
            {
                ball_d_h[i].f_vx = ball_d_h[i - 1].f_vx;
                ball_d_h[i].f_vy = ball_d_h[i - 1].f_vy;
            }
            ball_d_h[0].f_vx = (ball_d_h[0].f_x - ball_d_h[1].f_x) / T; // field 场地坐标x 方向上的速度
            ball_d_h[0].f_vy = (ball_d_h[0].f_y - ball_d_h[1].f_y) / T; // field 场地坐标y 方向上的速度
        }
        else
        {
            float T = Gb_loop_time * 0.001; // 0.067;//周期时间单位秒
            float bfvx =
                simple_filter(ball_d_h[0].f_vx, ball_d_h[1].f_vx, ball_d_h[2].f_vx, ball_d_h[3].f_vx, ball_d_h[4].f_vx);
            float bfvy =
                simple_filter(ball_d_h[0].f_vy, ball_d_h[1].f_vy, ball_d_h[2].f_vy, ball_d_h[3].f_vy, ball_d_h[4].f_vy);
            for (i = 19; i > 0; i--) // 有延时的球在20周期内的 信息历史记录
            {
                ball_d_h[i].f_x = ball_d_h[i - 1].f_x;
                ball_d_h[i].f_y = ball_d_h[i - 1].f_y;
            }
            ball_d_h[0].f_x += bfvx * T; // 估算后 球对地面的位置 在地面坐标系里
            ball_d_h[0].f_y += bfvy * T; // 估算后 球对地面的位置 在地面坐标系里

            for (i = 19; i > 0; i--) // 有延时的球在20周期内的 速度信息历史记录
            {
                ball_d_h[i].f_vx = ball_d_h[i - 1].f_vx;
                ball_d_h[i].f_vy = ball_d_h[i - 1].f_vy;
            }
            ball_d_h[0].f_vx = 0; //(ball_d_h[0].f_x-ball_d_h[1].f_x)/T;//field 场地坐标x 方向上的速度
            ball_d_h[0].f_vy = 0; //(ball_d_h[0].f_y-ball_d_h[1].f_y)/T;//field 场地坐标y 方向上的速度
        }
        // m_find.pField->SetAgent(11 ,robot_L.f_x*100     ,robot_L.f_y*100    ,-robot_L.f_a);//场子正中向上
        // 逆时针坐标系 m_find.pField->SetBall (11 ,ball_d_h[0].f_x*100 ,ball_d_h[0].f_y*100);
    }
    else
    //	enable!=1)// 停止时作位置清零 记录
    {
        //	Distance_Location_init();
        robot_L.f_x = f_x;
        robot_L.f_y = f_y;
        robot_L.f_a = f_a;
        for (i = 19; i > 0; i--) // 有延时的球在20周期内的 信息历史记录
        {
            ball_d_h[i].f_x = ball_d_h[i - 1].f_x;
            ball_d_h[i].f_y = ball_d_h[i - 1].f_y;
        }
        ball_d_h[0].f_x = l2f_x(robot_L.f_x, robot_L.f_y, robot_L.f_a, ball_angle,
                                ball_Rradius / 100.0); // 估算后 球对地面的位置 在地面坐标系里
        ball_d_h[0].f_y = l2f_y(robot_L.f_x, robot_L.f_y, robot_L.f_a, ball_angle,
                                ball_Rradius / 100.0); // 估算后 球对地面的位置 在地面坐标系里
        for (i = 19; i > 0; i--)                       // 有延时的球在20周期内的 速度信息历史记录
        {
            ball_d_h[i].f_vx = ball_d_h[i - 1].f_vx;
            ball_d_h[i].f_vy = ball_d_h[i - 1].f_vy;
        }
        ball_d_h[0].f_vx = (ball_d_h[0].f_x - ball_d_h[1].f_x) / T; // field 场地坐标x 方向上的速度
        ball_d_h[0].f_vy = (ball_d_h[0].f_y - ball_d_h[1].f_y) / T; // field 场地坐标y 方向上的速度
    }

    ///*显示球*/Debug_D[11].Draw_line_	   ( /*因调试去掉*/coordinate_transfer_x
    ///(angle_add_transfer(ball_angle,-90),ball_Rradius/5.0) +320
    //										,/*因调试去掉*/coordinate_transfer_y
    //(angle_add_transfer(ball_angle,-90),ball_Rradius/5.0) +240
    //,angle_add_transfer(angle_add_transfer(coordinate_transfer_angle
    //(ball_d_h[0].f_vx,ball_d_h[0].f_vy),-robot_L.f_a),-90)
    //										,coordinate_transfer_radius(ball_d_h[0].f_vx,ball_d_h[0].f_vy)*40
    //										,0,255,0,  pBuffer);

    // float axdl=	angle_add_transfer(angle_add_transfer(coordinate_transfer_angle
    // (ball_d_h[0].f_vx,ball_d_h[0].f_vy),-robot_L.f_a),0) ;
    ///*
    // 运动估算前的输出
    ball_org_angle = f2l_angle(robot_L.f_x, robot_L.f_y, robot_L.f_a, ball_d_h[0].f_x,
                               ball_d_h[0].f_y); // ball_angle;//全景球的angle;
    ball_org_radius = f2l_radius(robot_L.f_x, robot_L.f_y, robot_L.f_a, ball_d_h[0].f_x,
                                 ball_d_h[0].f_y); // ball_Rradius/100.0;//全景球的radius;
    // 运动估算
    // 全景运动估算后的输出
    ball_estimate_angle = ball_org_angle;
    ball_estimate_radius = ball_org_radius;
    float bfvx =
        simple_filter(ball_d_h[0].f_vx, ball_d_h[1].f_vx, ball_d_h[2].f_vx, ball_d_h[3].f_vx, ball_d_h[4].f_vx);
    float bfvy =
        simple_filter(ball_d_h[0].f_vy, ball_d_h[1].f_vy, ball_d_h[2].f_vy, ball_d_h[3].f_vy, ball_d_h[4].f_vy);
    Movement_estimate_delay_compute3(&ball_estimate_angle, &ball_estimate_radius, bfvx, bfvy,
                                     Gb_loop_time *
                                         0.001); //  angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
    ball_estimate_radius = ball_estimate_radius * 100.0; // 全景球的radius
    // 前景运动估算后的输出
    // ball_estimate_F_angle =ball_op_Fa;//前景球的angle
    // ball_estimate_F_radius=ball_op_Fr/100.0;//前景球的radius
    ball_estimate_F_radius = ball_estimate_radius; // 前景球的radius
    ball_estimate_F_angle = ball_estimate_angle;
    ball_org_radius = ball_org_radius * 100;
    ball_op_Fa = ball_org_angle;
    ball_op_Fr = ball_org_radius;
    // 对方门左右边缘的原只值
    ddoor_left_o =
        f2l_angle(robot_L.f_x, robot_L.f_y, robot_L.f_a, TargetGoalLX * FieldZoomTime, TargetGoalLY * FieldZoomTime);
    ddoor_right_o =
        f2l_angle(robot_L.f_x, robot_L.f_y, robot_L.f_a, TargetGoalRX * FieldZoomTime, TargetGoalRY * FieldZoomTime);
    // 对方门左右边缘的估算值
    ddoor_left_e = revise_gool_L_edge_A();
    ddoor_right_e = revise_gool_R_edge_A();
    //*/
}
void Dribble::Movement_estimate_delay_compute3(
    float *angle, float *radius, float fvx, float fvy,
    float T) // angle单位[度] radius 单位[米]  主要对对地面静止的物体有效 如门
{
    // float    T =Gb_loop_time*0.001;//0.067;//周期时间单位秒
    obj_properties robot_L_estimate, ball_estimate;
    robot_L_estimate.f_x = robot_L.f_x; // robot_L;有延时的 机体 直角坐标系位置  通过全场定位得到 其实也可以用任意值
    robot_L_estimate.f_y = robot_L.f_y;
    robot_L_estimate.f_a = robot_L.f_a;
    ball_estimate.f_x =
        l2f_x(robot_L.f_x, robot_L.f_y, robot_L.f_a, *angle, *radius) + fvx * T; // 估算后 球对地面的位置 在地面坐标系里
    ball_estimate.f_y =
        l2f_y(robot_L.f_x, robot_L.f_y, robot_L.f_a, *angle, *radius) + fvy * T; // 估算后 球对地面的位置 在地面坐标系里

    Movement(T, RMfv, RMfa, RMw, &robot_L_estimate);

    *radius = f2l_radius(robot_L_estimate.f_x, robot_L_estimate.f_y, robot_L_estimate.f_a, ball_estimate.f_x,
                         ball_estimate.f_y);
    *angle = f2l_angle(robot_L_estimate.f_x, robot_L_estimate.f_y, robot_L_estimate.f_a, ball_estimate.f_x,
                       ball_estimate.f_y);
}
