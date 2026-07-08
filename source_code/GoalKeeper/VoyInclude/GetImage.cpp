// GetImage.cpp: implementation of the CGetImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
// #include "VoyVideo.h"
#include "GetImage.h"
#include "MSL_Protocol.h"
// #include "motion_predict.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define SOFTWARE_VER 1.0f //***程序版本号，会发送到教练机***

// float debug[600];  int debugP=0;  float debug2[600];  int debug2P=0;  float debug3[600];  int debug3P=0;
float trun__set = 0;  // 设置防守旋转速度
float rb_angle_p = 0; // 设置防守对准上次角度

int in_fouce_stat = 0; // 我在瞄准准备传球
float T_r_PP = 0;
float y_ball_chang_min = 0;
float y_ball_chang_real_min = 0;
float ballC_sx = 9;
float ballC_sy = 6;
float rb_dis_p = 0;
float rb_d_dis_h[6] = {0, 0, 0, 0, 0, 0};
float v_radius_set = 0;
float t_radius1_h[20];
float t_radius2_h[20];
float ball_f_x_h[20];
float ball_f_y_h[20];
float ball_f_hw[20];

float t_radius1_107_h[20];

float Display_A = 0, Display_B = 0, Display_C = 0;
float catchballtempX[80];
float catchballtempY[80];

void CGetImage::zhuiqiu_(double mujiao, double muchang, double c_shang)
{
    float xiang_dui_sudu = (muchang - 50) * 1.5 / (69.0 - 50.0) + 1.0;
    if (xiang_dui_sudu > 2.5)
        xiang_dui_sudu = 2.5;
    if (xiang_dui_sudu < 2.1)
        xiang_dui_sudu = 2.1;
    int xuanzhuan1;
    int n;
    if (muchang < 69)
        n = (muchang - c_shang + xiang_dui_sudu) * 400;
    else if (muchang < 100)
        n = (muchang - c_shang + 2.5) * 400;
    else if (muchang < 130)
        n = (muchang - c_shang + 2.5) * 200;
    else
        n = (muchang - c_shang + 3.5) * 300;
    if (n < 200 && n > -1600) // && abs(mujiao)<30)
        jifen = jifen + n;
    else if (n <= -1600)
        jifen -= 1600;
    else
        // if(abs(mujiao)<30)
        jifen += 200;
    if (jifen > 10000) // 对此微分的积分量的上下限进行限制
        jifen = 10000;
    if (jifen < 3000)
        jifen = 3000;
    jifen -= abs(y.ball_jiao);
    //	jifen+=(y.ball_chang-60)*1;

    zhuan = 2 * duizhun1(mujiao, 0, 2000); // 追球的转动量
    if (zhuan > 0 && zhuan < 300)
        zhuan = 300;
    if (zhuan < 0 && zhuan > -300)
        zhuan = -300;
    if (abs(zhuan) > abs(zhuan_shang))
    {
        if (zhuan - zhuan_shang > 180)
            zhuan = zhuan_shang + 180;
        if (zhuan - zhuan_shang < -180)
            zhuan = zhuan_shang - 180;
    }
    if (muchang > 65)
        Controlmoto((long)-Path_LA, (long)jifen, 20000, &mtl, &mtr, &mtb);
    else
        Controlmoto((long)y.ball_jiao, (long)jifen, 20000, &mtl, &mtr,
                    &mtb);   // 追球的平移分量 速度是长度关系+速度积分+加速度
    qiansudu = jifen + 1800; //; //保留追球时候向前的速度分量
                             //		if(y.quanbx>1300 || y.quanbx<100 || y.quanby>1100 || y.quanby<100)
    //			Controlmoto(y.ball_jiao+90,-duizhun(jiajiao(y.ddoor_jiao,y.ball_jiao),0,2000),10000,&mtl,&mtr,&mtb);
    zhuangtai = "执行追球";
    chongqiu = 0;
}

/*************************************************************************************************************************
** 函数名称:			CalcPenaltyAreaBorder
**
** 函数描述:			禁区边缘计算函数，输入量为目标位置，输出量为该位置与球门中点所确定的直线，与禁区边线的交点坐标。
** 					当目标位置坐标位于对方禁区内的时候，返回己方球门中点与目标位置连线与对方禁区边线交点。
**
**
** 输入变量:			float target_x, float target_y, float penalty_x, float penalty_y, *float p_limit_x ,*float
*p_limit_y;
** 返回值:			void;
**
** 使用宏或常量:		None;
** 使用全局变量:		None;
**
** 调用函数			;
**
** 创建人:			律晔
** 创建日期:			2010-06-21
**------------------------------------------------------------------------------------------------------------------------
** 修订人:			律晔
** 修订日期:			2010-060-22
**------------------------------------------------------------------------------------------------------------------------
*************************************************************************************************************************/
void CalcPenaltyAreaBorder(float target_x, float target_y, float penalty_x, float penalty_y, float *p_limit_x,
                           float *p_limit_y)
{
#define TOUCH_LINE_Y 600
#define GOAL_LINE_X 1800

    float slope = 0;
    float up_border = 0;
    float down_border = 0;

    float temp_target_y = 0;
    float temp_target_x = 0;

    if (target_x < 10)
    {
        temp_target_x = 10;
    }
    else if (target_x > (GOAL_LINE_X - 10))
    {
        temp_target_x = GOAL_LINE_X - 10;
    }
    else
    {
        temp_target_x = target_x;
    }

    if (target_y > TOUCH_LINE_Y)
    {
        temp_target_y = TOUCH_LINE_Y - 10;
    }
    else if (target_y < -TOUCH_LINE_Y)
    {
        temp_target_y = -TOUCH_LINE_Y + 10;
    }
    else
    {
        temp_target_y = target_y;
    }

    slope = temp_target_y / temp_target_x; // 计算输入位置与球门中点所确定的直线的斜率

    if (((temp_target_x < penalty_x) && (temp_target_y < penalty_y) && (temp_target_y > -penalty_y)))
    {

        up_border = penalty_y / penalty_x; // 斜率上边界计算
        down_border = -up_border;          // 斜率下边界计算

        if (slope >= up_border)
        {
            *p_limit_x = penalty_y / slope;
            *p_limit_y = penalty_y;
        }
        else if (slope <= down_border)
        {
            *p_limit_x = -(penalty_y / slope);
            *p_limit_y = -penalty_y;
        }
        else
        {
            *p_limit_x = penalty_x;
            *p_limit_y = slope * penalty_x;
        }
    }
    else if (temp_target_x > (GOAL_LINE_X - penalty_x))
    {
        *p_limit_x = GOAL_LINE_X - penalty_x;
        *p_limit_y = slope * (GOAL_LINE_X - penalty_x);
    }
    else
    {
        *p_limit_x = temp_target_x;
        *p_limit_y = temp_target_y;
    }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// Jiaoa:球与球门的连线与球和机器连线的夹角
// Jiaoc:球与球门中点的连线与水平方向的夹角
// ball_Rradius :球与机器的距离
float test_c_Va = 0; // 设置 测试电流闭环 验证程序 速度变量
float test_c_V = 0;
float test_c_W = 0;
float time_cb_1 = 0; // 抢球计时器1  记录已经有几周期达到或超过需要电流  做完准备动作
float time_cb_2 = 0; // 抢球计时器2  记录已经进行截断动作  多少时间

int m_pMotorPos_P[3] = {0, 0, 0};     // 上周期码盘位置
int m_pMotorPos_H[100][3];            // 上100帧周期码盘位置历史记录
int m_pMotorCurrent_P[3] = {0, 0, 0}; // 上周期电流
int m_pIO_P[4] = {0, 0, 0, 0};        // 上周期io
int m_pAD_P[4] = {0, 0, 0, 0};        // 上周期ad
int PowerOnDelay = 0;                 // 记录检测到上电多少帧后开始数据稳定 PowerOn置1
int PowerOn = 0;

float Pw; // 转速上次数据

void CGetImage::call_machine_location(double jiaoa, double ball_Rradius, double *tf_x, double *tf_y, double fx,
                                      double fy, double fa, double br, double ba)
{
    double jiaob, jiaoc, fbx, fby;
    fbx = l2f_x(fx, fy, fa, ba, br);
    fby = l2f_y(fx, fy, fa, ba, br);
    jiaoc = atan2(600 - fby, 0 - fbx);
    jiaob = jiaoc - jiaoa * 3.14 / 180.0;

    double fb_dis = sqrt((0 - fbx) * (0 - fbx) + (600 - fby) * (600 - fby));
    if (fb_dis <= 100)
    {
        jiaob = jiaoc;
        *tf_x = fbx;
        *tf_y = fby;
        return;
    }
    else if (ball_Rradius > fb_dis - 100)
        ball_Rradius = fb_dis - 100;

    *tf_x = fbx + ball_Rradius * cos(jiaob);
    *tf_y = fby + ball_Rradius * sin(jiaob);
    // printf("y.benjix=%.6f,y.benjiy=%.6f",&y.benjix,&y.benjiy);
}

CGetImage::CGetImage()
{
    ballzdj = 0;
    smjishu++;
    puqiulj = 0;
    dzhenlv = 0;
    dbx_s = 0;
    dby_s = 0;
    xiqiuz_s = 0;
    xiqiuy_s = 0;
    mujiao_s = 1000;
    fSoftwareVer = SOFTWARE_VER;
    nVisionFps = 0;
    motol_as = 0;
    motor_as = 0;
    motob_as = 0;
    zwxiqiu = 0;
    xiqiuzhi = 0;
    move_Emu_ballX = 9;
    move_Emu_ballY = 6;
    move_Emu_robot_f_x = 9;
    move_Emu_robot_f_y = 6;
    move_Emu_robot_f_a = 0;
    move_Emu_ballSX = 9;
    move_Emu_ballSY = 6;
    move_Emu_robot_Sf_x = 9;
    move_Emu_robot_Sf_y = 6;
    move_Emu_robot_Sf_a = 0;
    catch_start_wait_loop = 0; // 接球 队员 切接球后 等待球速度方向稳定 等待周期计数
    xingwei_catch_timer = 0;
    catch_xingwei_P = 1002; // 接球队员最后执行的是接球 还是接球跑位 默认1002接球跑位
    move_Net_ballf_SX = 9;
    move_Net_ballf_SY = 6;
    jieqiuqs = 0;
    jieqiux = 0;
    jieqiuy = 0;
    jdx = 0;
    jdy = 0;
    jieqiusudu_s = 0;
    jieqiuc_s = 0;
    jieqiujf = 0;
    catch_first_view_ball_X = 0;
    catch_first_view_ball_Y = 0;
    catch_first_view_ball_W = 0;

    daduobi = 0;
    y.benjix = 0;
    y.benjiy = 0;
    y.benjijiao = 0;
    jifen_s = 0;
    jieqiuys = 0;
    benjix_s = 900;
    benjiy_s = 600;
    benjijiao_s = 0;
    daiqiumb = 0;
    pass_move_stick_timer = 0;
    kaijipd = 0;
    pqzy = 100;
    kaiqiujfx = 0;
    kaiqiujfy = 0;
    kaiqiusj = 0;
    kaiqiubk = 0;
    kaiqiudj = 0;
    chiqiupd = 0;
    gbpanding = 50;
    pxqiehuan = 0;
    pxqiehuanbj = 0;
    for (i = 0; i < 1000; i++)
    {
        bxyc[i] = 0;
        byyc[i] = 0;
    }
    for (i = 0; i < 90; i++)
    {
        smycy[i] = 100000;
        smycy[i] = 100000;
    }
    for (i = 0; i < 1000; i++)
    {
        quanball_h[i][0] = 0; //[0]y,[1]x,[2]w //全场球信息历史记录
        quanball_h[i][1] = 0; //[0]y,[1]x,[2]w //全场球信息历史记录
        quanball_h[i][2] = 0; //[0]y,[1]x,[2]w //全场球信息历史记录
    }
    for (i = 0; i < 100; i++)
    {
        m_pMotorPos_H[i][0] = 0; // 上100帧周期码盘位置历史记录
        m_pMotorPos_H[i][1] = 0; // 上100帧周期码盘位置历史记录
        m_pMotorPos_H[i][2] = 0; // 上100帧周期码盘位置历史记录
    }
    Shot_ball_point_X = 0; // 球起飞全场定位点x
    Shot_ball_point_Y = 0; // 球起飞全场定位点y
    Shot_ball = 0;         // 球起飞判定
    set_speed_x = 0;       // 存储机体在场地上的希望行进速度
    set_speed_y = 0;
    min_dis_ball_x = 0; // 存储从起飞判定成立开始距机体最近的球的全场坐标 球没起飞时为球的当前场地坐标
    min_dis_ball_y = 0;

    fangshoujiao = 0;
    leijiayc = 0;
    leijiaycx = 0;
    leijiaycy = 0;
    diuqiuleijia = 0;
    bjxyc = 320;
    bjyyc = 240;
    bjjyc = 0;
    leijia = 0;
    leijia1 = 0;
    int_zijian(0, 0, 0);
    movesudu = 0;
    movepanding = 0;
    meny = 0;
    cdbl = MK;
    xing13 = 0;
    kaiqiuting = 0;
    gbxiang = 0;
    kaifuzhu = 0;
    guibizhuangtai = 0;
    QueryPerformanceFrequency(&Frequency1);
    QueryPerformanceCounter(&CountStart1);
    QueryPerformanceCounter(&Loop_Time_Count_Start); // 存储 每周期计算时间 和帧数
    dfElapseMS4 = 0;
    ms_shang1 = 0;
    // ms_shang=0;
    num = 0;
    L_moto1 = 0;
    R_moto1 = 0;
    int xing13;
    //	m_para.ReSet();
    //	m_para.Ven = TRUE;
    //	m_para.Vmin = 55;
    //	m_para.Vmax = 127;
    // m_vis.SetParam(&m_find);
    //	UPRColor *m_find2=new UPRColor;
    moto_l = 0;
    moto_r = 0;
    moto_b = 0;
    tiqiu = 0;
    valxx = 1;
    shemenleijia = 0;
    netmove = 1;
    qiansudu = 0;
    jiasudu1 = 0;
    jifen = 0;
    zhenshu = 0;
    zhuan_shang = 0;
    leijiaqi = 0;
    ms_shang[0] = 0;
    ms_shang[1] = 0;
    ms_shang[2] = 0;
    ms_shang[3] = 0;
    ms_shang[4] = 0;
    ms_shang[5] = 0;
    ms_shang[6] = 0;
    ms_shang[7] = 0;
    ms_shang[8] = 0;
    ms_shang[9] = 0;
    qishipanding = 0;
    qishipanding1 = 0;
    tingzhipanding = 0;
    tingzhipanding1 = 0;
    y.ball_jiao = 0;
    y.ball_chang = 0;
    xingwei = 6;
    holdball = 55;
    paowei = 0;
    paoweileiji = 0;
    xingwei = 0;
    shijian = 0;
    shijiandian = -500;
    pingjun = 0;
    daiqiuz_s = 0;
    for (int i = 0; i < 144; i++) // 将路径规划 Cpathplan类 中用OPENCV得到的 机体不处理区域信息 赋给全局变量
        robot_min_radius[i] = pp1.robot_min_r[i];
    ball_fxp = 1800 / 2;
    ball_fyp = 1200 / 2;
    gankzt = 0;
    gankpq = 0;
    ganksx = 0;
    gankjs = 0;
    m_bNetSend = true;
    xqpanding = 0;
    xqjishu = 0;
    djkaiqiu = 0;
    djkaiqiujs = 0;
    first_touch_bfx = 0; // 首次触球 场地坐标系x
    first_touch_bfy = 0; // 首次触球 场地坐标系y
    first_touch_bt = 0;  // 首次触球 有效计数器0 非触球 >0触球过程中
    field_p_num_P = 0;   // 记录上周期（进攻 传球 跑位）目的路点编号

    xingwei_1001_shoot_end_timer = 0; // 球传出修正接球位置  保持时间标志
}

CGetImage::~CGetImage()
{
    //	delete m_find2;
}

void CGetImage::AfterUpdateVideoSample(BYTE *pBuffer, long lWidth, long lHeight, double dbTime, UINT nState)
{
    // m_find2->ImageBuf(lWidth,lHeight,pBuffer);
    RGBTRIPLE ball;
    ball.rgbtBlue = 0x00;
    ball.rgbtGreen = 0x00;
    ball.rgbtRed = 0x00;
    for (long i = 0; i < 230400; i++)
        tuxiang2[i] = pBuffer[i];
    for (int x = 155; x < 165; x++)
        for (int y = 0; y < 240; y++)
        {
            pBuffer[y * 960 + x * 3] = 255;
            pBuffer[y * 960 + x * 3 + 1] = 255;
            pBuffer[y * 960 + x * 3 + 2] = 255;
        }
}

void CGetImage::zhuanhuan(int x)
{
    y.val_qiehuan = x;
}

void CGetImage::jiaoduizhun(int *motol, int *motor, int jiao, int jizhun, double zhuan, double qian)
{ // 转动时候向前前进的速度
    int xx;
    double yy;
    double jiao1;
    jiao1 = jiao;
    jiao1 = jiao1 + jizhun;
    if (jiao1 > 180 || jiao1 < -180)
        jiao1 = -jiao1 - 2 * jizhun;
    if (jiao1 >= 0)
        xx = (jiao1) * (jiao1)*zhuan;
    else
        xx = -(jiao1) * (jiao1)*zhuan;
    yy = qian;
    // yy=(y.ball_chang-4)*(y.ball_chang-4)*(y.ball_chang-4)*qian;
    if (yy > qian)
        yy = qian;
    if (xx > 2000)
        xx = 2000;
    if (xx < -2000)
        xx = -2000;
    *motol = -xx + yy;
    *motor = xx + yy;
}

int CGetImage::jiajiao(int jizhun, int jiao)
{
    double jiajiao;
    jiajiao = jiao - jizhun;
    if (jiajiao > 180)
        jiajiao = -360 + jiajiao;
    if (jiajiao < -180)
        jiajiao = 360 + jiajiao;
    return jiajiao;
}

#ifdef RUN_WITH_EMU

// 建立仿真网络的变量和接收函数
// server
SOCKET m_receive_EMU_server_skt;
SOCKADDR_IN ServerAdr;
int targAdrlen;

// target
SOCKADDR_IN targAdr;
SOCKET m_send_EMU_server_skt;
//	unsigned char m_tarIP[4];

UINT UDPReceiveThread(LPVOID pParam)
{
    //	CUDPServer* pudp = (CUDPServer*) pParam;

    int reclen = 0;

    SOCKADDR linkAdr;
    int len = sizeof(SOCKADDR);

    while (1) // pudp->m_serverskt != INVALID_SOCKET)
    {
        reclen = recvfrom(m_receive_EMU_server_skt, (char *)&ReceiveServerData, sizeof(serverSendStruct), 0,
                          (SOCKADDR *)&linkAdr, &len);
#ifdef RUN_WITH_EMU_LINK_MSLCoach2 // 设置使用本机上的2号教练机 用于测试双队对抗
        for (int i = 0; i < 12; i++)
        {
            ReceiveServerData.robotLocal_x[i] = 1800 - ReceiveServerData.robotLocal_x[i];
            ReceiveServerData.robotLocal_y[i] = 1200 - ReceiveServerData.robotLocal_y[i];
            ReceiveServerData.robotLocal_a[i] = angle_add_transfer(ReceiveServerData.robotLocal_a[i], 180);
        }
        ReceiveServerData.ballLocal_x = 1800 - ReceiveServerData.ballLocal_x;
        ReceiveServerData.ballLocal_y = 1200 - ReceiveServerData.ballLocal_y;
#endif
#ifdef TECHCHALLENGE2013_1
        for (int i = 0; i < 12; i++)
        {
            ReceiveServerData.robotLocal_x[i] = 1800 - ReceiveServerData.robotLocal_x[i];
            ReceiveServerData.robotLocal_y[i] = 1200 - ReceiveServerData.robotLocal_y[i];
            ReceiveServerData.robotLocal_a[i] = angle_add_transfer(ReceiveServerData.robotLocal_a[i], 180);
        }
        ReceiveServerData.ballLocal_x = 1800 - ReceiveServerData.ballLocal_x;
        ReceiveServerData.ballLocal_y = 1200 - ReceiveServerData.ballLocal_y;
#endif

        if (reclen != SOCKET_ERROR)
        {
            reclen = 0;
        }
    }

    /*	if (pudp->m_serverskt != INVALID_SOCKET)
        {
            closesocket(pudp->m_serverskt);
            pudp->m_serverskt = INVALID_SOCKET;
        }
    */
    return 1L;
}

#ifdef RUN_WITH_LOCAL_EMU
IplImage *LOCAL_EMU_IMG;
int pass_test_flag = 0;

Emu_Robot robot_Cy[6];
obj_properties ballC;
IplImage *main_disp_data; // 定义输出和运算用图像指针
char *main_disp_image_data;
int EMU_init_flag = 1;
void CGetImage::AfterUpdateOverlookEMU(BYTE *pBuffer, long lWidth, long lHeight, double dbTime, UINT nState)
{
    if (xingwei == 30)
    {
        Sleep(300); // cvWaitKey(500);
        if (pass_test_flag == 0)
            pass_test_flag = 100;
        if (pass_test_flag == 100)
            EMU_init_flag = 1;
    }
    if (xingwei == 11)
    {
        robot_Cy[m_nAgentID_g_set].robot_emu.f_x = 900; // 900+450;
        robot_Cy[m_nAgentID_g_set].robot_emu.f_y = 600; // 600+200;
        robot_Cy[m_nAgentID_g_set].robot_emu.f_a = 0;
        ballC.f_x = 1800; //(200+1000)/100;
        ballC.f_y = 600;  // 60.0/100.0;
    }
    if (pass_test_flag > 0)
        pass_test_flag--;

    if (EMU_init_flag == 1)
    {
        EMU_init_flag = 0;
        LOCAL_EMU_IMG = cvLoadImage(".\\Config\\image1.bmp");

        cvNamedWindow("main_disp", CV_WINDOW_AUTOSIZE); // 开主显示窗口
        main_disp_data = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
        main_disp_image_data = main_disp_data->imageData;
        main_disp_data->origin = 1;

        robot_Cy[m_nAgentID_g_set].robot_emu.f_x = 900; // 900+450;
        robot_Cy[m_nAgentID_g_set].robot_emu.f_y = 600; // 600+200;
        robot_Cy[m_nAgentID_g_set].robot_emu.f_a = 0;
        robot_Cy[m_nAgentID_g_set].robot_emu.f_w = 0;
        robot_Cy[m_nAgentID_g_set].robot_emu.f_vx = 0;
        robot_Cy[m_nAgentID_g_set].robot_emu.f_vy = 0;
        robot_Cy[m_nAgentID_g_set].robot_emu.mass = 10; // 机器
        ballC.f_x = 1800;                               //(200+1000)/100;
        ballC.f_y = 600;                                // 60.0/100.0;
        ballC.f_vx = -2;                                //-3.8;
        ballC.f_vy = +0.4;
        ballC.mass = 1; // 球
    }
    ballC.f_x = ballC.f_x * 0.01;
    ballC.f_y = ballC.f_y * 0.01;

    float T = 0.066;
    int j, i = m_nAgentID_g_set;
    float ball_ra = 0.2; // reduce_acceleration球的减速 加速度 设置
    robot_Cy[i].BallMovement2(ball_ra, T, ballC.f_vx, ballC.f_vy, &ballC);
    robot_Cy[i].hit3_r(40, 1, &robot_Cy[i].robot_emu,
                       &ballC); // 在这里加机体和球的碰撞检测会 比较便于带球代码执行(显示带球效果好)
    robot_Cy[i].hit_film_line(1, &ballC);
    ballC.f_x = ballC.f_x * 100;
    ballC.f_y = ballC.f_y * 100;
    for (j = 0; j < 6; j++)
    {
        ReceiveServerData.robotLocal_x[j] = 900;
        ReceiveServerData.robotLocal_y[j] = 0;
        ReceiveServerData.robotLocal_a[j] = 0;
    }
    for (j = 0; j < 6; j++)
    {
        ReceiveServerData.robotLocal_x[j + 6] = 900;
        ReceiveServerData.robotLocal_y[j + 6] = 0;
        ReceiveServerData.robotLocal_a[j + 6] = 0;
    }
    ReceiveServerData.robotLocal_x[i] = robot_Cy[i].robot_emu.f_x;
    ReceiveServerData.robotLocal_y[i] = robot_Cy[i].robot_emu.f_y;
    ReceiveServerData.robotLocal_a[i] = robot_Cy[i].robot_emu.f_a;
    ReceiveServerData.ballLocal_x = ballC.f_x;
    ReceiveServerData.ballLocal_y = ballC.f_y;

    for (j = 0; j < 640 * 480 * 3; j++)
        pBuffer[j] = 10;

    unsigned char *LOCAL_EMU_IMGdata = (unsigned char *)LOCAL_EMU_IMG->imageData;
    for (j = 0; j < 640 * 480 * 3; j++)
        pBuffer[j] = LOCAL_EMU_IMGdata[j];

    AfterUpdateOverlook(pBuffer, lWidth, lHeight, dbTime, nState);

    float front_v_c;
    float turn_v_c;
    float front_v_a_c;

    // 更新机器人自己探测到自己的位置

    // srobotrobot_Cy[0].load_obj_state(&temp_robot,1);
    robot_Cy[0].update_robot_location(robot_Cy[0].robot_emu);
    // srobotrobot_Cy[1].load_obj_state(&temp_robot,1);
    robot_Cy[1].update_robot_location(robot_Cy[1].robot_emu);
    robot_Cy[2].update_robot_location(robot_Cy[2].robot_emu);
    robot_Cy[3].update_robot_location(robot_Cy[3].robot_emu);
    robot_Cy[4].update_robot_location(robot_Cy[4].robot_emu);
    robot_Cy[5].update_robot_location(robot_Cy[5].robot_emu);
    // 更新机器人看到的球的位置
    // ballC_d.f_x=ballC_d.f_x*100;
    // ballC_d.f_y=ballC_d.f_y*100;
    // ballC  .f_x=ballC  .f_x*100;
    // ballC  .f_y=ballC  .f_y*100;

    robot_Cy[0].update_ball_door(ballC); //(ballC_d);
    robot_Cy[1].update_ball_door(ballC);
    robot_Cy[2].update_ball_door(ballC); //(ballC_d);
    robot_Cy[3].update_ball_door(ballC);
    robot_Cy[4].update_ball_door(ballC); //(ballC_d);
    robot_Cy[5].update_ball_door(ballC);
    // ballC_d.f_x=ballC_d.f_x*0.01;
    // ballC_d.f_y=ballC_d.f_y*0.01;
    // ballC  .f_x=ballC  .f_x*0.01;
    // ballC  .f_y=ballC  .f_y*0.01;

    ballC.f_x = ballC.f_x * 0.01;
    ballC.f_y = ballC.f_y * 0.01;
    char hold_ball_flag = 0;
    float tempholdballradius = robot_Cy[i].ball.radius;
    if ((robot_Cy[i].ball.radius < 55) && (fabs(robot_Cy[i].ball.angle) < 11))
    {
        hold_ball_flag = 1;
        tempholdballradius = 54;
    }
    O_movement_T_analyze(&front_v_a_c, &front_v_c, &turn_v_c, -sendToServerData.serverReceiveEMU_motospeed[0],
                         -sendToServerData.serverReceiveEMU_motospeed[1],
                         -sendToServerData.serverReceiveEMU_motospeed[2]); // *move_v_angle, *move_v_speed, *turn_v
    front_v_c = front_v_c / wheel_linear_speed_ratio;
    turn_v_c = turn_v_c * 360.0 / wheel_rotate_speed_ratio;
    front_v_a_c = front_v_a_c;
    robot_Cy[i].SetMovement(T, front_v_c, front_v_a_c, turn_v_c);

    if (sendToServerData.kickTime > 10)
    {
        float ball_f_xtemp = l2f_x(robot_Cy[i].robot_emu.f_x, robot_Cy[i].robot_emu.f_y, robot_Cy[i].robot_emu.f_a, 0,
                                   (tempholdballradius + 50) / 100.0);
        float ball_f_ytemp = l2f_y(robot_Cy[i].robot_emu.f_x, robot_Cy[i].robot_emu.f_y, robot_Cy[i].robot_emu.f_a, 0,
                                   (tempholdballradius + 50) / 100.0);
        ballC.f_x = ball_f_xtemp / 100.0;
        ballC.f_y = ball_f_ytemp / 100.0;

        float ball_f_vxtemp = l2f_x(0, 0, robot_Cy[i].robot_emu.f_a, 0, sendToServerData.kickTime * 3.0 / 14000.0);
        float ball_f_vytemp = l2f_y(0, 0, robot_Cy[i].robot_emu.f_a, 0, sendToServerData.kickTime * 3.0 / 14000.0);
        ballC.f_vx = ball_f_vxtemp;
        ballC.f_vy = ball_f_vytemp;
    }
    else if (hold_ball_flag == 1)
    {
        float ball_f_xtemp = l2f_x(robot_Cy[i].robot_emu.f_x, robot_Cy[i].robot_emu.f_y, robot_Cy[i].robot_emu.f_a, 0,
                                   tempholdballradius);
        float ball_f_ytemp = l2f_y(robot_Cy[i].robot_emu.f_x, robot_Cy[i].robot_emu.f_y, robot_Cy[i].robot_emu.f_a, 0,
                                   tempholdballradius);
        float rr = f2l_radius(robot_Cy[i].robot_emu.f_x, robot_Cy[i].robot_emu.f_y, robot_Cy[i].robot_emu.f_a,
                              ball_f_xtemp, ball_f_ytemp);
        ballC.f_x = ball_f_xtemp / 100.0;
        ballC.f_y = ball_f_ytemp / 100.0;
    }
    ballC.f_x = ballC.f_x * 100;
    ballC.f_y = ballC.f_y * 100;

    //		robot_Cy[i].hit3_r(40,1,&robot_Cy[i].robot_emu,&ballC);//在这里加机体和球的碰撞检测会
    // 对于带球代码执行比较困难(显示带球效果不好 容易把球顶飞)
    for (j = 0; j < 640 * 480 * 3; j++)
        main_disp_image_data[j] = pBuffer[j];

    cvShowImage("main_disp", main_disp_data);
    cvWaitKey(1);
}

#endif

#endif
int xingwei_org = 0;

const float F[] = {1, 1, 0, 1};                                         /*状态转移矩阵*/
CvKalman *kalman_X = NULL; /*状态向量为维，观测向量为维，无激励输入维*/ // State is phi, delta_phi - angle and angular
                                                                        // velocity // Initialize with random guess
CvMat *z_k_X = NULL;                                                    /*定义观测变量*/
CvKalman *kalman_Y = NULL; /*状态向量为维，观测向量为维，无激励输入维*/ // State is phi, delta_phi - angle and angular
                                                                        // velocity // Initialize with random guess
CvMat *z_k_Y = NULL;                                                    /*定义观测变量*/
int kalman_init_flag = 1;                                               // 卡尔曼滤波器 每次使用前初始化标志

CvKalman *kalman_GYRO_angleD = NULL;
/*状态向量为维，观测向量为维，无激励输入维*/ // State is phi, delta_phi - angle and angular velocity // Initialize
                                             // with random guess
CvMat *z_k_GYRO_angleD = NULL;               /*定义观测变量*/
int kalman_GYRO_angleD_init_flag = 1;        // 卡尔曼滤波器 每次使用前初始化标志

CvKalman *kalman_test = NULL;
/*状态向量为维，观测向量为维，无激励输入维*/ // State is phi, delta_phi - angle and angular velocity // Initialize
                                             // with random guess
CvMat *kalman_test_z_k_Mat = NULL;           /*定义观测变量*/
int kalman_test_init_flag = 1;               // 卡尔曼滤波器 每次使用前初始化标志

void kalman_init(CvKalman *&kalman_filter, CvMat *&kalman_z_k_Mat, double measure_noise, double Kalman_init_state_post)
{
    kalman_z_k_Mat = cvCreateMat(1, 1, CV_32FC1);
    kalman_filter = cvCreateKalman(2, 1, 0);
    memcpy(kalman_filter->transition_matrix->data.fl, F, sizeof(F));
    /*初始化转移矩阵，行列，具体见CvKalman* kalman = cvCreateKalman( 2, 1, 0 );*/     // Initialize other Kalman
                                                                                      // parameters
    cvSetIdentity(kalman_filter->measurement_matrix, cvRealScalar(1));                /*观测矩阵*/
    cvSetIdentity(kalman_filter->process_noise_cov, cvRealScalar(1e-5));              /*过程噪声*/
    cvSetIdentity(kalman_filter->measurement_noise_cov, cvRealScalar(measure_noise)); /*观测噪声*/
    cvSetIdentity(kalman_filter->error_cov_post, cvRealScalar(1)); /*后验误差协方差*/ // Choose random initial state

    kalman_filter->state_post->data.fl[0] = Kalman_init_state_post;
    return;
}
double kalman_Correct_(CvKalman *kalman_filter, CvMat *kalman_z_k_Mat, double measure_noise, double measure_data)
{
    cvSetIdentity(kalman_filter->measurement_noise_cov, cvRealScalar(measure_noise)); // 观测噪声

    kalman_z_k_Mat->data.fl[0] = measure_data;                                         // 观测数据
    const CvMat *y_k_GYRO_angleD = cvKalmanPredict(kalman_filter, 0); /*激励项输入为*/ // Generate Measurement (z_k)
    cvKalmanCorrect(kalman_filter, kalman_z_k_Mat); // Apply the transition matrix F and apply"process noise" w_k
    return (y_k_GYRO_angleD->data.fl[0]);
}
double kalman_Correct(CvKalman *kalman_filter, CvMat *kalman_z_k_Mat, double measure_data)
{
    kalman_z_k_Mat->data.fl[0] = measure_data;                                        // 观测数据
    const CvMat *kalman_y_k_Mat = cvKalmanPredict(kalman_filter, 0); /*激励项输入为*/ // Generate Measurement (z_k)
    cvKalmanCorrect(kalman_filter, kalman_z_k_Mat); // Apply the transition matrix F and apply"process noise" w_k
    return (kalman_y_k_Mat->data.fl[0]);
}
void kalman_delete(CvKalman *kalman_filter, CvMat *kalman_z_k_Mat)
{
    cvReleaseMat(&kalman_z_k_Mat);
    cvReleaseKalman(&kalman_filter);
    kalman_z_k_Mat = NULL;
    kalman_filter = NULL;
}

struct Kalman_lite
{
    CvKalman *kalman_L_filter;
    /*状态向量为维，观测向量为维，无激励输入维*/ // State is phi, delta_phi - angle and angular velocity //
                                                 // Initialize with random guess
    CvMat *kalman_L_z_k_Mat;                     /*定义观测变量*/
    int kalman_L_init_flag;                      // 卡尔曼滤波器 每次使用前初始化标志
    Kalman_lite()
    {
        kalman_L_filter = NULL;
        /*状态向量为维，观测向量为维，无激励输入维*/ // State is phi, delta_phi - angle and angular velocity //
                                                     // Initialize with random guess
        kalman_L_z_k_Mat = NULL;                     /*定义观测变量*/
        kalman_L_init_flag = 1;                      // 卡尔曼滤波器 每次使用前初始化标志
    }
};

void kalman_L_init(Kalman_lite *Kalman_l, double measure_noise, double Kalman_init_state_post)
{
    if (Kalman_l->kalman_L_init_flag == 1)
    {
        Kalman_l->kalman_L_init_flag = 0;
        kalman_init(Kalman_l->kalman_L_filter, Kalman_l->kalman_L_z_k_Mat, measure_noise, Kalman_init_state_post);
    }
}
double kalman_L_Correct_(Kalman_lite *Kalman_l, double measure_noise, double measure_data)
{
    return (kalman_Correct_(Kalman_l->kalman_L_filter, Kalman_l->kalman_L_z_k_Mat, measure_noise, measure_data));
}
double kalman_L_Correct(Kalman_lite *Kalman_l, double measure_data)
{
    return (kalman_Correct(Kalman_l->kalman_L_filter, Kalman_l->kalman_L_z_k_Mat, measure_data));
}
void kalman_L_delete(Kalman_lite *Kalman_l)
{
    kalman_delete(Kalman_l->kalman_L_filter, Kalman_l->kalman_L_z_k_Mat);
}
Kalman_lite Kalman_lite_test;

/************************************************************************/
/*
//机器人传感器信息
int m_pMotorPos[3];			电机位置
short m_pMotorCurrent[3];	电机电流
bool m_pIO[4];				IO传感器
WORD m_pAD[4];				AD传感器
//向教练机发送的信息
Clear_Obst()				第一步：清除上一次发送的障碍物信息
net.AddObst()				第二步：逐个调用添加障碍物信息，最多添加10个

net.toS_V_Angle;			向教练机发送自己的移动速度方向（0~360）
net.toS_V;					向教练机发送自己的移动速度（0~255）

//教练机发来的信息
net.m_tm[4];				队友信息
net.m_obst[10]              障碍物信息
net.AttentionKick()			提示教练机球已经传出去，参数：1、自己坐标x 2、自己坐标y 3、落点坐标x 4、落点坐标y
*/
/************************************************************************/
#define int16_t int
#define uint8_t unsigned char
struct Quaternion_
{
    float w, x, y, z;
};
struct VectorFloat
{
    float x, y, z;
};
struct VectorInt16
{
    int x, y, z;
};
uint8_t dmpGetQuaternion(int16_t *data, const uint8_t *packet)
{
    // TODO: accommodate different arrangements of sent data (ONLY default supported now)
    //    if (packet == 0) packet = dmpPacketBuffer;
    data[0] = ((packet[0] << 8) + packet[1]);
    if (data[0] > 32768)
        data[0] = -65536 + data[0];

    data[1] = ((packet[4] << 8) + packet[5]);
    if (data[1] > 32768)
        data[1] = -65536 + data[1];

    data[2] = ((packet[8] << 8) + packet[9]);
    if (data[2] > 32768)
        data[2] = -65536 + data[2];

    data[3] = ((packet[12] << 8) + packet[13]);
    if (data[3] > 32768)
        data[3] = -65536 + data[3];
    return 0;
}
uint8_t dmpGetQuaternion_(Quaternion_ *q, const uint8_t *packet)
{
    // TODO: accommodate different arrangements of sent data (ONLY default supported now)
    int16_t qI[4];
    uint8_t status = dmpGetQuaternion(qI, packet);
    if (status == 0)
    {
        q->w = (float)qI[0] / 16384.0f;
        q->x = (float)qI[1] / 16384.0f;
        q->y = (float)qI[2] / 16384.0f;
        q->z = (float)qI[3] / 16384.0f;
        return 0;
    }
    return status; // int16 return value, indicates error if this line is reached
}
uint8_t dmpGetGravity(VectorFloat *v, Quaternion_ *q)
{
    v->x = 2 * (q->x * q->z - q->w * q->y);
    v->y = 2 * (q->w * q->x + q->y * q->z);
    v->z = q->w * q->w - q->x * q->x - q->y * q->y + q->z * q->z;
    return 0;
}

uint8_t dmpGetAccel(int16_t *data, const uint8_t *packet)
{ // (int16_t *data, const unsigned char* packet) {
    // TODO: accommodate different arrangements of sent data (ONLY default supported now)
    //    if (packet == 0) packet = dmpPacketBuffer;

    data[0] = ((packet[28] << 8) + packet[29]); // x
    if (data[0] > 32768)
        data[0] = -65536 + data[0];
    data[1] = ((packet[32] << 8) + packet[33]); // y
    if (data[1] > 32768)
        data[1] = -65536 + data[1];
    data[2] = ((packet[36] << 8) + packet[37]); // z
    if (data[2] > 32768)
        data[2] = -65536 + data[2];
    return 0;
}

uint8_t dmpGetAccel_(VectorInt16 *v, const uint8_t *packet)
{
    // TODO: accommodate different arrangements of sent data (ONLY default supported now)
    //    if (packet == 0) packet = dmpPacketBuffer;
    int16_t AAdata[3];
    dmpGetAccel(AAdata, packet);
    v->x = AAdata[0];
    v->y = AAdata[1];
    v->z = AAdata[2];
    return 0;
}
uint8_t dmpGetYawPitchRoll(float *data, Quaternion_ *q, VectorFloat *gravity)
{
    // yaw: (about Z axis)
    data[0] = atan2(2 * q->x * q->y - 2 * q->w * q->z, 2 * q->w * q->w + 2 * q->x * q->x - 1);
    // pitch: (nose up/down, about Y axis)
    data[1] = atan(gravity->x / sqrt(gravity->y * gravity->y + gravity->z * gravity->z));
    // roll: (tilt left/right, about X axis)
    data[2] = atan(gravity->y / sqrt(gravity->x * gravity->x + gravity->z * gravity->z));
    return 0;
}
// uint8_t MPU6050::dmpGetLinearAccel(long *data, const uint8_t* packet);
uint8_t dmpGetLinearAccel(VectorInt16 *v, VectorInt16 *vRaw, VectorFloat *gravity)
{
    // get rid of the gravity component (+1g = +4096 in standard DMP FIFO packet)
    v->x = vRaw->x - gravity->x * 4096 * 2;
    v->y = vRaw->y - gravity->y * 4096 * 2;
    v->z = vRaw->z - gravity->z * 4096 * 2;
    return 0;
}
Quaternion_ q_;         // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
float ypr_p[3];         // 上周期3角度[yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
int GYRO_available = 0; // 通过比对上一周期和本周期陀螺测量数据是否变化 判断陀螺数据是否可用

VectorInt16 aa;                  // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;              // [x, y, z]            gravity-free accel sensor measurements
float GYRO_Yangle_h[800];        // 陀螺历史 如果长时间 有误差 就使用视觉的所得角度逐渐校正陀螺
float Pipei_angle_h[800];        // 视觉定位历史
float GYRO_angleD_h[800];        // 陀螺与视觉定位差历史 用于减少陀螺零飘
float kalman_GYRO_angleD_h[800]; // kalman滤波后的 陀螺与视觉定位差历史 用于减少陀螺零飘
float GYRO_angleDV_h[800];       // 陀螺与视觉定位差速度历史 用于修正陀螺零飘
int GYRO_init_time_loop = 1900;  // 400;
float GYRO_init_angle = 0;
float GYRO_zero_V_base = 0; // 零飘 修正基准

float GYRO_angleD_p = 0;     // 解决kalman 输入值不能 接受+-180跳变问题
double GYRO_angleD_base = 0; // 解决kalman 输入值不能 接受+-180跳变问题

float Pipei_angle_p = 0; // 将视觉测量角 变成累计转动角
double Pipei_angle_base = 0;
float Pipei_angle = 0;                          // 输入累计转动角
double Pipei_angle_accumulative = 0;            // 输出累计转动角
double Pipei_angle_accumulative_init_angle = 0; // 累计转动角 起始角

float GYRO_Yangle_p = 0; // 将陀螺测量角 变成累计转动角
double GYRO_Yangle_base = 0;
float GYRO_Yangle = 0;                          // 输入累计转动角
double GYRO_Yangle_accumulative = 0;            // 输出累计转动角
double GYRO_Yangle_accumulative_init_angle = 0; // 累计转动角 起始角

double angle_proportion = 1;
double angle_proportion_USE = 1;

float xingwei_1117_rotate_direct_test = 1; // 测试陀螺旋转动作方向
float xingwei_30_motor_test_timer = 0;
float GYRO_zero_V_test = 0; // 测试变量 测试零飘
float jifenCS = 0;          // 进攻测试用变量
float track_black_OBJ_map_X[100];
float track_black_OBJ_map_Y[100];
float track_black_OBJ_map_L = 0;
float set_track_OBJ_X = 0;
float set_track_OBJ_Y = 0;
float move_Emu_ballX_p = 0;
float move_Emu_ballY_p = 0;
float move_Emu_ballX_h[100] = {0};
float move_Emu_ballY_h[100] = {0};
float RMfv_h[100] = {0};
float RMfa_h[100] = {0};
float RMw_h[100] = {0};
int xingwei30panding_jishu = 0; // 自动切 行为30 的保持计数器

float DebugV[100] = {0}; //
float test_joy_max_zhuan = 0;
float test_joy_max_jifen = 0;
float catch_dif = 0;
float catch_zoom = 0;
float catch_3 = 0;

long shoottime = 0;
int shoot_cool_time = 0;

int m_pMotorPos_V[3] = {0, 0, 0}; // 显示码盘数据用变量
int motospeedtemp_H[100][3];      // 控制电机输出速度值记录

double robot__ball_radius_min = 1000000; // 接球终止条件中记录 球距离机体距离

double y_quanbx_P[190];
double y_quanby_P[190];

// 是否有球传向本机检测 接球开始检测
int catch_start_detection(float angle_err, float min_speed, int all_measure_time, int step_long, float robot_f_x,
                          float robot_f_y, float *ballX_h, float *ballY_h)
{
    int i = 0;
    float angle_0 = 0;
    float angle_add1 = 0;
    //	float angle_add2=0;
    //	float angle_add3=0;
    int xingwei30panding = 0;
    //	int   step_long=15;				//单次测量间隔桢数
    float step_time = step_long / 90.0; // 单次测量间隔时间
    //	int   all_measure_time=10;		//全部3次测量间隔桢数
    //	float angle_err=20;				//监测条件 球速度的角度 与 球朝向机器角度 的 偏差角
    //	float min_speed=10;//30;		//检测条件 球移动速度 cm/s
    i = all_measure_time * 0;
    if (f2l_radius(ballX_h[i], ballY_h[i], 0, ballX_h[i + step_long], ballY_h[i + step_long]) > min_speed * step_time)
    {
        angle_0 = f2l_angle(robot_f_x, robot_f_y, 0, ballX_h[i + step_long], ballY_h[i + step_long]);
        angle_add1 = f2l_angle(ballX_h[i], ballY_h[i], 0, ballX_h[i + step_long], ballY_h[i + step_long]);
        if (fabs(angle_add_transfer2(angle_0, -angle_add1)) < angle_err)
        {
            i = all_measure_time * 0.5; // step_long/3;
            if (f2l_radius(ballX_h[i], ballY_h[i], 0, ballX_h[i + step_long], ballY_h[i + step_long]) >
                min_speed * step_time)
            {
                angle_0 = f2l_angle(robot_f_x, robot_f_y, 0, ballX_h[i + step_long], ballY_h[i + step_long]);
                angle_add1 = f2l_angle(ballX_h[i], ballY_h[i], 0, ballX_h[i + step_long], ballY_h[i + step_long]);
                if (fabs(angle_add_transfer2(angle_0, -angle_add1)) < angle_err)
                {
                    i = all_measure_time * 1.0; // step_long*2/3;
                    if (f2l_radius(ballX_h[i], ballY_h[i], 0, ballX_h[i + step_long], ballY_h[i + step_long]) >
                        min_speed * step_time)
                    {
                        angle_0 = f2l_angle(robot_f_x, robot_f_y, 0, ballX_h[i + step_long], ballY_h[i + step_long]);
                        angle_add1 =
                            f2l_angle(ballX_h[i], ballY_h[i], 0, ballX_h[i + step_long], ballY_h[i + step_long]);
                        if (fabs(angle_add_transfer2(angle_0, -angle_add1)) < angle_err)
                        {
                            xingwei30panding = 1;
                        }
                    }
                }
            }
        }
    }
    return (xingwei30panding);
}
Emu_Robot robot_catch_ball;         // 用来做接球定位用 仿真机体计算
obj_properties ball_for_catch_ball; // 用来做接球定位用 球定位计算

/************************************************************************/
/* 视觉数据入口
【1】运动控制	pRobot->SetFiveMotorsSpeed
【2】射门杆提升 pRobot->SetLiftPos（设置完提升高度后，需要调用一次pRobot->SetFiveMotorsSpeed才能把调节值发下去）
【3】射门力度	pRobot->Kick
【4】守门网抬升	pRobot->GKSave(0)                                                                 */
/************************************************************************/
void CGetImage::AfterUpdateOverlook(BYTE *pBuffer, long lWidth, long lHeight, double dbTime, UINT nState)
{
    //	m_zubo.m_FaSongShuJu.nJieQiu_x=1;
    //	m_zubo.m_FaSongShuJu.nJieQiu_y=2;
    //	m_zubo.PostData();
    pRobot->SetLiftPos(9000);
//
// 	net.toS_other[0]=(int)m_pAD[1];
// 	net.toS_other[1]=(int)m_pAD[1];
//	Sleep(30);
//	Sleep(30);
//	return;
//	if(kongzhitai=="xx")
//	{
//		AfxMessageBox("123");
//	}
#ifndef RUN_WITH_EMU // 如果不使用仿真环境就 对GYRO加开机启动延时
#ifdef USE_GYRO_MPU6050
    if (GYRO_init_time_loop > 0)
    {
        GYRO_init_time_loop--;
        //	if(GYRO_init_time_loop==1)
        GYRO_init_angle = angle_add_transfer2(ypr[0] * 180.0 / 3.14, -y.benjijiao); // ypr[0]*180.0/3.14;
        //	return;

        //	if(dianjipanding>0)
        //	{	//在使用鼠标标定 机体方向
        //			GYRO_init_angle=angle_add_transfer(ypr[0]*180.0/3.14,-y.benjijiao);
        //	}
        //	if(dianjipanding>0)
        //	{	//在使用鼠标标定 机体方向
        //			GYRO_Yangle_base=-GYRO_Yangle;
        //			GYRO_Yangle_accumulative_init_angle=GYRO_Yangle;
        //			Pipei_angle_base=-Pipei_angle;
        //			Pipei_angle_accumulative_init_angle=Pipei_angle;
        //	}
    }
#endif
#endif

    cdbl = MK;
    //********************
    LARGE_INTEGER Frequency, CountEnd, CountStart;
    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&CountStart);
    double dfElapseMS = 0;
    double dfElapseMS1 = 0;
    double dfElapseMS2 = 0;
    double dfElapseMS3 = 0;
    double dfElapseMS4 = 0;
    double dfElapseMS5 = 0;
    double dfElapseMS6 = 0;
    double dfElapseMSend = 0;

    QueryPerformanceCounter(&Loop_Time_Count_End);
    LOOPTIME = (double)((double)(Loop_Time_Count_End.QuadPart - Loop_Time_Count_Start.QuadPart + 10) /
                        (double)Frequency.QuadPart) *
               1000.0;
    Dribble_ball.Gb_loop_time = LOOPTIME;
    QueryPerformanceCounter(&Loop_Time_Count_Start);
    //********************测试时间的起始点
    //****************************************************************************************************
    // QueryPerformanceCounter(&CountEnd);
    // dfElapseMS=(double)((double)(CountEnd.QuadPart-CountStart.QuadPart+10)/(double)Frequency.QuadPart)*1000.0;
    //****************************************************************************************************第一个时间节点

    /*/////////////////////////////////////////////////////////////////////////
    //此部分已经被移到类声明中去了
    int zhuan;  //转动量
    int mtl;    //左电机转速
    int mtr;	//右电机转速
    int mtb;	//后电机转速
    int kick;	//射门
    int zhangaijiao; //有障碍物时门的角度
    int zhangaiyuan; //有障碍物时门的角度
    int zhangaiyuanh;
    double dingweijiao;
    double dingweichang;
    CString zhuangtai;//机器人的策略状态
    CString strShow;  //输出运动过程中的调试参数
    //HSIParam  *m_HSI= new HSIParam; //生成HSI结构体
    RGBTRIPLE ball;

    /////////////////////////////////////////////////////////////////////////*/
    long double pipeijuli;
    //	hecheng(pBuffer); //将前景前景图像与全景图像进行合成
    m_find.ImageBuf(lWidth, lHeight, pBuffer); // 将图像的宽度高度指针 给m_find类(UPRColor)
    if (bianliang1 == 1)                       // 类初始化的时候bianliang1=1当 第一次执行此函数时需要执行以下东西：:
    {

        z_k_X = cvCreateMat(1, 1, CV_32FC1);
        z_k_Y = cvCreateMat(1, 1, CV_32FC1);
        kalman_X = cvCreateKalman(2, 1, 0);
        memcpy(kalman_X->transition_matrix->data.fl, F, sizeof(F));
        /*初始化转移矩阵，行列，具体见CvKalman* kalman = cvCreateKalman( 2, 1, 0 );*/ // Initialize other Kalman
                                                                                      // parameters
        cvSetIdentity(kalman_X->measurement_matrix, cvRealScalar(1));                 /*观测矩阵*/
        cvSetIdentity(kalman_X->process_noise_cov, cvRealScalar(1e-5));               /*过程噪声*/
        cvSetIdentity(kalman_X->measurement_noise_cov, cvRealScalar(1e-3));           /*观测噪声*/
        cvSetIdentity(kalman_X->error_cov_post, cvRealScalar(1)); /*后验误差协方差*/  // Choose random initial state
        kalman_Y = cvCreateKalman(2, 1, 0);
        memcpy(kalman_Y->transition_matrix->data.fl, F, sizeof(F));
        /*初始化转移矩阵，行列，具体见CvKalman* kalman = cvCreateKalman( 2, 1, 0 );*/ // Initialize other Kalman
                                                                                      // parameters
        cvSetIdentity(kalman_Y->measurement_matrix, cvRealScalar(1));                 /*观测矩阵*/
        cvSetIdentity(kalman_Y->process_noise_cov, cvRealScalar(1e-5));               /*过程噪声*/
        cvSetIdentity(kalman_Y->measurement_noise_cov, cvRealScalar(1e-3));           /*观测噪声*/
        cvSetIdentity(kalman_Y->error_cov_post, cvRealScalar(1)); /*后验误差协方差*/  // Choose random initial state

#ifdef RUN_WITH_EMU

        //		cvDestroyWindow("src_Pp");//如果使用仿真环境就 将路径规划显示窗口关闭

        SOCKADDR_IN localIP;
        targAdrlen = sizeof(SOCKADDR);

        localIP.sin_family = AF_INET;
        localIP.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //("192.168.11.1");	///本机

#ifndef RUN_WITH_EMU_LINK_MSLCoach2                         // 设置使用本机上的原始教练机
        localIP.sin_port = htons(13100 + m_nAgentID_g_set); // htons();             ///监听端口
#endif
#ifdef RUN_WITH_EMU_LINK_MSLCoach2                          // 设置使用本机上的2号教练机 用于测试双队对抗
        localIP.sin_port = htons(13120 + m_nAgentID_g_set); // htons();             ///监听端口
#endif
        m_receive_EMU_server_skt = socket(AF_INET, SOCK_DGRAM, 0);
        if (bind(m_receive_EMU_server_skt, (SOCKADDR *)&localIP, sizeof(SOCKADDR)) != 0)
        {
            printf("bind err!");
            AfxMessageBox("监听端口打不开！");
            bianliang1 = 0;
            return;
        }

        AfxBeginThread(UDPReceiveThread, this);

#ifndef RUN_WITH_EMU_LINK_MSLCoach2                         // 设置使用本机上的原始教练机
        localIP.sin_port = htons(13200 + m_nAgentID_g_set); // htons();             ///发送用标示源端口
#endif
#ifdef RUN_WITH_EMU_LINK_MSLCoach2                          // 设置使用本机上的2号教练机 用于测试双队对抗
        localIP.sin_port = htons(13220 + m_nAgentID_g_set); // htons();             ///发送用标示源端口
#endif
        m_send_EMU_server_skt = socket(AF_INET, SOCK_DGRAM, 0);
        if (bind(m_send_EMU_server_skt, (SOCKADDR *)&localIP, sizeof(SOCKADDR)) != 0)
        {
            printf("bind err!");
            AfxMessageBox("发送端口打不开！");
            bianliang1 = 0;
            return;
        }

        // 发送本机运动参数结构体初始化
        sendToServerData.serverReceiveEMU_motospeed[0] = 0; //
        sendToServerData.serverReceiveEMU_motospeed[1] = 0; //
        sendToServerData.serverReceiveEMU_motospeed[2] = 0; //
        sendToServerData.serverReceiveEMU_motospeed[3] = 0; //
        sendToServerData.serverReceiveEMU_motospeed[4] = 0; //
        sendToServerData.kickTime = 0;
        sendToServerData.holdball = 0; // 构造函数初始化
        sendToServerData_kickTimelate = 0;

        // 接收环境数据结构体初始化
        for (i = 0; i < 6; i++)
        {
            ReceiveServerData.robotLocal_x[i] = 0;
            ReceiveServerData.robotLocal_y[i] = 0;
            ReceiveServerData.robotLocal_a[i] = 0;
        }
        ReceiveServerData.ballLocal_x = 0;
        ReceiveServerData.ballLocal_y = 0;
#endif

        //	CString str;
        //	str.Format("%d,%d",m_hsiset.blue.Hue_max,m_hsiset.overball.Hue_min);
        //	AfxMessageBox(str);
        write.starwrite(); // 开始读取文件
        y.bhs = m_hsiset.overball.Hue_max;
        y.bhx = m_hsiset.overball.Hue_min;
        y.bs = m_hsiset.overball.Satu_min;
        y.bis = m_hsiset.overball.I_max;
        y.bix = m_hsiset.overball.I_min;
        // 球
        y.chs = m_hsiset.green.Hue_max;
        y.chx = m_hsiset.green.Hue_min;
        y.cs = m_hsiset.green.Satu_min;
        y.cis = m_hsiset.green.I_max;
        y.cix = m_hsiset.green.I_min;
        // y.chs=360;
        //	y.chx=0;
        //	y.cs=0;
        //	y.cis=255;
        //	y.cix=0;
        // 场地
        y.sh = m_hsiset.black.Satu_max;
        y.ih = m_hsiset.black.I_max;
        // 黑色
        y.sw = m_hsiset.whitelines.Satu_max;
        y.iw = m_hsiset.whitelines.I_min;
        // 白色
        // 		if(y.val_qiehuan!=1)
        // 		{
        // 			y.dhs=m_hsiset.blue.Hue_max;
        // 			y.dhx=m_hsiset.blue.Hue_min;
        // 			y.ds=m_hsiset.blue.Satu_min;
        // 			y.dis=m_hsiset.blue.I_max;
        // 			y.dix=m_hsiset.blue.I_min;
        //
        // 			y.jhs=m_hsiset.yellow.Hue_max;
        // 			y.jhx=m_hsiset.yellow.Hue_min;
        // 			y.js=m_hsiset.yellow.Satu_min;
        // 			y.jis=m_hsiset.yellow.I_max;
        // 			y.jix=m_hsiset.yellow.I_min;
        // 			//在进攻不同门的条件下，需要做不同的门颜色赋值
        // 		}
        // 		else
        // 		{
        // 			y.jhs=m_hsiset.blue.Hue_max;
        // 			y.jhx=m_hsiset.blue.Hue_min;
        // 			y.js=m_hsiset.blue.Satu_min;
        // 			y.jis=m_hsiset.blue.I_max;
        // 			y.jix=m_hsiset.blue.I_min;
        //
        // 			y.dhs=m_hsiset.yellow.Hue_max;
        // 			y.dhx=m_hsiset.yellow.Hue_min;
        // 			y.ds=m_hsiset.yellow.Satu_min;
        // 			y.dis=m_hsiset.yellow.I_max;
        // 			y.dix=m_hsiset.yellow.I_min;
        // 		}

        CString zhi;
        lmoto = 0;
        rmoto = 0;
        bianliang1 = 0;
    }

    // 	switch(net.m_ctrl.targetGoal)
    // 	{
    // 	case 0:
    // 			y.dhs=m_hsiset.blue.Hue_max;
    // 			y.dhx=m_hsiset.blue.Hue_min;
    // 			y.ds=m_hsiset.blue.Satu_min;
    // 			y.dis=m_hsiset.blue.I_max;
    // 			y.dix=m_hsiset.blue.I_min;
    //
    // 			y.jhs=m_hsiset.yellow.Hue_max;
    // 			y.jhx=m_hsiset.yellow.Hue_min;
    // 			y.js=m_hsiset.yellow.Satu_min;
    // 			y.jis=m_hsiset.yellow.I_max;
    // 			y.jix=m_hsiset.yellow.I_min;
    // 		break;
    //
    // 	case 1:
    // 					y.jhs=m_hsiset.blue.Hue_max;
    // 			y.jhx=m_hsiset.blue.Hue_min;
    // 			y.js=m_hsiset.blue.Satu_min;
    // 			y.jis=m_hsiset.blue.I_max;
    // 			y.jix=m_hsiset.blue.I_min;
    //
    // 			y.dhs=m_hsiset.yellow.Hue_max;
    // 			y.dhx=m_hsiset.yellow.Hue_min;
    // 			y.ds=m_hsiset.yellow.Satu_min;
    // 			y.dis=m_hsiset.yellow.I_max;
    // 			y.dix=m_hsiset.yellow.I_min;
    // 		break;
    // 	}

    //	m_r2h.m_FixOverlookOnLogiDeluxe(pBuffer,lWidth,lHeight);
    // 进行扭曲校准,逻辑pro5000加强，在640*480的情况下会出现图像横向拉伸的问题
    mtl = 0;
    mtr = 0;
    mtb = 0;
    //	m_find.DelWL(3); //删除无效的白色场线（目前无效果）
    //	m_find.pengzhang(3); //膨胀（目前无效果）

    // m_find.Getblack(80,50,5);//采集黑色

    //	m_find.lianxuyu(4,1,&y);//连续 对地面的颜色进行连通域 并且对其做8个像素的十字膨胀
    //	m_find.lianxuyu(2,0,&y);//连续 对敌方门进行连同域检测
    //	m_find.lianxuyu(3,0,&y);//连续 对自己门进行连同域检测
    //	m_find.lianxuyu(1,1,&y);//连续 对球进行连同域检测,并且任意一个连通域必须在膨胀的地面的范围内

    m_find.display_process_image = 0; // 控制是否显示 图像处理中间过程图像
    float obj_weight_map;

    //****************************************************************************************************
    QueryPerformanceCounter(&CountEnd);
    dfElapseMS = (double)((double)(CountEnd.QuadPart - CountStart.QuadPart + 10) / (double)Frequency.QuadPart) * 1000.0;
    //****************************************************************************************************第一个时间节点
#ifndef RUN_WITH_EMU // 使用仿真时屏蔽所有图像处理代码	<<<<<<<<<<++++
    // Sleep(100);
    if (0) // xiandianshu<=1)
    {
        m_find.GetHSIFix2(&y);                                                     // 二值化
        m_find.xianshiweitu();                                                     // 显示场线匹配模板地图
        m_find.bool_max_connectivity_analyze2_1green(&obj_weight_map);             // 绿色连续域
        m_find.bool_max_connectivity_analyze2_1_InGreen_ball(&obj_weight_map, &y); // 球连续域
        m_find.getchangxian();                                                     // 获取场线
        pipeijuli = m_find.pipei(&y);                                              // 场线定位
        if (dianjipanding > 0)
        {
            dianjipanding--;
            y.benjix = dianjix;
            y.benjiy = dianjiy;
            y.benjijiao = dianjijiao;
        }
        //	pipeijuli=m_find.pipei(&y);//场线定位
    }
    else
    {
        // 2010
        //		m_find.GetHSIFix3(&y); //二值化
        //		m_find.getchangxian2(); //获取场线
        // 2011
        m_find.GetHSIFix3_no_G_W(&y); // 二值化 不做绿 白
        m_find.getchangxian_3(&y);    // 二值化 同时 做 绿 白  获取场线

        m_find.xianshiweitu(); // 显示场线匹配模板地图

        //		pipeijuli=m_find.pipei(&y);//场线定位

#ifndef MOUSE_SET_BALL         // 用鼠标测试球时暂时注释掉
        if (dianjipanding > 0) // 使用鼠标 标定机体在场地位置
        {
            dianjipanding--;
            y.benjix = dianjix;
            y.benjiy = dianjiy;
            y.benjijiao = dianjijiao;
        }

        if (m_pIO[3] == 1) // 使用i/o 标定机体在场地位置;				IO传感器
        {
            io_cont++;
        }
        if (m_pIO[3] != 1) // 使用i/o 标定机体在场地位置;				IO传感器
        {
            io_cont = 0;
        }
        if (io_cont > 60)
        {
            io_cont = 110;
        }
        if (io_cont > 50)
        {
            //	dianjipanding--;	y.benjix=dianjix;	y.benjiy=dianjiy;	y.benjijiao=dianjijiao;
            y.benjix = net.rec_init_x;
            y.benjiy = net.rec_init_y;
            y.benjijiao = atan2(600 - net.rec_init_y, 1800 - net.rec_init_x) * 180.0 / 3.14; // net.rec_init_angle;
        }

#endif
        if (0) // 1使用无路程定位 0使用有路程定位
        {
            Dribble_ball.Dis_Location = 0; // 路程定位在定位中占的影响 数值0~2 浮点数 0为完全不使用路程定位
                                           // 2为完全使用路程定位  默认为0 比较合适的使用系数1.8
            pipeijuli = m_find.pipei(&y);  // 场线定位
        }
        else
        {

            dmpGetQuaternion_(&q_, m_pMPU6050Data); //(&q_, (unsigned char*)(&m_Com.m_pRcvData[0]));
            dmpGetGravity(&gravity, &q_);
            dmpGetYawPitchRoll(ypr, &q_, &gravity);

            dmpGetAccel_(&aa, m_pMPU6050Data);
            dmpGetLinearAccel(&aaReal, &aa,
                              &gravity); // aaReal_G[0]=aaReal.x;	aaReal_G[1]=aaReal.y;	aaReal_G[2]=aaReal.z;
            //	ypr[0]=179.99/180.0*3.14;//test

            ypr[0] = -ypr[0];
            ypr_p[1] = ypr[1];
            ypr_p[2] = ypr[2]; // 修正安装位置引起的坐标系变化
            //	aaReal.x=aaReal.x;	aaReal.y=aaReal.y;	aaReal.z=aaReal.z;

            GYRO_available = 1;
            if ((ypr_p[0] == ypr[0]) && (ypr_p[1] == ypr[1]) && (ypr_p[2] == ypr[2]))
            {
                GYRO_available = 0;
            }
            ypr_p[0] = ypr[0];
            ypr_p[1] = ypr[1];
            ypr_p[2] = ypr[2];

            // 计算 视觉和陀螺 角度差的 400历史记录平均值
            double GYRO_angleD_add = 0;
            for (int i = 0; i < 400; i++) // 存储上800周期定位历史记录
            {
                GYRO_angleD_add += GYRO_angleD_h[i];
            }
            GYRO_angleD_add = GYRO_angleD_add / 400.0;

            // 计算 视觉和陀螺 角度差变化速度 500历史记录平均值
            double GYRO_angleDV_h_add = 0;
            for (i = 0; i < 500; i++) // 存储上800周期定位历史记录
            {
                GYRO_angleDV_h_add += GYRO_angleDV_h[i];
            }
            GYRO_angleDV_h_add = GYRO_angleDV_h_add / 500.0;
            float GYRO_angle_zero_V = GYRO_angleDV_h_add;

            for (i = 800 - 1; i > 0; i--) // 存储上800周期定位历史记录
            {
                GYRO_Yangle_h[i] = GYRO_Yangle_h[i - 1];               // 陀螺定位角度 原始数据
                Pipei_angle_h[i] = Pipei_angle_h[i - 1];               // 视觉定位角度 原始数据
                GYRO_angleD_h[i] = GYRO_angleD_h[i - 1];               // 视觉和陀螺 角度差
                kalman_GYRO_angleD_h[i] = kalman_GYRO_angleD_h[i - 1]; // 视觉和陀螺 角度差 经kalman滤波
                GYRO_angleDV_h[i] = GYRO_angleDV_h[i - 1];             //
            }
            /*记录*/ GYRO_Yangle_h[0] = ypr[0] * 180.0 / 3.14;
            /*记录*/ Pipei_angle_h[0] = y.benjijiao;

#ifdef USE_GYRO_MPU6050
            if (GYRO_init_time_loop <= 0)
            { // 使用动态GYRO_init_angle修正陀螺示值

                if ((dianjipanding <= 0) && (io_cont <= 50) && (GYRO_available == 1))
                {
                    y.benjijiao = angle_add_transfer2(ypr[0] * 180.0 / 3.14, -GYRO_init_angle);
                }
            }
#endif

            double rectify_GYRO_Yangle_accumulative = GYRO_Yangle_accumulative * angle_proportion_USE;
            double temp_angle_GP =
                angle_add_transfer(GYRO_Yangle_accumulative_init_angle, -Pipei_angle_accumulative_init_angle);
            //			double
            // rectify_GYRO_Yangle=angle_add_transfer(rectify_GYRO_Yangle_accumulative-GYRO_Yangle_base-temp_angle_GP,0);

            int time360 = rectify_GYRO_Yangle_accumulative / 360;
            double rectify_GYRO_Yangle = angle_add_transfer(rectify_GYRO_Yangle_accumulative - 360.0 * time360 +
                                                                GYRO_Yangle_accumulative_init_angle - temp_angle_GP,
                                                            0);
            // Debug_D[16].Draw_vp_( GYRO_Yangle_accumulative,360        ,1000        ,1          ,255,0,255, pBuffer);
            // Debug_D[17].Draw_vp_( Pipei_angle_accumulative,360        ,1000        ,1          ,0,255,0, pBuffer);
            Debug_D[18].Draw_vp_(Pipei_angle_accumulative - rectify_GYRO_Yangle_accumulative, 360, 10, 1, 0, 255, 0,
                                 pBuffer);
            // Debug_D[19].Draw_vp_( angle_proportion-1,360        ,1        ,1          ,0,255,0, pBuffer);

#ifdef USE_GYRO_MPU6050
            // 使用修正陀螺输出速度误差 修正陀螺示值
            if ((dianjipanding <= 0) && (io_cont <= 50) && (GYRO_available == 1))
            {
                //		y.benjijiao=rectify_GYRO_Yangle;//
            }
#endif

            m_find.pipei2Display = 1;
            Dribble_ball.Dis_Location = 0;   // 1.8;//路程定位在定位中占的影响 数值0~2 浮点数 0为完全不使用路程定位
                                             // 2为完全使用路程定位  默认为0 比较合适的使用系数1.8
            pipeijuli = m_find.pipei2_5(&y); // 场线定位

            y.benjijiao = angle_add_transfer2(y.benjijiao, 0);

            float GYRO_angleT = angle_add_transfer2(ypr[0] * 180.0 / 3.14, -GYRO_init_angle);
            float GYRO_angleD = angle_add_transfer2(-y.benjijiao, GYRO_angleT);
            ///*测试代码*/GYRO_zero_V_test = angle_add_transfer(GYRO_zero_V_test,0.03);//-0.1);//1.9);
            ///*测试代码*/GYRO_angleD      =
            /// angle_add_transfer((rand()/16383.5-1.0)*2,GYRO_zero_V_test);//人工插入测试加噪数据
            /// *测试代码*/GYRO_angleD
            ///=
            /// cos(GYRO_zero_V_test)*2+(rand()/16383.5-1.0)*2;//angle_add_transfer((rand()/16383.5-1.0)*2,GYRO_zero_V_test);//人工插入测试加噪数据

            /*记录*/ GYRO_angleD_h[0] = GYRO_angleD;

            GYRO_zero_V_base = GYRO_zero_V_base - GYRO_angle_zero_V / 10.0;

            if (1) // 使用卡尔曼滤波器 对球的位置滤波
            {
                if (kalman_GYRO_angleD_init_flag == 1) // 接球卡尔曼滤波器初始化
                {
                    kalman_GYRO_angleD_init_flag = 0;
                    //	cvReleaseMat(&z_k_GYRO_angleD);
                    z_k_GYRO_angleD = cvCreateMat(1, 1, CV_32FC1);
                    //	cvReleaseKalman(&kalman_GYRO_angleD);
                    kalman_GYRO_angleD = cvCreateKalman(2, 1, 0);
                    memcpy(kalman_GYRO_angleD->transition_matrix->data.fl, F, sizeof(F));
                    /*初始化转移矩阵，行列，具体见CvKalman* kalman = cvCreateKalman( 2, 1, 0 );*/ // Initialize
                                                                                                  // other Kalman
                                                                                                  // parameters
                    cvSetIdentity(kalman_GYRO_angleD->measurement_matrix, cvRealScalar(1));       /*观测矩阵*/
                    cvSetIdentity(kalman_GYRO_angleD->process_noise_cov, cvRealScalar(1e-5));     /*过程噪声*/
                    cvSetIdentity(kalman_GYRO_angleD->measurement_noise_cov,
                                  cvRealScalar(1e-1)); //(1e-3) ); /*观测噪声*/
                    cvSetIdentity(kalman_GYRO_angleD->error_cov_post, cvRealScalar(1));
                    /*后验误差协方差*/ // Choose random initial state

                    kalman_GYRO_angleD->state_post->data.fl[0] = 0; // move_Emu_ballX;
                    GYRO_angleD_p = GYRO_angleD;
                }
                else
                {
                    double GYRO_angleD_mensure_noise = 0.0005; // 60;//ballchang_temp*0.0001+0.001;
                    cvSetIdentity(kalman_GYRO_angleD->measurement_noise_cov,
                                  cvRealScalar(GYRO_angleD_mensure_noise)); //(1e-3) ); /*观测噪声*/

                    if (GYRO_angleD_p > GYRO_angleD + 180) // 解决kalman 输入值不能 接受+-180跳变问题
                        GYRO_angleD_base += 360;
                    if (GYRO_angleD_p < GYRO_angleD - 180) // 解决kalman 输入值不能 接受+-180跳变问题
                        GYRO_angleD_base -= 360;
                    GYRO_angleD_p = GYRO_angleD;

                    z_k_GYRO_angleD->data.fl[0] = GYRO_angleD_p + GYRO_angleD_base; // GYRO_angleD;
                    const CvMat *y_k_GYRO_angleD = cvKalmanPredict(kalman_GYRO_angleD, 0);
                    /*激励项输入为*/ // Generate Measurement (z_k)
                    cvKalmanCorrect(kalman_GYRO_angleD,
                                    z_k_GYRO_angleD); // Apply the transition matrix F and apply"process noise" w_k
                    double Kalman_GYRO_angleD_data = y_k_GYRO_angleD->data.fl[0];

                    /*记录*/ kalman_GYRO_angleD_h[0] = Kalman_GYRO_angleD_data;
                    /*记录*/ GYRO_angleDV_h[0] = kalman_GYRO_angleD_h[0] - kalman_GYRO_angleD_h[10];

                    //				Kalman_GYRO_angleD_data=Kalman_GYRO_angleD_data-GYRO_angleD_base;

                    /*显示*/ //	Debug_D[9].Draw_vp_( GYRO_angleD,360        ,10        ,1          ,0,255,0, pBuffer);
                    ///*显示*/		Debug_D[11].Draw_vp_( GYRO_angleDV_h[0],360        ,10        ,1          ,0,255,0,
                    /// pBuffer); *显示*/		Debug_D[12].Draw_vp_( GYRO_angleDV_h_add,360        ,10        ,1
                    ///,0,255,0, pBuffer);
                    /*显示*/ //	Debug_D[10].Draw_vp_( 0         ,360        ,10        ,1          ,0,255,0, pBuffer);
                             // 微分再微分测零飘
                    if (1)   // 1 选择使用 卡尔曼滤波  0选择使用均值滤波  作为修正陀螺零偏的部分
                    {
                        /*显示*/ Debug_D[10].Draw_vp_(Kalman_GYRO_angleD_data, 360, 100, 1, 255, 0, 0, pBuffer);
                        GYRO_angleD = Kalman_GYRO_angleD_data;
                    }
                    else
                    {
                        ///*显示*/			Debug_D[10].Draw_vp_( GYRO_angleD_add         ,360        ,10        ,1
                        ///,0,0,0, pBuffer);
                        GYRO_angleD = GYRO_angleD_add;
                    }
                }
            }

            if (0)
            { // 测试 精简kalman
                if (kalman_test_init_flag == 1)
                {
                    kalman_test_init_flag = 0;
                    kalman_init(kalman_test, kalman_test_z_k_Mat, 1, 0);
                }
                else
                {
                    Debug_D[9].Draw_vp_(GYRO_angleD, 400, 10, 1, 0, 255, 0, pBuffer);
                    Debug_D[10].Draw_vp_(kalman_Correct(kalman_test, kalman_test_z_k_Mat, GYRO_angleD), 400, 10, 1, 255,
                                         255, 0, pBuffer);

                    kalman_L_init(&Kalman_lite_test, 100, 0);
                    Debug_D[11].Draw_vp_(kalman_L_Correct(&Kalman_lite_test, GYRO_angleD), 400, 10, 1, 255, 255, 0,
                                         pBuffer);
                    // kalman_L_delete	 (Kalman_lite_test);
                }
            }
            //			Debug_D[14].Draw_vp_( GYRO_angleD,360        ,10        ,1          ,0,0,255, pBuffer);
            ///*显示*/	Debug_D[15].Draw_vp_( GYRO_zero_V_base,360        ,1000        ,1          ,255,255,255,
            /// pBuffer);
            //			GYRO_angleD=GYRO_angleD+GYRO_zero_V_base;//angle_add_transfer(GYRO_angleD,GYRO_zero_V_base);//
            ///*显示*/	Debug_D[13].Draw_vp_( GYRO_angleD,360        ,1000        ,1          ,0,0,0, pBuffer);

            if (GYRO_angleD > 0.05)
                GYRO_angleD = 0.05;
            if (GYRO_angleD < -0.05)
                GYRO_angleD = -0.05;

            if ((dianjipanding > 0) || (io_cont > 50))
            { // 在使用鼠标标定 机体方向
                GYRO_init_angle = angle_add_transfer2(ypr[0] * 180.0 / 3.14, -y.benjijiao);
                GYRO_angleD_base = 0;
                GYRO_angleD_p = angle_add_transfer2(-y.benjijiao, ypr[0] * 180.0 / 3.14 - GYRO_init_angle);
            }
            else
            // if(dianjipanding<=0)
            {
                GYRO_init_angle = angle_add_transfer2(GYRO_init_angle, GYRO_angleD);
            }

            // 校正 角度测量灵敏度 即: 实际转x度 陀螺显示y度
            // rectify_GYRO_Yangle_accumulative=GYRO_Yangle_accumulative*angle_proportion_USE
            // angle_proportion =
            // Pipei_angle_accumulative / GYRO_Yangle_accumulative;
            GYRO_Yangle = ypr[0] * 180.0 / 3.14;
            Pipei_angle = y.benjijiao;
            if ((dianjipanding > 0) || (io_cont > 50))
            { // 在使用鼠标标定 机体方向
                GYRO_Yangle_base = -GYRO_Yangle;
                GYRO_Yangle_accumulative_init_angle = GYRO_Yangle;
                Pipei_angle_base = -Pipei_angle;
                Pipei_angle_accumulative_init_angle = Pipei_angle;
            }

            if (GYRO_Yangle_p > GYRO_Yangle + 180)
                GYRO_Yangle_base += 360;
            if (GYRO_Yangle_p < GYRO_Yangle - 180)
                GYRO_Yangle_base -= 360;
            GYRO_Yangle_p = GYRO_Yangle;
            GYRO_Yangle_accumulative = GYRO_Yangle + GYRO_Yangle_base;
            // 如果需要速度可以使用GYRO_Yangle_accumulative -GYRO_Yangle_accumulative_P
            // 或者angle_add_transfer(GYRO_Yangle,-GYRO_Yangle_p);

            // GYRO_Yangle_accumulative+=angle_add_transfer(GYRO_Yangle,-GYRO_Yangle_p);
            // GYRO_Yangle_p=GYRO_Yangle;

            if (Pipei_angle_p > Pipei_angle + 180)
                Pipei_angle_base += 360;
            if (Pipei_angle_p < Pipei_angle - 180)
                Pipei_angle_base -= 360;
            Pipei_angle_p = Pipei_angle;
            Pipei_angle_accumulative = Pipei_angle + Pipei_angle_base;

            if (((Pipei_angle_accumulative > 720) && (GYRO_Yangle_accumulative > 720)) ||
                ((Pipei_angle_accumulative < -720) && (GYRO_Yangle_accumulative < -720)))
            { // 累计转角达到720度开始校正
                angle_proportion = Pipei_angle_accumulative / GYRO_Yangle_accumulative;
            }
            double angle_proportion_D = angle_proportion - angle_proportion_USE;
            double angle_proportion_acc = 0.00001;
            if (angle_proportion_D > angle_proportion_acc)
                angle_proportion_D = angle_proportion_acc;
            if (angle_proportion_D < -angle_proportion_acc)
                angle_proportion_D = -angle_proportion_acc;
            angle_proportion_USE = angle_proportion_USE + angle_proportion_D;
        }
        if (0) //(kaijipd>400)
        {
            float julicha = 400 / 45;
            if (y.benjix - benjix_s < -julicha)
                y.benjix = benjix_s - julicha;
            if (y.benjix - benjix_s > julicha)
                y.benjix = benjix_s + julicha;
            if (y.benjiy - benjiy_s < -julicha)
                y.benjiy = benjiy_s - julicha;
            if (y.benjiy - benjiy_s > julicha)
                y.benjiy = benjiy_s + julicha;
            float jiaoducha = 360 / 45;
            if (angle_add_transfer(y.benjijiao, -benjijiao_s) > jiaoducha)
                y.benjijiao = angle_add_transfer(benjijiao_s, jiaoducha);
            if (angle_add_transfer(y.benjijiao, -benjijiao_s) < -jiaoducha)
                y.benjijiao = angle_add_transfer(benjijiao_s, -jiaoducha);
        }

        //		if(y.benjijiao-benjijiao_s<-10 )
        //			y.benjijiao=benjijiao_s-10;
        //		if(y.benjijiao-benjijiao_s>10)
        //			y.benjijiao=benjijiao_s+10;

        if (y.benjix > (1800 + 200))
        {
            y.benjix = (1800 + 200);
        }
        if (y.benjix < (-200))
        {
            y.benjix = (-200);
        }

        if (y.benjiy > (1200 + 200))
        {
            y.benjiy = (1200 + 200);
        }
        if (y.benjiy < (-200))
        {
            y.benjiy = (-200);
        }
        // y.benjijiao=-181-360;
        if ((y.benjijiao > 180) || (y.benjijiao < -180)) // 角度取整
        {
            int temp_benjijiao = y.benjijiao / 360;
            y.benjijiao = y.benjijiao - temp_benjijiao * 360;
            angle_add_transfer(y.benjijiao, 0);
        }

        float obj_weight_map__; //[101];
        float outx, outy, min_r, max_r;

        // 设置足球查找范围参数
        m_find.film_w = 1880; // 设置右边线坐标
        m_find.film_h = 1280; // 设置上边线坐标
        m_find.film_w_ = -80; // 设置左边线坐标
        m_find.film_h_ = -80; // 设置下边线坐标
        m_find.make_in_film_radius_table(y.benjix, y.benjiy, y.benjijiao);

        m_find.bool_max_connectivity_analyze2_2_checked_box_lookupA(&obj_weight_map__, &outx, &outy, &min_r, &max_r);
        //		m_find. bool_max_connectivity_analyze2_2_checked_box_lookup(&obj_weight_map__,&outx,&outy,&min_r);

        // 设置下一帧二值化等图像操作的范围参数
        m_find.film_w = 1900;  // 设置右边线坐标
        m_find.film_h = 1300;  // 设置上边线坐标
        m_find.film_w_ = -100; // 设置左边线坐标
        m_find.film_h_ = -100; // 设置下边线坐标
        m_find.make_in_film_radius_table(y.benjix, y.benjiy, y.benjijiao);

        y.ball_x = outx;
        y.ball_y = outy;
        y.ball_zhong = obj_weight_map__;
        //		if (y.ball_zhong<25)//2014test+
        //			y.ball_zhong=0;
        y.ball_jiao =
            angle_add_transfer(coordinate_transfer_angle(y.ball_x - MIRROR_CENTER_X, y.ball_y - MIRROR_CENTER_Y), 90);
        //		y.ball_chang=min_r;//使用球半径最小像素作极坐标半径
        y_ball_chang_min = max_r;   // 20110705
        if (y_ball_chang_min > 215) // cs2014+
            y_ball_chang_min = 215; // cs2014+
        if (y_ball_chang_min < 10)  // 20110705
            y_ball_chang_min = 10;  // 20110705
        y_ball_chang_real_min = min_r;
        if (y_ball_chang_real_min > 210) // cs2014+
            y_ball_chang_real_min = 210; // cs2014+
        if (y_ball_chang_real_min < 10)  // 20110705
            y_ball_chang_real_min = 10;  // 20110705
        y.ball_chang = coordinate_transfer_radius(y.ball_x - MIRROR_CENTER_X, y.ball_y - MIRROR_CENTER_Y);
        RGBTRIPLE ball2;
        ball2.rgbtBlue = 128;
        ball2.rgbtGreen = 128;
        ball2.rgbtRed = 255;
        //	m_find.	DrawFocus(y.ball_x,y.ball_y,&ball2);
        m_find.DrawFocus(sin(y.ball_jiao * 3.14 / 180.0) * y.ball_chang + 320,
                         cos(y.ball_jiao * 3.14 / 180.0) * y.ball_chang + 240, &ball2);
    }
    if (y.ball_chang > 240)
        y.ball_chang = 240;
    if (y.ball_chang < 0)
        y.ball_chang = 0;

#ifdef MOUSE_SET_BALL      ////用鼠标	测试球
    if (dianjipanding > 0) // 使用鼠标 标定机体在场地位置
    {
        dianjipanding; //	y.benjix=dianjix;	y.benjiy=dianjiy;	y.benjijiao=dianjijiao;
        y.ball_jiao = -f2l_angle(y.benjix, y.benjiy, -y.benjijiao, dianjix, dianjiy);  //	y.b
        y.ball_chang = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, dianjix, dianjiy); //	y.b
        y.ball_chang = fanquxian[int(y.ball_chang * cdbl / 2.7)];
        y.ball_zhong = 40 - 1;
    }
#endif

    if (y.ball_zhong > 10)
    {
        double balljiao, ballchang, ballx, bally;
        balljiao = -y.ball_jiao - y.benjijiao + 90;
        ballchang = quxian[int(y.ball_chang)] / cdbl * 2.7; // quxian[int(changdi->ball_chang)];
        m_find.zhizuobiao(&ballx, &bally, balljiao, ballchang, y.benjix, y.benjiy);
        y.quanbx = ballx;
        y.quanby = bally;
    }

#endif //  RUN_WITH_EMU	//使用仿真时屏蔽所有图像处理代码 结束	<<<<<<<<<<++++
#ifdef RUN_WITH_EMU
    // 仿真环境插入数据
    // 插入机体全场位置
#ifndef RUN_WITH_EMU_LINK_MSLCoach2
    y.benjix = ReceiveServerData.robotLocal_x[0 + m_nAgentID_g_set];
    y.benjiy = ReceiveServerData.robotLocal_y[0 + m_nAgentID_g_set];
    y.benjijiao = -ReceiveServerData.robotLocal_a[0 + m_nAgentID_g_set];
#endif
#ifdef RUN_WITH_EMU_LINK_MSLCoach2 // 设置使用本机上的2号教练机 用于测试双队对抗 向左进攻
    y.benjix = ReceiveServerData.robotLocal_x[6 + m_nAgentID_g_set];
    y.benjiy = ReceiveServerData.robotLocal_y[6 + m_nAgentID_g_set];
    y.benjijiao = -ReceiveServerData.robotLocal_a[6 + m_nAgentID_g_set];
#endif
    // 插入球全场位置
    y.quanbx = ReceiveServerData.ballLocal_x;
    y.quanby = ReceiveServerData.ballLocal_y;
    y.ball_zhong = 1000;
    // 场地上机体对本机是否能看到球的遮挡检测
    if (1)
    {
        const float obstruct_obj_size_r = 25;   // 遮挡物体的遮挡半径 20cm
        const float max_ball_view_radius = 800; // 最大可以看到球的距离
        float ball_obj_angle = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, y.quanbx, y.quanby);
        float ball_obj_radius = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, y.quanbx, y.quanby);
        for (i = 0; i < 12; i++)
        {
#ifndef RUN_WITH_EMU_LINK_MSLCoach2 // 设置使用本机上的原教练机
            if (i != m_nAgentID_g_set)
#endif
#ifdef RUN_WITH_EMU_LINK_MSLCoach2 // 设置使用本机上的2号教练机 用于测试双队对抗
                if (i != m_nAgentID_g_set + 6)
#endif
                {
                    float obstruct_obj_angle =
                        f2l_angle(y.benjix, y.benjiy, -y.benjijiao, ReceiveServerData.robotLocal_x[i],
                                  ReceiveServerData.robotLocal_y[i]);
                    float obstruct_obj_radius =
                        f2l_radius(y.benjix, y.benjiy, -y.benjijiao, ReceiveServerData.robotLocal_x[i],
                                   ReceiveServerData.robotLocal_y[i]); // 25为机体遮挡球的半径
                    float obstruct_obj_D_angle =
                        atan2(obstruct_obj_size_r, obstruct_obj_radius) * 180.0 / 3.14; // 25为机体遮挡球的半径
                    if (fabs(angle_add_transfer(obstruct_obj_angle, -ball_obj_angle)) <
                                fabs(obstruct_obj_D_angle)           // 球在遮挡锥角度内
                            && obstruct_obj_radius < ball_obj_radius // 球比遮挡物远
                        || max_ball_view_radius < ball_obj_radius)   // 球比距离大于800cm
                        y.ball_zhong = 0;
                }
        }
    }
    // 插入球本机坐标位置
    y.ball_jiao = -f2l_angle(y.benjix, y.benjiy, -y.benjijiao, y.quanbx, y.quanby);
    if (int(f2l_radius(y.benjix, y.benjiy, -y.benjijiao, y.quanbx, y.quanby) * cdbl / 2.7) > 2999 ||
        int(f2l_radius(y.benjix, y.benjiy, -y.benjijiao, y.quanbx, y.quanby) * cdbl / 2.7) < 0)
        y.ball_chang = fanquxian[2999];
    else
        y.ball_chang = fanquxian[int(f2l_radius(y.benjix, y.benjiy, -y.benjijiao, y.quanbx, y.quanby) * cdbl / 2.7)];

    float y_ball_chang_mintemptemp = (f2l_radius(y.benjix, y.benjiy, -y.benjijiao, y.quanbx, y.quanby) * cdbl / 2.7);
    y_ball_chang_min = y.ball_chang + 5000 / y_ball_chang_mintemptemp;
    // 插入AD传感器值
    if ((y.ball_chang < 55) && abs(y.ball_jiao) < 20)
    {
        m_pAD[0] = 2400 + 1; // 1800+1;
        m_pAD[1] = 2400 + 1; // 1800+1;
        sendToServerData.holdball = 1;
    }
    else
    {
        m_pAD[0] = 100;
        m_pAD[1] = 100;
        sendToServerData.holdball = 0;
    }
    if (sendToServerData_kickTimelate > 0)
    {
        sendToServerData_kickTimelate--;
    }
    else
    {
        sendToServerData.kickTime = 0;
    }
    // 插入开机判定 数据 使仿真立即开机
    kaijipd = 111 + 1; // 450+1;
#endif

    balldiushi(&y); // 用鼠标测试球时暂时注释掉

    ball_angle = -y.ball_jiao;
    ball_radius = y.ball_chang;
    ball_weight = y.ball_zhong;
    ball_Rradius =
        quxian[int(ball_radius)] / cdbl * 2.7 * Dribble_ball.fieldzoomtime; // adjust_table[int(ball_radius) ];//22;

    /*	double	balljiao_temp=angle_add_transfer2(-y.ball_jiao-y.benjijiao+90,0);
        double	ballchang_temp=quxian[int(y.ball_chang)]/cdbl*2.7;//quxian[int(changdi->ball_chang)];
        //double	ballchang_temp=quxian[int(y.ball_chang_min)]/cdbl*2.7;//quxian[int(changdi->ball_chang)];
        double  temp_y_quanbx;
        double  temp_y_quanby;
        m_find.zhizuobiao(&temp_y_quanbx,&temp_y_quanby,balljiao_temp,ballchang_temp,y.benjix,y.benjiy);
        //		pField->SetBall(2,ballx,bally);
        //		pField->SetBall(3,1550,1000);
        y.quanbx=temp_y_quanbx;
        y.quanby=temp_y_quanby;
    */

    // 判断机体是否上电
    if (
        /*	(m_pMotorPos_P[0]==m_pMotorPos[0])&&
            (m_pMotorPos_P[1]==m_pMotorPos[1])&&
            (m_pMotorPos_P[2]==m_pMotorPos[2])&&
            (m_pMotorCurrent_P[0]==m_pMotorCurrent[0])&&
            (m_pMotorCurrent_P[1]==m_pMotorCurrent[1])&&
            (m_pMotorCurrent_P[2]==m_pMotorCurrent[2])&&
            (m_pIO_P[0]==m_pIO[0])&&
            (m_pIO_P[1]==m_pIO[1])&&
            (m_pIO_P[2]==m_pIO[2])&&
            (m_pIO_P[3]==m_pIO[3])&&
            (m_pAD_P[0]==m_pAD[0])&&
            (m_pAD_P[1]==m_pAD[0])&&
            (m_pAD_P[2]==m_pAD[1])&&
            (m_pAD_P[3]==m_pAD[0])||
        */
        (SCDataCount_P[60] == SCDataCount))
    {
        //	PowerOn=0;
        PowerOnDelay--; // 如果 10帧内没有效数据从串口接收就判定下位机关机
    }
    else
    {
        //	PowerOn=1;
        PowerOnDelay += 10;
    }

    if (PowerOnDelay > 10)
        PowerOnDelay = 10;

    if (PowerOnDelay < 0)
        PowerOnDelay = 0;

    if (PowerOnDelay == 0) // 如果 10帧内没有效数据从串口接收就判定下位机关机
    {
        PowerOn = 0;
    }
    else
    {
        PowerOn = 1;
    }

    float front_a, front_v, turn_w;
    // 机体 转一周码盘计数192405.3  22.34cm旋转半径 2*3.14159*0.2234=1.403662412(米)<=>192405.3(计数)
    // 1计数=1.403662412/192405.3 	1370737 对 单轴10 m/s int vl=m_pMotorPos_P[0]-m_pMotorPos[0]; int
    // vb=m_pMotorPos_P[2]-m_pMotorPos[2]; int vr=m_pMotorPos_P[1]-m_pMotorPos[1];
    ///////////////////////////////////////////////////////////////////
    int vl, vb, vr, j;
    float read_time_l, read_time_b, read_time_r; // 11 11 11 12 12 12 14
    int time_out = 20;                           // 记录多少个一样的数据认为 电机已经停转
    for (i = 0; i < 100; i++)
    {
        vl = m_pMotorPos_H[i][0] - m_pMotorPos[0];
        if (vl != 0)
            break;
    }
    for (j = i; j < 100; j++)
    {
        if (m_pMotorPos_H[j][0] != m_pMotorPos_H[i][0])
            break;
    }
    if (j >= time_out)
    {
        vl = 0;
        read_time_l = 100;
    }
    else
        read_time_l = j - i;

    for (i = 0; i < 100; i++)
    {
        vb = m_pMotorPos_H[i][2] - m_pMotorPos[2];
        if (vb != 0)
            break;
    }
    for (j = i; j < 100; j++)
    {
        if (m_pMotorPos_H[j][2] != m_pMotorPos_H[i][2])
            break;
    }
    if (j >= time_out)
    {
        vb = 0;
        read_time_b = 100;
    }
    else
        read_time_b = j - i;

    for (i = 0; i < 100; i++)
    {
        vr = m_pMotorPos_H[i][1] - m_pMotorPos[1];
        if (vr != 0)
            break;
    }
    for (j = i; j < 100; j++)
    {
        if (m_pMotorPos_H[j][1] != m_pMotorPos_H[i][1])
            break;
    }
    if (j >= time_out)
    {
        vr = 0;
        read_time_r = 100;
    }
    else
        read_time_r = j - i;

// if ((read_time_l==0)||(read_time_b==0)||(read_time_r==0))//测试代码
//{read_time_l=read_time_l;//测试代码
//					 CString str;
//					 str.Format("((read_time_l==0)||(read_time_b==0)||(read_time_r==0))//测试代码",2);
//					 AfxMessageBox(str);
// }
///////////////////////////////////////////////////////////////////
#ifdef CODED_EMU                                                                    // 使用码盘模拟
    float userset = -10;                                                            // 2.8;//0;//
    vl = motospeedtemp_H[1][0] * 0.7 * 512 * 12 * 4 / 43 * DIANJI / 1000 * userset; // 控制电机输出速度值记录
    vr = motospeedtemp_H[1][1] * 0.7 * 512 * 12 * 4 / 43 * DIANJI / 1000 * userset;
    vb = motospeedtemp_H[1][2] * 0.7 * 512 * 12 * 4 / 43 * DIANJI / 1000 * userset;
#endif
    // 21950.2==2.5m/s
    /*因调试去掉*/ Dribble_ball.O_movement_T_analyze(
        &front_a, &front_v, &turn_w, vl / read_time_l, vb / read_time_b,
        vr / read_time_r); //-mhLR[1]->Pos_[0],-mhBR[1]->Pos_[2],-mhRR[1]->Pos_[1]);//[0]是上个周期的结果
                           //*move_v_angle, *move_v_speed, *turn_v
    Dribble_ball.RMfv_m = front_v;
    Dribble_ball.RMfa_m = front_a;
    Dribble_ball.RMw_m = turn_w;
    Dribble_ball.RMfv = front_v * 0.0000072953417187572275815686989911401 * 1000 / Dribble_ball.Gb_loop_time; //
    Dribble_ball.RMfa = front_a;
    Dribble_ball.RMw = turn_w * 0.0000072953417187572275815686989911401 * 1000 / Dribble_ball.Gb_loop_time * 360 /
                       (2 * 3.14159 * 0.2234); // 米/s->度/s n*360/(2*pi*r)

    for (i = 100 - 1; i > 0; i--) // 上100帧周期码盘位置历史记录
    {
        RMfv_h[i] = RMfv_h[i - 1];
        RMfa_h[i] = RMfa_h[i - 1];
        RMw_h[i] = RMw_h[i - 1];
    }
    RMfv_h[0] = Dribble_ball.RMfv;
    RMfa_h[0] = Dribble_ball.RMfa;
    RMw_h[0] = Dribble_ball.RMw;

    if (turn_w == 0)     // 测试代码
        turn_w = turn_w; // 测试代码
    if ((m_pMotorPos_H[0][0] != m_pMotorPos[0]) || (m_pMotorPos_H[0][2] != m_pMotorPos[2]) ||
        (m_pMotorPos_H[0][1] != m_pMotorPos[1]))
    {
        if (!((read_time_l == 100) || (read_time_b == 100) || (read_time_r == 100)))
            if (Pw == turn_w) // 测试代码
            {
                Pw = Pw; // 测试代码
                CString str;
                str.Format("if (Pw==turn_w)//测试代码", 2);
                //		 AfxMessageBox(str);
            }
    }
    Pw = turn_w; // 测试代码//转速上次数据

    net.toS_V_Angle = angle_add_transfer(front_a, Dribble_ball.robot_L.f_a); // 向教练机发送自己的移动速度方向（0~360）
    net.toS_V = front_v;                                                     // 向教练机发送自己的移动速度（0~255）
    // 码盘数据 机体掉电 数据判断vl/read_time_l,vb/read_time_b,vr/read_time_r
    if ((fabs(vl / read_time_l) < 19000) && (fabs(vb / read_time_b) < 19000) && (fabs(vr / read_time_r) < 19000) &&
        (PowerOn == 1)) //&&(fabs(Dribble_ball.RMfv)<10.0)
    // if (((abs(vl)<1370737)&&(abs(vb)<1370737)&&(abs(vr)<1370737)))
    //	&&((abs(m_pMotorPos[0])>1000)&&(abs(m_pMotorPos[1])>1000)&&(abs(m_pMotorPos[2])>1000)))
    {
        // 显示机体状态
        Dribble_ball.Distance_Location_2(ball_angle, ball_Rradius, ball_weight, y.benjix / 100.0, y.benjiy / 100.0,
                                         -y.benjijiao, valxx); // 注意徐欣欣 场地坐标是逆时针坐标系
        ///*调试预测球用机体走向*/m_find.pField->SetAgent(12 ,Dribble_ball.robot_L.f_x*100/Dribble_ball.fieldzoomtime
        ///+Dribble_ball.fount_xVset*10     ,
        //										Dribble_ball.robot_L.f_y*100/Dribble_ball.fieldzoomtime
        //+Dribble_ball.fount_yVset*10
        //,-Dribble_ball.robot_L.f_a);//场子正中向上  逆时针坐标系
        /*显示球*/ Debug_D[11].Draw_line_(320, 240, ball_angle - 90, ball_Rradius, 255, 0, 0, pBuffer);
        /*显示平移*/ Debug_D[11].Draw_line_(324, 240, Dribble_ball.RMfa - 90, Dribble_ball.RMfv * 100, 255, 255, 0,
                                            pBuffer);
        float tempRMw = Dribble_ball.RMw;
        if (tempRMw > 360)
            tempRMw = 360;
        if (tempRMw < -360)
            tempRMw = -360;
        /*显示旋转*/ Debug_D[11].Draw_circle(
            320, 240, -90, tempRMw * 0.5 - 90, 50, 255, 255, 0,
            pBuffer); //(320 ,240 ,-90		,Dribble_ball.robot_w*4-90,50,255,255,0,  pBuffer);
        // 将码盘和视觉复合
        y.benjix = Dribble_ball.robot_L.f_x * 100.0 / Dribble_ball.fieldzoomtime;
        y.benjiy = Dribble_ball.robot_L.f_y * 100.0 / Dribble_ball.fieldzoomtime;
        y.benjijiao = -Dribble_ball.robot_L.f_a;
    }
    else
    {
        Dribble_ball.robot_L.f_x = y.benjix * Dribble_ball.fieldzoomtime / 100.0;
        Dribble_ball.robot_L.f_y = y.benjiy * Dribble_ball.fieldzoomtime / 100.0;
        Dribble_ball.robot_L.f_a = -y.benjijiao;
    }
    ///*调试程序*/m_find.pField->SetAgent(12 ,Dribble_ball.robot_L.f_x*100/Dribble_ball.fieldzoomtime     +0
    ///,Dribble_ball.robot_L.f_y*100/Dribble_ball.fieldzoomtime   +0   ,-Dribble_ball.robot_L.f_a);//场子正中向上
    /// 逆时针坐标系 *调试程序*/m_find.pField->SetAgent(10 ,Dribble_ball.robot_L_test.f_x*100/Dribble_ball.fieldzoomtime
    ///+0     ,Dribble_ball.robot_L_test.f_y*100/Dribble_ball.fieldzoomtime   +0
    ///,-Dribble_ball.robot_L_test.f_a);//场子正中向上  逆时针坐标系 *调试程序*/m_find.pField->SetAgent(10
    ///,y.benjix,y.benjiy,y.benjijiao);//场子正中向上  逆时针坐标系

    m_find.pField->SetBall(11, y.quanbx, y.quanby);

    /*调试程序*/ m_find.pField->SetAgent(11, Dribble_ball.robot_L.f_x * 100 / Dribble_ball.fieldzoomtime,
                                         Dribble_ball.robot_L.f_y * 100 / Dribble_ball.fieldzoomtime,
                                         -Dribble_ball.robot_L.f_a); // 场子正中向上  逆时针坐标系
    //	/*调试程序*/m_find.pField->SetBall (11
    //,Dribble_ball.ball_d_h[0].f_x*100/Dribble_ball.fieldzoomtime,Dribble_ball.ball_d_h[0].f_y*100/Dribble_ball.fieldzoomtime);
    //	/*调试程序*/m_find.pField->SetBall (12
    //,Dribble_ball.ball_d_h[0].f_x*100/Dribble_ball.fieldzoomtime+Dribble_ball.ball_d_h[0].f_vx*20
    //										   ,Dribble_ball.ball_d_h[0].f_y*100/Dribble_ball.fieldzoomtime+Dribble_ball.ball_d_h[0].f_vy*20);
    //	Debug_D[11].Draw_line	   (100 ,100 ,100+bv_f_,100,0,255,0,  pBuffer);

    //****************************************************************************************************
    // QueryPerformanceCounter(&CountEnd);
    QueryPerformanceCounter(&CountEnd);
    dfElapseMS1 =
        (double)((double)(CountEnd.QuadPart - CountStart.QuadPart + 10) / (double)Frequency.QuadPart) * 1000.0 -
        dfElapseMS;
    //****************************************************************************************************第二个时间节点
    //	return ;//调试 返回

    // 规避测试部分

    if (0) // 2012障碍物检测
    {
        m_find.getzhangaiwu2(10, 239, 15, 1);

        // 视觉的障碍地图
        m_find.getblackobj(y.benjix, y.benjiy,
                           -y.benjijiao); //????????????y.benjijiao取负？？？？？？？？得到 聚类后 的几个黑色目标坐标
    }

#ifdef RUN_WITH_EMU
    // 仿真环境代码	 清掉视觉处理得到的障碍物
    in_obj_ll = 0; //
    in_obj_l = 0;
#endif

#ifndef RUN_WITH_EMU // 使用仿真时屏蔽所有图像处理代码 障碍部分
    if (1)           // 2013障碍物检测
    {
        in_obj_ll = 0;
        in_obj_l = 0;
        //	m_find.getblackobj2(y.benjix,y.benjiy,-y.benjijiao);//????????????y.benjijiao取负？？？？？？？？得到 聚类后
        // 的几个黑色目标坐标
    }
#endif // RUN_WITH_EMU	//使用仿真时屏蔽所有图像处理代码 障碍部分 结束

    // 在视觉的障碍地图加入网络障碍
    /*	for (i=0;i<AG_OBST_NUM;i++)
        {	if(net.rec_obst[i].valid==1)
            {//	in_obj_x[in_obj_l]=net.rec_obst[i].x;
            //	in_obj_y[in_obj_l]=net.rec_obst[i].y;
            //	in_obj_w[in_obj_l]=net.rec_obst[i].size;
            //	in_obj_l++;
                pp1.add_obstacle_point(net.rec_obst[i].x,net.rec_obst[i].y,net.rec_obst[i].size);//设置规避球
       注意到实际场地可能因为场地分辨率太小规避总规避得太大
            }
        }
    */

    m_find.pField->RemoveAllObsts();
    net.Clear_Obst();
    for (i = 0; i < Learn_num; i++)
    {
        if (Learn_Flag[i] > 1) // 大于等于两个障碍射线 就计算在内
        {
            double balljiao, ballchang, ballx, bally;
            balljiao = -Learn_A[i] - y.benjijiao + 90; // angle_add_transfer(-Learn_A[i],y.benjijiao);
            ballchang = quxian[int(Learn_R[i])] / cdbl * 2.7;
            m_find.zhizuobiao(&ballx, &bally, balljiao, ballchang, y.benjix, y.benjiy);
            //	m_find.zhizuobiao(&ballx,&bally,Learn_A[i],Learn_R[i],320,240);
            //	RGBTRIPLE ball1;
            //	ball1.rgbtBlue = 0xFF;	ball1.rgbtGreen = 0xFF; ball1.rgbtRed = 0xFF;
            //	m_find.DrawFocus(ballx,bally,&ball1);
            int xt = ballx;
            int yt = bally;
            /*调试程序*/ m_find.pField->SetObst(i, xt, yt); // 场子正中向上  逆时针坐标系
            //		m_find.pField->SetBall(5,xt,yt);
            net.AddObst(xt, yt, Learn_Flag[i]);
        }
        //	net.Rep_Obst();
    }
#ifdef RUN_WITH_EMU
#ifndef RUN_WITH_LOCAL_EMU
    m_find.pField->RemoveAllObsts();
    net.Clear_Obst();
    black_OBJ_L = 0;
    // 仿真环境代码		 加入仿真环境障碍物
    for (i = 0; i < 12; i++)
    {
#ifndef RUN_WITH_EMU_LINK_MSLCoach2 // 设置使用本机上的原教练机
        if (i != m_nAgentID_g_set)
#endif
#ifdef RUN_WITH_EMU_LINK_MSLCoach2 // 设置使用本机上的2号教练机 用于测试双队对抗
            if (i != m_nAgentID_g_set + 6)
#endif
            {
                float obs_A = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, ReceiveServerData.robotLocal_x[i],
                                        ReceiveServerData.robotLocal_y[i]);
                float obs_R = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, ReceiveServerData.robotLocal_x[i],
                                         ReceiveServerData.robotLocal_y[i]);
                obs_A = obs_A + (rand() / 16383.5 - 1.0) * 1.9;
                obs_R = obs_R + (rand() / 16383.5 - 1.0) * obs_R / 10.0; // 6
                float obs_X = l2f_x(y.benjix, y.benjiy, -y.benjijiao, obs_A, obs_R);
                float obs_Y = l2f_y(y.benjix, y.benjiy, -y.benjijiao, obs_A, obs_R);

                pp1.add_obstacle_point(obs_X, obs_Y,
                                       10); // 设置规避球   注意到实际场地可能因为场地分辨率太小规避总规避得太大
                /*调试程序*/ m_find.pField->SetObst(i, obs_X, obs_Y); // 场子正中向上  逆时针坐标系
                net.AddObst(obs_X, obs_Y, 10);

                black_OBJ_fx[black_OBJ_L] =
                    obs_X; // l2f_x(f_x,f_y,f_a,
                           // -black_OBJ_A[i],quxian[(int)black_OBJ_R[i]]/cdbl*2.7);//输入本机测量得到的障碍物数组
                           // 全局变量
                black_OBJ_fy[black_OBJ_L] =
                    obs_Y; // l2f_y(f_x,f_y,f_a, -black_OBJ_A[i],quxian[(int)black_OBJ_R[i]]/cdbl*2.7);
                black_OBJ_A[black_OBJ_L] = -f2l_angle(y.benjix, y.benjiy, -y.benjijiao, obs_X, obs_Y);
                black_OBJ_R[black_OBJ_L] =
                    fanquxian[(int)(f2l_radius(y.benjix, y.benjiy, y.benjijiao, obs_X, obs_Y) / 2.7 * cdbl)];
                black_OBJ_L++;

                //					pp1.add_obstacle_point(ReceiveServerData.robotLocal_x[i],ReceiveServerData.robotLocal_y[i],10);//设置规避球
                // 注意到实际场地可能因为场地分辨率太小规避总规避得太大
                //					/*调试程序*/m_find.pField->SetObst(i,ReceiveServerData.robotLocal_x[i],ReceiveServerData.robotLocal_y[i]);//场子正中向上
                // 逆时针坐标系
                // net.AddObst(ReceiveServerData.robotLocal_x[i],ReceiveServerData.robotLocal_y[i],10);
            }
    }
#endif

#endif
    ////////////////////////////////////////////////////////////////////////////////////////////
    // pp1.Obstacle_size_set=Obstacle_Parameter/field_zoom_time;// 8.2/0.4=20  实验室场地 20的障碍设置大小
    // 设置每个障碍点的影响范围 	pp1.datain(y.benjix,y.benjiy,y.quanbx,y.quanby);//输入路径规划起始点和终点
    // y.benjix,y.benjiy起点  y.quanbx,y.quanby终点 	float Close_direct=atan2(y.quanby-600,y.quanbx-900)*360/6.28;
    //	if((ttx<0)||(ttx>1800)||(tty<0)||(tty>1200 ))
    //		Direct_Cost=10;

    //	pp1.Close_target_direction_01(y.quanbx,y.quanby,Close_direct,6);
    //	pp1.Ppgrow();
    // aaaa=angle_add_transfer(-pp1.path_plan_angle,y.benjijiao);//aaaa是路径规划结果方向 前方0度 右正左负 顺时针坐标系
    //	/*显示路径规划结果*/Debug_D[11].Draw_line_	(320 ,240 ,aaaa -90,240,255,0,0,  pBuffer);
    //	/*显示路径规划结果*/Debug_D[11].Draw_line_	(322 ,242 , aaaa-90,240,0,255,255,  pBuffer);
    //	/*显示路径规划结果*/Debug_D[11].Draw_line_	(322 ,238 , aaaa-90,240,255,0,0,  pBuffer);

    //	for (i=1;i<path_l;i++)//path_l是路径使用了多少个点
    //	{	// path_x[i]/(float)MAP_X*(float)MAP_Xmm -MAP_oX  是横坐标 [1800*1200场地]
    // path_y[i]/(float)MAP_Y*(float)MAP_Ymm -MAP_oY  是纵坐标 [1800*1200场地]
    //	/*调试程序*/m_find.pField->SetAgent(i ,path_x[i]/(float)MAP_X*(float)MAP_Xmm
    //-MAP_oX,path_y[i]/(float)MAP_Y*(float)MAP_Ymm -MAP_oY,0);//场子正中向上  逆时针坐标系
    //	}
    ////////////////////////////////////////////////////////////////////////////////////////////

    m_find.bianxiantouying(y);
    m_find.guibi(); // 规避函数目前无效果

    //	m_find.menbianjie(&y);//取得门的左右边界
    jiaodujiaozhun(0, &y); // 对全景角度的安装误差进行校准

    //****************************************************************************************************
    QueryPerformanceCounter(&CountEnd);
    dfElapseMS2 =
        (double)((double)(CountEnd.QuadPart - CountStart.QuadPart + 10) / (double)Frequency.QuadPart) * 1000.0 -
        dfElapseMS - dfElapseMS1;
    //	dfElapseMS3=(double)((double)(CountEnd.QuadPart-CountStart.QuadPart+10)/(double)Frequency.QuadPart)*1000.0;
    //****************************************************************************************************第三个时间节点

    // 	double jxx;
    // 	double cxx;
    //
    // 	m_find.shichangfa(1,&jxx,&cxx);
    // 	zhuan=duizhun(0,0,1500);
    // 	Controlmoto(jxx,cxx*50,3000,&mtl,&mtr,&mtb);

    //	pipeijuli=m_find.pipei2(&y);//场线定位
    juli_shang = pipeijuli;
    double chang1;
    double x1;
    double y1;
    double jiao1;
    int panding;
    m_find.jizuobiao(1800, 500, &y.ddoor_zuo, &chang1, y.benjix, y.benjiy);
    y.ddoor_zuo = -jiajiao(-y.ddoor_zuo, y.benjijiao - 90);
    // 将场线匹配出来的敌方门的左门柱投影到图像中
    m_find.jizuobiao(1800, 700, &y.ddoor_you, &chang1, y.benjix, y.benjiy);
    y.ddoor_you = -jiajiao(-y.ddoor_you, y.benjijiao - 90);
    // 敌方门右门柱
    m_find.jizuobiao(1800, 600, &y.ddoor_jiao, &y.ddoor_chang, y.benjix, y.benjiy);
    y.ddoor_jiao = -jiajiao(-y.ddoor_jiao, y.benjijiao - 90);
    // 敌方门的中间角度
    y.ddoor_chang = fanquxian[int(y.ddoor_chang * cdbl / 2.7)]; /// cdbl*2.7)];
    // 将敌方门的长度投影
    m_find.zhizuobiao(&y.ddoor_x, &y.ddoor_y, y.ddoor_jiao, y.ddoor_chang, 320, 240);
    m_find.zhizuobiao(&x1, &y1, y.ddoor_zuo, y.ddoor_chang, 320, 240);
    m_find.DrawFocus((long)x1, (long)y1, &ball);
    m_find.zhizuobiao(&x1, &y1, y.ddoor_you, y.ddoor_chang, 320, 240);
    m_find.DrawFocus((long)x1, (long)y1, &ball);
    m_find.zhizuobiao(&x1, &y1, y.ddoor_jiao, y.ddoor_chang, 320, 240);
    m_find.DrawFocus((long)x1, (long)y1, &ball);
    double jiao11, chang11, x11, y11;
    for (i = 500; i < 700; i = i + 10)
    {
        m_find.jizuobiao(1800, i, &jiao11, &chang11, y.benjix, y.benjiy);
        jiao11 = -jiajiao(-jiao11, y.benjijiao - 90);
        chang11 = fanquxian[int(chang11 * cdbl / 2.7)];
        m_find.zhizuobiao(&x11, &y11, jiao11, chang11, 320, 240);
        m_find.SetColor((long)x11, (long)y11, ball);
        m_find.SetColor((long)x11 + 1, (long)y11, ball);
        m_find.SetColor((long)x11 - 1, (long)y11, ball);
        m_find.SetColor((long)x11, (long)y11 + 1, ball);
        m_find.SetColor((long)x11, (long)y11 - 1, ball);
        m_find.jizuobiao(0, i, &jiao11, &chang11, y.benjix, y.benjiy);
        jiao11 = -jiajiao(-jiao11, y.benjijiao - 90);
        chang11 = fanquxian[int(chang11 * cdbl / 2.7)];
        m_find.zhizuobiao(&x11, &y11, jiao11, chang11, 320, 240);
        m_find.SetColor((long)x11, (long)y11, ball);
        m_find.SetColor((long)x11 + 1, (long)y11, ball);
        m_find.SetColor((long)x11 - 1, (long)y11, ball);
        m_find.SetColor((long)x11, (long)y11 + 1, ball);
        m_find.SetColor((long)x11, (long)y11 - 1, ball);
    }
    for (i = 0; i < 640; i++)
    {
        m_find.SetColor(i, 240, ball);
    }
    for (i = 0; i < 480; i++)
    {
        m_find.SetColor(320, i, ball);
    }
    // 己方门做同样的处理
    m_find.jizuobiao(0, 700, &y.jdoor_zuo, &chang1, y.benjix, y.benjiy);
    y.jdoor_zuo = -jiajiao(-y.jdoor_zuo, y.benjijiao - 90);
    m_find.jizuobiao(0, 500, &y.jdoor_you, &chang1, y.benjix, y.benjiy);
    y.jdoor_you = -jiajiao(-y.jdoor_you, y.benjijiao - 90);
    m_find.jizuobiao(0, 600, &y.jdoor_jiao, &y.jdoor_chang, y.benjix, y.benjiy);
    y.jdoor_jiao = -jiajiao(-y.jdoor_jiao, y.benjijiao - 90);

    y.jdoor_chang = fanquxian[int(y.jdoor_chang * cdbl / 2.7)]; /// cdbl*2.7)];
    m_find.zhizuobiao(&y.jdoor_x, &y.jdoor_y, y.jdoor_jiao, y.jdoor_chang, 320, 240);
    m_find.zhizuobiao(&x1, &y1, y.jdoor_zuo, y.jdoor_chang, 320, 240);
    m_find.DrawFocus((long)x1, (long)y1, &ball);
    m_find.zhizuobiao(&x1, &y1, y.jdoor_you, y.jdoor_chang, 320, 240);
    m_find.DrawFocus((long)x1, (long)y1, &ball);
    m_find.zhizuobiao(&x1, &y1, y.jdoor_jiao, y.jdoor_chang, 320, 240);
    m_find.DrawFocus((long)x1, (long)y1, &ball);

    // jiaoduizhun(&L_moto,&R_moto,y.ball_jiao,70,1,0);
    // m_find.xianshi();
    ball.rgbtBlue = 0x00;
    ball.rgbtGreen = 0x00;
    ball.rgbtRed = 0x00;
    // strShow.Format("H=%3d,S=%3d,I=%3d,x=%d,y=%d",m_HSI->H ,m_HSI->S,m_HSI->I,m_pt.x,m_pt.y);
    // 显示采集颜色
    leijiaqi++;
    //	if(leijiaqi<200)
    //	m_find.changdiyidong(95, 75, &y.ball_jiao, &y.ball_chang);
    //	else if(leijiaqi<400)
    //	m_find.changdiyidong(205, 75, &y.ball_jiao, &y.ball_chang);
    //	else if(leijiaqi<600)
    //	m_find.changdiyidong(205, 130, &y.ball_jiao, &y.ball_chang);
    //	else if(leijiaqi<800)
    //	m_find.changdiyidong(95, 130, &y.ball_jiao, &y.ball_chang);
    //	else
    //	leijiaqi=1;

    double xz;
    double yz;
    double jz;
    double cz;

    //	m_find.zhizuobiao(&xz,&yz,jz,cz,320,240);
    //	m_find.DrawFocus((long)xz,(long)yz,&ball);
    //	m_find.DrawFocus((long)y.ball_x,(long)y.ball_y,&ball); //在球的位置上画十字
    //	m_find.DrawFocus((long)y.ddoor_x,(long)y.ddoor_y,&ball);//在对方门的位置上画十字
    //	m_find.DrawFocus((long)y.jdoor_x,(long)y.jdoor_y,&ball);//在己方门的位置上画十字
    //	m_find.DrawFocus(320,240, &ball); //在320,240中心点的位置上画十字,为了调节全景摄像头
    //	m_find.jizuobiao(y.ball_x,y.ball_y,&y.ball_jiao,&y.ball_chang,320,240);
    // 将球的极坐标
    //	m_find.xianshiweitu();

    // 时间
    kick = 0;
    // 控制全向

    //	return ;//调试 返回

    // 行为1进攻，2守门，3定位跑位，4己开球跑位主运动员跑位,5对方开球主运动员跑位
    // 6对方跑位后的后续动作 7全场跑位
    // 8测试规避 9辅助开球跑位 10辅助开球跑位后
    // 12防守 14全场跑。
    //////////////////////////////////////////////////////////////////////////
    // 状态反馈

    net.toS_status = net.rec_ctrl;

    long dmenchang;
    dmenchang = sqrt((y.benjix - 1800) * (y.benjix - 1800) + (y.benjiy - 600) * (y.benjiy - 600));

    switch (net.rec_ctrl)
    {
    case CTRL_STOP:
        xingwei = 11;
        break; // 停止
    case CTRL_ATTACK:
        xingwei = 1; // 1002;//1;//1002;//
        break;       // 开始
    case CTRL_GOALKEEP:
        xingwei = 2; // 守门
        break;

    // KickOff
    case CTRL_KICKOFF_PRIM_READY:
        xingwei = 4; // 主动开球跑位
        break;
    case CTRL_KICKOFF_SLAVE_READY:
        xingwei = 9; // 辅助开球跑位
        break;
    case CTRL_KICKOFF_PRIM:
        xingwei = 13; // 主动开球开始动作
        break;
    case CTRL_KICKOFF_SLAVE:
        xingwei = 10; // 辅助开球开始动作
        break;
    // 防守
    case CTRL_PRODEF:
        xingwei = 5; // 被开球跑位
        break;

    case CTRL_ATTENTIONBALL:
        xingwei = 6; // 被开球开始动作
        break;

    case CTRL_SEARCHBALL:
        xingwei = 14; // 全场寻球
        break;

    case CTRL_DEFENCE: // 定角度防守(net.rec_Def_Dist为离球距离，net.rec_Def_Ang为防守角度)
        xingwei = 107; // 12; //防守状态
        break;
        //

    case CTRL_MOVETO:
        xingwei = 7; // 全场跑位
        break;

    case 30:
        xingwei = 117; // DEF跑位
        break;

    case CTRL_PENALTY: // 点球
        xingwei = 22;
        break;

    case CTRL_PASS:   // 传球：（net.targetx，net.targety）为接球者的坐标，供参考
        xingwei = 10; //
        break;

    case CTRL_CATCH: // 接球：（net.rec_fromx，net.rec_fromy）为传球者的坐标，供参考
        xingwei = 30;
        break;

    case CTRL_PASS_MOVE: // 传球运动
        xingwei = 1001;  // 3000;
        break;

    case CTRL_CATCH_MOVE: // 接球运动1002    1003
        xingwei = 1002;
        break;

    case CTRL_CATCH_FOCUS:
        xingwei = 1004;
        break;

    case CTRL_FOLLOW:
        xingwei = 1127;
        break;
    case CTRL_DEF_BALL:
        xingwei = 1137;
        break;
    case CTRL_DEF_GOAL:
        xingwei = 1147;
        break;
    }
    xingwei_org = xingwei;
    for (i = 0; i < 5; i++)
        m_find.pField->SetAgent(100 + i, net.m_tm[i].x, net.m_tm[i].y, net.m_tm[i].angle);

    //	for (i=0;i<10;i++)
    //		m_find.pField->SetBall (100+i , net.rec_obst[i].x , net.rec_obst[i].y );

    net.rec_init_x;
    net.rec_init_y;
    // 球坐标
    net.re_ballx;
    net.re_bally;
    net.rec_other[0];
    if ((m_pMotorPos[0] > -10000) && (m_pMotorPos[0] < 1000000))
        m_pMotorPos_V[0] = m_pMotorPos[0];
    if ((m_pMotorPos[1] > -10000) && (m_pMotorPos[1] < 1000000))
        m_pMotorPos_V[1] = m_pMotorPos[1];
    if ((m_pMotorPos[2] > -10000) && (m_pMotorPos[2] < 1000000))
        m_pMotorPos_V[2] = m_pMotorPos[2];
    //		strShow.Format("%4.2f %5d %5d %5d st%d  GYRO可用%d Gia%f %f %d %d bx%f by%f bc%f 门长:%5d  时间:%10.4f,
    // 球角=%9.4f,球长=%9.4f,球重=%5.0f,门角%5d,门长%5.1f,夹角=%5d,本机角:%6.1f,B=%6d,L=%6d,R=%6d,转=%6d,状态:%s,时零%3.2f,时一%3.2f,时二%3.2f,时三%3.2f,时四%3.2f,时五%3.2f,时六%3.2f,耗时end:%3.2f
    // ms,帧率:%7.4f ms,本机x%5.1f,y%5.1f,匹配距离%7.1f,行为=%d\n 周期时间%3.1f 极坐标a%d d%d 上电%d 白点数 %d "// %f
    //		,robot__ball_radius_min,m_pMotorPos_V[0],m_pMotorPos_V[1],m_pMotorPos_V[2],shoottime,GYRO_available,GYRO_init_angle,move_Emu_robot_f_a,xingwei_catch_timer,xingwei_org,Display_A,Display_B,Display_C,dmenchang,ms_shang1,y.ball_jiao,y.ball_chang,y.ball_zhong,int(y.ddoor_jiao),y.ddoor_chang,jiajiao(y.ball_jiao,y.ddoor_jiao),y.benjijiao,mtb,mtl,mtr,zhuan,zhuangtai,dfElapseMS,dfElapseMS1,dfElapseMS2,dfElapseMS3,dfElapseMS4,dfElapseMS5,dfElapseMS6,dfElapseMSend,1/pingjun*1000,y.benjix,y.benjiy,pipeijuli,xingwei,LOOPTIME,net.rec_Def_Ang,net.rec_Def_Dist,kaijipd,xiandianshu);//,l_x,l_y,l_a
    //

    if (0)
    {
        // 存储上1000周期球的全场位置信息
        for (i = 1000 - 1; i > 0; i--)
        {
            quanball_h[i][0] = quanball_h[i - 1][0]; //[0]y,[1]x,[2]w //全场球信息历史记录
            quanball_h[i][1] = quanball_h[i - 1][1];
            quanball_h[i][2] = quanball_h[i - 1][2];
        }
        quanball_h[0][0] = y.quanby;
        quanball_h[0][1] = y.quanbx;
        quanball_h[0][2] = y.ball_zhong;
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
            ball_velocity =
                ball_distance * 45.0 / (endT - startT); // 45.0每秒帧数 计算 球视觉中走过的场地上路程的平均速度
            if (ball_velocity > 1000)                   // 10m/s 10 00
            {
                if (Shot_ball == 0)
                {
                    Shot_ball_point_Y = quanball_h[int(endT)][0]; // 球起飞全场定位点y
                    Shot_ball_point_X = quanball_h[int(endT)][1]; // 球起飞全场定位点x
                    min_dis_ball_y = Shot_ball_point_Y;           // 存储从起飞判定成立开始距机体最近的球的全场坐标
                                                                  // 球没起飞时为球的当前场地坐标
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

        if (Shot_ball > 0)
        {
            ; // 存储从起飞判定成立开始距机体最近的球的全场坐标 球没起飞时为球的当前场地坐标
            min_dis_ball_y;
            if (coordinate_transfer_radius(min_dis_ball_y - y.benjiy, min_dis_ball_x - y.benjix) >
                coordinate_transfer_radius(y.quanby - y.benjiy, y.quanbx - y.benjix))
            {
                min_dis_ball_y = y.quanby;
                min_dis_ball_x = y.quanbx;
            }
        }
        else
        {
            min_dis_ball_x = y.quanbx; // 存储从起飞判定成立开始距机体最近的球的全场坐标 球没起飞时为球的当前场地坐标
            min_dis_ball_y = y.quanby;
        }
    }
    bshot.Ball_Shot_Determine(y.benjix, y.benjiy, y.quanbx, y.quanby, y.ball_zhong);
    Shot_ball_point_X = bshot.Shot_ball_point_X; // 球起飞全场定位点x
    Shot_ball_point_Y = bshot.Shot_ball_point_Y; // 球起飞全场定位点y
    min_dis_ball_x = bshot.min_dis_ball_x;       // 存储从起飞判定成立开始距机体最近的球的全场坐标
                                                 // 球没起飞时为球的当前场地坐标
    min_dis_ball_y = bshot.min_dis_ball_y;
    // 计算 球朝向机体的速度远离
    Shot_ball = bshot.Shot_ball;
    if (Shot_ball > 0)
    { //	for(int i=50;i>=0;i--)//显示 球历史位置
        //	{
        //	/*调试程序*/m_find.pField->SetBall (i ,bxyc[i],byyc[i]);
        //	}
        //	/*调试程序*/m_find.pField->SetAgent(60 ,Shot_ball_point_X,Shot_ball_point_Y,90);//场子正中向上  逆时针坐标系
        //	/*调试程序*/m_find.pField->SetBall (60 ,Shot_ball_point_X,Shot_ball_point_Y);
        if (bshot.step_ball_ok == 1) //
        { // bshot.step_ball_x;bshot.step_ball_y;存储从起飞判定成立开始距机体最近的球的全场坐标
          // 球没起飞时为球的当前场地坐标
          //			/*调试程序*/m_find.pField->SetAgent(66 ,bshot.step_ball_x,bshot.step_ball_y,90);//场子正中向上
          // 逆时针坐标系
          //			/*调试程序*/m_find.pField->SetBall (66 ,bshot.step_ball_x,bshot.step_ball_y);
        }
    }
    // xingwei=22;
    //////////////////////////////////////////////////////////////////
    // 强制设置行为
    int xingwei_1_to_107_flag = 0;
    //	if(y.quanbx<=225 && (y.quanby>=300 && y.quanby<=900))
    if ((xingwei == 1001)
        //		&&(y.quanbx<=300 &&  y.quanby>=(650-350) && y.quanby<=(600+350))
        && (y.quanbx <= 390 && y.quanby >= (650 - 350) && y.quanby <= (600 + 350)))
    {
        xingwei_1_to_107_flag = 1;
        xingwei = 107;
        net.rec_Def_Ang = 0;
        net.rec_Def_Dist = 100;

        int xiaojinqu_flag = 0;
        if ((y.quanbx <= 100 && (y.quanby >= 600 - 390 && y.quanby <= 600 + 390)))
            xiaojinqu_flag = 1;

        if ((y.benjix < y.quanbx) && (y.ball_chang < 70) //||(y.quanbx<(600-195))||(y.quanbx>(600+195))
            && (xiaojinqu_flag == 0))
        {
            xingwei = 1001;
        }
        if ((y.ball_chang < 60) //&& (m_pAD[0]>1800 || m_pAD[1]>1800)拼抢中球进吸球器判定
            && (xiaojinqu_flag == 0))
        {
            xingwei = 1001;
        }
    }
    // xingwei=201;//
    // xingwei=23;//
    // xingwei=1;
    // xingwei=113;//防守 追球

    // if ((xingwei==7)&&(xingweiP==11))
    //	xingwei_1117_rotate_direct_test=-xingwei_1117_rotate_direct_test;
    // if (xingwei==7)
    //	xingwei=1117;

    // if (xingwei==7)
    //	xingwei=1127;
    if (xingwei == 7)
        shoottime = 0;

    if (xingwei == 1) // xingwei==301suyi
    {
        xingwei = 301;
        // 右侧平移
        // mtl=6000*cos((30-90)*3.14/180);
        // mtr=6000*cos((150-90)*3.14/180);
        // mtb=6000*cos((270-90)*3.14/180);

        // 向前平移
        // mtl=6000*cos((30)*3.14/180);
        // mtr=6000*cos((150)*3.14/180);
        // mtb=6000*cos((270)*3.14/180);
        mtl = (ball_Rradius - 100) * 10 * cos((30 - ball_angle) * 3.14 / 180) + 50 * ball_angle;
        mtr = (ball_Rradius - 100) * 10 * cos((150 - ball_angle) * 3.14 / 180) + 50 * ball_angle;
        mtb = (ball_Rradius - 100) * 10 * cos((270 - ball_angle) * 3.14 / 180) + 50 * ball_angle;
    }

    // if (xingwei==107)
    //	xingwei=1137;
    //	xingwei=1147;

    // if (y.benjix<900)
    //	if (xingwei==1)
    //		xingwei=1001;

    // if (xingwei==1004)	//用行为30测试1004接定点传球	//2014test+ 行为1004为教练机发送过来的球已经传出的信号
    //{//	xingwei= 1004;
    //	;//	net.rec_ctrl=CTRL_CATCH_FOCUS;
    //	xingwei=1;
    // }

    /*
    if (xingwei==30||xingwei==1002||xingwei==1004)	//用行为30测试1004接定点传球	//2014test+
    行为1004为教练机发送过来的球已经传出的信号
    {
        xingwei= 1004;
        net.rec_targetx=300;//1350;//Catch_robot_f_x;				//xingwei==1004 单机测试用
        net.rec_targety=600;//200 ;//200 //600//Catch_robot_f_y;	//xingwei==1004 单机测试用
        net.rec_ctrl=CTRL_CATCH_FOCUS;net.rec_targetx=300;//1350;//Catch_robot_f_x;				//xingwei==1004
    单机测试用 net.rec_targety=600;//200 ;//200 //600//Catch_robot_f_y;	//xingwei==1004 单机测试用
        net.rec_ctrl=CTRL_CATCH_FOCUS;
    }
    */
    /*
    if (xingwei==107)
    {
        net.rec_Def_Dist=200;
        net.rec_Def_Ang =1;//
    }

    if (xingwei==1)	//用行为1测试1004接定点传球(x300,y600)点	//2014test+ 行为1004为教练机发送过来的球已经传出的信号
    {
        xingwei=1001;
        net.rec_targetx=1600;//1350;
        net.rec_targety=600;//200 ;
        net.rec_ctrl=CTRL_PASS_MOVE;
    }
    */

    /*
    if (xingwei==1)	//用行为1测试1004接定点传球(x300,y600)点	//2014test+ 行为1004为教练机发送过来的球已经传出的信号
    {
        //xingwei= 1004;
        net.rec_targetx=300;//1350;//Catch_robot_f_x;				//xingwei==1004 单机测试用
        net.rec_targety=600;//200 ;//200 //600//Catch_robot_f_y;	//xingwei==1004 单机测试用
        net.rec_ctrl=CTRL_CATCH_FOCUS;
    }

    */
    // xingwei=2;//防守 追球xingwei=2;//防守防高球
    // pp1.use_film_line=1;//设置路径规划考虑场地边线
    // xingwei=101;//防守 追球
    // xingwei=110;//接球追球
    // xingwei=101;//防守 追球
    // xingwei=2;//进攻
    // xingwei=30;//
    // xingwei=107;net.rec_Def_Dist=200;net.rec_Def_Ang=90;//为防守角度//
    // pp1.add_obstacle_point(y.quanbx,y.quanby,3);//设置规避球   注意到实际场地可能因为场地分辨率太小规避总规避得太大

    int Emu_moto_L = 0;
    int Emu_moto_R = 0;
    int Emu_moto_B = 0;
    char xingwei_catch_flag = 0;
    char xingwei_passing = 0;

    if (xingwei == 11)
    {
        xingwei_catch_timer = 0; //
        catch_xingwei_P = 1002;  // 接球队员最后执行的是接球 还是接球跑位 默认1002接球跑位
    }
    if ((xingwei == 1002) || (xingwei == 1004))
    {
        if ((catch_xingwei_P == 1002) && (xingwei == 1004))
            xingwei_passing = 1;
        catch_xingwei_P = xingwei;
        xingwei_catch_timer = 120; // 80;//45;
    }
    else
    {
        if (xingwei_catch_timer > 0)
        {
            xingwei_catch_timer--;
            if (catch_xingwei_P == 1004)
            {
                xingwei = catch_xingwei_P;
            }
            if (catch_xingwei_P == 1002)
            {
                xingwei = catch_xingwei_P;
            }
        }
    }
    if (xingwei == 1004) // 行为1004为教练机发送过来的球已经传出的信号
    {                    // 如果收到1004球的坐标就不是当前球的坐标而是球的发出点坐标
                         // 并且球在本机视野外则向教练机发来的接球位置跑位
                         //	  球在本机视野内则开始接球
                         //	  球过中线也开始接球
        xingwei_catch_flag = 1;
        if ((jieqiuqs >= 1) || ((y.ball_zhong > 10 + 18) && (y_ball_chang_min < 210))) // 2014test+
        {
            xingwei = 30;
        }
        else
        {
            xingwei = 7; //
                         //		net.rec_targetx=200;//1350;//Catch_robot_f_x;				//xingwei==1004 单机测试用
                         //		net.rec_targety=600;//200 ;//200 //600//Catch_robot_f_y;	//xingwei==1004 单机测试用
                         //		y.quanbx=900;y.quanby=600;//								//xingwei==1004 单机测试用
            // tfa=angle_add_transfer(-net.rec_targetang,y.benjijiao);
            net.rec_targetang = -f2l_angle(y.benjix, y.benjiy, 0, y.quanbx, y.quanby);
        }
    }
    if (xingwei == 1002) // 初始0读对最小二乘的影响??????
    {
        xingwei_catch_flag = 1;
        if (0) //(y.quanbx>(900-50))  //cs2014g   //2012 passing
        {
            xingwei = 30;
            //		xingwei_Passing_flag=0;
        }
        else
        {
            xingwei = 7; //
                         //		net.rec_targetx=1350;//Catch_robot_f_x;
                         //		net.rec_targety=600 ;//200 //600//Catch_robot_f_y;
            // tfa=angle_add_transfer(-net.rec_targetang,y.benjijiao);
            net.rec_targetang = -f2l_angle(y.benjix, y.benjiy, 0, y.quanbx, y.quanby);
        }
    }

    char set_use_vision_location = USE_VISION_LOCATION; // 设置使用视觉定位做接球所有数据基础 1使用视觉 0使用码盘
#ifdef RUN_WITH_EMU
    set_use_vision_location = 1; // 如果使用仿真则没有码盘信息 所以只能用仿真的视觉定位信息
#endif
    set_use_vision_location = 1; // 如果使用仿真则没有码盘信息 所以只能用仿真的视觉定位信息
    if (set_use_vision_location)
    { // 使用视觉定位信息 做接球 兼容[20120620接球cs+xxx ] [xxx]
        move_Emu_robot_f_x = y.benjix;
        move_Emu_robot_f_y = y.benjiy;
        move_Emu_robot_f_a = y.benjijiao;
    }
    else
    { // 使用码盘定位信息 做接球 兼容[20120620接球cs+xxx ]

        if (1) //(0)// 1 使用 视觉 和码盘 做接球基准   0使用陀螺 码盘 做接球基准
        {
            if (xingwei != 30) //(xingwei==11)
            {
                Dribble_ball.robot_L_test.f_x = y.benjix * 0.01; // 6;//
                Dribble_ball.robot_L_test.f_y = y.benjiy * 0.01; // 3;//
                Dribble_ball.robot_L_test.f_a = -y.benjijiao;    // ypr[0]*180.0/3.14;//
                //	Dribble_ball.robot_L_test.f_a=0;
            }
            move_Emu_robot_f_x = Dribble_ball.robot_L_test.f_x * 100; // y.benjix;
            move_Emu_robot_f_y = Dribble_ball.robot_L_test.f_y * 100; // y.benjiy;
            move_Emu_robot_f_a = y.benjijiao;                         //-ypr[0]*180.0/3.14;//
            //	move_Emu_robot_f_a = -Dribble_ball.robot_L_test.f_a;//y.benjijiao;
        }
        else
        {
            if (xingwei != 30) //(xingwei==11)
            {
                Dribble_ball.robot_L_test.f_x = y.benjix * 0.01; // 6;
                Dribble_ball.robot_L_test.f_y = y.benjiy * 0.01; // 3;
                Dribble_ball.robot_L_test.f_a =
                    -angle_add_transfer(ypr[0] * 180.0 / 3.14, -GYRO_init_angle); // ypr[0]*180.0/3.14;//
                //	Dribble_ball.robot_L_test.f_a=0;
            }
            move_Emu_robot_f_x = Dribble_ball.robot_L_test.f_x * 100; // y.benjix;
            move_Emu_robot_f_y = Dribble_ball.robot_L_test.f_y * 100; // y.benjiy;
            move_Emu_robot_f_a = angle_add_transfer(ypr[0] * 180.0 / 3.14, -GYRO_init_angle);
        }
    }
    float move_Emu_ballA = ball_angle; // f2l_angle (y.benjix ,y.benjiy  ,-y.benjijiao,y.quanbx,y.quanby);
    double balljiao_temp = angle_add_transfer2(-y.ball_jiao - y.benjijiao + 90, 0);
    //	double	ballchang_temp=quxian[int(y_ball_chang_min)]/cdbl*2.7;//quxian[int(changdi->ball_chang)];
    double ballchang_temp = quxian[int(y_ball_chang_min)]; // quxian[int(changdi->ball_chang)];
    double ballchang_tempballchang_temp =
        (11.25 * (ballchang_temp + sqrt(ballchang_temp * ballchang_temp + 73 * 73)) / 73);
    ballchang_temp = ballchang_temp - ballchang_tempballchang_temp;

    if (set_use_vision_location) // cs2014g
    {
        ballchang_temp = ballchang_temp / cdbl * 2.7; //;
    }

    float move_Emu_ballR =
        ballchang_temp / 100.0; // ball_radius;//f2l_radius(y.benjix ,y.benjiy  ,-y.benjijiao,y.quanbx,y.quanby);

    double temp_y_quanbx =
        l2f_x(move_Emu_robot_f_x, move_Emu_robot_f_y, -move_Emu_robot_f_a, move_Emu_ballA, ballchang_temp);
    double temp_y_quanby =
        l2f_y(move_Emu_robot_f_x, move_Emu_robot_f_y, -move_Emu_robot_f_a, move_Emu_ballA, ballchang_temp);
    //	m_find.zhizuobiao(&temp_y_quanbx,&temp_y_quanby,balljiao_temp,ballchang_temp,y.benjix,y.benjiy);

    if (y.ball_zhong > 10)
    {
        move_Emu_ballX = temp_y_quanbx;
        move_Emu_ballY = temp_y_quanby;
        move_Emu_ballX_p = temp_y_quanbx;
        move_Emu_ballY_p = temp_y_quanby;
    }
    else
    {
        if (0) // diuqiuleijia>100)
        {
            move_Emu_ballX = move_Emu_ballX_p;
            move_Emu_ballY = move_Emu_ballY_p;
        }
        else
        {
            //	move_Emu_ballX=move_Emu_robot_f_x+(y.quanbx-y.benjix)*cdbl/2.7;
            //	move_Emu_ballY=move_Emu_robot_f_y+(y.quanby-y.benjiy)*cdbl/2.7;
            move_Emu_ballX = move_Emu_robot_f_x + (net.re_ballx - y.benjix) * cdbl / 2.7;
            move_Emu_ballY = move_Emu_robot_f_y + (net.re_bally - y.benjiy) * cdbl / 2.7;
        }
    }

    for (i = 100 - 1; i > 0; i--) // 上100帧周期码盘位置历史记录
    {
        move_Emu_ballX_h[i] = move_Emu_ballX_h[i - 1];
        move_Emu_ballY_h[i] = move_Emu_ballY_h[i - 1];
    }
    move_Emu_ballX_h[0] = move_Emu_ballX;
    move_Emu_ballY_h[0] = move_Emu_ballY;
    // 还应测量速度变化率
    int start_time = 0; // 记录 判定是从当前桢往前数多少桢开始生效的
    int xingwei30panding = 0;
    int step_long = 25;        // 15 单次测量间隔桢数
    int all_measure_time = 50; // 10 全部3次测量间隔桢数
    float angle_err = 20;      // 20 监测条件 球速度的角度 与 球朝向机器角度 的 偏差角
    float min_speed = 19;      // 30;		//10 检测条件 球移动速度 cm/s

    // xingwei30panding=catch_start_detection(angle_err,min_speed,all_measure_time,step_long,move_Emu_robot_f_x,move_Emu_robot_f_y,move_Emu_ballX_h,move_Emu_ballY_h);
    // if (xingwei30panding==1)
    //	start_time=all_measure_time+step_long;
    if (xingwei30panding == 0)
    {
        step_long = 15;        // 单次测量间隔桢数
        all_measure_time = 30; // 全部3次测量间隔桢数
        angle_err = 20;        // 监测条件 球速度的角度 与 球朝向机器角度 的 偏差角
        min_speed = 100;       // 30;		//检测条件 球移动速度 cm/s
        xingwei30panding = catch_start_detection(angle_err, min_speed, all_measure_time, step_long, move_Emu_robot_f_x,
                                                 move_Emu_robot_f_y, move_Emu_ballX_h, move_Emu_ballY_h);
        if (xingwei30panding == 1)
            start_time = all_measure_time + step_long;
    }
    if (xingwei30panding == 0)
    {
        step_long = 19;        // 单次测量间隔桢数
        all_measure_time = 10; // 全部3次测量间隔桢数
        angle_err = 20;        // 监测条件 球速度的角度 与 球朝向机器角度 的 偏差角
        min_speed = 300;       // 30;		//检测条件 球移动速度 cm/s
        xingwei30panding = catch_start_detection(angle_err, min_speed, all_measure_time, step_long, move_Emu_robot_f_x,
                                                 move_Emu_robot_f_y, move_Emu_ballX_h, move_Emu_ballY_h);
        if (xingwei30panding == 1)
            start_time = all_measure_time + step_long;
    }
    //(0)&&
    if (((xingwei30panding_jishu > 0) || (xingwei30panding == 1))
        //	&&((xingwei==7)||(xingwei==107)||(xingwei==1127)||(xingwei==1137)||(xingwei==1147))
        && ((xingwei == 1127) || (xingwei == 1137) || (xingwei == 1147)))
    {

        // if(y.ball_zhong>10 && jieqiuqs==0) //xingwei==30  切xingwei==1 服务程序
        if ((y.ball_zhong > 10) && (xingwei30panding_jishu == 0))
        {
            // jieqiuqs>=3 && ball__Sball_radius>start_catch_ball_move_dis)//xingwei==30  接球开始判定
            jieqiux = move_Emu_ballX_h[start_time]; // 2013
            jieqiuy = move_Emu_ballY_h[start_time];
            jieqiuqs = 3;
            jieqiuys = 0;
        }

        if (xingwei30panding == 1)
            xingwei30panding_jishu += 25;
        else
            xingwei30panding_jishu -= 1;
        if (xingwei30panding_jishu > 25)
            xingwei30panding_jishu = 25;
        if (xingwei30panding_jishu < 0)
            xingwei30panding_jishu = 0;

        xingwei = 30; // 2014
    }
    else
    {
        xingwei30panding_jishu = 0;
    }

    DebugV[4] = xingwei30panding;
    if (0) //(xingwei==30)//||(xingwei==1004))//2014g
    {
        if (y.ball_zhong > 10) //(1)//使用卡尔曼滤波器 对球的位置滤波
        {
            if ((kalman_init_flag == 1) && (USE_KALMAN_BALL == 1)) // 接球卡尔曼滤波器初始化
            {
                kalman_init_flag = 0;
                cvReleaseMat(&z_k_X);
                cvReleaseMat(&z_k_Y);

                z_k_X = cvCreateMat(1, 1, CV_32FC1);
                z_k_Y = cvCreateMat(1, 1, CV_32FC1);

                cvReleaseKalman(&kalman_X);
                kalman_X = cvCreateKalman(2, 1, 0);
                memcpy(kalman_X->transition_matrix->data.fl, F, sizeof(F));
                /*初始化转移矩阵，行列，具体见CvKalman* kalman = cvCreateKalman( 2, 1, 0 );*/ // Initialize other
                                                                                              // Kalman parameters
                cvSetIdentity(kalman_X->measurement_matrix, cvRealScalar(1));                 /*观测矩阵*/
                cvSetIdentity(kalman_X->process_noise_cov, cvRealScalar(1e-5));               /*过程噪声*/
                cvSetIdentity(kalman_X->measurement_noise_cov, cvRealScalar(1e-1));           //(1e-3) ); /*观测噪声*/
                cvSetIdentity(kalman_X->error_cov_post, cvRealScalar(1));
                /*后验误差协方差*/ // Choose random initial state

                cvReleaseKalman(&kalman_Y);
                kalman_Y = cvCreateKalman(2, 1, 0);
                memcpy(kalman_Y->transition_matrix->data.fl, F, sizeof(F));
                /*初始化转移矩阵，行列，具体见CvKalman* kalman = cvCreateKalman( 2, 1, 0 );*/ // Initialize other
                                                                                              // Kalman parameters
                cvSetIdentity(kalman_Y->measurement_matrix, cvRealScalar(1));                 /*观测矩阵*/
                cvSetIdentity(kalman_Y->process_noise_cov, cvRealScalar(1e-5));               /*过程噪声*/
                cvSetIdentity(kalman_Y->measurement_noise_cov, cvRealScalar(1e-1));           //(1e-3) ); /*观测噪声*/
                cvSetIdentity(kalman_Y->error_cov_post, cvRealScalar(1));
                /*后验误差协方差*/ // Choose random initial state

                kalman_X->state_post->data.fl[0] = move_Emu_ballX;
                kalman_Y->state_post->data.fl[0] = move_Emu_ballY;
                for (int i = 0; i < 80; i++) // 上100帧周期码盘位置历史记录
                {
                    catchballtempX[i] = move_Emu_ballX;
                    catchballtempY[i] = move_Emu_ballY;
                }
            }
            else
            {
                double ball_mensure_noise =
                    ballchang_temp * 0.00001 + 0.0001; // ballchang_temp*0.00005+0.0001;//ballchang_temp*0.0001+0.001;
                cvSetIdentity(kalman_X->measurement_noise_cov,
                              cvRealScalar(ball_mensure_noise)); //(1e-3) ); /*观测噪声*/
                cvSetIdentity(kalman_Y->measurement_noise_cov,
                              cvRealScalar(ball_mensure_noise)); //(1e-3) ); /*观测噪声*/

                double Kalman_ballC_d_f_x = 0;
                double Kalman_ballC_d_f_y = 0;

                z_k_X->data.fl[0] = move_Emu_ballX;
                const CvMat *y_k_X = cvKalmanPredict(kalman_X, 0); /*激励项输入为*/ // Generate Measurement (z_k)
                cvKalmanCorrect(kalman_X, z_k_X); // Apply the transition matrix F and apply"process noise" w_k
                Kalman_ballC_d_f_x = y_k_X->data.fl[0];

                z_k_Y->data.fl[0] = move_Emu_ballY;
                const CvMat *y_k_Y = cvKalmanPredict(kalman_Y, 0); /*激励项输入为*/ // Generate Measurement (z_k)
                cvKalmanCorrect(kalman_Y, z_k_Y); // Apply the transition matrix F and apply"process noise" w_k
                Kalman_ballC_d_f_y = y_k_Y->data.fl[0];
                m_find.pField->SetBall(4, Kalman_ballC_d_f_x, Kalman_ballC_d_f_y + 200);
                move_Emu_ballX = Kalman_ballC_d_f_x;
                move_Emu_ballY = Kalman_ballC_d_f_y;
            }
        }

        for (int i = 80 - 1; i > 0; i--) // 上100帧周期码盘位置历史记录
        {
            catchballtempX[i] = catchballtempX[i - 1];
            catchballtempY[i] = catchballtempY[i - 1];
        }
        catchballtempX[0] = move_Emu_ballX;
        catchballtempY[0] = move_Emu_ballY;
        for (int z = 0; z < 80; z++)
            m_find.pField->SetBall(16 + z, catchballtempX[z], catchballtempY[z]);
    }
    else
    {
        kalman_init_flag = 1;

        for (int z = 0; z < 80; z++)
            m_find.pField->SetBall(16 + z, catchballtempX[z], catchballtempY[z]);
    }

    //	m_find.pField->SetBall(15,move_Emu_ballX ,move_Emu_ballY);//(15,move_Emu_ballX*100 ,move_Emu_ballY*100);
    //	m_find.pField->SetAgent(15 ,move_Emu_robot_f_x,move_Emu_robot_f_y   ,move_Emu_robot_f_a);//(15
    //,move_Emu_robot_f_x,move_Emu_robot_f_y   ,move_Emu_robot_f_y);//场子正中向上  逆时针坐标系

    if (xingwei == 1004)          // 行为1004为教练机发送过来的球已经传出的信号
    {                             // 如果收到1004球的坐标就不是当前球的坐标而是球的发出点坐标
                                  // 并且球在本机视野外则向教练机发来的接球位置跑位
                                  //	  球在本机视野内则开始接球
                                  //	  球过中线也开始接球
        if (xingwei_passing == 1) // if (((catch_xingwei_P==1002)&&(xingwei==1004))  ==ture
        {
            //		move_Emu_ballA=f2l_angle (y.benjix ,y.benjiy
            //,-y.benjijiao,net.re_ballx,net.re_bally);//y.quanbx,y.quanby);//net.rec_fromx,net.rec_fromy);
            //		move_Emu_ballR=f2l_radius(y.benjix ,y.benjiy
            //,-y.benjijiao,net.re_ballx,net.re_bally);//y.quanbx,y.quanby);//net.rec_fromx,net.rec_fromy);
            //		move_Emu_ballX=l2f_x     (move_Emu_robot_f_x ,move_Emu_robot_f_y
            //,move_Emu_robot_f_a,move_Emu_ballA,move_Emu_ballR); 		move_Emu_ballY=l2f_y     (move_Emu_robot_f_x
            //,move_Emu_robot_f_y  ,move_Emu_robot_f_a,move_Emu_ballA,move_Emu_ballR);
            move_Emu_ballSX = move_Emu_ballX; // 码盘定位 坐标系球起始位置
            move_Emu_ballSY = move_Emu_ballY;
            move_Net_ballf_SX = net.re_ballx; // 球视觉全局坐标起始位置
            move_Net_ballf_SY = net.re_bally;
        }
    }

    if (0) //(xingwei==11)||(xingwei==7))
    {      // 接球服务初始化代码
        if (y.ball_zhong < 10)
        {
            move_Emu_ballA = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, net.re_ballx,
                                       net.re_bally); // net.rec_fromx,net.rec_fromy);
            move_Emu_ballR = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, net.re_ballx,
                                        net.re_bally); // net.rec_fromx,net.rec_fromy);
            move_Emu_ballX =
                l2f_x(move_Emu_robot_f_x, move_Emu_robot_f_y, move_Emu_robot_f_a, move_Emu_ballA, move_Emu_ballR);
            move_Emu_ballY =
                l2f_y(move_Emu_robot_f_x, move_Emu_robot_f_y, move_Emu_robot_f_a, move_Emu_ballA, move_Emu_ballR);
        }
        move_Emu_ballSX = move_Emu_ballX;
        move_Emu_ballSY = move_Emu_ballY;
        move_Emu_robot_Sf_x = move_Emu_robot_f_x;
        move_Emu_robot_Sf_y = move_Emu_robot_f_y;
        move_Emu_robot_Sf_a = move_Emu_robot_f_a;
        rb_dis_p = 0;
        v_radius_set = 0;
        for (i = 0; i < 20; i++)
        {
            t_radius1_h[i] = 0;
            t_radius2_h[i] = 0;
            ball_f_x_h[i] = move_Emu_ballSX;
            ball_f_y_h[i] = move_Emu_ballSY;
            ball_f_hw[i] = 0;
        }

        /*		ballC_sx=y.quanbx*0.01;
                ballC_sy=y.quanby*0.01;
                rb_dis_p=0;
                v_radius_set=0;
                move_test_C.CMovement2Control::CMovement2Control();
                for (i=0;i<20;i++)
                {
                    t_radius1_h[i]=0;
                    t_radius2_h[i]=0;
                    ball_f_x_h [i]=ballC_sx;
                    ball_f_y_h [i]=ballC_sy;
                    ball_f_hw  [i]=0;
                }
        */		//t1 ping yi ping jun
    }

    if (xingwei != 30) // && xingwei!=1 )// && xingwei!=1001)//2014 ??????
    {
        jieqiuys = 0;
        jieqiuys_2 = 0;
    }
    if (xingwei == 30) // 2012 pan ding 0)//0)//
    {

        if (y.ball_zhong > 10 + 18 && y_ball_chang_min < 210)
        {
            if (jieqiuqs >= 1)
            {
                jieqiuqs++;
            }
            if (jieqiuqs == 0)
            {
                jieqiux = move_Emu_ballX; // 2013
                jieqiuy = move_Emu_ballY;
                //	jieqiux=y.quanbx;//2012
                //	jieqiuy=y.quanby;
                jdx = jieqiux;
                jdy = jieqiuy; // 2013修改
                jieqiuqs = 1;

                if (1) //(xingwei==11)//(xingwei==11)
                {
                    robot_catch_ball.robot_emu.f_x = y.benjix * 0.01; //
                    robot_catch_ball.robot_emu.f_y = y.benjiy * 0.01; //
                    robot_catch_ball.robot_emu.f_a = -y.benjijiao;    // ypr[0]*180.0/3.14;//
                }
            }
        }

        // float ball__Sball_radius;
        //	ball__Sball_radius= f2l_radius(jieqiux,jieqiuy,0, y.quanbx,y.quanby);
        float ball__Sball_radius = f2l_radius(jieqiux, jieqiuy, 0, move_Emu_ballX, move_Emu_ballY);
        float robot_Sball_radius = f2l_radius(jieqiux, jieqiuy, 0, move_Emu_robot_f_x, move_Emu_robot_f_y);
        DebugV[0] = jieqiux;
        DebugV[1] = jieqiuy;
        // 依据发球点到本机的距离 生成球距离发球点多少距离开始接球
        //	float start_catch_ball_move_dis = robot_Sball_radius * 0.5;//开始接球的距离为 球移动
        // 出发球点到本机距离的一半时再动 	if (start_catch_ball_move_dis<80) 		start_catch_ball_move_dis=80; 	if
        //(start_catch_ball_move_dis>200) 		start_catch_ball_move_dis=200;
        float start_catch_ball_move_dis =
            robot_Sball_radius * 0.16 * 0.01; // 开始接球的距离为 球移动 出发球点到本机距离的一半时再动
        if (start_catch_ball_move_dis < 0.6)
            start_catch_ball_move_dis = 0.6;
        if (start_catch_ball_move_dis > 1.8)
            start_catch_ball_move_dis = 1.8;
        start_catch_ball_move_dis = start_catch_ball_move_dis * 100;
        DebugV[2] = start_catch_ball_move_dis;
        DebugV[3] = ball__Sball_radius;
        ///*	//2014test	jieqiuys接球自动切xingwei==1超时数值调整
        int next_xingwei = 1; //
        if (jieqiuys < 170 * 2)
        {
            if (y.ball_zhong > 10)
            {
                if (jieqiuys > 100 * 2) // 2.0s
                    xingwei = next_xingwei;
                else
                    xingwei = 30;
            }
            else
            {
                if (jieqiuys > 160 * 2) // 4.0s
                    xingwei = next_xingwei;
                else
                    xingwei = 30;
            }
        }
        else
        {
            xingwei = next_xingwei;
        }
        //*/	//2014test
        if (xingwei == 30) //(y.ball_zhong>10)
        {
            y.quanbx = move_Emu_ballX;
            y.quanby = move_Emu_ballY;
            // y.benjix=move_Emu_robot_f_x;
            // y.benjiy=move_Emu_robot_f_y;
            // y.benjijiao=y.benjijiao;
        }
        //	m_find.pField->SetBall(18,jieqiux ,jieqiuy);//(15,move_Emu_ballX*100 ,move_Emu_ballY*100);
    }

    if (0) // xingwei==30)//!!!!!0)//
    {
        jieqiuys++;

        //	obj_properties ballC_d ;
        //	ballC_d .f_x = y.quanbx*0.01;
        //	ballC_d .f_y = y.quanby*0.01;
        float ball__Sball_radius;
        if (y.ball_zhong > 10)
        {
            ball__Sball_radius = f2l_radius(move_Net_ballf_SX, move_Net_ballf_SY, 0, y.quanbx, y.quanby) * 0.01;
        }
        else
        {
            ball__Sball_radius = 0;
        }

        if (0) // 2011现在版本接球
        {
            if (ball__Sball_radius < 1.0)
            {
                jieqiuys_2 = jieqiuys;
            }

            if (jieqiuys > jieqiuys_2 + 45)
            {
                jieqiuys = 158;
                net.toS_status = CTRL_ATTACK;
                xingwei = 1;
            }
        }
        if (0) // 2010版本接球
        {
            if (ball__Sball_radius < 0.4) // 如果球的距离距 球开始位置ballC_sx,ballC_sy 小于0.4 m
            {
                jieqiuys_2 = jieqiuys; // jieqiuys_2 纪录 球离开开始位置0.4 m的时间
            }

            if (jieqiuys > jieqiuys_2 + 10) //??????应该是切158时间在后面由接球时间超时判定判断切 行为1 用行为1内置接球
                                            // 但是在此判定时间前应该让机器停止不应该执行行为30
            {
                jieqiuys = 158;
                net.toS_status = CTRL_STOP;
                xingwei = 11;
            }
        }
        if (xingwei_catch_flag == 1) // 2012版本接球
        {
            if (ball__Sball_radius < 0.4) // 如果球的距离距 球开始位置ballC_sx,ballC_sy 小于0.4 m
            {
                jieqiuys_2 = jieqiuys; // jieqiuys_2 纪录 球离开开始位置0.4 m的时间
            }

            if (jieqiuys > jieqiuys_2) // 在切换
            {
                net.toS_status = CTRL_PASS;
                xingwei = 30;
            }
            else
            {
                net.toS_status = CTRL_STOP;
                xingwei = 11;
            }
            if (jieqiuys > 68) // 如果接球超时68周期 1.5s 则转进攻
            {
                jieqiuys = 158;
                net.toS_status = CTRL_ATTACK;
                xingwei = 1;
            }

            if (jieqiuys < 0)
            {
                jieqiuys = 0;
            }
        }
        else
        {                                 // 2010版本接球
            if (ball__Sball_radius < 0.4) // 如果球的距离距 球开始位置ballC_sx,ballC_sy 小于0.4 m
            {
                jieqiuys_2 = jieqiuys; // jieqiuys_2 纪录 球离开开始位置0.4 m的时间
            }

            if (jieqiuys > jieqiuys_2 + 10) //??????应该是切158时间在后面由接球时间超时判定判断切 行为1 用行为1内置接球
                                            // 但是在此判定时间前应该让机器停止不应该执行行为30
            {
                jieqiuys = 158;
                net.toS_status = CTRL_STOP;
                xingwei = 11;
            }
            if (jieqiuys > 150) // 如果接球超时150周期 3.3s 则转进攻
            {
                jieqiuys = 158;
                net.toS_status = CTRL_ATTACK;
                xingwei = 1;
            }

            if (jieqiuys < 0)
            {
                jieqiuys = 0;
            }
        }
        mtl = 0;
        mtr = 0;
        mtb = 0;
        zhuan = 0;
    }

    if (xingwei == 25)
    {
        double dingweijiao;
        double dingweichang;
        double jiao;
        double chang;
        double ballx;
        double bally;
        double ballx_s;
        double bally_s;
        if (y.ball_zhong > 10 && kaiqiubk == 0)
        {
            kaiqiubk = 1;
            kaiqiubx = y.quanbx;
            kaiqiuby = y.quanby;
            m_find.jizuobiao(kaiqiubx, kaiqiuby, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
            kaiqiudj = dingweijiao;
        }
        if (kaiqiubk == 1)
        {
            kaiqiusj++;
            motospeed[3] = 16000;  //
            motospeed[4] = -16000; //
        }
        //	if(kaiqiusj>200 ||	m_pAD[0]>1000 && m_pAD[1]>1000 && y.ball_chang<50 )
        //	{
        //		xingwei=1;
        //	}

        dingweijiao = -jiajiao(-kaiqiudj, y.benjijiao - 90);
        // 根据教练机给的球的位置,将这个坐标投影到视觉
        jiao = dingweijiao;
        chang = fanquxian[int(dingweichang * cdbl / 2.7)];
        zhuan = 3 * duizhun(dingweijiao, 0, 1000);
        if (abs(zhuan) > abs(zhuan_shang))
        {
            if (zhuan - zhuan_shang > 150)
                zhuan = zhuan_shang + 150;
            if (zhuan - zhuan_shang < -150)
                zhuan = zhuan_shang - 150;
        }

        m_find.jizuobiao(y.ball_x, y.ball_y, &jiao, &chang, 320, 240);
        chang = quxian[int(chang)];
        m_find.zhizuobiao(&ballx, &bally, jiao, chang, 320, 240);
        m_find.jizuobiao(ballx_s1, bally_s1, &jiao, &chang, 320, 240);
        chang = quxian[int(chang)];
        m_find.zhizuobiao(&ballx_s, &bally_s, jiao, chang, 320, 240);
        kaiqiujfx = kaiqiujfx + (ballx - ballx_s) * 100;
        kaiqiujfx = kaiqiujfx + (ballx - 320) * 10;
        //	kaiqiujfx=(ballx-320)*200;
        kaiqiujfy = kaiqiujfy + (bally - bally_s) * 5;
        Controlmoto(y.ball_jiao, 1000, 3000, &mtl, &mtr, &mtb);
        Controlmoto(90, kaiqiujfx, 5000, &mtl, &mtr, &mtb);
        Controlmoto(0, kaiqiujfy, 10000, &mtl, &mtr, &mtb);
    }
    else
    {
        kaiqiujfx = 0;
        kaiqiujfy = 0;
        kaiqiusj = 0;
        kaiqiubk = 0;
    }
    if (xingwei == 1117) // 自转加平移测试机体定位系统
    {
        float tfx = net.rec_targetx;
        float tfy = net.rec_targety;
        float tfa = angle_add_transfer(-net.rec_targetang, y.benjijiao); //-net.rec_targetang;

        float mudi_angle = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, net.rec_targetx, net.rec_targety);
        float mudi_radius = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, net.rec_targetx, net.rec_targety) * 0.01;

        mtl = 0;
        mtr = 0;
        mtb = 0;
        float mb, ml, mr;
        Dribble_ball.move2Control.limit_parameter_set_dis(
            30, 8); //(zs , 10);//(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
        Dribble_ball.move2Control.initializing_movement(20 * xingwei_1117_rotate_direct_test, mudi_radius * 6,
                                                        mudi_angle); //
        Dribble_ball.move2Control.front_v_maxlimit();
        Dribble_ball.move2Control.uptech_output_O(&mb, &ml, &mr);

        float p = 1;
        mtb = mb * p;
        mtl = ml * p;
        mtr = mr *
              p; // 设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000 = 2转/秒
        motospeed[0] = mtl;
        motospeed[1] = mtr;
        motospeed[2] = mtb;
        zhuan = 0; // 鑫鑫程序里的转动变量设置成0
    }

    int force_xingwei_107_quanbx_y_flag = 0;
    double set_xingwei_107_quanbx = 0;
    double set_xingwei_107_quanby = 0;
    double set_xingwei_107_temp_quanbx = 0;
    double set_xingwei_107_temp_quanby = 0;

    int force_xingwei_107_target_flag = 0;
    double set_xingwei_107_tfa = 0;
    double set_xingwei_107_tfx = 0;
    double set_xingwei_107_tfy = 0;

    int force_xingwei_7_target_flag = 0;
    double set_xingwei_7_tfa = 0;
    double set_xingwei_7_tfx = 0;
    double set_xingwei_7_tfy = 0;

    int temp_xingwei_num = 0;

    if (xingwei == 107) // 跟踪球与己方门之间
    {
        if (y.ball_zhong < 10)
        {
            net.rec_CDGtargetx = y.quanbx;
            net.rec_CDGtargety = y.quanby;
            temp_xingwei_num = xingwei;
            xingwei = 1147;
        }
    }
    if (0) // xingwei==1)//跟踪球消失位置障碍物
    {
        if ((y.ball_zhong < 10) && (diuqiuleijia <= 0))
        {
            net.rec_CFtargetx = y.quanbx;
            net.rec_CFtargety = y.quanby;
            temp_xingwei_num = xingwei;
            xingwei = 1127;
        }
    }
    if ((xingwei == 1127) || (xingwei == 1137) || (xingwei == 1147) ||
        (xingwei == 1)) // 测试障碍物追踪 由行为7 xy目的坐标 指定障碍物
    {
        int temp_x = net.rec_CFtargetx;
        int temp_y = net.rec_CFtargety;
        int temp_a = net.rec_CFtargetang;
        switch (net.rec_ctrl)
        {
        case CTRL_FOLLOW:
            temp_x = net.rec_CFtargetx;
            temp_y = net.rec_CFtargety;
            temp_a = net.rec_CFtargetang;
            break;
        case CTRL_DEF_BALL:
            temp_x = net.rec_CDBtargetx;
            temp_y = net.rec_CDBtargety;
            temp_a = net.rec_CDBtargetang;
            break;
        case CTRL_DEF_GOAL:
            temp_x = net.rec_CDGtargetx;
            temp_y = net.rec_CDGtargety;
            temp_a = net.rec_CDGtargetang;
            break;
        case CTRL_ATTACK:
            temp_x = y.quanbx;
            temp_y = y.quanby;
            temp_a = 0;
            break;
        default:
            break;
        }
        switch (xingwei)
        {
        case 1127: // CTRL_FOLLOW:
            temp_x = net.rec_CFtargetx;
            temp_y = net.rec_CFtargety;
            temp_a = net.rec_CFtargetang;
            break;
        case 1137: // CTRL_DEF_BALL:
            temp_x = net.rec_CDBtargetx;
            temp_y = net.rec_CDBtargety;
            temp_a = net.rec_CDBtargetang;
            break;
        case 1147: // CTRL_DEF_GOAL:
            temp_x = net.rec_CDGtargetx;
            temp_y = net.rec_CDGtargety;
            temp_a = net.rec_CDGtargetang;
            break;
        case 1:
            temp_x = y.quanbx;
            temp_y = y.quanby;
            temp_a = 0;
            break;
        default:
            break;
        }

        if ((set_track_OBJ_X != temp_x) || (set_track_OBJ_Y != temp_y))
        {
            set_track_OBJ_X = temp_x;
            set_track_OBJ_Y = temp_y;
            track_black_OBJ_map_X[0] = set_track_OBJ_X;
            track_black_OBJ_map_Y[0] = set_track_OBJ_Y;
        }
        //	if ((set_track_OBJ_X!=net.rec_targetx)||(set_track_OBJ_Y!=net.rec_targety))
        //	{
        //		set_track_OBJ_X=net.rec_targetx;
        //		set_track_OBJ_Y=net.rec_targety;
        //		track_black_OBJ_map_X[0]=set_track_OBJ_X;
        //		track_black_OBJ_map_Y[0]=set_track_OBJ_Y;
        //	}
    }
    if (1)
    {
        float track_dis_P2C = 0; // f2l_radius(y.benjix,y.benjiy,-y.benjijiao,net.rec_targetx,net.rec_targety);
        float temp_OBJ_dis = 0;
        float temp_OBJ_angle = 0;
        float min_dis_OBJ_dis = 150;
        float min_dis_OBJ_x = 0;
        float min_dis_OBJ_y = 0;
        for (i = 0; i < black_OBJ_L; i++)
        {
            double xxt, yyt;
            if (black_OBJ_R[i] > 210)
                continue;

            temp_OBJ_angle = -black_OBJ_A[i] - y.benjijiao + 90; // angle_add_transfer(-Learn_A[i],y.benjijiao);
            temp_OBJ_dis = quxian[int(black_OBJ_R[i])] / cdbl * 2.7;
            m_find.zhizuobiao(&xxt, &yyt, temp_OBJ_angle, temp_OBJ_dis, y.benjix, y.benjiy);
            //	m_find.zhizuobiao(&ballx,&bally,Learn_A[i],Learn_R[i],320,240);
            int xt = xxt; // l2f_x (y.benjix,y.benjiy,-y.benjijiao,-temp_OBJ_angle,temp_OBJ_dis);;
            int yt = yyt; // l2f_y (y.benjix,y.benjiy,-y.benjijiao,-temp_OBJ_angle,temp_OBJ_dis);;
            //	/*调试程序*/m_find.pField->SetBall(i+10, xt, yt);
            if (min_dis_OBJ_dis > f2l_radius(track_black_OBJ_map_X[0], track_black_OBJ_map_Y[0], 0, xt, yt))
            {
                min_dis_OBJ_x = xt;
                min_dis_OBJ_y = yt;
                min_dis_OBJ_dis = f2l_radius(track_black_OBJ_map_X[0], track_black_OBJ_map_Y[0], 0, xt, yt);
            }
        }
        if (min_dis_OBJ_dis < 150)
        {
            float OBJ_max_speed = 500.0 / 45.0;
            if (min_dis_OBJ_dis < OBJ_max_speed) // 如果被跟踪目标速度超过5m/s 认为跟踪错误 做加速度限制
            {
                track_black_OBJ_map_X[0] = min_dis_OBJ_x;
                track_black_OBJ_map_Y[0] = min_dis_OBJ_y;
            }
            else
            {
                float temp_angle =
                    f2l_angle(track_black_OBJ_map_X[0], track_black_OBJ_map_Y[0], 0, min_dis_OBJ_x, min_dis_OBJ_y);
                track_black_OBJ_map_X[0] =
                    l2f_x(track_black_OBJ_map_X[0], track_black_OBJ_map_Y[0], 0, temp_angle, OBJ_max_speed);
                track_black_OBJ_map_Y[0] =
                    l2f_y(track_black_OBJ_map_X[0], track_black_OBJ_map_Y[0], 0, temp_angle, OBJ_max_speed);
            }
            //		m_find.pField->SetBall(111, track_black_OBJ_map_X[0], track_black_OBJ_map_Y[0]);
        }
    }

    for (i = 100 - 1; i > 0; i--) // 上100帧周期电机发送数据历史记录
    {
        y_quanbx_P[i] = y_quanbx_P[i - 1];
        y_quanby_P[i] = y_quanby_P[i - 1];
    }
    y_quanbx_P[0] = y.quanbx;
    y_quanby_P[0] = y.quanby;

    // 禁区内球消失处理应该往障碍和门之间跻
    if ((xingwei == 1) || (xingwei == 1001) && (y_quanbx_P[19] == y_quanbx_P[0] &&
                                                y_quanby_P[19] == y_quanby_P[0])) // 跟踪球消失位置障碍物
    {
        if (y.quanbx <= 300 && y.quanby >= (650 - 350) && y.quanby <= (600 + 350))
        {
            ;
        }
        else
        {
            if ((y.ball_zhong < 10) && (diuqiuleijia <= 0))
            {
                double coil_direct = 1; // 绕障碍物方向 1顺时针 -1逆时针
                if (track_black_OBJ_map_Y[0] > 600)
                    coil_direct = -1;
                double tempangle = atan2(track_black_OBJ_map_X[0] - y.benjix, track_black_OBJ_map_Y[0] - y.benjiy);
                //		y.quanbx=track_black_OBJ_map_X[0]-100*sin(tempangle+coil_direct*3.14*0.4);//track_black_OBJ_map_X[0]*2-y.benjix;
                //		y.quanby=track_black_OBJ_map_Y[0]-100*cos(tempangle+coil_direct*3.14*0.4);//track_black_OBJ_map_Y[0]*2-y.benjiy;
                y.quanbx = track_black_OBJ_map_X[0] -
                           60 * sin(tempangle + coil_direct * 3.14 * 0.3); // 70 cm绕对方机体距离 0.3 是绕障碍偏转角
                y.quanby = track_black_OBJ_map_Y[0] - 60 * cos(tempangle + coil_direct * 3.14 * 0.3);
                double dingweijiao, dingweichang;
                m_find.jizuobiao(y.quanbx, y.quanby, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
                dingweijiao = -jiajiao(-dingweijiao, y.benjijiao - 90);
                // 根据教练机给的球的位置,将这个坐标投影到视觉
                y.ball_jiao = dingweijiao;
                y.ball_chang = fanquxian[int(dingweichang * cdbl / 2.7)];
                dingweichang = y.ball_chang;
                //		m_find.pField->SetBall(1111, y.quanbx, y.quanby);
            }
        }
    }

    if (xingwei == 1127) // 测试障碍物追踪 由行为7 xy目的坐标 指定障碍物
    {
        xingwei = temp_xingwei_num;
        float tfx = net.rec_targetx;
        float tfy = net.rec_targety;
        float tfa = angle_add_transfer(-net.rec_targetang, y.benjijiao); //-net.rec_targetang;

        float mudi_angle =
            f2l_angle(y.benjix, y.benjiy, -y.benjijiao, track_black_OBJ_map_X[0], track_black_OBJ_map_Y[0]);
        float mudi_radius =
            f2l_radius(y.benjix, y.benjiy, -y.benjijiao, track_black_OBJ_map_X[0], track_black_OBJ_map_Y[0]) * 0.01;

        float temp_f_v = (mudi_radius - 1) * 8;
        if (temp_f_v < 0)
            temp_f_v = 0;
        if (temp_f_v > 10)
            temp_f_v = 10;

        mtl = 0;
        mtr = 0;
        mtb = 0;
        float mb, ml, mr;
        Dribble_ball.move2Control.limit_parameter_set_dis(
            18, 8); //(zs , 10);//(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
        Dribble_ball.move2Control.initializing_movement(mudi_angle * 0.1, temp_f_v,
                                                        mudi_angle); //(mudi_angle*0.1 ,mudi_radius*6,mudi_angle);//
        Dribble_ball.move2Control.front_v_maxlimit();
        Dribble_ball.move2Control.uptech_output_O(&mb, &ml, &mr);

        float p = 1;
        mtb = mb * p;
        mtl = ml * p;
        mtr = mr *
              p; // 设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000 = 2转/秒
        motospeed[0] = mtl;
        motospeed[1] = mtr;
        motospeed[2] = mtb;
        zhuan = 0; // 鑫鑫程序里的转动变量设置成0

        if (0)
        {
            // 2013  force_xingwei_7_target
            set_xingwei_7_tfa = mudi_angle;
            set_xingwei_7_tfx = track_black_OBJ_map_X[0];
            set_xingwei_7_tfy = track_black_OBJ_map_Y[0];
            force_xingwei_7_target_flag = 1;
            temp_xingwei_num = xingwei;
            xingwei = 7;
            // 2013  force_xingwei_7_target
        }
        if (1)
        {
            // 2013  force_xingwei_107_target
            set_xingwei_107_tfa = mudi_angle;
            set_xingwei_107_tfx = track_black_OBJ_map_X[0];
            set_xingwei_107_tfy = track_black_OBJ_map_Y[0];
            force_xingwei_107_target_flag = 1;
            temp_xingwei_num = xingwei;
            xingwei = 107;
        }
    }
    if (xingwei == 1137) // 跟踪球与指定机体之间
    {
        float def_base_X = y.quanbx;
        float def_base_Y = y.quanby;
        float def_OBJ_X = track_black_OBJ_map_X[0];
        float def_OBJ_Y = track_black_OBJ_map_Y[0];

        // net.rec_Def_Dist=200;net.rec_Def_Ang=90;//
        float tfx = net.rec_targetx;
        float tfy = net.rec_targety;
        float tfa = angle_add_transfer(-net.rec_targetang, y.benjijiao); //-net.rec_targetang;

        float obj_base_angle = f2l_angle(def_OBJ_X, def_OBJ_Y, 0, def_base_X, def_base_Y);
        float obj_base_radius = f2l_radius(def_OBJ_X, def_OBJ_Y, 0, def_base_X, def_base_Y);

        float obj_def_radius = 130;

        float t_fx =
            l2f_x(def_OBJ_X, def_OBJ_Y, 0, obj_base_angle, obj_def_radius); // 估算后 球对地面的位置 在地面坐标系里
        float t_fy = l2f_y(def_OBJ_X, def_OBJ_Y, 0, obj_base_angle, obj_def_radius);
        m_find.pField->SetBall(6, t_fx, t_fy);

        float temp_tA = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, t_fx, t_fy);
        float temp_tR = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, t_fx, t_fy);

        float temp_OBJ_A = angle_add_transfer2(f2l_angle(y.benjix, y.benjiy, -y.benjijiao, def_OBJ_X, def_OBJ_Y), 180);
        float temp_OBJ_R = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, def_OBJ_X, def_OBJ_Y);

        float temp_f_v = (temp_tR) * 1.0; // 1.6;
        if (temp_f_v < 0)
            temp_f_v = 0;
        //	if (temp_f_v>10)
        //		temp_f_v=10;

        mtl = 0;
        mtr = 0;
        mtb = 0;
        float mb, ml, mr;
        Dribble_ball.move2Control.limit_parameter_set_dis(
            25, 8); //(zs , 10);//(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
        Dribble_ball.move2Control.initializing_movement(temp_OBJ_A * 0.1, temp_f_v,
                                                        temp_tA); //(mudi_angle*0.1 ,mudi_radius*6,mudi_angle);//
        Dribble_ball.move2Control.front_v_maxlimit();
        Dribble_ball.move2Control.uptech_output_O(&mb, &ml, &mr);

        float p = 1;
        mtb = mb * p;
        mtl = ml * p;
        mtr = mr *
              p; // 设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000 = 2转/秒
        motospeed[0] = mtl;
        motospeed[1] = mtr;
        motospeed[2] = mtb;
        zhuan = 0; // 鑫鑫程序里的转动变量设置成0
        //		DebugV[5]=Dribble_ball.RMfv*cos(angle_add_transfer2(Dribble_ball.RMfa,0)*3.14/180.0)*5100;
        //		DebugV[6]=mtl/0.866;

        // 2013  force_xingwei_7_target
        if ((t_fy > 600 - 325) && (t_fy < 600 + 325) && (t_fx < 225))
            t_fx = 225;
        if (0)
        {
            // 2013  force_xingwei_7_target
            set_xingwei_7_tfa = temp_OBJ_A;
            set_xingwei_7_tfx = t_fx;
            set_xingwei_7_tfy = t_fy;
            force_xingwei_7_target_flag = 1;
            temp_xingwei_num = xingwei;
            xingwei = 7;
        }
        if (1)
        {
            // 2013  force_xingwei_107_target
            set_xingwei_107_tfa = temp_OBJ_A;
            set_xingwei_107_tfx = t_fx;
            set_xingwei_107_tfy = t_fy;
            force_xingwei_107_target_flag = 1;
            temp_xingwei_num = xingwei;
            xingwei = 107;
        }
    }

    // 跟踪球与己方门之间
    if (xingwei == 1147) // 跟踪球与己方门之间
    {
        xingwei = temp_xingwei_num;
        float def_base_X = 0;   // y.quanbx;
        float def_base_Y = 600; // y.quanby;
        float def_OBJ_X = track_black_OBJ_map_X[0];
        float def_OBJ_Y = track_black_OBJ_map_Y[0];

        // net.rec_Def_Dist=200;net.rec_Def_Ang=90;//
        float tfx = net.rec_targetx;
        float tfy = net.rec_targety;
        float tfa = angle_add_transfer(-net.rec_targetang, y.benjijiao); //-net.rec_targetang;

        float obj_base_angle = f2l_angle(def_OBJ_X, def_OBJ_Y, 0, def_base_X, def_base_Y);
        float obj_base_radius = f2l_radius(def_OBJ_X, def_OBJ_Y, 0, def_base_X, def_base_Y);

        float obj_def_radius = 130;

        float t_fx =
            l2f_x(def_OBJ_X, def_OBJ_Y, 0, obj_base_angle, obj_def_radius); // 估算后 球对地面的位置 在地面坐标系里
        float t_fy = l2f_y(def_OBJ_X, def_OBJ_Y, 0, obj_base_angle, obj_def_radius);
        m_find.pField->SetBall(6, t_fx, t_fy);

        float temp_tA = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, t_fx, t_fy);
        float temp_tR = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, t_fx, t_fy);

        float temp_OBJ_A = angle_add_transfer2(f2l_angle(y.benjix, y.benjiy, -y.benjijiao, def_OBJ_X, def_OBJ_Y), 0);
        float temp_OBJ_R = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, def_OBJ_X, def_OBJ_Y);

        float temp_f_v = (temp_tR) * 1.0; // 1.6;
        if (temp_f_v < 0)
            temp_f_v = 0;
        //	if (temp_f_v>10)
        //		temp_f_v=10;

        mtl = 0;
        mtr = 0;
        mtb = 0;
        float mb, ml, mr;
        Dribble_ball.move2Control.limit_parameter_set_dis(
            25, 8); //(zs , 10);//(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
        Dribble_ball.move2Control.initializing_movement(temp_OBJ_A * 0.1, temp_f_v,
                                                        temp_tA); //(mudi_angle*0.1 ,mudi_radius*6,mudi_angle);//
        Dribble_ball.move2Control.front_v_maxlimit();
        Dribble_ball.move2Control.uptech_output_O(&mb, &ml, &mr);

        float p = 1;
        mtb = mb * p;
        mtl = ml * p;
        mtr = mr *
              p; // 设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000 = 2转/秒
        motospeed[0] = mtl;
        motospeed[1] = mtr;
        motospeed[2] = mtb;
        zhuan = 0; // 鑫鑫程序里的转动变量设置成0
        //		DebugV[5]=Dribble_ball.RMfv*cos(angle_add_transfer2(Dribble_ball.RMfa,0)*3.14/180.0)*5100;
        //		DebugV[6]=mtl/0.866;

        // 2013  force_xingwei_7_target
        if ((t_fy > 600 - 325) && (t_fy < 600 + 325) && (t_fx < 225))
            t_fx = 225;
        if (0)
        {
            // 2013  force_xingwei_7_target
            set_xingwei_7_tfa = temp_OBJ_A;
            set_xingwei_7_tfx = t_fx;
            set_xingwei_7_tfy = t_fy;
            force_xingwei_7_target_flag = 1;
            temp_xingwei_num = xingwei;
            xingwei = 7;
            // 2013  force_xingwei_7_target
        }
        if (1)
        {
            // 2013  force_xingwei_107_quanbx_y
            set_xingwei_107_quanbx = track_black_OBJ_map_X[0];
            set_xingwei_107_quanby = track_black_OBJ_map_Y[0];
            force_xingwei_107_quanbx_y_flag = 1;
            temp_xingwei_num = xingwei;
            xingwei = 107;
            net.rec_Def_Ang = 1;
            net.rec_Def_Dist = 150; //*2.7/cdbl/100.0;
                                    // 2013  force_xingwei_107_quanbx_y
        }
        if (0)
        {
            // 2013  force_xingwei_107_target
            set_xingwei_107_tfa = temp_OBJ_A;
            set_xingwei_107_tfx = t_fx;
            set_xingwei_107_tfy = t_fy;
            force_xingwei_107_target_flag = 1;
            temp_xingwei_num = xingwei;
            xingwei = 107;
        }
    }

    if (xingwei == 111) // 电流环力量跟踪
    {
        // 电机力量跟踪 使用电流的积分 使机器达到扭矩和平移力成为目的大小和比例
        // 达到条件后让机体 开始带球绕对方机器的运动  l,m
        mtl = 0;
        mtr = 0;
        mtb = 0;
        float mb, ml, mr;
        if ((m_pMotorCurrent[0] != 0) && (m_pMotorCurrent[2] != 0) && (m_pMotorCurrent[1] != 0) &&
            (abs(m_pMotorCurrent[0]) < 20000) && (abs(m_pMotorCurrent[2]) < 20000) &&
            (abs(m_pMotorCurrent[1]) < 20000)) // 设置电流的限制 防止通讯出错 只在认为正确的范围内才运行
        {
            if ((abs(m_pMotorCurrent[0]) > 6000) || (abs(m_pMotorCurrent[2]) > 6000) ||
                (abs(m_pMotorCurrent[1]) > 6000)) // 设置电流的限制 防止通讯出错 只在认为正确的范围内才运行
            {
                float a = m_pMotorCurrent[0] + m_pMotorCurrent[1] + m_pMotorCurrent[2];
            }

            if (valxx != 1)
            {
                test_c_V = 0; // 设置 测试电流闭环 验证程序 速度变量
                test_c_Va = 0;
                test_c_W = 0;
            }
            float current_a, current_v, current_w;
            Dribble_ball.O_movement_T_analyze(
                &current_a, &current_v, &current_w, m_pMotorCurrent[0], m_pMotorCurrent[2],
                m_pMotorCurrent[1]); //-mhLR[1]->Pos_[0],-mhBR[1]->Pos_[2],-mhRR[1]->Pos_[1]);//[0]是上个周期的结果
                                     //*move_v_angle, *move_v_speed, *turn_v
            current_w -= 2000;       // 修正电流
            /*显示平移*/ Debug_D[11].Draw_line_(324, 240, current_a - 90, current_v * 0.02, 255, 255, 0, pBuffer);
            /*显示旋转*/ Debug_D[11].Draw_circle(
                320, 240, -90, current_w * 0.01 - 90, 50, 255, 255, 0,
                pBuffer); //(320 ,240 ,-90		,Dribble_ball.robot_w*4-90,50,255,255,0,  pBuffer);
            char black_white_ball[20];
            float angle_p_1 = 0.19;    // 0.180*ax;
            float angle_i_1 = 0;       // 0.3*ax;//0.0002*ax;//0.005*ax;  //0.06*ax;//0;
            float angle_d_1 = -0.0070; //-0.0070*ax;//
            Dribble_ball.move2Control.limit_parameter_set_angle(
                10.0, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
            Dribble_ball.move2Control.limit_parameter_set_dis(
                50.0, 15); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
            Dribble_ball.move2Control.initializing_movement(0, 0, 0); //
            Dribble_ball.move2Control.front_v_c = test_c_V;           // 设置 测试电流闭环 验证程序 速度变量
            Dribble_ball.move2Control.front_v_angle_c = test_c_Va;
            Dribble_ball.move2Control.turn_v_c = test_c_W;
            Dribble_ball.move2Control.turn_v_c += current_w * 0.0001;
            Dribble_ball.move2Control.zs_translate_compound_O(current_a, current_v * 0.0001);
            test_c_V = Dribble_ball.move2Control.front_v_c; // 设置 测试电流闭环 验证程序 速度变量
            test_c_Va = Dribble_ball.move2Control.front_v_angle_c;
            test_c_W = Dribble_ball.move2Control.turn_v_c;

            Dribble_ball.move2Control.front_a_maxlimit();
            Dribble_ball.move2Control.uptech_output_O(&mb, &ml, &mr);
        }
        float p = 1;
        mtb = mb * p;
        mtl = ml * p;
        mtr = mr *
              p; // 设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000 = 2转/秒
        motospeed[0] = mtl;
        motospeed[1] = mtr;
        motospeed[2] = mtb;
        zhuan = 0; // 鑫鑫程序里的转动变量设置成0
    }
    if (xingwei == 112) // 电流环力量跟踪
    {
        // 电机力量跟踪 使用电流的积分 使机器达到扭矩和平移力成为目的大小和比例
        // 达到条件后让机体 开始带球绕对方机器的运动  l,m

        mtl = 0;
        mtr = 0;
        mtb = 0;
        float mb, ml, mr;
        if ((m_pMotorCurrent[0] != 0) && (m_pMotorCurrent[2] != 0) && (m_pMotorCurrent[1] != 0) &&
            (abs(m_pMotorCurrent[0]) < 20000) && (abs(m_pMotorCurrent[2]) < 20000) &&
            (abs(m_pMotorCurrent[1]) < 20000)) // 设置电流的限制 防止通讯出错 只在认为正确的范围内才运行
        {
            if (valxx != 1)
            {
                test_c_V = 0; // 设置 测试电流闭环 验证程序 速度变量
                test_c_Va = 0;
                test_c_W = 0;
            }
            float traget_c_a = 30;
            float traget_c_v = 2000;
            float traget_c_w = -1000;
            float current_a, current_v, current_w; // 当前采集到各电流分量
            float err_c_a, err_c_v, err_c_w;       // 各电流分量的误差
            // time_cb_1//抢球计时器1  记录已经有几周期达到或超过需要电流  做完准备动作
            // time_cb_2//抢球计时器2  记录已经进行截断动作  多少时间
            // 抢球成功判定 己方持球 电流持续多少时间不超过预计 且在前掩膜无对方机器
            // 进入抢球判定 己方持球 电流持续多少时间超彻预计 在前掩膜无对方机器
            // 需要一个辅助程序 记录电流 并计算 多少周期内有 多要总和电流超差
            /*		if ((time_cb_1>100)&&(time_cb_2<200)) //单位ms 正在进行截断动作
                    {
                        traget_c_a=-30;
                        traget_c_v=2000;
                        traget_c_w=-1000;
                        time_cb_2 +=1;
                        if (time_cb_2>=200)
                        {
                            time_cb_1=0;
                        }
                    }
                    if ((time_cb_1<100)&&(time_cb_2>=200)) //单位ms 正在进行准备动作
                    {
                        traget_c_a=30;
                        traget_c_v=2000;
                        traget_c_w=1000;
                        time_cb_1 +=1;//
                        if (time_cb_1>=100)
                        {
                            time_cb_2=0;
                        }
                    }
            */
            Dribble_ball.O_movement_T_analyze(
                &current_a, &current_v, &current_w, m_pMotorCurrent[0], m_pMotorCurrent[2],
                m_pMotorCurrent[1]); //-mhLR[1]->Pos_[0],-mhBR[1]->Pos_[2],-mhRR[1]->Pos_[1]);//[0]是上个周期的结果
                                     //*move_v_angle, *move_v_speed, *turn_v
            current_w -= 2000;       // 修正电流

            err_c_a = angle_add_transfer(traget_c_a, -current_a);
            err_c_v = traget_c_v - current_v;
            err_c_w = traget_c_w - current_w; // 计算各电流分量的误差

            /*显示平移*/ Debug_D[11].Draw_line_(324, 240, current_a - 90, current_v * 0.02, 255, 255, 0, pBuffer);
            /*显示旋转*/ Debug_D[11].Draw_circle(
                320, 240, -90, current_w * 0.01 - 90, 50, 255, 255, 0,
                pBuffer); //(320 ,240 ,-90		,Dribble_ball.robot_w*4-90,50,255,255,0,  pBuffer);
            char black_white_ball[20];
            float angle_p_1 = 0.19;    // 0.180*ax;
            float angle_i_1 = 0;       // 0.3*ax;//0.0002*ax;//0.005*ax;  //0.06*ax;//0;
            float angle_d_1 = -0.0070; //-0.0070*ax;//
            Dribble_ball.move2Control.limit_parameter_set_angle(
                10.0, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
            Dribble_ball.move2Control.limit_parameter_set_dis(
                50.0, 15); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
            Dribble_ball.move2Control.initializing_movement(0, 0, 0); //
            Dribble_ball.move2Control.front_v_c = test_c_V;           // 设置 测试电流闭环 验证程序 速度变量
            Dribble_ball.move2Control.front_v_angle_c = test_c_Va;
            Dribble_ball.move2Control.turn_v_c = test_c_W;

            Dribble_ball.move2Control.turn_v_c += err_c_w * 0.0001;                       // 积分跟踪扭矩
            Dribble_ball.move2Control.zs_translate_compound_O(err_c_a, err_c_v * 0.0001); // 积分跟踪平移力量

            test_c_V = Dribble_ball.move2Control.front_v_c; // 设置 测试电流闭环 验证程序 速度变量
            test_c_Va = Dribble_ball.move2Control.front_v_angle_c;
            test_c_W = Dribble_ball.move2Control.turn_v_c;

            Dribble_ball.move2Control.front_a_maxlimit();
            Dribble_ball.move2Control.uptech_output_O(&mb, &ml, &mr);
        }
        float p = 1;
        mtb = mb * p;
        mtl = ml * p;
        mtr = mr *
              p; // 设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000 = 2转/秒
        motospeed[0] = mtl;
        motospeed[1] = mtr;
        motospeed[2] = mtb;
        zhuan = 0; // 鑫鑫程序里的转动变量设置成0
    }

    if (xingwei == 113) // 电流环力量跟踪  截屏错误数据  留错误复现可执行程序 看万杰程序接收到的原始数据 设置串口速度
    {
        // 电机力量跟踪 使用电流的积分 使机器达到扭矩和平移力成为目的大小和比例
        // 达到条件后让机体 开始带球绕对方机器的运动  l,m

        mtl = 0;
        mtr = 0;
        mtb = 0;
        float mb, ml, mr;
        //	float traget_c_a=-60;
        //	float traget_c_v=15000;
        //	float traget_c_w=9000;//-1000;
        //	float traget_c_a=40;
        //	float traget_c_v=22000;
        //	float traget_c_w=4000;//-1000;

        float traget_c_a = 0;                  // 60;
        float traget_c_v = 0;                  // 15000;
        float traget_c_w = 0;                  //-9000;//-1000;
                                               //	float traget_c_a=-40;
                                               //	float traget_c_v=22000;
                                               //	float traget_c_w=-4000;//-1000;
        float current_a, current_v, current_w; // 当前采集到各电流分量
        float err_c_a = 0;                     // 各电流分量的误差
        float err_c_v = 0;
        float err_c_w = 0;

        if ((m_pMotorCurrent[0] != 0) && (m_pMotorCurrent[2] != 0) && (m_pMotorCurrent[1] != 0) &&
            (abs(m_pMotorCurrent[0]) < 20000) && (abs(m_pMotorCurrent[2]) < 20000) &&
            (abs(m_pMotorCurrent[1]) < 20000) &&
            (PowerOn == 1)) // 设置电流的限制 防止通讯出错 只在认为正确的范围内才运行
        {
            // time_cb_1//抢球计时器1  记录已经有几周期达到或超过需要电流  做完准备动作
            // time_cb_2//抢球计时器2  记录已经进行截断动作  多少时间
            // 抢球成功判定 己方持球 电流持续多少时间不超过预计 且在前掩膜无对方机器
            // 进入抢球判定 己方持球 电流持续多少时间超彻预计 在前掩膜无对方机器
            // 需要一个辅助程序 记录电流 并计算 多少周期内有 多要总和电流超差
            /*
                if ((valxx!=1)||(PowerOn==0))
                {
                    time_cb_1=100;
                    time_cb_2=0;
            ;	}

                    if ((time_cb_1>=100)&&(time_cb_2<120)) //单位ms 正在进行截断动作
                    {
                //		traget_c_a=-30;
                //		traget_c_v=2000;
                //		traget_c_w=-1000;
                traget_c_a=60;
                traget_c_v=15000;
                traget_c_w=-9000;//-1000;
                xingwei=1000;
                        time_cb_2 +=1;
                        if (time_cb_2>=120)
                        {
                            time_cb_1=0;
                        }
                    }
                    if ((time_cb_1<100)&&(time_cb_2>=120)) //单位ms 正在进行准备动作
                    {
                //		traget_c_a=30;
                //		traget_c_v=2000;
                //		traget_c_w=1000;
                traget_c_a=-40;
                traget_c_v=22000;
                traget_c_w=-4000;//-1000;
                xingwei=2000;
                        time_cb_1 +=1;//
                        if (time_cb_1>=100)
                        {
                            time_cb_2=0;
                        }
                    }
            */
            Dribble_ball.O_movement_T_analyze(
                &current_a, &current_v, &current_w, m_pMotorCurrent[0], m_pMotorCurrent[2],
                m_pMotorCurrent[1]); //-mhLR[1]->Pos_[0],-mhBR[1]->Pos_[2],-mhRR[1]->Pos_[1]);//[0]是上个周期的结果
                                     //*move_v_angle, *move_v_speed, *turn_v
            current_w += 2000;       // 修正电流
            /*显示平移*/ Debug_D[11].Draw_line_(324, 240, current_a - 90, current_v * 0.02, 255, 255, 0, pBuffer);
            /*显示旋转*/ Debug_D[11].Draw_circle(
                320, 240, -90, current_w * 0.01 - 90, 50, 255, 255, 0,
                pBuffer); //(320 ,240 ,-90		,Dribble_ball.robot_w*4-90,50,255,255,0,  pBuffer);

            // 将两个向量	相加	得到结果向量的角度
            err_c_a = Dribble_ball.move2Control.vector_add_angle(
                current_a, current_v, traget_c_a, traget_c_v); // angle_add_transfer(-traget_c_a,current_a);
            err_c_v = Dribble_ball.move2Control.vector_add_length(current_a, current_v, traget_c_a,
                                                                  traget_c_v); //-traget_c_v+current_v;
            err_c_w = -traget_c_w + current_w;                                 // 计算各电流分量的误差
            if (err_c_w > 1000)
                err_c_w = 1000;
            if (err_c_w < -1000)
                err_c_w = -1000;
            if (err_c_v > 1000)
                err_c_v = 1000;
            if (err_c_v < -1000)
                err_c_v = -1000;
        }

        if ((valxx != 1) || (PowerOn == 0))
        {
            test_c_V = 0; // 设置 测试电流闭环 验证程序 速度变量
            test_c_Va = 0;
            test_c_W = 0;
        }

        char black_white_ball[20];
        float angle_p_1 = 0.19;    // 0.180*ax;
        float angle_i_1 = 0;       // 0.3*ax;//0.0002*ax;//0.005*ax;  //0.06*ax;//0;
        float angle_d_1 = -0.0070; //-0.0070*ax;//
        Dribble_ball.move2Control.limit_parameter_set_angle(
            10.0, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
        Dribble_ball.move2Control.limit_parameter_set_dis(
            50.0, 15); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
        Dribble_ball.move2Control.initializing_movement(0, 0, 0); //
        Dribble_ball.move2Control.front_v_c = test_c_V;           // 设置 测试电流闭环 验证程序 速度变量
        Dribble_ball.move2Control.front_v_angle_c = test_c_Va;
        Dribble_ball.move2Control.turn_v_c = test_c_W;

        Dribble_ball.move2Control.turn_v_c += err_c_w * 0.00024;                       // 积分跟踪扭矩
        Dribble_ball.move2Control.zs_translate_compound_O(err_c_a, err_c_v * 0.00024); // 积分跟踪平移力量

        test_c_V = Dribble_ball.move2Control.front_v_c; // 设置 测试电流闭环 验证程序 速度变量
        test_c_Va = Dribble_ball.move2Control.front_v_angle_c;
        test_c_W = Dribble_ball.move2Control.turn_v_c;

        Dribble_ball.move2Control.front_a_maxlimit();
        Dribble_ball.move2Control.front_v_maxlimit();
        Dribble_ball.move2Control.uptech_output_O(&mb, &ml, &mr);

        float p = 1;
        mtb = mb * p;
        mtl = ml * p;
        mtr = mr *
              p; // 设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000 = 2转/秒

        motospeed[0] = mtl;
        motospeed[1] = mtr;
        motospeed[2] = mtb;
        zhuan = 0; // 鑫鑫程序里的转动变量设置成0
        if (mtl > 300)
            mtl = mtl;
        if (mtl < -300)
            mtl = mtl;
        // y:  x轴(既：t时间)高度     y_add:y偏置量   quotiety:数值缩放比例		enable==1记录使能为1就记录    R G B
        // 视频存储区 BOOL CDebug_Display::Draw_vp(float y,int y_add,float quotiety,char enable,unsigned char r,unsigned
        // char g,unsigned char b, BYTE *Buf)
        //		/*显示旋转*/Debug_D[11].Draw_vp    (mtl ,240 ,0.001		,valxx,0,255,255,  pBuffer);//(320 ,240 ,-90
        //,Dribble_ball.robot_w*4-90,50,255,255,0,  pBuffer);
    }

    if (xingwei == 100) // 源自101 接球 并在接住后调用 1追球进攻
    {                   // 球比较远应该往前追  往后退的时间点 太早 再早应该追球
        if (valxx != 1)
        {
            Dribble_ball.fount_xVset = 0;
            Dribble_ball.fount_yVset = 0;
        }

        mtl = 0;
        mtr = 0;
        mtb = 0;
        float mb, ml, mr;
        if (1)     //(y.ball_zhong>50
                   //	   (Dribble_ball.ball_op_Fr>(Dribble_ball.holdballFmin+1.0))
                   //	   ||(abs(Dribble_ball.ball_op_Fa)>5)
                   //	   ||(Dribble_ball.FHaveBall!=1)//(abs(Dribble_ball.ball_op_Fweight)<1000)
                   //	   ) //||(1)//(1)//应该再加一个球的大小前景  如果球的距离太远(0)//
        {          //	target_num		=1;	//设置路径规划目的地为 球	约定 traget_numC号 初始化值0   1球 2敌方门 3己方门
                   // 4以上其他点
                   // use_side_line	=0;	//	约定use_side_line号 为1 路径规划中加入边线信息  !=1时无边线信息
                   // add_oppo_radius	=8.0;//规避半径初始化6.5 8.0
                   // if (ball_radius<60)
                   //{	add_oppo_radius=sqrt(ball_radius);
                   //}
            if (0) //(m_pAD[0]>1200 || m_pAD[1]>1200)//接球参数清零
            {
                Dribble_ball.fount_xVset = 0;
                Dribble_ball.fount_yVset = 0;
            }

            Dribble_ball.Ball_Track3(&mb, &ml, &mr); // 接球

            if (0) //(m_pAD[0]>1200 || m_pAD[1]>1200)//追球
            {
                Dribble_ball.Ball_Into_Holder4(Dribble_ball.ball_estimate_angle, Dribble_ball.ball_estimate_radius,
                                               Dribble_ball.ball_estimate_F_angle, Dribble_ball.ball_estimate_F_radius);
                Dribble_ball.move_test_C.uptech_output_O(&mb, &ml, &mr); // 对Ball_Into_Holder进行输出
            }
            //		Debug_D[15].Draw_vp(Dribble_ball.view_add_x		,160  ,100.0  ,valxx,255,0,255,
            // pBuffer);//显示参考线 		Debug_D[16].Draw_vp(Dribble_ball.view_add_y     ,320  ,100.0
            // ,valxx,255,0,255, pBuffer);//显示参考线
            Dribble_ball.Dribble_ball_other_moment_flag = 1; // 先运行了其他程序再运行带球函数标志
        }
        float p = 1.0; // 0;//
        mtb = mb * p;
        mtl = ml * p;
        mtr = mr *
              p; // 设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000 = 2转/秒
        motospeed[0] = mtl; // 0;//
        motospeed[1] = mtr; // 0;//
        motospeed[2] = mtb; // 0;//
        motospeed[3] = 16000;
        0; //
        motospeed[4] = -16000;
        0; //

        zhuan = 0; // 鑫鑫程序里的转动变量设置成0
        if (0)     //(m_pAD[0]>1200 || m_pAD[1]>1200)
        {
            xingwei = 1;
            mtl = 0;
            mtr = 0;
            mtb = 0;
            O_movement_T_analyze(&front_a, &front_v, &turn_w, ml, mb,
                                 mr); //-mhLR[1]->Pos_[0],-mhBR[1]->Pos_[2],-mhRR[1]->Pos_[1]);//[0]是上个周期的结果
                                      //*move_v_angle, *move_v_speed, *turn_v
            jifen = front_v;
            zhuan = 0; // 鑫鑫程序里的转动变量设置成0
        }
    }
    ///*
    if ((valxx == 0) || (xingweiP != 1)) // 1说明本周期运行过 0说明没运行过 要连接数据
    {
        Dribble_ball.fount_xVset =
            l2f_x(0, 0, Dribble_ball.robot_L.f_a, Dribble_ball.RMfa, Dribble_ball.RMfv) * 22.648 * 1.4; // 1.2;
        Dribble_ball.fount_yVset =
            l2f_y(0, 0, Dribble_ball.robot_L.f_a, Dribble_ball.RMfa, Dribble_ball.RMfv) * 22.648 * 1.4; // 1.2;
    }
    float xingwei1mb, xingwei1ml, xingwei1mr;
    Dribble_ball.Ball_Track3(&xingwei1mb, &xingwei1ml, &xingwei1mr); // 接球

    if (0) //(xingwei==30)//2011 接球
    {
        // 接球
        float robot_dx = 0; // 0.20;//; //前后方向 前为正
        float robot_dy = 0; // 0.15;//; //左右方向 左为正
        float robot_da = coordinate_transfer_angle(robot_dx, robot_dy);
        float robot_dr = coordinate_transfer_radius(robot_dx, robot_dy);
        //	和f2l_angle (ballC_sx,ballC_sy,0, robotA_d.f_x,robotA_d.f_y);

        obj_properties robotA_d;
        obj_properties ballC_d;
        robotA_d.f_x = y.benjix * 0.01;
        robotA_d.f_y = y.benjiy * 0.01;
        robotA_d.f_a = -y.benjijiao;
        ballC_d.f_x = y.quanbx * 0.01;
        ballC_d.f_y = y.quanby * 0.01;
        float robotTx = l2f_x(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, robot_da, robot_dr);
        float robotTy = l2f_y(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, robot_da, robot_dr);
        robotA_d.f_x = robotTx;
        robotA_d.f_y = robotTy;

        //	m_find.pField->SetAgent(5,robotTx*100 ,robotTy*100,0);//场子正中向上  逆时针坐标系
        float mb = 0;
        float ml = 0;
        float mr = 0;

        // 注意 需要用拟合等方式 先确认速度方向(10桢间取差直线拟合)基本是向本机的再开始此程序 否则接了也接不着
        // 使用f2l_angle (ballC_sx,ballC_sy,0, ballC_d .f_x,ballC_d .f_y);
        //	和f2l_angle (ballC_sx,ballC_sy,0, robotA_d.f_x,robotA_d.f_y);
        //   对比看机器和球的飞行角差多少要差90度(或者60度)以上就追球就行了
        float robot_Sball_radius = f2l_radius(ballC_sx, ballC_sy, 0, robotA_d.f_x, robotA_d.f_y);
        float ball__Sball_radius = f2l_radius(ballC_sx, ballC_sy, 0, ballC_d.f_x, ballC_d.f_y);
        float robot__ball_radius = f2l_radius(ballC_d.f_x, ballC_d.f_y, 0, robotA_d.f_x, robotA_d.f_y);

        if (ball__Sball_radius > 1.0)
        {

            for (i = 20 - 1; i > 0; i--) // 存储上100周期电机的发送转速
            {
                ball_f_x_h[i] = ball_f_x_h[i - 1];
                ball_f_y_h[i] = ball_f_y_h[i - 1];
            }
            ball_f_x_h[0] = ballC_d.f_x;
            ball_f_y_h[0] = ballC_d.f_y;

            for (i = 0; i < 10; i++) // 存储上100周期电机的发送转速
            {
                ball_f_hw[i] = pow(1 + (robot_Sball_radius - robot__ball_radius) / robot_Sball_radius, 10 - i);
            }

            ball_f_x_h[9] = ballC_sx;
            ball_f_y_h[9] = ballC_sy;
            ball_f_hw[9] = pow(2, 9);

            float tempa = least_square_method(ball_f_x_h, ball_f_y_h, ball_f_hw, 10);
            // least_square_method(ball_f_x_h,ball_f_y_h,ball_f_hw,10);
            if (fabs(angle_add_transfer2(tempa, -f2l_angle(ballC_sx, ballC_sy, 0, ballC_d.f_x, ballC_d.f_y))) >= 90)
                tempa = angle_add_transfer2(tempa, 180);
            // float tempa = f2l_angle (ballC_sx,ballC_sy,0, ballC_d.f_x,ballC_d.f_y);//估算后 球对地面的位置
            // 在地面坐标系里

            float tempangle = f2l_angle(ballC_sx, ballC_sy, tempa, robotA_d.f_x,
                                        robotA_d.f_y); // 估算后 球对地面的位置 在地面坐标系里
            float tempradius = robot_Sball_radius * cos(tempangle * 3.14 / 180.0);
            float t_fxx = l2f_x(ballC_sx, ballC_sy, 0, tempa, tempradius); // 估算后 球对地面的位置 在地面坐标系里
            float t_fyy = l2f_y(ballC_sx, ballC_sy, 0, tempa, tempradius);
            /*
            float	v_ball = f2l_radius(ball_f_x_h [0],ball_f_y_h [0],0, ball_f_x_h [5],ball_f_y_h [5])/5.0/0.33;
            Diplay_A=v_ball;//
            float	v_robot = 0.5;
            float   time=f2l_radius(t_fxx,t_fyy,0, robotA_d.f_x,robotA_d.f_y)/v_robot;
            float   radius_add=time*v_ball+(v_ball+0.7)*robot__ball_radius;
                 t_fxx = l2f_x     (ballC_sx,ballC_sy,0, tempa,ball__Sball_radius+radius_add);//估算后 球对地面的位置
            在地面坐标系里 t_fyy = l2f_y     (ballC_sx,ballC_sy,0, tempa,ball__Sball_radius+radius_add);
            */

            // float tempr = f2l_radius(ballC_sx,ballC_sy,0, ballC_d.f_x,ballC_d.f_y);
            //	float t_fxx = l2f_x     (ballC_sx,ballC_sy,0, tempa,robot_Sball_radius);//估算后 球对地面的位置
            // 在地面坐标系里 	float t_fyy = l2f_y     (ballC_sx,ballC_sy,0, tempa,robot_Sball_radius);

            float rb_dis = ball__Sball_radius - robot_Sball_radius; // 当前 球C~机器人A x 距离
            float rb_d_dis = rb_dis - rb_dis_p;
            if (rb_d_dis > 0.20)
                rb_d_dis = 0.20;
            if (rb_d_dis < -0.20)
                rb_d_dis = -0.20;
            // v_radius_set +=(rb_d_dis + rb_dis*0.0179)*30.12;//设置速度
            v_radius_set += (rb_d_dis * 1.2 + rb_dis * 0.012 - 0.042 * 1.25 / MK) * 30.12 * MK / 1.25; // 28 ;//设置速度

            rb_dis_p = rb_dis;

            float t_angle1 = f2l_angle(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx, t_fyy);
            float t_radius1 = f2l_radius(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx, t_fyy) * 100.0 * 1.2 * MK /
                              1.25; // 源坐标系是m（米）制还要换成厘米制*100.0
            float t_radius1P = (3.0 - robot__ball_radius) / 3.0;
            if (t_radius1P < 0)
                t_radius1P = 0;

            t_radius1 = t_radius1 * (t_radius1P * 0.5 + 0.26); //(t_radius1P*0.4+0.36)
            if (t_radius1 > 39)
                t_radius1 = 39;

            float t_angle2 = f2l_angle(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, ballC_sx, ballC_sy);
            float t_radius2 = -v_radius_set;
            t_radius2 = t_radius2; // 0.045;//
            if (t_radius2 > 100)
                t_radius2 = 100;
            if (t_radius2 < -100)
                t_radius2 = -100;

            for (i = 20 - 1; i > 0; i--) // 存储上100周期电机的发送转速
            {
                t_radius1_h[i] = t_radius1_h[i - 1];
                t_radius2_h[i] = t_radius2_h[i - 1];
            }
            t_radius1_h[0] = t_radius1;
            t_radius2_h[0] = t_radius2;
            //	/*
            //	t_radius1=t_radius1_h[0]*0.7
            //			 +t_radius1_h[1]*0.3
            //			 +t_radius1_h[2]*0.2
            //			 +t_radius1_h[3]*0.1;
            t_radius2 = t_radius2_h[0] * 0.35 + t_radius2_h[1] * 0.25 + t_radius2_h[2] * 0.15 + t_radius2_h[3] * 0.09 +
                        t_radius2_h[4] * 0.06 + t_radius2_h[5] * 0.05 + t_radius2_h[6] * 0.05;
            //	*/

            float t_angle = move_test_C.vector_add_angle(t_angle1, t_radius1, t_angle2, t_radius2);
            float t_radius = move_test_C.vector_add_length(t_angle1, t_radius1, t_angle2, t_radius2);

            m_find.pField->SetAgent(5, t_fxx * 100, t_fyy * 100, 0); // 场子正中向上  逆时针坐标系
            // m_find.pField->SetBall (7,ballA .f_x*100 ,ballA .f_y*100);
            //	t_radius=t_radius*0.8;
            if (t_radius > 100)
                t_radius = 100;

            float turn = ball_angle * 1.2; // 1.2;//0.35;//*turn_p;
            //	float turn_p=(39-t_radius1)/39.0;
            float turn_p = (robot_Sball_radius - robot__ball_radius) / robot_Sball_radius;
            if (turn_p < 0)
                turn_p = 0;
            // if (turn>10*turn_p)
            //	turn=10*turn_p;
            // if (turn<-10*turn_p)
            //	turn=-10*turn_p;

            move_test_C.limit_parameter_set_angle(
                18 * turn_p, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
            move_test_C.limit_parameter_set_dis(
                60,
                24); //(62.0 , 15);//(39.0 , 12);//(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
            move_test_C.initializing_movement(turn, t_radius * 1.2, t_angle); //(float turn,float front,float angle)
            move_test_C.turn_v_maxlimit();
            move_test_C.front_v_maxlimit();
            move_test_C.front_a_maxlimit();
            move_test_C.uptech_output_O(&mb, &ml, &mr); // ？？？？？？？？？？？？？？
        }
        else
        {
            move_test_C.limit_parameter_set_angle(
                11, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
            move_test_C.initializing_movement(ball_angle * 0.2, 0, 0); //(float turn,float front,float angle)
            move_test_C.uptech_output_O(&mb, &ml, &mr);                // ？？？？？？？？？？？？？？
            //	mb=0;ml=0;mr=0;
        }
        float p = 0.6; // 1.0;
        mtb = mb * p;
        mtl = ml * p;
        mtr = mr *
              p; // 设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000 = 2转/秒
        //		mtb=0; mtl=0; mtr=0;//设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1  是2000/2
        // 即发2000 = 2转/秒

        xingwei1mb = mtb;
        xingwei1ml = mtl;
        xingwei1mr = mtr;
        //			motospeed[0]=mtl;
        //			motospeed[1]=mtr;
        //			motospeed[2]=mtb;
        //			motospeed[3]=9000;
        //			motospeed[4]=-9000;
        // 接球
        motospeed[3] = 16000;
        motospeed[4] = -16000;
        //			zhuan=0;//鑫鑫程序里的转动变量设置成0
        if (m_pAD[0] > 2000 && m_pAD[1] > 2000 && y.ball_chang < 55 && abs(y.ball_jiao) < 20)
            jieqiuys = 158;
    }

    //*/
    if (xingwei == 110)
    {
        if ((valxx == 0) || (xingweiP != 110)) // 1说明本周期运行过 0说明没运行过 要连接数据
        {
            Dribble_ball.fount_xVset =
                l2f_x(0, 0, Dribble_ball.robot_L.f_a, Dribble_ball.RMfa, Dribble_ball.RMfv) * 22.648 * 1.4; // 1.2;
            Dribble_ball.fount_yVset =
                l2f_y(0, 0, Dribble_ball.robot_L.f_a, Dribble_ball.RMfa, Dribble_ball.RMfv) * 22.648 * 1.4; // 1.2;
        }
        if (1) //(chang>50 && abs(jiajiao(jiao,jiaox))<40)
        {
            if ((m_pAD[0] > 1200 || m_pAD[1] > 1200))
            //	&&(chang>50 && abs(jiajiao(jiao,jiaox))<40))//接球参数清零
            {
                //	Dribble_ball.fount_xVset=0;
                //	Dribble_ball.fount_yVset=0;
                xingwei = 11;
            }
            float mb, ml, mr;
            Dribble_ball.Ball_Track3(&mb, &ml, &mr);         // 接球
            Dribble_ball.Dribble_ball_other_moment_flag = 1; // 先运行了其他程序再运行带球函数标志
            mtb = mb;
            mtl = ml;
            mtr = mr;
            zhuan = 0;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    if (xingwei != 30) // xingwei==1)//2012接球
        catch_start_wait_loop = 0;
    else
        catch_start_wait_loop++;

    if (0) // xingwei==30)//xingwei==1)//2012接球
    {
        // 接球
        //	float robot_dx=0;//0.20;//; //前后方向 前为正
        //	float robot_dy=0;//0.15;//; //左右方向 左为正
        //	float robot_da=coordinate_transfer_angle(robot_dx,robot_dy);
        //	float robot_dr=coordinate_transfer_radius(robot_dx,robot_dy);
        //	和f2l_angle (ballC_sx,ballC_sy,0, robotA_d.f_x,robotA_d.f_y);
        ballC_sx = move_Emu_ballSX;
        ballC_sy = move_Emu_ballSY;
        obj_properties robotA_d;
        obj_properties ballC_d;
        robotA_d.f_x = move_Emu_robot_f_x; // y.benjix*0.01;
        robotA_d.f_y = move_Emu_robot_f_y; // y.benjiy*0.01;
        robotA_d.f_a = move_Emu_robot_f_a; //-y.benjijiao;
        ballC_d.f_x = move_Emu_ballX;      // y.quanbx*0.01;
        ballC_d.f_y = move_Emu_ballY;      // y.quanby*0.01;
        //	float robotTx= l2f_x (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a, robot_da,robot_dr);
        //	float robotTy= l2f_y (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a, robot_da,robot_dr);
        //	robotA_d.f_x=robotTx;
        //	robotA_d.f_y=robotTy;

        //	m_find.pField->SetAgent(5,robotTx*100 ,robotTy*100,0);//场子正中向上  逆时针坐标系
        float mb = 0;
        float ml = 0;
        float mr = 0;

        // 注意 需要用拟合等方式 先确认速度方向(10桢间取差直线拟合)基本是向本机的再开始此程序 否则接了也接不着
        // 使用f2l_angle (ballC_sx,ballC_sy,0, ballC_d .f_x,ballC_d .f_y);
        //	和f2l_angle (ballC_sx,ballC_sy,0, robotA_d.f_x,robotA_d.f_y);
        //   对比看机器和球的飞行角差多少要差90度(或者60度)以上就追球就行了
        float robot_Sball_radius = f2l_radius(ballC_sx, ballC_sy, 0, robotA_d.f_x, robotA_d.f_y);
        float ball__Sball_radius = f2l_radius(ballC_sx, ballC_sy, 0, ballC_d.f_x, ballC_d.f_y);
        float robot__ball_radius = f2l_radius(ballC_d.f_x, ballC_d.f_y, 0, robotA_d.f_x, robotA_d.f_y);
        for (i = 20 - 1; i > 0; i--) // 存储上100周期电机的发送转速
        {
            ball_f_x_h[i] = ball_f_x_h[i - 1];
            ball_f_y_h[i] = ball_f_y_h[i - 1];
        }
        ball_f_x_h[0] = ballC_d.f_x;
        ball_f_y_h[0] = ballC_d.f_y;
        ball_f_hw[0] = 10;
        ball_f_hw[1] = 5;
        ball_f_hw[2] = 5;
        ball_f_hw[3] = 4;
        ball_f_hw[4] = 4;
        ball_f_hw[5] = 4;
        ball_f_hw[6] = 3;
        ball_f_hw[7] = 3;
        ball_f_hw[8] = 2;
        ball_f_hw[9] = 2;

        if (catch_start_wait_loop >= 6) // 接球 队员 切接球后 等待球速度方向稳定 等待周期计数
        {
            catch_start_wait_loop = 6;

            float tempa;
            if (1)
            {
                if (1) // 对发球点是否做特殊 加高权值处理
                {
                    ball_f_x_h[9] = ballC_sx;
                    ball_f_y_h[9] = ballC_sy;
                    ball_f_hw[9] = 10; // pow(2, 9);
                    tempa = least_square_method(ball_f_x_h, ball_f_y_h, ball_f_hw, 10);
                }
                else
                {
                    tempa = least_square_method(ball_f_x_h, ball_f_y_h, ball_f_hw, 9);
                }
            }
            else
            {
                tempa = f2l_angle(ballC_sx, ballC_sy, 0, ballC_d.f_x, ballC_d.f_y);
            }
            // for(i=400-1;i>0;i--)  {debug[i]=debug[i-1];}debugP++;  debug[0]=tempa;
            // debug[debugP]=tempa;
            // debug2[debugP]=move_Emu_ballX;
            // debug3[debugP]=move_Emu_ballY;
            // debugP++;
            if (ball__Sball_radius > 1.0)
            {

                //	for(i=0 ; i<10 ; i++)//生成最小 二乘 权值    ？？？？？？
                //	{
                //		ball_f_hw [i] = (10-i)*0.1;//pow(1+(robot_Sball_radius-robot__ball_radius)/robot_Sball_radius,
                // 10-i);
                //	}

                float tempv = f2l_radius(ball_f_x_h[0], ball_f_y_h[0], 0, ball_f_x_h[9], ball_f_y_h[9]);
                //		display_a=tempv;
                // v_p对速度加权处理 速度大于0.5m/s以上方向好判断以下不好判断
                // v_p对t_fxx,t_fyy的位置产生影响 也应该对 球速度方向产生影响
                float v_p = fabs(tempv) / 0.5;
                if (v_p > 1)
                    v_p = 1;

                // least_square_method(ball_f_x_h,ball_f_y_h,ball_f_hw,10);
                if (fabs(angle_add_transfer2(
                        tempa, -f2l_angle(ball_f_x_h[9], ball_f_y_h[9], 0, ballC_d.f_x, ballC_d.f_y))) >= 90)
                    tempa = angle_add_transfer2(tempa, 180);

                // float tempa = f2l_angle (ballC_sx,ballC_sy,0, ballC_d.f_x,ballC_d.f_y);//估算后 球对地面的位置
                // 在地面坐标系里
                float t_fxx;
                float t_fyy;
                if (0)
                { // 使用发球 球位置生成机体跑位位置 误差取决于发球球位置
                    float tempangle = f2l_angle(ballC_sx, ballC_sy, tempa, robotA_d.f_x,
                                                robotA_d.f_y); // 估算后 球对地面的位置 在地面坐标系里
                    float tempradius = robot_Sball_radius * cos(tempangle * 3.14 / 180.0);
                    t_fxx = l2f_x(ballC_sx, ballC_sy, 0, tempa, tempradius); // 估算后 球对地面的位置 在地面坐标系里
                    t_fyy = l2f_y(ballC_sx, ballC_sy, 0, tempa, tempradius);
                }
                if (0)
                { // 使用当前 球位置生成机体跑位位置 误差取决于当前球位置 可以参考机器看到最近几桢球的
                  // 位置和移动距离看和拟和直线的误差判断球是否在直线运动
                    float tempangle = f2l_angle(ballC_sx, ballC_sy, tempa, robotA_d.f_x,
                                                robotA_d.f_y); // 估算后 球对地面的位置 在地面坐标系里
                    float tempradius = robot_Sball_radius * cos(tempangle * 3.14 / 180.0);
                    float temp2radius = tempradius - ball__Sball_radius;
                    t_fxx =
                        l2f_x(ballC_d.f_x, ballC_d.f_y, 0, tempa, temp2radius); // 估算后 球对地面的位置 在地面坐标系里
                    t_fyy = l2f_y(ballC_d.f_x, ballC_d.f_y, 0, tempa, temp2radius);
                }
                //	if (1)
                //	{	//使用当前 球位置生成机体跑位位置 误差取决于当前球位置 可以参考机器看到最近几桢球的
                // 位置和移动距离看和拟和直线的误差判断球是否在直线运动
                float tempangle = f2l_angle(ballC_d.f_x, ballC_d.f_y, tempa, robotA_d.f_x,
                                            robotA_d.f_y); // 估算后 球对地面的位置 在地面坐标系里
                float tempradius = robot__ball_radius * cos(tempangle * 3.14 / 180.0);
                float tempradius_2 = tempradius;
                t_fxx = l2f_x(ballC_d.f_x, ballC_d.f_y, 0, tempa, tempradius); // 估算后 球对地面的位置 在地面坐标系里
                t_fyy = l2f_y(ballC_d.f_x, ballC_d.f_y, 0, tempa, tempradius);
                tempangle = f2l_angle(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx,
                                      t_fyy); // 估算后 球对地面的位置 在地面坐标系里
                tempradius = f2l_radius(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx,
                                        t_fyy); // 估算后 球对地面的位置 在地面坐标系里
                //			  t_fxx		 = l2f_x
                //(robotA_d.f_x,robotA_d.f_y,robotA_d.f_a,tempangle,tempradius*v_p);//估算后 球对地面的位置
                // 在地面坐标系里 			  t_fyy		 = l2f_y
                // (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a,tempangle,tempradius*v_p);
                t_fxx = l2f_x(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, tempangle,
                              tempradius); // 估算后 球对地面的位置 在地面坐标系里
                t_fyy = l2f_y(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, tempangle, tempradius);

                //	}
                //

                /*
                float	v_ball = f2l_radius(ball_f_x_h [0],ball_f_y_h [0],0, ball_f_x_h [5],ball_f_y_h [5])/5.0/0.33;
                Diplay_A=v_ball;//
                float	v_robot = 0.5;
                float   time=f2l_radius(t_fxx,t_fyy,0, robotA_d.f_x,robotA_d.f_y)/v_robot;
                float   radius_add=time*v_ball+(v_ball+0.7)*robot__ball_radius;
                     t_fxx = l2f_x     (ballC_sx,ballC_sy,0, tempa,ball__Sball_radius+radius_add);//估算后
                球对地面的位置 在地面坐标系里 t_fyy = l2f_y     (ballC_sx,ballC_sy,0,
                tempa,ball__Sball_radius+radius_add);
                */

                // float tempr = f2l_radius(ballC_sx,ballC_sy,0, ballC_d.f_x,ballC_d.f_y);
                //	float t_fxx = l2f_x     (ballC_sx,ballC_sy,0, tempa,robot_Sball_radius);//估算后 球对地面的位置
                // 在地面坐标系里 	float t_fyy = l2f_y     (ballC_sx,ballC_sy,0, tempa,robot_Sball_radius);
                if (0)
                {
                    float rb_dis = ball__Sball_radius - robot_Sball_radius; // 当前 球C~机器人A x 距离
                    float rb_d_dis = rb_dis - rb_dis_p;
                    if (rb_d_dis > 0.20)
                        rb_d_dis = 0.20;
                    if (rb_d_dis < -0.20)
                        rb_d_dis = -0.20;
                    v_radius_set += (rb_d_dis + rb_dis * 0.0179) * 30.12; // 设置速度
                    // v_radius_set +=(rb_d_dis*1.2 + rb_dis*0.012 - 0.042*1.25/MK )*30.12* MK /1.25;//28  ;//设置速度

                    rb_dis_p = rb_dis;
                }
                else
                {
                    float rb_dis = -tempradius_2; // 当前 球C~机器人A x 距离
                    //		Display_A = rb_dis;
                    float rb_d_dis = rb_dis - rb_dis_p;
                    //		Display_B = rb_d_dis;
                    // float	display_c = rb_dis*rb_dis*rb_dis;
                    //		if (display_c> 1.2)		display_c= 1.2;
                    //		if (display_c<-1.2)		display_c=-1.2;
                    if (rb_d_dis > 0.10)
                        rb_d_dis = 0.10;
                    if (rb_d_dis < -0.10)
                        rb_d_dis = -0.10;
                    //		v_radius_set +=(rb_d_dis + rb_dis*0.0179)*30.12;//设置速度
                    v_radius_set += (rb_d_dis + rb_dis * 0.0179) * 100.12; // 设置速度
                    //		v_radius_set +=(rb_d_dis + 0)*30.12;//设置速度
                    //		v_radius_set +=(rb_d_dis + display_c*0.0006)*60.12;//设置速度
                    // v_radius_set +=(rb_d_dis*1.2 + rb_dis*0.012 - 0.042*1.25/MK )*30.12* MK /1.25;//28  ;//设置速度
                    if (v_radius_set > 60)
                        v_radius_set = 60;
                    if (v_radius_set < -60)
                        v_radius_set = -60;
                    rb_dis_p = rb_dis;
                }
                // v_radius_set =0;//ceshi duizhun!!!!!!!!!!!!!!!!!

                float t_angle1 = f2l_angle(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx, t_fyy);
                float t_radius1 = f2l_radius(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx, t_fyy) *
                                  100.0; // 1.2* MK /1.25;//源坐标系是m（米）制还要换成厘米制*100.0

                float robot_dx = 0;     // 0.20;//; //前后方向 前为正
                float robot_dy = 0.018; // 0.025;//0.15;//; //左右方向 左为正
                //	if(t_angle1<0)
                //		robot_dy=-0.000;
                //	else
                //		robot_dy= 0.024;

                float robot_move_angle =
                    f2l_angle(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, move_Emu_robot_Sf_x, move_Emu_robot_Sf_y);
                // 调节每台机体 左右对准偏移 修正量
                // 如果 接球碰机体 左边 给-值做修正
                // 如果 接球碰机体 右边 给+值做修正
                if (robot_move_angle > 0) //+right -left
                    robot_dy = -0.039;    //.033//3/ -0.053;// -0.033;//L 0.003;//-0.000;
                else
                    robot_dy = -0.015; // 3/ -0.015;// -0.024;//R 0.024;//

                float robot_da = coordinate_transfer_angle(robot_dx, robot_dy);
                float robot_dr = coordinate_transfer_radius(robot_dx, robot_dy);
                //	和f2l_angle (ballC_sx,ballC_sy,0, robotA_d.f_x,robotA_d.f_y);
                float robotTx = l2f_x(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, robot_da, robot_dr);
                float robotTy = l2f_y(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, robot_da, robot_dr);

                t_angle1 = f2l_angle(robotTx, robotTy, robotA_d.f_a, t_fxx, t_fyy);
                t_radius1 = f2l_radius(robotTx, robotTy, robotA_d.f_a, t_fxx, t_fyy) *
                            100.0; // 1.2* MK /1.25;//源坐标系是m（米）制还要换成厘米制*100.0

                float ppt = 0.15; // 左右分别加的系数
                float ppa = t_angle1;
                //		float ppa=angle_add_transfer(f2l_angle (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a, t_fxx,t_fyy)
                //									,f2l_angle (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a,
                // ballC_sx,ballC_sy));
                float ptemp = 1.0; // 生成的系数
                if ((-180 < ppa) && (ppa > -90))
                    ptemp = 1 - (ppt * (-180 - ppa) / 90.0);
                else if ((-90 < ppa) && (ppa > 90))
                    ptemp = 1 - (ppt * ppa / 90.0);
                else if ((90 < ppa) && (ppa > 180))
                    ptemp = 1 - (ppt * (180 - ppa) / 90.0);
                // t_radius1=t_radius1*ptemp;

                float t_radius1P = (3.0 - fabs(robot__ball_radius)) / 3.0;
                if (t_radius1P < 0)
                    t_radius1P = 0;

                t_radius1 = t_radius1 * (t_radius1P * 0.5 + 0.26) * 2.7; //(t_radius1P*0.4+0.36)
                if (t_radius1 > 39)
                    t_radius1 = 39;

                float t_angle2 = angle_add_transfer2(
                    tempa, -robotA_d.f_a); // f2l_angle (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a, ballC_sx,ballC_sy);
                float t_radius2 = v_radius_set;
                t_radius2 = t_radius2; // 0.045;//
                if (t_radius2 > 100)
                    t_radius2 = 100;
                if (t_radius2 < -100)
                    t_radius2 = -100;

                for (i = 20 - 1; i > 0; i--) // 存储上100周期电机的发送转速
                {
                    t_radius1_h[i] = t_radius1_h[i - 1];
                    t_radius2_h[i] = t_radius2_h[i - 1];
                }
                t_radius1_h[0] = t_radius1;
                t_radius2_h[0] = t_radius2;
                //	/*
                //	t_radius1=t_radius1_h[0]*0.7
                //			 +t_radius1_h[1]*0.3
                //			 +t_radius1_h[2]*0.2
                //			 +t_radius1_h[3]*0.1;
                t_radius2 = t_radius2_h[0] * 0.35 + t_radius2_h[1] * 0; // 0.1;//0.25
                +t_radius2_h[2] * 0;                                    // 0.1;//0.15
                +t_radius2_h[3] * 0;                                    // 0.05;//0.09
                +t_radius2_h[4] * 0;                                    // 0.05;//0.06
                +t_radius2_h[5] * 0;                                    // 0.025;//0.05
                +t_radius2_h[6] * 0;                                    // 0.025;//0.05;
                //	*/

                float t_angle = move_test_C.vector_add_angle(t_angle1, t_radius1, t_angle2, t_radius2);
                float t_radius = move_test_C.vector_add_length(t_angle1, t_radius1, t_angle2, t_radius2);

                m_find.pField->SetAgent(5, t_fxx * 100, t_fyy * 100, 0); // 场子正中向上  逆时针坐标系
                // m_find.pField->SetBall (7,ballA .f_x*100 ,ballA .f_y*100);
                //	t_radius=t_radius*0.8;
                if (t_radius > 100)
                    t_radius = 100;

                // 注意 这里为了 减少平移中转动 可以在朝向球运动反方向的方向上 为中心 偏差除固定系数
                //	float turn  =ball_angle*0.6;//1.2;//1.2;//0.35;//*turn_p;
                float turn = f2l_angle(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, ballC_sx, ballC_sy) *
                             0.6; // 1.2;//1.2;//0.35;//*turn_p;
                //	float turn  =(f2l_angle (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a,
                // ballC_sx,ballC_sy)+1.2)*0.6;//1.2;//1.2;//0.35;//*turn_p;

                //	float turn_p=(39-t_radius1)/39.0;
                float turn_p = (robot_Sball_radius - robot__ball_radius) / robot_Sball_radius;
                if (turn_p < 0)
                    turn_p = 0;
                // if (turn>10*turn_p)
                //	turn=10*turn_p;
                // if (turn<-10*turn_p)
                //	turn=-10*turn_p;
                double difft = t_radius;
                double diff = fabs(t_radius);
                double output = 0;
                double x1 = 10;       //
                double y1 = x1 * 1.6; //
                double k1 = 1.45;
                if (diff >= x1)
                    output = k1 * (diff - x1) + y1;
                else
                    output = y1 / x1 * diff;

                if (difft != diff)
                    output = -output;
                //	turn=0;
                move_test_C.limit_parameter_set_angle(
                    18 * turn_p, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                move_test_C.limit_parameter_set_dis(100, 15); //(50,3.5);//(60 , 24);//(62.0 , 15);//(39.0 ,
                                                              // 12);//(21,1);//3(128,328);//float velocity_limit_,float
                                                              // acceleration_limit_ 			move_test_C
                //.initializing_movement(turn,t_radius*1.5,t_angle);//1.5(turn,t_radius*1.2,t_angle);//(float turn,float
                // front,float angle)
                move_test_C.initializing_movement(
                    turn, output, t_angle); // 1.5(turn,t_radius*1.2,t_angle);//(float turn,float front,float angle)
                move_test_C.turn_v_maxlimit();
                move_test_C.front_v_maxlimit();
                move_test_C.front_a_maxlimit();
                move_test_C.uptech_output_O(&mb, &ml, &mr); // ？？？？？？？？？？？？？？

                //			move_test_C
                //.initializing_movement(0,t_radius*1.1,t_angle);//(turn,t_radius*1.2,t_angle);//(float turn,float
                // front,float angle) 			move_test_C .uptech_output_O(&mb,&ml,&mr);//？？？？？？？？？？？？？？

                //	turn=0;
                //	move_test_C  .O_movement_analyze(t_angle,t_radius*1.2,&ml,&mb,&mr);//全向解析
                //	ml=((turn-ml)*max_wheel_linear_speed)*wheel_linear_speed_ratio/128.0;
                //	mb=((turn-mb)*max_wheel_linear_speed)*wheel_linear_speed_ratio/128.0;
                //	mr=((turn-mr)*max_wheel_linear_speed)*wheel_linear_speed_ratio/128.0;
            }
            else
            {
                move_test_C.limit_parameter_set_angle(
                    11, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                move_test_C.initializing_movement(ball_angle * 0.6, 0, 0); //(float turn,float front,float angle)
                move_test_C.uptech_output_O(&mb, &ml, &mr);                // ？？？？？？？？？？？？？？
                //	mb=0;ml=0;mr=0;
            }
            // if(xingwei==30)
            //{
            float p = 0.33; // 0.255;//0.24;//35;//0.6;//1.0;
            mtb = mb * p;
            mtl = ml * p;
            mtr = mr * p; // 设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000
                          // = 2转/秒
            //		mtb=0; mtl=0; mtr=0;//设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1
            // 是2000/2 即发2000 = 2转/秒
            //}
            ///*
            xingwei1mb = mtb;
            xingwei1ml = mtl;
            xingwei1mr = mtr;
            //			motospeed[0]=mtl;
            //			motospeed[1]=mtr;
            //			motospeed[2]=mtb;
            //			motospeed[3]=9000;
            //			motospeed[4]=-9000;
            // 接球
            motospeed[3] = 16000;
            motospeed[4] = -16000;
            //			zhuan=0;//鑫鑫程序里的转动变量设置成0
            if (m_pAD[0] > 2000 && m_pAD[1] > 2000 && y.ball_chang < 55 && abs(y.ball_jiao) < 20)
            {
                //	m_pCmd->Kick(4000);
                jieqiuys = 158;
            }
            //*/
            Emu_moto_L = mtl;
            Emu_moto_R = mtr;
            Emu_moto_B = mtb;
        }
        else
        {
            mtb = 0;
            mtl = 0;
            mtr = 0;
            xingwei1mb = mtb;
            xingwei1ml = mtl;
            xingwei1mr = mtr;
            Emu_moto_L = mtl;
            Emu_moto_R = mtr;
            Emu_moto_B = mtb;
            // 接球
            motospeed[3] = 0;
            motospeed[4] = 0;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (xingwei != 30) // xingwei==1)//2012接球
    {
        catch_start_wait_loop = 0;
        catch_first_view_ball_X = 0;
        catch_first_view_ball_Y = 0;
        catch_first_view_ball_W = 0;

        jieqiuqs = 0;
        jieqiusudu_s = 0;
        jieqiuc_s;
        jieqiujf = 0;
    }

    if (0) // xingwei==30)//xingwei==1)//2012接球
    {
        // 接球
        //	float robot_dx=0;//0.20;//; //前后方向 前为正
        //	float robot_dy=0;//0.15;//; //左右方向 左为正
        //	float robot_da=coordinate_transfer_angle(robot_dx,robot_dy);
        //	float robot_dr=coordinate_transfer_radius(robot_dx,robot_dy);
        //	和f2l_angle (ballC_sx,ballC_sy,0, robotA_d.f_x,robotA_d.f_y);
        if ((y.ball_zhong > 10) && (catch_first_view_ball_W == 0))
        {
            catch_first_view_ball_X = move_Emu_ballX;
            catch_first_view_ball_Y = move_Emu_ballY;
            catch_first_view_ball_W = y.ball_zhong;
        }
        if (catch_first_view_ball_W > 10)
        {
            move_Emu_ballSX = catch_first_view_ball_X;
            move_Emu_ballSY = catch_first_view_ball_Y;
        }
        //	move_Emu_ballSX=18.00;//170;//t_fxx*100.0;
        //	move_Emu_ballSY=0;//1130;//t_fyy*100.0;
        ballC_sx = move_Emu_ballSX;
        ballC_sy = move_Emu_ballSY;

        obj_properties robotA_d;
        obj_properties ballC_d;
        robotA_d.f_x = move_Emu_robot_f_x; // y.benjix*0.01;
        robotA_d.f_y = move_Emu_robot_f_y; // y.benjiy*0.01;
        robotA_d.f_a = move_Emu_robot_f_a; //-y.benjijiao;
        ballC_d.f_x = move_Emu_ballX;      // y.quanbx*0.01;
        ballC_d.f_y = move_Emu_ballY;      // y.quanby*0.01;
        //	float robotTx= l2f_x (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a, robot_da,robot_dr);
        //	float robotTy= l2f_y (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a, robot_da,robot_dr);
        //	robotA_d.f_x=robotTx;
        //	robotA_d.f_y=robotTy;

        //	m_find.pField->SetAgent(5,robotTx*100 ,robotTy*100,0);//场子正中向上  逆时针坐标系
        float mb = 0;
        float ml = 0;
        float mr = 0;

        // 注意 需要用拟合等方式 先确认速度方向(10桢间取差直线拟合)基本是向本机的再开始此程序 否则接了也接不着
        // 使用f2l_angle (ballC_sx,ballC_sy,0, ballC_d .f_x,ballC_d .f_y);
        //	和f2l_angle (ballC_sx,ballC_sy,0, robotA_d.f_x,robotA_d.f_y);
        //   对比看机器和球的飞行角差多少要差90度(或者60度)以上就追球就行了
        float robot_Sball_radius = f2l_radius(ballC_sx, ballC_sy, 0, robotA_d.f_x, robotA_d.f_y);
        float ball__Sball_radius = f2l_radius(ballC_sx, ballC_sy, 0, ballC_d.f_x, ballC_d.f_y);
        float robot__ball_radius = f2l_radius(ballC_d.f_x, ballC_d.f_y, 0, robotA_d.f_x, robotA_d.f_y);
        for (i = 20 - 1; i > 0; i--) // 存储上100周期电机的发送转速
        {
            ball_f_x_h[i] = ball_f_x_h[i - 1];
            ball_f_y_h[i] = ball_f_y_h[i - 1];
        }
        ball_f_x_h[0] = ballC_d.f_x;
        ball_f_y_h[0] = ballC_d.f_y;
        ball_f_hw[0] = 10;
        ball_f_hw[1] = 5;
        ball_f_hw[2] = 5;
        ball_f_hw[3] = 4;
        ball_f_hw[4] = 4;
        ball_f_hw[5] = 4;
        ball_f_hw[6] = 3;
        ball_f_hw[7] = 3;
        ball_f_hw[8] = 2;
        ball_f_hw[9] = 2;

        if (catch_start_wait_loop >= 6) // 接球 队员 切接球后 等待球速度方向稳定 等待周期计数
        {
            catch_start_wait_loop = 6;

            float tempa;
            if (0)
            {
                if (1) // 对发球点是否做特殊 加高权值处理
                {
                    ball_f_x_h[9] = ballC_sx;
                    ball_f_y_h[9] = ballC_sy;
                    ball_f_hw[9] = 10; // pow(2, 9);
                    tempa = least_square_method(ball_f_x_h, ball_f_y_h, ball_f_hw, 10);
                }
                else
                {
                    tempa = least_square_method(ball_f_x_h, ball_f_y_h, ball_f_hw, 9);
                }
            }
            else
            {
                tempa = f2l_angle(ballC_sx, ballC_sy, 0, ballC_d.f_x, ballC_d.f_y);
            }
            // for(i=400-1;i>0;i--)  {debug[i]=debug[i-1];}debugP++;  debug[0]=tempa;
            // debug[debugP]=tempa;
            // debug2[debugP]=move_Emu_ballX;
            // debug3[debugP]=move_Emu_ballY;
            // debugP++;
            if ((catch_first_view_ball_W > 10) && (ball__Sball_radius > 2.0)) //<<<<-------ball__Sball_radius>2.0
            {

                //	for(i=0 ; i<10 ; i++)//生成最小 二乘 权值    ？？？？？？
                //	{
                //		ball_f_hw [i] = (10-i)*0.1;//pow(1+(robot_Sball_radius-robot__ball_radius)/robot_Sball_radius,
                // 10-i);
                //	}

                float tempv = f2l_radius(ball_f_x_h[0], ball_f_y_h[0], 0, ball_f_x_h[9], ball_f_y_h[9]);
                //		display_a=tempv;
                // v_p对速度加权处理 速度大于0.5m/s以上方向好判断以下不好判断
                // v_p对t_fxx,t_fyy的位置产生影响 也应该对 球速度方向产生影响
                float v_p = fabs(tempv) / 0.5;
                if (v_p > 1)
                    v_p = 1;

                // least_square_method(ball_f_x_h,ball_f_y_h,ball_f_hw,10);
                if (fabs(angle_add_transfer2(
                        tempa, -f2l_angle(ball_f_x_h[9], ball_f_y_h[9], 0, ballC_d.f_x, ballC_d.f_y))) >= 90)
                    tempa = angle_add_transfer2(tempa, 180);

                // float tempa = f2l_angle (ballC_sx,ballC_sy,0, ballC_d.f_x,ballC_d.f_y);//估算后 球对地面的位置
                // 在地面坐标系里
                float t_fxx;
                float t_fyy;
                if (0)
                { // 使用发球 球位置生成机体跑位位置 误差取决于发球球位置
                    float tempangle = f2l_angle(ballC_sx, ballC_sy, tempa, robotA_d.f_x,
                                                robotA_d.f_y); // 估算后 球对地面的位置 在地面坐标系里
                    float tempradius = robot_Sball_radius * cos(tempangle * 3.14 / 180.0);
                    t_fxx = l2f_x(ballC_sx, ballC_sy, 0, tempa, tempradius); // 估算后 球对地面的位置 在地面坐标系里
                    t_fyy = l2f_y(ballC_sx, ballC_sy, 0, tempa, tempradius);
                }
                if (0)
                { // 使用当前 球位置生成机体跑位位置 误差取决于当前球位置 可以参考机器看到最近几桢球的
                  // 位置和移动距离看和拟和直线的误差判断球是否在直线运动
                    float tempangle = f2l_angle(ballC_sx, ballC_sy, tempa, robotA_d.f_x,
                                                robotA_d.f_y); // 估算后 球对地面的位置 在地面坐标系里
                    float tempradius = robot_Sball_radius * cos(tempangle * 3.14 / 180.0);
                    float temp2radius = tempradius - ball__Sball_radius;
                    t_fxx =
                        l2f_x(ballC_d.f_x, ballC_d.f_y, 0, tempa, temp2radius); // 估算后 球对地面的位置 在地面坐标系里
                    t_fyy = l2f_y(ballC_d.f_x, ballC_d.f_y, 0, tempa, temp2radius);
                }
                //	if (1)
                //	{	//使用当前 球位置生成机体跑位位置 误差取决于当前球位置 可以参考机器看到最近几桢球的
                // 位置和移动距离看和拟和直线的误差判断球是否在直线运动
                float tempangle = f2l_angle(ballC_d.f_x, ballC_d.f_y, tempa, robotA_d.f_x,
                                            robotA_d.f_y); // 估算后 球对地面的位置 在地面坐标系里
                float tempradius = robot__ball_radius * cos(tempangle * 3.14 / 180.0);
                float tempradius_2 = tempradius;
                t_fxx = l2f_x(ballC_d.f_x, ballC_d.f_y, 0, tempa, tempradius); // 估算后 球对地面的位置 在地面坐标系里
                t_fyy = l2f_y(ballC_d.f_x, ballC_d.f_y, 0, tempa, tempradius);
                tempangle = f2l_angle(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx,
                                      t_fyy); // 估算后 球对地面的位置 在地面坐标系里
                tempradius = f2l_radius(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx,
                                        t_fyy); // 估算后 球对地面的位置 在地面坐标系里
                //			  t_fxx		 = l2f_x
                //(robotA_d.f_x,robotA_d.f_y,robotA_d.f_a,tempangle,tempradius*v_p);//估算后 球对地面的位置
                // 在地面坐标系里 			  t_fyy		 = l2f_y
                // (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a,tempangle,tempradius*v_p);
                t_fxx = l2f_x(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, tempangle,
                              tempradius); // 估算后 球对地面的位置 在地面坐标系里
                t_fyy = l2f_y(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, tempangle, tempradius);

                //	if(jieqiuqs>=5 &&
                //((jieqiux-y.quanbx)*(jieqiux-y.quanbx)+(jieqiuy-y.quanby)*(jieqiuy-y.quanby)>20000))
                if (1)
                {
                    double jdx = t_fxx * 100.0;
                    double jdy = t_fyy * 100.0;
                    float mb;
                    float ml;
                    float mr;
                    double t_angle1;
                    double t_radius1;
                    t_angle1 = -f2l_angle(robotA_d.f_x * 100.0, robotA_d.f_y * 100.0, robotA_d.f_a, jdx,
                                          jdy); //(y.benjix,y.benjiy,-y.benjijiao, jdx,jdy);
                    t_radius1 = f2l_radius(robotA_d.f_x * 100.0, robotA_d.f_y * 100.0, robotA_d.f_a, jdx,
                                           jdy); //(y.benjix,y.benjiy,-y.benjijiao, jdx,jdy);
                    //		move_test_C  .initializing_movement(0,30,0);//(float turn,float front,float angle)
                    //		move_test_C  .uptech_output_O(&mb,&ml,&mr);//？？？？？？？？？？？？？？
                    double jieqiusudu = abs(t_radius1) * 133.0;

                    if (jieqiusudu > 6000)
                        jieqiusudu = 6000;
                    else
                    {
                        if (jieqiusudu > 0 && jieqiusudu < 800)
                            jieqiusudu = 800;
                        if (jieqiusudu < 0 && jieqiusudu > -800)
                            jieqiusudu = -800;
                    }
                    if (jieqiusudu > jieqiusudu_s + 300)
                        jieqiusudu = jieqiusudu_s + 300;
                    if (jieqiusudu < jieqiusudu_s - 500)
                        jieqiusudu = jieqiusudu_s - 500;

                    float t_angle2 =
                        -f2l_angle(robotA_d.f_x * 100.0, robotA_d.f_y * 100.0, robotA_d.f_a, move_Emu_ballSX * 100.0,
                                   move_Emu_ballSY * 100.0); //(y.benjix,y.benjiy,-y.benjijiao, jieqiux,jieqiuy);
                    zhuan = 2 * duizhun(t_angle2, 0, 500);

                    if (y.ball_chang < 110)
                        Controlmoto(90, y.ball_jiao * 120, 20000, &mtl, &mtr, &mtb);

                    //	Controlmoto(90,(m_pAD[0]-m_pAD[1])*2,20000,&mtl,&mtr,&mtb);
                    if (y.ball_chang < 130)
                    {
                        // cs			jieqiujf+=(jieqiuc_s-y.ball_chang-2)*100;
                        // cs			Controlmoto(180,jieqiujf,20000,&mtl,&mtr,&mtb);
                    }

                    Controlmoto(t_angle1, jieqiusudu, 20000, &mtl, &mtr, &mtb);
                    double hcjiao;
                    double hcchang;
                    //		vhecheng(90,(m_pAD[0]-m_pAD[1])*2,20000,t_angle1,jieqiusudu,20000,&hcjiao,&hcchang);
                    //		Controlmoto(hcjiao,hcchang,20000,&mtl,&mtr,&mtb);
                    //	float	p=0.33;//0.255;//0.24;//35;//0.6;//1.0;
                    //	mtb=mb*p; mtl=ml*p; mtr=mr*p;//设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是
                    // 1000/1  是2000/2  即发2000 = 2转/秒
                    motospeed[3] = 16000;
                    motospeed[4] = -16000;
                    jieqiusudu_s = jieqiusudu;
                    m_find.pField->SetAgent(5, jdx, jdy, 0); //(5,900,600,0);//
                }
                //	else
                //	{
                //		zhuan=0;
                //		Controlmoto(0,0,20000,&mtl,&mtr,&mtb);
                //	}
                //	jieqiuc_s=y.ball_chang;

                //	}
                //

                /*
                float	v_ball = f2l_radius(ball_f_x_h [0],ball_f_y_h [0],0, ball_f_x_h [5],ball_f_y_h [5])/5.0/0.33;
                Diplay_A=v_ball;//
                float	v_robot = 0.5;
                float   time=f2l_radius(t_fxx,t_fyy,0, robotA_d.f_x,robotA_d.f_y)/v_robot;
                float   radius_add=time*v_ball+(v_ball+0.7)*robot__ball_radius;
                     t_fxx = l2f_x     (ballC_sx,ballC_sy,0, tempa,ball__Sball_radius+radius_add);//估算后
                球对地面的位置 在地面坐标系里 t_fyy = l2f_y     (ballC_sx,ballC_sy,0,
                tempa,ball__Sball_radius+radius_add);
                */

                // float tempr = f2l_radius(ballC_sx,ballC_sy,0, ballC_d.f_x,ballC_d.f_y);
                //	float t_fxx = l2f_x     (ballC_sx,ballC_sy,0, tempa,robot_Sball_radius);//估算后 球对地面的位置
                // 在地面坐标系里 	float t_fyy = l2f_y     (ballC_sx,ballC_sy,0, tempa,robot_Sball_radius);
                if (0)
                {
                    float rb_dis = ball__Sball_radius - robot_Sball_radius; // 当前 球C~机器人A x 距离
                    float rb_d_dis = rb_dis - rb_dis_p;
                    if (rb_d_dis > 0.20)
                        rb_d_dis = 0.20;
                    if (rb_d_dis < -0.20)
                        rb_d_dis = -0.20;
                    v_radius_set += (rb_d_dis + rb_dis * 0.0179) * 30.12; // 设置速度
                    // v_radius_set +=(rb_d_dis*1.2 + rb_dis*0.012 - 0.042*1.25/MK )*30.12* MK /1.25;//28  ;//设置速度

                    rb_dis_p = rb_dis;
                }
                else
                {
                    float rb_dis = -tempradius_2; // 当前 球C~机器人A x 距离
                    //		Display_A = rb_dis;
                    float rb_d_dis = rb_dis - rb_dis_p;
                    //		Display_B = rb_d_dis;
                    // float	display_c = rb_dis*rb_dis*rb_dis;
                    //		if (display_c> 1.2)		display_c= 1.2;
                    //		if (display_c<-1.2)		display_c=-1.2;
                    if (rb_d_dis > 0.10)
                        rb_d_dis = 0.10;
                    if (rb_d_dis < -0.10)
                        rb_d_dis = -0.10;
                    //		v_radius_set +=(rb_d_dis + rb_dis*0.0179)*30.12;//设置速度
                    v_radius_set += (rb_d_dis + rb_dis * 0.0179) * 100.12; // 设置速度
                    //		v_radius_set +=(rb_d_dis + 0)*30.12;//设置速度
                    //		v_radius_set +=(rb_d_dis + display_c*0.0006)*60.12;//设置速度
                    // v_radius_set +=(rb_d_dis*1.2 + rb_dis*0.012 - 0.042*1.25/MK )*30.12* MK /1.25;//28  ;//设置速度
                    if (v_radius_set > 60)
                        v_radius_set = 60;
                    if (v_radius_set < -60)
                        v_radius_set = -60;
                    rb_dis_p = rb_dis;
                }
                // v_radius_set =0;//ceshi duizhun!!!!!!!!!!!!!!!!!

                float t_angle1 = f2l_angle(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx, t_fyy);
                float t_radius1 = f2l_radius(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx, t_fyy) *
                                  100.0; // 1.2* MK /1.25;//源坐标系是m（米）制还要换成厘米制*100.0

                float robot_dx = 0;     // 0.20;//; //前后方向 前为正
                float robot_dy = 0.018; // 0.025;//0.15;//; //左右方向 左为正
                //	if(t_angle1<0)
                //		robot_dy=-0.000;
                //	else
                //		robot_dy= 0.024;

                float robot_move_angle =
                    f2l_angle(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, move_Emu_robot_Sf_x, move_Emu_robot_Sf_y);
                // 调节每台机体 左右对准偏移 修正量
                // 如果 接球碰机体 左边 给-值做修正
                // 如果 接球碰机体 右边 给+值做修正
                if (robot_move_angle > 0) //+right -left
                    robot_dy = 0;         // 4/+0.000;//2//-0.039;//  .033//3/ -0.053;// -0.033;//L 0.003;//-0.000;
                else
                    robot_dy = 0; // 4/+0.045;//2//-0.015;//  3/ -0.015;// -0.024;//R 0.024;//

                float robot_da = coordinate_transfer_angle(robot_dx, robot_dy);
                float robot_dr = coordinate_transfer_radius(robot_dx, robot_dy);
                //	和f2l_angle (ballC_sx,ballC_sy,0, robotA_d.f_x,robotA_d.f_y);
                float robotTx = l2f_x(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, robot_da, robot_dr);
                float robotTy = l2f_y(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, robot_da, robot_dr);

                t_angle1 = f2l_angle(robotTx, robotTy, robotA_d.f_a, t_fxx, t_fyy);
                t_radius1 = f2l_radius(robotTx, robotTy, robotA_d.f_a, t_fxx, t_fyy) *
                            100.0; // 1.2* MK /1.25;//源坐标系是m（米）制还要换成厘米制*100.0

                float ppt = 0.15; // 左右分别加的系数
                float ppa = t_angle1;
                //		float ppa=angle_add_transfer(f2l_angle (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a, t_fxx,t_fyy)
                //									,f2l_angle (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a,
                // ballC_sx,ballC_sy));
                float ptemp = 1.0; // 生成的系数
                if ((-180 < ppa) && (ppa > -90))
                    ptemp = 1 - (ppt * (-180 - ppa) / 90.0);
                else if ((-90 < ppa) && (ppa > 90))
                    ptemp = 1 - (ppt * ppa / 90.0);
                else if ((90 < ppa) && (ppa > 180))
                    ptemp = 1 - (ppt * (180 - ppa) / 90.0);
                // t_radius1=t_radius1*ptemp;

                float t_radius1P = (3.0 - fabs(robot__ball_radius)) / 3.0;
                if (t_radius1P < 0)
                    t_radius1P = 0;

                t_radius1 = t_radius1 * (t_radius1P * 0.5 + 0.26) * 2.7; //(t_radius1P*0.4+0.36)
                if (t_radius1 > 39)
                    t_radius1 = 39;

                float t_angle2 = angle_add_transfer2(
                    tempa, -robotA_d.f_a); // f2l_angle (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a, ballC_sx,ballC_sy);
                float t_radius2 = v_radius_set;
                t_radius2 = t_radius2; // 0.045;//
                if (t_radius2 > 100)
                    t_radius2 = 100;
                if (t_radius2 < -100)
                    t_radius2 = -100;

                for (i = 20 - 1; i > 0; i--) // 存储上100周期电机的发送转速
                {
                    t_radius1_h[i] = t_radius1_h[i - 1];
                    t_radius2_h[i] = t_radius2_h[i - 1];
                }
                t_radius1_h[0] = t_radius1;
                t_radius2_h[0] = t_radius2;
                //	/*
                //	t_radius1=t_radius1_h[0]*0.7
                //			 +t_radius1_h[1]*0.3
                //			 +t_radius1_h[2]*0.2
                //			 +t_radius1_h[3]*0.1;
                t_radius2 = t_radius2_h[0] * 0.35 + t_radius2_h[1] * 0; // 0.1;//0.25
                +t_radius2_h[2] * 0;                                    // 0.1;//0.15
                +t_radius2_h[3] * 0;                                    // 0.05;//0.09
                +t_radius2_h[4] * 0;                                    // 0.05;//0.06
                +t_radius2_h[5] * 0;                                    // 0.025;//0.05
                +t_radius2_h[6] * 0;                                    // 0.025;//0.05;
                //	*/

                float t_angle = move_test_C.vector_add_angle(t_angle1, t_radius1, t_angle2, t_radius2);
                float t_radius = move_test_C.vector_add_length(t_angle1, t_radius1, t_angle2, t_radius2);

                m_find.pField->SetAgent(5, t_fxx * 100, t_fyy * 100, 0); // 场子正中向上  逆时针坐标系
                // m_find.pField->SetBall (7,ballA .f_x*100 ,ballA .f_y*100);
                //	t_radius=t_radius*0.8;
                if (t_radius > 100)
                    t_radius = 100;

                // 注意 这里为了 减少平移中转动 可以在朝向球运动反方向的方向上 为中心 偏差除固定系数
                //	float turn  =ball_angle*0.6;//1.2;//1.2;//0.35;//*turn_p;
                float turn = f2l_angle(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, ballC_sx, ballC_sy) *
                             0.6; // 1.2;//1.2;//0.35;//*turn_p;
                //	float turn  =(f2l_angle (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a,
                // ballC_sx,ballC_sy)+1.2)*0.6;//1.2;//1.2;//0.35;//*turn_p;

                //	float turn_p=(39-t_radius1)/39.0;
                float turn_p = (robot_Sball_radius - robot__ball_radius) / robot_Sball_radius;
                if (turn_p < 0)
                    turn_p = 0;
                // if (turn>10*turn_p)
                //	turn=10*turn_p;
                // if (turn<-10*turn_p)
                //	turn=-10*turn_p;
                double difft = t_radius;
                double diff = fabs(t_radius);
                double output = 0;
                double x1 = 10;       //
                double y1 = x1 * 1.6; //
                double k1 = 1.45;
                if (diff >= x1)
                    output = k1 * (diff - x1) + y1;
                else
                    output = y1 / x1 * diff;

                if (difft != diff)
                    output = -output;
                //	turn=0;
                move_test_C.limit_parameter_set_angle(
                    10, 10); //(18*turn_p,1);//(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                move_test_C.limit_parameter_set_dis(100, 15); //(50,3.5);//(60 , 24);//(62.0 , 15);//(39.0 ,
                                                              // 12);//(21,1);//3(128,328);//float velocity_limit_,float
                                                              // acceleration_limit_ 			move_test_C
                //.initializing_movement(turn,t_radius*1.5,t_angle);//1.5(turn,t_radius*1.2,t_angle);//(float turn,float
                // front,float angle)
                move_test_C.initializing_movement(
                    turn, output, t_angle); // 1.5(turn,t_radius*1.2,t_angle);//(float turn,float front,float angle)
                move_test_C.turn_v_maxlimit();
                move_test_C.front_v_maxlimit();
                move_test_C.front_a_maxlimit();
                move_test_C.uptech_output_O(&mb, &ml, &mr); // ？？？？？？？？？？？？？？

                //			move_test_C
                //.initializing_movement(0,t_radius*1.1,t_angle);//(turn,t_radius*1.2,t_angle);//(float turn,float
                // front,float angle) 			move_test_C .uptech_output_O(&mb,&ml,&mr);//？？？？？？？？？？？？？？

                //	turn=0;
                //	move_test_C  .O_movement_analyze(t_angle,t_radius*1.2,&ml,&mb,&mr);//全向解析
                //	ml=((turn-ml)*max_wheel_linear_speed)*wheel_linear_speed_ratio/128.0;
                //	mb=((turn-mb)*max_wheel_linear_speed)*wheel_linear_speed_ratio/128.0;
                //	mr=((turn-mr)*max_wheel_linear_speed)*wheel_linear_speed_ratio/128.0;
            }
            else
            {
                move_test_C.limit_parameter_set_angle(
                    11, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                move_test_C.initializing_movement(ball_angle * 0.6, 0, 0); //(float turn,float front,float angle)
                move_test_C.uptech_output_O(&mb, &ml, &mr);                // ？？？？？？？？？？？？？？

                zhuan = 0;
                Controlmoto(0, 0, 20000, &mtl, &mtr, &mtb);
                //	mb=0;ml=0;mr=0;
            }
            // if(xingwei==30)
            //{
            float p = 0.40; // 0.33;//0.255;//0.24;//35;//0.6;//1.0;
            //			mtb=mb*p; mtl=ml*p; mtr=mr*p;//设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是
            // 1000/1  是2000/2  即发2000 = 2转/秒 		mtb=0; mtl=0; mtr=0;//设置平移速度 每个电机的变量 //严重注意
            // 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000 = 2转/秒
            //}
            ///*
            xingwei1mb = mtb;
            xingwei1ml = mtl;
            xingwei1mr = mtr;
            //			motospeed[0]=mtl;
            //			motospeed[1]=mtr;
            //			motospeed[2]=mtb;
            //			motospeed[3]=9000;
            //			motospeed[4]=-9000;
            // 接球
            motospeed[3] = 16000;
            motospeed[4] = -16000;
            //			zhuan=0;//鑫鑫程序里的转动变量设置成0
            if (m_pAD[0] > 1800 && m_pAD[1] > 1800 && y.ball_chang < 55 && abs(y.ball_jiao) < 20)
            {
                //	m_pCmd->Kick(4000);
                jieqiuys = 158;
            }
            //*/
            Emu_moto_L = mtl;
            Emu_moto_R = mtr;
            Emu_moto_B = mtb;
        }
        else
        {
            mtb = 0;
            mtl = 0;
            mtr = 0;
            xingwei1mb = mtb;
            xingwei1ml = mtl;
            xingwei1mr = mtr;
            Emu_moto_L = mtl;
            Emu_moto_R = mtr;
            Emu_moto_B = mtb;
            // 接球
            motospeed[3] = 0;
            motospeed[4] = 0;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (xingwei != 30) // xingwei==1)//2012接球cs+xxx 0)//
    {
        jieqiusudu_s = 0;
        jieqiuqs = 0;
    }
    if (0) // xingwei==30)//xingwei==1)//2012接球cs+xxx
    {
        // 接球
        if (y.ball_zhong > 10 && jieqiuqs == 0)
        {

            //	jieqiux=move_Emu_ballX;//y.quanbx;
            //	jieqiuy=move_Emu_ballY;//y.quanby;
            //	jdx=jieqiux;
            //	jdy=jieqiux;
            jieqiux = move_Emu_ballX;
            jieqiuy = move_Emu_ballY;
            move_Emu_ballSX = move_Emu_ballX;
            move_Emu_ballSY = move_Emu_ballY;
            jieqiuqs = 1;
        }

        if (y.ball_zhong > 10 && jieqiuqs < 5)
        {
            jieqiuqs++;
        }
        //	float robot_dx=0;//0.20;//; //前后方向 前为正
        //	float robot_dy=0;//0.15;//; //左右方向 左为正
        //	float robot_da=coordinate_transfer_angle(robot_dx,robot_dy);
        //	float robot_dr=coordinate_transfer_radius(robot_dx,robot_dy);
        //	和f2l_angle (ballC_sx,ballC_sy,0, robotA_d.f_x,robotA_d.f_y);
        ballC_sx = move_Emu_ballSX;
        ballC_sy = move_Emu_ballSY;
        obj_properties robotA_d;
        obj_properties ballC_d;
        robotA_d.f_x = move_Emu_robot_f_x; // y.benjix*0.01;
        robotA_d.f_y = move_Emu_robot_f_y; // y.benjiy*0.01;
        robotA_d.f_a = move_Emu_robot_f_a; //-y.benjijiao;
        ballC_d.f_x = move_Emu_ballX;      // y.quanbx*0.01;
        ballC_d.f_y = move_Emu_ballY;      // y.quanby*0.01;
        //	float robotTx= l2f_x (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a, robot_da,robot_dr);
        //	float robotTy= l2f_y (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a, robot_da,robot_dr);
        //	robotA_d.f_x=robotTx;
        //	robotA_d.f_y=robotTy;

        //	m_find.pField->SetAgent(5,robotTx*100 ,robotTy*100,0);//场子正中向上  逆时针坐标系
        float mb = 0;
        float ml = 0;
        float mr = 0;

        // 注意 需要用拟合等方式 先确认速度方向(10桢间取差直线拟合)基本是向本机的再开始此程序 否则接了也接不着
        // 使用f2l_angle (ballC_sx,ballC_sy,0, ballC_d .f_x,ballC_d .f_y);
        //	和f2l_angle (ballC_sx,ballC_sy,0, robotA_d.f_x,robotA_d.f_y);
        //   对比看机器和球的飞行角差多少要差90度(或者60度)以上就追球就行了
        float robot_Sball_radius = f2l_radius(ballC_sx, ballC_sy, 0, robotA_d.f_x, robotA_d.f_y);
        float ball__Sball_radius = f2l_radius(ballC_sx, ballC_sy, 0, ballC_d.f_x, ballC_d.f_y);
        float robot__ball_radius = f2l_radius(ballC_d.f_x, ballC_d.f_y, 0, robotA_d.f_x, robotA_d.f_y);
        for (i = 20 - 1; i > 0; i--) // 存储上100周期电机的发送转速
        {
            ball_f_x_h[i] = ball_f_x_h[i - 1];
            ball_f_y_h[i] = ball_f_y_h[i - 1];
        }
        ball_f_x_h[0] = ballC_d.f_x;
        ball_f_y_h[0] = ballC_d.f_y;
        ball_f_hw[0] = 10;
        ball_f_hw[1] = 5;
        ball_f_hw[2] = 5;
        ball_f_hw[3] = 4;
        ball_f_hw[4] = 4;
        ball_f_hw[5] = 4;
        ball_f_hw[6] = 3;
        ball_f_hw[7] = 3;
        ball_f_hw[8] = 2;
        ball_f_hw[9] = 2;

        if (1) // catch_start_wait_loop>=6)//接球 队员 切接球后 等待球速度方向稳定 等待周期计数
        {      // catch_start_wait_loop =6;

            float tempa;
            if (0)
            {
                if (1) // 对发球点是否做特殊 加高权值处理
                {
                    ball_f_x_h[9] = ballC_sx;
                    ball_f_y_h[9] = ballC_sy;
                    ball_f_hw[9] = 10; // pow(2, 9);
                    tempa = least_square_method(ball_f_x_h, ball_f_y_h, ball_f_hw, 10);
                }
                else
                {
                    tempa = least_square_method(ball_f_x_h, ball_f_y_h, ball_f_hw, 9);
                }
            }
            else
            {
                tempa = f2l_angle(move_Emu_ballSX, move_Emu_ballSY, 0, move_Emu_ballX, move_Emu_ballY);
            }
            // for(i=400-1;i>0;i--)  {debug[i]=debug[i-1];}debugP++;  debug[0]=tempa;
            // debug[debugP]=tempa;
            // debug2[debugP]=move_Emu_ballX;
            // debug3[debugP]=move_Emu_ballY;
            // debugP++;
            if (y.ball_zhong > 10 && jieqiuqs >= 5 && ball__Sball_radius > 1.0)
            {

                //	for(i=0 ; i<10 ; i++)//生成最小 二乘 权值    ？？？？？？
                //	{
                //		ball_f_hw [i] = (10-i)*0.1;//pow(1+(robot_Sball_radius-robot__ball_radius)/robot_Sball_radius,
                // 10-i);
                //	}

                float tempv = f2l_radius(ball_f_x_h[0], ball_f_y_h[0], 0, ball_f_x_h[9], ball_f_y_h[9]);
                //		display_a=tempv;
                // v_p对速度加权处理 速度大于0.5m/s以上方向好判断以下不好判断
                // v_p对t_fxx,t_fyy的位置产生影响 也应该对 球速度方向产生影响
                float v_p = fabs(tempv) / 0.5;
                if (v_p > 1)
                    v_p = 1;

                // least_square_method(ball_f_x_h,ball_f_y_h,ball_f_hw,10);
                if (fabs(angle_add_transfer2(
                        tempa, -f2l_angle(ball_f_x_h[9], ball_f_y_h[9], 0, ballC_d.f_x, ballC_d.f_y))) >= 90)
                    tempa = angle_add_transfer2(tempa, 180);

                // float tempa = f2l_angle (ballC_sx,ballC_sy,0, ballC_d.f_x,ballC_d.f_y);//估算后 球对地面的位置
                // 在地面坐标系里
                float t_fxx;
                float t_fyy;
                if (0)
                { // 使用发球 球位置生成机体跑位位置 误差取决于发球球位置
                    float tempangle = f2l_angle(ballC_sx, ballC_sy, tempa, robotA_d.f_x,
                                                robotA_d.f_y); // 估算后 球对地面的位置 在地面坐标系里
                    float tempradius = robot_Sball_radius * cos(tempangle * 3.14 / 180.0);
                    t_fxx = l2f_x(ballC_sx, ballC_sy, 0, tempa, tempradius); // 估算后 球对地面的位置 在地面坐标系里
                    t_fyy = l2f_y(ballC_sx, ballC_sy, 0, tempa, tempradius);
                }
                if (0)
                { // 使用当前 球位置生成机体跑位位置 误差取决于当前球位置 可以参考机器看到最近几桢球的
                  // 位置和移动距离看和拟和直线的误差判断球是否在直线运动
                    float tempangle = f2l_angle(ballC_sx, ballC_sy, tempa, robotA_d.f_x,
                                                robotA_d.f_y); // 估算后 球对地面的位置 在地面坐标系里
                    float tempradius = robot_Sball_radius * cos(tempangle * 3.14 / 180.0);
                    float temp2radius = tempradius - ball__Sball_radius;
                    t_fxx =
                        l2f_x(ballC_d.f_x, ballC_d.f_y, 0, tempa, temp2radius); // 估算后 球对地面的位置 在地面坐标系里
                    t_fyy = l2f_y(ballC_d.f_x, ballC_d.f_y, 0, tempa, temp2radius);
                }
                //	if (1)
                //	{	//使用当前 球位置生成机体跑位位置 误差取决于当前球位置 可以参考机器看到最近几桢球的
                // 位置和移动距离看和拟和直线的误差判断球是否在直线运动
                float tempangle = f2l_angle(ballC_d.f_x, ballC_d.f_y, tempa, robotA_d.f_x,
                                            robotA_d.f_y); // 估算后 球对地面的位置 在地面坐标系里
                float tempradius = robot__ball_radius * cos(tempangle * 3.14 / 180.0);
                float tempradius_2 = tempradius;
                t_fxx = l2f_x(ballC_d.f_x, ballC_d.f_y, 0, tempa, tempradius); // 估算后 球对地面的位置 在地面坐标系里
                t_fyy = l2f_y(ballC_d.f_x, ballC_d.f_y, 0, tempa, tempradius);
                tempangle = f2l_angle(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx,
                                      t_fyy); // 估算后 球对地面的位置 在地面坐标系里
                tempradius = f2l_radius(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx,
                                        t_fyy); // 估算后 球对地面的位置 在地面坐标系里
                //			  t_fxx		 = l2f_x
                //(robotA_d.f_x,robotA_d.f_y,robotA_d.f_a,tempangle,tempradius*v_p);//估算后 球对地面的位置
                // 在地面坐标系里 			  t_fyy		 = l2f_y
                // (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a,tempangle,tempradius*v_p);
                t_fxx = l2f_x(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, tempangle,
                              tempradius); // 估算后 球对地面的位置 在地面坐标系里
                t_fyy = l2f_y(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, tempangle, tempradius);

                //	if(jieqiuqs>=5 &&
                //((jieqiux-y.quanbx)*(jieqiux-y.quanbx)+(jieqiuy-y.quanby)*(jieqiuy-y.quanby)>20000))
                if (1)
                {
                    double jdx = t_fxx * 100.0;
                    double jdy = t_fyy * 100.0;
                    float mb;
                    float ml;
                    float mr;
                    double t_angle1;
                    double t_radius1;
                    t_angle1 = -f2l_angle(robotA_d.f_x * 100.0, robotA_d.f_y * 100.0, robotA_d.f_a, jdx,
                                          jdy); //(y.benjix,y.benjiy,-y.benjijiao, jdx,jdy);
                    t_radius1 = f2l_radius(robotA_d.f_x * 100.0, robotA_d.f_y * 100.0, robotA_d.f_a, jdx,
                                           jdy); //(y.benjix,y.benjiy,-y.benjijiao, jdx,jdy);
                    //		move_test_C  .initializing_movement(0,30,0);//(float turn,float front,float angle)
                    //		move_test_C  .uptech_output_O(&mb,&ml,&mr);//？？？？？？？？？？？？？？
                    double jieqiusudu = abs(t_radius1) * 180.0;

                    if (jieqiusudu > 6000)
                        jieqiusudu = 6000;
                    else
                    {
                        if (jieqiusudu > 0 && jieqiusudu < 800)
                            jieqiusudu = 800;
                        if (jieqiusudu < 0 && jieqiusudu > -800)
                            jieqiusudu = -800;
                    }
                    if (jieqiusudu > jieqiusudu_s + 150)
                        jieqiusudu = jieqiusudu_s + 150;
                    if (jieqiusudu < jieqiusudu_s - 500)
                        jieqiusudu = jieqiusudu_s - 500;

                    float t_angle2 =
                        -f2l_angle(robotA_d.f_x * 100.0, robotA_d.f_y * 100.0, robotA_d.f_a, move_Emu_ballSX * 100.0,
                                   move_Emu_ballSY * 100.0); //(y.benjix,y.benjiy,-y.benjijiao, jieqiux,jieqiuy);
                    zhuan = 2 * duizhun(t_angle2, 0, 500);
                    //		if (fabs(jieqiusudu)<1000)
                    //		zhuan=3*duizhun(t_angle2,0,500);//(y.balljiao,0,1000);

                    // cs		if(y.ball_chang<110)
                    // cs			Controlmoto(90,y.ball_jiao*120,20000,&mtl,&mtr,&mtb);

                    //	Controlmoto(90,(m_pAD[0]-m_pAD[1])*2,20000,&mtl,&mtr,&mtb);
                    if (y.ball_chang < 130)
                    {
                        // cs			jieqiujf+=(jieqiuc_s-y.ball_chang-2)*100;
                        // cs			Controlmoto(180,jieqiujf,20000,&mtl,&mtr,&mtb);
                    }

                    Controlmoto(t_angle1, jieqiusudu, 20000, &mtl, &mtr, &mtb);
                    double hcjiao;
                    double hcchang;
                    //		vhecheng(90,(m_pAD[0]-m_pAD[1])*2,20000,t_angle1,jieqiusudu,20000,&hcjiao,&hcchang);
                    //		Controlmoto(hcjiao,hcchang,20000,&mtl,&mtr,&mtb);
                    //	float	p=0.33;//0.255;//0.24;//35;//0.6;//1.0;
                    //	mtb=mb*p; mtl=ml*p; mtr=mr*p;//设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是
                    // 1000/1  是2000/2  即发2000 = 2转/秒
                    motospeed[3] = 16000;
                    motospeed[4] = -16000;
                    jieqiusudu_s = jieqiusudu;
                    m_find.pField->SetAgent(5, jdx, jdy, 0); //(5,900,600,0);//
                }
                //	else
                //	{
                //		zhuan=0;
                //		Controlmoto(0,0,20000,&mtl,&mtr,&mtb);
                //	}
                //	jieqiuc_s=y.ball_chang;

                //	}
                //

                /*
                float	v_ball = f2l_radius(ball_f_x_h [0],ball_f_y_h [0],0, ball_f_x_h [5],ball_f_y_h [5])/5.0/0.33;
                Diplay_A=v_ball;//
                float	v_robot = 0.5;
                float   time=f2l_radius(t_fxx,t_fyy,0, robotA_d.f_x,robotA_d.f_y)/v_robot;
                float   radius_add=time*v_ball+(v_ball+0.7)*robot__ball_radius;
                     t_fxx = l2f_x     (ballC_sx,ballC_sy,0, tempa,ball__Sball_radius+radius_add);//估算后
                球对地面的位置 在地面坐标系里 t_fyy = l2f_y     (ballC_sx,ballC_sy,0,
                tempa,ball__Sball_radius+radius_add);
                */

                // float tempr = f2l_radius(ballC_sx,ballC_sy,0, ballC_d.f_x,ballC_d.f_y);
                //	float t_fxx = l2f_x     (ballC_sx,ballC_sy,0, tempa,robot_Sball_radius);//估算后 球对地面的位置
                // 在地面坐标系里 	float t_fyy = l2f_y     (ballC_sx,ballC_sy,0, tempa,robot_Sball_radius);
                if (0)
                {
                    float rb_dis = ball__Sball_radius - robot_Sball_radius; // 当前 球C~机器人A x 距离
                    float rb_d_dis = rb_dis - rb_dis_p;
                    if (rb_d_dis > 0.20)
                        rb_d_dis = 0.20;
                    if (rb_d_dis < -0.20)
                        rb_d_dis = -0.20;
                    v_radius_set += (rb_d_dis + rb_dis * 0.0179) * 30.12; // 设置速度
                    // v_radius_set +=(rb_d_dis*1.2 + rb_dis*0.012 - 0.042*1.25/MK )*30.12* MK /1.25;//28  ;//设置速度

                    rb_dis_p = rb_dis;
                }
                else
                {
                    float rb_dis = -tempradius_2; // 当前 球C~机器人A x 距离
                    //		Display_A = rb_dis;
                    float rb_d_dis = rb_dis - rb_dis_p;
                    //		Display_B = rb_d_dis;
                    // float	display_c = rb_dis*rb_dis*rb_dis;
                    //		if (display_c> 1.2)		display_c= 1.2;
                    //		if (display_c<-1.2)		display_c=-1.2;
                    if (rb_d_dis > 0.10)
                        rb_d_dis = 0.10;
                    if (rb_d_dis < -0.10)
                        rb_d_dis = -0.10;
                    //		v_radius_set +=(rb_d_dis + rb_dis*0.0179)*30.12;//设置速度
                    v_radius_set += (rb_d_dis + rb_dis * 0.0179) * 100.12; // 设置速度
                    //		v_radius_set +=(rb_d_dis + 0)*30.12;//设置速度
                    //		v_radius_set +=(rb_d_dis + display_c*0.0006)*60.12;//设置速度
                    // v_radius_set +=(rb_d_dis*1.2 + rb_dis*0.012 - 0.042*1.25/MK )*30.12* MK /1.25;//28  ;//设置速度
                    if (v_radius_set > 60)
                        v_radius_set = 60;
                    if (v_radius_set < -60)
                        v_radius_set = -60;
                    rb_dis_p = rb_dis;
                }
                // v_radius_set =0;//ceshi duizhun!!!!!!!!!!!!!!!!!

                float t_angle1 = f2l_angle(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx, t_fyy);
                float t_radius1 = f2l_radius(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx, t_fyy) *
                                  100.0; // 1.2* MK /1.25;//源坐标系是m（米）制还要换成厘米制*100.0

                float robot_dx = 0;     // 0.20;//; //前后方向 前为正
                float robot_dy = 0.018; // 0.025;//0.15;//; //左右方向 左为正
                //	if(t_angle1<0)
                //		robot_dy=-0.000;
                //	else
                //		robot_dy= 0.024;

                float robot_move_angle =
                    f2l_angle(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, move_Emu_robot_Sf_x, move_Emu_robot_Sf_y);
                // 调节每台机体 左右对准偏移 修正量
                // 如果 接球碰机体 左边 给-值做修正
                // 如果 接球碰机体 右边 给+值做修正
                if (robot_move_angle > 0) //+right -left
                    robot_dy = -0.039;    //.033//3/ -0.053;// -0.033;//L 0.003;//-0.000;
                else
                    robot_dy = -0.015; // 3/ -0.015;// -0.024;//R 0.024;//

                float robot_da = coordinate_transfer_angle(robot_dx, robot_dy);
                float robot_dr = coordinate_transfer_radius(robot_dx, robot_dy);
                //	和f2l_angle (ballC_sx,ballC_sy,0, robotA_d.f_x,robotA_d.f_y);
                float robotTx = l2f_x(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, robot_da, robot_dr);
                float robotTy = l2f_y(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, robot_da, robot_dr);

                t_angle1 = f2l_angle(robotTx, robotTy, robotA_d.f_a, t_fxx, t_fyy);
                t_radius1 = f2l_radius(robotTx, robotTy, robotA_d.f_a, t_fxx, t_fyy) *
                            100.0; // 1.2* MK /1.25;//源坐标系是m（米）制还要换成厘米制*100.0

                float ppt = 0.15; // 左右分别加的系数
                float ppa = t_angle1;
                //		float ppa=angle_add_transfer(f2l_angle (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a, t_fxx,t_fyy)
                //									,f2l_angle (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a,
                // ballC_sx,ballC_sy));
                float ptemp = 1.0; // 生成的系数
                if ((-180 < ppa) && (ppa > -90))
                    ptemp = 1 - (ppt * (-180 - ppa) / 90.0);
                else if ((-90 < ppa) && (ppa > 90))
                    ptemp = 1 - (ppt * ppa / 90.0);
                else if ((90 < ppa) && (ppa > 180))
                    ptemp = 1 - (ppt * (180 - ppa) / 90.0);
                // t_radius1=t_radius1*ptemp;

                float t_radius1P = (3.0 - fabs(robot__ball_radius)) / 3.0;
                if (t_radius1P < 0)
                    t_radius1P = 0;

                t_radius1 = t_radius1 * (t_radius1P * 0.5 + 0.26) * 2.7; //(t_radius1P*0.4+0.36)
                if (t_radius1 > 39)
                    t_radius1 = 39;

                float t_angle2 = angle_add_transfer2(
                    tempa, -robotA_d.f_a); // f2l_angle (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a, ballC_sx,ballC_sy);
                float t_radius2 = v_radius_set;
                t_radius2 = t_radius2; // 0.045;//
                if (t_radius2 > 100)
                    t_radius2 = 100;
                if (t_radius2 < -100)
                    t_radius2 = -100;

                for (i = 20 - 1; i > 0; i--) // 存储上100周期电机的发送转速
                {
                    t_radius1_h[i] = t_radius1_h[i - 1];
                    t_radius2_h[i] = t_radius2_h[i - 1];
                }
                t_radius1_h[0] = t_radius1;
                t_radius2_h[0] = t_radius2;
                //	/*
                //	t_radius1=t_radius1_h[0]*0.7
                //			 +t_radius1_h[1]*0.3
                //			 +t_radius1_h[2]*0.2
                //			 +t_radius1_h[3]*0.1;
                t_radius2 = t_radius2_h[0] * 0.35 + t_radius2_h[1] * 0; // 0.1;//0.25
                +t_radius2_h[2] * 0;                                    // 0.1;//0.15
                +t_radius2_h[3] * 0;                                    // 0.05;//0.09
                +t_radius2_h[4] * 0;                                    // 0.05;//0.06
                +t_radius2_h[5] * 0;                                    // 0.025;//0.05
                +t_radius2_h[6] * 0;                                    // 0.025;//0.05;
                //	*/

                float t_angle = move_test_C.vector_add_angle(t_angle1, t_radius1, t_angle2, t_radius2);
                float t_radius = move_test_C.vector_add_length(t_angle1, t_radius1, t_angle2, t_radius2);

                m_find.pField->SetAgent(5, t_fxx * 100, t_fyy * 100, 0); // 场子正中向上  逆时针坐标系
                // m_find.pField->SetBall (7,ballA .f_x*100 ,ballA .f_y*100);
                //	t_radius=t_radius*0.8;
                if (t_radius > 100)
                    t_radius = 100;

                // 注意 这里为了 减少平移中转动 可以在朝向球运动反方向的方向上 为中心 偏差除固定系数
                //	float turn  =ball_angle*0.6;//1.2;//1.2;//0.35;//*turn_p;
                float turn = f2l_angle(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, ballC_sx, ballC_sy) *
                             0.6; // 1.2;//1.2;//0.35;//*turn_p;
                //	float turn  =(f2l_angle (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a,
                // ballC_sx,ballC_sy)+1.2)*0.6;//1.2;//1.2;//0.35;//*turn_p;

                //	float turn_p=(39-t_radius1)/39.0;
                float turn_p = (robot_Sball_radius - robot__ball_radius) / robot_Sball_radius;
                if (turn_p < 0)
                    turn_p = 0;
                // if (turn>10*turn_p)
                //	turn=10*turn_p;
                // if (turn<-10*turn_p)
                //	turn=-10*turn_p;
                double difft = t_radius;
                double diff = fabs(t_radius);
                double output = 0;
                double x1 = 10;       //
                double y1 = x1 * 1.6; //
                double k1 = 1.45;
                if (diff >= x1)
                    output = k1 * (diff - x1) + y1;
                else
                    output = y1 / x1 * diff;

                if (difft != diff)
                    output = -output;
                //	turn=0;
                move_test_C.limit_parameter_set_angle(
                    18 * turn_p, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                move_test_C.limit_parameter_set_dis(100, 15); //(50,3.5);//(60 , 24);//(62.0 , 15);//(39.0 ,
                                                              // 12);//(21,1);//3(128,328);//float velocity_limit_,float
                                                              // acceleration_limit_ 			move_test_C
                //.initializing_movement(turn,t_radius*1.5,t_angle);//1.5(turn,t_radius*1.2,t_angle);//(float turn,float
                // front,float angle)
                move_test_C.initializing_movement(
                    turn, output, t_angle); // 1.5(turn,t_radius*1.2,t_angle);//(float turn,float front,float angle)
                move_test_C.turn_v_maxlimit();
                move_test_C.front_v_maxlimit();
                move_test_C.front_a_maxlimit();
                move_test_C.uptech_output_O(&mb, &ml, &mr); // ？？？？？？？？？？？？？？

                //			move_test_C
                //.initializing_movement(0,t_radius*1.1,t_angle);//(turn,t_radius*1.2,t_angle);//(float turn,float
                // front,float angle) 			move_test_C .uptech_output_O(&mb,&ml,&mr);//？？？？？？？？？？？？？？

                //	turn=0;
                //	move_test_C  .O_movement_analyze(t_angle,t_radius*1.2,&ml,&mb,&mr);//全向解析
                //	ml=((turn-ml)*max_wheel_linear_speed)*wheel_linear_speed_ratio/128.0;
                //	mb=((turn-mb)*max_wheel_linear_speed)*wheel_linear_speed_ratio/128.0;
                //	mr=((turn-mr)*max_wheel_linear_speed)*wheel_linear_speed_ratio/128.0;
            }
            else
            {
                zhuan = 0;
                Controlmoto(0, 0, 20000, &mtl, &mtr, &mtb);

                move_test_C.limit_parameter_set_angle(
                    11, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                move_test_C.initializing_movement(ball_angle * 0.6, 0, 0); //(float turn,float front,float angle)
                move_test_C.uptech_output_O(&mb, &ml, &mr);                // ？？？？？？？？？？？？？？
                //	mb=0;ml=0;mr=0;
            }
            // if(xingwei==30)
            //{
            float p = 0.33; // 0.255;//0.24;//35;//0.6;//1.0;
            //			mtb=mb*p; mtl=ml*p; mtr=mr*p;//设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是
            // 1000/1  是2000/2  即发2000 = 2转/秒 		mtb=0; mtl=0; mtr=0;//设置平移速度 每个电机的变量 //严重注意
            // 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000 = 2转/秒
            //}
            ///*
            xingwei1mb = mtb;
            xingwei1ml = mtl;
            xingwei1mr = mtr;
            //			motospeed[0]=mtl;
            //			motospeed[1]=mtr;
            //			motospeed[2]=mtb;
            //			motospeed[3]=9000;
            //			motospeed[4]=-9000;
            // 接球
            //	motospeed[3]=16000;
            //	motospeed[4]=-16000;
            //			zhuan=0;//鑫鑫程序里的转动变量设置成0
            if (m_pAD[0] > 1800 && m_pAD[1] > 1800 && y.ball_chang < 55 && abs(y.ball_jiao) < 20)
            {
                //	m_pCmd->Kick(4000);
                jieqiuys = 158;
            }
            //*/
            Emu_moto_L = mtl;
            Emu_moto_R = mtr;
            Emu_moto_B = mtb;
        }
        else
        {
            mtb = 0;
            mtl = 0;
            mtr = 0;
            xingwei1mb = mtb;
            xingwei1ml = mtl;
            xingwei1mr = mtr;
            Emu_moto_L = mtl;
            Emu_moto_R = mtr;
            Emu_moto_B = mtb;
            // 接球
            motospeed[3] = 0;
            motospeed[4] = 0;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    // xxx
    if (0) // xingwei==30)//接球接球接球接球
    {
        if (y.ball_zhong > 10)
        {
            y.quanbx = move_Emu_ballX;
            y.quanby = move_Emu_ballY;
        }
        Display_A = 0; ////move_Emu_ballX;
        Display_B = 0; ////move_Emu_ballY;

        double jqjiao;
        double jqchang;
        double mubiaox;
        double mubiaoy;

        double julin = 100000000;
        //	if(y.ball_zhong>10 && jieqiuqs==0)
        //	{
        //
        //		jieqiux=y.quanbx;
        //		jieqiuy=y.quanby;
        //		jdx=jieqiux;
        //		jdy=jieqiux;
        //		jieqiuqs=1;
        //	}

        if (y.ball_zhong > 10 && jieqiuqs < 3)
        {
            jieqiuqs++;
        }
        if (y.ball_zhong > 10 && jieqiuqs >= 3)
        {

            m_find.jizuobiao(y.quanbx, y.quanby, &jqjiao, &jqchang, jieqiux, jieqiuy);
            for (i = 0; i < 1200; i++)
            {
                m_find.zhizuobiao(&mubiaox, &mubiaoy, jqjiao, i, jieqiux, jieqiuy);
                if ((mubiaox - y.benjix) * (mubiaox - y.benjix) + (mubiaoy - y.benjiy) * (mubiaoy - y.benjiy) < julin)
                {
                    jdx = mubiaox;
                    jdy = mubiaoy;
                    julin = (mubiaox - y.benjix) * (mubiaox - y.benjix) + (mubiaoy - y.benjiy) * (mubiaoy - y.benjiy);
                }
            }
        }
        if (jieqiuqs >= 3 &&
            ((jieqiux - y.quanbx) * (jieqiux - y.quanbx) + (jieqiuy - y.quanby) * (jieqiuy - y.quanby) > 2500))
        { //	jieqiux=1800;//170;//t_fxx*100.0;
            //	jieqiuy=0;//1130;//t_fyy*100.0;

            float mb;
            float ml;
            float mr;
            double t_angle1;
            double t_radius1;
            t_angle1 = -f2l_angle(y.benjix, y.benjiy, -y.benjijiao, jdx, jdy);
            t_radius1 = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, jdx, jdy);
            //		move_test_C  .initializing_movement(0,30,0);//(float turn,float front,float angle)
            //		move_test_C  .uptech_output_O(&mb,&ml,&mr);//？？？？？？？？？？？？？？
            if (y.benjiy > jdy)
                Debug_D[0].Draw_vp_(t_radius1, 240, 500, 1, 0, 255, 0, pBuffer);
            else
                Debug_D[0].Draw_vp_(-t_radius1, 240, 500, 1, 0, 255, 0, pBuffer);

            double jieqiusudu = abs(t_radius1) * 225;

            if (jieqiusudu > 8000)
                jieqiusudu = 8000;

            //		if(jieqiusudu>jieqiusudu_s+100+abs(t_radius1)*2)
            //			jieqiusudu=jieqiusudu_s+100+abs(t_radius1)*2;
            if (jieqiusudu > jieqiusudu_s + 200)
                jieqiusudu = jieqiusudu_s + 200;
            if (jieqiusudu < jieqiusudu_s - 1500)
                jieqiusudu = jieqiusudu_s - 1500;

            if (jieqiusudu > 0 && jieqiusudu < 400)
                jieqiusudu = 400;
            if (jieqiusudu < 0 && jieqiusudu > -400)
                jieqiusudu = -400;

            float t_angle2 = -f2l_angle(y.benjix, y.benjiy, -y.benjijiao, jieqiux, jieqiuy);
            zhuan = 2 * duizhun(t_angle2, 0, 500);

            //	if(y.ball_chang<110)
            //		Controlmoto(90,y.ball_jiao*120,20000,&mtl,&mtr,&mtb);

            //	Controlmoto(90,(m_pAD[0]-m_pAD[1])*2,20000,&mtl,&mtr,&mtb);
            if (y.ball_chang < 130)
            {
                jieqiujf += (jieqiuc_s - y.ball_chang - 2) * 100;
                if (jieqiujf < -2000)
                    jieqiujf = -2000;
                Controlmoto(180, jieqiujf, 20000, &mtl, &mtr, &mtb);
            }
            if (fabs(t_radius1) < 300)
            {
                Controlmoto(t_angle1, jieqiusudu, 20000, &mtl, &mtr, &mtb);
            }
            else
            {
                Controlmoto(0, 0, 20000, &mtl, &mtr, &mtb);
            }
            double hcjiao;
            double hcchang;
            //		vhecheng(90,(m_pAD[0]-m_pAD[1])*2,20000,t_angle1,jieqiusudu,20000,&hcjiao,&hcchang);
            //		Controlmoto(hcjiao,hcchang,20000,&mtl,&mtr,&mtb);
            //	float	p=0.33;//0.255;//0.24;//35;//0.6;//1.0;
            //	mtb=mb*p; mtl=ml*p; mtr=mr*p;//设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1
            // 是2000/2  即发2000 = 2转/秒
            motospeed[3] = 20000;
            motospeed[4] = -20000;
            jieqiusudu_s = jieqiusudu;
        }
        else
        {
            zhuan = 0;
            Controlmoto(0, 0, 20000, &mtl, &mtr, &mtb);
        }
        m_find.pField->SetAgent(5, jdx, jdy, 0); //(5,900,600,0);//
        jieqiuc_s = y.ball_chang;
        if (y.ball_chang < 65)
        {                             //	m_pCmd->Kick(4000);
            jieqiuys = jieqiuys + 20; // 158;
        }
        //	zhuan=0;
    }
    if (xingwei != 30)
    {
        jieqiuqs = 0;
        jieqiusudu_s = 0;
        jieqiuc_s;
        jieqiujf = 0;
    }

    static float xw30_i_result;
    static float xw30_d_result;
    static float xw30_datainP;
    static float xw30_dataoutP;
    static float mlp, mbp, mrp;

    if (xingwei != 30) //
    {
        jieqiuqs = 0;

        xw30_i_result = 0;
        xw30_d_result = 0;
        xw30_datainP = 0;
        xw30_dataoutP = 0;
        mlp = 0;
        mbp = 0;
        mrp = 0;

        v_radius_set = 0;
        rb_dis_p = 0;
    }
    if (0) //(xingwei==1)//2013行为30电机响应测试
    {
        xingwei_30_motor_test_timer += 0.2;
        float motorSpeed = 8000 * cos(xingwei_30_motor_test_timer);
        mtr = motorSpeed;
        mtb = motorSpeed;
        mtl = motorSpeed;
        Debug_D[2].Draw_vp_(mtr, 50, 10000, 1, 255, 0, 0, pBuffer);
        Debug_D[3].Draw_vp_(mtb, 100, 10000, 1, 0, 255, 0, pBuffer);
        Debug_D[4].Draw_vp_(mtl, 150, 10000, 1, 0, 0, 255, pBuffer);
        Debug_D[2].Draw_vp_(mtr, 50, 10000, 1, 255, 0, 0, pBuffer);
        Debug_D[3].Draw_vp_(mtb, 100, 10000, 1, 0, 255, 0, pBuffer);
        Debug_D[4].Draw_vp_(mtl, 150, 10000, 1, 0, 0, 255, pBuffer);

        // Dribble_ball.O_movement_T_analyze(&front_a,&front_v,&turn_w,vl/read_time_l,vb/read_time_b,vr/read_time_r);//-mhLR[1]->Pos_[0],-mhBR[1]->Pos_[2],-mhRR[1]->Pos_[1]);//[0]是上个周期的结果
        // *move_v_angle, *move_v_speed, *turn_v Dribble_ball.RMfv_m =front_v; Dribble_ball.RMfa_m =front_a;
        // Dribble_ball.RMw_m  =turn_w;
        // Dribble_ball.RMfv=front_v*0.0000072953417187572275815686989911401*1000/Dribble_ball.Gb_loop_time;//
        // Dribble_ball.RMfa=front_a;
        // Dribble_ball.RMw
        // =turn_w*0.0000072953417187572275815686989911401*1000/Dribble_ball.Gb_loop_time*360/(2*3.14159*0.2234);//
        // 米/s->度/s n*360/(2*pi*r)

        Debug_D[5].Draw_vp_(-vr / read_time_r, 50, 10000, 1, 128, 0, 0, pBuffer);
        Debug_D[6].Draw_vp_(-vb / read_time_b, 100, 10000, 1, 0, 128, 0, pBuffer);
        Debug_D[7].Draw_vp_(-vl / read_time_l, 150, 10000, 1, 0, 0, 128, pBuffer);
        Debug_D[5].Draw_vp_(-vr / read_time_r, 50, 10000, 1, 128, 0, 0, pBuffer);
        Debug_D[6].Draw_vp_(-vb / read_time_b, 100, 10000, 1, 0, 128, 0, pBuffer);
        Debug_D[7].Draw_vp_(-vl / read_time_l, 150, 10000, 1, 0, 0, 128, pBuffer);

        zhuan = 0;
        xingwei = 110;
    }
    if (xingwei == 30) // 2013//0)//xingwei==1)//20120620接球cs+xxx
    {
        // 使用码盘定位 辅助代码 move_Emu_robot是过度信息 可以用其 切换码盘信息或者视觉信息
        float temp_benjix = y.benjix;       //
        float temp_benjiy = y.benjiy;       //
        float temp_benjijiao = y.benjijiao; //

        y.benjix = move_Emu_robot_f_x;    //
        y.benjiy = move_Emu_robot_f_y;    //
        y.benjijiao = move_Emu_robot_f_a; //

        // 接球
        move_Emu_ballSX = jieqiux; // 900;//
        move_Emu_ballSY = jieqiuy; // 600;//

        ballC_sx = move_Emu_ballSX * 0.01;
        ballC_sy = move_Emu_ballSY * 0.01;

        obj_properties robotA_d;
        obj_properties ballC_d;
        robotA_d.f_x = y.benjix * 0.01;      // move_Emu_robot_f_x*0.01;//
        robotA_d.f_y = y.benjiy * 0.01;      // move_Emu_robot_f_y*0.01;//
        robotA_d.f_a = y.benjijiao;          //-move_Emu_robot_f_a;//
        ballC_d.f_x = move_Emu_ballX * 0.01; // y.quanbx*0.01;
        ballC_d.f_y = move_Emu_ballY * 0.01; // y.quanby*0.01;

        //	m_find.pField->SetAgent(5,robotTx*100 ,robotTy*100,0);//场子正中向上  逆时针坐标系
        float mb = 0;
        float ml = 0;
        float mr = 0;

        // 注意 需要用拟合等方式 先确认速度方向(10桢间取差直线拟合)基本是向本机的再开始此程序 否则接了也接不着
        // 使用f2l_angle (ballC_sx,ballC_sy,0, ballC_d .f_x,ballC_d .f_y);
        //	和f2l_angle (ballC_sx,ballC_sy,0, robotA_d.f_x,robotA_d.f_y);
        //   对比看机器和球的飞行角差多少要差90度(或者60度)以上就追球就行了
        float robot_Sball_radius = f2l_radius(ballC_sx, ballC_sy, 0, robotA_d.f_x, robotA_d.f_y);
        float ball__Sball_radius = f2l_radius(ballC_sx, ballC_sy, 0, ballC_d.f_x, ballC_d.f_y);
        float robot__ball_radius = f2l_radius(ballC_d.f_x, ballC_d.f_y, 0, robotA_d.f_x, robotA_d.f_y);

        float tempa = f2l_angle(move_Emu_ballSX, move_Emu_ballSY, 0, move_Emu_ballX, move_Emu_ballY);
        float tempangle = f2l_angle(ballC_d.f_x, ballC_d.f_y, tempa, robotA_d.f_x,
                                    robotA_d.f_y); // 估算后 球对地面的位置 在地面坐标系里
        float tempradius = robot__ball_radius * cos(tempangle * 3.14 / 180.0);
        float tempradius_2 = tempradius;
        Display_A = ball__Sball_radius; //

        //	if(y.ball_zhong>10 && jieqiuqs<5 )
        //	{
        //		jieqiuqs++;
        //	}
        if (y.ball_zhong > 10 && jieqiuqs >= 5)
        {
            float t_fxx;
            float t_fyy;
            // 使用当前 球位置生成机体跑位位置 误差取决于当前球位置 可以参考机器看到最近几桢球的
            // 位置和移动距离看和拟和直线的误差判断球是否在直线运动
            t_fxx = l2f_x(ballC_d.f_x, ballC_d.f_y, 0, tempa, tempradius); // 估算后 球对地面的位置 在地面坐标系里
            t_fyy = l2f_y(ballC_d.f_x, ballC_d.f_y, 0, tempa, tempradius);
            tempangle = f2l_angle(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx,
                                  t_fyy); // 估算后 球对地面的位置 在地面坐标系里
            tempradius = f2l_radius(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx,
                                    t_fyy); // 估算后 球对地面的位置 在地面坐标系里
            //			  t_fxx		 = l2f_x (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a,tempangle,tempradius*v_p);//估算后
            // 球对地面的位置 在地面坐标系里 			  t_fyy		 = l2f_y
            //(robotA_d.f_x,robotA_d.f_y,robotA_d.f_a,tempangle,tempradius*v_p);
            t_fxx = l2f_x(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, tempangle,
                          tempradius); // 估算后 球对地面的位置 在地面坐标系里
            t_fyy = l2f_y(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, tempangle, tempradius);

            jdx = t_fxx * 100.0;
            jdy = t_fyy * 100.0;

            //		jdx=600-750;//test  t_fxx*100.0;
            //		jdy=300		;//test t_fyy*100.0;
        }
        // 依据发球点到本机的距离 生成球距离发球点多少距离开始接球
        float start_catch_ball_move_dis =
            robot_Sball_radius * 0.3; // 0.16;//开始接球的距离为 球移动 出发球点到本机距离的一半时再动
        if (start_catch_ball_move_dis < 1.0)
            start_catch_ball_move_dis = 1.0;
        if (start_catch_ball_move_dis > 1.8)
            start_catch_ball_move_dis = 1.8;
        // 也可以试验判定 只有t_radius1 =  f2l_radius(y.benjix,y.benjiy,-y.benjijiao, jdx,jdy); 趋于稳定再开始接球
        //	if (jieqiuqs>=3 && ball__Sball_radius>0.8)
        if (y.ball_zhong > 10 + 18 && ball__Sball_radius > start_catch_ball_move_dis)
        {
            jieqiuys++;
        }
        if (jieqiuqs >= 5 && ball__Sball_radius > start_catch_ball_move_dis) // 2014test+
        {
            //////////////////////////////////////////////////////判定开始接球动作
            if (1)
            {

                float mb;
                float ml;
                float mr;
                double t_angle1;
                double t_radius1;
                t_angle1 = -f2l_angle(y.benjix, y.benjiy, -y.benjijiao, jdx, jdy);
                t_radius1 = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, jdx, jdy);
                //		move_test_C  .initializing_movement(0,30,0);//(float turn,float front,float angle)
                //		move_test_C  .uptech_output_O(&mb,&ml,&mr);//？？？？？？？？？？？？？？
                float datain = 0;
                float dataout = 0;
                float angle_out = 0;
                if (1) // 得到机体与目标移动位置间的修整误差
                {
                    float jd_xy_Sball_angle = f2l_angle(move_Emu_ballSX, move_Emu_ballSY, 0, jdx, jdy);
                    float angle_aaa = f2l_angle(jdx, jdy, jd_xy_Sball_angle, y.benjix, y.benjiy);
                    float angle_bbb = angle_add_transfer(jd_xy_Sball_angle, 90); //
                    angle_out = angle_add_transfer(angle_bbb, y.benjijiao);
                    //	Draw_line_		(int x ,int y ,float angle ,float radius,unsigned char r,unsigned char
                    // g,unsigned char b, BYTE *Buf)
                    Debug_D[0].Draw_line_(100, 100, -y.benjijiao, 50, 255, 0, 0, pBuffer);
                    Debug_D[0].Draw_line_(100, 100, -angle_bbb, 50, 0, 0, 255, pBuffer);
                    Debug_D[0].Draw_line_(100, 100, -jd_xy_Sball_angle, 50, 0, 255, 0, pBuffer);
                    Debug_D[0].Draw_line_(100, 100, -angle_out, 50, 255, 255, 255, pBuffer);
                    Debug_D[0].Draw_line_(100, 100, -angle_aaa, 50, 255, 0, 255, pBuffer);

                    if (angle_aaa > 0) //(y.benjiy>jdy)
                        datain = t_radius1;
                    else
                        datain = -t_radius1;

                    Debug_D[0].Draw_vp_(datain, 240, 500, 1, 0, 255, 0, pBuffer);
                }
                if (1) // PID跟踪部分
                {
                    //	Dribble_ball.RMfv//
                    //	Dribble_ball.RMfa;			angle_out= angle_add_transfer(angle_bbb,y.benjijiao);
                    //	Dribble_ball.RMw;
                    if (1)
                    {
                        //		if (fabs(angle_add_transfer(Dribble_ball.RMfa,-angle_out))>90)
                        //			datain=datain-Dribble_ball.RMfv*13.1;
                        //		else
                        //			datain=datain+Dribble_ball.RMfv*13.1;
                        datain = datain + Dribble_ball.RMfv *
                                              cos(angle_add_transfer(Dribble_ball.RMfa, -angle_out) * 3.14 / 180.0) *
                                              13.1;
                        Debug_D[20].Draw_vp_(-Dribble_ball.RMfv *
                                                 cos(angle_add_transfer(Dribble_ball.RMfa, -angle_out) * 3.14 / 180.0) *
                                                 13.1,
                                             240, 100, 1, 0, 255, 255, pBuffer);
                        float Pp = 3.0; // 2.8;//4.8;//3.9;    //4.1;//3.0;//3.9->3m 3.0->10   2.5;
                        //		float Ip=0.2;//0.1;
                        //		if (((datain>0)&&(xw30_i_result>0))||((datain<0)&&(xw30_i_result<0)))
                        //			  Ip=0.01;
                        float Dp = 0; //-0.8;//0;//0.6;//0.8;//0.8;//0.2;//1;

                        if (datain > 0)
                            datain = sqrt(datain);
                        else
                            datain = -sqrt(-datain);

                        float xw30_p_result = datain;
                        //			  xw30_i_result= xw30_i_result +datain  *Ip;
                        //	  xw30_d_result+=((datain   -   xw30_datainP)*Dp);
                        xw30_d_result = ((datain - xw30_datainP) * Dp);

                        xw30_datainP = datain;
                        //			dataout=(xw30_p_result+xw30_i_result+xw30_d_result)*Pp;//   p_result+i_result+
                        dataout = (xw30_p_result + xw30_d_result) * Pp; //   p_result+i_result+
                        //		dataout=(xw30_p_result*Pp);//+xw30_d_result);//   p_result+i_result+
                        //			float Amax=0.1
                        //			V=sqrt(2*datain*Amax);
                    }
                    if (0)
                    {
                    }

                    if (1) // 加速 和减速度 限制
                    {
                        float do_max_acc = 300; // 240;
                        do_max_acc =
                            do_max_acc * 128 / (max_wheel_linear_speed * wheel_linear_speed_ratio); // 转换成电机转数
                        float do_max_dacc = 800;                                                    // 600;//450;//
                        do_max_dacc =
                            do_max_dacc * 128 / (max_wheel_linear_speed * wheel_linear_speed_ratio); // 转换成电机转数
                        if (xw30_dataoutP > 0)
                        {
                            if ((dataout - xw30_dataoutP) > do_max_acc)
                                dataout = xw30_dataoutP + do_max_acc;
                            if ((dataout - xw30_dataoutP) < -do_max_dacc)
                                dataout = xw30_dataoutP - do_max_dacc;
                        }
                        else
                        {
                            if ((dataout - xw30_dataoutP) > do_max_dacc)
                                dataout = xw30_dataoutP + do_max_dacc;
                            if ((dataout - xw30_dataoutP) < -do_max_acc)
                                dataout = xw30_dataoutP - do_max_acc;
                        }
                    }
                    xw30_dataoutP = dataout;
                }

                Debug_D[1].Draw_vp_(dataout, 240, 500, 1, 255, 255, 0, pBuffer);

                if (0) // 让机器向后退 减小机器和球的相对速度
                {
                }
                if (0) // 让机器向后退 减小机器和球的相对速度
                {
                    float rb_dis = -tempradius_2; // 当前 球C~机器人A x 距离
                    if (rb_dis_p == 0)
                        rb_dis_p = rb_dis;
                    float rb_d_dis = rb_dis - rb_dis_p;
                    float rb_dis_t = -0.02;                              //;rb_dis*0.015//0.0179;
                    float v_radius_set_D = (rb_d_dis + rb_dis_t) * 1600; // 100.12;//计算加速度
                    // 如果平移速度大 前后速度变小 加速度也变小
                    float v_max_acc_p = (500 - fabs(t_radius1)) / 500; //(1.5-robot__ball_radius)/1.5;
                    if (v_max_acc_p < 0)
                        v_max_acc_p = 0;
                    float v_max_acc_p2 = (1.8 - robot__ball_radius) / 1.8; //
                    if (v_max_acc_p2 < 0)
                        v_max_acc_p2 = 0;
                    float v_max_acc = 240 * v_max_acc_p * v_max_acc_p2 * 128 /
                                      (max_wheel_linear_speed * wheel_linear_speed_ratio); // 设置前后分量加速度

                    if (v_radius_set_D > v_max_acc)
                        v_radius_set_D = v_max_acc;
                    if (v_radius_set_D < -v_max_acc * 2)
                        v_radius_set_D = -v_max_acc * 2;
                    v_radius_set += v_radius_set_D;
                    float max_v_set =
                        2500 * 128 / (max_wheel_linear_speed * wheel_linear_speed_ratio); // 设置前后分量最大速度
                    if (v_radius_set > max_v_set)
                        v_radius_set = max_v_set;
                    if (v_radius_set < -max_v_set)
                        v_radius_set = -max_v_set;
                    rb_dis_p = rb_dis;
                    rb_d_dis_h[5] = rb_d_dis_h[4];
                    rb_d_dis_h[4] = rb_d_dis_h[3];
                    rb_d_dis_h[3] = rb_d_dis_h[2];
                    rb_d_dis_h[2] = rb_d_dis_h[1];
                    rb_d_dis_h[1] = rb_d_dis_h[0];
                    rb_d_dis_h[0] = rb_d_dis;
                    Debug_D[8].Draw_vp_(v_radius_set, 300, 100, 1, 255, 255, 0, pBuffer);
                }
                float t_angle2 = angle_add_transfer2(
                    tempa, robotA_d.f_a); // f2l_angle (robotA_d.f_x,robotA_d.f_y,robotA_d.f_a, ballC_sx,ballC_sy);
                float t_radius2 = v_radius_set;

                if (0) // 鑫鑫参考代码
                {
                }

                if (0) // 不使用球相对机体速度的控制 接球不后退
                {
                    t_angle2 = 0;
                    t_radius2 = 0;
                }

                if (0)
                { // 2014test
                    float t_angle = move_test_C.vector_add_angle(angle_out, -dataout, t_angle2, t_radius2);
                    float t_radius = move_test_C.vector_add_length(angle_out, -dataout, t_angle2, t_radius2);
                    //		float	t_angle	  = angle_out;
                    //		float	t_radius  = -dataout;
                    //		float	t_angle	  = t_angle2;
                    //		float	t_radius  = t_radius2;
                    move_test_C.limit_parameter_set_angle(
                        100, 100); //(21, 1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                    move_test_C.limit_parameter_set_dis(
                        100,
                        100); //(60, 12);//(15, 5);//(128,328);//float velocity_limit_dis_,float acceleration_limit_dis_
                    move_test_C.initializing_movement(
                        (f2l_angle(y.benjix, y.benjiy, -y.benjijiao, jieqiux, jieqiuy) + 0) * 0.2, t_radius,
                        t_angle); //(float turn,float front,float angle)
                                  //	move_test_C  .initializing_movement(f2l_angle (y.benjix ,y.benjiy
                    //,-y.benjijiao,jieqiux,jieqiuy)*0.2,-dataout,angle_out);//(float turn,float front,float angle)
                    //	move_test_C  .turn_v_maxlimit();//
                    //	move_test_C  .front_v_maxlimit();//
                    //	move_test_C  .front_a_maxlimit();//
                    move_test_C.uptech_output_O(&mb, &ml, &mr); // ？？？？？？？？？？？？？？
                    mtb = mb;
                    mtl = ml;
                    mtr = mr;
                    zhuan = 0;
                    motospeed[3] = 20000;
                    motospeed[4] = -20000;
                    //	if(m_pAD[0]>1800 && m_pAD[1]>1800 && y.ball_chang<100)
                    //	{					m_pCmd->Kick(4000);
                    //	}

                    //	mb=mtb; ml=mtl; mr=mtr;//测试xxx

                    Debug_D[2].Draw_vp_(mtr, 50, 30000, 1, 255, 0, 0, pBuffer);
                    Debug_D[3].Draw_vp_(mtb, 100, 30000, 1, 0, 255, 0, pBuffer);
                    Debug_D[4].Draw_vp_(mtl, 150, 30000, 1, 0, 0, 255, pBuffer);
                    float max_acc = 250;
                    float max_dacc = 1000;
                    // 电机输出查看加速度 如果监控线一直是单线 就说明没超出单轴加速度限制 出现双线说明超了
                    if (mlp > 0)
                    {
                        if ((ml - mlp) > max_acc)
                            ml = mlp + max_acc;
                        if ((ml - mlp) < -max_dacc)
                            ml = mlp - max_dacc;
                    }
                    else
                    {
                        if ((ml - mlp) > max_dacc)
                            ml = mlp + max_dacc;
                        if ((ml - mlp) < -max_acc)
                            ml = mlp - max_acc;
                    }

                    if (mbp > 0)
                    {
                        if ((mb - mbp) > max_acc)
                            mb = mbp + max_acc;
                        if ((mb - mbp) < -max_dacc)
                            mb = mbp - max_dacc;
                    }
                    else
                    {
                        if ((mb - mbp) > max_dacc)
                            mb = mbp + max_dacc;
                        if ((mb - mbp) < -max_acc)
                            mb = mbp - max_acc;
                    }

                    if (mrp > 0)
                    {
                        if ((mr - mrp) > max_acc)
                            mr = mrp + max_acc;
                        if ((mr - mrp) < -max_dacc)
                            mr = mrp - max_dacc;
                    }
                    else
                    {
                        if ((mr - mrp) > max_dacc)
                            mr = mrp + max_dacc;
                        if ((mr - mrp) < -max_acc)
                            mr = mrp - max_acc;
                    }
                    mlp = ml;
                    mbp = mb;
                    mrp = mr;
                    if (1)
                    {
                        Debug_D[5].Draw_vp_(mr, 50, 30000, 1, 128, 0, 0, pBuffer);
                        Debug_D[6].Draw_vp_(mb, 100, 30000, 1, 0, 128, 0, pBuffer);
                        Debug_D[7].Draw_vp_(ml, 150, 30000, 1, 0, 0, 128, pBuffer);
                    }

                } // 2014test

                if (0) // 2014test++
                {

                    //	Dribble_ball.O_movement_T_analyze(&front_a,&front_v,&turn_w,vl/read_time_l,vb/read_time_b,vr/read_time_r);//-mhLR[1]->Pos_[0],-mhBR[1]->Pos_[2],-mhRR[1]->Pos_[1]);//[0]是上个周期的结果
                    //*move_v_angle, *move_v_speed, *turn_v
                    float front_a, front_v, turn_w;
                    Dribble_ball.O_movement_T_analyze(&front_a, &front_v, &turn_w, motol_as, motob_as,
                                                      motor_as); //    *move_v_angle, *move_v_speed, *turn_v
                    move_test_C.front_v_p = front_v * 0.005126;
                    move_test_C.front_v_angle_p = angle_add_transfer(front_a, -180);
                    //	Dribble_ball.RMfv=front_v*0.0000072953417187572275815686989911401*1000/Dribble_ball.Gb_loop_time;//
                    //	Dribble_ball.RMfa=front_a;
                    //	Dribble_ball.RMw
                    //=turn_w*0.0000072953417187572275815686989911401*1000/Dribble_ball.Gb_loop_time*360/(2*3.14159*0.2234);//
                    // 米/s->度/s n*360/(2*pi*r)

                    y.benjix = temp_benjix;       //
                    y.benjiy = temp_benjiy;       //
                    y.benjijiao = temp_benjijiao; //
                    double robot_Sball_radius = f2l_radius(move_Emu_ballSX, move_Emu_ballSY, 0, y.benjix, y.benjiy);
                    double ball__Sball_radius = f2l_radius(y.quanbx, y.quanby, 0, move_Emu_ballSX, move_Emu_ballSY);
                    double robot__ball_radius = f2l_radius(y.quanbx, y.quanby, 0, y.benjix, y.benjiy);

                    double tempa = f2l_angle(move_Emu_ballSX, move_Emu_ballSY, 0, y.quanbx, y.quanby);
                    double tempangle = f2l_angle(y.quanbx, y.quanby, tempa, y.benjix,
                                                 y.benjiy); // 估算后 球对地面的位置 在地面坐标系里
                    double tempradius = robot__ball_radius * cos(tempangle * 3.14 / 180.0);
                    double tempradius_2 = tempradius;

                    xingwei30ballXh[xingwei30ballLh] = y.quanbx; // 接球用 开始接球后球历史记录 2014
                    xingwei30ballYh[xingwei30ballLh] = y.quanby;
                    xingwei30ballLh++; // 接球用数组记录到的位置
                    if (xingwei30ballLh >= 900)
                        xingwei30ballLh = 900 - 1;
                    float tempa_ball_a = 0;
                    float tempa_ball_a_ = 0;
                    float tempa_ball_a__ = 0;
                    float tempa_ball_a__0 = 0;
                    float tempa_ball_aadd2 = 0;
                    float tempa_ball_aadd = 0;
                    float tempa_ball_v = 0;

                    /*			move_Emu_ballSX=600;move_Emu_ballSY=0;//
                                xingwei30ballXh[xingwei30ballLh- 1-0]=400;xingwei30ballYh[xingwei30ballLh- 1-0]=-1;
                                xingwei30ballXh[xingwei30ballLh- 1-1]=400;xingwei30ballYh[xingwei30ballLh- 1-1]=-3;
                                xingwei30ballXh[xingwei30ballLh- 1-2]=400;xingwei30ballYh[xingwei30ballLh- 1-2]=-2;
                                xingwei30ballXh[xingwei30ballLh- 1-3]=400;xingwei30ballYh[xingwei30ballLh- 1-3]=-4;
                                xingwei30ballXh[xingwei30ballLh- 1-4]=400;xingwei30ballYh[xingwei30ballLh- 1-4]=4;
                                xingwei30ballXh[xingwei30ballLh- 1-5]=400;xingwei30ballYh[xingwei30ballLh- 1-5]=3;
                                xingwei30ballXh[xingwei30ballLh- 1-6]=400;xingwei30ballYh[xingwei30ballLh- 1-6]=2;
                                xingwei30ballXh[xingwei30ballLh- 1-7]=400;xingwei30ballYh[xingwei30ballLh- 1-7]=1;
                    */
                    // 球行进方向
                    /* 			if(xingwei30ballLh>=30+8-1)
                                {
                                    tempa_ball_a__=f2l_angle (move_Emu_ballSX,move_Emu_ballSY,0,
                                                              xingwei30ballXh[xingwei30ballLh-
                    1-0],xingwei30ballYh[xingwei30ballLh- 1-0]); tempa_ball_a__0=tempa_ball_a__;
                                    tempa_ball_a=tempa_ball_a__0;
                                    for(i=0;i<8;i++)//(i=0;i<8;i++)
                                    {
                                        tempa_ball_a_= f2l_angle
                    (xingwei30ballXh[xingwei30ballLh-30-i],xingwei30ballYh[xingwei30ballLh-30-i],0,
                                                                  xingwei30ballXh[xingwei30ballLh-
                    1-i],xingwei30ballYh[xingwei30ballLh- 1-i]);
                    //					tempa_ball_a_= f2l_angle (move_Emu_ballSX,move_Emu_ballSY,0,
                    //											  xingwei30ballXh[xingwei30ballLh-
                    1-i],xingwei30ballYh[xingwei30ballLh- 1-i]); tempa_ball_v+=
                    f2l_radius(xingwei30ballXh[xingwei30ballLh-30-i],xingwei30ballYh[xingwei30ballLh-30-i],0,
                                                                  xingwei30ballXh[xingwei30ballLh-
                    1-i],xingwei30ballYh[xingwei30ballLh- 1-i]);
                                    //	tempa_ball_a+=tempa_ball_a_;abcde
                                        tempa_ball_aadd+=tempa_ball_a_;
                                        tempa_ball_a   +=angle_add_transfer(tempa_ball_a_,-tempa_ball_a__);
                                        tempa_ball_aadd2+=tempa_ball_a;
                                        tempa_ball_a__  =tempa_ball_a_;
                                    }


                                    tempa_ball_a=tempa_ball_aadd2/8;
                                    tempa_ball_v=tempa_ball_v/8;
                                    tempa=tempa_ball_a;
                                }

                                if(xingwei30ballLh>=60+8-1)
                                {
                                    tempa_ball_a__=f2l_angle (move_Emu_ballSX,move_Emu_ballSY,0,
                                                              xingwei30ballXh[xingwei30ballLh-
                    1-0],xingwei30ballYh[xingwei30ballLh- 1-0]); tempa_ball_a__0=tempa_ball_a__;
                                    tempa_ball_a=tempa_ball_a__0;
                                    for(i=0;i<8;i++)//(i=0;i<8;i++)
                                    {
                                        tempa_ball_a_= f2l_angle
                    (xingwei30ballXh[xingwei30ballLh-60-i],xingwei30ballYh[xingwei30ballLh-60-i],0,
                                                                  xingwei30ballXh[xingwei30ballLh-
                    1-i],xingwei30ballYh[xingwei30ballLh- 1-i]);
                    //					tempa_ball_a_= f2l_angle (move_Emu_ballSX,move_Emu_ballSY,0,
                    //											  xingwei30ballXh[xingwei30ballLh-
                    1-i],xingwei30ballYh[xingwei30ballLh- 1-i]); tempa_ball_v+=
                    f2l_radius(xingwei30ballXh[xingwei30ballLh-60-i],xingwei30ballYh[xingwei30ballLh-60-i],0,
                                                                  xingwei30ballXh[xingwei30ballLh-
                    1-i],xingwei30ballYh[xingwei30ballLh- 1-i]);
                                    //	tempa_ball_a+=tempa_ball_a_;abcde
                                        tempa_ball_aadd+=tempa_ball_a_;
                                        tempa_ball_a   +=angle_add_transfer(tempa_ball_a_,-tempa_ball_a__);
                                        tempa_ball_aadd2+=tempa_ball_a;
                                        tempa_ball_a__  =tempa_ball_a_;
                                    }


                                    tempa_ball_a=tempa_ball_aadd2/8;
                                    tempa_ball_v=tempa_ball_v/8;
                                    tempa=tempa_ball_a;
                                }
                    */
                    if (xingwei30ballLh >= 60 + 8 - 1)
                    {
                        tempa_ball_a__ =
                            f2l_angle(move_Emu_ballSX, move_Emu_ballSY, 0, xingwei30ballXh[xingwei30ballLh - 1 - 0],
                                      xingwei30ballYh[xingwei30ballLh - 1 - 0]);
                        tempa_ball_a__0 = tempa_ball_a__;
                        tempa_ball_a = tempa_ball_a__0;
                        for (i = 0; i < 1; i++) //(i=0;i<8;i++)
                        {
                            tempa_ball_a_ = f2l_angle(
                                xingwei30ballXh[xingwei30ballLh - 60 - i], xingwei30ballYh[xingwei30ballLh - 60 - i], 0,
                                xingwei30ballXh[xingwei30ballLh - 1 - i], xingwei30ballYh[xingwei30ballLh - 1 - i]);
                            //					tempa_ball_a_= f2l_angle (move_Emu_ballSX,move_Emu_ballSY,0,
                            //											  xingwei30ballXh[xingwei30ballLh-
                            // 1-i],xingwei30ballYh[xingwei30ballLh- 1-i]);
                            tempa_ball_v += f2l_radius(
                                xingwei30ballXh[xingwei30ballLh - 60 - i], xingwei30ballYh[xingwei30ballLh - 60 - i], 0,
                                xingwei30ballXh[xingwei30ballLh - 1 - i], xingwei30ballYh[xingwei30ballLh - 1 - i]);
                            //	tempa_ball_a+=tempa_ball_a_;abcde
                            tempa_ball_aadd += tempa_ball_a_;
                            tempa_ball_a += angle_add_transfer(tempa_ball_a_, -tempa_ball_a__);
                            tempa_ball_aadd2 += tempa_ball_a;
                            tempa_ball_a__ = tempa_ball_a_;
                        }

                        tempa_ball_a = tempa_ball_aadd2;
                        tempa_ball_v = tempa_ball_v;
                        tempa = tempa_ball_a;
                    }

                    double t_fxx;
                    double t_fyy;
                    // 使用当前 球位置生成机体跑位位置 误差取决于当前球位置 可以参考机器看到最近几桢球的
                    // 位置和移动距离看和拟和直线的误差判断球是否在直线运动
                    t_fxx = l2f_x(y.quanbx, y.quanby, 0, tempa, tempradius); // 估算后 球对地面的位置 在地面坐标系里
                    t_fyy = l2f_y(y.quanbx, y.quanby, 0, tempa, tempradius);

                    //	if(y.ball_zhong>10	)
                    //	{
                    jdx = t_fxx;
                    jdy = t_fyy;
                    //	}

                    float jd_xy_Sball_angle = f2l_angle(move_Emu_ballSX, move_Emu_ballSY, 0, jdx, jdy);
                    float angle_aaa = f2l_angle(jdx, jdy, jd_xy_Sball_angle, y.benjix, y.benjiy);
                    float angle_bbb = angle_add_transfer(jd_xy_Sball_angle, 90); //
                    angle_out = angle_add_transfer(angle_bbb, y.benjijiao);

                    double tfx = jdx; // y.quanbx-200;
                    double tfy = jdy; // y.quanby;
                    double tfa = -y.ball_jiao;
                    //			double tfa=f2l_angle (y.benjix ,y.benjiy
                    //,-y.benjijiao,move_Emu_ballSX,move_Emu_ballSY)*0.4;
                    double la = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, tfx, tfy);
                    double lr = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, tfx, tfy);
                    char black_white_ball[10];

                    float ballrealR =
                        move_Emu_ballR * cdbl /
                        2.7; //	ballchang_temp=ballchang_temp/cdbl*2.7;//;	float move_Emu_ballR=ballchang_temp/100.0

                    float p_acc = (3.9 - ballrealR) / 3.9;
                    if (p_acc > 1)
                        p_acc = 1;
                    if (p_acc < 0)
                        p_acc = 0;

                    float plr_acc = (lr * cdbl / 2.7) / 200.0;
                    if (plr_acc > 1)
                        plr_acc = 1;
                    if (plr_acc < 0)
                        plr_acc = 0;
                    //		/*front_tv*/Debug_D[31].Draw_vp_(lr,0,30000,1,  0,0,128, pBuffer);//p_acc lr
                    //		/*front_v */Debug_D[30].Draw_vp_(plr_acc,0,30000,1,  0,0,128, pBuffer);//front_v

                    //			float front_p  =7.5+1.8*p_acc;//3.6*p_acc;//6;//0.37;//
                    //			float front_p  =10.5+0.8*p_acc;//3.6*p_acc;//6;//0.37;//
                    //		float front_p  =11.5+0.0*p_acc;//3.6*p_acc;//6;//0.37;//
                    //	float front_p  =7.5+1.8*p_acc+4.0*plr_acc;//3.6*p_acc;//6;//0.37;//
                    //	float front_p  =8.5+0.8*p_acc+6.0*plr_acc;//3.6*p_acc;//6;//0.37;//
                    //		float front_p  =7.5+0.8*p_acc+6.0*plr_acc;//3.6*p_acc;//6;//0.37;//
                    //		float front_p  =6.0+1.8*p_acc+6.0*plr_acc;//3.6*p_acc;//6;//0.37;//
                    float front_p = 10; // 6.5;//+0.8*p_acc+6.0*plr_acc;//3.6*p_acc;//6;//0.37;//
                    float front_acc = front_p * front_p * 0.019; // 0.027;//3;//66;//
                    move_test_C.limit_parameter_set_angle(
                        10.0, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                    move_test_C.limit_parameter_set_dis(
                        80,
                        front_acc); //(zs , 10);//(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                    move_test_C.initializing_movement(0, 0, 0); //
                    move_test_C.zs_angle_track_001(tfa, 0 /*benchmark_angle*/, black_white_ball);
                    //	float path_long = lr*front_p*cdbl/2.7*1.421;//0.69;//0.5;
                    float path_long = (lr * cdbl / 2.7 * 1.421 - front_v * 0.0025) * front_p; // 0.69;//0.5;
                    //	float path_long = (lr-5)*front_p*cdbl/2.7*1.421;//0.69;//0.5;
                    //	if (path_long<0)
                    //		path_long=0;
                    //	Debug_D[31].Draw_vp_("lr",lr,0,30000,1,  0,0,128, pBuffer);
                    //	Debug_D[30].Draw_vp_("la",la,0,30000,1,  0,0,128, pBuffer);
                    //	Debug_D[35].Draw_vp_("front_v",front_v,0,30000,1,  0,0,128, pBuffer);
                    //	Debug_D[34].Draw_vp_("front_a",front_a,0,30000,1,  0,0,128, pBuffer);

                    move_test_C.turn_v_c = tfa * 0.66; // 0.33;
                    if (path_long > 0)
                        move_test_C.front_v_c = sqrt(path_long);
                    else
                        move_test_C.front_v_c = -sqrt(-path_long);
                    move_test_C.front_v_angle_c = la;
                    ///*front_tv*/Debug_D[31].Draw_vp_(move_test_C.front_v_c,0,30000,1,  0,0,128, pBuffer);//p_acc lr
                    move_test_C.front_v_maxlimit();
                    //		Debug_D[31].Draw_vp_("front_v_c",move_test_C.front_v_c,0,30000,1,  0,0,128,
                    // pBuffer);//("100",100,0,30000,1,  0,0,128, pBuffer);
                    //		Debug_D[30].Draw_vp_("front_v_a_c",move_test_C.front_v_angle_c,0,30000,1,  0,0,128,
                    // pBuffer); 		Debug_D[35].Draw_vp_("front_v_p",move_test_C.front_v_p,0,30000,1,  0,0,128,
                    // pBuffer); 		Debug_D[34].Draw_vp_("front_v_a_c",move_test_C.front_v_angle_p,0,30000,1,
                    // 0,0,128, pBuffer);

                    move_test_C.front_a_maxlimit();
                    ///*front_v */Debug_D[30].Draw_vp_(move_test_C.front_v_c,0,30000,1,  0,0,128, pBuffer);//front_v
                    move_test_C.uptech_output_O(&mb, &ml, &mr);

                    //			float front_ta,front_tv,turn_tw;
                    //			float amax=150;
                    //			move_test_C.O_movement_T_analyze(&front_ta,&front_tv,&turn_tw,ml,mb,mr);//
                    //*move_v_angle, *move_v_speed, *turn_v

                    //			float vp=front_tv*0.005126;
                    //			float va=angle_add_transfer(front_ta,-180);
                    //	Debug_D[31].Draw_vp_("front_v_c",move_test_C.front_v_c,0,30000,1,  0,0,128, pBuffer);
                    //	Debug_D[30].Draw_vp_("front_v_a",move_test_C.front_v_angle_c,0,30000,1,  0,0,128, pBuffer);
                    //	Debug_D[35].Draw_vp_("vp",vp,0,30000,1,  0,0,128, pBuffer);
                    //	Debug_D[34].Draw_vp_("va",va,0,30000,1,  0,0,128, pBuffer);
                    //	Debug_D[35].Draw_vp_("front_tv",front_tv,0,30000,1,  0,0,128, pBuffer);
                    //	Debug_D[34].Draw_vp_("front_ta",front_ta,0,30000,1,  0,0,128, pBuffer);
                    /*
                                if (front_tv>front_v+amax)
                                    front_tv=front_v+amax;
                                if (front_tv<front_v-amax)
                                    front_tv=front_v-amax;
                                Dribble_ball.move2Control.turn_v_c=tfa*0.33;
                                Dribble_ball.move2Control.front_v_c=front_tv;
                                Dribble_ball.move2Control.front_v_angle_c=la;
                                Dribble_ball.move2Control.movement_output_O(&motor1,&motor2,&motor3);
                                motor1=-motor1;
                                motor2=-motor2;
                                motor3=-motor3;
                                *mtb=m2_linear_speed;	*mtl=m1_linear_speed;	*mtr=m3_linear_speed;
                    */

                    mtb = mb;
                    mtl = ml;
                    mtr = mr;
                    //		mtb=0; mtl=0; mtr=0;
                    if (0) //(m_pAD[0]>2000 || m_pAD[1]>2000)
                    {
                        motospeed[3] = 1000;
                        motospeed[4] = -1000;
                    }
                    else
                    {
                        motospeed[3] = 6001;
                        motospeed[4] = -6001;
                    }

                    //...
                    Debug_D[28].Draw_vp_(jdx, 0, 30000, 1, 0, 0, 128, pBuffer);
                    Debug_D[29].Draw_vp_(jdy, 0, 30000, 1, 0, 0, 128, pBuffer);
                }

                if (1) // 2014test++^
                {

                    //	Dribble_ball.O_movement_T_analyze(&front_a,&front_v,&turn_w,vl/read_time_l,vb/read_time_b,vr/read_time_r);//-mhLR[1]->Pos_[0],-mhBR[1]->Pos_[2],-mhRR[1]->Pos_[1]);//[0]是上个周期的结果
                    //*move_v_angle, *move_v_speed, *turn_v
                    float front_a, front_v, turn_w;
                    Dribble_ball.O_movement_T_analyze(&front_a, &front_v, &turn_w, motol_as, motob_as,
                                                      motor_as); //    *move_v_angle, *move_v_speed, *turn_v
                    move_test_C.front_v_p = front_v * 0.005126;
                    move_test_C.front_v_angle_p = angle_add_transfer(front_a, -180);
                    //	Dribble_ball.RMfv=front_v*0.0000072953417187572275815686989911401*1000/Dribble_ball.Gb_loop_time;//
                    //	Dribble_ball.RMfa=front_a;
                    //	Dribble_ball.RMw
                    //=turn_w*0.0000072953417187572275815686989911401*1000/Dribble_ball.Gb_loop_time*360/(2*3.14159*0.2234);//
                    // 米/s->度/s n*360/(2*pi*r)

                    y.benjix = temp_benjix;       //
                    y.benjiy = temp_benjiy;       //
                    y.benjijiao = temp_benjijiao; //
                    double robot_Sball_radius = f2l_radius(move_Emu_ballSX, move_Emu_ballSY, 0, y.benjix, y.benjiy);
                    double ball__Sball_radius = f2l_radius(y.quanbx, y.quanby, 0, move_Emu_ballSX, move_Emu_ballSY);
                    double robot__ball_radius = f2l_radius(y.quanbx, y.quanby, 0, y.benjix, y.benjiy);

                    double tempa = f2l_angle(move_Emu_ballSX, move_Emu_ballSY, 0, y.quanbx, y.quanby);
                    double tempangle = f2l_angle(y.quanbx, y.quanby, tempa, y.benjix,
                                                 y.benjiy); // 估算后 球对地面的位置 在地面坐标系里
                    double tempradius = robot__ball_radius * cos(tempangle * 3.14 / 180.0);
                    double tempradius_2 = tempradius;

                    xingwei30ballXh[xingwei30ballLh] = y.quanbx; // 接球用 开始接球后球历史记录 2014
                    xingwei30ballYh[xingwei30ballLh] = y.quanby;
                    xingwei30ballLh++; // 接球用数组记录到的位置
                    if (xingwei30ballLh >= 900)
                        xingwei30ballLh = 900 - 1;
                    float tempa_ball_a = 0;
                    float tempa_ball_a_ = 0;
                    float tempa_ball_a__ = 0;
                    float tempa_ball_a__0 = 0;
                    float tempa_ball_aadd2 = 0;
                    float tempa_ball_aadd = 0;
                    float tempa_ball_v = 0;

                    /*			move_Emu_ballSX=600;move_Emu_ballSY=0;//
                                xingwei30ballXh[xingwei30ballLh- 1-0]=400;xingwei30ballYh[xingwei30ballLh- 1-0]=-1;
                                xingwei30ballXh[xingwei30ballLh- 1-1]=400;xingwei30ballYh[xingwei30ballLh- 1-1]=-3;
                                xingwei30ballXh[xingwei30ballLh- 1-2]=400;xingwei30ballYh[xingwei30ballLh- 1-2]=-2;
                                xingwei30ballXh[xingwei30ballLh- 1-3]=400;xingwei30ballYh[xingwei30ballLh- 1-3]=-4;
                                xingwei30ballXh[xingwei30ballLh- 1-4]=400;xingwei30ballYh[xingwei30ballLh- 1-4]=4;
                                xingwei30ballXh[xingwei30ballLh- 1-5]=400;xingwei30ballYh[xingwei30ballLh- 1-5]=3;
                                xingwei30ballXh[xingwei30ballLh- 1-6]=400;xingwei30ballYh[xingwei30ballLh- 1-6]=2;
                                xingwei30ballXh[xingwei30ballLh- 1-7]=400;xingwei30ballYh[xingwei30ballLh- 1-7]=1;
                    */
                    // 球行进方向
                    /* 			if(xingwei30ballLh>=30+8-1)
                                {
                                    tempa_ball_a__=f2l_angle (move_Emu_ballSX,move_Emu_ballSY,0,
                                                              xingwei30ballXh[xingwei30ballLh-
                    1-0],xingwei30ballYh[xingwei30ballLh- 1-0]); tempa_ball_a__0=tempa_ball_a__;
                                    tempa_ball_a=tempa_ball_a__0;
                                    for(i=0;i<8;i++)//(i=0;i<8;i++)
                                    {
                                        tempa_ball_a_= f2l_angle
                    (xingwei30ballXh[xingwei30ballLh-30-i],xingwei30ballYh[xingwei30ballLh-30-i],0,
                                                                  xingwei30ballXh[xingwei30ballLh-
                    1-i],xingwei30ballYh[xingwei30ballLh- 1-i]);
                    //					tempa_ball_a_= f2l_angle (move_Emu_ballSX,move_Emu_ballSY,0,
                    //											  xingwei30ballXh[xingwei30ballLh-
                    1-i],xingwei30ballYh[xingwei30ballLh- 1-i]); tempa_ball_v+=
                    f2l_radius(xingwei30ballXh[xingwei30ballLh-30-i],xingwei30ballYh[xingwei30ballLh-30-i],0,
                                                                  xingwei30ballXh[xingwei30ballLh-
                    1-i],xingwei30ballYh[xingwei30ballLh- 1-i]);
                                    //	tempa_ball_a+=tempa_ball_a_;abcde
                                        tempa_ball_aadd+=tempa_ball_a_;
                                        tempa_ball_a   +=angle_add_transfer(tempa_ball_a_,-tempa_ball_a__);
                                        tempa_ball_aadd2+=tempa_ball_a;
                                        tempa_ball_a__  =tempa_ball_a_;
                                    }


                                    tempa_ball_a=tempa_ball_aadd2/8;
                                    tempa_ball_v=tempa_ball_v/8;
                                    tempa=tempa_ball_a;
                                }

                                if(xingwei30ballLh>=60+8-1)
                                {
                                    tempa_ball_a__=f2l_angle (move_Emu_ballSX,move_Emu_ballSY,0,
                                                              xingwei30ballXh[xingwei30ballLh-
                    1-0],xingwei30ballYh[xingwei30ballLh- 1-0]); tempa_ball_a__0=tempa_ball_a__;
                                    tempa_ball_a=tempa_ball_a__0;
                                    for(i=0;i<8;i++)//(i=0;i<8;i++)
                                    {
                                        tempa_ball_a_= f2l_angle
                    (xingwei30ballXh[xingwei30ballLh-60-i],xingwei30ballYh[xingwei30ballLh-60-i],0,
                                                                  xingwei30ballXh[xingwei30ballLh-
                    1-i],xingwei30ballYh[xingwei30ballLh- 1-i]);
                    //					tempa_ball_a_= f2l_angle (move_Emu_ballSX,move_Emu_ballSY,0,
                    //											  xingwei30ballXh[xingwei30ballLh-
                    1-i],xingwei30ballYh[xingwei30ballLh- 1-i]); tempa_ball_v+=
                    f2l_radius(xingwei30ballXh[xingwei30ballLh-60-i],xingwei30ballYh[xingwei30ballLh-60-i],0,
                                                                  xingwei30ballXh[xingwei30ballLh-
                    1-i],xingwei30ballYh[xingwei30ballLh- 1-i]);
                                    //	tempa_ball_a+=tempa_ball_a_;abcde
                                        tempa_ball_aadd+=tempa_ball_a_;
                                        tempa_ball_a   +=angle_add_transfer(tempa_ball_a_,-tempa_ball_a__);
                                        tempa_ball_aadd2+=tempa_ball_a;
                                        tempa_ball_a__  =tempa_ball_a_;
                                    }


                                    tempa_ball_a=tempa_ball_aadd2/8;
                                    tempa_ball_v=tempa_ball_v/8;
                                    tempa=tempa_ball_a;
                                }
                    */
                    if (xingwei30ballLh >= 60 + 8 - 1)
                    {
                        tempa_ball_a__ =
                            f2l_angle(move_Emu_ballSX, move_Emu_ballSY, 0, xingwei30ballXh[xingwei30ballLh - 1 - 0],
                                      xingwei30ballYh[xingwei30ballLh - 1 - 0]);
                        tempa_ball_a__0 = tempa_ball_a__;
                        tempa_ball_a = tempa_ball_a__0;
                        for (i = 0; i < 1; i++) //(i=0;i<8;i++)
                        {
                            tempa_ball_a_ = f2l_angle(
                                xingwei30ballXh[xingwei30ballLh - 60 - i], xingwei30ballYh[xingwei30ballLh - 60 - i], 0,
                                xingwei30ballXh[xingwei30ballLh - 1 - i], xingwei30ballYh[xingwei30ballLh - 1 - i]);
                            //					tempa_ball_a_= f2l_angle (move_Emu_ballSX,move_Emu_ballSY,0,
                            //											  xingwei30ballXh[xingwei30ballLh-
                            // 1-i],xingwei30ballYh[xingwei30ballLh- 1-i]);
                            tempa_ball_v += f2l_radius(
                                xingwei30ballXh[xingwei30ballLh - 60 - i], xingwei30ballYh[xingwei30ballLh - 60 - i], 0,
                                xingwei30ballXh[xingwei30ballLh - 1 - i], xingwei30ballYh[xingwei30ballLh - 1 - i]);
                            //	tempa_ball_a+=tempa_ball_a_;abcde
                            tempa_ball_aadd += tempa_ball_a_;
                            tempa_ball_a += angle_add_transfer(tempa_ball_a_, -tempa_ball_a__);
                            tempa_ball_aadd2 += tempa_ball_a;
                            tempa_ball_a__ = tempa_ball_a_;
                        }

                        tempa_ball_a = tempa_ball_aadd2;
                        tempa_ball_v = tempa_ball_v;
                        tempa = tempa_ball_a;
                    }

                    double t_fxx;
                    double t_fyy;
                    // 使用当前 球位置生成机体跑位位置 误差取决于当前球位置 可以参考机器看到最近几桢球的
                    // 位置和移动距离看和拟和直线的误差判断球是否在直线运动
                    t_fxx = l2f_x(y.quanbx, y.quanby, 0, tempa, tempradius); // 估算后 球对地面的位置 在地面坐标系里
                    t_fyy = l2f_y(y.quanbx, y.quanby, 0, tempa, tempradius);

                    //	if(y.ball_zhong>10	)
                    //	{
                    jdx = t_fxx;
                    jdy = t_fyy;
                    //	}

                    double t_angle1;
                    double t_radius1;
                    t_angle1 = -f2l_angle(y.benjix, y.benjiy, -y.benjijiao, jdx, jdy);
                    t_radius1 = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, jdx, jdy);

                    float jd_xy_Sball_angle = f2l_angle(move_Emu_ballSX, move_Emu_ballSY, 0, jdx, jdy);
                    float angle_aaa = f2l_angle(jdx, jdy, jd_xy_Sball_angle, y.benjix, y.benjiy);
                    float angle_bbb = angle_add_transfer(jd_xy_Sball_angle, 90); //
                    angle_out = angle_add_transfer(angle_bbb, y.benjijiao);

                    if (angle_aaa > 0) //(y.benjiy>jdy)
                        datain = t_radius1;
                    else
                        datain = -t_radius1;

                    double tfx = jdx; // y.quanbx-200;
                    double tfy = jdy; // y.quanby;
                    double tfa = -y.ball_jiao;
                    //			double tfa=f2l_angle (y.benjix ,y.benjiy
                    //,-y.benjijiao,move_Emu_ballSX,move_Emu_ballSY)*0.4;
                    double la = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, tfx, tfy);
                    double lr = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, tfx, tfy);
                    char black_white_ball[10];

                    float ballrealR =
                        move_Emu_ballR * cdbl /
                        2.7; //	ballchang_temp=ballchang_temp/cdbl*2.7;//;	float move_Emu_ballR=ballchang_temp/100.0

                    float p_acc = (3.9 - ballrealR) / 3.9;
                    if (p_acc > 1)
                        p_acc = 1;
                    if (p_acc < 0)
                        p_acc = 0;

                    float plr_acc = (lr * cdbl / 2.7) / 200.0;
                    if (plr_acc > 1)
                        plr_acc = 1;
                    if (plr_acc < 0)
                        plr_acc = 0;
                    //		/*front_tv*/Debug_D[31].Draw_vp_(lr,0,30000,1,  0,0,128, pBuffer);//p_acc lr
                    //		/*front_v */Debug_D[30].Draw_vp_(plr_acc,0,30000,1,  0,0,128, pBuffer);//front_v

                    //			float front_p  =7.5+1.8*p_acc;//3.6*p_acc;//6;//0.37;//
                    //			float front_p  =10.5+0.8*p_acc;//3.6*p_acc;//6;//0.37;//
                    //		float front_p  =11.5+0.0*p_acc;//3.6*p_acc;//6;//0.37;//
                    //	float front_p  =7.5+1.8*p_acc+4.0*plr_acc;//3.6*p_acc;//6;//0.37;//
                    //	float front_p  =8.5+0.8*p_acc+6.0*plr_acc;//3.6*p_acc;//6;//0.37;//
                    //		float front_p  =7.5+0.8*p_acc+6.0*plr_acc;//3.6*p_acc;//6;//0.37;//
                    //		float front_p  =6.0+1.8*p_acc+6.0*plr_acc;//3.6*p_acc;//6;//0.37;//
                    float front_p = 10; // 6.5;//+0.8*p_acc+6.0*plr_acc;//3.6*p_acc;//6;//0.37;//
                    float front_acc = front_p * front_p * 0.019; // 0.027;//3;//66;//
                    move_test_C.limit_parameter_set_angle(
                        40.0, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                    move_test_C.limit_parameter_set_dis(
                        80,
                        front_acc); //(zs , 10);//(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                    move_test_C.initializing_movement(0, 0, 0); //
                    move_test_C.zs_angle_track_001(tfa, 0 /*benchmark_angle*/, black_white_ball);
                    //	float path_long = lr*front_p*cdbl/2.7*1.421;//0.69;//0.5;
                    //		float path_long = (lr*cdbl/2.7*1.421-front_v*0.0025)*front_p;//0.69;//0.5;
                    //		float path_long =
                    //(datain*cdbl/2.7*1.421-front_v*cos(angle_add_transfer(front_a,-angle_out)*3.14/180.0)*0.0025)*front_p;//0.69;//0.5;
                    float path_long = (datain * cdbl / 2.7 * 1.421 -
                                       front_v * cos(angle_add_transfer(front_a, -angle_out) * 3.14 / 180.0) * 0.0025) *
                                      front_p; // 0.69;//0.5;
                    //	Debug_D[37].Draw_vp_("datain", datain*cdbl/2.7*1.421 ,0,30000,1,  0,0,128, pBuffer);
                    //	Debug_D[38].Draw_vp_("fv", front_v*cos(angle_add_transfer(front_a,-angle_out)*3.14/180.0)*0.0025
                    //,0,30000,1,  0,0,128, pBuffer);

                    //	float path_long = (lr-5)*front_p*cdbl/2.7*1.421;//0.69;//0.5;
                    //	if (path_long<0)
                    //		path_long=0;
                    //	Debug_D[31].Draw_vp_("lr",lr,0,30000,1,  0,0,128, pBuffer);
                    //	Debug_D[30].Draw_vp_("la",la,0,30000,1,  0,0,128, pBuffer);
                    //	Debug_D[35].Draw_vp_("front_v",front_v,0,30000,1,  0,0,128, pBuffer);
                    //	Debug_D[34].Draw_vp_("front_a",front_a,0,30000,1,  0,0,128, pBuffer);

                    move_test_C.turn_v_c = tfa * 0.6; // 0.69;//0.33;
                    //			if (path_long>0)
                    //				move_test_C.front_v_c=sqrt(path_long);
                    //			else
                    //				move_test_C.front_v_c=-sqrt(-path_long);
                    //			move_test_C.front_v_angle_c=la;

                    //			if (path_long>0)
                    //				path_long=sqrt(path_long);
                    //			else
                    //				path_long=-sqrt(-path_long);
                    float w = 18;
                    float path_long2 = 0;
                    if (datain > w)
                        path_long2 =
                            sqrt(((datain - w) * cdbl / 2.7 * 1.421 -
                                  front_v * cos(angle_add_transfer(front_a, -angle_out) * 3.14 / 180.0) * 0.0029) *
                                 front_p);
                    else if (datain < -w)
                        path_long2 =
                            -sqrt(-(((datain + w) * cdbl / 2.7 * 1.421 -
                                     front_v * cos(angle_add_transfer(front_a, -angle_out) * 3.14 / 180.0) * 0.0029) *
                                    front_p));
                    else
                        path_long2 = 0;
                    if (fabs(path_long2) > fabs(path_long * 0.061))
                        move_test_C.front_v_c = -path_long2;
                    else
                        move_test_C.front_v_c = -path_long * 0.061;

                    move_test_C.front_v_angle_c = angle_out;
                    ///*front_tv*/Debug_D[31].Draw_vp_(move_test_C.front_v_c,0,30000,1,  0,0,128, pBuffer);//p_acc lr
                    move_test_C.front_v_maxlimit();
                    //		Debug_D[31].Draw_vp_("front_v_c",move_test_C.front_v_c,0,30000,1,  0,0,128,
                    // pBuffer);//("100",100,0,30000,1,  0,0,128, pBuffer);
                    //		Debug_D[30].Draw_vp_("front_v_a_c",move_test_C.front_v_angle_c,0,30000,1,  0,0,128,
                    // pBuffer); 		Debug_D[35].Draw_vp_("front_v_p",move_test_C.front_v_p,0,30000,1,  0,0,128,
                    // pBuffer); 		Debug_D[34].Draw_vp_("front_v_a_c",move_test_C.front_v_angle_p,0,30000,1,
                    // 0,0,128, pBuffer);

                    move_test_C.front_a_maxlimit();
                    ///*front_v */Debug_D[30].Draw_vp_(move_test_C.front_v_c,0,30000,1,  0,0,128, pBuffer);//front_v
                    move_test_C.uptech_output_O(&mb, &ml, &mr);

                    //			float front_ta,front_tv,turn_tw;
                    //			float amax=150;
                    //			move_test_C.O_movement_T_analyze(&front_ta,&front_tv,&turn_tw,ml,mb,mr);//
                    //*move_v_angle, *move_v_speed, *turn_v

                    //			float vp=front_tv*0.005126;
                    //			float va=angle_add_transfer(front_ta,-180);
                    //	Debug_D[31].Draw_vp_("front_v_c",move_test_C.front_v_c,0,30000,1,  0,0,128, pBuffer);
                    //	Debug_D[30].Draw_vp_("front_v_a",move_test_C.front_v_angle_c,0,30000,1,  0,0,128, pBuffer);
                    //	Debug_D[35].Draw_vp_("vp",vp,0,30000,1,  0,0,128, pBuffer);
                    //	Debug_D[34].Draw_vp_("va",va,0,30000,1,  0,0,128, pBuffer);
                    //	Debug_D[35].Draw_vp_("front_tv",front_tv,0,30000,1,  0,0,128, pBuffer);
                    //	Debug_D[34].Draw_vp_("front_ta",front_ta,0,30000,1,  0,0,128, pBuffer);
                    /*
                                if (front_tv>front_v+amax)
                                    front_tv=front_v+amax;
                                if (front_tv<front_v-amax)
                                    front_tv=front_v-amax;
                                Dribble_ball.move2Control.turn_v_c=tfa*0.33;
                                Dribble_ball.move2Control.front_v_c=front_tv;
                                Dribble_ball.move2Control.front_v_angle_c=la;
                                Dribble_ball.move2Control.movement_output_O(&motor1,&motor2,&motor3);
                                motor1=-motor1;
                                motor2=-motor2;
                                motor3=-motor3;
                                *mtb=m2_linear_speed;	*mtl=m1_linear_speed;	*mtr=m3_linear_speed;
                    */

                    mtb = mb;
                    mtl = ml;
                    mtr = mr;
                    //		mtb=0; mtl=0; mtr=0;
                    if (0) //(m_pAD[0]>2000 || m_pAD[1]>2000)
                    {
                        motospeed[3] = 1000;
                        motospeed[4] = -1000;
                    }
                    else
                    {
                        motospeed[3] = 6001;
                        motospeed[4] = -6001;
                    }

                    //...
                    Debug_D[28].Draw_vp_(jdx, 0, 30000, 1, 0, 0, 128, pBuffer);
                    Debug_D[29].Draw_vp_(jdy, 0, 30000, 1, 0, 0, 128, pBuffer);
                }

                //??????
                if (0) //(0)// 1 使用 视觉 和码盘 做接球基准   0使用陀螺 码盘 做接球基准
                {
                    if (0) //(xingwei!=30)//(xingwei==11)
                    {
                        robot_catch_ball.robot_emu.f_x = y.benjix * 0.01; // 6;//
                        robot_catch_ball.robot_emu.f_y = y.benjiy * 0.01; // 3;//
                        robot_catch_ball.robot_emu.f_a = -y.benjijiao;    // ypr[0]*180.0/3.14;//
                        //	Dribble_ball.robot_L_test.f_a=0;
                    }
                    if (0) //(xingwei==11)//(xingwei==11)
                    {
                        robot_catch_ball.robot_emu.f_x = y.benjix * 0.01; // 6;//
                        robot_catch_ball.robot_emu.f_y = y.benjiy * 0.01; // 3;//
                        robot_catch_ball.robot_emu.f_a = -y.benjijiao;    // ypr[0]*180.0/3.14;//
                        //	Dribble_ball.robot_L_test.f_a=0;
                    }
                    if (1)
                    {
                        robot_catch_ball.robot_emu.f_a = -y.benjijiao; // ypr[0]*180.0/3.14;//
                    }
                    //	Emu_Robot		robot_catch_ball;//用来做接球定位用 仿真机体计算
                    //	obj_properties  ball_for_catch_ball;//用来做接球定位用 球定位计算
                    float Gb_loop_time = LOOPTIME;
                    float T = Gb_loop_time * 0.001; // 0.067;//周期时间单位秒
                    // 注：矫正方法 使用单独 角度对准程序矫正wp 单独平移对准程序矫正vp
                    // 矫正结果就是球在场地地图上的位置不动
                    const double vp = 0.475; // 0.35;//2.2 ;//静态2.2 ;//平移矫正系数 常数
                    const double wp = 1;     // 1.2 ;//静态2.0 ;//旋转矫正系数 常数
                    // 对球的对地运动矢量计算001
                    // 运动反解
                    float front_v = 0; // 1.99012;//临时存储 反解出的 平移速度 转动速度 平移方向
                    float turn_w = 0;  // 61.9297;
                    float front_a = 0; //-29.3969;
                    Dribble_ball.O_movement_T_analyze(&front_a, &front_v, &turn_w, -motol_as, -motob_as, -motor_as);
                    front_v = front_v * 0.0000072953417187572275815686989911401 * vp * 1000 / Gb_loop_time;
                    turn_w = turn_w * 0.0000072953417187572275815686989911401 * wp * 1000 / Gb_loop_time * 360 /
                             (2 * 3.14159 * 0.23);
                    // 电机速度真值
                    // float max_a=540;
                    robot_catch_ball.Movement(T, front_v, front_a, turn_w);
                    move_Emu_robot_f_x = robot_catch_ball.robot_emu.f_x * 100; // y.benjix;
                    move_Emu_robot_f_y = robot_catch_ball.robot_emu.f_y * 100; // y.benjiy;
                    move_Emu_robot_f_a = y.benjijiao;                          //-ypr[0]*180.0/3.14;//

                    m_find.pField->SetAgent(666, move_Emu_robot_f_x, move_Emu_robot_f_y, move_Emu_robot_f_a);

                    //			float move_Emu_ballA=ball_angle;//f2l_angle (y.benjix ,y.benjiy
                    //,-y.benjijiao,y.quanbx,y.quanby); 			double
                    // balljiao_temp=angle_add_transfer2(-y.ball_jiao-y.benjijiao+90,0); 	double
                    // ballchang_temp=quxian[int(y_ball_chang_min)]/cdbl*2.7;//quxian[int(changdi->ball_chang)];
                    //			double	ballchang_temp=quxian[int(y_ball_chang_min)];//quxian[int(changdi->ball_chang)];
                    //			double
                    // ballchang_tempballchang_temp=(11.25*(ballchang_temp+sqrt(ballchang_temp*ballchang_temp+73*73))/73);
                    //			ballchang_temp=ballchang_temp-ballchang_tempballchang_temp;
                    //			float move_Emu_ballR=ballchang_temp/100.0;//ball_radius;//f2l_radius(y.benjix ,y.benjiy
                    //,-y.benjijiao,y.quanbx,y.quanby);

                    float move_Emu_ballA =
                        ball_angle; // f2l_angle (y.benjix ,y.benjiy  ,-y.benjijiao,y.quanbx,y.quanby);
                    //	double	balljiao_temp=angle_add_transfer2(-y.ball_jiao-y.benjijiao+90,0);
                    //	double
                    // ballchang_temp=quxian[int(y_ball_chang_min)]/cdbl*2.7;//quxian[int(changdi->ball_chang)];
                    double ballchang_temp = quxian[int(y_ball_chang_min)]; // quxian[int(changdi->ball_chang)];
                    double ballchang_tempballchang_temp =
                        (11.25 * (ballchang_temp + sqrt(ballchang_temp * ballchang_temp + 73 * 73)) / 73);
                    ballchang_temp = ballchang_temp - ballchang_tempballchang_temp;

                    if (set_use_vision_location) // cs2014g
                    {
                        ballchang_temp = ballchang_temp / cdbl * 2.7; //;
                    }

                    double temp_y_quanbx = l2f_x(move_Emu_robot_f_x, move_Emu_robot_f_y, -move_Emu_robot_f_a,
                                                 move_Emu_ballA, ballchang_temp);
                    double temp_y_quanby = l2f_y(move_Emu_robot_f_x, move_Emu_robot_f_y, -move_Emu_robot_f_a,
                                                 move_Emu_ballA, ballchang_temp);
                    if (1)
                    {
                        y.quanbx = temp_y_quanbx;
                        y.quanby = temp_y_quanby;
                        temp_benjix = move_Emu_robot_f_x;    // robot_catch_ball.robot_emu.f_x*100;
                        temp_benjiy = move_Emu_robot_f_y;    // robot_catch_ball.robot_emu.f_y*100;
                        temp_benjijiao = move_Emu_robot_f_a; // robot_catch_ball.robot_emu.f_a;
                    }
                    //	m_find.zhizuobiao(&temp_y_quanbx,&temp_y_quanby,balljiao_temp,ballchang_temp,y.benjix,y.benjiy);

                    if (1) // 2014test++^^
                    {

                        //	Dribble_ball.O_movement_T_analyze(&front_a,&front_v,&turn_w,vl/read_time_l,vb/read_time_b,vr/read_time_r);//-mhLR[1]->Pos_[0],-mhBR[1]->Pos_[2],-mhRR[1]->Pos_[1]);//[0]是上个周期的结果
                        //*move_v_angle, *move_v_speed, *turn_v
                        float front_a, front_v, turn_w;
                        Dribble_ball.O_movement_T_analyze(&front_a, &front_v, &turn_w, motol_as, motob_as,
                                                          motor_as); //    *move_v_angle, *move_v_speed, *turn_v
                        move_test_C.front_v_p = front_v * 0.005126;
                        move_test_C.front_v_angle_p = angle_add_transfer(front_a, -180);
                        //	Dribble_ball.RMfv=front_v*0.0000072953417187572275815686989911401*1000/Dribble_ball.Gb_loop_time;//
                        //	Dribble_ball.RMfa=front_a;
                        //	Dribble_ball.RMw
                        //=turn_w*0.0000072953417187572275815686989911401*1000/Dribble_ball.Gb_loop_time*360/(2*3.14159*0.2234);//
                        // 米/s->度/s n*360/(2*pi*r)

                        y.benjix = temp_benjix;       //
                        y.benjiy = temp_benjiy;       //
                        y.benjijiao = temp_benjijiao; //
                        double robot_Sball_radius = f2l_radius(move_Emu_ballSX, move_Emu_ballSY, 0, y.benjix, y.benjiy);
                        double ball__Sball_radius = f2l_radius(y.quanbx, y.quanby, 0, move_Emu_ballSX, move_Emu_ballSY);
                        double robot__ball_radius = f2l_radius(y.quanbx, y.quanby, 0, y.benjix, y.benjiy);

                        double tempa = f2l_angle(move_Emu_ballSX, move_Emu_ballSY, 0, y.quanbx, y.quanby);
                        double tempangle = f2l_angle(y.quanbx, y.quanby, tempa, y.benjix,
                                                     y.benjiy); // 估算后 球对地面的位置 在地面坐标系里
                        double tempradius = robot__ball_radius * cos(tempangle * 3.14 / 180.0);
                        double tempradius_2 = tempradius;

                        xingwei30ballXh[xingwei30ballLh] = y.quanbx; // 接球用 开始接球后球历史记录 2014
                        xingwei30ballYh[xingwei30ballLh] = y.quanby;
                        xingwei30ballLh++; // 接球用数组记录到的位置
                        if (xingwei30ballLh >= 900)
                            xingwei30ballLh = 900 - 1;
                        float tempa_ball_a = 0;
                        float tempa_ball_a_ = 0;
                        float tempa_ball_a__ = 0;
                        float tempa_ball_a__0 = 0;
                        float tempa_ball_aadd2 = 0;
                        float tempa_ball_aadd = 0;
                        float tempa_ball_v = 0;

                        if (xingwei30ballLh >= 60 + 8 - 1)
                        {
                            tempa_ball_a__ =
                                f2l_angle(move_Emu_ballSX, move_Emu_ballSY, 0, xingwei30ballXh[xingwei30ballLh - 1 - 0],
                                          xingwei30ballYh[xingwei30ballLh - 1 - 0]);
                            tempa_ball_a__0 = tempa_ball_a__;
                            tempa_ball_a = tempa_ball_a__0;
                            for (i = 0; i < 1; i++) //(i=0;i<8;i++)
                            {
                                tempa_ball_a_ = f2l_angle(xingwei30ballXh[xingwei30ballLh - 60 - i],
                                                          xingwei30ballYh[xingwei30ballLh - 60 - i], 0,
                                                          xingwei30ballXh[xingwei30ballLh - 1 - i],
                                                          xingwei30ballYh[xingwei30ballLh - 1 - i]);
                                //					tempa_ball_a_= f2l_angle (move_Emu_ballSX,move_Emu_ballSY,0,
                                //											  xingwei30ballXh[xingwei30ballLh-
                                // 1-i],xingwei30ballYh[xingwei30ballLh- 1-i]);
                                tempa_ball_v += f2l_radius(xingwei30ballXh[xingwei30ballLh - 60 - i],
                                                           xingwei30ballYh[xingwei30ballLh - 60 - i], 0,
                                                           xingwei30ballXh[xingwei30ballLh - 1 - i],
                                                           xingwei30ballYh[xingwei30ballLh - 1 - i]);
                                //	tempa_ball_a+=tempa_ball_a_;abcde
                                tempa_ball_aadd += tempa_ball_a_;
                                tempa_ball_a += angle_add_transfer(tempa_ball_a_, -tempa_ball_a__);
                                tempa_ball_aadd2 += tempa_ball_a;
                                tempa_ball_a__ = tempa_ball_a_;
                            }

                            tempa_ball_a = tempa_ball_aadd2;
                            tempa_ball_v = tempa_ball_v;
                            tempa = tempa_ball_a;
                        }

                        double t_fxx;
                        double t_fyy;
                        // 使用当前 球位置生成机体跑位位置 误差取决于当前球位置 可以参考机器看到最近几桢球的
                        // 位置和移动距离看和拟和直线的误差判断球是否在直线运动
                        t_fxx = l2f_x(y.quanbx, y.quanby, 0, tempa, tempradius); // 估算后 球对地面的位置 在地面坐标系里
                        t_fyy = l2f_y(y.quanbx, y.quanby, 0, tempa, tempradius);

                        //	if(y.ball_zhong>10	)
                        //	{
                        jdx = t_fxx;
                        jdy = t_fyy;
                        //	}

                        double t_angle1;
                        double t_radius1;
                        t_angle1 = -f2l_angle(y.benjix, y.benjiy, -y.benjijiao, jdx, jdy);
                        t_radius1 = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, jdx, jdy);

                        float jd_xy_Sball_angle = f2l_angle(move_Emu_ballSX, move_Emu_ballSY, 0, jdx, jdy);
                        float angle_aaa = f2l_angle(jdx, jdy, jd_xy_Sball_angle, y.benjix, y.benjiy);
                        float angle_bbb = angle_add_transfer(jd_xy_Sball_angle, 90); //
                        angle_out = angle_add_transfer(angle_bbb, y.benjijiao);

                        if (angle_aaa > 0) //(y.benjiy>jdy)
                            datain = t_radius1;
                        else
                            datain = -t_radius1;

                        double tfx = jdx; // y.quanbx-200;
                        double tfy = jdy; // y.quanby;
                        double tfa = -y.ball_jiao;
                        //			double tfa=f2l_angle (y.benjix ,y.benjiy
                        //,-y.benjijiao,move_Emu_ballSX,move_Emu_ballSY)*0.4;
                        double la = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, tfx, tfy);
                        double lr = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, tfx, tfy);
                        char black_white_ball[10];

                        float ballrealR = move_Emu_ballR * cdbl / 2.7; //	ballchang_temp=ballchang_temp/cdbl*2.7;//;
                                                                       // float move_Emu_ballR=ballchang_temp/100.0

                        float p_acc = (3.9 - ballrealR) / 3.9;
                        if (p_acc > 1)
                            p_acc = 1;
                        if (p_acc < 0)
                            p_acc = 0;

                        float plr_acc = (lr * cdbl / 2.7) / 200.0;
                        if (plr_acc > 1)
                            plr_acc = 1;
                        if (plr_acc < 0)
                            plr_acc = 0;
                        //		/*front_tv*/Debug_D[31].Draw_vp_(lr,0,30000,1,  0,0,128, pBuffer);//p_acc lr
                        //		/*front_v */Debug_D[30].Draw_vp_(plr_acc,0,30000,1,  0,0,128, pBuffer);//front_v

                        //			float front_p  =7.5+1.8*p_acc;//3.6*p_acc;//6;//0.37;//
                        //			float front_p  =10.5+0.8*p_acc;//3.6*p_acc;//6;//0.37;//
                        //		float front_p  =11.5+0.0*p_acc;//3.6*p_acc;//6;//0.37;//
                        //	float front_p  =7.5+1.8*p_acc+4.0*plr_acc;//3.6*p_acc;//6;//0.37;//
                        //	float front_p  =8.5+0.8*p_acc+6.0*plr_acc;//3.6*p_acc;//6;//0.37;//
                        //		float front_p  =7.5+0.8*p_acc+6.0*plr_acc;//3.6*p_acc;//6;//0.37;//
                        //		float front_p  =6.0+1.8*p_acc+6.0*plr_acc;//3.6*p_acc;//6;//0.37;//
                        float front_p = 6.0; // 10;//6.5;//+0.8*p_acc+6.0*plr_acc;//3.6*p_acc;//6;//0.37;//
                        float front_acc = front_p * front_p * 0.019; // 0.027;//3;//66;//
                        move_test_C.limit_parameter_set_angle(
                            40.0, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                        move_test_C.limit_parameter_set_dis(80, front_acc); //(zs , 10);//(21,1);//3(128,328);//float
                                                                            // velocity_limit_,float acceleration_limit_
                        move_test_C.initializing_movement(0, 0, 0);         //
                        move_test_C.zs_angle_track_001(tfa, 0 /*benchmark_angle*/, black_white_ball);
                        //	float path_long = lr*front_p*cdbl/2.7*1.421;//0.69;//0.5;
                        //		float path_long = (lr*cdbl/2.7*1.421-front_v*0.0025)*front_p;//0.69;//0.5;
                        //		float path_long =
                        //(datain*cdbl/2.7*1.421-front_v*cos(angle_add_transfer(front_a,-angle_out)*3.14/180.0)*0.0025)*front_p;//0.69;//0.5;
                        float path_long =
                            (datain * cdbl / 2.7 * 1.421 -
                             front_v * cos(angle_add_transfer(front_a, -angle_out) * 3.14 / 180.0) * 0.0025) *
                            front_p; // 0.69;//0.5;
                        //	Debug_D[37].Draw_vp_("datain", datain*cdbl/2.7*1.421 ,0,30000,1,  0,0,128, pBuffer);
                        //	Debug_D[38].Draw_vp_("fv",
                        // front_v*cos(angle_add_transfer(front_a,-angle_out)*3.14/180.0)*0.0025 ,0,30000,1,  0,0,128,
                        // pBuffer);

                        move_test_C.turn_v_c = tfa * 0.6; // 0.69;//0.33;
                        float w = 18;
                        float path_long2 = 0;
                        if (datain > w)
                            path_long2 =
                                sqrt(((datain - w) * cdbl / 2.7 * 1.421 -
                                      front_v * cos(angle_add_transfer(front_a, -angle_out) * 3.14 / 180.0) * 0.0029) *
                                     front_p);
                        else if (datain < -w)
                            path_long2 = -sqrt(
                                -(((datain + w) * cdbl / 2.7 * 1.421 -
                                   front_v * cos(angle_add_transfer(front_a, -angle_out) * 3.14 / 180.0) * 0.0029) *
                                  front_p));
                        else
                            path_long2 = 0;
                        if (fabs(path_long2) > fabs(path_long * 0.061))
                            move_test_C.front_v_c = -path_long2;
                        else
                            move_test_C.front_v_c = -path_long * 0.061;

                        move_test_C.front_v_angle_c = angle_out;
                        ///*front_tv*/Debug_D[31].Draw_vp_(move_test_C.front_v_c,0,30000,1,  0,0,128, pBuffer);//p_acc
                        /// lr
                        move_test_C.front_v_maxlimit();
                        //		Debug_D[31].Draw_vp_("front_v_c",move_test_C.front_v_c,0,30000,1,  0,0,128,
                        // pBuffer);//("100",100,0,30000,1,  0,0,128, pBuffer);
                        //		Debug_D[30].Draw_vp_("front_v_a_c",move_test_C.front_v_angle_c,0,30000,1,  0,0,128,
                        // pBuffer); 		Debug_D[35].Draw_vp_("front_v_p",move_test_C.front_v_p,0,30000,1,  0,0,128,
                        // pBuffer); 		Debug_D[34].Draw_vp_("front_v_a_c",move_test_C.front_v_angle_p,0,30000,1,
                        // 0,0,128, pBuffer);

                        move_test_C.front_a_maxlimit();
                        ///*front_v */Debug_D[30].Draw_vp_(move_test_C.front_v_c,0,30000,1,  0,0,128, pBuffer);//front_v
                        move_test_C.uptech_output_O(&mb, &ml, &mr);

                        mtb = mb;
                        mtl = ml;
                        mtr = mr;
                        //		mtb=0; mtl=0; mtr=0;
                        if (0) //(m_pAD[0]>2000 || m_pAD[1]>2000)
                        {
                            motospeed[3] = 1000;
                            motospeed[4] = -1000;
                        }
                        else
                        {
                            motospeed[3] = 6001;
                            motospeed[4] = -6001;
                        }

                        //...
                        Debug_D[28].Draw_vp_(jdx, 0, 30000, 1, 0, 0, 128, pBuffer);
                        Debug_D[29].Draw_vp_(jdy, 0, 30000, 1, 0, 0, 128, pBuffer);
                    }
                }
            }
            //////////////////////////////////////////////////////判定开始接球动作  [动作生成完成]

            //////////////////////////////////////////////////////
            if ((tempradius_2 < 0) && (y.ball_zhong > 10))
            {                             //		m_pCmd->Kick(1000);
                jieqiuys = jieqiuys + 10; // 158;
            }
            float rb_d_dis_add =
                (rb_d_dis_h[5] + rb_d_dis_h[4] + rb_d_dis_h[3] + rb_d_dis_h[2] + rb_d_dis_h[1] + rb_d_dis_h[0]) * 45.0 /
                6.0;
            if ((rb_d_dis_add < 1.1) && (rb_d_dis_h[5] != 0)) // 0.1m/s   rb_d_dis_add>0.1m/s 机体接球 否则追球
            {                                                 //		m_pCmd->Kick(1000);
                                                              //			jieqiuys=jieqiuys+6;//158;
            }
            float rb_d_dis_h_max = 0;
            for (i = 0; i < 5; i++)
            {
                if (rb_d_dis_h_max < rb_d_dis_h[i])
                    rb_d_dis_h_max = rb_d_dis_h[i];
            }
            if ((rb_d_dis_h_max < 0.10) && (rb_d_dis_h[5] != 0) &&
                (y.ball_zhong > 10)) // 0.1m/s   rb_d_dis_add>0.1m/s 机体接球 否则追球
            {                        //		m_pCmd->Kick(1000);
                                     //			jieqiuys=jieqiuys+6;//158;
            }
            if (robot__ball_radius_min > robot__ball_radius) // 0.1m/s   rb_d_dis_add>0.1m/s 机体接球 否则追球
            {
                robot__ball_radius_min = robot__ball_radius;
            }
            if (((robot__ball_radius_min + 19) < robot__ball_radius) &&
                (y.ball_zhong > 10 + 18))    // 0.1m/s   rb_d_dis_add>0.1m/s 机体接球 否则追球
            {                                //		m_pCmd->Kick(1000);
                jieqiuys = jieqiuys + 6 * 2; // 158;
            }
        }
        else
        {
            //	zhuan=0;
            //	Controlmoto(0,0,20000,&mtl,&mtr,&mtb);

            if (net.rec_ctrl == CTRL_CATCH) // 静态接球使用机体停止平移只对准方向 //cs2014++
            {
                move_test_C.limit_parameter_set_angle(
                    11, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                move_test_C.initializing_movement(
                    (f2l_angle(y.benjix, y.benjiy, -y.benjijiao, move_Emu_ballX, move_Emu_ballY) + 0) * 0.6, 0,
                    0); //(f2l_angle (move_Emu_robot_f_x,move_Emu_robot_f_y,move_Emu_robot_f_a,
                        // move_Emu_ballX,move_Emu_ballY)*0.6,0,0);//(float turn,float front,float angle)
                //		move_test_C.initializing_movement(0,0,0);//(f2l_angle
                //(move_Emu_robot_f_x,move_Emu_robot_f_y,move_Emu_robot_f_a,
                // move_Emu_ballX,move_Emu_ballY)*0.6,0,0);//(float turn,float front,float angle)
                move_test_C.turn_v_maxlimit();
                move_test_C.uptech_output_O(&mb, &ml, &mr); // ？？？？？？？？？？？？？？
                mtb = mb;
                mtl = ml;
                mtr = mr;
                zhuan = 0;
                motospeed[3] = 0;
                motospeed[4] = 0;

                rb_dis_p = 0;
                rb_d_dis_h[5] = rb_d_dis_h[4] = rb_d_dis_h[3] = rb_d_dis_h[2] = rb_d_dis_h[1] = rb_d_dis_h[0] = 0;
            }
            else if (net.rec_ctrl == CTRL_CATCH_FOCUS) // 1行为30 自带跑位 移动接球使用
            {

                //		y.ball_zhong=0;//2014test+
                //	xingwei =7;//
                //	net.rec_targetx=200;//1350;//Catch_robot_f_x;
                //	net.rec_targety=600;//200;//200 ;//200 //600//Catch_robot_f_y;
                //	y.quanbx=900;y.quanby=600;//
                // tfa=angle_add_transfer(-net.rec_targetang,y.benjijiao);
                //	net.rec_targetang=-f2l_angle (y.benjix,y.benjiy,0, y.quanbx,y.quanby);

                float front_a, front_v, turn_w;
                Dribble_ball.O_movement_T_analyze(&front_a, &front_v, &turn_w, motol_as, motob_as,
                                                  motor_as); //    *move_v_angle, *move_v_speed, *turn_v
                move_test_C.front_v_p = front_v * 0.005126;
                move_test_C.front_v_angle_p = angle_add_transfer(front_a, -180);

                jdx = net.rec_targetx; // 200;//y.quanbx-200;
                jdy = net.rec_targety; // 600;//y.quanby;
                double tfx = jdx;      // y.quanbx-200;
                double tfy = jdy;      // y.quanby;
                double tfa = -y.ball_jiao;
                //	double tfa=f2l_angle (y.benjix ,y.benjiy  ,-y.benjijiao,move_Emu_ballSX,move_Emu_ballSY)*0.4;
                double la = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, tfx, tfy);
                double lr = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, tfx, tfy);
                char black_white_ball[10];

                float front_p = 8.5;
                float front_acc = front_p * front_p * 0.019; // 0.027;//3;//66;//
                move_test_C.limit_parameter_set_angle(
                    10.0, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                move_test_C.limit_parameter_set_dis(
                    80, front_acc); //(zs , 10);//(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                move_test_C.initializing_movement(0, 0, 0); //
                move_test_C.zs_angle_track_001(tfa, 0 /*benchmark_angle*/, black_white_ball);
                float path_long = (lr * cdbl / 2.7 * 1.421 - front_v * 0.0025) * front_p; // 0.69;//0.5;

                move_test_C.turn_v_c = tfa * 0.33;
                if (path_long > 0)
                    move_test_C.front_v_c = sqrt(path_long);
                else
                    move_test_C.front_v_c = -sqrt(-path_long);
                move_test_C.front_v_angle_c = la;

                move_test_C.front_v_maxlimit();
                move_test_C.front_a_maxlimit();
                move_test_C.uptech_output_O(&mb, &ml, &mr);
                mtl = ml;
                mtb = mb;
                mtr = mr;
            }
            else // 默认 切进攻  正常情况不会运行至此  //  测试 不对电机值变动直接使用上周期生成值
            {
                xingwei = 1; //	 mtl=motol_as;mtb=motob_as;mtr=motor_as;
            }
        }
        //	motospeed[3]=16000;
        //	motospeed[4]=-16000;
        m_find.pField->SetAgent(5, jdx, jdy, 0); //(5,900,600,0);//
        jieqiuc_s = y.ball_chang;
        if ((m_pAD[0] > 1800 || m_pAD[1] > 1800) && y.ball_chang < 65)
        {                                 //	m_pCmd->Kick(4000);
            jieqiuys = jieqiuys + 20 * 2; // 158;
        }
        //	zhuan=0;//鑫鑫程序里的转动变量设置成0

        y.benjix = temp_benjix;       //
        y.benjiy = temp_benjiy;       //
        y.benjijiao = temp_benjijiao; //
    }
    else
    {
        robot__ball_radius_min = 1000000;
        rb_dis_p = 0;
        rb_d_dis_h[5] = rb_d_dis_h[4] = rb_d_dis_h[3] = rb_d_dis_h[2] = rb_d_dis_h[1] = rb_d_dis_h[0] = 0;

        for (i = 0; i < 900; i++)
        {
            xingwei30ballXh[i] = 0; // 接球用 开始接球后球历史记录 2014
            xingwei30ballYh[i] = 0;
            xingwei30ballLh = 0; // 接球用数组记录到的位置
        }
    }
    if (xingwei != 11)
    {
        float front_a, front_v, turn_w;
        Dribble_ball.O_movement_T_analyze(&front_a, &front_v, &turn_w, motol_as, motob_as,
                                          motor_as); //    *move_v_angle, *move_v_speed, *turn_v
                                                     //	move_test_C.front_v_p		= front_v*0.005126;
                                                     //	move_test_C.front_v_angle_p	=angle_add_transfer(front_a,-180);

        Debug_D[31].Draw_vp_("front_v", front_v, 0, 30000, 1, 0, 0, 128,
                             pBuffer); //("100",100,0,30000,1,  0,0,128, pBuffer);
        Debug_D[30].Draw_vp_("front_a", front_a, 0, 30000, 1, 0, 0, 128, pBuffer);

        //	Debug_D[34].Draw_vp_("jieqiuqs",jieqiuqs,0,30000,1,  0,0,128, pBuffer);
        Debug_D[34].Draw_vp_("jieqiuys", jieqiuys, 0, 30000, 1, 0, 0, 128, pBuffer);
        Debug_D[35].Draw_vp_("ballw", y.ball_zhong, 0, 30000, 1, 0, 0, 128, pBuffer);

        Debug_D[36].Draw_vp_("xingwei", float(xingwei), 0, 30000, 1, 0, 0, 128, pBuffer);
        Debug_D[37].Draw_vp_("rec_ctrl", float(net.rec_ctrl), 0, 30000, 1, 0, 0, 128, pBuffer);

        Debug_D[26].Draw_vp_("quanbx", y.quanbx, 0, 30000, 1, 0, 0, 128, pBuffer);
        Debug_D[27].Draw_vp_("quanby", y.quanby, 0, 30000, 1, 0, 0, 128, pBuffer);
        Debug_D[32].Draw_vp_("benjix", y.benjix, 0, 30000, 1, 0, 0, 128, pBuffer); // move_test_C.front_v_c   front_tv
        Debug_D[33].Draw_vp_("benjiy", y.benjiy, 0, 30000, 1, 0, 0, 128, pBuffer); // move_test_C.front_v_c   front_tv

        Debug_D[25].Debug_Display_Circle_End();
    }
    if (xingwei == 11)
    {
        /*	float mb,ml,mr;	//测试电机返回值—as 回move_test_C使用值
            move_test_C.turn_v_c=100;
            move_test_C.front_v_c=10;
            move_test_C.front_v_angle_c=45;
            move_test_C.uptech_output_O(&mb,&ml,&mr);
            float front_a,front_v,turn_w;
            Dribble_ball.O_movement_T_analyze(&front_a,&front_v,&turn_w,ml,mb,mr);//    *move_v_angle, *move_v_speed,
           *turn_v float v		=					front_v*0.005126;// v=front_v/(-195.083) float a
           =angle_add_transfer(front_a,-180); float t     =turn_w*-0.00512605;// t=turn_w/(-195.082)
        */
        Debug_D[0].Draw_vp_(0, 240, 100, 0, 0, 255, 0, pBuffer); // datain输入测量误差
        Debug_D[20].Draw_vp_(0, 240, 100, 0, 0, 255, 255, pBuffer);
        Debug_D[1].Draw_vp_(0, 240, 100, 0, 255, 255, 0, pBuffer); // dataout输出
        Debug_D[2].Draw_vp_(0, 50, 30000, 0, 255, 0, 0, pBuffer);  // mtr实际输出量
        Debug_D[3].Draw_vp_(0, 100, 30000, 0, 0, 255, 0, pBuffer); // mtb
        Debug_D[4].Draw_vp_(0, 150, 30000, 0, 0, 0, 255, pBuffer); // mtl
        Debug_D[5].Draw_vp_(0, 50, 30000, 0, 128, 0, 0, pBuffer);  // mr受限制标准量
        Debug_D[6].Draw_vp_(0, 100, 30000, 0, 0, 128, 0, pBuffer); // mb
        Debug_D[7].Draw_vp_(0, 150, 30000, 0, 0, 0, 128, pBuffer); // ml
        Debug_D[8].Draw_vp_(0, 300, 100, 0, 255, 255, 0, pBuffer);

        /*
            Debug_D[25].Draw_point_scale(jieqiux,jieqiuy,255,255,255);
            Debug_D[25].DrawCircle_scale(jieqiux,jieqiuy,2,255,255,255);
            Debug_D[25].Draw_vp_string(&Debug_D[36] ,255,255,255);
            Debug_D[25].Draw_vp_string(Debug_D[30].save_frame ,"front_v",0,255,0);
            Debug_D[25].Draw_vp_string(Debug_D[31].save_frame ,"front_tv",0,255,0);
            Debug_D[25].Draw_point_string	   (Debug_D[28].save_frame ,Debug_D[29].save_frame ,1000,255,0,0);
            Debug_D[25].Draw_point_string	   (Debug_D[32].save_frame ,Debug_D[33].save_frame ,1000,0,0,255);
            Debug_D[25].Draw_line_string_polar_coordinates(&Debug_D[31] ,&Debug_D[30] ,320,240,1000,0,255,0);
            Debug_D[25].Draw_line_string_polar_coordinates(&Debug_D[35] ,&Debug_D[34] ,320,240,1000,0,0,255);
            Debug_D[25].Draw_point_string_onwin(Debug_D[26].save_frame ,Debug_D[27].save_frame ,1000,0,255,0,
           "xingwei30");
        */
        Debug_D[25].Draw_point_scale(jieqiux, jieqiuy, 255, 255, 255);
        Debug_D[25].DrawCircle_scale(jieqiux, jieqiuy, 2, 255, 255, 255);
        Debug_D[25].Draw_vp_string(&Debug_D[36], 255, 255, 255);
        Debug_D[25].Draw_vp_string(&Debug_D[37], 255, 255, 255);
        Debug_D[25].Draw_vp_string(&Debug_D[30], 0, 255, 0);
        Debug_D[25].Draw_vp_string(&Debug_D[31], 0, 255, 0);
        Debug_D[25].Draw_point_string(&Debug_D[28], &Debug_D[29], 1000, 255, 0, 0);
        Debug_D[25].Draw_point_string(&Debug_D[32], &Debug_D[33], 1000, 0, 0, 255);
        Debug_D[25].Draw_line_string_polar_coordinates(&Debug_D[31], &Debug_D[30], 320, 240, 1000, 0, 255, 0);
        Debug_D[25].Draw_line_string_polar_coordinates(&Debug_D[35], &Debug_D[34], 320, 240, 1000, 0, 0, 255);
        Debug_D[25].Draw_point_string_onwin(&Debug_D[26], &Debug_D[27], 1000, 0, 255, 0, "xingwei30");
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Catch_robot_f_x = net.rec_targetx; // 1350;//900; //Catch_robot_f_x接球队员X Y坐标
    float Catch_robot_f_y = net.rec_targety; // 600;		  Catch_robot_f_y
    float Pass_robot_move_to_f_x = 800;
    float Pass_robot_move_to_f_y = 600;

    float Pass_f_x1 = 350; // x坐标大于Pass_f_x1米可以开始传球动作
    float Pass_f_x2 = 800; // 700;//x大于Pass_f_x2米就不规避大偏角也传球&&(fabs(kaiqiujiao )<30)

    float field_px_set[4];
    float field_py_set[4];
    field_px_set[0] = 1600; // 1600,600);
    field_py_set[0] = 600;
    field_px_set[1] = 1200; //,1200,150)
    field_py_set[1] = 600;
    field_px_set[2] = 1200; // 1200,1050);
    field_py_set[2] = 400;
    field_px_set[3] = 1200; // 900,600);
    field_py_set[3] = 800;

    int xingwei_Passing_flag = 0;
    int xingwei_Passing_2direct_flag_timer = 0; // 当前是强制传球状态
    int xingwei_Passing_2direct_pass = 0;       // 强制瞄准 射定点控制标志
    int daiqiumb_change_timer_flow_value = 3;   // 2000;//开始强突需要跑过的路点数

    int att_pointx[2] = {1300, 1300}; // 进攻强突选择 点1 和2
    int att_pointy[2] = {400, 800};
    // test
    // Catch_robot_f_x=1800;
    // Catch_robot_f_y=600;

    if (xingwei == 1003) //(xingwei==1002)
    {
        if (y.quanbx > (900 - 50)) // 2012 passing
        {
            xingwei = 1;
            xingwei_Passing_flag = 0;
        }
        else
        {
            xingwei = 107;          //
            net.rec_Def_Dist = 150; //
            net.rec_Def_Ang = 90;
        }
    }

    if (xingwei != 1001)
    {
        xingwei_1001_shoot_end_timer = 0;
    }
    float first_touch_zero_dis = 60; // 每三米 一吐球  第一触球点清零  距离判定
    if (xingwei == 1)                // 2014
    {
        float dist[2];
        dist[0] = f2l_radius(first_touch_bfx, first_touch_bfy, 0, att_pointx[0], att_pointy[0]);
        dist[1] = f2l_radius(first_touch_bfx, first_touch_bfy, 0, att_pointx[1], att_pointy[1]);
        if (dist[0] > dist[1])
        {
            Catch_robot_f_x = att_pointx[0];
            Catch_robot_f_y = att_pointy[0];
        }
        else
        {
            Catch_robot_f_x = att_pointx[1];
            Catch_robot_f_y = att_pointy[1];
        }
        // 传球队员对 接球目标修正
        if ((shoot_cool_time == 0) && (chiqiupd > 3)) // 如果 射门冷却完 且持球 可重新瞄准新目标传球
        {
            xingwei_1001_shoot_end_timer = 0; // 球传出修正接球位置  保持时间标志
        }
        if (shoot_cool_time > 0)
        {
            xingwei_1001_shoot_end_timer = 190;
        }
        if (xingwei_1001_shoot_end_timer > 0)
        {
            float dis = f2l_radius(y.benjix, y.benjiy, 0, net.rec_targetx, net.rec_targety);
            float angle = f2l_angle(y.benjix, y.benjiy, 0, y.quanbx, y.quanby);
            float tx = l2f_x(y.benjix, y.benjiy, 0, angle, dis);
            float ty = l2f_y(y.benjix, y.benjiy, 0, angle, dis);
            net.SetDestToS(tx, ty);
        }
        else
        {
            net.SetDestToS(net.rec_targetx, net.rec_targety);
        }
        xingwei_1001_shoot_end_timer--; //
                                        // 根据三米圈 判断中心 和跑路点目标
        if (first_touch_bt > 0)         // 首次触球 有效计数器0 非触球 >0触球过程中
        {
            if (quxian[int(y.ball_chang)] / cdbl * 2.7 >
                first_touch_zero_dis) // 如果球与机体距离大于100cm 就让带球有效性 自减
            {
                first_touch_bt -= 50; // 首次触球 有效计数器0 非触球 >0触球过程中
            }
            if (first_touch_bt <= 0) // 如果带球有效性减为负 说明可以再次设置新三米圆中心
            {
                first_touch_bfx = 0; // 首次触球 场地坐标系x
                first_touch_bfy = 0; // 首次触球 场地坐标系y
                first_touch_bt = 0;  // 首次触球 有效计数器0 非触球 >0触球过程中
                daiqiumb_change_timer = 0;
                daiqiumb = 0;      //
                field_p_num_P = 0; // 记录上周期（进攻 传球 跑位）目的路点编号 重新计数
            }

            // xingwei1
            xingwei_Passing_flag = 0;
            xingwei = 1;

            // first_touch_bfx;//首次触球 场地坐标系x
            // first_touch_bfy;//首次触球 场地坐标系y
            int d = 180;

            field_px_set[0] = first_touch_bfx + d; //
            field_py_set[0] = first_touch_bfy - d; //
            field_px_set[1] = first_touch_bfx - d; //
            field_py_set[1] = first_touch_bfy + 0; //
            field_px_set[2] = first_touch_bfx + d; //
            field_py_set[2] = first_touch_bfy + d; //

            field_px_set[3] = first_touch_bfx + d; //	预计用不到此点
            field_py_set[3] = first_touch_bfy + 0; //

            float filmXmin = 1000; // 设置这些动态 路点 不可超出的范围
            float filmXmax = 1800 - 100 - 50;
            float filmYmin = 100;
            float filmYmax = 1200 - 100;
            for (i = 0; i < 4; i++)
            {
                if (field_px_set[i] < filmXmin)
                    field_px_set[i] = filmXmin;
                if (field_px_set[i] > filmXmax)
                    field_px_set[i] = filmXmax;

                if (field_py_set[i] < filmYmin)
                    field_py_set[i] = filmYmin;
                if (field_py_set[i] > filmYmax)
                    field_py_set[i] = filmYmax;
            }

            if (daiqiumb_change_timer >= daiqiumb_change_timer_flow_value)
            {
                xingwei_Passing_2direct_flag_timer = 1;
                xingwei = 1;
            }

            if ((xingwei_Passing_2direct_flag_timer == 1)) // 2014
            {
                for (int z = 0; z < 4; z++)
                {
                    field_px_set[z] = Catch_robot_f_x; //
                    field_py_set[z] = Catch_robot_f_y; //
                }
            }

            if (xingwei_Passing_2direct_flag_timer == 1 // 强制传球判定
                && chiqiupd > 10
                //		&& f2l_radius(first_touch_bfx,first_touch_bfy,0,y.quanbx,y.quanby)>=300
            )
            {
                xingwei_Passing_flag = 1;
                xingwei_Passing_2direct_pass = 1;
                xingwei = 1;
            }
            pass_move_stick_timer++;
        }
        else
        { //	说明上周期不在 传球状态 或者不在带球 状态 运行进攻 应执行基础追球
            xingwei = 1;
            xingwei_Passing_flag = 0;
        }
    }
    else if (xingwei == 1001)                         // 2014
    {                                                 // 传球队员对 接球目标修正
        if ((shoot_cool_time == 0) && (chiqiupd > 3)) // 如果 射门冷却完 且持球 可重新瞄准新目标传球
        {
            xingwei_1001_shoot_end_timer = 0; // 球传出修正接球位置  保持时间标志
        }
        if (shoot_cool_time > 0)
        {
            xingwei_1001_shoot_end_timer = 190;
        }
        if (xingwei_1001_shoot_end_timer > 0)
        {
            float dis = f2l_radius(y.benjix, y.benjiy, 0, net.rec_targetx, net.rec_targety);
            float angle = f2l_angle(y.benjix, y.benjiy, 0, y.quanbx, y.quanby);
            float tx = l2f_x(y.benjix, y.benjiy, 0, angle, dis);
            float ty = l2f_y(y.benjix, y.benjiy, 0, angle, dis);
            net.SetDestToS(tx, ty);
        }
        else
        {
            net.SetDestToS(net.rec_targetx, net.rec_targety);
        }
        xingwei_1001_shoot_end_timer--; //
                                        // 根据三米圈 判断中心 和跑路点目标
        if (first_touch_bt > 0)         // 首次触球 有效计数器0 非触球 >0触球过程中
        {
            if (quxian[int(y.ball_chang)] / cdbl * 2.7 >
                first_touch_zero_dis) // 如果球与机体距离大于100cm 就让带球有效性 自减
            {
                first_touch_bt -= 50; // 首次触球 有效计数器0 非触球 >0触球过程中
            }
            if (first_touch_bt <= 0) // 如果带球有效性减为负 说明可以再次设置新三米圆中心
            {
                first_touch_bfx = 0; // 首次触球 场地坐标系x
                first_touch_bfy = 0; // 首次触球 场地坐标系y
                first_touch_bt = 0;  // 首次触球 有效计数器0 非触球 >0触球过程中
                daiqiumb_change_timer = 0;
                daiqiumb = 0;      //
                field_p_num_P = 0; // 记录上周期（进攻 传球 跑位）目的路点编号 重新计数
            }

            xingwei_Passing_flag = 1;
            xingwei = 1;

            // first_touch_bfx;//首次触球 场地坐标系x
            // first_touch_bfy;//首次触球 场地坐标系y
            int d = 180;

            field_px_set[0] = first_touch_bfx + d; //
            field_py_set[0] = first_touch_bfy - d; //
            field_px_set[1] = first_touch_bfx - d; //
            field_py_set[1] = first_touch_bfy + 0; //
            field_px_set[2] = first_touch_bfx + d; //
            field_py_set[2] = first_touch_bfy + d; //

            field_px_set[3] = first_touch_bfx + d; //	预计用不到此点
            field_py_set[3] = first_touch_bfy + 0; //
            /*
            field_px_set[0]=first_touch_bfx+d;//
            field_py_set[0]=first_touch_bfy-d;//
            field_px_set[1]=first_touch_bfx-d;//
            field_py_set[1]=first_touch_bfy-d;//
            field_px_set[2]=first_touch_bfx-d;//
            field_py_set[2]=first_touch_bfy+d;//
            field_px_set[3]=first_touch_bfx+d;//	预计用不到此点
            field_py_set[3]=first_touch_bfy+d;//
            */

            float filmXmin = 100; // 设置这些动态 路点 不可超出的范围
            float filmXmax = 1800 - 100;
            float filmYmin = 100;
            float filmYmax = 1200 - 100;
            for (i = 0; i < 4; i++)
            {
                if (field_px_set[i] < filmXmin)
                    field_px_set[i] = filmXmin;
                if (field_px_set[i] > filmXmax)
                    field_px_set[i] = filmXmax;

                if (field_py_set[i] < filmYmin)
                    field_py_set[i] = filmYmin;
                if (field_py_set[i] > filmYmax)
                    field_py_set[i] = filmYmax;
            }

            if (daiqiumb_change_timer >= daiqiumb_change_timer_flow_value)
            {
                xingwei_Passing_2direct_flag_timer = 1;
                xingwei = 1;
            }

            if ((xingwei_Passing_2direct_flag_timer == 1)) // 2014
            {
                for (int z = 0; z < 4; z++)
                {
                    field_px_set[z] = Catch_robot_f_x; //
                    field_py_set[z] = Catch_robot_f_y; //
                }
            }

            if (xingwei_Passing_2direct_flag_timer == 1 // 强制传球判定
                && chiqiupd > 10 && f2l_radius(first_touch_bfx, first_touch_bfy, 0, y.quanbx, y.quanby) >= 300)
            {
                xingwei_Passing_2direct_pass = 1;
                xingwei = 1;
            }
            pass_move_stick_timer++;
        }
        else
        { //	说明上周期不在 传球状态 或者不在带球 状态 运行进攻 应执行基础追球
            xingwei = 1;
            xingwei_Passing_flag = 0;
        }
    }
    else
    {
        xingwei_Passing_flag = 0;
        daiqiumb_change_timer = 0;
        pass_move_stick_timer = 0;
    }
    /*
    if(xingwei==1001)
    {	  xingwei_Passing_flag=1;
            field_px_set[0]=700-190;//Pass_robot_move_to_f_x;//1600,600);
            field_py_set[0]=600;//Pass_robot_move_to_f_y;
            field_px_set[1]=800-190;//Pass_robot_move_to_f_x;//,1200,150)
            field_py_set[1]=300;//Pass_robot_move_to_f_y;
            field_px_set[2]=700-190;//Pass_robot_move_to_f_x;//1200,1050);
            field_py_set[2]=600;//Pass_robot_move_to_f_y;
            field_px_set[3]=800-190;//Pass_robot_move_to_f_x;//900,600);
            field_py_set[3]=900;//Pass_robot_move_to_f_y;

        if (daiqiumb_change_timer>=daiqiumb_change_timer_flow_value)
        {	xingwei_Passing_2direct_flag_timer=1;
            xingwei=1;
        }
        if (xingwei_Passing_2direct_flag_timer==1
            && y.quanbx>(900-100)
            && chiqiupd>10
            )
        {
            xingwei_Passing_2direct_pass=1;
            xingwei=1;
        }

        if (y.quanbx>(900+0))//(900+50))  //2012 passing
        {
            xingwei=107;//
            net.rec_Def_Dist=130;//400;//
            net.rec_Def_Ang =-90;//1;//15;

    //		xingwei=7;//
    //		net.rec_targetx=900;//Catch_robot_f_x;
    //		net.rec_targety=y.quanby;//200 //600//Catch_robot_f_y;
    //		net.rec_targetang=-f2l_angle (y.benjix,y.benjiy,0, y.quanbx,y.quanby);

            if (xingwei_Passing_2direct_pass==0
                &&(y.ball_chang>10  && y.ball_chang<=70 &&	fabs(y.ball_jiao)<50 && m_pAD[0]>2000 && m_pAD[1]>2000))
                pRobot->Kick(6000);
        }
        else
        {
            xingwei=1;
        }

        pass_move_stick_timer++;

    }
    else
    {	  xingwei_Passing_flag=0;
          daiqiumb_change_timer=0;
          pass_move_stick_timer=0;
    }
    */
    if (xingwei != 1)
    {
        in_fouce_stat = 0; // 不是进攻行为 肯定不在瞄准
    }
    if (xingwei == 1) // 2013
    {
        float temp_pass2catch_A, temp_pass2catch_R, temp_pass2OBJ_A, temp_pass2OBJ_R, temp_Obs_A;
        char pass_able_flag = 1;

        /*	for (i=0;i<in_obj_l;i++)
            {//	m_find.pField->SetBall(i+500,in_obj_x[i],in_obj_y[i]);
                temp_pass2catch_A = f2l_angle (y.benjix,y.benjiy,0,Catch_robot_f_x,Catch_robot_f_y);
                temp_pass2catch_R = f2l_radius(y.benjix,y.benjiy,0,Catch_robot_f_x,Catch_robot_f_y);
                temp_pass2OBJ_A   = f2l_angle (y.benjix,y.benjiy,0,in_obj_x[i]    ,in_obj_y[i]    );
                temp_pass2OBJ_R   = f2l_radius(y.benjix,y.benjiy,0,in_obj_x[i]    ,in_obj_y[i]    );
                temp_Obs_A		  = f2l_angle (0	   ,	   0,0,temp_pass2OBJ_R,190			  );
                if ((fabs(angle_add_transfer2(temp_pass2catch_A,-temp_pass2OBJ_A))<fabs(temp_Obs_A))
                  &&(temp_pass2OBJ_R+123<temp_pass2catch_R))
                {	pass_able_flag=0;
                    break;
                }
            }
        */
        for (i = 0; i < black_OBJ_L; i++)
        { //	m_find.pField->SetBall(i+500,black_OBJ_fx[i]    ,black_OBJ_fy[i]    );
            temp_pass2catch_A = f2l_angle(y.benjix, y.benjiy, 0, Catch_robot_f_x, Catch_robot_f_y);
            temp_pass2catch_R = f2l_radius(y.benjix, y.benjiy, 0, Catch_robot_f_x, Catch_robot_f_y);
            temp_pass2OBJ_A = f2l_angle(y.benjix, y.benjiy, 0, black_OBJ_fx[i], black_OBJ_fy[i]);
            temp_pass2OBJ_R = f2l_radius(y.benjix, y.benjiy, 0, black_OBJ_fx[i], black_OBJ_fy[i]);
            temp_Obs_A = f2l_angle(0, 0, 0, temp_pass2OBJ_R, 60 / cdbl * 2.7);
            if ((fabs(angle_add_transfer2(temp_pass2catch_A, -temp_pass2OBJ_A)) < fabs(temp_Obs_A)) &&
                (temp_pass2OBJ_R + 123 < temp_pass2catch_R))
            {
                pass_able_flag = 0;
                break;
            }
        }
        //	if (Catch_robot_f_x <1000)
        //	{
        //		pass_able_flag=0;
        //	}

        char kick_fouce = 0;
        float pinqiangpd, pinqiang_L_pd, pinqiang_R_pd;
        pinqiang_R_pd = m_find.qijiaopanding(y, 3);
        pinqiang_L_pd = m_find.qijiaopanding(y, 4);
        Display_C = pinqiang_L_pd + pinqiang_R_pd;
        pinqiangpd = Display_C;
        in_fouce_stat = 0; // 瞄准变量先清零
        //	if (valxx!=1)
        //	{
        //		Dribble_ball.fount_xVset =0;
        //		Dribble_ball.fount_yVset =0;
        //	}
        //	Dribble_ball.fount_xVset=(Dribble_ball.fount_xVset+l2f_x(0,0,Dribble_ball.robot_L.f_a,Dribble_ball.RMfa,Dribble_ball.RMfv)*22.648)/2.0;//*0.5;//
        //	Dribble_ball.fount_yVset=(Dribble_ball.fount_yVset+l2f_y(0,0,Dribble_ball.robot_L.f_a,Dribble_ball.RMfa,Dribble_ball.RMfv)*22.648)/2.0;//*0.5;//
        //	if ((valxx==0)||(xingweiP!=1))// 1说明本周期运行过 0说明没运行过 要连接数据
        //	{
        //		Dribble_ball.fount_xVset=l2f_x(0,0,Dribble_ball.robot_L.f_a,Dribble_ball.RMfa,Dribble_ball.RMfv)*22.648*1.4;//1.2;
        //		Dribble_ball.fount_yVset=l2f_y(0,0,Dribble_ball.robot_L.f_a,Dribble_ball.RMfa,Dribble_ball.RMfv)*22.648*1.4;//1.2;
        //	}
        //	Dribble_ball.fount_xVset=l2f_x(0,0,Dribble_ball.robot_L.f_a,Dribble_ball.RMfa,Dribble_ball.RMfv)*22.648*1.2;
        //	Dribble_ball.fount_yVset=l2f_y(0,0,Dribble_ball.robot_L.f_a,Dribble_ball.RMfa,Dribble_ball.RMfv)*22.648*1.2;
        // 上周期电机发送值历史记录		mhL [0];		mhB [0];		mhR [0];
        // test
        //	Dribble_ball.fount_xVset=(y.quanbx-y.benjix)*field_zoom_time *0.1;
        //	Dribble_ball.fount_yVset=(y.quanby-y.benjiy)*field_zoom_time *0.1;;

        mtl = 0;
        mtr = 0;
        mtb = 0;
        //	float mb,ml,mr,front_v,front_a,turn_w;
        //	Dribble_ball.Ball_Track3(&mb,&ml,&mr);//接球

        double jiao = 0;
        double chang = 0;
        double jiaoj;
        double changj;
        double xx;
        double yy;
        double jiaox;
        double changx;
        RGBTRIPLE yanse;
        yanse.rgbtBlue = 0;
        yanse.rgbtGreen = 255;
        yanse.rgbtRed = 0;
        bjjyc += yczhuan;
        // 球坐标历史

        // 球行进方向
        for (i = 0; i < 5; i++)
        {
            m_find.jizuobiao(bxyc[i], byyc[i], &jiaoj, &changj, bxyc[i + 25], byyc[i + 25]);
            jiao += jiaoj;
            chang += changj;
        }
        jiao = jiao / 5;
        chang = chang / 5;
        if (chang > 100)
            chang = 100;
        // 显示前进方向
        for (i = 0; i < chang * 2; i++)
        {
            m_find.zhizuobiao(&xx, &yy, jiao, i, y.ball_x, y.ball_y);
            m_find.SetColor(xx, yy, yanse);
        }

        m_find.jizuobiao(y.benjix, y.benjiy, &jiaox, &changx, y.quanbx, y.quanby);

        for (i = 0; i < 100; i++)
        {
            m_find.zhizuobiao(&xx, &yy, jiaox, i, y.ball_x, y.ball_y);
            m_find.SetColor(xx, yy, yanse);
        }

        /*	if(Path_LA-Path_LA_P>20)
                Path_LA=Path_LA_P+20;
            if(Path_LA-Path_LA_P<-20)
                Path_LA=Path_LA_P-20;*/
        if (shemenleijia > 0)
            pqzy = 0;
        if (pqzy < 10)
        {
            pqzy++;
        }
        else
        {
            if (m_find.sijiao(y) == 2)
                pqzy1 = int(y.benjix) % 2;
            else if (m_find.sijiao(y) > 0)
                pqzy1 = 1;
            else
                pqzy1 = 0;
            pqzy = 0;
        }
        if (xingwei_Passing_flag == 0) // 射门
        {
            if (pqzy1 == 1)
                m_find.jizuobiao(1805, 660, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
            else
                m_find.jizuobiao(1805, 540, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
            //		m_find.jizuobiao(1805,700-29/*600+100*/,&dingweijiao,&dingweichang,y.benjix,y.benjiy);
            //	else
            //		m_find.jizuobiao(1805,500+29/*600-100*/,&dingweijiao,&dingweichang,y.benjix,y.benjiy);
        }
        else // 传球
        {
            if (pqzy1 == 1)
                m_find.jizuobiao(Catch_robot_f_x, Catch_robot_f_y, &dingweijiao, &dingweichang, y.benjix,
                                 y.benjiy); //(1805,660,&dingweijiao,&dingweichang,y.benjix,y.benjiy);
            else
                m_find.jizuobiao(Catch_robot_f_x, Catch_robot_f_y, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
        }
        dingweijiao = -jiajiao(-dingweijiao, y.benjijiao - 90);
        // 根据教练机给的球的位置,将这个坐标投影到视觉
        kaiqiujiao = dingweijiao;
        kaiqiuchang = fanquxian[int(dingweichang * cdbl / 2.7)];
        motospeed[3] = 16000;
        motospeed[4] = -16000;
        if (m_pAD[0] > 2000 && m_pAD[1] > 2000 && y.ball_chang < 55 && abs(y.ball_jiao) < 20)
        {
            chiqiupd++; //+=10;//
            if (first_touch_bt <= 0)
            {
                first_touch_bfx = y.benjix; // 首次触球 场地坐标系x
                first_touch_bfy = y.benjiy; // 首次触球 场地坐标系y
                first_touch_bt = 100;       // 首次触球 有效计数器0 非触球 >0触球过程中
            }
            else
            {
                first_touch_bt = 100; // 首次触球 有效计数器0 非触球 >0触球过程中
            }
        }
        else
        {
            chiqiupd = 0; //-=15;//
        }
        //	chiqiupd=50;
        //	if(m_pAD[0]<1000 || m_pAD[1]<1000 || y.ball_chang>47 || abs(y.ball_jiao)>20)//(1)//
        if (chiqiupd < 3) // 18)//2014test
        {
            //		shemenleijia=0;
            motospeed[3] = 16000;
            motospeed[4] = -16000;
            pp1.path_plan_start_angle_cost = 1.5; // 2.4;
            pp1.Obstacle_size_set = Obstacle_Parameter /
                                    field_zoom_time; // 8.2/0.4=20  实验室场地 20的障碍设置大小 设置每个障碍点的影响范围
            pp1.datain(y.benjix, y.benjiy, y.quanbx,
                       y.quanby); // 输入路径规划起始点和终点 y.benjix,y.benjiy起点  y.quanbx,y.quanby终点
            pp1.Ppgrow();
            Path_LA = angle_add_transfer(-pp1.path_plan_angle,
                                         y.benjijiao); // aaaa是路径规划结果方向 前方0度 右正左负 顺时针坐标系
            if (0)                                     //(chang>100 && abs(jiajiao(jiao,jiaox))<60)||(path_l<1))
            // if((chang>20 && abs(jiajiao(jiao,jiaox))<80)||(path_l<1))//
            {
                if ((m_pAD[0] > 1000 || m_pAD[1] > 1000) && (chang > 25 && abs(jiajiao(jiao, jiaox)) < 60) &&
                    (fabs(y.ball_jiao) < 20 && y.ball_chang < 50 && y.ball_zhong > 310)) // 接球参数清零
                {
                    //	float xxxx=(y.quanbx-y.benjix)*field_zoom_time *0.3;//0.05;
                    //	float yyyy=(y.quanby-y.benjiy)*field_zoom_time *0.3;//0.05;
                    Dribble_ball.fount_xVset = Dribble_ball.fount_xVset * 0.25;
                    Dribble_ball.fount_yVset = Dribble_ball.fount_yVset * 0.25;
                }
                if (chang > 25 && abs(jiajiao(jiao, jiaox)) < 60)
                {
                    motospeed[3] = 16000;  //
                    motospeed[4] = -16000; //
                }
                Dribble_ball.Dribble_ball_other_moment_flag = 1; // 先运行了其他程序再运行带球函数标志
                mtb = xingwei1mb;
                mtl = xingwei1ml;
                mtr = xingwei1mr;
                O_movement_T_analyze(
                    &front_a, &front_v, &turn_w, xingwei1ml, xingwei1mb,
                    xingwei1mr); //-mhLR[1]->Pos_[0],-mhBR[1]->Pos_[2],-mhRR[1]->Pos_[1]);//[0]是上个周期的结果
                                 //*move_v_angle, *move_v_speed, *turn_v
                jifen = fabs(front_v);
                zhuan = 0;
                mtl = mtl * 0.6;
                mtr = mtr * 0.6;
                mtb = mtb * 0.6;
            }
            else
            {
                //		Dribble_ball.fount_xVset=0;
                //		Dribble_ball.fount_yVset=0;
                /*cs加*/ // 上周期电机发送值历史记录		mhL [0];		mhB [0];		mhR [0];
                /*cs加*/ //	O_movement_T_analyze(&front_a,&front_v,&turn_w,Dribble_ball.mhL [0],Dribble_ball.mhB
                         //[0],Dribble_ball.mhR
                         //[0]);//-mhLR[1]->Pos_[0],-mhBR[1]->Pos_[2],-mhRR[1]->Pos_[1]);//[0]是上个周期的结果
                         //*move_v_angle, *move_v_speed, *turn_v
                /*cs加*/ //	jifen=fabs(front_v)*cos(front_a*3.14/180.0);//得到上周期电机发送速度 前进分量
                if (1)   //(pinqiangpd>600)
                {
                    zhuiqiu(y.ball_jiao, y.ball_chang, chang_shang);
                }
                else
                {
                    zhuiqiu_(y.ball_jiao, y.ball_chang, chang_shang);
                }
                //			Dribble_ball.fount_xVset=l2f_x(0,0,Dribble_ball.robot_L.f_a,Dribble_ball.RMfa,Dribble_ball.RMfv)*22.648*1.4;//1.2;
                //			Dribble_ball.fount_yVset=l2f_y(0,0,Dribble_ball.robot_L.f_a,Dribble_ball.RMfa,Dribble_ball.RMfv)*22.648*1.4;//1.2;
            }
            if (0) //((chiqiupd>0)&&(pinqiangpd<600))
            {
                float vset = 3000; //(16000-abs(Path_LA)*90);
                                   //	if (vset<2000)
                                   //		vset=2000;
                if (jifenCS < vset)
                    jifenCS += 400;
                if (jifenCS > vset)
                    jifenCS -= 400;
                if (jifenCS < 0)
                    jifenCS = 0;
                jifen = jifenCS;
            }
            else
            {
                jifenCS = jifen;
            }
            // if(jifen>6000)
            //  jifen=6000;
        }
        else
        {
            net.toS_status = CTRL_ATTACK;
            jg_wending -= 0.03;
            if (jg_wending < 0.2)
                jg_wending = 2.5;
            pp1.path_plan_start_angle_cost = jg_wending; // 2.5;//3.6;//2.4;//1.5;

            if (xingwei_Passing_flag == 1)
            {                          // 改为传球者可以全场跑位带球 路径可以穿过中线
                pp1.use_film_line = 5; // 1;//设置路径规划考虑场地边线 原路经规划考虑场线 在场地范围内带球  mask3.bmp
                pp1.ftb_circle_ox = first_touch_bfx; // use_film_line=5专用参数 指定3m圆中心
                pp1.ftb_circle_oy = first_touch_bfy;
            }
            else
            { // 行为1进攻路径 不可穿过中线//现在行为1 只能在对方半场带球
                //	pp1.use_film_line=3;//设置路径规划考虑场地边线 且优先只在对方半场带球 mask5.bmp
                pp1.use_film_line = 6; // 1;//设置路径规划考虑场地边线 原路经规划考虑场线 在场地范围内带球  mask3.bmp
                pp1.ftb_circle_ox = first_touch_bfx; // use_film_line=5专用参数 指定3m圆中心
                pp1.ftb_circle_oy = first_touch_bfy;
            }
            pp1.Obstacle_size_set = Obstacle_Parameter * 3 /
                                    field_zoom_time; // 8.2/0.4=20  实验室场地 20的障碍设置大小 设置每个障碍点的影响范围
            //		pp1.datain(y.benjix,y.benjiy,1800,600);//输入路径规划起始点和终点 y.benjix,y.benjiy起点
            // y.quanbx,y.quanby终点
            int field_px = 0;
            int field_py = 0;
            int field_p_num = 0;

            daiqiumb++;
            if (xingwei_Passing_flag == 1)
            { // 普通传球 跑的路点
                /*	if(daiqiumb<300)
                    {	field_p_num=0;
                    }//输入路径规划起始点和终点 y.benjix,y.benjiy起点  y.quanbx,y.quanby终点
                    else if(daiqiumb<600)
                    {	field_p_num=1;
                    }
                    else if(daiqiumb<900)
                    {	field_p_num=2;
                    }
                    else if(daiqiumb<1200)
                    {	field_p_num=3;
                    }
                    else
                    {	field_p_num=0;
                        daiqiumb=0;
                    }
                    */
                if (daiqiumb < 300)
                {
                    field_p_num = 0;
                } // 输入路径规划起始点和终点 y.benjix,y.benjiy起点  y.quanbx,y.quanby终点
                else if (daiqiumb < 600)
                {
                    field_p_num = 1;
                }
                else if (daiqiumb < 900)
                {
                    field_p_num = 2;
                }
                else
                {
                    field_p_num = 0;
                    daiqiumb = 0;
                }
            }
            else
            { // 普通进攻 跑的路点
                /*		if(daiqiumb<100)
                        {	field_p_num=0;
                        }//输入路径规划起始点和终点 y.benjix,y.benjiy起点  y.quanbx,y.quanby终点
                        else if(daiqiumb<300)
                        {	field_p_num=1;
                        }
                        else if(daiqiumb<500)
                        {	field_p_num=2;
                        }
                        else if(daiqiumb<800)
                        {	field_p_num=3;
                        }
                        else
                        {	field_p_num=0;
                            daiqiumb=0;
                        }
                */
                if (daiqiumb < 300)
                {
                    field_p_num = 0;
                } // 输入路径规划起始点和终点 y.benjix,y.benjiy起点  y.quanbx,y.quanby终点
                else if (daiqiumb < 600)
                {
                    field_p_num = 1;
                }
                else if (daiqiumb < 900)
                {
                    field_p_num = 2;
                }
                else
                {
                    field_p_num = 0;
                    daiqiumb = 0;
                }
            }

            field_px = field_px_set[field_p_num]; // 1600;//1600,600);
            field_py = field_py_set[field_p_num]; // 600;

            if (abs(y.benjix - field_px) < 100 && abs(y.benjiy - field_py) < 100)
            {
                daiqiumb += 100;
                //	daiqiumb_change_timer++;
            }

            // if ((daiqiumb==100)||(daiqiumb==300)||(daiqiumb==500)||(daiqiumb==0))
            //	daiqiumb_change_timer++;

            if (field_p_num_P != field_p_num)
            {
                daiqiumb_change_timer++;
            }
            field_p_num_P = field_p_num;

            // 在需要规避的时候gbpanding==0 不能做2配合传球需要继续带球不能冲
            //		if ((xingwei_Passing_flag==1)&&gbpanding==0
            //		&&	daiqiumb_change_timer>=daiqiumb_change_timer_flow_value)
            //			daiqiumb_change_timer =daiqiumb_change_timer_flow_value-1;

            //	if ((daiqiumb_change_timer>=daiqiumb_change_timer_flow_value)&&(xingwei_Passing_flag==1))
            // 强制冲到中线附近传球 给两机强突用
            //	if ((xingwei_Passing_flag==1)&&(xingwei_Passing_2direct_flag_timer==1))	//2013
            //	if ((xingwei_Passing_2direct_flag_timer==1))							//2014
            //	{
            //		field_px=Catch_robot_f_x;//1600;//1600,600);
            //		field_py=Catch_robot_f_y;//600;
            //	}
            QueryPerformanceCounter(&CountEnd);
            dfElapseMS3 =
                (double)((double)(CountEnd.QuadPart - CountStart.QuadPart + 10) / (double)Frequency.QuadPart) * 1000.0 -
                dfElapseMS - dfElapseMS1 - dfElapseMS2;

            m_find.pField->SetAgent(12, field_px, field_py, pp1.path_plan_angle);
            pp1.datain(y.quanbx, y.quanby, field_px, field_py);
            // pp1.datain(y.benjix,y.benjiy,field_px,field_py);

            QueryPerformanceCounter(&CountEnd);
            dfElapseMS4 =
                (double)((double)(CountEnd.QuadPart - CountStart.QuadPart + 10) / (double)Frequency.QuadPart) * 1000.0 -
                dfElapseMS - dfElapseMS1 - dfElapseMS2 - dfElapseMS3;

            pp1.Ppgrow();
            Path_LA = angle_add_transfer(-pp1.path_plan_angle,
                                         y.benjijiao); // aaaa是路径规划结果方向 前方0度 右正左负 顺时针坐标系
            QueryPerformanceCounter(&CountEnd);
            dfElapseMS5 =
                (double)((double)(CountEnd.QuadPart - CountStart.QuadPart + 10) / (double)Frequency.QuadPart) * 1000.0 -
                dfElapseMS - dfElapseMS1 - dfElapseMS2 - dfElapseMS3 - dfElapseMS4;
            //		Display_A=Path_LA;
            /*		Path_FA=pp1.path_plan_angle;
                    float Path_DLA=angle_add_transfer(-Path_FA_P,Path_FA);
                    float ddda=(Path_DLA*Path_DLA/45.0/45.0)+0.3;		if (ddda>1)		ddda=1;
                    Path_FA=angle_add_transfer(Path_FA_P,Path_DLA*ddda);
                    Path_LA=angle_add_transfer(-Path_FA,y.benjijiao);//(-pp1.path_plan_angle,y.benjijiao);//Path_LA是路径规划结果方向
               前方0度 右正左负 顺时针坐标系
            */

            if (y.benjix > 1700)
                gbpanding = 5;
            int xzhang, zzhang, dzhang;
            if (xingwei_Passing_flag == 0)
            {
                xzhang = m_find.qijiaopanding(y, 0);
                zzhang = m_find.qijiaopanding(y, 1) + xzhang;
                //	dzhang=m_find.qijiaopanding(y,2);
            }
            else
            {
                double temp_y_ddoor_jiao = y.ddoor_jiao;
                double temp_y_ddoor_chang = y.ddoor_chang;
                y.ddoor_jiao = kaiqiujiao;
                y.ddoor_chang = kaiqiuchang;
                xzhang = m_find.qijiaopanding(y, 0);
                zzhang = m_find.qijiaopanding(y, 1) + xzhang;
                //	dzhang=m_find.qijiaopanding(y,2);
                y.ddoor_jiao = temp_y_ddoor_jiao;
                y.ddoor_chang = temp_y_ddoor_chang;
            }
            QueryPerformanceCounter(&CountEnd);
            dfElapseMS6 =
                (double)((double)(CountEnd.QuadPart - CountStart.QuadPart + 10) / (double)Frequency.QuadPart) * 1000.0 -
                dfElapseMS - dfElapseMS1 - dfElapseMS2 - dfElapseMS3 - dfElapseMS4 - dfElapseMS5;

            if (daduobi == 0 && xzhang > 200)
            {
                daduobi = 1;
                gbpanding = -20;
            }
            if (daduobi == 1 && zzhang < 300)
            {
                daduobi = 0;
            }
            if (xingwei_Passing_flag == 0) // 2012 passing
            {
                if (daduobi == 0)
                {
                    if (xzhang < 200 && y.benjix > 900)
                        gbpanding++;
                    else
                        gbpanding = 0;
                }
                else
                {
                    if (zzhang < 300 && y.benjix > 900)
                        gbpanding++;
                    else
                        gbpanding = 0;
                }
            }
            else // 2012 passing
            {
                if (daduobi == 0)
                {
                    if (xzhang < 200) // 任何位置可以传球				不执行	&&
                                      // y.benjix>Pass_f_x1)//x坐标大于Pass_f_x1米可以开始传球动作
                        gbpanding++;
                    else
                        gbpanding = 0;
                }
                else
                {
                    if (zzhang < 300) // 任何位置可以传球				不执行	&&
                                      // y.benjix>Pass_f_x1)//x坐标大于Pass_f_x1米可以开始传球动作
                        gbpanding++;
                    else
                        gbpanding = 0;
                }
            }
            if ((xingwei_Passing_flag == 1) && (pass_able_flag == 0))
            {
                gbpanding = 5;
            }
            if (((gbpanding > 10 || (y.benjix > 1500 && y.benjiy > 300 && y.benjiy < 900)))
                //	||((xingwei_Passing_flag==1)&&(y.benjix>Pass_f_x2)&&(fabs(y.benjijiao)<60))
                ////大于Pass_f_x2米就不规避大致朝前传球&&(fabs(y.benjijiao)<60)  2012 passing
                // 2014		||((xingwei_Passing_flag==1)&&(y.benjix>Pass_f_x2)&&(fabs(kaiqiujiao )<30))
                // //大于Pass_f_x2米就不规避大偏角也传球&&(fabs(kaiqiujiao )<30)  2012 passing
                || ((xingwei_Passing_flag == 1) &&
                    (gbpanding > 10)) // 大于Pass_f_x2米就不规避大偏角也传球&&(fabs(kaiqiujiao )<30)  2012 passing
                //			||((xingwei_Passing_flag==1)&&(xingwei_Passing_2direct_pass==1))//强制冲到中线附近传球
                // 给两机强突用
                || ((xingwei_Passing_2direct_pass == 1)) // 强制冲到中线附近传球 给两机强突用
            )
            //	;	if(0)//0只测试带球代码//2014test
            //	;	if   ((xingwei_Passing_2direct_pass==1))//测试 强制冲到中线附近传球 给两机强突用
            // 强制冲到中线附近传球 给两机强突用		测试
            //	;	if((xingwei_Passing_flag==1)&&(xingwei_Passing_2direct_pass==1))
            {
                if (xingwei_Passing_flag == 1)
                {
                    in_fouce_stat = 1; // 我在瞄准接球队员不要动
                }
                else
                {
                    in_fouce_stat = 0; // 我没在瞄准
                }
                kick_fouce = 1;
                motospeed[3] = 2000;
                motospeed[4] = -2000;
                if (jifen >= 300)
                    jifen -= 150;
                if (jifen < 300)
                    jifen = 300;
                //	if ((xingwei_Passing_flag==1)&&(xingwei_Passing_2direct_pass==1))
                //	{
                //		if(jifen>=300)
                //			jifen-=350;
                //	}
                if (jifen > 1500)
                {
                    motospeed[3] = -2000;
                    motospeed[4] = 2000;
                }
                if (jifen > 2000)
                {
                    motospeed[3] = -4000;
                    motospeed[4] = 4000;
                }
                if (jifen > 3000)
                {
                    motospeed[3] = -6000;
                    motospeed[4] = 6000;
                }
                if (jifen > 4000)
                {
                    motospeed[3] = -8000;
                    motospeed[4] = 8000;
                }

                //	zhuan=2*duizhun1(kaiqiujiao,0,1500);
                zhuan = 3 * duizhun1(kaiqiujiao, 0, 1000); // 2013cstest

                if (abs(zhuan) > abs(zhuan_shang))
                {
                    if (zhuan - zhuan_shang > 120)
                        zhuan = zhuan_shang + 120;
                    if (zhuan - zhuan_shang < -120)
                        zhuan = zhuan_shang - 120;
                }
                motospeed[3] += abs(zhuan);
                motospeed[4] -= abs(zhuan);
                //	if(jifen<abs(zhuan/2))
                //		jifen=abs(zhuan/2);
                /*			Controlmoto(0,jifen,12000,&mtl,&mtr,&mtb);
                        //	Controlmoto(-90,-1.5*zhuan,15000,&mtl,&mtr,&mtb);
                        //	Controlmoto(-90,-1.3*zhuan,15000,&mtl,&mtr,&mtb);//2013cstest
                            Controlmoto(0,abs(zhuan)*1.8,15000,&mtl,&mtr,&mtb);//(0,abs(zhuan)*1.59,15000,&mtl,&mtr,&mtb);//2013cstest
                            Controlmoto(-90,-1.5*zhuan,15000,&mtl,&mtr,&mtb);//(-90,-1.9*zhuan,15000,&mtl,&mtr,&mtb);//2013cstest
                */

                Controlmoto(0, jifen, 20000, &mtl, &mtr, &mtb);
                if (abs(kaiqiujiao) > 15)
                    Controlmoto(-90, -1.5 * zhuan, 20000, &mtl, &mtr, &mtb);

                float tra_dis = f2l_radius(y.benjix, y.benjiy, 0, Catch_robot_f_x, Catch_robot_f_y);
                float aim_angle_err = fabs((1800 - tra_dis) / 1800.0 * 5 + 6);
                //	float aim_angle_err=f2l_angle (0	   ,	   0,0,tra_dis,60/cdbl*2.7	  );

                if ((y.ball_chang > 10 && y.ball_chang <= 70 && jiajiao(0, y.ddoor_zuo) <= -0 &&
                     jiajiao(0, y.ddoor_you) >= 0 && abs(y.ball_jiao) < 50 && m_pAD[0] > 1800 && m_pAD[1] > 1800)
                    //				||((y.ball_chang>10  && y.ball_chang<=70 &&	fabs(y.ball_jiao)<50 && m_pAD[0]>1800 &&
                    // m_pAD[1]>1800)&&(xingwei_Passing_flag==1)&&(y.benjix>Pass_f_x2)&&(fabs(y.benjijiao)<60)) 2014
                    // ||((y.ball_chang>10  && y.ball_chang<=70 &&	fabs(y.ball_jiao)<50 && m_pAD[0]>1800 &&
                    // m_pAD[1]>1800)&&(xingwei_Passing_flag==1)&&(y.benjix>Pass_f_x2)&&(fabs(kaiqiujiao )<15) &&
                    // Catch_robot_f_x>950) 2014			||((y.ball_chang>10  && y.ball_chang<=70 &&
                    // fabs(y.ball_jiao)<50 && m_pAD[0]>1800 && m_pAD[1]>1800)&&(xingwei_Passing_flag==1)&&
                    // (fabs(kaiqiujiao)<=5)&& Catch_robot_f_x>950)
                    || ((y.ball_chang > 10 && y.ball_chang <= 70 && fabs(y.ball_jiao) < 50 && m_pAD[0] > 1800 &&
                         m_pAD[1] > 1800) &&
                        (xingwei_Passing_flag == 1) && (fabs(kaiqiujiao) < 10)))
                { // 满足机器不在规避，并且球长度在24内，门的相对角度小于5度，球的角度小于30

                    if (xingwei_Passing_flag == 1)
                    {
                        //		if(abs(kaiqiujiao)<10)
                        //			shemenleijia=shemenleijia+(10-abs(kaiqiujiao));
                        //		else
                        //			shemenleijia++;//射门计数器开始工作
                        if (fabs(kaiqiujiao) < aim_angle_err)
                            shemenleijia = shemenleijia + (aim_angle_err - fabs(kaiqiujiao)) / aim_angle_err * 10.0;
                        else
                            shemenleijia++; // 射门计数器开始工作
                    }
                    else
                    {
                        if (abs(kaiqiujiao) < 10)
                            shemenleijia = shemenleijia + (10 - 0.5 * abs(kaiqiujiao));
                        else
                            shemenleijia++; // 射门计数器开始工作
                    }
                }
                else
                    shemenleijia = 0;
                if (shemenleijia > 0)
                {
                    motospeed[3] = 16000;
                    motospeed[4] = -16000;
                }
                // 球过中线临时起脚判定
                //			if(xingwei_Passing_flag==1 && (y.ball_chang>10  && y.ball_chang<=70 &&	fabs(y.ball_jiao)<50
                //&& m_pAD[0]>1200 && m_pAD[1]>1200) && y.benjix>850 && abs(y.benjijiao)<90)
                //			{
                //				shemenleijia=101;
                //			}
                // shemenleijia=0;//2013cstest
                // passing过程 机会起脚判定
                if (xingwei_Passing_flag == 1 &&
                    (y.ball_chang > 10 && y.ball_chang <= 70 && fabs(y.ball_jiao) < 50 && m_pAD[0] > 1200 &&
                     m_pAD[1] > 1200) &&
                    (abs(kaiqiujiao) < 6) && Catch_robot_f_x > 950) // 10+5
                {
                    if (pass_able_flag == 1)
                        shemenleijia = 101;
                    else
                        shemenleijia = 0;
                }
                // 强制冲到中线附近传球 给两机强突用
                if ((xingwei_Passing_2direct_pass == 1) && (abs(kaiqiujiao) < 10))
                {
                    shemenleijia = 101;
                }

                if (((xingwei_Passing_flag == 0 && shemenleijia > 180 && valxx == 1) ||
                     (xingwei_Passing_flag == 1 && shemenleijia > 50 && valxx == 1)) &&
                    (shoot_cool_time == 0)) // && jifen<400 ))
                {
                    shoot_cool_time = 190;
                    shemenleijia = 0;
                    if (xingwei_Passing_flag == 0)
                    {
                        if (abs(shemenb[abs(dingweichang * cdbl / 2.7 - abs(jifen / 200))]) > 16700)
                        {
                            pRobot->Kick(abs(shemenb[abs(dingweichang * cdbl / 2.7 - abs(jifen / 200))]) -
                                         1000); // t(jifen/5)]);//dmenchang/50*1000+0);
                            // m_pCmd->Kick(8000-jifen/5);
                        }
                        else
                            pRobot->Kick(16700);
                        //	m_pCmd->Kick(10000);
                        // m_pCmd->Kick(4000);
                    }
                    else
                    {
                        if (xingwei_Passing_2direct_pass == 1)
                        {
                            pRobot->Kick(5000); //(3500);
                            net.AttentionKick(y.benjix, y.benjiy, net.rec_targetx, net.rec_targety);
                        }
                        else
                        {
                            // if (.benjix>Pass_f_x2)
                            //	m_pCmd->Kick(6900);//(5000);//t(jifen/5)]);//dmenchang/50*1000+0);
                            // else
                            //	m_pCmd->Kick(8600-);//(4000);//t(jifen/5)]);//dmenchang/50*1000+0);
                            double kick_dis = sqrt((Catch_robot_f_x - y.benjix) * (Catch_robot_f_x - y.benjix) +
                                                   (Catch_robot_f_y - y.benjiy) * (Catch_robot_f_y - y.benjiy));
                            pRobot->Kick(8500 - jifen * 0.28 + kick_dis / 2);
                            if (shoottime == 0)
                                shoottime = 8500 - jifen * 0.28 + kick_dis / 2; // 2013 test
                            net.AttentionKick(y.benjix, y.benjiy, net.rec_targetx, net.rec_targety);
                        }
                    }
                }
            }
            else
            {
                if (0) // fabs(Path_LA)<60)// && pxqiehuan>30)// || jifen<3000)//带球行进 吸球行进 判定
                {
                    pxqiehuanbj = 0;
                    // 				if(fabs(Path_LA)<60)
                    // 					pxqiehuan=0;
                    // 				else
                    // 					pxqiehuan=1;
                    //		motospeed[3]=10000;//-jifen*5;//+abs(Path_LA)*100;
                    //		motospeed[4]=-10000;//+jifen*5;//-abs(Path_LA)*100;
                    //			xiqiujiexi(mtl,mtr,mtb,0.02,&motospeed[3],&motospeed[4]);
                    /*cs加*/ //		O_movement_T_analyze(&front_a,&front_v,&turn_w,Dribble_ball.mhL [0],Dribble_ball.mhB
                             //[0],Dribble_ball.mhR
                             //[0]);//-mhLR[1]->Pos_[0],-mhBR[1]->Pos_[2],-mhRR[1]->Pos_[1]);//[0]是上个周期的结果
                             //*move_v_angle, *move_v_speed, *turn_v
                    /*cs加*/ //		jifen=fabs(front_v)*cos(front_a*3.14/180.0);//得到上周期电机发送速度 前进分量
                    float vset = (16000 - abs(Path_LA) * 90);
                    if (vset < 2000)
                        vset = 2000;
                    if (jifen < vset)
                        jifen += 150;
                    if (jifen > vset)
                        jifen -= 150;
                    if (jifen < 0)
                        jifen = 0;
                    if (-abs(Path_LA) * 7 + 50 > 0)
                        jifen = jifen - abs(Path_LA) * 7 + 50;
                    motospeed[3] = 15000 - jifen * 5;
                    motospeed[4] = -15000 + jifen * 5;
                    if (motospeed[3] < -3000)
                        motospeed[3] = -3000;
                    if (motospeed[4] > 3000)
                        motospeed[4] = 3000;
                    //		jifen=4000;

                    Controlmoto(-90, -1.0 * zhuan * fabs(Path_LA) / 60, 20000, &mtl, &mtr, &mtb);
                    Controlmoto(-Path_LA * (1 - fabs(Path_LA) / 60), jifen, 20000, &mtl, &mtr, &mtb);
                    if (m_pAD[1] < 2400 || m_pAD[0] < 2400)
                        zhuan = 0;
                    else
                        zhuan = 2 * duizhun(-Path_LA, 0, 300);
                    if (abs(zhuan) > abs(zhuan_shang))
                    {
                        if (zhuan - zhuan_shang > 60)
                            zhuan = zhuan_shang + 60;
                        if (zhuan - zhuan_shang < -60)
                            zhuan = zhuan_shang - 60;
                    }
                    if (abs(Path_LA) < 60 && jifen < 1000)
                    {
                        motospeed[3] = 8000;
                        motospeed[4] = -8000;
                    }
                    if (abs(Path_LA) < 60 && jifen > 1000)
                    {
                        motospeed[3] = -1000;
                        motospeed[4] = 1000;
                    }
                    if (abs(Path_LA) < 60 && jifen > 2000)
                    {
                        motospeed[3] = -3000;
                        motospeed[4] = 3000;
                    }
                    if (abs(Path_LA) < 60 && jifen > 6000)
                    {
                        motospeed[3] = -4000;
                        motospeed[4] = 4000;
                    }
                    if (abs(Path_LA) < 60 && jifen > 8000)
                    {
                        motospeed[3] = -12000;
                        motospeed[4] = 12000;
                    }
                    motospeed[3] += abs(zhuan * 3);
                    motospeed[4] -= abs(zhuan * 3);
                }
                else
                {
                    if (pxqiehuanbj == 0)
                    {
                        pxqiehuanbj = 1;
                        pxqiehuan = 0;
                    }
                    pxqiehuan++;
                    motospeed[3] = 3000;
                    motospeed[4] = -3000;

                    /*cs加*/ //		O_movement_T_analyze(&front_a,&front_v,&turn_w,Dribble_ball.mhL [0],Dribble_ball.mhB
                             //[0],Dribble_ball.mhR
                             //[0]);//-mhLR[1]->Pos_[0],-mhBR[1]->Pos_[2],-mhRR[1]->Pos_[1]);//[0]是上个周期的结果
                             //*move_v_angle, *move_v_speed, *turn_v
                    /*cs加*/ //		jifen=fabs(front_v)*cos(front_a*3.14/180.0);//得到上周期电机发送速度 前进分量
                             //		float			vset=(8000-abs(zhuan));
                    // 带球带球带球带球带球带球带球带球带球带球带球带球带球带球带球带球带球带球带球带球带球带球
                    //		float			vset=(14000-abs(angle_add_transfer2(Path_LA,0))*80)*0.5;
                    ///**/		Path_LA=180.0*JOY_XposF; //cs2013test
                    test_joy_max_jifen = 15000; // 5000;//9000;
                    float temp_max_jifen_zoom = 1.0;

                    if ((xingwei_Passing_flag == 1) && (xingwei_Passing_2direct_pass == 1)) // 强突用到中线减速
                    { //	temp_max_jifen_zoom=(900-y.benjix)*0.01;
                      //	if (temp_max_jifen_zoom>1)
                      //		temp_max_jifen_zoom=1;
                      //	if (temp_max_jifen_zoom<0)
                      //		temp_max_jifen_zoom=0;
                      //	test_joy_max_jifen=test_joy_max_jifen*(temp_max_jifen_zoom*0.6+0.4);
                    }
                    JOY_ZposF = 1.0; //
                    //		float			vset=(test_joy_max_jifen*JOY_ZposF-abs(angle_add_transfer2(Path_LA,0))*80);
                    ////cs2013test
                    float vset =
                        (test_joy_max_jifen * JOY_ZposF - abs(angle_add_transfer2(Path_LA, 0)) * 80); // cs2013test
                    if (vset < 300)
                        vset = 300;
                    //				if (vset<1000)//TEST
                    //					vset=1000;
                    if (jifen < vset)
                        jifen += 150; //(200+50);
                    if (jifen > vset)
                        jifen -= 230; //(250+90);
                    if (jifen < 100)
                        jifen = 100;
                    //		jifen=4000;//TEST
                    test_joy_max_zhuan = 950;
                    if (test_joy_max_zhuan > 4000000 / jifen) //(R=v*360/w/2pi  R=n*v/w)
                        test_joy_max_zhuan = 4000000 / jifen;
                    if (test_joy_max_zhuan < 400)
                        test_joy_max_zhuan = 400;

                    //				test_joy_max_zhuan=60;//1100;//1190;//1390;//1500;//800;//1000;
                    if (1) // fabs(Path_LA)<140)
                    {
                        zhuan = 3 * duizhun1(-Path_LA, 0, test_joy_max_zhuan); // 1500);

                        if (abs(zhuan) > abs(zhuan_shang))
                        {
                            if (zhuan - zhuan_shang > 120)  // 150)//120)
                                zhuan = zhuan_shang + 120;  // 150;//120;
                            if (zhuan - zhuan_shang < -120) // 150)//120)
                                zhuan = zhuan_shang - 120;  // 150;//120;
                        }
                        if (abs(zhuan) < abs(zhuan_shang))
                        {
                            if (zhuan - zhuan_shang > 160)  // 300)//400)
                                zhuan = zhuan_shang + 160;  // 300;//400;
                            if (zhuan - zhuan_shang < -160) // 300)//400)
                                zhuan = zhuan_shang - 160;  // 300;//400;
                        }
                        //		Controlmoto(-90,-1.0*zhuan*fabs(Path_LA)/50,20000,&mtl,&mtr,&mtb);
                        //		Controlmoto(-Path_LA*(1-fabs(Path_LA)/50),jifen,20000,&mtl,&mtr,&mtb);

                        //	Controlmoto(-90,-1.8*zhuan,20000,&mtl,&mtr,&mtb);
                        if (1) // abs(Path_LA)>20)
                        {
                            Controlmoto(-90, -1.2 * zhuan, 25000, &mtl, &mtr, &mtb);
                            Controlmoto(0, jifen, 25000, &mtl, &mtr, &mtb);
                        }
                        else
                        {
                            Controlmoto(0, jifen, 25000, &mtl, &mtr, &mtb);
                        }
                        //	Controlmoto(0,0,20000,&mtl,&mtr,&mtb);
                    }
                    else
                    {
                        zhuan = 2 * duizhun1(-Path_LA, 0, test_joy_max_zhuan); // 1500);

                        if (abs(zhuan) > abs(zhuan_shang))
                        {
                            if (zhuan - zhuan_shang > 200)
                                zhuan = zhuan_shang + 200;
                            if (zhuan - zhuan_shang < -200)
                                zhuan = zhuan_shang - 200;
                        }
                        Controlmoto(-90, -1.8 * zhuan, 20000, &mtl, &mtr, &mtb);
                        //	Controlmoto(-90,-1.8*zhuan,20000,&mtl,&mtr,&mtb);
                        Controlmoto(0, jifen, 20000, &mtl, &mtr, &mtb);
                    }
                    //				float zhuanT=-1.1*zhuan;
                    //				if (zhuanT>jifen*0.1)
                    //					zhuanT=jifen*0.1;
                    //				Controlmoto(-90,zhuanT,20000,&mtl,&mtr,&mtb);
                    //				Controlmoto(0,jifen,20000,&mtl,&mtr,&mtb);
                    /*			if (jifen<abs(zhuan)*0.89)
                                    jifen=abs(zhuan)*0.89;
                            Controlmoto(0,jifen,12000,&mtl,&mtr,&mtb);			//2013cstest
                //			Controlmoto(0,abs(zhuan)*1.59,15000,&mtl,&mtr,&mtb);//2013cstest
                            Controlmoto(-90,-1.3*zhuan,15000,&mtl,&mtr,&mtb);	//2013cstest
                    */
                    double W = zhuan;      // test_joy_max_zhuan*JOY_XposF;//3200*JOY_XposF;//
                    double maxv = 6100;    // test_joy_max_jifen;//14000;
                    double maxturn = 3000; // test_joy_max_zhuan;//1500;
                    double minR = 50;      // 60;//35;//在最大转动速度下 球预计移动半径 30cm
                    double P = minR * maxturn / maxv;
                    double R = 100000;
                    if (fabs(W) > 0)
                        R = fabs(maxv / W * P);
                    double angle_d_ball = 80; // 45;//30带球偏角 图[带球解算] 角b
                    if (minR / R < 1)
                        angle_d_ball = angle_d_ball * minR / R;
                    angle_d_ball = angle_d_ball * 3.14 / 180.0;
                    double angle_move =
                        atan2(R * cos(angle_d_ball), R * sin(angle_d_ball) + 30); // 机器运动中心到球心 30cm
                    double fv = maxv * sin(angle_move);                           // 500  maxv  1000
                    double tv = maxv * cos(angle_move);                           // 860  zhuan=2090  R=33cm
                    double lixin = fabs(W * 0.8);                                 // 1.2 1.4 catch_zoom);//
                    fv = fv + lixin * cos(angle_move);
                    tv = tv - lixin * sin(angle_move);
                    if (W > 0)
                        tv = -tv;

                    if (1) // pinqiangpd>600)//认为原来的带球方式 适合抢球
                    {
                        ;
                    }
                    else
                    {
                        if (jifen < fv)
                            jifen = fv;
                        mtl = 0;
                        mtr = 0;
                        mtb = 0;
                        Controlmoto(0, jifen, 15000, &mtl, &mtr, &mtb); //(0,jifen,12000,&mtl,&mtr,&mtb);
                        Controlmoto(-90, tv, 15000, &mtl, &mtr, &mtb);  //(-90,tv,15000,&mtl,&mtr,&mtb);
                        //			Controlmoto(0,fv,18000,&mtl,&mtr,&mtb);		//2013cstest
                        //			Controlmoto(-90,tv,18000,&mtl,&mtr,&mtb);	//2013cstest
                        //			zhuan=W;									//2013cstest
                    }

                    if (jifen > 3000)
                    {
                        motospeed[3] = -4000;
                        motospeed[4] = 4000;
                    }
                    motospeed[3] += abs(zhuan * 3);
                    motospeed[4] -= abs(zhuan * 3);

                    float RMfv_avg = 0;
                    for (i = 0; i < 10; i++)
                    {
                        RMfv_avg += RMfv_h[i];
                    }
                    RMfv_avg = RMfv_avg / 10.0;

                    //		if(y.ball_chang<55)//cs
                    //		{
                    //	float sudu=jifen;
                    //	float Csudu=0;
                    //	Csudu=sudu*0.80-600-abs(zhuan*3.8);
                    //	if (Csudu>6900)
                    //		Csudu=6900;
                    //	motospeed[3]=-Csudu;//jifen*1.5+2500;
                    //	motospeed[4]= Csudu;

                    float sudu = jifen;
                    float Csudu = 0;
                    //	Csudu=sudu*0.60-1000-abs(zhuan*5);
                    float jiaodu = angle_add_transfer2(Dribble_ball.RMfa, 0);
                    //	if (fabs(jiaodu)<90)
                    //		jiaodu=jiaodu*(1-cos(jiaodu*3.14/180.0));

                    float motorRV = DebugV[5] = RMfv_avg * cos(jiaodu * 3.14 / 180.0) * 3600 * 0.5; // 5100;
                    //		Csudu=motorRV*1.0-abs((zhuan,2.0)*100);//motorRV*1.0-abs((zhuan,1.7)*3);
                    catch_3 = 1.0;    // 1.5;//1.3;//1.1;//p3
                    catch_zoom = 0.6; // 0.8;//1.1;//1.5;//p2
                    catch_dif = 2.3;  // 1.9;
                    double pow_zhuan_temp = 0;
                    if (zhuan >= 0)
                        pow_zhuan_temp = pow(zhuan, catch_3);
                    else
                        pow_zhuan_temp = -pow(-zhuan, catch_3);

                    Csudu = motorRV * 1.0 -
                            fabs(pow_zhuan_temp *
                                 catch_zoom); // 0.6);//abs(pow(zhuan,2.0)*100);//motorRV*1.0-abs((zhuan,1.7)*3);
                    if (Csudu > 6900)
                        Csudu = 6900;
                    if (Csudu < -15000)
                        Csudu = -15000;
                    //		if (Csudu<0)
                    //			Csudu=Csudu*10;
                    //		if (abs(zhuan)>1150)
                    //			Csudu-=(abs(zhuan)-1150)*test_joy_max_jifen/100 ;
                    DebugV[6] = Csudu;
                    motospeed[3] = -Csudu + catch_dif * (float)zhuan; // jifen*1.5+2500;
                    motospeed[4] = Csudu + catch_dif * (float)zhuan;

                    //		}
                }
            }
        }

        float RMfv_avg = 0;
        for (i = 0; i < 10; i++)
        {
            RMfv_avg += RMfv_h[i];
        }
        RMfv_avg = RMfv_avg / 10.0;

        if (chiqiupd < 6) // 18)
        {                 // motor 1000*DIANJI 1r 0.68s
            // 3.14*107.22mm*/680.0 *cos(30)
            //	float sudu=jifen;
            float Csudu = jifen * 1.1; //-1000
            float motorRV = DebugV[5] = RMfv_avg * cos(angle_add_transfer2(Dribble_ball.RMfa, 0) * 3.14 / 180.0) * 5100;
            Csudu = motorRV * 0.8;
            if (Csudu > 16000)
                Csudu = 16000;
            motospeed[3] = -Csudu; // jifen*1.5+2500;
            motospeed[4] = Csudu;
        }

        //	Controlmoto(0,2000,12000,&mtl,&mtr,&mtb);
        /*
            if(chiqiupd<12)
            {
                motospeed[3]=8000;
                motospeed[4]=-8000;
            }
            if(chiqiupd>5 &&  jifen<5000)
            {
                motospeed[3]=2000;
                motospeed[4]=-2000;
            }
            if(chiqiupd5 &&  jifen>5000)
            {
                motospeed[3]=-2000;
                motospeed[4]=2000;
            }
            if(chiqiupd>5 &&  jifen>8000)
            {
                motospeed[3]=-4000;
                motospeed[4]=4000;
            }
            if(chiqiupd>5 &&  jifen>10000)
            {
                motospeed[3]=-6000;
                motospeed[4]=6000;
            }
            if(chiqiupd<=2 || shemenleijia>0)
            {
                motospeed[3]=8000;
                motospeed[4]=-8000;
            }
            if(m_pAD[1]<2200 || m_pAD[0]<2200)
            {
                motospeed[3]=10000;
                motospeed[4]=-10000;
            }
            */

        double dwsudu;
        gankx[0] = y.benjix;
        ganky[0] = y.benjiy;

        dwsudu = (gankx[20] - y.benjix) * (gankx[20] - y.benjix) + (ganky[20] - y.benjiy) * (ganky[20] - y.benjiy);
        for (i = 20; i > 0; i--)
        {
            gankx[i] = gankx[i - 1];
            ganky[i] = ganky[i - 1];
        }
        if (jifen > 3000 && y.ball_chang < 100 && dwsudu < 900)
        {
            gankjs++;
        }
        else
        {
            gankjs = gankjs - 2;
        }
        if (gankjs < 0)
            gankjs = 0;
        if (gankjs > 50 && ganksx == 0)
        {
            ganksx = 500;
        }
        if (ganksx > 0)
        {
            gankjs = 29; // 准备好下周期0.5s 立刻进入继续抢球 0.5s防止误判
            mtl = 0;
            mtr = 0;
            mtb = 0;
            zhuan = 0;
            gankzt = 1;
            ganksx--;
            if ((y.ball_chang > 60 || m_pAD[0] < 2000 || m_pAD[1] < 2000) && gankpq == 0) // 拼抢中球进吸球器判定
            {
                gankpq = 4; //				球进吸球器再顶4周期
            }
            if (gankpq > 0)
            { // 向球上顶动作  球进吸球器再顶4周期
                gankpq--;
                zhuan = 2 * duizhun1(y.ball_jiao, 0, 1500);
                Controlmoto(y.ball_jiao, 4000, 15000, &mtl, &mtr, &mtb); // 加强 4000->6000
            }
            else
            { // 拔球时序
                if (ganksx > 39)
                    ganksx = 39;
                if (ganksx > 25) // 39~25 向后拔 1/4 s
                {
                    Controlmoto(180, 9000, 15000, &mtl, &mtr, &mtb);
                    zhuan = 0;
                }
                if (ganksx > 10) // 19~0 向旁拐 1/4 s
                {                //	Controlmoto(0,6000,15000,&mtl,&mtr,&mtb);
                                 //	zhuan=6000;
                    if (pinqiang_L_pd < pinqiang_R_pd)
                    {
                        mtl = -6000;
                        mtr = -13000;
                        mtb = -13000;
                    } // 左小于右 向左突
                    else
                    {
                        mtl = 13000;
                        mtr = 6000;
                        mtb = 13000;
                    } // 右小于左 向右突
                    zhuan = 0;
                }
                if (ganksx > 0) // 19~0 向
                {
                    Controlmoto(0, 6900, 15000, &mtl, &mtr, &mtb);
                    zhuan = 0;
                }
                if (y.ball_chang < 60 && (m_pAD[0] > 2000 || m_pAD[1] > 2000) && pinqiangpd < 600) // 一旦前方无障碍前冲
                {
                    ganksx -= 2;
                    jifen = 6900;
                    Controlmoto(0, jifen, 15000, &mtl, &mtr, &mtb);
                    zhuan = 0;
                }
            }
        }
        else
        {
            gankzt = 0;
        }

        //	if(y.ball_chang<55)
        //	{
        //		motospeed[3]=-jifen*1.5+2500;
        //		motospeed[4]=jifen*1.5-2500;
        //	}
        if (gankzt == 1)
        {
            motospeed[3] = 15000;
            motospeed[4] = -15000;
        }
        if (xqpanding >= 0)
            xqjishu = 20;
        if (chiqiupd < 6 && xqjishu > 1)
        {
            xqjishu--;
            xqpanding = -200;
            motospeed[3] = 15000;
            motospeed[4] = -15000;
        }
        xqpanding++;
        //	if(chiqiupd>=6  && abs(Path_LA)>50 && jifen>5000)  //比赛9
        //	{
        //		if(zhuan>0)
        //			motospeed[3]=-20000;
        //		else
        //			motospeed[4]=20000;
        //	}

        /*	if(chiqiupd>=3  && abs(Path_LA)>30 && jifen>3000)	//比赛10	//带球拐弯
            {
                if(zhuan<0)
                    motospeed[3]=20000;
                else
                    motospeed[4]=-20000;
            }
        */
        if (pinqiangpd > 600)
        {
            motospeed[3] = 15000;
            motospeed[4] = -15000;
        }
        if (gankjs > 6)
        {
            motospeed[3] = 15000;
            motospeed[4] = -15000;
        }
        if (0) // kick_fouce==1)	//待测试
        {
            motospeed[3] = 9000;
            motospeed[4] = -9000;
        }

        // 强制冲到中线附近传球 给两机强突用
        //	if ((xingwei_Passing_flag==1)&&(xingwei_Passing_2direct_pass==1)
        //		&&  (abs(kaiqiujiao)<18))
        //	{
        //		motospeed[3]=-15000;
        //		motospeed[4]= 15000;
        //	}
        //	Debug_D[20].Draw_vp_CV		(chiqiupd,240,100   ,1,128,128,128, "chiqiupd");//y:  x轴(既：t时间)高度
        // y_add:y偏置量   Value:数值值域 	Sleep(50);

        //	if(m_pAD[1]<2200 || m_pAD[0]<2200)
        //	{
        //		motospeed[3]=10000;
        //		motospeed[4]=-10000;
        //	}
        // 	mtl=0;
        // 	mtr=0;
        // 	mtb=0;
        // 	zhuan=0;
        // Controlmoto(45,500,20000,&mtl,&mtr,&mtb);
        // 		motospeed[3]=2000;
        //		motospeed[4]=-2000;//
        //	Controlmoto(-aaaa,2000,3000,&mtl,&mtr,&mtb);
        //	zhuan=4*duizhun(y.ball_jiao,0,500);
        catch_dif = -0.3; //-0.25;//-0.3;//
        /*
        if (m_pAD[0]>2000 && m_pAD[1]>2000)
        {	motospeed[3]=-jifen*0.5+catch_dif*(float)zhuan;//jifen*1.5+2500;
            motospeed[4]= jifen*0.5+catch_dif*(float)zhuan;
        }
        else
        {	motospeed[3]=-jifen*0.25+catch_dif*(float)zhuan;//jifen*1.5+2500;
            motospeed[4]= jifen*0.25+catch_dif*(float)zhuan;
        }
        */

        if (m_pAD[0] > 2000) // && m_pAD[1]>2000)
        {
            motospeed[4] = jifen * 0.2 + catch_dif * (float)zhuan + 1000;
        }
        else
        {
            motospeed[4] = jifen * 0.1 + 1000; //+catch_dif*(float)zhuan;
        }
        if (m_pAD[1] > 2000)
        {
            motospeed[3] = -jifen * 0.2 + catch_dif * (float)zhuan - 1000; // jifen*1.5+2500;
        }
        else
        {
            motospeed[3] = -jifen * 0.1 - 1000; //+catch_dif*(float)zhuan;//jifen*1.5+2500;
        }

        if (0) //(chiqiupd>6)//18)
        {      // 测试 横向带球参数
            float front_a, front_v, turn_w;
            Dribble_ball.O_movement_T_analyze(&front_a, &front_v, &turn_w, motol_as, motob_as,
                                              motor_as); //    *move_v_angle, *move_v_speed, *turn_v
            mtl = 0;
            mtr = 0;
            mtb = 0;
            Controlmoto(-90, 4000, 20000, &mtl, &mtr, &mtb);
            zhuan = 0;

            float a = -sin(front_a * 3.14 / 180.0) * front_v;
            if (a > 0)
            {
                float a1 = a * 0.3;  ////测试 横向带球参数1
                float a20 = a1 * 15; ////测试 横向带球参数2
                motospeed[3] = -(int)a1;
                motospeed[4] = -(int)a20;
            }
            else
            {
                float a1 = a * 0.3;  // 测试 横向带球参数
                float a20 = a1 * 15; // 测试 横向带球参数
                motospeed[3] = -(int)a20;
                motospeed[4] = -(int)a1;
            }
        }
        if (0) //(chiqiupd>6)//18)
        {      // 测试 前向带球参数
            float front_a, front_v, turn_w;
            Dribble_ball.O_movement_T_analyze(&front_a, &front_v, &turn_w, motol_as, motob_as,
                                              motor_as); //    *move_v_angle, *move_v_speed, *turn_v
            mtl = 0;
            mtr = 0;
            mtb = 0;
            Controlmoto(0, 1000, 20000, &mtl, &mtr, &mtb);
            zhuan = 0;

            float a = -sin(front_a * 3.14 / 180.0) * front_v;
            if (a > 0)
            {
                float a1 = a * 0.3;
                float a20 = a1 * 15;
                motospeed[3] = -(int)a1;
                motospeed[4] = -(int)a20;
            }
            else
            {
                float a1 = a * 0.3;
                float a20 = a1 * 15;
                motospeed[3] = -(int)a20;
                motospeed[4] = -(int)a1;
            }

            motospeed[3] += (-abs(cos(front_a * 3.14 / 180.0) * front_v) * 0.5); ////测试 前向带球参数
            motospeed[4] += (abs(cos(front_a * 3.14 / 180.0) * front_v) * 0.5);  ////测试 前向带球参数
        }
        if (chiqiupd > 6) // 18)
        {
            float front_a, front_v, turn_w;
            Dribble_ball.O_movement_T_analyze(&front_a, &front_v, &turn_w, motol_as, motob_as,
                                              motor_as); //    *move_v_angle, *move_v_speed, *turn_v
                                                         //	mtl=0;mtr=0;mtb=0;						//测试 旋转带球参数
                                                         //	Controlmoto(0,0,20000,&mtl,&mtr,&mtb);	//测试 旋转带球参数
                                                         //	zhuan=500;								//测试 旋转带球参数
                                                         /*
                                                             float a=-turn_w*1.1 -sin(front_a*3.14/180.0)*front_v;////测试 旋转带球参数1.1
                                                             if (a>0)
                                                             {
                                                                 float a1=a*0.3;
                                                                 float a20=a1*20;
                                                                 motospeed[3]=-(int)a1;
                                                                 motospeed[4]=-(int)a20;
                                                             }
                                                             else
                                                             {	float a1=a*0.3;
                                                             float a20=a1*20;
                                                             motospeed[3]=-(int)a20;
                                                             motospeed[4]=-(int)a1;
                                             
                                                             }
                                                             motospeed[3]+=(-abs(cos(front_a*3.14/180.0)*front_v)*0.5)+2500;//+catch_dif*(float)zhuan;//jifen*1.5+2500;
                                                             motospeed[4]+=(
                                                            abs(cos(front_a*3.14/180.0)*front_v)*0.5)-2500;//+catch_dif*(float)zhuan;//jifen*1.5+2500;
                                                             */
            /*
            if (m_pAD[0]>2000)// && m_pAD[1]>2000)
            {	motospeed[4]= jifen*0.2+catch_dif*(float)zhuan+1000;
            }
            else
            {	motospeed[4]= jifen*0.1+1000;//+catch_dif*(float)zhuan;
            }
            if ( m_pAD[1]>2000)
            {	motospeed[3]=-jifen*0.2+catch_dif*(float)zhuan-1000;//jifen*1.5+2500;
            }
            else
            {	motospeed[3]=-jifen*0.1-1000;//+catch_dif*(float)zhuan;//jifen*1.5+2500;
            }
            */

            motospeed[3] = -jifen * 0.2 + 800; //+catch_dif*(float)zhuan;//jifen*1.5+2500;

            motospeed[4] = jifen * 0.2 - 800;
            if (jifen < 4000 && abs(zhuan) > 100)
            {
                motospeed[3] = 4000; //+catch_dif*(float)zhuan;//jifen*1.5+2500;
                motospeed[4] = -4000;
            }
        }

        // cs2013test
#ifdef RUN_WITH_JOY
        if (JOY_Button[10] || JOY_Button[11])
        {
            xingwei = 11;
        }
        if (JOY_Button[0])
            test_joy_max_zhuan += 10;
        if (JOY_Button[1])
            test_joy_max_zhuan -= 10;
        if (test_joy_max_zhuan < 0)
            test_joy_max_zhuan = 0;

        if (JOY_Button[3])
            test_joy_max_jifen += 100;
        if (JOY_Button[2])
            test_joy_max_jifen -= 100;
        if (test_joy_max_jifen < 0)
            test_joy_max_jifen = 0;

        if (JOY_Button[9])
            catch_dif += 0.1;
        if (JOY_Button[8])
            catch_dif -= 0.1;

        if (JOY_Button[5])
            catch_zoom += 0.1;
        if (JOY_Button[7])
            catch_zoom -= 0.1;

        if (JOY_Button[4])
            catch_3 += 0.1;
        if (JOY_Button[6])
            catch_3 -= 0.1;
#endif
    }
    if (shoot_cool_time > 0) //
        shoot_cool_time--;

    if (0) // 测试
    {
        if (m_GKRecv.m_BallFly.nSeen == 1)
        {
            dbx_s = m_GKRecv.m_BallFly.fBallx;
            dby_s = m_GKRecv.m_BallFly.fBally;
            double balljiao;
            double ballchang;
            double ballx;
            double bally;
            double ball_x;
            double ball_y;
            double ball_z;
            ball_x = m_GKRecv.m_BallFly.fBallx * 100;
            ball_y = m_GKRecv.m_BallFly.fBally * 100;
            ball_z = m_GKRecv.m_BallFly.fBallz * 100;
            //	ball_y=ball_y-(ball_z-120)*sin(20/57.29578)+30;
            ball_y = sin(70 / 57.29578) * sqrt((ball_z - 120) * (ball_z - 120) + ball_y * ball_y) - 20;

            m_find.jizuobiao(ball_x, ball_y, &balljiao, &ballchang, 0, 0);
            //	ballchang=ballchang/cdbl*2.7;
            balljiao = -jiajiao(-balljiao, y.benjijiao - 90);
            strShow.Format("%5.1f", balljiao);
            m_find.zhizuobiao(&ballx, &bally, balljiao, ballchang, y.benjix, y.benjiy);
            //			m_find.zhizuobiao(&ballx,&bally,balljiao,ballchang,0,600);
            m_find.pField->SetBall(8, ballx, bally);
            y.quanbx = ballx;
            y.quanby = bally;
            //	y.quanbx=ballx;
            //	y.quanby=bally;

            //	if(leijiaqi%20==0)
            //	strShow.Format("%5.1f,%5.1f本机:%5.1f,%5.1f深度相机:%5.1f,%5.1f",ballx,bally,y.benjix,y.benjiy,m_GKRecv.m_BallFly.fBallx,m_GKRecv.m_BallFly.fBally);
        }
    }
    if (leijiayc > 0)
        leijiayc--;
    if (puqiulj > 0)
        puqiulj--;
    if (xingwei != 2)
    {
        for (i = 0; i < 1000; i++)
        {
            bxyc[i] = 0;
            byyc[i] = 0;
        }
    }
    if (xingwei == 2)
    {
        double jiao = 0;
        double chang = 0;
        double jiaoj;
        double changj;
        double xx;
        double yy;
        double jiaox1;
        double changx1;
        RGBTRIPLE yanse;
        yanse.rgbtBlue = 0;
        yanse.rgbtGreen = 255;
        yanse.rgbtRed = 0;
        bjjyc += yczhuan;

        // 球坐标历史
        if (y.ball_zhong <= 10)
        {
            net.rec_CDGtargetx = y.quanbx;
            net.rec_CDGtargety = y.quanby;
        }
        else
        {
            //		ballzdj++;
        }
        if (m_GKRecv.m_BallFly.nSeen == 1 &&
            (fabs(dbx_s - m_GKRecv.m_BallFly.fBallx) > 0.05 ||
             fabs(dby_s - m_GKRecv.m_BallFly.fBally) >
                 0.05)) // m_GKRecv.m_BallFly.fBallx>-18 && m_GKRecv.m_BallFly.fBally>-18
                        // &&m_GKRecv.m_BallFly.fBallz>-18)
        {
            //		ballzdj=0;
            smjishu++;
            dbx_s = m_GKRecv.m_BallFly.fBallx;
            dby_s = m_GKRecv.m_BallFly.fBally;
            double balljiao;
            double ballchang;
            double ballx;
            double bally;
            double ball_x;
            double ball_y;
            double ball_z;
            ball_x = m_GKRecv.m_BallFly.fBallx * 100;
            ball_y = m_GKRecv.m_BallFly.fBally * 100;
            ball_z = m_GKRecv.m_BallFly.fBallz * 100;
            //	ball_y=ball_y-(ball_z-120)*sin(20/57.29578)+30;
            ball_y = sin(70 / 57.29578) * sqrt((ball_z - 120) * (ball_z - 120) + ball_y * ball_y) - 20;

            m_find.jizuobiao(ball_x, ball_y, &balljiao, &ballchang, 0, 0);
            ballchang = ballchang / cdbl * 2.7;
            balljiao = -jiajiao(-balljiao, y.benjijiao - 90);
            m_find.zhizuobiao(&ballx, &bally, balljiao, ballchang, y.benjix, y.benjiy);
            //			m_find.zhizuobiao(&ballx,&bally,balljiao,ballchang,0,600);
            m_find.pField->SetBall(3, ballx, bally);

            //			if( ballzdj>5)
            //			{}
            //			else
            //			{
            y.quanbx = ballx;
            y.quanby = bally;
            //			}
            strShow.Format("%d", smjishu);
            //	if(leijiaqi%20==0)
            //			strShow.Format("%5.1f,%5.1f本机:%5.1f,%5.1f深度相机:%5.1f,%5.1f",y.quanbx,y.quanby,y.benjix,y.benjiy,m_GKRecv.m_BallFly.fBallx,m_GKRecv.m_BallFly.fBally);

            // 球历史记录

            for (int i = 50; i >= 0; i--)
            {
                bxyc[i + 1] = bxyc[i];
                byyc[i + 1] = byyc[i];
            }
            bxyc[0] = y.quanbx;
            byyc[0] = y.quanby;

            // 球行进方向
            jiao = 0;
            chang = 0;
            j = 0;

            for (i = 0; i < 1; i++)
            {
                if (fabs(bxyc[0]) > 0.1 && fabs(bxyc[i + 6]) > 0.1)
                {
                    m_find.jizuobiao(bxyc[i], byyc[i], &jiaoj, &changj, bxyc[i + 4], byyc[i + 4]);
                    jiao += jiaoj;
                    chang += changj;
                    j++;
                }
            }
            if (j > 0)
            {
                jiao = jiao / j;
                chang = chang / j;
            }
            else
            {
                chang = 0;
            }
            // 显示前进方向
            /*
            for(i=0;i<chang*3;i++)
            {
                m_find.zhizuobiao(&xx,&yy,jiao,i,y.ball_x,y.ball_y);
                m_find.SetColor(xx,yy,yanse);
                m_find.SetColor(xx+1,yy,yanse);
                m_find.SetColor(xx,yy+1,yanse);
                m_find.SetColor(xx-1,yy,yanse);
                m_find.SetColor(xx,yy-1,yanse);
            }
            */
            int n = 0;
            int xx1;
            int yy1 = 600;
            int jiaoxx;
            // 球与门线交点
            for (i = 0; i < 12000; i++)
            {
                m_find.zhizuobiao(&xx, &yy, jiao, i, bxyc[0], byyc[0]);
                if (xx < 33 && xx > 27 && yy > 470 && yy < 730)
                {

                    if (yy > 655)
                        yy = 655;
                    if (yy < 545)
                        yy = 545;

                    xx1 = xx;
                    yy1 = yy;
                    n = 1;
                    break;
                }
                else
                {
                    n = 0;
                }
            }

            for (i = 90; i >= 0; i--)
            {
                smycy[i + 1] = smycy[i];
            }
            smycy[0] = chang;

            int fangxpd = 0;
            int akuhysd;
            for (i = 0; i < 5; i++)
                for (j = 0; j < 5; j++)
                {
                    akuhysd = fabs(smycy[i] - smycy[j]);
                    if (akuhysd > 100)
                        akuhysd = 100;
                    fangxpd = fangxpd + akuhysd;
                }
            //	strShow.Format("11:%d,%f",fangxpd,chang);
            // 有交点并且速度大于n

            //		strShow.Format("%d,%f",fangxpd,chang);

            if (n == 1 && chang > 80 && chang < 800 && leijiayc < 170) //&& fangxpd<400 )
            {
                //	strShow.Format("11:%d,%f",fangxpd,chang);
                leijiayc = 180;
                for (i = 0; i < 90; i++)
                {
                    smycy[i] = 100000;
                }

                for (i = 0; i < 1000; i++)
                {
                    bxyc[i] = 0;
                    byyc[i] = 0;
                }
                if (puqiulj < 50)
                {
                    pRobot->GKSave(0);
                    puqiulj = 280;
                }
                leijiaycx = xx1;
                leijiaycy = yy1;
                fangshoujiao = jiao;
                jiaoxx = jiao;
                m_find.pField->SetBall(3, leijiaycx, leijiaycy);
            }

            //	strShow.Format("%5.1f",chang);
            zhuan = 0;
            xx = 900;
            yy = 600;

            /*
                for(i=0;i<12000;i++)
                {
                    m_find.zhizuobiao(&xx,&yy,fangshoujiao,i,bxyc[0],byyc[0]);

                    if(xx<10 && xx>-10 && yy>400 && yy<800)
                    {
                        if(yy<500)
                            yy=500;
                        if(yy>700)
                            yy=700;


                    leijiaycx=xx;
                    leijiaycy=yy;
                        break;
                    }

                }
                */
        }

        // 如果交点成立
        if (leijiayc > 0)
        {
            //	leijiayc--;
            m_find.pField->SetBall(9, leijiaycx, leijiaycy);
        }
        else
        {

            double xx;
            xx = y.quanbx;
            if (xx < 200)
                xx = 200;
            leijiaycx = 20; // 修改让机器往前站点
            leijiaycy = (y.quanby - 600) / (xx / 20);
            leijiaycy += 600;
            if (leijiaycy > 630)
                leijiaycy = 630;
            if (leijiaycy < 570)
                leijiaycy = 570;
            // if(y.ball_chang>150)
            //	leijiaycy=-(leijiaycy-600)+600;

            //	leijiaycx=0;
            //	leijiaycy=600;
        }
        //	if((y.quanbx<900 && y.quanbx>-80) || leijiayc>0)

        if (y.quanbx < 220 && y.quanby > 410 && y.quanby < 790)
        {
            double YYY;
            double XXX;
            dingweijiao = 0;
            dingweichang = 0;
            zhuan = 0;
            YYY = leijiaycy;
            XXX = leijiaycx;
            if (y.quanbx < 220 && y.quanby > 410 && y.quanby < 790) // && leijiayc<20)
            {
                XXX = 35;
                YYY = y.quanby;
                if (YYY > 670)
                    YYY = 670;
                if (YYY < 530)
                    YYY = 530;
                zhuan = 3 * duizhun(-y.benjijiao, 0, 800);
            }
            else if (abs(y.ball_jiao) > 1)
                zhuan = 2 * duizhun(y.ball_jiao, 0, 700);
            m_find.jizuobiao(XXX, YYY, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
            dingweijiao = -jiajiao(-dingweijiao, y.benjijiao - 90);
            Controlmoto(dingweijiao, dingweichang * 50, 6000, &mtl, &mtr, &mtb);
        }
        else if (leijiayc > 0)
        {
            // leijiaycy=(leijiaycy-600)*0.5+600;
            leijiaycx = 30;
            //		if(y.benjiy>=650 || y.benjiy<550)
            //		{
            //			zhuan=2*duizhun(y.ddoor_jiao,0,800);
            //		}
            //		else
            //		{
            //	if(abs(y.ball_jiao)>15)
            zhuan = 0;

            //			zhuan=2*duizhun(-y.benjijiao,0,400);
            //		}

            m_find.jizuobiao(leijiaycx, leijiaycy, &jiaox1, &changx1, y.benjix, y.benjiy);
            jiaox1 = -jiajiao(-jiaox1, y.benjijiao - 90);
            changx1 = fanquxian[int(changx1 * 2.7 / cdbl)];

            m_find.jizuobiao(leijiaycx, leijiaycy, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
            dingweijiao = -jiajiao(-dingweijiao, y.benjijiao - 90);
            Controlmoto(dingweijiao, dingweichang * 100 + 500, 6000, &mtl, &mtr, &mtb);
        }
        /*	else if(y.ball_zhong>10 && y.quanbx>-10)
            {
                int xx;
                int yy;
                //if(y.quanbx<-10)


                            yanchangxian(y.jdoor_x,y.jdoor_y,y.ball_x,y.ball_y,-60,&xx,&yy);
                    m_find.jizuobiao(xx,yy,&jiao,&chang,320,240);
                    if(jiajiao(y.ball_jiao,y.jdoor_jiao)>0)
                        if(y.benjiy<700)
                            Controlmoto(jiao,chang*200,3000,&mtl,&mtr,&mtb);
                        else
                            Controlmoto(y.ddoor_jiao+90,0,2000,&mtl,&mtr,&mtb);
                    else
                    if( y.benjiy>500)
                            Controlmoto(jiao,chang*200,3000,&mtl,&mtr,&mtb);
                        else
                            Controlmoto(y.ddoor_jiao+90,0,2000,&mtl,&mtr,&mtb);
                    Controlmoto(y.jdoor_jiao,(-1+y.jdoor_chang)*80,6000,&mtl,&mtr,&mtb);
                    zhuan=3*duizhun(y.ball_jiao,0,800);
            }
            */
        else
        {
            double YYY;
            double XXX;
            dingweijiao = 0;
            dingweichang = 0;
            zhuan = 0;
            YYY = leijiaycy;
            XXX = leijiaycx;
            if (y.quanbx < 200 && y.quanby > 430 && y.quanby < 780 && leijiayc < 20)
            {
                XXX = 35;
                YYY = y.quanby;
                if (YYY > 670)
                    YYY = 670;
                if (YYY < 530)
                    YYY = 530;
                zhuan = 3 * duizhun(-y.benjijiao, 0, 800);
            }
            else if (abs(y.ball_jiao) > 1)
                zhuan = 2 * duizhun(y.ball_jiao, 0, 700);
            m_find.jizuobiao(XXX, YYY, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
            dingweijiao = -jiajiao(-dingweijiao, y.benjijiao - 90);
            Controlmoto(dingweijiao, dingweichang * 40, 4000, &mtl, &mtr, &mtb);
        }
        //	zhuan=3*duizhun(y.ball_jiao,0,800);
        if (y.ball_zhong > 10)
        {
            motospeed[3] = 16000;
            motospeed[4] = -16000;
        }
        else
        {
            motospeed[3] = 0;
            motospeed[4] = 0;
        }
        //	strShow.Format("%d,%5.1f本机:%5.1f,%5.1f深度相机:%5.1f,%5.1f",leijiayc,bally,y.benjix,y.benjiy,m_GKRecv.m_BallFly.fBallx,m_GKRecv.m_BallFly.fBally);
        //	strShow.Format("%d",leijiayc);
        motospeed[3] = 0;
        motospeed[4] = 0;
        mtl = mtl * 3;
        mtr = mtr * 3;
        mtb = mtb * 3;
    }
    if (xingwei == 22)
    {
        if (y.ball_chang < 55 && m_pAD[0] > 1500 && m_pAD[1] > 1500 && y.ball_zhong > 10)
        {
            Controlmoto(0, 600, 1000, &mtl, &mtr, &mtb);
            zhuan = duizhun(y.ddoor_jiao, 0, 500);
            Controlmoto(-90, -2 * zhuan, 10000, &mtl, &mtr, &mtb);
        }
        else
        {
            zhuan = duizhun1(y.ball_jiao, 0, 1500);
            Controlmoto(y.ball_jiao, 1000, 1500, &mtl, &mtr, &mtb);
        }

        if (y.ball_chang > 10 && y.ball_chang <= 50 && jiajiao(0, (int)y.ddoor_zuo) <= -3 &&
            jiajiao(0, (int)y.ddoor_you) >= 3 && abs(y.ball_jiao) < 50)
        {                                    // 满足机器不在规避，并且球长度在24内，门的相对角度小于5度，球的角度小于30
            shemenleijia = shemenleijia + 1; // 射门计数器开始工作
        }
        else
            shemenleijia = 0;
        if ((shemenleijia > 30 && valxx == 1))
        {
            shemenleijia = 0;
            pRobot->Kick(16000); // dmenchang/50*1000+0);
        }
        motospeed[3] = 2000;
        motospeed[4] = -2000;
    }
    if (xingwei == 15)
    {
        mtl = 0;
        mtr = 0;
        mtb = 0;
        zhuan = 500;
    }
    if (xingwei == 14) // 全场跑位找球
    {
        if (y.ball_zhong < 10)
        {
            int xx;
            int yy;
            double jiao;
            double chang;
            int zuo;
            int you;
            if (paowei == 0)
                m_find.jizuobiao(1600, 200, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
            if (paowei == 1)
                m_find.jizuobiao(1600, 1000, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
            if (paowei == 2)
                m_find.jizuobiao(300, 200, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
            if (paowei == 3)
                m_find.jizuobiao(300, 1000, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
            dingweijiao = -jiajiao(-dingweijiao, y.benjijiao - 90);

            paoweileiji++;
            m_find.guibiqiu(&y);                                              // 将球也放入规避区范围内
            m_find.getzhangaiwu(40, 100, 10, dingweijiao, 80, 0, &zuo, &you); // 生成路径（16（内径），
            // chang(外径),10(射线连续长度),jiao(目标角),50(规避区域),zuo（输出左射线）,you(输出右射线)）
            //	zhuan=duizhun(0,0,1500);
            if (gbxiang == 0)
            {
                if (abs(jiajiao(dingweijiao, zuo)) < abs(jiajiao(dingweijiao, you)))
                    gbxiang = 30;
                else
                    gbxiang = -30;
            }
            if (gbxiang > 0)
            {
                zhuan = duizhun(zuo, 180, 500);
                gbxiang--;
                if (dingweichang > 30)
                    Controlmoto(zuo, dingweichang * 10 + 1000, 4000, &mtl, &mtr, &mtb);
                else
                    Controlmoto(zuo, dingweichang * 1, 1000, &mtl, &mtr, &mtb);
            }
            else if (gbxiang < 0)
            {
                zhuan = duizhun(you, 170, 500);
                gbxiang++;
                if (dingweichang > 30)
                    Controlmoto(you, dingweichang * 10 + 1000, 4000, &mtl, &mtr, &mtb);
                else
                    Controlmoto(you, dingweichang * 1, 1000, &mtl, &mtr, &mtb);
            }
            if (dingweichang < 50 && paoweileiji > 10)
            {
                if (paowei < 4)
                    paowei++;
                else
                    paowei = 0;
                paoweileiji = 0;
            }
        }
        else
        {
            mtb = 0;
            mtr = 0;
            mtl = 0;
            zhuan = 0;
            xingwei = 1;
        }
    }
    if (xingwei != 6)
    {
        qishipanding1 = 0;
        tingzhipanding1 = 0;
    }
    if (xingwei != 13)
    {
        tingzhipanding = 0;
        qishipanding = 0;
    }
    if (xingwei != 10)
    {
        kaiqiuting = 0;
    }
    if (xingwei == 13)
    {
        Controlmoto(0, 0, 2000, &mtl, &mtr, &mtb);
        tingzhipanding++;
        if (tingzhipanding > 10)
        {
            if (qishipanding == 0)
            {
                qishipanding = 1;
                ballqx = y.ball_x;
                ballqy = y.ball_y;
                zhuan = 0; // 1,目标物角度。2,自己哪个角度对准目标物。3,速度上限
                // 1,需要平移的角度。2,速度。3,速度上限
            }
            if (abs(y.ball_x - ballqx) > 10 || abs(y.ball_y - ballqy) > 10)
            {
                xingwei = 1;
                net.toS_status = CTRL_ATTACK;
            }
        }
    }
    else
    {
        xing13 = 0;
    }
    if (0) //(xingwei==11)
    {
        zhuan = 0;
        mtl = 0;
        mtr = 0;
        mtb = 0;
    }
    int guibichang;
    if (xingwei == 24)
    {
        if (y.ball_chang > holdball + 10) // 如果球的距离太远
        {
            jiasudu1 = 0; // 加速度量清零
        }
        if ((y.ball_chang > holdball || abs(y.ball_jiao) > 40) || m_pAD[1] < 1000 ||
            m_pAD[0] < 1000) // 9.4) //如果球的距离在持球半径范围外
        {
            int xx;
            int yy;
            double jiao;
            double chang;
            int zuo;
            int you;
            if (y.ball_chang < 110)
                m_find.getzhangaiwu(40, y.ball_chang - 15, 10, y.ball_jiao, 80, 0, &zuo, &you);
            else
                m_find.getzhangaiwu(40, 100, 10, y.ball_jiao, 80, 0, &zuo, &you);
            //	m_find.getzhangaiwu(19,100,15,y.ball_jiao,80,0,&zuo,&you);
            if (gbxiang == 0)
            {
                if (abs(jiajiao(y.ball_jiao, zuo)) < abs(jiajiao(y.ball_jiao, you)))
                {
                    gbxiang = 10;
                    //					strShow.Format("追球左");
                }
                else
                {
                    gbxiang = -10;
                    //					strShow.Format("追球右");
                }
            }
            if (y.ball_chang > 80)
            {

                if (gbxiang > 0)
                {
                    gbxiang--;
                    zhuiqiu(zuo, y.ball_chang, chang_shang);
                }
                else if (gbxiang < 0)
                {
                    gbxiang++;
                    zhuiqiu(you, y.ball_chang, chang_shang);
                }
            }
            else
                zhuiqiu(y.ball_jiao, y.ball_chang, chang_shang);
        }
        else
        {

            int xx;
            int yy;
            double jiao;
            double chang;
            int zuo;
            int you;
            if (y.ddoor_chang - 40 < 100)
                guibichang = y.ddoor_chang - 40;
            else
                guibichang = 100;
            panding = m_find.getzhangaiwu(40, guibichang, 6, y.ddoor_jiao, 90, 1, &zuo, &you);
            /*		if(panding<10)
                        panding=m_find.getzhangaiwu(19,guibichang,10,y.ddoor_jiao,70,&zuo,&you);
                    if(panding<10)
                        panding=m_find.getzhangaiwu(19,guibichang,10,y.ddoor_jiao,60,&zuo,&you);
                    if(panding<10)
                        panding=m_find.getzhangaiwu(19,guibichang,10,y.ddoor_jiao,50,&zuo,&you);
                    if(panding<10)
                        panding=m_find.getzhangaiwu(19,guibichang,10,y.ddoor_jiao,40,&zuo,&you);
            */
            //	zhuan=duizhun(y.ball_jiao,0,1500);
            //	Controlmoto(jiao,(chang-3)*100,3000,&mtl,&mtr,&mtb);
            if (gbxiang == 0)
            {
                /*
                        if(y.benjix>1550 || y.benjiy>1000 || y.benjiy <200)
                        {
                            int zuo;
                            int you;
                            double jiao;
                            double chang;
                            m_find.jizuobiao(900,600,&dingweijiao,&dingweichang,y.benjix,y.benjiy);
                            dingweijiao=-jiajiao(-dingweijiao,y.benjijiao-90);
                            if(abs(jiajiao(dingweijiao,zuo))<abs(jiajiao(dingweijiao,you)))
                                gbxiang=30;
                            else
                                gbxiang=-30;
                        }

                    */
                //	else
                //	{
                if (abs(jiajiao(y.ddoor_jiao, zuo)) < abs(jiajiao(y.ddoor_jiao, you)))
                {
                    //				strShow.Format("左");
                    gbxiang = 60;
                }
                else
                {
                    gbxiang = -60;
                    //				strShow.Format("右");
                }
                //	}
            }
            //	if (gbxiang>0)
            //	{
            //		gbxiang--;
            daiqiu(qiansudu, y.ddoor_jiao);
            //	}
            //	else if (gbxiang<0)
            //	{
            //		daiqiu(qiansudu,y.ddoor_jiao);
            //		gbxiang++;
            //	}

            //	daiqiu(jiasudu1,qiansudu,y.ddoor_jiao);
            if (y.ball_chang > 10 && y.ball_chang <= 50 && jiajiao(0, (int)y.ddoor_zuo) <= -3 &&
                jiajiao(0, (int)y.ddoor_you) >= 3 &&
                abs(y.ball_jiao) <
                    50) // && y.ddoor_chang<90)// && y.ddoor_chang<100)// y.ddoor_chang<65)//y.ddoor_zhong>50)
            {           // 满足机器不在规避，并且球长度在24内，门的相对角度小于5度，球的角度小于30
                shemenleijia = shemenleijia + 1; // 射门计数器开始工作
            }
            else
                shemenleijia = 0;
            if (y.ball_chang > 10 && y.ball_chang <= 50 && jiajiao(0, (int)y.ddoor_zuo) <= 0 &&
                jiajiao(0, (int)y.ddoor_you) >= 0 && abs(y.ball_jiao) < 50)
            {
                shemenleijia = shemenleijia + 1;
            }
            if ((shemenleijia > 3 && valxx == 1))
            { // 如果射门计数器大于10则射门

                shemenleijia = 0;
                zhuangtai = "射门";
                jiasudu1 = 0;
                pRobot->Kick(shemenb[int(dmenchang / 2.4) - 50] + 1000);
                //	m_pCmd->SetFourMotorsSpeed(0,0,0,0);
                //	Sleep(10000);
                //	Sleep(1000);
                jifen = 0;
            }
        }
        zhuangtai = "进攻";

        //	motospeed[3]=10000;
    }
    if (xingwei != 1)
    {
        daduobi = 0;
        gbpanding = 50;
        if (jiasudu1 > 500)
            jiasudu1 -= 200;
        if (jifen > 500)
            jifen -= 200;
        if (qiansudu > 500)
            qiansudu -= 200;
        jifen = 0;
        gankpq = 0;
        gankzt = 0;
        ganksx = 0;
        gankjs = 0;
        xqpanding = 0;
        xqjishu = 0;
    }

    double suduxx1;
    if (xingwei == 23)
    {
        if ((y.ball_zhong > 10 || y.quanbx < 900) && y.quanbx > -10)
        {
            int xx;
            int yy;
            double jiao;
            double chang;
            int zuo;
            int you;

            //	zhuan=duizhun(jiao,0,1500);
            zhuan = duizhun(y.ball_jiao / 2, 0, 1500);
            if (y.ball_zhong > 10 && y.ball_chang <= holdball && abs(y.ball_jiao) < 15)
            {
                // m_pCmd->Kick(30,0);
                pRobot->Kick(0);
            }
            if (y.ball_chang < holdball + 5 && y.jdoor_chang < 70)
            {
                //	yanchangxian(y.ball_x,y.ball_y,y.jdoor_x,y.jdoor_y,-10,&xx,&yy);
                //	m_find.jizuobiao(xx,yy,&jiao,&chang,320,240);
                //	Controlmoto(jiao,chang*100,5000,&mtl,&mtr,&mtb);
                zhuiqiu(y.ball_jiao, y.ball_chang, chang_shang);
            }
            else
            {
                yanchangxian(y.jdoor_x, y.jdoor_y, y.ball_x, y.ball_y, -60, &xx, &yy);
                m_find.jizuobiao(xx, yy, &jiao, &chang, 320, 240);
                if (jiajiao(y.ball_jiao, y.jdoor_jiao) > 0)
                    if (y.benjiy < 700)
                        Controlmoto(jiao, chang * 200, 3000, &mtl, &mtr, &mtb);
                    else
                        Controlmoto(y.ddoor_jiao + 90, 0, 2000, &mtl, &mtr, &mtb);
                else if (y.benjiy > 500)
                    Controlmoto(jiao, chang * 200, 3000, &mtl, &mtr, &mtb);
                else
                    Controlmoto(y.ddoor_jiao + 90, 0, 2000, &mtl, &mtr, &mtb);
                Controlmoto(y.jdoor_jiao, (-1 + y.jdoor_chang) * 80, 6000, &mtl, &mtr, &mtb);
            }
            zhuangtai = "守门";
        }
        if (y.ball_zhong <= 10 || y.quanbx < -10)
        {
            zhuan = duizhun(y.ddoor_jiao, 0, 1000);
            Controlmoto(y.jdoor_jiao, (-1 + y.jdoor_chang) * 40, 1000, &mtl, &mtr, &mtb);
            if (jiajiao(y.ddoor_jiao, y.jdoor_jiao) > 0)
                Controlmoto(y.ddoor_jiao + 90, (180 - jiajiao(y.ddoor_jiao, y.jdoor_jiao)) * 50, 2000, &mtl, &mtr,
                            &mtb);
            else
                Controlmoto(y.ddoor_jiao + 90, (-180 - jiajiao(y.ddoor_jiao, y.jdoor_jiao)) * 50, 2000, &mtl, &mtr,
                            &mtb);
            zhuangtai = "回位置";
        }
        zhuangtai = "守门";
        /*
            int meny1=0;
            int youxiaodian=0;
            if(leijiaqi<=1)
            {
                xp=y.quanbx;
                xvp=0;
                yp=y.quanby;
                yvp=0;
            }
            else
            {
                if(y.ball_zhong>5)
                {

                Kalman(&xp,&xvp,y.quanbx,0.9,0.9);
                Kalman(&yp,&yvp,y.quanby,0.9,0.9);

                }
        //		else
        //		{
        //		Kalman(&xp,&xvp,xp,0.3,0.3);
        //		Kalman(&yp,&yvp,yp,0.3,0.3);
        //		}
            }
            xianshix[1]=xp;
            xianshiy[1]=yp;
            if(leijiaqi<=5)
            {
                ballx[leijiaqi-1]=xp;
                bally[leijiaqi-1]=yp;
            }
            else
            {
                for(int i=19;i>=1;i--)
                {
                    ballx[i]=ballx[i-1];
                    bally[i]=bally[i-1];
                }
                ballx[0]=xp;
                bally[0]=yp;

                for(i=0;i<=0;i++)
                {
                    suduxx1=sqrt(xvp*xvp+yvp*yvp);
                    if(suduxx1>300 && suduxx1<500000 && (ballx[i+1]-ballx[i])!=0)
                    {//某两帧见像素点的距离差如果大于n

                        youxiaodian++;
                        meny1+=bally[i]-(bally[i+1]-bally[i])*ballx[i]/(ballx[i+1]-ballx[i]);
                    }
                    if(youxiaodian>0 && abs(ballx[i+1]-ballx[i])+abs(bally[i+1]-bally[i])>30)
                        meny=meny1/youxiaodian;
                }
                CString str;
            //	str.Format("%f",xvp);
            //	AfxMessageBox(str);
                if(meny>700)
                    meny=700;
                if(meny<500)
                    meny=500;
                int xx;
                int yy;
                double jiao;
                double chang;
                m_find.jizuobiao(200,meny,&dingweijiao,&dingweichang,y.benjix,y.benjiy);
                dingweijiao=-jiajiao(-dingweijiao,y.benjijiao-90);
            //	zhuan=duizhun(y.ball_jiao,0,1000);
                zhuan=0;
                if(y.quanbx<1200)
                    Controlmoto(dingweijiao,dingweichang*40+0,3000,&mtl,&mtr,&mtb);
                else
                    Controlmoto(0,0,300,&mtl,&mtr,&mtb);
            }
            zhuangtai="守门";
        */
    }
    if (xingwei == 3)
    {

        m_find.jizuobiao(600, 600, &dingweijiao, &dingweichang, y.benjix, y.benjiy);

        dingweijiao = -jiajiao(-dingweijiao, y.benjijiao - 90);

        zhuan = 0;
        if (dingweichang > 100)
            Controlmoto(dingweijiao, dingweichang * 2 + 100, 1000, &mtl, &mtr, &mtb);
        else
            Controlmoto(dingweijiao, dingweichang * 1, 1000, &mtl, &mtr, &mtb);
    }
    if (xingwei == 4) // 己方开球 主开球员
    {
        int xx;
        int yy;
        double jiao;
        double chang;
        int zuo;
        int you;
        yanchangxian(y.ball_x, y.ball_y, y.ddoor_x, y.ddoor_y, 35, &xx,
                     &yy); // 取点ball_x,ball_y,到door_x,door_y,延长30个像素点长度后返回xx,yy
        m_find.jizuobiao(xx, yy, &jiao, &chang, 320, 240);           // 求xx,yy的极坐标
        m_find.guibiqiu(&y);                                         // 将球也放入规避区范围内
        m_find.getzhangaiwu(40, chang, 10, jiao, 50, 0, &zuo, &you); // 生成路径（16（内径），
        // chang(外径),10(射线连续长度),jiao(目标角),50(规避区域),zuo（输出左射线）,you(输出右射线)）
        zhuan = duizhun(y.ball_jiao, 0, 1500);
        if (gbxiang == 0)
        {
            if (abs(jiajiao(jiao, zuo)) < abs(jiajiao(jiao, you)))
                gbxiang = 30;
            else
                gbxiang = -30;
        }
        if (gbxiang > 0)
        {
            gbxiang--;
            Controlmoto(zuo, (chang - 3) * 60, 4000, &mtl, &mtr, &mtb);
        }
        else if (gbxiang < 0)
        {
            Controlmoto(you, (chang - 3) * 60, 4000, &mtl, &mtr, &mtb);
            gbxiang++;
        }
        zhuangtai = "主动开球跑位";
        jiasudu1 = 0;
    }
    if (xingwei == 5) // 己方防守跑位
    {
        int xx;
        int yy;
        double jiao;
        double chang;
        int zuo;
        int you;
        yanchangxian(y.ball_x, y.ball_y, y.jdoor_x, y.jdoor_y, -120, &xx, &yy);
        m_find.jizuobiao(xx, yy, &jiao, &chang, 320, 240);
        m_find.guibiqiu(&y);
        m_find.getzhangaiwu(40, chang, 10, jiao, 50, 0, &zuo, &you);
        zhuan = duizhun(y.ball_jiao, 0, 1500);
        //	Controlmoto(jiao,(chang-3)*100,3000,&mtl,&mtr,&mtb);
        if (gbxiang == 0)
        {
            if (abs(jiajiao(jiao, zuo)) < abs(jiajiao(jiao, you)))
                gbxiang = 30;
            else
                gbxiang = -30;
        }
        if (gbxiang > 0)
        {
            gbxiang--;
            Controlmoto(zuo, (chang - 3) * 100, 3000, &mtl, &mtr, &mtb);
        }
        else if (gbxiang < 0)
        {
            Controlmoto(you, (chang - 3) * 100, 3000, &mtl, &mtr, &mtb);
            gbxiang++;
        }
    }
    if (xingwei == 6) // 对方开球开始动作
    {
        Controlmoto(0, 0, 2000, &mtl, &mtr, &mtb);
        if (y.ball_zhong > 10)
        {
            tingzhipanding1++;
            if (tingzhipanding1 > 10)
            {
                if (qishipanding1 == 0)
                {
                    qishipanding1 = 1;
                    ballqx = y.quanbx;
                    ballqy = y.quanby;
                    ballqz = y.ball_zhong;
                    zhuan = 0; // 1,目标物角度。2,自己哪个角度对准目标物。3,速度上限
                    // 1,需要平移的角度。2,速度。3,速度上限
                }
                else
                {
                    if (y.ball_zhong > 10 && (abs(y.quanbx - ballqx) > 50 || abs(y.quanby - ballqy) > 50))
                    {
                        qishipanding1++;
                    }
                    else
                    {
                        if (qishipanding1 >= 1)
                            qishipanding1 == 1;
                    }

                    if (qishipanding1 > 4)
                    {
                        xingwei = 1;
                        net.toS_status = CTRL_ATTACK;
                    }
                    jifen = 2000;
                }
            }
        }
    }
    if (0) //(xingwei==7)//移动到指定点
    {
        // call_machine_location(double jiaoa,double ball_Rradius, double *tf_x, double *tf_y ,double fx,double fy
        // ,double fa, double br,double ba)
        /*	double tfx,tfy,tfa;
            call_machine_location(0           ,200                , &tfx, &tfy ,y.benjix ,y.benjiy
        ,-y.benjijiao,quxian[int(y.ball_chang)]*1.9201 ,-y.ball_jiao);//quxian[int(y.ball_chang)]*0.49/cdbl
            tfa=atan2(y.quanby-tfy,y.quanbx-tfx)*180/3.14;
            m_find.pField->SetAgent(6,tfx,tfy,tfa);
            m_find.pField->SetBall(6,tfx,tfy);
        net.rec_targetx=tfx;
        net.rec_targety=tfy;
        net.rec_targetang=tfa;
        */
        int xx;
        int yy;
        double jiao;
        double chang;
        int zuo;
        int you;
        double jiaox1;
        double changx1;
        int sudu;
        m_find.jizuobiao(net.rec_targetx, net.rec_targety, &jiaox1, &changx1, y.benjix, y.benjiy);
        jiaox1 = -jiajiao(-jiaox1, y.benjijiao - 90);
        changx1 = fanquxian[int(changx1 * 2.7 / cdbl)];
        if (changx1 > 60)
        {
            changx1 = 60;
        }

        m_find.jizuobiao(net.rec_targetx, net.rec_targety, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
        dingweijiao = -jiajiao(-dingweijiao, y.benjijiao - 90);
        //	zhuan=0;
        zhuan = -duizhun(jiajiao(net.rec_targetang, y.benjijiao), 0, 1000);
        //	strShow.Format("本机角:%10.5f,转动%d",y.benjijiao,zhuan);

        m_find.guibiqiu(&y);                                                            // 将球也放入规避区范围内
        m_find.getzhangaiwu(40, int(changx1) + 10, 10, dingweijiao, 80, 0, &zuo, &you); // 生成路径（16（内径），
        // chang(外径),10(射线连续长度),jiao(目标角),50(规避区域),zuo（输出左射线）,you(输出右射线)）
        //	zhuan=duizhun(0,0,1500);
        if (gbxiang == 0)
        {
            if (abs(jiajiao(dingweijiao, zuo)) < abs(jiajiao(dingweijiao, you)))
                gbxiang = 30;
            else
                gbxiang = -30;
        }
        if (movesudu < dingweichang * 30 + 2800)
        {
            movesudu += 150;
        }
        else
        {
            movesudu = dingweichang * 30 + 2800;
        }
        if (gbxiang > 0)
        {
            gbxiang--;
            if (dingweichang > 150)
                Controlmoto(zuo, movesudu, 6000, &mtl, &mtr, &mtb);
            else
                Controlmoto(zuo, dingweichang * 50, 3000, &mtl, &mtr, &mtb);
        }
        else if (gbxiang < 0)
        {
            gbxiang++;
            if (dingweichang > 150)
                Controlmoto(you, movesudu, 6000, &mtl, &mtr, &mtb);
            else
                Controlmoto(you, dingweichang * 50, 3000, &mtl, &mtr, &mtb);
        }
    }
    if (xingwei != 7)
    {
        movesudu = 0;
    }
    if (xingwei == 8)
    {
        int zuo;
        int you;
        double gchang;
        double gjiao;
        gchang = y.ball_chang;
        gjiao = y.ball_jiao;
        if (gchang < 40)
            gchang = 40;
        m_find.getzhangaiwu(40, gchang, 5, (long)(gjiao), 50, 0, &zuo, &you); // 做从35到85范围的射线,如果这个射线范围有
        // 黑色的物体并且连续的长度超过13个像素点,则会将gjiao进行偏移并返回.如果没有则返回1000
        //	zhangaiyuan=m_find.getzhangaiwu(65,(long)gchang-90,13,int(gjiao));
        //	zhangaiyuanh=m_find.getzhangaiwu(100,(long)gchang-10,13,int(gjiao));
        if (gbxiang == 0)
        {
            if (abs(jiajiao(y.ball_jiao, zuo)) < abs(jiajiao(y.ball_jiao, you)))
                gbxiang = 30;
            else
                gbxiang = -30;
        }
        if (gbxiang > 0)
        {
            gbxiang--;

            Controlmoto(zuo, (y.ball_chang - 30) * 60, 1500, &mtl, &mtr, &mtb);
            zhuan = duizhun(zuo, 0, 1000);
        }
        else if (gbxiang < 0)
        {
            Controlmoto(you, (y.ball_chang - 30) * 60, 1500, &mtl, &mtr, &mtb);
            gbxiang++;
            zhuan = duizhun(you, 0, 1000);
        }
    }
    if (xingwei == 9)
    {
        int xx;
        int yy;
        double jiao;
        double chang;
        int zuo;
        int you;
        yanchangxian(y.ball_x, y.ball_y, y.ddoor_x, y.ddoor_y, -35, &xx, &yy);
        m_find.jizuobiao(xx, yy, &jiao, &chang, 320, 240);
        m_find.guibiqiu(&y);
        m_find.getzhangaiwu(40, chang, 10, jiao, 50, 0, &zuo, &you);
        zhuan = duizhun(y.ball_jiao + 120, 0, 1500);
        //	Controlmoto(jiao,(chang-3)*100,3000,&mtl,&mtr,&mtb);
        if (gbxiang == 0)
        {
            if (abs(jiajiao(jiao, zuo)) < abs(jiajiao(jiao, you)))
                gbxiang = 30;
            else
                gbxiang = -30;
        }
        if (gbxiang > 0)
        {
            gbxiang--;
            Controlmoto(zuo, (chang - 3) * 60, 4000, &mtl, &mtr, &mtb);
        }
        else if (gbxiang < 0)
        {
            Controlmoto(you, (chang - 3) * 60, 4000, &mtl, &mtr, &mtb);
            gbxiang++;
        }
    }
    if (xingwei != 10)
    {
        djkaiqiu = 0;
        djkaiqiujs = 0;
    }
    if (xingwei == 10) // 辅助开球开始动作
    {

        djkaiqiujs++;
        if (y.benjix < 200 && djkaiqiujs == 1)
        {
            djkaiqiu = 1;
        }
        if (y.benjix < 400 && djkaiqiujs == 1 && int(y.benjix) % 2 == 0)
        {
            djkaiqiu = 1;
        }
        if (djkaiqiu == 1)
        {
            m_find.jizuobiao(1800, 600, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
            dingweijiao = -jiajiao(-dingweijiao, y.benjijiao - 90);
        }
        else
        {
            m_find.jizuobiao(net.rec_targetx, net.rec_targety, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
            dingweijiao = -jiajiao(-dingweijiao, y.benjijiao - 90);
        }
        // 根据教练机给的球的位置,将这个坐标投影到视觉
        kaiqiujiao = dingweijiao;
        kaiqiuchang = fanquxian[int(dingweichang * cdbl / 2.7)];

        if (m_pAD[0] > 2300 && m_pAD[1] > 2300 && y.ball_chang < 55 && abs(y.ball_jiao) < 20)
            chiqiupd++;
        else
            chiqiupd = 0;
        if (chiqiupd < 8)
        {
            zhuan = 2 * duizhun(y.ball_jiao, 0, 1000);
            if (abs(zhuan) > abs(zhuan_shang))
            {
                if (zhuan - zhuan_shang > 100)
                    zhuan = zhuan_shang + 100;
                if (zhuan - zhuan_shang < -100)
                    zhuan = zhuan_shang - 100;
            }
            Controlmoto(y.ball_jiao, 2000 - zhuan * 2, 2500, &mtl, &mtr, &mtb);
        }
        else
        {
            Controlmoto(0, 0, 2000, &mtl, &mtr, &mtb);
            zhuan = 3 * duizhun(kaiqiujiao, 0, 500);
            if (abs(zhuan) > abs(zhuan_shang))
            {
                if (zhuan - zhuan_shang > 100)
                    zhuan = zhuan_shang + 100;
                if (zhuan - zhuan_shang < -100)
                    zhuan = zhuan_shang - 100;
            }
            if (abs(zhuan) > 400)
                Controlmoto(-90, -2 * zhuan, 10000, &mtl, &mtr, &mtb);
        }

        if (m_pAD[0] > 2300 && m_pAD[1] > 2300 && y.ball_chang > 10 && y.ball_chang <= 60 && abs(kaiqiujiao) < 5 &&
            abs(y.ball_jiao) < 50)
        {                                    // 满足机器不在规避，并且球长度在24内，门的相对角度小于5度，球的角度小于30
            shemenleijia = shemenleijia + 1; // 射门计数器开始工作
        }
        else
            shemenleijia = 0;

        motospeed[3] = 6000;
        motospeed[4] = -6000;
        if ((shemenleijia > 20 && valxx == 1))
        {
            shemenleijia = 0;
            net.AttentionKick(y.benjix, y.benjiy, net.rec_targetx, net.rec_targety);
            if (djkaiqiu == 1)
            {
                pRobot->Kick(23000);
            }
            else
            {
                if (dingweichang > 500)
                    pRobot->Kick(8500);
                else if (abs(y.benjix - 900) < 100 && abs(y.benjiy - 600) < 100)
                    pRobot->Kick(5700); // dmenchang/50*1000+0);
                else
                    pRobot->Kick(6000);
            }
        }
    }
    if (xingwei == 12)
    {
        // 定角度防守
        //*net.rec_Def_Dist 防守时机器人距离球的距离，单位：分米*//
        //*net.rec_Def_Ang 防守队员-球-己方球门的夹角，球为夹角顶点，范围:-180~180*//
        zhuan = duizhun(y.ball_jiao, 0, 1000); // 1,目标物角度。2,自己哪个角度对准目标物。3,速度上限
        int xx1;
        int yy1;
        double jiaox;
        double changx;
        int changdu;
        changdu = fanquxian[net.rec_Def_Dist * 10] / cdbl;
        yanjiao(y.ball_x, y.ball_y, y.jdoor_x, y.jdoor_y, changdu, net.rec_targetang, &xx1, &yy1);

        m_find.jizuobiao(xx1, yy1, &jiaox, &changx, 320, 240);
        m_find.DrawFocus(xx1, yy1, &ball);
        Controlmoto(jiaox, changx * 100, 4000, &mtl, &mtr, &mtb);
    }
    if (xingwei == 15) // 截球
    {
        //	dianjijiexi()
    }

    if (0) //(xingwei==227)//防守防高球demo
    {
        if (Shot_ball > 0)
        {
            xingwei = 7;
            float yyyy = Shot_ball_point_Y + (y.benjiy - Shot_ball_point_Y) / (y.benjix - Shot_ball_point_X) *
                                                 (y.quanbx - Shot_ball_point_X);
            if (y.quanby > yyyy)
            {
                net.rec_targetx = 0;
                net.rec_targety = y.benjiy + 120;
            }
            else
            {
                net.rec_targetx = 0;
                net.rec_targety = y.benjiy - 120;
            }
            net.rec_targetang = 0;
        }
        else
        {
            xingwei = 11;
        }
    }

    char jinqubaohu = 1;
    if ((xingwei != 107)) // 移动到指定点 极坐标防守
    {
        obj_properties robotA_d;
        obj_properties ballC_d;
        robotA_d.f_x = y.benjix * 0.01;
        robotA_d.f_y = y.benjiy * 0.01;
        robotA_d.f_a = -y.benjijiao;
        ballC_d.f_x = y.quanbx * 0.01;
        ballC_d.f_y = y.quanby * 0.01;
        float ball_our_goal_radius =
            f2l_radius(ballC_d.f_x, ballC_d.f_y, 0, 0, 6); // 估算后 球对地面的位置 在地面坐标系里
        for (i = 20 - 1; i > 0; i--)                       // 存储上100周期电机的发送转速
        {
            t_radius1_107_h[i] = t_radius1_107_h[i - 1];
        }
        t_radius1_107_h[0] = ball_our_goal_radius;

        float ball_rv = t_radius1_107_h[0] - t_radius1_107_h[10];
        //	Display_A=ball_rv;
    }

    if (xingwei != 107)
    {
        trun__set = 0; // 设置速度
        rb_angle_p = 0;
    }
    if ((xingwei == 107) || (xingwei == 7) || (xingwei == 117)) // 移动到指定点 极坐标防守
    {                                                           // 移动到指定点 极坐标防守
        // call_machine_location(double jiaoa,double ball_Rradius, double *tf_x, double *tf_y ,double fx,double fy
        // ,double fa, double br,double ba)
        double tfx, tfy, tfa; // 要求到达的全场直角坐标位置
        double la, lr;        // 要求到达全场直角坐标 在本机极坐标中投影位置
        if (0)                // xingwei==107)
        {

            if (0) // y.ball_zhong<=10)// 在校正完共享球信息后 开始使用
            {

                // 当球的重量小于30时,告诉教练机自己看不见球
                //	net.m_ctrl.re_ballx
                if (diuqiuleijia < 100)
                {

                    y.quanbx = net.re_ballx;
                    y.quanby = net.re_bally;
                }
            }
            // 2011 test
            obj_properties robotA_d;
            obj_properties ballC_d;
            robotA_d.f_x = y.benjix * 0.01;
            robotA_d.f_y = y.benjiy * 0.01;
            robotA_d.f_a = -y.benjijiao;
            ballC_d.f_x = y.quanbx * 0.01;
            ballC_d.f_y = y.quanby * 0.01;

            float ball_our_goal_angle =
                f2l_angle(ballC_d.f_x, ballC_d.f_y, 0, 0, 6); // 估算后 球对地面的位置 在地面坐标系里
            float ball_our_goal_radius =
                f2l_radius(ballC_d.f_x, ballC_d.f_y, 0, 0, 6); // 估算后 球对地面的位置 在地面坐标系里
            for (i = 20 - 1; i > 0; i--)                       // 存储上100周期电机的发送转速
            {
                t_radius1_107_h[i] = t_radius1_107_h[i - 1];
            }
            t_radius1_107_h[0] = ball_our_goal_radius;
            float ball_rv = t_radius1_107_h[0] - t_radius1_107_h[10];
            //	Display_A=ball_rv;

            float robot_our_goal_angle =
                f2l_angle(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, 0, 6); // 估算后 球对地面的位置 在地面坐标系里
            float robot_our_goal_radius = f2l_radius(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, 0, 6);

            float ball_robot_angle = f2l_angle(ballC_d.f_x, ballC_d.f_y, 0, robotA_d.f_x,
                                               robotA_d.f_y); // 估算后 球对地面的位置 在地面坐标系里
            //	float a_a=fabs(angle_add_transfer2(ball_our_goal_angle-net.rec_Def_Ang,-ball_robot_angle));
            float a_a = fabs(angle_add_transfer2(ball_our_goal_angle, -ball_robot_angle));
            if (a_a > 60)
                a_a = 60;
            if (a_a < 0)
                a_a = 0;
            //	Display_A=a_a;
            float T_r = 0.36 + net.rec_Def_Dist * (a_a / 60.0) * 2.7 / cdbl / 100.0;

            float T_a = net.rec_Def_Ang;
            //	float T_r=net.rec_Def_Dist*2.7/cdbl/100.0;

            //	float T_a=net.rec_Def_Ang;
            //	float T_r=net.rec_Def_Dist*2.7/cdbl/100.0;

            float tempa = angle_add_transfer(ball_our_goal_angle, 180);
            float tempangle = f2l_angle(0, 6, tempa, robotA_d.f_x, robotA_d.f_y); // 估算后 球对地面的位置
                                                                                  // 在地面坐标系里
            float tempradius = robot_our_goal_radius * cos(tempangle * 3.14 / 180.0);
            float t_fxx = l2f_x(0, 6, 0, tempa, tempradius); // 估算后 球对地面的位置 在地面坐标系里
            float t_fyy = l2f_y(0, 6, 0, tempa, tempradius);

            float dis_robot_tx_ty = f2l_radius(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx, t_fyy);
            float v_robot = 1.0; // 2;//2.5;//5;//1.25;//dis_robot_tx_ty * 2.5;//5;//

            // 需要改成 用变速度计算 近似定价速度速度变化 下面积图形三角 或者梯形
            float time = dis_robot_tx_ty / v_robot;
            // display_b=time;

            T_r = -time * ball_rv + T_r;
            if (T_r > ball_our_goal_radius - 0.1)
                T_r = ball_our_goal_radius - 0.1;
            if (T_r < 0)
                T_r = 0;
            float net_rec_Def_Dist_temp = T_r * 100.0 * cdbl / 2.7;

            //	display_b=T_r;
            t_fxx =
                l2f_x(ballC_d.f_x, ballC_d.f_y, ball_our_goal_angle, T_a, T_r); // 估算后 球对地面的位置 在地面坐标系里
            t_fyy = l2f_y(ballC_d.f_x, ballC_d.f_y, ball_our_goal_angle, T_a, T_r);
            m_find.pField->SetAgent(5, t_fxx * 100, t_fyy * 100, 0); // 场子正中向上  逆时针坐标系

            // 2011 test

            float tempballA = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, y.quanbx, y.quanby);
            float tempballR = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, y.quanbx, y.quanby);
            // 2011 test		call_machine_location((double)(net.rec_Def_Ang) ,(double)(net.rec_Def_Dist)*2.7/cdbl ,
            // &tfx, &tfy ,y.benjix ,y.benjiy  ,-y.benjijiao,tempballR ,tempballA);//    *2.7/cdbl
            // quxian[int(y.ball_chang)]*0.49/cdbl 				call_machine_location((double)(net.rec_Def_Ang)
            //,(double)(net_rec_Def_Dist_temp)*2.7/cdbl                , &tfx, &tfy ,y.benjix ,y.benjiy
            //,-y.benjijiao,tempballR ,tempballA);//    *2.7/cdbl   quxian[int(y.ball_chang)]*0.49/cdbl

            // 2011 test		call_machine_location((double)(net.rec_Def_Ang) ,(double)(net.rec_Def_Dist)*2.7/cdbl ,
            // &tfx, &tfy ,y.benjix ,y.benjiy  ,-y.benjijiao,tempballR ,tempballA);//    *2.7/cdbl
            // quxian[int(y.ball_chang)]*0.49/cdbl
            if (net.rec_Def_Ang == 0)
            {
                call_machine_location((double)(net.rec_Def_Ang), (double)(net_rec_Def_Dist_temp) * 2.7 / cdbl, &tfx,
                                      &tfy, y.benjix, y.benjiy, -y.benjijiao, tempballR,
                                      tempballA); //    *2.7/cdbl   quxian[int(y.ball_chang)]*0.49/cdbl
            }
            else
            {
                call_machine_location((double)(net.rec_Def_Ang), (double)(net.rec_Def_Dist) * 2.7 / cdbl, &tfx, &tfy,
                                      y.benjix, y.benjiy, -y.benjijiao, tempballR,
                                      tempballA); //    *2.7/cdbl   quxian[int(y.ball_chang)]*0.49/cdbl
            }

            //???		tfa=-y.ball_jiao;//atan2(y.quanby-tfy,y.quanbx-tfx)*180/3.14;
            tfa = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, 0, 600);
            tfa = angle_add_transfer(tfa, 180);
            float penalty_y = 340;
            float penalty_x = 240;
            // if(1)//((y.benjix<penalty_x)&&(y.benjiy>(600-penalty_y))&&(y.benjiy<(1200-(600-penalty_y))))//(
            // jinqubaohu==1)
            if (xingwei_1_to_107_flag == 0)
            {
                float temptfx, temptfy;
                CalcPenaltyAreaBorder(tfx, tfy - 600, penalty_x, penalty_y, &temptfx, &temptfy);
                tfx = temptfx;
                tfy = temptfy + 600;
            }
            else
            {
                float penalty_y_ = 210;
                float penalty_x_ = 109;
                float temptfx, temptfy;
                CalcPenaltyAreaBorder(tfx, tfy - 600, penalty_x_, penalty_y_, &temptfx, &temptfy);
                tfx = temptfx;
                tfy = temptfy + 600;
                //	if (tfx<115)
                //		tfx=115;
            }

            //		la=-y.ball_jiao;//f2l_angle (y.benjix ,y.benjiy  ,-y.benjijiao,tfx,tfy);
            //		la=atan2(y.benjiy-tfy,y.benjix-tfx)*180/3.14;
            // ok1		la=f2l_angle (y.benjix ,y.benjiy  ,-y.benjijiao,tfx,tfy);
            //???	la=f2l_angle (y.benjix ,y.benjiy  ,-y.benjijiao,0,600);
            la = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, tfx, tfy);
            //		la=angle_add_transfer(la,180);
            lr = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, tfx, tfy);
            //		m_find.pField->SetAgent(6,tfx,tfy,tfa);
            m_find.pField->SetBall(6, tfx, tfy);
            pp1.add_obstacle_point(y.quanbx, y.quanby,
                                   3); // 设置规避球   注意到实际场地可能因为场地分辨率太小规避总规避得太大

            pp1.use_film_line = 2; // 设置路径规划考虑场地边线

            pp1.Obstacle_size_set = Obstacle_Parameter /
                                    field_zoom_time; // 8.2/0.4=20  实验室场地 20的障碍设置大小 设置每个障碍点的影响范围
            pp1.datain(y.benjix, y.benjiy, tfx,
                       tfy); // 输入路径规划起始点和终点 y.benjix,y.benjiy起点  y.quanbx,y.quanby终点
            //	float Close_direct=atan2(y.quanby-600,y.quanbx-900)*360/6.28;
            //	if((ttx<0)||(ttx>1800)||(tty<0)||(tty>1200 ))
            //		Direct_Cost=10;
            //	pp1.Close_target_direction_01(y.quanbx,y.quanby,Close_direct,6);
            pp1.path_plan_angle_P = angle_add_transfer(-Dribble_ball.move2Control.front_v_angle_p, y.benjijiao);
            pp1.path_plan_start_angle_cost =
                1.5; ////	front_v_p;//   可以先从 这里得到 极坐标方向掩膜 初始方向花费 的大小
            pp1.Ppgrow();
            Path_FA = pp1.path_plan_angle;
            float Path_DLA = angle_add_transfer(-Path_FA_P, Path_FA);
            float ddda = (Path_DLA * Path_DLA / 45.0 / 45.0) + 0.3;
            if (ddda > 1)
                ddda = 1;
            Path_FA = angle_add_transfer(Path_FA_P, Path_DLA * ddda);
            Path_LA = angle_add_transfer(-Path_FA,
                                         y.benjijiao); //(-pp1.path_plan_angle,y.benjijiao);//Path_LA是路径规划结果方向
                                                       // 前方0度 右正左负 顺时针坐标系
            /*显示路径规划结果*/ Debug_D[11].Draw_line_(320, 240, Path_LA - 90, 240, 255, 0, 0, pBuffer);
            /*显示路径规划结果*/ Debug_D[11].Draw_line_(322, 242, Path_LA - 90, 240, 0, 255, 255, pBuffer);
            /*显示路径规划结果*/ Debug_D[11].Draw_line_(322, 238, Path_LA - 90, 240, 255, 0, 0, pBuffer);

            mtl = 0;
            mtr = 0;
            mtb = 0;
            float mb, ml, mr;
            char black_white_ball[20];
            float angle_p_1 = 0.10; // 0.180*ax;
            float angle_i_1 = 0;    // 0.3*ax;//0.0002*ax;//0.005*ax;  //0.06*ax;//0;
            float angle_d_1 = 0;    //-0.0070;//-0.0070*ax;//
            float fangshou = 100;
            angle_p_1 = (fangshou - lr) / fangshou * 0.8 + 0.2;
            if (angle_p_1 < 0)
            {
                angle_p_1 = 0;
            }
            if (angle_p_1 > 0.10)
            {
                angle_p_1 = 0.10;
            }
            float aaaaaaa = 36;      // 3.6;//3.6;//2;//加速度
            float zuixiaosudu = 100; // 20;//18;//还是 走路径 规划 的最小速度
            Dribble_ball.move2Control.limit_parameter_set_angle(
                10.0, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
            Dribble_ball.move2Control.limit_parameter_set_dis(
                zuixiaosudu * 1.35,
                36); //(62.0 , 15);//(39.0 , 12);//(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
            Dribble_ball.move2Control.parameter_set_angle(angle_p_1, angle_i_1, angle_d_1, 0);

            // Diplay_B=path_l;
            if (path_l > 1) // 韩秋实
            {
                Dribble_ball.move2Control.initializing_movement(0, 0, 0); //
                Dribble_ball.move2Control.zs_angle_track_001(tfa, 0 /*benchmark_annngle*/, black_white_ball);
                float path_long = (path_l - 2) * 9.0 * MK / 1.25 + zuixiaosudu;
                if (path_long > (lujingchang + aaaaaaa))
                    path_long = lujingchang + aaaaaaa;
                if (path_long < (lujingchang - aaaaaaa * 1.4))
                    path_long = lujingchang - aaaaaaa * 1.4;
                lujingchang = path_long;
                // Diplay_A=path_long;
                Dribble_ball.move2Control.front_v_c = lujingchang;
                Dribble_ball.move2Control.front_v_angle_c = Path_LA;
                Dribble_ball.move2Control.front_v_maxlimit();
                Dribble_ball.move2Control.front_a_maxlimit();
                Dribble_ball.move2Control.uptech_output_O(&mb, &ml, &mr);
                // Diplay_C=Path_LA;
                if (path_l < 6)
                    path_long = (path_l - 2) * 9.0 * MK / 1.25 + zuixiaosudu;
            }
            else
            {
                Dribble_ball.move2Control.limit_parameter_set_dis(
                    zuixiaosudu,
                    36); //(zs , 10);//(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                Dribble_ball.move2Control.initializing_movement(0, 0, 0); //
                Dribble_ball.move2Control.zs_angle_track_001(tfa, 0 /*benchmark_angle*/, black_white_ball);
                float path_long = lr * 1.25 * MK / 1.25; // 0.5;
                // Diplay_A=path_long;
                // Diplay_C=la;
                //			if (path_long>(lujingchang+aaaaaaa))
                lujingchang = path_long;
                Dribble_ball.move2Control.front_v_c = lujingchang;
                Dribble_ball.move2Control.front_v_angle_c = la;
                Dribble_ball.move2Control.front_v_maxlimit();
                Dribble_ball.move2Control.front_a_maxlimit();
                Dribble_ball.move2Control.uptech_output_O(&mb, &ml, &mr);
            }
            float p = 0.4; // 0.6;
            mtb = mb * p;
            mtl = ml * p;
            mtr = mr * p; // 设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000
                          // = 2转/秒
            motospeed[0] = mtl;
            motospeed[1] = mtr;
            motospeed[2] = mtb;
            zhuan = 0; // 鑫鑫程序里的转动变量设置成0
        }

        if (xingwei == 107)
        {

            if (y.ball_zhong <= 10) // 在校正完共享球信息后 开始使用
            {

                // 当球的重量小于30时,告诉教练机自己看不见球
                //	net.m_ctrl.re_ballx
                if (diuqiuleijia < 100 - 20)
                {

                    y.quanbx = net.re_ballx;
                    y.quanby = net.re_bally;
                }
            }
            // 2013  force_xingwei_107_quanbx_y
            if (force_xingwei_107_quanbx_y_flag == 1)
            {
                set_xingwei_107_temp_quanbx = y.quanbx;
                set_xingwei_107_temp_quanby = y.quanby;
                y.quanbx = set_xingwei_107_quanbx;
                y.quanby = set_xingwei_107_quanby;
            }
            // 2013  force_xingwei_107_quanbx_y

            // 2011 test
            obj_properties robotA_d;
            obj_properties ballC_d;
            robotA_d.f_x = y.benjix * 0.01;
            robotA_d.f_y = y.benjiy * 0.01;
            robotA_d.f_a = -y.benjijiao;
            ballC_d.f_x = y.quanbx * 0.01;
            ballC_d.f_y = y.quanby * 0.01;

            float ball_our_goal_angle =
                f2l_angle(ballC_d.f_x, ballC_d.f_y, 0, 0, 6); // 估算后 球对地面的位置 在地面坐标系里
            float ball_our_goal_radius =
                f2l_radius(ballC_d.f_x, ballC_d.f_y, 0, 0, 6); // 估算后 球对地面的位置 在地面坐标系里
            for (i = 20 - 1; i > 0; i--)                       // 存储上100周期电机的发送转速
            {
                t_radius1_107_h[i] = t_radius1_107_h[i - 1];
            }
            t_radius1_107_h[0] = ball_our_goal_radius;
            float ball_rv = t_radius1_107_h[0] - t_radius1_107_h[10];
            //	Display_A=ball_rv;

            float robot_our_goal_angle =
                f2l_angle(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, 0, 6); // 估算后 球对地面的位置 在地面坐标系里
            float robot_our_goal_radius = f2l_radius(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, 0, 6);

            float ball_robot_angle = f2l_angle(ballC_d.f_x, ballC_d.f_y, 0, robotA_d.f_x,
                                               robotA_d.f_y); // 估算后 球对地面的位置 在地面坐标系里
            //	float a_a=fabs(angle_add_transfer2(ball_our_goal_angle-net.rec_Def_Ang,-ball_robot_angle));
            float a_a = fabs(angle_add_transfer2(ball_our_goal_angle, -ball_robot_angle));
            if (a_a > 60)
                a_a = 60;
            if (a_a < 0)
                a_a = 0;
            //	Display_A=a_a;
            float distT_r = 0.39;                                                         // 最近距离球    (米)
            float T_r_a = (distT_r + 150 * 2.7 / cdbl / 100.0) / 60.0;                    // 接近球加速度  60周期 (米)
            float T_r = distT_r + net.rec_Def_Dist * (a_a / 60.0) * 2.7 / cdbl / 100.0;   // 预计防守距离 (米)
            float lrr = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, y.quanbx, y.quanby); // 距离 目标点距离  (厘米)

            if (lrr > distT_r * 100.0 + net.rec_Def_Dist + 100)
            {
                T_r = distT_r + net.rec_Def_Dist * 2.7 / cdbl / 100.0;
                T_r_PP = T_r;
            }
            if (T_r > T_r_PP + T_r_a * 2.0)
                T_r = T_r_PP + T_r_a * 2.0;
            if (T_r < T_r_PP - T_r_a)
                T_r = T_r_PP - T_r_a;
            T_r_PP = T_r;
            Display_A = T_r_PP;
            Display_B = T_r;
            Display_C = lrr;

            float T_a = net.rec_Def_Ang;
            //	float T_r=net.rec_Def_Dist*2.7/cdbl/100.0;

            //	float T_a=net.rec_Def_Ang;
            //	float T_r=net.rec_Def_Dist*2.7/cdbl/100.0;

            float tempa = angle_add_transfer(ball_our_goal_angle, 180);
            float tempangle = f2l_angle(0, 6, tempa, robotA_d.f_x, robotA_d.f_y); // 估算后 球对地面的位置
                                                                                  // 在地面坐标系里
            float tempradius = robot_our_goal_radius * cos(tempangle * 3.14 / 180.0);
            float t_fxx = l2f_x(0, 6, 0, tempa, tempradius); // 估算后 球对地面的位置 在地面坐标系里
            float t_fyy = l2f_y(0, 6, 0, tempa, tempradius);

            float dis_robot_tx_ty = f2l_radius(robotA_d.f_x, robotA_d.f_y, robotA_d.f_a, t_fxx, t_fyy);
            float v_robot = 1.0; // 2;//2.5;//5;//1.25;//dis_robot_tx_ty * 2.5;//5;//

            // 需要改成 用变速度计算 近似定价速度速度变化 下面积图形三角 或者梯形
            float time = dis_robot_tx_ty / v_robot;
            // display_b=time;

            T_r = -time * ball_rv + T_r;
            if (T_r > ball_our_goal_radius - 0.1)
                T_r = ball_our_goal_radius - 0.1;
            if (T_r < 0)
                T_r = 0;
            float net_rec_Def_Dist_temp = T_r * 100.0 * cdbl / 2.7;

            //	display_b=T_r;
            t_fxx =
                l2f_x(ballC_d.f_x, ballC_d.f_y, ball_our_goal_angle, T_a, T_r); // 估算后 球对地面的位置 在地面坐标系里
            t_fyy = l2f_y(ballC_d.f_x, ballC_d.f_y, ball_our_goal_angle, T_a, T_r);
            m_find.pField->SetAgent(5, t_fxx * 100, t_fyy * 100, 0); // 场子正中向上  逆时针坐标系

            // 2011 test

            float tempballA = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, y.quanbx, y.quanby);
            float tempballR = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, y.quanbx, y.quanby);
            // 2011 test		call_machine_location((double)(net.rec_Def_Ang) ,(double)(net.rec_Def_Dist)*2.7/cdbl ,
            // &tfx, &tfy ,y.benjix ,y.benjiy  ,-y.benjijiao,tempballR ,tempballA);//    *2.7/cdbl
            // quxian[int(y.ball_chang)]*0.49/cdbl 				call_machine_location((double)(net.rec_Def_Ang)
            //,(double)(net_rec_Def_Dist_temp)*2.7/cdbl                , &tfx, &tfy ,y.benjix ,y.benjiy
            //,-y.benjijiao,tempballR ,tempballA);//    *2.7/cdbl   quxian[int(y.ball_chang)]*0.49/cdbl

            // 2011 test		call_machine_location((double)(net.rec_Def_Ang) ,(double)(net.rec_Def_Dist)*2.7/cdbl ,
            // &tfx, &tfy ,y.benjix ,y.benjiy  ,-y.benjijiao,tempballR ,tempballA);//    *2.7/cdbl
            // quxian[int(y.ball_chang)]*0.49/cdbl
            if (net.rec_Def_Ang == 0)
            {
                call_machine_location((double)(net.rec_Def_Ang), (double)(net_rec_Def_Dist_temp) * 2.7 / cdbl, &tfx,
                                      &tfy, y.benjix, y.benjiy, -y.benjijiao, tempballR,
                                      tempballA); //    *2.7/cdbl   quxian[int(y.ball_chang)]*0.49/cdbl
            }
            else
            {
                call_machine_location((double)(net.rec_Def_Ang), (double)(net.rec_Def_Dist) * 2.7 / cdbl, &tfx, &tfy,
                                      y.benjix, y.benjiy, -y.benjijiao, tempballR,
                                      tempballA); //    *2.7/cdbl   quxian[int(y.ball_chang)]*0.49/cdbl
            }

            //???		tfa=-y.ball_jiao;//atan2(y.quanby-tfy,y.quanbx-tfx)*180/3.14;
            tfa = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, 0, 600);
            tfa = angle_add_transfer(tfa, 180);
            float penalty_y = 340;
            float penalty_x = 240;
            if (xingwei_1_to_107_flag == 0)
            {
                float temptfx, temptfy;
                CalcPenaltyAreaBorder(tfx, tfy - 600, penalty_x, penalty_y, &temptfx, &temptfy);
                tfx = temptfx;
                tfy = temptfy + 600;
            }
            else
            { /*
                 float penalty_y_=210;
                 float penalty_x_=109;
                 float temptfx,temptfy;
                 CalcPenaltyAreaBorder(tfx, tfy-600, penalty_x_, penalty_y_, &temptfx,&temptfy);
                 tfx=temptfx;
                 tfy=temptfy+600;
             //	if (tfx<115)
             //		tfx=115;
             */
                float penalty_y_ = 210;
                float penalty_x_ = 109;
                float temptfx, temptfy;
                CalcPenaltyAreaBorder(tfx, tfy - 600, penalty_x_, penalty_y_, &temptfx, &temptfy);
                tfx = temptfx;
                tfy = temptfy + 600;
                //	if (tfx<115)
                //		tfx=115;
                if (y.quanbx <= 150 && y.quanby >= (600 - 255) && y.quanby <= (600 + 255))
                {
                    float penalty_y_ = 270;
                    float penalty_x_ = 169;
                    float temptfx, temptfy;
                    CalcPenaltyAreaBorder(tfx, tfy - 600, penalty_x_, penalty_y_, &temptfx, &temptfy);
                    tfx = temptfx;
                    tfy = y.quanby; // temptfy+600;
                    if (tfx < y.quanbx + 60)
                        tfx = y.quanbx + 60;
                }
            }

            // 2013 force_xingwei_107_target
            if (force_xingwei_107_target_flag == 1)
            {
                tfa = set_xingwei_107_tfa;
                tfx = set_xingwei_107_tfx;
                tfy = set_xingwei_107_tfy;
                xingwei = temp_xingwei_num;
            }
            force_xingwei_107_target_flag = 0;
            // 2013  force_xingwei_107_target

            //		la=-y.ball_jiao;//f2l_angle (y.benjix ,y.benjiy  ,-y.benjijiao,tfx,tfy);
            //		la=atan2(y.benjiy-tfy,y.benjix-tfx)*180/3.14;
            // ok1		la=f2l_angle (y.benjix ,y.benjiy  ,-y.benjijiao,tfx,tfy);
            //???	la=f2l_angle (y.benjix ,y.benjiy  ,-y.benjijiao,0,600);
            la = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, tfx, tfy);
            //		la=angle_add_transfer(la,180);
            lr = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, tfx, tfy);
            //		m_find.pField->SetAgent(6,tfx,tfy,tfa);
            m_find.pField->SetBall(6, tfx, tfy);
            pp1.add_obstacle_point(y.quanbx, y.quanby,
                                   3); // 设置规避球   注意到实际场地可能因为场地分辨率太小规避总规避得太大

            if (xingwei_1_to_107_flag == 0)
            {
                pp1.use_film_line = 2; // 设置路径规划考虑场地边线
            }
            else
            {
                pp1.use_film_line = 0; // 设置路径规划no考虑场地边线
            }

            pp1.Obstacle_size_set = Obstacle_Parameter /
                                    field_zoom_time; // 8.2/0.4=20  实验室场地 20的障碍设置大小 设置每个障碍点的影响范围
            pp1.datain(y.benjix, y.benjiy, tfx,
                       tfy); // 输入路径规划起始点和终点 y.benjix,y.benjiy起点  y.quanbx,y.quanby终点
            //	float Close_direct=atan2(y.quanby-600,y.quanbx-900)*360/6.28;
            //	if((ttx<0)||(ttx>1800)||(tty<0)||(tty>1200 ))
            //		Direct_Cost=10;
            //	pp1.Close_target_direction_01(y.quanbx,y.quanby,Close_direct,6);
            pp1.path_plan_angle_P = angle_add_transfer(-Dribble_ball.move2Control.front_v_angle_p, y.benjijiao);
            pp1.path_plan_start_angle_cost =
                1.5; ////	front_v_p;//   可以先从 这里得到 极坐标方向掩膜 初始方向花费 的大小
            pp1.Ppgrow();
            Path_FA = pp1.path_plan_angle;
            float Path_DLA = angle_add_transfer(-Path_FA_P, Path_FA);
            float ddda = (Path_DLA * Path_DLA / 45.0 / 45.0) + 0.3;
            if (ddda > 1)
                ddda = 1;
            Path_FA = angle_add_transfer(Path_FA_P, Path_DLA * ddda);
            Path_LA = angle_add_transfer(-Path_FA,
                                         y.benjijiao); //(-pp1.path_plan_angle,y.benjijiao);//Path_LA是路径规划结果方向
                                                       // 前方0度 右正左负 顺时针坐标系
            /*显示路径规划结果*/ Debug_D[11].Draw_line_(320, 240, Path_LA - 90, 240, 255, 0, 0, pBuffer);
            /*显示路径规划结果*/ Debug_D[11].Draw_line_(322, 242, Path_LA - 90, 240, 0, 255, 255, pBuffer);
            /*显示路径规划结果*/ Debug_D[11].Draw_line_(322, 238, Path_LA - 90, 240, 255, 0, 0, pBuffer);

            mtl = 0;
            mtr = 0;
            mtb = 0;
            float mb, ml, mr;
            char black_white_ball[20];
            float angle_p_1 = 0.10; // 0.180*ax;
            float angle_i_1 = 0;    // 0.3*ax;//0.0002*ax;//0.005*ax;  //0.06*ax;//0;
            float angle_d_1 = 0;    //-0.0070;//-0.0070*ax;//
            float fangshou = 180;

            float lrt = fabs(lr);
            if (lrt > fangshou)
                lrt = fangshou;
            if (lrt < 0)
                lrt = 0;
            angle_p_1 = (fangshou - lr) / fangshou * 0.8 + 0.2;
            if (angle_p_1 < 0)
            {
                angle_p_1 = 0;
            }
            if (angle_p_1 > 1.0)
            {
                angle_p_1 = 1.0;
            }
            angle_p_1 = angle_p_1 * 0.33;
            // Display_A=angle_p_1;
            // Display_B=lr;
            float p = 0.65; // 0.1;//0.65;//0.4;//
            if (xingwei_1_to_107_flag == 1)
            {
                float ap = 0;
                //	ap=(y.benjix-80.0)/130.0;
                float dr = sqrt((y.benjix + 150) * (y.benjix + 150) + (y.benjiy - 600) * (y.benjiy - 600));
                ap = (dr - 285.0) / 300.0;
                if (ap > 1)
                    ap = 1;
                if (ap < 0)
                    ap = 0;
                p = 0.36 + 0.29 * ap;

                if (y.quanbx <= 150 && y.quanby >= (600 - 255) && y.quanby <= (600 + 255))
                {
                    ap = (dr - 285.0) / 600.0;
                    if (ap > 1)
                        ap = 1;
                    if (ap < 0)
                        ap = 0;
                    p = 0.16 + 0.19 * ap;
                }
            }
            //	float front_a,front_v,turn_w;
            //	Dribble_ball.O_movement_T_analyze(&front_a,&front_v,&turn_w,motol_as,motob_as,motor_as);//
            //*move_v_angle, *move_v_speed, *turn_v 	Dribble_ball.move2Control.front_v_p		=
            // front_v*0.005126/p; 	lujingchang								=					front_v*0.005126/p;
            //	Dribble_ball.move2Control.front_v_angle_p	=angle_add_transfer(front_a,-180);
            Dribble_ball.move2Control.Moto_to_P_movement(
                motol_as / p, motob_as / p, motor_as / p); // 全向解析 输入上周期电机速度 得到上周期平移三分量

            float aaaaaaa = 2.5;     // 6;//66;//66//3.6;//3.6;//2;//加速度
            float zuixiaosudu = 100; // 135;//180;//180//20;//18;//还是 走路径 规划 的最小速度
            float speedP = 0.6;      // 0.6+0.3;//0.6
            Dribble_ball.move2Control.limit_parameter_set_angle(
                10.0, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
            Dribble_ball.move2Control.limit_parameter_set_dis(
                zuixiaosudu * 1.0, aaaaaaa); //(62.0 , 15);//(39.0 , 12);//(21,1);//3(128,328);//float
                                             // velocity_limit_,float acceleration_limit_
            Dribble_ball.move2Control.parameter_set_angle(angle_p_1, angle_i_1, angle_d_1, 0);
            // test
            //		Dribble_ball.move2Control.parameter_set_angle(    angle_p_1*2	, angle_i_1 , angle_d_1 , 0);

            // Diplay_B=path_l;
            if (path_l > 1) // 韩秋实
            {
                Dribble_ball.move2Control.initializing_movement(0, 0, 0); //
                Dribble_ball.move2Control.zs_angle_track_001(tfa, 0 /*benchmark_annngle*/, black_white_ball);
                //	float path_long = lr*speedP* MK /1.25;//(path_l-2)*18.0* MK /1.25+zuixiaosudu;
                float path_long = lr * speedP; //(path_l-2)*18.0* MK /1.25+zuixiaosudu;
                if (path_long > (lujingchang + aaaaaaa))
                    path_long = lujingchang + aaaaaaa;
                if (path_long < (lujingchang - aaaaaaa * 1.4))
                    path_long = lujingchang - aaaaaaa * 1.4;
                lujingchang = path_long;
                // Diplay_A=path_long;
                if (0)
                {
                    float rb_angle = y.benjijiao; // ball_angle;//当前 球C~机器人A x 距离
                    float rb_d_angle = rb_angle - rb_angle_p;
                    if (rb_d_angle > 0.15)
                        rb_d_angle = 0.15;
                    if (rb_d_angle < -0.15)
                        rb_d_angle = -0.15;
                    //				trun__set +=(rb_d_angle + rb_angle*0.0110)*3.12;//设置速度
                    trun__set += (rb_d_angle + 0) * 11.00; // 设置速度
                    //				trun__set +=(rb_d_angle + rb_angle*0.0110)*30.12;//设置速度
                    if (trun__set > 60)
                        trun__set = 60;
                    if (trun__set < -60)
                        trun__set = -60;
                    rb_angle_p = rb_angle;

                    Dribble_ball.move2Control.turn_v_c = trun__set;
                }

                Dribble_ball.move2Control.front_v_c = lujingchang;
                Dribble_ball.move2Control.front_v_angle_c = Path_LA;
                Dribble_ball.move2Control.front_v_maxlimit();
                Dribble_ball.move2Control.front_a_maxlimit();
                Dribble_ball.move2Control.uptech_output_O(&mb, &ml, &mr);
                // Diplay_C=Path_LA;
                if (path_l < 6)
                    path_long = lr * speedP * MK / 1.25; //(path_l-2)*9.0* MK /1.25+zuixiaosudu;
            }
            else
            {
                //			Dribble_ball.move2Control.limit_parameter_set_dis  (zuixiaosudu , 66);//(zs ,
                // 10);//(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                Dribble_ball.move2Control.initializing_movement(0, 0, 0); //
                Dribble_ball.move2Control.zs_angle_track_001(tfa, 0 /*benchmark_angle*/, black_white_ball);
                //	float path_long = lr*speedP* MK /1.25;//0.5;
                float path_long = lr * speedP; // 0.5;
                // Diplay_A=path_long;
                // Diplay_C=la;
                //			if (path_long>(lujingchang+aaaaaaa))
                lujingchang = path_long;
                Dribble_ball.move2Control.front_v_c = lujingchang;
                Dribble_ball.move2Control.front_v_angle_c = la;
                Dribble_ball.move2Control.front_v_maxlimit();
                Dribble_ball.move2Control.front_a_maxlimit();
                //	Dribble_ball.move2Control.turn_a_maxlimit();
                Dribble_ball.move2Control.uptech_output_O(&mb, &ml, &mr);
            }
            // Display_A=Dribble_ball.move2Control.front_v_c;

            mtb = mb * p;
            mtl = ml * p;
            mtr = mr * p; // 设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000
                          // = 2转/秒
            motospeed[0] = mtl;
            motospeed[1] = mtr;
            motospeed[2] = mtb;
            zhuan = 0; // 鑫鑫程序里的转动变量设置成0

            // 2013  force_xingwei_107_quanbx_y
            if (force_xingwei_107_quanbx_y_flag == 1)
            {
                y.quanbx = set_xingwei_107_temp_quanbx;
                y.quanby = set_xingwei_107_temp_quanby;
                xingwei = temp_xingwei_num;
            }
            force_xingwei_107_quanbx_y_flag = 0;
            // 2013  force_xingwei_107_quanbx_y
            if (xingwei_1_to_107_flag == 1)
            {
                xingwei = 1;
            }
            net.SetDestToS(tfx, tfy);
        }

        if (xingwei == 7)
        {
            tfx = net.rec_targetx;
            tfy = net.rec_targety;
            tfa = angle_add_transfer(-net.rec_targetang, y.benjijiao); //-net.rec_targetang;

            // 2013  force_xingwei_7_target
            if (force_xingwei_7_target_flag == 1)
            {
                tfa = set_xingwei_7_tfa;
                tfx = set_xingwei_7_tfx;
                tfy = set_xingwei_7_tfy;
                xingwei = temp_xingwei_num;
            }
            force_xingwei_7_target_flag = 0;
            // 2013  force_xingwei_7_target

            //	la=angle_add_transfer(-tfa,y.benjijiao);//f2l_angle (y.benjix ,y.benjiy  ,-y.benjijiao,tfx,tfy);
            //		la=atan2(y.benjiy-tfy,y.benjix-tfx)*180/3.14;
            la = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, tfx, tfy);
            lr = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, tfx, tfy);
            m_find.pField->SetAgent(6, tfx, tfy, tfa);
            m_find.pField->SetBall(6, tfx, tfy);

            pp1.add_obstacle_point(y.quanbx, y.quanby,
                                   3); // 设置规避球   注意到实际场地可能因为场地分辨率太小规避总规避得太大

            pp1.use_film_line = 0; // 设置路径规划考虑场地边线

            //		pp1.add_obstacle_point(0,y.quanby,3);//设置规避球
            // 注意到实际场地可能因为场地分辨率太小规避总规避得太大
            //		pp1.add_obstacle_point(y.quanbx,y.quanby,3);//设置规避球
            // 注意到实际场地可能因为场地分辨率太小规避总规避得太大
            pp1.Obstacle_size_set = Obstacle_Parameter /
                                    field_zoom_time; // 8.2/0.4=20  实验室场地 20的障碍设置大小 设置每个障碍点的影响范围
            pp1.datain(y.benjix, y.benjiy, tfx,
                       tfy); // 输入路径规划起始点和终点 y.benjix,y.benjiy起点  y.quanbx,y.quanby终点
            //	float Close_direct=atan2(y.quanby-600,y.quanbx-900)*360/6.28;
            //	if((ttx<0)||(ttx>1800)||(tty<0)||(tty>1200 ))
            //		Direct_Cost=10;
            //	pp1.Close_target_direction_01(y.quanbx,y.quanby,Close_direct,6);
            pp1.path_plan_angle_P = angle_add_transfer(-Dribble_ball.move2Control.front_v_angle_p, y.benjijiao);
            pp1.path_plan_start_angle_cost =
                1.5; ////	front_v_p;//   可以先从 这里得到 极坐标方向掩膜 初始方向花费 的大小

            pp1.Ppgrow();
            QueryPerformanceCounter(&CountEnd);
            dfElapseMS3 =
                (double)((double)(CountEnd.QuadPart - CountStart.QuadPart + 10) / (double)Frequency.QuadPart) * 1000.0;

            Path_FA = pp1.path_plan_angle;
            float Path_DLA = angle_add_transfer(-Path_FA_P, Path_FA);
            float ddda = (Path_DLA * Path_DLA / 45.0 / 45.0) + 0.3;
            if (ddda > 1)
                ddda = 1;
            Path_FA = angle_add_transfer(Path_FA_P, Path_DLA * ddda);
            Path_LA = angle_add_transfer(-Path_FA,
                                         y.benjijiao); //(-pp1.path_plan_angle,y.benjijiao);//Path_LA是路径规划结果方向
                                                       // 前方0度 右正左负 顺时针坐标系
            /*显示路径规划结果*/ Debug_D[11].Draw_line_(320, 240, Path_LA - 90, 240, 255, 0, 0, pBuffer);
            /*显示路径规划结果*/ Debug_D[11].Draw_line_(322, 242, Path_LA - 90, 240, 0, 255, 255, pBuffer);
            /*显示路径规划结果*/ Debug_D[11].Draw_line_(322, 238, Path_LA - 90, 240, 255, 0, 0, pBuffer);
            float p = 1.2; // 35;//1.05;//1.1;//1.15;
            Dribble_ball.move2Control.Moto_to_P_movement(
                motol_as / p, motob_as / p, motor_as / p); // 全向解析 输入上周期电机速度 得到上周期平移三分量

            mtl = 0;
            mtr = 0;
            mtb = 0;
            float mb, ml, mr;
            char black_white_ball[20];
            float angle_p_1 = 0.19;    // 0.180*ax;
            float angle_i_1 = 0;       // 0.3*ax;//0.0002*ax;//0.005*ax;  //0.06*ax;//0;
            float angle_d_1 = -0.0070; //-0.0070*ax;//
            float acc = 1.21;          //
            Dribble_ball.move2Control.limit_parameter_set_angle(
                10.0, 0.8); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
            Dribble_ball.move2Control.limit_parameter_set_dis(
                39.0, acc); //(39.0 , 1.5);//(42.0 , 15);//(21,1);//3(128,328);//float velocity_limit_,float
                            // acceleration_limit_
            Dribble_ball.move2Control.parameter_set_angle(angle_p_1, angle_i_1, angle_d_1, 0);
            float aaaaaaa = 0.95;   // 1.0;//1.2;1.4;//1.9;//1.5;//2;//加速度
            float zuixiaosudu = 36; // 36;//18;//还是 走路径 规划 的最小速度

            //		Dribble_ball.move2Control.limit_parameter_set_angle(10.0,1);//(21,1);//3(128,328);//float
            // velocity_limit_,float acceleration_limit_ 		Dribble_ball.move2Control.limit_parameter_set_dis  (68.0
            // , 20);//(62.0 , 15);//(39.0 , 12);//(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
            //		Dribble_ball.move2Control.parameter_set_angle(    angle_p_1	, angle_i_1 , angle_d_1 , 0);
            //		float aaaaaaa=3;//3.6;//2;//加速度
            //		float zuixiaosudu=18;//18;//还是 走路径 规划 的最小速度

            if (path_l > 1) // 韩秋实
            {
                Dribble_ball.move2Control.initializing_movement(0, 0, 0); //
                Dribble_ball.move2Control.zs_angle_track_001(tfa, 0 /*benchmark_annngle*/, black_white_ball);
                float path_long = (path_l - 2) * 6.9 + zuixiaosudu; // 5.0
                if (path_long > (lujingchang + aaaaaaa))
                    path_long = lujingchang + aaaaaaa;
                if (path_long < (lujingchang - aaaaaaa * 1.19))
                    path_long = lujingchang - aaaaaaa * 1.19;
                lujingchang = path_long;
                Dribble_ball.move2Control.front_v_c = lujingchang;
                Dribble_ball.move2Control.front_v_angle_c = Path_LA;
                Dribble_ball.move2Control.front_v_maxlimit();
                Dribble_ball.move2Control.front_a_maxlimit();
                Dribble_ball.move2Control.uptech_output_O(&mb, &ml, &mr);
            }
            else
            {
                Dribble_ball.move2Control.limit_parameter_set_dis(
                    zuixiaosudu,
                    acc); //(zs , 10);//(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                Dribble_ball.move2Control.initializing_movement(0, 0, 0); //
                Dribble_ball.move2Control.zs_angle_track_001(tfa, 0 /*benchmark_angle*/, black_white_ball);
                float path_long = lr * 0.35; // 0.5;//1.1 0.6
                //			if (path_long>(lujingchang+aaaaaaa))
                lujingchang = path_long;
                Dribble_ball.move2Control.front_v_c = lujingchang;
                Dribble_ball.move2Control.front_v_angle_c = la;
                Dribble_ball.move2Control.front_v_maxlimit();
                Dribble_ball.move2Control.front_a_maxlimit();
                Dribble_ball.move2Control.uptech_output_O(&mb, &ml, &mr);
            }
            mtb = mb * p;
            mtl = ml * p;
            mtr = mr * p; // 设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000
                          // = 2转/秒
            motospeed[0] = mtl;
            motospeed[1] = mtr;
            motospeed[2] = mtb;
            zhuan = 0; // 鑫鑫程序里的转动变量设置成0

            net.SetDestToS(tfx, tfy);
        }

        if (xingwei == 117)
        {
            tfx = net.rec_targetx;
            tfy = net.rec_targety;
            tfa = angle_add_transfer(-net.rec_targetang, y.benjijiao); //-net.rec_targetang;
            //	la=angle_add_transfer(-tfa,y.benjijiao);//f2l_angle (y.benjix ,y.benjiy  ,-y.benjijiao,tfx,tfy);
            //		la=atan2(y.benjiy-tfy,y.benjix-tfx)*180/3.14;
            la = f2l_angle(y.benjix, y.benjiy, -y.benjijiao, tfx, tfy);
            lr = f2l_radius(y.benjix, y.benjiy, -y.benjijiao, tfx, tfy);
            m_find.pField->SetAgent(6, tfx, tfy, tfa);
            m_find.pField->SetBall(6, tfx, tfy);

            int x, yy;
            for (x = 0; x <= 150; x += 50)
                for (yy = 0; yy <= 150; yy += 50)
                {
                    pp1.add_obstacle_point(y.quanbx + x, y.quanby + yy,
                                           6); // 设置规避球   注意到实际场地可能因为场地分辨率太小规避总规避得太大
                    pp1.add_obstacle_point(y.quanbx + x, y.quanby - yy,
                                           6); // 设置规避球   注意到实际场地可能因为场地分辨率太小规避总规避得太大
                    pp1.add_obstacle_point(y.quanbx - x, y.quanby + yy,
                                           6); // 设置规避球   注意到实际场地可能因为场地分辨率太小规避总规避得太大
                    pp1.add_obstacle_point(y.quanbx - x, y.quanby - yy,
                                           6); // 设置规避球   注意到实际场地可能因为场地分辨率太小规避总规避得太大
                }

            pp1.use_film_line = 1; // 设置路径规划考虑场地边线

            //		pp1.add_obstacle_point(0,y.quanby,3);//设置规避球
            // 注意到实际场地可能因为场地分辨率太小规避总规避得太大
            //		pp1.add_obstacle_point(y.quanbx,y.quanby,3);//设置规避球
            // 注意到实际场地可能因为场地分辨率太小规避总规避得太大
            pp1.Obstacle_size_set = Obstacle_Parameter /
                                    field_zoom_time; // 8.2/0.4=20  实验室场地 20的障碍设置大小 设置每个障碍点的影响范围
            pp1.datain(y.benjix, y.benjiy, tfx,
                       tfy); // 输入路径规划起始点和终点 y.benjix,y.benjiy起点  y.quanbx,y.quanby终点
            //	float Close_direct=atan2(y.quanby-600,y.quanbx-900)*360/6.28;
            //	if((ttx<0)||(ttx>1800)||(tty<0)||(tty>1200 ))
            //		Direct_Cost=10;
            //	pp1.Close_target_direction_01(y.quanbx,y.quanby,Close_direct,6);
            pp1.path_plan_angle_P = angle_add_transfer(-Dribble_ball.move2Control.front_v_angle_p, y.benjijiao);
            pp1.path_plan_start_angle_cost =
                1.5; ////	front_v_p;//   可以先从 这里得到 极坐标方向掩膜 初始方向花费 的大小

            pp1.Ppgrow();

            QueryPerformanceCounter(&CountEnd);
            dfElapseMS3 =
                (double)((double)(CountEnd.QuadPart - CountStart.QuadPart + 10) / (double)Frequency.QuadPart) * 1000.0;

            Path_FA = pp1.path_plan_angle;
            float Path_DLA = angle_add_transfer(-Path_FA_P, Path_FA);
            float ddda = (Path_DLA * Path_DLA / 45.0 / 45.0) + 0.3;
            if (ddda > 1)
                ddda = 1;
            Path_FA = angle_add_transfer(Path_FA_P, Path_DLA * ddda);
            Path_LA = angle_add_transfer(-Path_FA,
                                         y.benjijiao); //(-pp1.path_plan_angle,y.benjijiao);//Path_LA是路径规划结果方向
                                                       // 前方0度 右正左负 顺时针坐标系
            /*显示路径规划结果*/ Debug_D[11].Draw_line_(320, 240, Path_LA - 90, 240, 255, 0, 0, pBuffer);
            /*显示路径规划结果*/ Debug_D[11].Draw_line_(322, 242, Path_LA - 90, 240, 0, 255, 255, pBuffer);
            /*显示路径规划结果*/ Debug_D[11].Draw_line_(322, 238, Path_LA - 90, 240, 255, 0, 0, pBuffer);

            mtl = 0;
            mtr = 0;
            mtb = 0;
            float mb, ml, mr;
            char black_white_ball[20];
            float angle_p_1 = 0.19;    // 0.180*ax;
            float angle_i_1 = 0;       // 0.3*ax;//0.0002*ax;//0.005*ax;  //0.06*ax;//0;
            float angle_d_1 = -0.0070; //-0.0070*ax;//
            Dribble_ball.move2Control.limit_parameter_set_angle(
                10.0, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
            Dribble_ball.move2Control.limit_parameter_set_dis(
                39.0,
                6); //(39.0 , 1.5);//(42.0 , 15);//(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
            Dribble_ball.move2Control.parameter_set_angle(angle_p_1, angle_i_1, angle_d_1, 0);
            float aaaaaaa = 1.5;   // 2;//加速度
            float zuixiaosudu = 6; // 还是 走路径 规划 的最小速度

            //		Dribble_ball.move2Control.limit_parameter_set_angle(10.0,1);//(21,1);//3(128,328);//float
            // velocity_limit_,float acceleration_limit_ 		Dribble_ball.move2Control.limit_parameter_set_dis  (68.0
            // , 20);//(62.0 , 15);//(39.0 , 12);//(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
            //		Dribble_ball.move2Control.parameter_set_angle(    angle_p_1	, angle_i_1 , angle_d_1 , 0);
            //		float aaaaaaa=3;//3.6;//2;//加速度
            //		float zuixiaosudu=18;//18;//还是 走路径 规划 的最小速度

            if (path_l > 1) // 韩秋实
            {
                Dribble_ball.move2Control.initializing_movement(0, 0, 0); //
                Dribble_ball.move2Control.zs_angle_track_001(tfa, 0 /*benchmark_annngle*/, black_white_ball);
                float path_long = (path_l - 2) * 5.0 + zuixiaosudu;
                if (path_long > (lujingchang + aaaaaaa))
                    path_long = lujingchang + aaaaaaa;
                if (path_long < (lujingchang - aaaaaaa * 1.4))
                    path_long = lujingchang - aaaaaaa * 1.4;
                lujingchang = path_long;
                Dribble_ball.move2Control.front_v_c = lujingchang;
                Dribble_ball.move2Control.front_v_angle_c = Path_LA;
                Dribble_ball.move2Control.front_v_maxlimit();
                Dribble_ball.move2Control.front_a_maxlimit();
                Dribble_ball.move2Control.uptech_output_O(&mb, &ml, &mr);
            }
            else
            {
                Dribble_ball.move2Control.limit_parameter_set_dis(
                    zuixiaosudu, 8); //(zs , 10);//(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
                Dribble_ball.move2Control.initializing_movement(0, 0, 0); //
                Dribble_ball.move2Control.zs_angle_track_001(tfa, 0 /*benchmark_angle*/, black_white_ball);
                float path_long = lr * 0.6;
                //			if (path_long>(lujingchang+aaaaaaa))
                lujingchang = path_long;
                Dribble_ball.move2Control.front_v_c = lujingchang;
                Dribble_ball.move2Control.front_v_angle_c = la;
                Dribble_ball.move2Control.front_v_maxlimit();
                //	Dribble_ball.move2Control.front_a_maxlimit();
                Dribble_ball.move2Control.uptech_output_O(&mb, &ml, &mr);
            }
            float p = 1;
            mtb = mb * p;
            mtl = ml * p;
            mtr = mr * p; // 设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000
                          // = 2转/秒
            motospeed[0] = mtl;
            motospeed[1] = mtr;
            motospeed[2] = mtb;
            zhuan = 0; // 鑫鑫程序里的转动变量设置成0

            net.SetDestToS(tfx, tfy);
        }
    }
    else
    {
        if (lujingchang > 0)
            lujingchang -= 2;
        if (lujingchang < 0)
            lujingchang += 2;
        Dribble_ball.move2Control.front_v_p = 0;
    }

    //	if(y.ball_zhong<5)
    //		{
    //			m_pCmd->SetFourMotorsSpeed(0,0,0,0);
    //		}
    //	y.ddoor_jiao=zhangaijiao;

    //	net.SendtoAll((char*)&m_status,sizeof(m_status));
    /*
        if (net.targetx > -200 && net.targety > -200)
        {
            m_find.jizuobiao(net.targetx,net.targety,&dingweijiao,&dingweichang,y.benjix,y.benjiy);
        }
        else
        {
            m_find.jizuobiao(600,600,&dingweijiao,&dingweichang,y.benjix,y.benjiy);
        }
        //dingweijiao=-dingweijiao+y.benjijiao+180;
        //	m_find.jizuobiao(500,500,&dingweijiao,&dingweichang,600,600);
            dingweijiao=-jiajiao(-dingweijiao,y.benjijiao-90);
        CString str11;
        //	dingweijiao=jiajiao(dingweijiao,-90);
        str11.Format("%5f",dingweijiao);
        //	str11.Format("%5f",y.benjijiao);
        //	AfxMessageBox(str11);
        zhuan=duizhun(dingweijiao,0,500);
        if(dingweichang>100)
        Controlmoto(-dingweijiao,dingweichang*2+100,1000,&mtl,&mtr,&mtb);
        else
        Controlmoto(-dingweijiao,dingweichang*0.1,1000,&mtl,&mtr,&mtb);

      */

    chang_shang = y.ball_chang;
    zhuan_shang = zhuan;

    //	mtl=0;
    //	mtb=0;
    //	mtr=0;
    //	zhuan=duizhun(y.ddoor_zuo+5,0,500);
    mtl += zhuan;
    mtr += zhuan;
    mtb += zhuan;

    //	mtl=500;
    // mtr=-500;
    //	mtb=0;
    //	jiasudu(&mtl,&mtr,&mtb,&moto_l,&moto_r,&moto_b,800);
    xianzhi(&mtl, &mtr, &mtb, 20000);
    //	Controlmoto();

    /*
    QueryPerformanceCounter(&CountEnd);
    dfElapseMS=(double)((double)(CountEnd.QuadPart-CountStart.QuadPart+10)/(double)Frequency.QuadPart)*1000.0;

    *///********************time

    // mtb=mtb*netmove;
    // mtl=mtl*netmove;
    // mtr=mtr*netmove;

    //	mtb=200;mtl=210;mtr=00;
    pingjun = (ms_shang[0] + ms_shang[1] + ms_shang[2] + ms_shang[3] + ms_shang[4] + ms_shang[5] + ms_shang[6] +
               ms_shang[7] + ms_shang[8] + ms_shang[9]) /
              10.0;

    //	xingwei=101;//防守 追球

    if (xingwei == 101) // 防守 追球
    {
        if (valxx != 1)
        {
            Dribble_ball.fount_xVset = 0;
            Dribble_ball.fount_yVset = 0;
        }

        mtl = 0;
        mtr = 0;
        mtb = 0;
        float mb, ml, mr;
        if (1) //(y.ball_zhong>50
               //	   (Dribble_ball.ball_op_Fr>(Dribble_ball.holdballFmin+1.0))
               //	   ||(abs(Dribble_ball.ball_op_Fa)>5)
               //	   ||(Dribble_ball.FHaveBall!=1)//(abs(Dribble_ball.ball_op_Fweight)<1000)
               //	   ) //||(1)//(1)//应该再加一个球的大小前景  如果球的距离太远(0)//
        {
            target_num = 1; // 设置路径规划目的地为 球	约定 traget_numC号 初始化值0   1球 2敌方门 3己方门  4以上其他点
            use_side_line = 0;     //	约定use_side_line号 为1 路径规划中加入边线信息  !=1时无边线信息
            add_oppo_radius = 8.0; // 规避半径初始化6.5 8.0
            if (ball_radius < 60)
            {
                add_oppo_radius = sqrt(ball_radius);
            }
            if (m_pAD[0] > 1200 || m_pAD[1] > 1200)
            {
                Dribble_ball.fount_xVset = 0;
                Dribble_ball.fount_yVset = 0;
                Dribble_ball.Ball_Into_Holder4(Dribble_ball.ball_estimate_angle, Dribble_ball.ball_estimate_radius,
                                               Dribble_ball.ball_estimate_F_angle, Dribble_ball.ball_estimate_F_radius);
                Dribble_ball.move_test_C.uptech_output_O(&mb, &ml, &mr); // 对Ball_Into_Holder进行输出
            }
            Dribble_ball.Ball_Track3(&mb, &ml, &mr);
            //		Debug_D[15].Draw_vp(Dribble_ball.view_add_x		,160  ,100.0  ,valxx,255,0,255,
            // pBuffer);//显示参考线 		Debug_D[16].Draw_vp(Dribble_ball.view_add_y     ,320  ,100.0
            // ,valxx,255,0,255, pBuffer);//显示参考线
            Dribble_ball.Dribble_ball_other_moment_flag = 1; // 先运行了其他程序再运行带球函数标志
        }
        float p = 1.0; // 0;//
        mtb = mb * p;
        mtl = ml * p;
        mtr = mr *
              p; // 设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000 = 2转/秒
        motospeed[0] = mtl; // 0;//
        motospeed[1] = mtr; // 0;//
        motospeed[2] = mtb; // 0;//
        motospeed[3] = 6000;
        0; //
        motospeed[4] = -6000;
        0; //

        zhuan = 0; // 鑫鑫程序里的转动变量设置成0
    }
    if (xingwei == 201) // 防守 追球
    {
        mtl = 0;
        mtr = 0;
        mtb = 0;
        float mb, ml, mr;
        pp1.path_plan_start_angle_cost = 1.5; // 2.4;
        pp1.Obstacle_size_set =
            Obstacle_Parameter / field_zoom_time; // 8.2/0.4=20  实验室场地 20的障碍设置大小  设置每个障碍点的影响范围
        pp1.datain(y.benjix, y.benjiy, y.quanbx,
                   y.quanby); // 输入路径规划起始点和终点 y.benjix,y.benjiy起点  y.quanbx,y.quanby终点
        pp1.Ppgrow();
        Path_LA = angle_add_transfer(-pp1.path_plan_angle,
                                     y.benjijiao); // aaaa是路径规划结果方向 前方0度 右正左负 顺时针坐标系
        if (diuqiuleijia > 300)                    //(
        {
            char black_white_ball[20];
            float angle_p_1 = 0.19;    // 0.180*ax;
            float angle_i_1 = 0;       // 0.3*ax;//0.0002*ax;//0.005*ax;  //0.06*ax;//0;
            float angle_d_1 = -0.0070; //-0.0070*ax;//
            Dribble_ball.move2Control.limit_parameter_set_angle(
                10.0, 1); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
            Dribble_ball.move2Control.limit_parameter_set_dis(
                50.0, 15); //(21,1);//3(128,328);//float velocity_limit_,float acceleration_limit_
            Dribble_ball.move2Control.parameter_set_angle(angle_p_1, angle_i_1, angle_d_1, 0);
            Dribble_ball.move2Control.initializing_movement(0, 0, 0); //
            Dribble_ball.move2Control.zs_angle_track_001(-y.ball_jiao, 0 /*benchmark_angle*/, black_white_ball);
            Dribble_ball.move2Control.front_v_c = 25;
            Dribble_ball.move2Control.front_v_angle_c = Path_LA;
            Dribble_ball.move2Control.front_a_maxlimit();
            Dribble_ball.move2Control.uptech_output_O(&mb, &ml, &mr);
        }
        float p = 1;
        mtb = mb * p;
        mtl = ml * p;
        mtr = mr *
              p; // 设置平移速度 每个电机的变量 //严重注意 电机速度 和发的速度 不是 1000/1  是2000/2  即发2000 = 2转/秒
        motospeed[0] = mtl;
        motospeed[1] = mtr;
        motospeed[2] = mtb;
        zhuan = 0; // 鑫鑫程序里的转动变量设置成0
    }

    // 注意构造函数里要用void int_zijian(float f_x,float f_y,float f_a);初始化历史记录
    motor_a_limit(mtb, mtl, mtr); // cs使用加速度限制 预测 机体电机轴速 结果存储在mbR,mlR,mrR,里
    // SQd//路程定位 10贞的累计路程
    // SQv//视觉 10贞的累计路程
    // SQ //视觉路程和 路程定位路程 的差
    // SQa//视觉 角度10贞的累计角度差
    // SQc//视觉 位置10贞的累计距离差
    float SQc, SQa, SQd, SQv, SQ;
    zijian(mbR, mlR, mrR, y.benjix, y.benjiy, y.benjijiao, &SQd, &SQv, &SQ, &SQa, &SQc);

    if (xingwei == 1 && y.ball_chang <= 35)
    {
        net.toS_ballx = y.benjix;
        net.toS_bally = y.benjiy;
    }

    QueryPerformanceCounter(&CountEnd);
    dfElapseMSend =
        (double)((double)(CountEnd.QuadPart - CountStart.QuadPart + 10) / (double)Frequency.QuadPart) * 1000.0;

    shijiandian++;
    if (shijiandian >= 1)
        shijian += dfElapseMSend;

    dianjijiexi(m_pMotorPos[0], m_pMotorPos[1], m_pMotorPos[2], mapan_s[0], mapan_s[1], mapan_s[2], &ycjiao, &ycchang,
                &yczhuan);
    leijia++;

    /*
    if(leijia<30)
    {
        benjix1=y.benjix;
        benjiy1=y.benjiy;
        benjijiao1=y.benjijiao;
    }
    else if(leijia<500)
    {
        m_find.zhizuobiao()
    }
    */
    Dribble_ball.motor_history(
        mtb, mtl,
        mtr); // 包括 motor_a_limit的功能 且作历史记录使用加速度限制 预测 机体电机轴速  在周期末电机输出的位置使用
    // 必须在本周期最后电机输出处使用!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    Dribble_ball.Save_Moment_State(mtb, mtl, mtr); // 通过电机参数 保存本周期最后输出的运动矢量
    short motospeedtemp[5];
    if (0) // xingwei==1)
    {
        mtl = 12000;
        mtb = 12000;
        mtr = 12000;
    }

    if (xingwei == 11)
    {
        mtl = 0;
        mtr = 0;
        mtb = 0;
    }

    // 保护场地边界 减速程序  机体速度从边界1倍速 在到达save_film_line_slow_down_dis设置 距离时
    // 减为0.2倍速
    float sflsd_dis = 400; // 50;//save_film_line_slow_down_dis=50;//suyi 越大越快
    float ppt = 1.0;
    float ppt2 = 1.0;
    if (y.benjix < 0)
        ppt = (y.benjix + sflsd_dis) / sflsd_dis;
    if (y.benjix > 1800)
        ppt = (-y.benjix + 1800 + sflsd_dis) / sflsd_dis;
    if (y.benjiy < 0)
        ppt2 = (y.benjiy + sflsd_dis) / sflsd_dis;
    if (y.benjiy > 1200)
        ppt2 = (-y.benjiy + 1200 + sflsd_dis) / sflsd_dis;

    if (fabs(ppt) > fabs(ppt2)) //()
        ppt = ppt2;

    if (ppt < 0)
        ppt = 0;
    if (ppt > 1)
        ppt = 1;
    float pp = ppt * 0.9 + 0.1;
    //		if (ppt==0)
    //			pp=0.01;
    //		const float pp=1;//0.8;//0;//0.2;//ppt*0.8+0.2;//1.0;//设置电机启停

    // int shangzhenzuida;
    // int dangqianzuida;

    // dangqianzuida=m_find.Max(mtl*pp,mtr*pp,mtb*pp);
    // shangzhenzuida=m_find.Max(motol_as,motor_as,motob_as);
    // if(abs(dangqianzuida)-abs(shangzhenzuida))
    float moto_acc_enab = 0;
    switch (net.rec_ctrl) // 2014 根据原始行为信息控制全局加速度控制使能
    {
    case CTRL_STOP: // xingwei=11; //停止
        moto_acc_enab = 150;
        break;
    case CTRL_ATTACK: // xingwei=1;//开始//1002;//1;//1002;//
        moto_acc_enab = 200;
        break;
    case CTRL_GOALKEEP: // xingwei=2; //守门
        moto_acc_enab = 240;
        break;

        // KickOff
    case CTRL_KICKOFF_PRIM_READY: // xingwei=4; //主动开球跑位
        break;
    case CTRL_KICKOFF_SLAVE_READY: // xingwei=9; //辅助开球跑位
        break;
    case CTRL_KICKOFF_PRIM: // xingwei=13; //主动开球开始动作
        break;
    case CTRL_KICKOFF_SLAVE: // xingwei=10;  //辅助开球开始动作
        break;
        // 防守
    case CTRL_PRODEF: // xingwei=5; //被开球跑位
        break;
    case CTRL_ATTENTIONBALL: // xingwei=6; //被开球开始动作
        break;
    case CTRL_SEARCHBALL: // xingwei=14; //全场寻球
        break;
    case CTRL_DEFENCE:     // xingwei=107;//12;
                           // //防守状态//定角度防守(net.rec_Def_Dist为离球距离，net.rec_Def_Ang为防守角度)
        moto_acc_enab = 0; // 此动作不使用全局加速度限制
        break;
    case CTRL_MOVETO: // xingwei=7; //全场跑位
        moto_acc_enab = 0;
        //	moto_acc_enab=200;
        break;

    case 30: // xingwei=117; //DEF跑位
        break;
    case CTRL_PENALTY: // xingwei=22;	//点球
        break;
    case CTRL_PASS:        // xingwei=10;//传球：（net.targetx，net.targety）为接球者的坐标，供参考
        moto_acc_enab = 0; // 此动作不使用全局加速度限制
        break;
    case CTRL_CATCH:       // xingwei=30;	//接球：（net.rec_fromx，net.rec_fromy）为传球者的坐标，供参考
        moto_acc_enab = 0; // 此动作不使用全局加速度限制
        break;
    case CTRL_PASS_MOVE: // xingwei=1001;//3000;	//传球运动
        moto_acc_enab = 0;
        break;
    case CTRL_CATCH_MOVE: // xingwei=1002;//接球运动1002    1003
        moto_acc_enab = 0;
        break;
    case CTRL_CATCH_FOCUS: // xingwei=1004;
        moto_acc_enab = 0; // 此动作不使用全局加速度限制
        break;

    case CTRL_FOLLOW: // xingwei=1127;
        moto_acc_enab = 0;
        break;
    case CTRL_DEF_BALL: // xingwei=1137;
        moto_acc_enab = 0;
        break;
    case CTRL_DEF_GOAL: // xingwei=1147;
        moto_acc_enab = 0;
        break;
    }
    if ((xingwei == 7) || (xingwei == 30))
    {
        moto_acc_enab = 0;
    }
    if (motospeed[3] - xiqiuz_s > 100)
        motospeed[3] = xiqiuz_s + 100;
    if (motospeed[3] - xiqiuz_s < -100)
        motospeed[3] = xiqiuz_s - 100;

    if (motospeed[4] - xiqiuy_s > 100)
        motospeed[4] = xiqiuy_s + 100;
    if (motospeed[4] - xiqiuy_s < -100)
        motospeed[4] = xiqiuy_s - 100;

    xiqiuz_s = motospeed[3];
    xiqiuy_s = motospeed[4];
    // moto_acc_enab=0;
    if (moto_acc_enab > 0)
    {
        float qcjiasd = moto_acc_enab;
        float qcjiansd = moto_acc_enab;
        float maxjsd;
        float midjsd;
        float qcjiasdl = qcjiasd;
        float qcjiasdr = qcjiasd;
        float qcjiasdb = qcjiasd;
        float qcjiansdl = qcjiansd;
        float qcjiansdr = qcjiansd;
        float qcjiansdb = qcjiansd;
        mtl1 = mtl;
        mtr1 = mtr;
        mtb1 = mtb;

        /*
        if (1)
        {
            float front_a,front_v,turn_w;
            float front_as,front_vs,turn_ws;
            double movea,movev,movez;
            Dribble_ball.O_movement_T_analyze(&front_a,&front_v,&turn_w,-mtl,-mtb,-mtr);//    *move_v_angle,
        *move_v_speed, *turn_v
            Dribble_ball.O_movement_T_analyze(&front_as,&front_vs,&turn_ws,-motol_as,-motob_as,-motor_as);//
        *move_v_angle, *move_v_speed, *turn_v mtl=0; mtr=0; mtb=0;
            vhecheng(front_as,front_vs,20000,front_a,qcjiasd,20000,&movea,&movev);
            Controlmoto(-movea,movev,20000,&mtl,&mtr,&mtb);
            mtl+=turn_w;
            mtr+=turn_w;
            mtb+=turn_w;
        }
        */
        float front_a, front_v, turn_w;
        float front_as, front_vs, turn_ws;
        double movea, movev, movez;

        Dribble_ball.O_movement_T_analyze(&front_a, &front_v, &turn_w, -mtl, -mtb,
                                          -mtr); //    *move_v_angle, *move_v_speed, *turn_v
        Dribble_ball.O_movement_T_analyze(&front_as, &front_vs, &turn_ws, -motol_as, -motob_as,
                                          -motor_as); //    *move_v_angle, *move_v_speed, *turn_v

        /*
        if(abs(turn_w-turn_ws)>300)
        {
            qcjiasd+=300;
            qcjiansd+=300;
        }
        else
        {
            qcjiasd+=abs(turn_w-turn_ws);
            qcjiansd+=abs(turn_w-turn_ws);
        }
        */
        //	vhecheng(0,100,1000,1,100,1000,&movea,&movev);
        // if(leijiaqi%20==0)
        //	strShow.Format("a1:%5.1f,a2:%5.1f,v1:%5.1f,v2:%5.1f,a3:%5.1f,v3:%5.1f",front_a,front_as,front_v,front_vs,movea,movev);

        maxjsd = m_find.Max(abs(mtl - motol_as), abs(mtr - motor_as), abs(mtb - motob_as));
        midjsd = (abs(mtl - motol_as) + abs(mtr - motor_as) + abs(mtb - motob_as)) / 3;
        if (maxjsd != 0)
        {
            qcjiasdl = qcjiasd * abs(mtl - motol_as) / midjsd;
            qcjiasdr = qcjiasd * abs(mtr - motor_as) / midjsd;
            qcjiasdb = qcjiasd * abs(mtb - motob_as) / midjsd;

            qcjiansdl = qcjiansd * abs(mtl - motol_as) / midjsd;
            qcjiansdr = qcjiansd * abs(mtr - motor_as) / midjsd;
            qcjiansdb = qcjiansd * abs(mtb - motob_as) / midjsd;
        }

        if (mtl >= 0 && (mtl - motol_as >= qcjiasdl))
            mtl = motol_as + qcjiasdl;
        else if (mtl >= 0 && mtl - motol_as <= -qcjiansdl)
            mtl = motol_as - qcjiansdl;
        else if (mtl < 0 && mtl - motol_as < -qcjiasdl)
            mtl = motol_as - qcjiasdl;
        else if (mtl < 0 && mtl - motol_as > qcjiansdl)
            mtl = motol_as + qcjiansdl;

        if (mtr >= 0 && (mtr - motor_as >= qcjiasdr))
            mtr = motor_as + qcjiasdr;
        else if (mtr >= 0 && mtr - motor_as <= -qcjiansdr)
            mtr = motor_as - qcjiansdr;
        else if (mtr < 0 && mtr - motor_as < -qcjiasdr)
            mtr = motor_as - qcjiasdr;
        else if (mtr < 0 && mtr - motor_as > qcjiansdr)
            mtr = motor_as + qcjiansdr;

        if (mtb >= 0 && mtb - motob_as >= qcjiasdb)
            mtb = motob_as + qcjiasdb;
        else if (mtb >= 0 && mtb - motob_as <= -qcjiansdb)
            mtb = motob_as - qcjiansdb;
        else if (mtb < 0 && mtb - motob_as < -qcjiasdb)
            mtb = motob_as - qcjiasdb;
        else if (mtb < 0 && mtb - motob_as > qcjiansdb)
            mtb = motob_as + qcjiansdb;
    }

    mtl = mtl * pp;
    mtr = mtr * pp;
    mtb = mtb * pp;
    motospeed[0] = mtl;
    motospeed[1] = mtr;
    motospeed[2] = mtb;

    if (valxx == 1 && xingwei != 11 && kaijipd > 111) // 450)// )// && y.ball_zhong>20)
    {
        ///*

        //		xiqiujiexi(motospeed[0],motospeed[1],motospeed[2],0.033333,&motospeed[3],&motospeed[4]);
        //*/
        if (xingwei_1_to_107_flag == 1)
        {
            pp = 1;
        }

        motospeedtemp[0] = double(motospeed[0]) / DIANJI;
        motospeedtemp[1] = double(motospeed[1]) / DIANJI;
        motospeedtemp[2] = double(motospeed[2]) / DIANJI;
        motospeedtemp[3] = double(motospeed[3]) / DIANJI;
        motospeedtemp[4] = double(motospeed[4]) / DIANJI;
#ifdef NO_MOVE
        motospeedtemp[0] = 0;
        motospeedtemp[1] = 0;
        motospeedtemp[2] = 0;
        motospeedtemp[3] = 0;
        motospeedtemp[4] = 0;
#endif
        if (motospeedtemp[3] > 2000)
            motospeedtemp[3] = 2000;
        if (motospeedtemp[3] < -2000)
            motospeedtemp[3] < -2000;
        if (motospeedtemp[4] > 2000)
            motospeedtemp[4] = 2000;
        if (motospeedtemp[4] < -2000)
            motospeedtemp[4] < -2000;

        pRobot->SetFiveMotorsSpeed(motospeedtemp);
        // m_find.pipei()=10;

        //		strShow.Format("x=%10.1f,估:%10.1f,y=%10.1f,估:%10.1f,速度=%10.1f,估:%10.1f,时间:%10.0f\n",y.quanbx,xp,y.quanby,yp,suduxx1,suduxx1,ms_shang1/1000);

        motospeedP[0] = motospeed[0];
        motospeedP[1] = motospeed[1];
        motospeedP[2] = motospeed[2];
        motospeedP[3] = motospeed[3];
        motospeedP[4] = motospeed[4];
        stopvalxx = 1;
    }
    else
    {
        ycjiao = 0;
        ycchang = 0;
        yczhuan = 0;
        jifen = 0;
        //	m_pCmd->SetFourMotorsSpeed(0,0,0,0);
        //		motospeed[0]=motospeedP[0]*stopvalxx;
        //		motospeed[1]=motospeedP[1]*stopvalxx;
        //		motospeed[2]=motospeedP[2]*stopvalxx;
        motospeed[3] = 0;
        motospeed[4] = 0;
        stopvalxx -= 0.012; // 0.08;//
        if (stopvalxx < 0)
            stopvalxx = 0;
        motospeedtemp[0] = motospeed[0] / DIANJI;
        motospeedtemp[1] = motospeed[1] / DIANJI;
        motospeedtemp[2] = motospeed[2] / DIANJI;
        motospeedtemp[3] = motospeed[3];
        motospeedtemp[4] = motospeed[4];

#ifdef NO_MOVE
        motospeedtemp[0] = 0;
        motospeedtemp[1] = 0;
        motospeedtemp[2] = 0;
        motospeedtemp[3] = 0;
        motospeedtemp[4] = 0;
#endif
        if (motospeedtemp[3] > 2000)
            motospeedtemp[3] = 2000;
        if (motospeedtemp[3] < -2000)
            motospeedtemp[3] < -2000;
        if (motospeedtemp[4] > 2000)
            motospeedtemp[4] = 2000;
        if (motospeedtemp[4] < -2000)
            motospeedtemp[4] < -2000;

        pRobot->SetFiveMotorsSpeed(motospeedtemp);
    }
    motol_as = motospeed[0]; // 记录当前周期最后电机速度 用来下周期参考控制加速度
    motor_as = motospeed[1];
    motob_as = motospeed[2];

    for (i = 100 - 1; i > 0; i--) // 上100帧周期电机发送数据历史记录
    {
        motospeedtemp_H[i][0] = motospeedtemp_H[i - 1][0];
        motospeedtemp_H[i][1] = motospeedtemp_H[i - 1][1];
        motospeedtemp_H[i][2] = motospeedtemp_H[i - 1][2];
    }
    motospeedtemp_H[0][0] = motospeedtemp[0];
    motospeedtemp_H[0][1] = motospeedtemp[1];
    motospeedtemp_H[0][2] = motospeedtemp[2];
#ifdef RUN_WITH_EMU
    // 仿真程序输出数据到环境服务器
    targAdrlen = sizeof(SOCKADDR);
    targAdr.sin_family = AF_INET;
    targAdr.sin_port = htons(13000);                                   // htons();                  ///target 的监听端口
    targAdr.sin_addr.s_addr = inet_addr("127.0.0.1");                  //("192.168.11.1"); ///target 的地址
    sendToServerData.serverReceiveEMU_motospeed[0] = motospeedtemp[0]; // mtl/DIANJI;//=1000;//
    sendToServerData.serverReceiveEMU_motospeed[1] = motospeedtemp[2]; // mtb/DIANJI;//=0;//
    sendToServerData.serverReceiveEMU_motospeed[2] = motospeedtemp[1]; // mtr/DIANJI;//=-1000;//
    sendto(m_send_EMU_server_skt, (char *)&sendToServerData, sizeof(serverReceiveStruct), 0, (SOCKADDR *)&targAdr,
           targAdrlen);
// 仿真程序输出数据到环境服务器结束
#endif
    nVisionFps = 1 / pingjun * 1000;
    if (leijiaqi % 20 == 0)
    {
        //		strShow.Format("shemen%d xx%d sct%d %4.2f %5d %5d %5d st%d  GYRO可用%d Gia%f %f %d %d bx%f by%f bc%f
        // 门长:%5d  时间:%10.4f,
        // 球角=%9.4f,球长=%9.4f,球重=%5.0f,门角%5d,门长%5.1f,夹角=%5d,本机角:%6.1f,B=%6d,L=%6d,R=%6d,转=%6d,状态:%s,时零%3.2f,时一%3.2f,时二%3.2f,时三%3.2f,时四%3.2f,时五%3.2f,时六%3.2f,耗时end:%3.2f
        // ms,帧率:%7.4f ms,本机x%5.1f,y%5.1f,匹配距离%7.1f,行为=%d\n 周期时间%3.1f 极坐标a%d d%d 上电%d 白点数 %d "//
        // %f 		,shemenleijia,valxx,shoot_cool_time
        //,robot__ball_radius_min,m_pMotorPos_V[0],m_pMotorPos_V[1],m_pMotorPos_V[2],shoottime,GYRO_available,GYRO_init_angle,move_Emu_robot_f_a,xingwei_catch_timer,xingwei_org,Display_A,Display_B,Display_C,dmenchang,ms_shang1,y.ball_jiao,y.ball_chang,y.ball_zhong,int(y.ddoor_jiao),y.ddoor_chang,jiajiao(y.ball_jiao,y.ddoor_jiao),y.benjijiao,mtb,mtl,mtr,zhuan,zhuangtai,dfElapseMS,dfElapseMS1,dfElapseMS2,dfElapseMS3,dfElapseMS4,dfElapseMS5,dfElapseMS6,dfElapseMSend,1/pingjun*1000,y.benjix,y.benjiy,pipeijuli,xingwei,LOOPTIME,net.rec_Def_Ang,net.rec_Def_Dist,kaijipd,xiandianshu);//,l_x,l_y,l_a
        //
        //		strShow.Format("A=%f B=%f C=%f
        //%d,%d,%d",Display_A,Display_B,Display_C,net.re_ballx,net.re_bally,diuqiuleijia); 	strShow.Format("4 xingwei
        //%d,x %d,y %d",xingwei,net.rec_targetx,net.rec_targety);
        //		strShow.Format("门长:%5d , 预测角:%10.5f, 预测速度:%10.5f, 预测转:%10.5f",ycjiao,ycchang,yczhuan);
        //	strShow.Format("y%f p%f r%f %f %d %d bx%f by%f bc%f 门长:%5d ,SQc%10.1f SQa%10.1f SQd%10.1f SQv%10.1f
        // SQ%10.1f 时间:%10.4f,
        // 球角=%9.4f,球长=%9.4f,球重=%5.0f,门角%5d,门长%5.1f,夹角=%5d,本机角:%6.1f,B=%6d,L=%6d,R=%6d,转=%6d,状态:%s,耗时一:%9.4f
        // ms,耗时二:%7.4f ms,耗时三:%7.4f ms,耗时:%7.4f ms,帧率:%7.4f ms,本机x%5.1f,y%5.1f,匹配距离%7.1f,行为=%d\n
        // 周期时间%3.1f 极坐标a%d d%d 上电%d 白点数 %d "// %f
        //		,ypr[0]*180.0/3.14,ypr[1],ypr[2],move_Emu_robot_f_a,xingwei_catch_timer,xingwei_org,Display_A,Display_B,Display_C,dmenchang,SQc,SQa,SQd,SQv,SQ,ms_shang1,y.ball_jiao,y.ball_chang,y.ball_zhong,int(y.ddoor_jiao),y.ddoor_chang,jiajiao(y.ball_jiao,y.ddoor_jiao),y.benjijiao,mtb,mtl,mtr,zhuan,zhuangtai,dfElapseMS,dfElapseMS1,dfElapseMS2,dfElapseMSend,1/pingjun*1000,y.benjix,y.benjiy,pipeijuli,xingwei,LOOPTIME,net.rec_Def_Ang,net.rec_Def_Dist,kaijipd,xiandianshu);//,l_x,l_y,l_a
        //		strShow.Format("y%f %f %d %d bx%f by%f bc%f 门长:%5d ,SQc%10.1f SQa%10.1f SQd%10.1f SQv%10.1f SQ%10.1f
        // 时间:%10.4f,
        // 球角=%9.4f,球长=%9.4f,球重=%5.0f,门角%5d,门长%5.1f,夹角=%5d,本机角:%6.1f,B=%6d,L=%6d,R=%6d,转=%6d,状态:%s,时零%3.2f,时一%3.2f,时二%3.2f,时三%3.2f,时四%3.2f,时五%3.2f,时六%3.2f,耗时end:%3.2f
        // ms,帧率:%7.4f ms,本机x%5.1f,y%5.1f,匹配距离%7.1f,行为=%d\n 周期时间%3.1f 极坐标a%d d%d 上电%d 白点数 %d "//
        // %f
        //			,ypr[0]*180.0/3.14,move_Emu_robot_f_a,xingwei_catch_timer,xingwei_org,Display_A,Display_B,Display_C,dmenchang,SQc,SQa,SQd,SQv,SQ,ms_shang1,y.ball_jiao,y.ball_chang,y.ball_zhong,int(y.ddoor_jiao),y.ddoor_chang,jiajiao(y.ball_jiao,y.ddoor_jiao),y.benjijiao,mtb,mtl,mtr,zhuan,zhuangtai,dfElapseMS,dfElapseMS1,dfElapseMS2,dfElapseMS3,dfElapseMS4,dfElapseMS5,dfElapseMS6,dfElapseMSend,1/pingjun*1000,y.benjix,y.benjiy,pipeijuli,xingwei,LOOPTIME,net.rec_Def_Ang,net.rec_Def_Dist,kaijipd,xiandianshu);//,l_x,l_y,l_a

        // strShow.Format("x=%d y=%d a=%d	NetBx=%d NetBy=%d	mv=%f rv=%f xingwei=%d panding=%f x=%f y=%f d=%f dif=%f
        // lujingchang=%f",net.rec_CFtargetx , net.rec_CFtargety ,
        // net.rec_CFtargetang,DebugV[6],net.re_ballx,net.re_bally,DebugV[5],xingwei,DebugV[4],DebugV[0],DebugV[1],DebugV[2],DebugV[3],lujingchang);

        //		strShow.Format("dct%d	zhuan=%f	jifen=%f	p1=%f	p2=%f	p3=%f	x=%d,%f	y=%d,%f	z=%d,%f	B=%d
        //(0)%d(1)%d(2)%d(3)%d(4)%d(5)%d(6)%d(7)%d(8)%d(9)%d(10)%d(11)%d(12)%d(13)%d(14)%d(15)%d	fps%f"
        //						,daiqiumb_change_timer,test_joy_max_zhuan,test_joy_max_jifen,catch_dif,catch_zoom,catch_3
        //						,JOY_Xpos ,JOY_XposF, JOY_Ypos ,JOY_YposF, JOY_Zpos ,JOY_ZposF, JOY_Buttons
        //						,JOY_Button[0],JOY_Button[1],JOY_Button[2],JOY_Button[3],JOY_Button[4],JOY_Button[5],JOY_Button[6],JOY_Button[7],JOY_Button[8],JOY_Button[9],JOY_Button[10],JOY_Button[11],JOY_Button[12],JOY_Button[13],JOY_Button[14],JOY_Button[15]
        //						,1/pingjun*1000);

        // strShow.Format("bNoReply=%d kaijipd=%d",m_pCmd->bNoReply,kaijipd);
        // strShow.Format("ma pan X=%f Y=%f A=%f
        // angle_proportion_USE=%f",Dribble_ball.robot_L_test.f_x,Dribble_ball.robot_L_test.f_x,Dribble_ball.robot_L_test.f_a,angle_proportion_USE);

        //			,jieqiuys,Display_A,Display_B,Display_C,dmenchang,SQc,SQa,SQd,SQv,SQ,ms_shang1,y.ball_jiao,y.ball_chang,y.ball_zhong,int(y.ddoor_jiao),y.ddoor_chang,jiajiao(y.ball_jiao,y.ddoor_jiao),y.benjijiao,mtb,mtl,mtr,zhuan,zhuangtai,dfElapseMS,dfElapseMS1,dfElapseMS2,dfElapseMSend,1/pingjun*1000,y.benjix,y.benjiy,pipeijuli,xingwei,LOOPTIME,net.rec_Def_Ang,net.rec_Def_Dist,kaijipd,xiandianshu);//,l_x,l_y,l_a
        //		strShow.Format("%d",m_find.qijiaopanding(y,1));
        //*net.rec_Def_Dist 防守时机器人距离球的距离，单位：分米*//
        //*net.rec_Def_Ang 防守队员-球-己方球门的夹角，球为夹角顶点，范围:-180~180*//

        //			strShow.Format("角%5.1f,速度%5.1f,转%5.1f,累加距离%f,累加角度%f",ycjiao,ycchang,yczhuan,leijia,leijia1);
        //				strShow.Format("l=%8d,l=%8d,r=%8d,r=%8d,b=%8d,b=%8d",mtl1,mtl,mtr1,mtr,mtb1,mtb);
        //			strShow.Format("x=%5.1f,%5.1f,      y=%5.1f,%5.1f,
        // j=%5.1f,%5.1f,",y.benjix,m_find.ycx,y.benjiy,m_find.ycy,y.benjijiao,m_find.ycj);
        // strShow.Format("%5.1f,%5.1f, %5.1f,%5.1f",y.ball_x,ballx_s1,y.ball_y,bally_s1);

        int xxa;
        int yya;
        xxa = m_pAD[1];
        yya = m_pAD[0];
        //	strShow.Format("%d,%d %f",xxa,yya,test_c_W);
        // write.write(strShow);
    }
    // strShow.Format("jiao=%10f,chang=%10f,左电机=%5d,右电机=%5d",y.ball_jiao,y.ball_chang,L_moto1,R_moto1);
    // 显示电机
    // strShow.Format("xxx=%d",m_find.quzhi(10));
    m_pShow->SetWindowText(strShow);
    //	strShow.Format("mPos0 %d mPos1 %d mPos2 %d W %f
    //\n",m_pMotorPos[0],m_pMotorPos[1],m_pMotorPos[2],Dribble_ball.RMw_m); 	write.write(strShow);
    //********************time
    //********************time
    QueryPerformanceCounter(&CountEnd1);
    dfElapseMS4 =
        (double)((double)(CountEnd1.QuadPart - CountStart1.QuadPart + 10) / (double)Frequency1.QuadPart) * 1000.0 -
        ms_shang1;
    //********************time
    QueryPerformanceCounter(&CountEnd1);
    ms_shang1 =
        (double)((double)(CountEnd1.QuadPart - CountStart1.QuadPart + 10) / (double)Frequency1.QuadPart) * 1000.0;
    ms_shang[0] = ms_shang[1];
    ms_shang[1] = ms_shang[2];
    ms_shang[2] = ms_shang[3];
    ms_shang[3] = ms_shang[4];
    ms_shang[4] = ms_shang[5];
    ms_shang[5] = ms_shang[6];
    ms_shang[6] = ms_shang[7];
    ms_shang[7] = ms_shang[8];
    ms_shang[8] = ms_shang[9];
    ms_shang[9] = dfElapseMS4;
    //********************time
    mapan_s[0] = m_pMotorPos[0];
    mapan_s[1] = m_pMotorPos[1];
    mapan_s[2] = m_pMotorPos[2];
    moto_l = motospeed[0];
    Path_LA_P = Path_LA; // 存储路径规划历史数据
    Path_FA_P = Path_FA;

    // 保存所有传感器历史记录 cs
    m_pMotorPos_P[0] = m_pMotorPos[0];
    m_pMotorPos_P[1] = m_pMotorPos[1];
    m_pMotorPos_P[2] = m_pMotorPos[2];

    for (i = 100 - 1; i > 0; i--) // 上100帧周期码盘位置历史记录
    {
        m_pMotorPos_H[i][0] = m_pMotorPos_H[i - 1][0];
        m_pMotorPos_H[i][1] = m_pMotorPos_H[i - 1][1];
        m_pMotorPos_H[i][2] = m_pMotorPos_H[i - 1][2];
    }
    m_pMotorPos_H[0][0] = m_pMotorPos[0];
    m_pMotorPos_H[0][1] = m_pMotorPos[1];
    m_pMotorPos_H[0][2] = m_pMotorPos[2];

    m_pMotorCurrent_P[0] = m_pMotorCurrent[0];
    m_pMotorCurrent_P[1] = m_pMotorCurrent[1];
    m_pMotorCurrent_P[2] = m_pMotorCurrent[2];
    m_pIO_P[0] = m_pIO[0];
    m_pIO_P[1] = m_pIO[1];
    m_pIO_P[2] = m_pIO[2];
    m_pIO_P[3] = m_pIO[3];
    m_pAD_P[0] = m_pAD[0];
    m_pAD_P[1] = m_pAD[0];
    m_pAD_P[2] = m_pAD[1];
    m_pAD_P[3] = m_pAD[0];
    mtl_s1 = mtl;
    mtr_s1 = mtr;
    mtb_s1 = mtb;
    for (i = 100 - 1; i > 0; i--) // 存储上100周期电机的发送转速
    {
        SCDataCount_P[i] = SCDataCount_P[i - 1];
    }
    SCDataCount_P[0] = SCDataCount;

    xingweiP = xingwei;
    Dribble_ball.xingwei_P = xingwei;
    ballx_shang = y.ball_x;
    bally_shang = y.ball_y;
    ballx_s1 = y.ball_x;
    bally_s1 = y.ball_y;
    if (pRobot->bNoReply == true)
        kaijipd -= 10;
    else
        kaijipd++;
    if (kaijipd < 0)
        kaijipd = 0;
    if (kaijipd > 1000)
        kaijipd = 1000;
    benjix_s = y.benjix;
    benjiy_s = y.benjiy;
    benjijiao_s = y.benjijiao;
    jifen_s = jifen;

    if (xingwei_catch_flag == 1)
    { //	if (y.quanbx>(900-50))
        //		net.toS_status =CTRL_ATTACK;
        //	else
        //		net.toS_status =CTRL_CATCH_MOVE;
        net.toS_status = net.rec_ctrl;
    }
    if (xingwei_Passing_flag == 1)
    { //	net.toS_status =CTRL_PASS_MOVE;
        net.toS_status = net.rec_ctrl;
    }
    // Sleep(500);
    //	net.Report();
}

void CGetImage::hecheng(BYTE *pBuffer)
{

    int x1;
    int y1;
    x1 = 267;
    y1 = 250;
    int beishu;
    beishu = 9;
    x1 = x1 * beishu;
    y1 = y1 * beishu;

    for (int x = 0; x < 320; x++)
        for (int y = 0; y < 240; y++)
        {
            if (x % beishu == 0 && y % beishu == 0)
            {
                pBuffer[((y + y1) * 640 + (640 - x + x1)) * 3 / beishu] = tuxiang2[y * 320 * 3 + x * 3];
                pBuffer[((y + y1) * 640 + (640 - x + x1)) * 3 / beishu + 1] = tuxiang2[y * 320 * 3 + x * 3 + 1];
                pBuffer[((y + y1) * 640 + (640 - x + x1)) * 3 / beishu + 2] = tuxiang2[y * 320 * 3 + x * 3 + 2];
            } //[y+y1][320-x+x1]                           [y][x]
        }

    /*
    for(int x=0;x<40;x++)
        for(int y=0;y<30;y++)
        {
        //	if(x%beishu==0 && y%beishu==0)
        //	{
                pBuffer[((y+y1)*640+(640-x+x1))*3]=tuxiang2[(y*320+x)*3];
        //		pBuffer[((y+y1)*640+(640-x+x1))*3+1]=tuxiang2[(y*320+x)*3*8+1];
        //		pBuffer[((y+y1)*640+(640-x+x1))*3+2]=tuxiang2[(y*320+x)*3*8+2];
        //	}//[y+y1][320-x+x1]                           [y][x]
        }
        */
}

void CGetImage::Controlmoto(int jiao, int sudu, int top, int *motol, int *motor, int *motob)
{
    *motol += -int(sin((float(jiajiao(-jiao, -60))) * 3.14 / 180) * sudu);
    *motor += -int(sin((float(jiajiao(-jiao, 60))) * 3.14 / 180) * sudu);
    *motob += -int(sin((float(jiajiao(-jiao, +180))) * 3.14 / 180) * sudu);
    xianzhi(motol, motor, motob, top);
}

int CGetImage::duizhun(double jiao, double jizhun, int top)
{
    double xx;
    double jiao1;
    //	jiao1=-jiao;
    //	jiao1=jiao1+jizhun;
    //	if (jiao1>180 || jiao1<-180)
    //		jiao1=-jiao1-2*jizhun;
    jiao1 = jiajiao(jizhun, jiao);

    if (jiao1 > 70)
        jiao1 = 70;
    if (jiao1 < -70)
        jiao1 = -70;
    if (jiao1 >= 0)
    {
        xx = (jiao1) * (jiao1) / 2 + jiao1 * 15;
        if (xx > top)
            xx = top;
    }
    else
    {
        xx = -(jiao1) * (jiao1) / 2 + jiao1 * 15;
        if (xx < -top)
            xx = -top;
    }

    //	if(xx>0)
    //		xx=xx+100;
    //	if(xx<-0)
    //		xx=xx-100;

    return -xx;
}

void CGetImage::jiasudu(int *motol_x, int *motor_x, int *motob_x, int *motol_y, int *motor_y, int *motob_y, int zhi)
{
    //*motol_x本针需要的速度
    //*motol_y保存的上次速度

    if (*motol_x - *motol_y > zhi)
        *motol_x = *motol_y + zhi;
    if (*motol_x - *motol_y <= -zhi)
        *motol_x = *motol_y - zhi;
    if (*motor_x - *motor_y > zhi)
        *motor_x = *motor_y + zhi;
    if (*motor_x - *motor_y <= -zhi)
        *motor_x = *motor_y - zhi;
    if (*motob_x - *motob_y > zhi)
        *motob_x = *motob_y + zhi;
    if (*motob_x - *motob_y <= -zhi)
        *motob_x = *motob_y - zhi;
    *motol_y = *motol_x;
    *motor_y = *motor_x;
    *motob_y = *motob_x;
}

void CGetImage::xianzhi(int *motol, int *motor, int *motob, int zhi)
{
    int maxx;
    maxx = m_find.Max(abs(*motol), abs(*motor), abs(*motob));
    if (maxx > zhi)
    {
        *motol = float(zhi) / float(maxx) * float(*motol);
        *motor = float(zhi) / float(maxx) * float(*motor);
        *motob = float(zhi) / float(maxx) * float(*motob);
    }
    /*
        if(abs(*motol)>=abs(*motor) && abs(*motol)>=abs(*motob))

        {
            *motol=float(zhi)/float(maxx)*float(*motol);
            *motor=float(zhi)/float(maxx)*float(*motor);
            *motob=float(zhi)/float(maxx)*float(*motob);
        }

        if(abs(*motor)>=abs(*motol) && abs(*motor)>=abs(*motob))

        {
            *motor=float(zhi)/float(maxx)*float(*motor);
            *motol=float(zhi)/float(maxx)*float(*motol);
            *motob=float(zhi)/float(maxx)*float(*motob);
        }
            if(abs(*motob)>=abs(*motor) && abs(*motob)>=abs(*motol))

        {
            *motob=float(zhi)/float(maxx)*float(*motob);
            *motor=float(zhi)/float(maxx)*float(*motor);
            *motol=float(zhi)/float(maxx)*float(*motol);
        }
        */
    /*
     */
}

void CGetImage::lujing(double jiaom, double changm, double *jiao, double *chang)
{
    int pathx;
    double pathjiao;
    int pathy;
    double pathchang;
    int pathx1;
    double pathjiao1;
    int pathy1;
    double pathchang1;
    int pathx2;
    double pathjiao2;
    int pathy2;
    double pathchang2;
    int pathx3;
    double pathjiao3;
    int pathy3;
    double pathchang3;
    double xx;
    double yy;
    double x11;
    double y11;
    double x22;
    double y22;
    double x33;
    double y33;

    //	int x44;
    //	int y44;
    //	int x55;
    //	int y55;

    m_find.zhizuobiao(&x11, &y11, jiaom - 130, 27, 160, 120); // 求从本机到目标物的那两个点
    m_find.zhizuobiao(&x22, &y22, jiaom + 130, 27, 160, 120); //
    m_find.zhizuobiao(&xx, &yy, jiaom, changm, 160, 120);     // 取出目标物的x,y位置
    //	m_find.path_generate(x11,y11,xx,yy,&pathx1,&pathy1);
    //	m_find.path_generate(x22,y22,xx,yy,&pathx2,&pathy2);
    m_find.path_generate(160, 120, xx, yy, &pathx3, &pathy3);
    // 从那两个点找目标点的路径
    //	m_find.jizuobiao(pathx1,pathy1,&pathjiao1,&pathchang1);
    //	m_find.jizuobiao(pathx2,pathy2,&pathjiao2,&pathchang2);
    m_find.jizuobiao(pathx3, pathy3, &pathjiao3, &pathchang3, 160, 120);
    double xva;
    double yva;
    int voll = 0;
    int volr;
    if (pathx3 != 400)
    {
        voll = 0;
        for (int jj = -10; jj < 10; jj++)
        {
            for (int ii = -1; ii < 4; ii++)
            {
                m_find.zhizuobiao(&xva, &yva, pathjiao3 + jj, pathchang3 + ii, 160, 120);
                if (erzhi_black[int(xva)][int(yva)] == 0)
                {
                    if (jj < 0)
                        voll++;
                    else
                        voll--;
                }
            }
        }
        if (voll > 0)
        {
            *jiao = pathjiao3 + 30;
            *chang = pathchang3;
            return;
        }
        else
        {
            *jiao = pathjiao3 - 30;
            *chang = pathchang3;
            return;
        }
    }
    /*
     if(pathx1==400 && pathx2!=400 )
    {
        voll=0;
        for(int jj=-10;jj<10;jj++)
        {
            for(int ii=-1;ii<4;ii++)
            {
            m_find.zhizuobiao(&xva,&yva,pathjiao2+jj,pathchang2);
            if(erzhi_black[xva][yva]==0)
            {
                if(jj<0)
                    voll++;
                else
                    voll--;
            }
            }
        }
        if(voll>0)
        {
            *jiao=pathjiao2+20;
            *chang=pathchang2;
            return;
        }
        else
        {
            *jiao=pathjiao2-20;
            *chang=pathchang2;
            return;
        }
    }
    if(pathx1!=400 && pathx2==400  )
    {
        voll=0;
        for(int jj=-10;jj<10;jj++)
        {
            for(int ii=-1;ii<4;ii++)
            {
            m_find.zhizuobiao(&xva,&yva,pathjiao1+jj,pathchang1);
            if(erzhi_black[xva][yva]==0)
            {
                if(jj<0)
                    voll++;
                else
                    voll--;
            }
            }
        }
        if(voll>0)
        {
            *jiao=pathjiao1+20;
            *chang=pathchang1;
            return;
        }
        else
        {
            *jiao=pathjiao1-20;
            *chang=pathchang1;
            return;
        }

    }
    if(pathx2!=400 && pathx1!=400)
        if(abs(jiajiao(pathjiao1,jiaom))>=abs(jiajiao(pathjiao2,jiaom)))
        {
            voll=0;
            for(int jj=-10;jj<10;jj++)
            {
                for(int ii=-1;ii<4;ii++)
                {
                m_find.zhizuobiao(&xva,&yva,pathjiao2+jj,pathchang2);
                    if(erzhi_black[xva][yva]==0)
                    {
                        if(jj<0)
                            voll++;
                        else
                            voll--;
                    }
                }
            }
            if(voll>0)
            {
                *jiao=pathjiao2+20;
                *chang=pathchang2;
                return;
            }
            else
            {
                *jiao=pathjiao2-20;
                *chang=pathchang2;
                return;
            }
        }
        else
        {
            voll=0;
            for(int jj=-10;jj<10;jj++)
            {
                for(int ii=-1;ii<4;ii++)
                {
                m_find.zhizuobiao(&xva,&yva,pathjiao1+jj,pathchang1);
                if(erzhi_black[xva][yva]==0)
                {
                    if(jj<0)
                        voll++;
                    else
                        voll--;
                }
                }
            }
            if(voll>0)
            {
                *jiao=pathjiao1+20;
                *chang=pathchang1;
                return;
            }
            else
            {
                *jiao=pathjiao1-20;
                *chang=pathchang1;
                return;
            }
        }
*/
    // x=400,说明没做路径。

    /*
    if(pathx3!=400)
    {
        if(jiajiao(jiaom,pathjiao3)>=0)
            {
                *jiao=pathjiao3+20;
                *chang=pathchang3;
            }
            else
            {
                *jiao=pathjiao3-20;
                *chang=pathchang3;
            }
    }
    else
        if(pathx1==400 && pathx2!=400)
        {
                if(jiajiao(jiaom,pathjiao2)>=0)
                {
                    *jiao=pathjiao2+20;
                    *chang=pathchang2;
                }
                else
                {
                    *jiao=pathjiao2-20;
                    *chang=pathchang2;
                }
        }
        if(pathx1!=400 && pathx2==400)
        {
                if(jiajiao(jiaom,pathjiao1)>=0)
                {
                    *jiao=pathjiao1+20;
                    *chang=pathchang1;
                }
                else
                {
                    *jiao=pathjiao1-20;
                    *chang=pathchang1;
                }
        }
        if(pathx1==400 && pathx2==400)
        {
            *jiao=jiaom;
            *chang=changm;
        }
        if(pathx1!=400 && pathx2!=400)
        {
            if(jiajiao(jiaom,pathjiao2)>=0 && jiajiao(jiaom,pathjiao1)>=0)
            {
                *jiao=pathjiao2+20;
                *chang=pathchang2;
            }
            else if(jiajiao(jiaom,pathjiao2)<0 && jiajiao(jiaom,pathjiao1)<0)
            {
                *jiao=pathjiao1-20;
                *chang=pathchang1;
            }
            else
            {
                if(abs(jiajiao(pathjiao1,jiaom))<abs(jiajiao(pathjiao2,jiaom)))
                {
                    if(jiajiao(jiaom,pathjiao1)>=0)
                    {
                        *jiao=pathjiao1-20;
                        *chang=pathchang1;
                    }
                    else
                    {
                        *jiao=pathjiao1-20;
                        *chang=pathchang1;
                    }
                }
                else
                {
                    if(jiajiao(jiaom,pathjiao2)>=0)
                    {
                        *jiao=pathjiao2+20;
                        *chang=pathchang2;
                    }
                    else
                    {
                        *jiao=pathjiao2+20;
                        *chang=pathchang2;
                    }
                }
        }
    }
    */
}

void CGetImage::jiaodujiaozhun(int jiao, Yuzhi *mubiao)
{
    mubiao->ball_jiao += jiao;
    if (mubiao->ball_jiao > 180)
        mubiao->ball_jiao -= 360;
    if (mubiao->ball_jiao < -180)
        mubiao->ball_jiao += 360;

    mubiao->ddoor_zuo += jiao;
    if (mubiao->ddoor_zuo > 180)
        mubiao->ddoor_zuo -= 360;
    if (mubiao->ddoor_zuo < -180)
        mubiao->ddoor_zuo += 360;

    mubiao->ddoor_you += jiao;
    if (mubiao->ddoor_you > 180)
        mubiao->ddoor_you -= 360;
    if (mubiao->ddoor_you < -180)
        mubiao->ddoor_you += 360;

    mubiao->ddoor_jiao += jiao;
    if (mubiao->ddoor_jiao > 180)
        mubiao->ddoor_jiao -= 360;
    if (mubiao->ddoor_jiao < -180)
        mubiao->ddoor_jiao += 360;

    mubiao->jdoor_jiao += jiao;
    if (mubiao->jdoor_jiao > 180)
        mubiao->jdoor_jiao -= 360;
    if (mubiao->jdoor_jiao < -180)
        mubiao->jdoor_jiao += 360;

    mubiao->jdoor_zuo += jiao;
    if (mubiao->jdoor_zuo > 180)
        mubiao->jdoor_zuo -= 360;
    if (mubiao->jdoor_zuo < -180)
        mubiao->jdoor_zuo += 360;

    mubiao->jdoor_you += jiao;
    if (mubiao->jdoor_you > 180)
        mubiao->jdoor_you -= 360;
    if (mubiao->jdoor_you < -180)
        mubiao->jdoor_you += 360;
}

void CGetImage::vhecheng(double jiao1, double sudu1, double top1, double jiao2, double sudu2, double top2, double *jiao,
                         double *sudu)
{
    double x1;
    double y1;
    double x2;
    double y2;
    double x;
    double y;
    if (sudu1 > top1)
        sudu1 = top1;
    if (sudu2 > top2)
        sudu2 = top2;
    if (jiao1 > 180)
        jiao1 -= 360;
    if (jiao1 < -180)
        jiao1 += 360;
    if (jiao2 > 180)
        jiao2 -= 360;
    if (jiao2 < -180)
        jiao2 += 360;
    m_find.zhizuobiao(&x1, &y1, jiao1, sudu1, 0, 0);
    m_find.zhizuobiao(&x2, &y2, jiao2, sudu2, 0, 0);
    x = x1 + x2;
    y = y1 + y2;
    m_find.jizuobiao(x, y, jiao, sudu, 0, 0);
}

void CGetImage::yanchangxian(double x1, double y1, double x2, double y2, double chang, int *x, int *y)
{
    *x = ((x1 - x2) * chang) / (sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2))) + x1;
    *y = ((y1 - y2) * chang) / (sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2))) + y1;
}

void CGetImage::zhuiqiu(double mujiao, double muchang, double c_shang)
{
    int xuanzhuan1;
    int n;
    float zhuanmax;
    float zhuansdmx;
    if (muchang < 130)
        n = (muchang - c_shang + 1.4) * 200;
    else
        n = (muchang - c_shang + 3.5) * 300;
    if (n < 200 && n > -1600) // && abs(mujiao)<30)
        jifen = jifen + n;
    else if (n <= -1600)
        jifen -= 1600;
    else
        // if(abs(mujiao)<30)
        jifen += 200;
    if (jifen > 18000) // 对此微分的积分量的上下限进行限制
        jifen = 18000;
    if (jifen < 3000)
        jifen = 3000;
    jifen -= abs(y.ball_jiao);
    //	jifen+=(y.ball_chang-60)*1;
    zhuansdmx = 2000;
    //	if(zhuansdmx>6000000/jifen)
    //		zhuansdmx=6000000/jifen;
    zhuansdmx = 1500;
    zhuan = 4 * duizhun1(mujiao, 0, zhuansdmx); // 追球的转动量
    if (zhuan > 0 && zhuan < 300)
        zhuan = 300;
    if (zhuan < 0 && zhuan > -300)
        zhuan = -300;
    zhuanmax = jifen / 100;
    zhuanmax = 20000 / jifen;
    if (zhuanmax > 300)
        ;
    zhuanmax = 300;
    if (abs(zhuan) > abs(zhuan_shang))
    {
        if (zhuan - zhuan_shang > zhuanmax)
            zhuan = zhuan_shang + zhuanmax;
        if (zhuan - zhuan_shang < -zhuanmax)
            zhuan = zhuan_shang - zhuanmax;
    }
    if (mujiao_s != 1000)
    {
        zhuan = zhuan + 100 * (mujiao_s - mujiao);
    }
    jifen -= abs(zhuan) * 0.04;
    //	jifen=0;
    if (muchang > 65)
        Controlmoto((long)-Path_LA, (long)jifen, 20000, &mtl, &mtr, &mtb);
    else
        Controlmoto((long)y.ball_jiao, (long)jifen, 20000, &mtl, &mtr,
                    &mtb);   // 追球的平移分量 速度是长度关系+速度积分+加速度
    qiansudu = jifen + 1800; //; //保留追球时候向前的速度分量
                             //		if(y.quanbx>1300 || y.quanbx<100 || y.quanby>1100 || y.quanby<100)
    //			Controlmoto(y.ball_jiao+90,-duizhun(jiajiao(y.ddoor_jiao,y.ball_jiao),0,2000),10000,&mtl,&mtr,&mtb);
    zhuangtai = "执行追球";
    chongqiu = 0;
    mujiao_s = mujiao;
}

void CGetImage::daiqiu(int qiansudu1, double mubiao)
{
    double jz;
    double cz;
    if (0) // chongqiu<3) //如果球从非持球状态进入持球状态，则向前先冲球一下
    {
        jiasudu1 += 300;     // 这时的加速度量是50
        if (jiasudu1 > 7000) // 加速度积分的上限是1000
            jiasudu1 = 7000;
        zhuan = 0;
        //	zhuan=duizhun(y.ball_jiao,0,800); //对准门
        Controlmoto(0, qiansudu + jiasudu1, 8500, &mtl, &mtr, &mtb); // 根据前方向加速度量向前方向做加速运动
        zhuangtai = "qianchong";
        chongqiu++;
    }
    else
    {
        //	jiasudu1+=(y.ball_chang-holdball+3)*30;
        if (jiasudu1 < 100)
            jiasudu1 = 100;
        if (jiasudu1 < 1000)
            jiasudu1 += 10;
        if (jiasudu1 > 5000)
            jiasudu1 -= 50;
        if ((-abs(mubiao) + 10) * 1 < -100)
            jiasudu1 += -100;
        else if ((-abs(mubiao) + 10) * 1 > 30)
            jiasudu1 += -30;
        else
            jiasudu1 += (-abs(mubiao) + 10) * 1;

        //	if (jiasudu1>3000) //最大1000
        //		jiasudu1=3000;
        if (jiasudu1 < -qiansudu)
            jiasudu1 = -qiansudu;

        if (qiansudu > 5000)
            qiansudu -= 180;
        if ((-abs(mubiao) + 10) * 1 < -200)
            qiansudu += -200;
        else if ((-abs(mubiao) + 10) * 1 > 30)
            qiansudu += 30;
        else
            qiansudu += (-abs(mubiao) + 10) * 1;
        if (qiansudu < 100)
            qiansudu = 100;
        if (qiansudu < 1000)
            qiansudu += 1;
        zhuan = 2 * duizhun1(mubiao, 0, 2000); // 对准门
        if (zhuan > 0 && zhuan < 100)
            zhuan = 100;
        if (zhuan < 0 && zhuan > -100)
            zhuan = -100;
        if (zhuan > daiqiuz_s + 200)
            zhuan += 200;
        if (zhuan < daiqiuz_s - 200)
            zhuan -= 200;
        //	if(abs(zhuan)>1300)
        vhecheng(0, qiansudu + jiasudu1, 10000, -90, -2 * zhuan, 15000, &jz, &cz);
        //	else
        //		vhecheng(0,qiansudu+jiasudu1,8500,-90,-0.5*zhuan,10000,&jz,&cz);
        //	vhecheng(0,qiansudu+jiasudu1,2000,y.ddoor_jiao,abs(zhuan),10000,&jz,&cz);
        Controlmoto(jz, cz, 15000, &mtl, &mtr, &mtb);
        zhuangtai = "向门对准1";
        daiqiuz_s = zhuan;
    }
}

void CGetImage::yanjiao(double x1, double y1, double x2, double y2, double chang, double jiao, int *x, int *y)
{
    double jiaox;
    double changx;
    double jiaox1;
    double changx1;
    double xx;
    double yy;
    m_find.jizuobiao(x2, y2, &jiaox, &changx, x1, y1);
    //	m_find.jizuobiao(x1,y1,&jiaox1,&changx1,x2,y2);
    jiaox += jiao;
    if (jiaox > 180)
        jiaox -= 360;
    if (jiaox < -180)
        jiaox += 360;
    m_find.zhizuobiao(&xx, &yy, jiaox, chang, x1, y1);
    *x = xx;
    *y = yy;
}

void CGetImage::guolvzaodian()
{
    int i;
    int j;

    for (i = 1; i < 639; i++)
        for (j = 1; j < 479; j++)
        {
            if (erzhi_bai[i][j] == 1)
            {
                linshi[i - 1][j - 1] = 1;
                linshi[i - 1][j + 1] = 1;
            }
        }
}

void CGetImage::dianjijiexi(double motol, double motor, double motob, double motol_s, double motor_s, double motob_s,
                            double *jiao, double *chang, double *zhuan)
{
    /*
    vhecheng(-30,motol-motol_s,500000,-150,motor-motor_s,500000,jiao,chang);
    vhecheng(*jiao,*chang,500000,-270,motob-motob_s,500000,jiao,chang);

    *zhuan=-(sin((float(jiajiao(-(*jiao),-60)))*3.1416/180)*(*chang)*0.667);
    *zhuan=-(*zhuan)+motol-motol_s;
    *chang=*chang*0.6667;
    *chang=*chang*0.0003272492347489*100;
    *zhuan=-*zhuan*0.1;
*/
    vhecheng(-30, motol - motol_s, 500000, -150, motor - motor_s, 500000, jiao, chang);
    vhecheng(*jiao, *chang, 500000, -270, motob - motob_s, 500000, jiao, chang);

    *zhuan = -(sin((float(jiajiao(-(*jiao), -60))) * 3.1416 / 180) * (*chang) * 0.66667);
    *zhuan = -(*zhuan) + motol - motol_s;
    *chang = *chang * 0.66667;
    *chang = *chang * 0.00174735;
    *chang = *chang / 2.4;
    *zhuan = -*zhuan * 0.1 * 0.0197;
}

void CGetImage::AfterUpdateAttitude(FLOAT inAngle, int inXMagn, int inYMagn, FLOAT inXRoll, FLOAT inYRoll)
{
    return;
    int he;
    long x;
    long y;
    x = inXMagn;
    y = inYMagn;
    // inAngle为偏北角,   inXMagn为x向场强,   inYMagn为y向场强
    inAngle = -inAngle; // 和场地定义方向相同
    inAngle += 0;       // 场地角度和地磁角度
    if (inAngle > 180)
        inAngle -= 360;
    if (inAngle < -180)
        inAngle += 360;          // 越界
    m_find.luopanjiao = inAngle; // 罗盘如果采集到数据，初始化场线定位的起始角
    he = sqrt(x * x + y * y);    // 算磁场强
    //	CString str;
    //	str.Format("%d",he);
    //	AfxMessageBox(str);
    if (he < 100 && he > 350)
        m_find.luopanqingchu = 1; // 如果磁场强不在范围内，则判定受干扰
}

void CGetImage::Kalman(double *x_p, double *v_p, double xm, double a, double b)
{
    double xs, vs;
    xs = *x_p + a * (xm - *x_p);
    vs = *v_p + b / 0.067 * (xm - *x_p);
    *x_p = xs + 0.067 * vs;
    *v_p = vs;
}

// 自检 用原始数据历史纪录清零
void CGetImage::int_zijian(float f_x, float f_y, float f_a)
{
    for (int i = 0; i < 20; i++) // 目标位置的历史纪录 用于求微分 积分用
    {
        robot_h[i].f_x = f_x;
        robot_h[i].f_y = f_y;
        robot_h[i].f_a = -f_a; // 存储机器 在20周期内的 运动位置等信息历史记录  鑫鑫是逆坐标系
        robot_L_vd_h[i] = 0;   // 存储视觉每贞间路程
        mlh[i] = 0;
        mbh[i] = 0;
        mrh[i] = 0;
    }
    mbR = 0;
    mlR = 0;
    mrR = 0;
}
// SQd//路程定位 最近10贞的累计路程
// SQv//视觉 最近10贞的累计路程
// SQ
// SQa//视觉 角度10贞的累计角度差
// SQc//视觉 位置10贞的累计距离差
void CGetImage::zijian(float mtb, float mtl, float mtr, float f_x, float f_y, float f_a, float *SQd, float *SQv,
                       float *SQ, float *SQa, float *SQc)
{
    // cs电机的历史记录 [0]当前周期的 为通过电机路程定位用
    for (i = 19; i > 0; i--)
    {
        mlh[i] = mlh[i - 1];
        mbh[i] = mbh[i - 1];
        mrh[i] = mrh[i - 1];
    }
    mlh[0] = mtl;
    mbh[0] = mtb;
    mrh[0] = mtr;

    // 视觉定位信息历史纪录
    for (i = 19; i > 0; i--) // 目标位置的历史纪录 用于求微分 积分用
    {
        robot_h[i].f_x = robot_h[i - 1].f_x;
        robot_h[i].f_y = robot_h[i - 1].f_y;
        robot_h[i].f_a = robot_h[i - 1].f_a;
        robot_L_vd_h[i] = robot_L_vd_h[i - 1]; // 存储视觉每贞间路程
    }
    robot_h[0].f_x = f_x;  // 存储机器 在20周期内的 运动位置等信息历史记录
    robot_h[0].f_y = f_y;  // 存储机器 在20周期内的 运动位置等信息历史记录
    robot_h[0].f_a = -f_a; // 存储机器 在20周期内的 运动位置等信息历史记录  鑫鑫是逆坐标系
    robot_L_vd_h[0] = sqrt((robot_h[0].f_x - robot_h[1].f_x) * (robot_h[0].f_x - robot_h[1].f_x) +
                           (robot_h[0].f_y - robot_h[1].f_y) * (robot_h[0].f_y - robot_h[1].f_y));

    // 注：矫正方法 使用单独 角度对准程序矫正wp 单独平移对准程序矫正vp  矫正结果就是球在场地地图上的位置不动
    const double vp = 2.2; // 2.2 ;//静态2.2 ;//平移矫正系数 常数
    const double wp = 2.0; // 1.2 ;//静态2.0 ;//旋转矫正系数 常数
    // if(valxx==1)
    //{
    robot_L_c.f_x = robot_h[10].f_x;
    robot_L_c.f_y = robot_h[10].f_y;
    robot_L_c.f_a = robot_h[10].f_a;
    float temp_f_x;
    float temp_f_y;
    *SQd = 0; // 路程定位 最近10贞的累计路程
    *SQv = 0; // 视觉 最近10贞的累计路程
    *SQ = 0;  // 视觉 和路程定位 路程差
    *SQa = 0; // 视觉 角度10贞的累计角度差
    *SQc = 0; // 视觉 位置10贞的累计距离差

    // cs电机的历史记录 [0]当上周期的 为通过电机路程定位用
    for (i = 10; i >= 1; i--)
    {
        temp_f_x = robot_L_c.f_x;
        temp_f_y = robot_L_c.f_y;
        const float T = 0.067; // 周期时间单位秒
        // 对球的对地运动矢量计算001
        // 运动反解
        float front_v = 0; // 1.99012;//临时存储 反解出的 平移速度 转动速度 平移方向
        float turn_w = 0;  // 61.9297;
        float front_a = 0; //-29.3969;
        // 电机的历史记录 [0]为上个周期的
        O_movement_T_analyze(&front_a, &front_v, &turn_w, -mlh[i], -mbh[i],
                             -mrh[i]); // *move_v_angle, *move_v_speed, *turn_v
        front_v = front_v * vp / wheel_linear_speed_ratio;
        turn_w = turn_w * wp * 360.0 / wheel_rotate_speed_ratio;
        // SetMovement(T, front_v, front_a, turn_w, &robot_L);//(float front_v_c,float front_v_angle_c,float
        // turn_w,obj_properties *obj);
        Movement(T, front_v * 314, front_a, turn_w, &robot_L_c);
        *SQd += sqrt((temp_f_x - robot_L_c.f_x) * (temp_f_x - robot_L_c.f_x) +
                     (temp_f_y - robot_L_c.f_y) * (temp_f_y - robot_L_c.f_y));
    }
    //}
    // 计算x y距离超差累积
    *SQc = sqrt((robot_L_c.f_x - robot_h[0].f_x) * (robot_L_c.f_x - robot_h[0].f_x) +
                (robot_L_c.f_y - robot_h[0].f_y) * (robot_L_c.f_y - robot_h[0].f_y));
    *SQa = angle_add_transfer(robot_L_c.f_a, -robot_h[0].f_a);
    // float SQv=0;//视觉 最近10贞的累计路程
    for (i = 0; i < 10; i++)
    {
        *SQv += robot_L_vd_h[i];
    }

    *SQ = *SQd - *SQv;

    // m_find.pField->SetAgent(1 ,robot_L_c.f_x    ,robot_L_c.f_y /*+80*/   ,-robot_L_c.f_a);//场子正中向上 逆时针坐标系
}

// T一个周期时间  v机体 平移速度 v_a机体速度角度 w机体转动速度 local机体物理状态存储结构体
void CGetImage::Movement(float T, float v, float v_a, float w, obj_properties *local)
{
    // 开始 通过电机输出值运动位置累加  //以后每一帧做累加
    if (fabs(w) > 0.1)
    {
        float Rc = (v * 180) / (w * 3.14); // v/(w*3.14/????128????);			//机体旋转中心的极坐标	半径w*pi*r/180=v
                                           // Rc w*(2*pi*r/360) =v  w(度/s)  v,r(米) 或(像素)
        double Rx, Ry;
        //		if(w>0)//算出旋转中心 按照旋转中心,起点(local.f_x,local.f_y)经过的弧度T*w
        // 找出弧形终点存入(local.f_x,local.f_y)
        //		{
        Rx = local->f_x + coordinate_transfer_x(local->f_a + 90 + v_a, Rc); // 算出旋转中心 地面坐标
        Ry = local->f_y + coordinate_transfer_y(local->f_a + 90 + v_a, Rc);
        local->f_x = Rx - coordinate_transfer_x(local->f_a + 90 + v_a + T * w, Rc); // 本机 新 场地x
        local->f_y = Ry - coordinate_transfer_y(local->f_a + 90 + v_a + T * w, Rc); // 本机 新 场地y
        //		}
        //		else
        //		{	Rx = local->f_x + coordinate_transfer_x(local->f_a-90+v_a,Rc);//算出旋转中心 地面坐标
        //			Ry = local->f_y + coordinate_transfer_y(local->f_a-90+v_a,Rc);
        //			local->f_x=Rx   - coordinate_transfer_x(local->f_a-90+v_a+T*w,Rc);//本机 新 场地x
        //			local->f_y=Ry   - coordinate_transfer_y(local->f_a-90+v_a+T*w,Rc);//本机 新 场地y
        //		}
        local->f_a = angle_add_transfer(T * w, local->f_a); // 本机场地angle = 时间*角速度 + 上周期本机场地angle
    }
    else
    {
        local->f_x += coordinate_transfer_x(local->f_a + v_a, v) * T; // 本机 新 场地x
        local->f_y += coordinate_transfer_y(local->f_a + v_a, v) * T; // 本机 新 场地y
    }
    // 位置估测完毕
}

bool CGetImage::O_movement_T_analyze(float *move_v_angle, float *move_v_speed, float *turn_v, float motor1,
                                     float motor2, float motor3) // 全向解析
{
    return (move2Control.O_movement_T_analyze(move_v_angle, move_v_speed, turn_v, motor1, motor2, motor3));
}

// 使用加速度限制 预测 机体电机轴速
void CGetImage::motor_a_limit(float mtb, float mtl, float mtr)
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

void CGetImage::xiqiujiexi(int motol, int motor, int motob, double time, short *l, short *r)
{
    RGBTRIPLE yanse;
    yanse.rgbtBlue = 255;
    yanse.rgbtGreen = 0;
    yanse.rgbtRed = 0;
    double jiao;
    double chang;
    double zhuan;
    int i = 0;
    int vl, vb, vr;
    for (i = 0; i < 5; i++)
    {
        vl = m_pMotorPos_H[i][0] - m_pMotorPos[0];
        vb = m_pMotorPos_H[i][2] - m_pMotorPos[2];
        vr = m_pMotorPos_H[i][1] - m_pMotorPos[1];
        if ((vl != 0) || (vb != 0) || (vr != 0))
        {
            mapan_s[0] = m_pMotorPos_H[i][0];
            mapan_s[1] = m_pMotorPos_H[i][1];
            mapan_s[2] = m_pMotorPos_H[i][2];
            break;
        }
    }
    dianjijiexi(m_pMotorPos[0], m_pMotorPos[1], m_pMotorPos[2], mapan_s[0], mapan_s[1], mapan_s[2], &jiao, &chang,
                &zhuan);
    //	dianjijiexi(motol,motor,motob,mtl_s1,mtr_s1,mtb_s1,&jiao,&chang,&zhuan);
    *l = 0;
    *r = 0;
    if (abs(jiao) < 80)
    {
        *l = chang * 30000 + 2000; //-cos(jiao*3.14159/180)*chang*30000+8000;
        *r = chang * 30000 - 2000; // cos(jiao*3.14159/180)*chang*30000-8000;
    }
    else
    {
        *l = -chang * 30000 + 8000; //-cos(jiao*3.14159/180)*chang*30000+8000;
        *r = -chang * 30000 - 8000;
    }
    *l += sin(jiao * 3.14 / 180) * chang * 5000;
    *r += sin(jiao * 3.14 / 180) * chang * 5000;
    *l = *l - zhuan * 5000;
    *r = *r - zhuan * 5000;
    if (*l < -3000)
        *l = -3000;
    if (*r > 3000)
        *r = 3000;
    double xx;
    double yy;
    /*
    for(i=0;i<chang*10;i++)
    {
        m_find.zhizuobiao(&xx,&yy,jiao,i,320,240);
        m_find.SetColor(xx,yy,yanse);
    }
    */
    /*
    if(*l>-2000)
    {
        *l=*l+8000;
        *l=*l-8000;
    }
    else
    {
        *l=*l+8000;
        *l=*l-8000;
    }
    */
}

void CGetImage::DebugInfoM(CString inStr)
{
    m_find.DebugInfoM(inStr);
}

void CGetImage::DebugInfoS(CString inStr)
{
    m_find.DebugInfoS(inStr);
}

void CGetImage::balldiushi(Yuzhi *changdi)
{
    RGBTRIPLE yanse;
    yanse.rgbtBlue = 255;
    yanse.rgbtGreen = 255;
    yanse.rgbtRed = 255;
    net.toS_x = y.benjix;
    net.toS_y = y.benjiy;
    net.toS_angle = y.benjijiao; // 返回给教练机自己的位置

    if (y.ball_zhong <= 10)
    {

        double dingweijiao;
        double dingweichang;
        net.toS_foundball = false;
        // 当球的重量小于30时,告诉教练机自己看不见球
        //	net.m_ctrl.re_ballx
        if (diuqiuleijia > 0)
        {
            diuqiuleijia--;

            m_find.jizuobiao(qbx_shang, qby_shang, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
            dingweijiao = -jiajiao(-dingweijiao, y.benjijiao - 90);
            // 根据教练机给的球的位置,将这个坐标投影到视觉
            y.ball_jiao = dingweijiao;
            y.ball_chang = fanquxian[int(dingweichang * cdbl / 2.7)];
            m_find.zhizuobiao(&y.ball_x, &y.ball_y, y.ball_jiao, y.ball_chang, 320, 240);
            m_find.DrawFocus(y.ball_x, y.ball_y, &yanse);

            y.quanbx = qbx_shang;
            y.quanby = qby_shang;

            if (sqrt((y.benjix - y.quanbx) * (y.benjix - y.quanbx) + (y.benjiy - y.quanby) * (y.benjiy - y.quanby)) <
                40)
                diuqiuleijia -= 5;
        }
        else
        {

            m_find.jizuobiao(net.re_ballx, net.re_bally, &dingweijiao, &dingweichang, y.benjix, y.benjiy);
            dingweijiao = -jiajiao(-dingweijiao, y.benjijiao - 90);
            // 根据教练机给的球的位置,将这个坐标投影到视觉
            y.ball_jiao = dingweijiao;
            y.ball_chang = fanquxian[int(dingweichang * cdbl / 2.7)];
            dingweichang = y.ball_chang;
            // 教练机给的球的长度换算成图像的长度.
            m_find.zhizuobiao(&y.ball_x, &y.ball_y, dingweijiao, dingweichang, 320, 240);
            // 根据球的极坐标算出球的直角坐标.
            m_find.DrawFocus(y.ball_x, y.ball_y, &yanse);
            m_find.DrawFocus(y.ball_x + 5, y.ball_y, &yanse);
            m_find.DrawFocus(y.ball_x, y.ball_y + 5, &yanse);
            m_find.DrawFocus(y.ball_x, y.ball_y - 5, &yanse);
            m_find.DrawFocus(y.ball_x - 5, y.ball_y, &yanse);

            y.quanbx = net.re_ballx;
            y.quanby = net.re_bally;
        }
    }
    else
    {

        ballx_shang = y.ball_x;
        bally_shang = y.ball_y;
        ballj_shang = y.ball_jiao;
        ballc_shang = y.ball_chang;
        qbx_shang = y.quanbx;
        qby_shang = y.quanby;

        diuqiuleijia = 100;
        net.toS_foundball = true;
        // 告诉教练机看见球.
        double balljiao_temp = angle_add_transfer2(-y.ball_jiao - y.benjijiao + 90, 0);
        // double	ballchang_temp=quxian[int(y.ball_chang)]/cdbl*2.7;//quxian[int(changdi->ball_chang)];
        double ballchang_temp = quxian[int(y_ball_chang_min)] / cdbl * 2.7; // quxian[int(changdi->ball_chang)];
        //		double	ballchang_temp=(quxian[int(y_ball_chang_min)]/cdbl*2.7);//quxian[int(changdi->ball_chang)];
        double ballchang_tempballchang_temp =
            (11.25 * (ballchang_temp + sqrt(ballchang_temp * ballchang_temp + 73 * 73)) / 73);
        //		Display_C=ballchang_tempballchang_temp;
        //		Display_A=ballchang_temp;
        //		Display_B=ballchang_temp-ballchang_tempballchang_temp;
        ballchang_temp = ballchang_temp - ballchang_tempballchang_temp;

        double temp_y_quanbx;
        double temp_y_quanby;
        m_find.zhizuobiao(&temp_y_quanbx, &temp_y_quanby, balljiao_temp, ballchang_temp, y.benjix, y.benjiy);

#ifndef RUN_WITH_EMU
        net.toS_ballx = temp_y_quanbx;
        net.toS_bally = temp_y_quanby;
#endif
#ifdef RUN_WITH_EMU
        // 仿真程序插入数据
        net.toS_ballx = ReceiveServerData.ballLocal_x;
        net.toS_bally = ReceiveServerData.ballLocal_y;
#endif
        //		net.toS_ballx = y.quanbx;
        //		net.toS_bally = y.quanby;
        // 如果看见的球的重量大于30,则把自己看见的球的坐标告诉教练机
    }
    if (m_pAD[0] > 1500 && m_pAD[1] > 1500 && y.ball_chang < 55 && abs(y.ball_jiao) < 20)
    {
        net.toS_ballx = y.benjix;
        net.toS_bally = y.benjiy;
    }
    if (in_fouce_stat == 1)
    {
        net.toS_ballx = y.benjix + 1;
        net.toS_bally = y.benjiy + 1;
    }
    if (m_bNetSend == true && leijiaqi % 3 == 0)
    {
        if (kaijipd > 111) // 450)
            net.Report();
        else
            net.ReportNew();
    }
    if (y.ball_x > 639)
    {
        y.ball_x = 639;
    }
    if (y.ball_x < 0)
    {
        y.ball_x = 0;
    }
    if (y.ball_y > 479)
    {
        y.ball_y = 479;
    }
    if (y.ball_y < 0)
    {
        y.ball_y = 0;
    }
    if (y.ball_chang > 239)
    {
        y.ball_chang = 239;
    }
}

void CGetImage::adxiqiu()
{
    motospeed[3] = (2000 - m_pAD[0]) * 8;
    motospeed[4] = -(2000 - m_pAD[1]) * 8;
    if (motospeed[3] > 8000)
        motospeed[3] = 8000;
    if (motospeed[4] < -8000)
        motospeed[4] = -8000;
}

void CGetImage::zhuiqiu1(double mujiao, double muchang, double c_shang)
{
    int xuanzhuan1;
    if ((muchang - c_shang + 3) * 300 < 200) // && abs(mujiao)<30)
        jifen = jifen + (muchang - c_shang + 3) * 300;
    else // if(abs(mujiao)<30)
        jifen += 200;
    if (jifen > 10000) // 对此微分的积分量的上下限进行限制
        jifen = 10000;
    if (jifen < 1000)
        jifen = 1000;
    jifen -= abs(y.ball_jiao);
    //	jifen+=(y.ball_chang-60)*1;

    zhuan = 2 * duizhun(mujiao, 0, 2000); // 追球的转动量
    if (abs(zhuan) > abs(zhuan_shang))
    {
        if (zhuan - zhuan_shang > 100)
            zhuan = zhuan_shang + 100;
        if (zhuan - zhuan_shang < -100)
            zhuan = zhuan_shang - 100;
    }
    if (muchang > 70)
        Controlmoto((long)-Path_LA, (long)jifen, 20000, &mtl, &mtr, &mtb);
    else
        Controlmoto((long)y.ball_jiao, (long)jifen, 20000, &mtl, &mtr,
                    &mtb);   // 追球的平移分量 速度是长度关系+速度积分+加速度
    qiansudu = jifen + 1800; //; //保留追球时候向前的速度分量
    //		if(y.quanbx>1300 || y.quanbx<100 || y.quanby>1100 || y.quanby<100)
    //			Controlmoto(y.ball_jiao+90,-duizhun(jiajiao(y.ddoor_jiao,y.ball_jiao),0,2000),10000,&mtl,&mtr,&mtb);
    zhuangtai = "执行追球";
    chongqiu = 0;
}

int CGetImage::duizhun1(double jiao, double jizhun, int top)
{
    double xx;
    double jiao1;
    //	jiao1=-jiao;
    //	jiao1=jiao1+jizhun;
    //	if (jiao1>180 || jiao1<-180)
    //		jiao1=-jiao1-2*jizhun;
    jiao1 = jiajiao(jizhun, jiao);

    if (jiao1 > 70)
        jiao1 = 70;
    if (jiao1 < -70)
        jiao1 = -70;
    if (jiao1 >= 0)
    {
        xx = jiao1 * 30;
        if (xx > top)
            xx = top;
    }
    else
    {
        xx = jiao1 * 30;
        if (xx < -top)
            xx = -top;
    }

    //	if(xx>0 && xx<500)
    //		xx=xx+100;
    //	if(xx<-0 && xx>-500)
    //		xx=xx-100;

    return -xx;
}
