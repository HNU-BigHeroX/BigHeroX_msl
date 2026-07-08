#include "stdafx.h"
#include "CatchPosition.h"

#include <highgui.h> //lw//
#include <cv.h>

static bool cvshow = true; // 显示OpenCV
// static bool cvshow = false;		//不显示OpenCV窗口

static BYTE PixColor[2000][3];

void initPixColor()
{
    for (int i = 0; i < 1000; i++)
    {
        PixColor[i][0] = 0;
        PixColor[i][1] = 0;
        PixColor[i][2] = 0;
    }
    // 0~255
    for (i = 0; i < 256; i++)
    {
        PixColor[i][0] = i;
        PixColor[i][1] = 0;
        PixColor[i][2] = 0;
    }
    // 255~512
    for (i = 256; i < 512; i++)
    {
        PixColor[i][0] = 255;
        PixColor[i][1] = i - 256;
        PixColor[i][2] = 0;
    }
    // 512~768
    for (i = 512; i < 768; i++)
    {
        PixColor[i][0] = 768 - i;
        PixColor[i][1] = 255;
        PixColor[i][2] = 0;
    }

    // 768~1024
    for (i = 768; i < 1024; i++)
    {
        PixColor[i][0] = 0;
        PixColor[i][1] = 255;
        PixColor[i][2] = i - 768;
    }

    // 1024~1280
    for (i = 1024; i < 1280; i++)
    {
        PixColor[i][0] = 0;
        PixColor[i][1] = 1280 - i;
        PixColor[i][2] = 255;
    }

    // 1280~1536
    for (i = 1280; i < 1536; i++)
    {
        PixColor[i][0] = i - 1280;
        PixColor[i][1] = 0;
        PixColor[i][2] = 255;
    }

    // 1536~1792
    for (i = 1536; i < 1792; i++)
    {
        PixColor[i][0] = 255;
        PixColor[i][1] = i - 1536;
        PixColor[i][2] = 255;
    }

    for (i = 1792; i < 2000; i++)
    {
        PixColor[i][0] = 255;
        PixColor[i][1] = 255;
        PixColor[i][2] = 255;
    }
}

CCatchPosition::CCatchPosition()
{
    initPixColor();
    pDataSummary = NULL;
    for (int i = 0; i < 3; i++)
    {
        posCatch[i].x = 0;
        posCatch[i].x = 0;
    }
    if (cvshow == true)
    {
        cvNamedWindow("重心", 0); // 命名窗口  //lw//
        // cv::resizeWindow("重心", 280, 200);
        cvNamedWindow("距离", 0); // 命名窗口  //lw//
        // cv::resizeWindow("距离", 280, 200);
    }
}

CCatchPosition::~CCatchPosition()
{
}

int CCatchPosition::m_Ag_GetStatus(UINT inSt, uAgent *inalist)
{
    int ret = -1;
    for (int i = 1; i < 5; i++)
    {
        if (inalist[i].online == true)
        {
            if (inalist[i].status == inSt)
            {
                ret = i;
                break;
            }
        }
    }
    return ret;
}

bool CCatchPosition::GetAtkPos(CPoint *inPos, uAgent *inAgent)
{
    bool res = false;
    for (int i = 0; i < AG_NUM; i++)
    {
        if (inAgent[i].online == true && inAgent[i].status == CTRL_ATTACK)
        {
            inPos->x = inAgent[i].x;
            inPos->y = inAgent[i].y;
            res = true;
        }
    }
    return res;
}

float P_posCatch_x = 0;
float P_posCatch_y = 0;
// 返回1 是开球状态 0是普通自主决策状态
bool CCatchPosition::Get_start_state(void)
{
    int state = 0;
    if ((pDataSummary->posCatch.x == P_posCatch_x) && (pDataSummary->posCatch.y == P_posCatch_y))
    {
        state = 1;
    }
    else
    {
        state = 0;
    }
    P_posCatch_x = pDataSummary->posCatch.x;
    P_posCatch_y = pDataSummary->posCatch.y;
    return (state);
}

double Catch_Radius_SET = 150;   // 接球队员可接球范围 单位 cm
double Catch_Radius_ratio = 0.3; // 0.25;// 0.18f;//【接球队员接球范围】   与  【球与接球队员距离】的 比例系数
// 临时变量定义处，请使用static前缀
// 例如：static unsigned char array[100000];
const int Obst_map_width = 180;                    // 120;//180;//300;			//障碍物地图 分辨率 x
const int Obst_map_height = 120;                   // 80 ;//120;//200;			//					y
const int film_zoom_time = 1800 / Obst_map_width;  // 场地厘米制像素和 障碍物地图 分辨率比例
static double max_dis_x_p = 1350 / film_zoom_time; // 上周期得到的目标接球位置
static double max_dis_y_p = 600 / film_zoom_time;
static double Obst_map_data[Obst_map_height][Obst_map_width];
static double Obst_dis_map[Obst_map_height][Obst_map_width];
void CCatchPosition::CalCatchPosition()
{
    if (pDataSummary == NULL)
    {
        return;
    }

    int start_state_flag = Get_start_state(); // 返回1 是开球状态 0是普通自主决策状态
    if (start_state_flag == 1)
    {
        Catch_Radius_SET = 140;    // 接球队员可接球范围 单位 cm
        Catch_Radius_ratio = 0.15; // 0.23;// 0.25;// 0.18f;//【接球队员接球范围】   与  【球与接球队员距离】的 比例系数
    }
    else
    {
        Catch_Radius_SET = 150;    // 190;//接球队员可接球范围 单位 cm
        Catch_Radius_ratio = 0.23; // 0.3;// 0.25;// 0.18f;//【接球队员接球范围】   与  【球与接球队员距离】的 比例系数
    }
    // 注意测试自己队员档传球的问题 因为己方队员不算在障碍地图内
    // 输入：
    // m_cObst[EACH_AG_OBST_NUM*(AG_NUM+1)]数组
    //
    // m_cObst[i].valid 该障碍物是否是有效障碍物（true-有效障碍物  false-无效障碍物）;
    // m_cObst[i].ID = 传回该障碍物的队员的号数（范围：1~5）;
    // m_cObst[i].x = 该障碍物的横坐标（0~1800）;
    // m_cObst[i].y = 该障碍物的纵坐标（0~1200）;
    // m_cObst[i].size = 该障碍物的大小;
    // m_trueball.x , m_trueball.y //球的全局坐标

    // 输出：
    // m_LocaToCatch.x 为接球位置x坐标，从己方到对方范围 0 -> 1800;
    // m_LocaToCatch.y 为接球位置y坐标，范围 0 ~ 1200;

    int pass_move_id = m_Ag_GetStatus(CTRL_PASS_MOVE, pDataSummary->agent);
    /*	if (pass_move_id >= 0)
    {
    int catch_id = 0;
    if (
    // 			m_balls[pass_move_id].x == m_agents[pass_move_id].x +1 &&
    // 			m_balls[pass_move_id].y == m_agents[pass_move_id].y +1
    m_agents[pass_move_id].dist_ball < 50
    )
    {
    catch_id = m_Ag_GetStatus(CTRL_CATCH_MOVE,m_agents);
    if (
    catch_id >0 && catch_id < 6 &&
    m_agents[catch_id].x > 1100						//?????????????????????不明意义 要测试 怀疑是
    m_agents[pass_move_id]？
    )
    {
    m_LocaToCatch.x = m_agents[catch_id].x;
    m_LocaToCatch.y = m_agents[catch_id].y;
    }
    return;
    }
    }
    */

    posCatch[0].x = 1350;
    posCatch[0].y = 600;
    //	posCatch[0].x = 1500;
    //	posCatch[0].y = 600;
    //	return;

    static CPoint atk_pos;
    if (GetAtkPos(&atk_pos, pDataSummary->agent) == true)
    {
        // 输入
        // atk_pos.x 进攻队员x坐标
        // atk_pos.y 进攻队员y坐标
    }
    //	return;
    // EACH_AG_OBST_NUM = 10
    // AG_NUM = 7
    int expect_catch_x = 1350;                  // 希望靠近什么位置接球  x
    int expect_catch_y = 600;                   //						y
    int ball_f_x = pDataSummary->cGlobalBall.x; // 450;	//球在场地坐标系中的 位置
    int ball_f_y = pDataSummary->cGlobalBall.y; // 600;
    int max_dis_x = 1350;                       // 用于输出的变量
    int max_dis_y = 600;
    int enable_catch_area_min_x = 1000;         // 1320;	//0;//0;//预设可进行接球区域 最小x
    int enable_catch_area_max_x = 1500;         // 1420;	//1800;//					 最大x
    int enable_catch_area_min_y = 100;          // 0;// 					 最小y
    int enable_catch_area_max_y = 1100;         // 1200;//					 最大y
    int catch_area_min_x_base = ball_f_x - 180; // 传球基线即 传球点选取尽量在这条线往右
    if (catch_area_min_x_base > 900)
        catch_area_min_x_base = 900;
    if (0)
    {
        enable_catch_area_min_x = 200;  // 1320;	//0;//0;//预设可进行接球区域 最小x
        enable_catch_area_max_x = 1500; // 1420;	//1800;//					 最大x
        enable_catch_area_min_y = 100;  // 0;// 					 最小y
        enable_catch_area_max_y = 1100; // 1200;//					 最大y
    }
    int Obst_t_x[EACH_AG_OBST_NUM * (AG_NUM + 1)];
    int Obst_t_y[EACH_AG_OBST_NUM * (AG_NUM + 1)];
    int Obst_t_l = 0;
    double a2[EACH_AG_OBST_NUM * (AG_NUM + 1)];
    double d1[EACH_AG_OBST_NUM * (AG_NUM + 1)];
    expect_catch_x = expect_catch_x / film_zoom_time;
    expect_catch_y = expect_catch_y / film_zoom_time;
    ball_f_x = ball_f_x / film_zoom_time;
    ball_f_y = ball_f_y / film_zoom_time;
    max_dis_x = max_dis_x / film_zoom_time; // 用于输出的变量
    max_dis_y = max_dis_y / film_zoom_time;
    enable_catch_area_min_x = enable_catch_area_min_x / film_zoom_time;
    enable_catch_area_max_x = enable_catch_area_max_x / film_zoom_time;
    enable_catch_area_min_y = enable_catch_area_min_y / film_zoom_time;
    enable_catch_area_max_y = enable_catch_area_max_y / film_zoom_time;
    catch_area_min_x_base = catch_area_min_x_base / film_zoom_time;
    double max_dis_rx[5] = {0};
    double max_dis_ry[5] = {0};

    register double t1, t2, j, max_dis;
    int i, temp, x, y;
    i = temp = x = y = 0;
    double a1, ao, k, k1, k2, k3, cos1;
    a1 = ao = k = k1 = k2 = k3 = cos1 = 0;
    //	for(i=0;i<80;i++)
    //	{
    //		Obst_t_x[i]=m_cObst[i].x/film_zoom_time;
    //		Obst_t_y[i]=m_cObst[i].y/film_zoom_time;
    //		a2[i]=atan2(ball_f_x-Obst_t_x[i], ball_f_y-Obst_t_y[i]);
    //	}
    Obst_t_l = pDataSummary->NumOfemy;
    if (Obst_t_l > 10)
        Obst_t_l = 10;
    for (i = 0; i < Obst_t_l; i++)
    {
        Obst_t_x[i] = pDataSummary->t_enemy[i].x / film_zoom_time;
        Obst_t_y[i] = pDataSummary->t_enemy[i].y / film_zoom_time;
        a2[i] = atan2(ball_f_x - Obst_t_x[i], ball_f_y - Obst_t_y[i]);
        d1[i] = sqrt((ball_f_x - Obst_t_x[i]) * (ball_f_x - Obst_t_x[i]) +
                     (ball_f_y - Obst_t_y[i]) * (ball_f_y - Obst_t_y[i]));
    }
    if (0) // 爆发测试 所有点都计算的 运算量测试
    {
        for (i = 0; i < AG_NUM; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                pDataSummary->agent[i].obstacle[j].valid =
                    true; //&&(m_cObst[i].x<1800)&&(m_cObst[i].x>0)&&(m_cObst[i].y<1200)&&(m_cObst[i].y>0))
            }
        }
    }
    if (0) // 爆发测试 所有点都计算的 运算量测试
    {
        for (i = 0; i < AG_NUM; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                pDataSummary->agent[i].obstacle[j].valid =
                    0; //&&(m_cObst[i].x<1800)&&(m_cObst[i].x>0)&&(m_cObst[i].y<1200)&&(m_cObst[i].y>0))
            }
        }
    }

    for (int l = 0; l < 5; l++)
        if (1)
        {
            double max_dis_x = 0;
            double max_dis_y = 0;
            double max_diss = 0;
            for (y = enable_catch_area_min_y; y < enable_catch_area_max_y; y++)
            {
                for (x = enable_catch_area_min_x; x < enable_catch_area_max_x; x++)
                {
                    if (x < catch_area_min_x_base)
                        continue;
                    j = 10000;
                    for (i = 0; i < Obst_t_l; i++)
                    {
                        k = sqrt((Obst_t_x[i] - x) * (Obst_t_x[i] - x) + (Obst_t_y[i] - y) * (Obst_t_y[i] - y));
                        if (j > k)
                            j = (float)k;
                    }
                    for (i = 0; i < l; i++)
                    {
                        k = sqrt((max_dis_rx[i] - x) * (max_dis_rx[i] - x) +
                                 (max_dis_ry[i] - y) * (max_dis_ry[i] - y)) *
                            1.3;
                        if (j > k)
                            j = (float)k;
                    }
                    Obst_dis_map[y][x] = j;
                    if (max_diss < j)
                    {
                        max_diss = j;
                        max_dis_x = (float)x;
                        max_dis_y = (float)y;
                    }
                }
            }
            max_dis_rx[l] = max_dis_x;
            max_dis_ry[l] = max_dis_y;
            //		if (x=)
        }

    if (1)
    {
        //	max_dis=0;
        for (y = enable_catch_area_min_y; y < enable_catch_area_max_y; y++)
        {
            for (x = enable_catch_area_min_x; x < enable_catch_area_max_x; x++)
            {

                j = 10000; // img->width+img->height;
                //	for(i=0;i<80;i++)
                for (i = 0; i < Obst_t_l; i++)
                {
                    if (1) //(m_cObst[i].valid ==
                           // true))//&&(m_cObst[i].x<1800)&&(m_cObst[i].x>0)&&(m_cObst[i].y<1200)&&(m_cObst[i].y>0))
                    {

                        a1 = atan2(x - Obst_t_x[i], y - Obst_t_y[i]);
                        //	a2=atan2(ball_f_x-Obst_t_x[i], ball_f_y-Obst_t_y[i]);
                        ao = a1 - a2[i];
                        ao = cos(ao);
                        //	ao=ao/3.141;
                        k = sqrt((Obst_t_x[i] - x) * (Obst_t_x[i] - x) + (Obst_t_y[i] - y) * (Obst_t_y[i] - y));
                        //	k=sqrt((m_cObst[i].x/film_zoom_time-x)*(m_cObst[i].x/film_zoom_time-x)+(m_cObst[i].y/film_zoom_time-y)*(m_cObst[i].y/film_zoom_time-y));

                        //	k1=sqrt(
                        //(m_cObst[i].x/film_zoom_time-x)*(m_cObst[i].x/film_zoom_time-x)+(m_cObst[i].y/film_zoom_time-y)*(m_cObst[i].y/film_zoom_time-y)
                        //); 	k2=sqrt(
                        //(m_cObst[i].x/film_zoom_time-ball_f_x)*(m_cObst[i].x/film_zoom_time-ball_f_x)+(m_cObst[i].y/film_zoom_time-ball_f_y)*(m_cObst[i].y/film_zoom_time-ball_f_y)
                        //); 	k3=sqrt( (ball_f_x-x)*(ball_f_x-x)+(ball_f_y-y)*(ball_f_y-y) );
                        //	cos=(k1*k1+k2*k2+k3*k3)/(2*k1*k2);
                        //						k=(float)k*(((float)ao+1)+0.5);
                        t1 = (float)k;
                        t2 = (float)ao;
                        //	t1=t1*(t2+1.5);
                        t1 = (float)t1 * (t2 + 1.2);

                        //		//	if (d1[i]<181/film_zoom_time)
                        //		//		j=j*()

                        if (j > t1)
                            j = t1;
                        //	temp=m_cObst[i].y/film_zoom_time*300 + m_cObst[i].x/film_zoom_time;
                        //	poi[temp+1]=0;	poi[temp+2]=0;	poi[temp+3]=0;	poi[temp-1]=0;	poi[temp-2]=0;
                        // poi[temp-3]=0;	poi[temp-300]=0;	poi[temp-600]=0;	poi[temp-900]=0;	poi[temp+300]=0;
                        // poi[temp+600]=0;	poi[temp+900]=0;
                    }
                }
                if (1) // 对球为中心400cm内的范围不传球
                {
                    double jt = sqrt((ball_f_x - x) * (ball_f_x - x) + (ball_f_y - y) * (ball_f_y - y));
                    if (jt < 400 / film_zoom_time)
                        jt = 0;
                    else
                        jt = (jt - 400 / film_zoom_time) * 1.1; //
                    if (j > jt)
                        j = jt;
                }
                if (1) // 加入尽量向对方门那边传的趋势
                {
                    //					float jt=sqrt((ball_f_x-x)*(ball_f_x-x)+(ball_f_y-y)*(ball_f_y-y));
                    //		float jt=(x-900/film_zoom_time)*1.0;
                    //		float jt=(x-enable_catch_area_min_x)*1.0;
                    //		float jt=(x-(ball_f_x-180/film_zoom_time))*1.0;
                    double jt = (x - catch_area_min_x_base) * 1.0;

                    //	if (jt<400/film_zoom_time)
                    //		jt=0;
                    //	else
                    //		jt=(jt-400/film_zoom_time)*1.1;//
                    if (j > jt)
                        j = jt;
                }

                //			if(max_dis<j)
                //			{   max_dis=j;
                //				max_dis_x=x;
                //				max_dis_y=y;
                //			}
                //			temp=y*img->width + x;
                //			poi[temp] = j*10;
                //				k=sqrt((ball_f_x-x)*(ball_f_x-x)+(ball_f_y-y)*(ball_f_y-y));
                //				if
                Obst_map_data[y][x] = j;
            }
        }
    }

    /*
    t1=expect_catch_x-max_dis_x_p;	//如果 周围没有障碍物机器就向 希望靠近什么位置接球
    if (t1> 1)		t1= 1;
    if (t1<-1)		t1=-1;
    t2=expect_catch_y-max_dis_y_p;
    if (t2> 1)		t2= 1;
    if (t2<-1)		t2=-1;
    max_dis_x_p+=t1;
    max_dis_y_p+=t2;
    */
    /*	for		(y=enable_catch_area_min_y; y<enable_catch_area_max_y; y++)
    {	for	(x=enable_catch_area_min_x; x<enable_catch_area_max_x; x++)
    {
    k=sqrt((max_dis_x_p-x)*(max_dis_x_p-x)+(max_dis_y_p-y)*(max_dis_y_p-y));
    if (k<30)//300/film_zoom_time
    Obst_map_data[y][x]+=450/film_zoom_time * (300/film_zoom_time-k)/(300/film_zoom_time);
    //	Obst_map_data[y][x]+=45.0 * (30.0-k)/(30.0);
    }
    }

    for		(y=enable_catch_area_min_y; y<enable_catch_area_max_y; y++)
    {	for	(x=enable_catch_area_min_x; x<enable_catch_area_max_x; x++)
    {
    k=sqrt((expect_catch_x-x)*(expect_catch_x-x)+(expect_catch_y-y)*(expect_catch_y-y));
    Obst_map_data[y][x]+=500/film_zoom_time * (800/film_zoom_time-k)/(800/film_zoom_time);
    }
    }
    */

    max_dis = 0;
    max_dis_x = enable_catch_area_min_x;
    max_dis_y = (enable_catch_area_min_y + enable_catch_area_max_y) / 2;
    max_dis = Obst_map_data[max_dis_y][max_dis_x];
    for (y = enable_catch_area_min_y; y < enable_catch_area_max_y; y++)
    {
        for (x = enable_catch_area_min_x; x < enable_catch_area_max_x; x++)
        {
            j = Obst_map_data[y][x];
            if (max_dis < j)
            {
                max_dis = j;
                max_dis_x = x;
                max_dis_y = y;
            }
        }
    }
    double area_weight[3] = {0, 0, 0};
    double area_weight_t = 0;
    for (i = 0; i < 3; i++)
    {
        double y1 = (enable_catch_area_max_y - enable_catch_area_min_y) * i / 3.0;
        double y2 = (enable_catch_area_max_y - enable_catch_area_min_y) * (i + 1) / 3.0;
        area_weight_t = 0;
        for (y = (int)y1; y < y2; y++)
        {
            for (x = enable_catch_area_min_x; x < enable_catch_area_max_x; x++)
            {
                area_weight_t += Obst_map_data[y][x];
            }
        }
        area_weight[i] = area_weight_t;
    }
    //	m_tacagent[ID_Catch].x...;
    //	m_tacagent[ID_Catch].y;
    float aaa = 1;                // 0.5;
    t1 = max_dis_x - max_dis_x_p; //
    if (t1 > aaa)
        t1 = aaa;
    if (t1 < -aaa)
        t1 = -aaa;
    t2 = max_dis_y - max_dis_y_p;
    if (t2 > aaa)
        t2 = aaa;
    if (t2 < -aaa)
        t2 = -aaa;

    //	if (m_bSomeOneMoveKick == true)
    //	{	waitCatchTime=6;
    //	}
    //	else
    //	{	waitCatchTime--;
    //	}
    //	if((m_bSomeOneMoveKick == true)||(waitCatchTime>0))
    if (pDataSummary->bPassing == true) // 0)//
    {
        max_dis_x = posCatch[0].x / film_zoom_time;
        max_dis_y = posCatch[0].y / film_zoom_time;
        if (max_dis_x > 1800 / film_zoom_time)
            max_dis_x = 1800 / film_zoom_time;
        if (max_dis_x < 900 / film_zoom_time)
            max_dis_x = 900 / film_zoom_time;
        if (max_dis_y > 1200 / film_zoom_time)
            max_dis_y = 1200 / film_zoom_time;
        if (max_dis_y < 0 / film_zoom_time)
            max_dis_y = 0 / film_zoom_time;
    }
    else
    {
        max_dis_x = (int)(max_dis_x_p + t1);
        max_dis_y = (int)(max_dis_y_p + t2);
    }

    int catch_id;
    catch_id = m_Ag_GetStatus(CTRL_CATCH_MOVE, pDataSummary->agent);
    if (catch_id < 0)
    {
        catch_id = m_Ag_GetStatus(CTRL_CATCH_FOCUS, pDataSummary->agent);
    }
    if (catch_id >= 0 && catch_id <= 4)
    {
        int catch_robot_fx = pDataSummary->agent[catch_id].x;
        int catch_robot_fy = pDataSummary->agent[catch_id].y;
        if (catch_robot_fy > 1200) // 测试代码
            catch_robot_fy = catch_robot_fy;
        if (catch_robot_fx > 960)
        {
            double catch_angle =
                atan2(catch_robot_fx - max_dis_x * film_zoom_time, catch_robot_fy - max_dis_y * film_zoom_time);
            double dis_ball2catch =
                sqrt((catch_robot_fx - pDataSummary->cGlobalBall.x) * (catch_robot_fx - pDataSummary->cGlobalBall.x) +
                     (catch_robot_fy - pDataSummary->cGlobalBall.y) * (catch_robot_fy - pDataSummary->cGlobalBall.y));
            double catch_radius = dis_ball2catch * Catch_Radius_ratio;
            if (catch_radius > Catch_Radius_SET)
                catch_radius = Catch_Radius_SET;

            double dis_catch =
                sqrt((catch_robot_fx - max_dis_x * film_zoom_time) * (catch_robot_fx - max_dis_x * film_zoom_time) +
                     (catch_robot_fy - max_dis_y * film_zoom_time) * (catch_robot_fy - max_dis_y * film_zoom_time));
            if (dis_catch > catch_radius)
            {
                double dx = -sin(catch_angle) * catch_radius;
                double dy = -cos(catch_angle) * catch_radius;
                max_dis_x = (int)((catch_robot_fx + dx) / film_zoom_time);
                max_dis_y = (int)((catch_robot_fy + dy) / film_zoom_time);
            }
        }
        else
        {
            max_dis_x = 1100 / film_zoom_time;
            max_dis_y = catch_robot_fy / film_zoom_time;
        }
    }
    max_dis_x_p = max_dis_x;

    max_dis_y_p = max_dis_y;

    if (cvshow == true)
    {
        IplImage *img = cvCreateImage(cvSize(Obst_map_width, Obst_map_height), 8, 3);
        img->origin = 1; // 1 - bottom-left origin (Windows bitmaps style)
        unsigned char *poi = (unsigned char *)(img->imageData);
        for (i = 0; i < img->width * img->height * 3; i++)
        {
            poi[i] = (char)255;
        }
        for (y = enable_catch_area_min_y; y < enable_catch_area_max_y; y++)
        {
            for (x = enable_catch_area_min_x; x < enable_catch_area_max_x; x++)
            {
                int tmpIndex = Obst_map_data[y][x] * 10;
                if (tmpIndex > 1999)
                {
                    tmpIndex = 1999;
                }
                poi[y * img->widthStep + x * 3 + 0] = PixColor[tmpIndex][0];
                poi[y * img->widthStep + x * 3 + 1] = PixColor[tmpIndex][1];
                poi[y * img->widthStep + x * 3 + 2] = PixColor[tmpIndex][2];
                // poi[y*img->widthStep + x * 3 + 0] = 0;//(char)Obst_map_data[y][x]*60;//*6
                // poi[y*img->widthStep + x * 3 + 1] = (char)Obst_map_data[y][x] * 60;//*6
                // poi[y*img->widthStep + x * 3 + 2] = 0;//(char)Obst_map_data[y][x]*60;//*6
            }
        }
        if (1) // 显示障碍物
        {
            for (i = 0; i < AG_NUM; i++)
            {
                for (int j = 0; j < 10; j++)
                {
                    if ((pDataSummary->agent[i].obstacle[j].valid ==
                         true)) //&&(m_cObst[i].x/film_zoom_time<(img->width-4))&&(m_cObst[i].x/film_zoom_time>(0+4))&&(m_cObst[i].y/film_zoom_time<(img->height-4))&&(m_cObst[i].y/film_zoom_time>(0+4)))
                    {
                        cvCircle(img,
                                 cvPoint(int(pDataSummary->agent[i].obstacle[j].x / film_zoom_time),
                                         int(pDataSummary->agent[i].obstacle[j].y / film_zoom_time)),
                                 1, cvScalar(190, 190, 190, 0), 2, 8, 0);
                        //				temp=m_cObst[i].y/film_zoom_time*img->width + m_cObst[i].x/film_zoom_time;
                        //				poi[temp+1]=0;	poi[temp+2]=0;	poi[temp+3]=0;	poi[temp-1]=0;	poi[temp-2]=0;
                        // poi[temp-3]=0;	poi[temp-img->width]=0;	poi[temp-img->width*2]=0;
                        // poi[temp-img->width*3]=0; poi[temp+img->width]=0;	poi[temp+img->width*2]=0;
                        // poi[temp+img->width*3]=0;
                    }
                }
            }
            for (i = 0; i < Obst_t_l; i++)
            {
                cvCircle(img, cvPoint(int(Obst_t_x[i]), int(Obst_t_y[i])), 1, cvScalar(0, 0, 0, 0), 2, 8, 0);
            }
        }
        if (1) // 显示目标移动位置
        {
            if (1) //(max_dis_x<(img->width-4))&&(max_dis_x>(0+4))&&(max_dis_y<(img->height-4))&&(max_dis_y>(0+4)))
            {
                cvCircle(img, cvPoint(int(ball_f_x), int(ball_f_y)), 1, cvScalar(0, 0, 255, 0), 2, 8, 0);
                cvLine(img, cvPoint(int(max_dis_x), int(max_dis_y)), cvPoint(int(ball_f_x), int(ball_f_y)),
                       cvScalar(128, 128, 128, 0), 2, 8, 0);
                cvCircle(img, cvPoint(int(max_dis_x), int(max_dis_y)), 1, cvScalar(255, 255, 0, 0), 2, 8, 0);
                //	temp=max_dis_y*img->width + max_dis_x;
                //	poi[temp+1]=0;	poi[temp+2]=0;	poi[temp+3]=0;	poi[temp-1]=0;	poi[temp-2]=0;	poi[temp-3]=0;
                // poi[temp-img->width]=0;	poi[temp-img->width*2]=0;	poi[temp-img->width*3]=0;
                // poi[temp+img->width]=0; poi[temp+img->width*2]=0;	poi[temp+img->width*3]=0;
            }
        }
        if (0)
        {
            pDataSummary->agent[0].obstacle[1].x = 300;
            pDataSummary->agent[0].obstacle[1].y = 300;
            temp = pDataSummary->agent[0].obstacle[1].y / 6 * 300 + pDataSummary->agent[0].obstacle[1].x / 6; //=30150
            poi[temp] = 0;
            poi[temp + 1] = 0;
            poi[temp + 2] = 0;
            poi[temp + 3] = 0;
            poi[temp - 1] = 0;
            poi[temp - 2] = 0;
            poi[temp - 3] = 0;
            poi[temp - 300] = 0;
            poi[temp - 600] = 0;
            poi[temp - 900] = 0;
            poi[temp + 300] = 0;
            poi[temp + 600] = 0;
            poi[temp + 900] = 0;
        }

        IplImage *imag_dis = cvCreateImage(cvSize(Obst_map_width, Obst_map_height), 8, 3);
        imag_dis->origin = 1; // 1 - bottom-left origin (Windows bitmaps style)
        poi = (unsigned char *)(imag_dis->imageData);
        for (i = 0; i < img->width * img->height * 3; i++)
        {
            poi[i] = (char)255;
        }
        for (y = enable_catch_area_min_y; y < enable_catch_area_max_y; y++)
        {
            for (x = enable_catch_area_min_x; x < enable_catch_area_max_x; x++)
            {
                int tmpIndex = Obst_dis_map[y][x] * 10;
                if (tmpIndex > 1999)
                {
                    tmpIndex = 1999;
                }
                poi[y * imag_dis->widthStep + x * 3 + 0] = PixColor[tmpIndex][0];
                poi[y * imag_dis->widthStep + x * 3 + 1] = PixColor[tmpIndex][1];
                poi[y * imag_dis->widthStep + x * 3 + 2] = PixColor[tmpIndex][2];
                // poi[y*imag_dis->widthStep + x * 3 + 0] = 0;//(char)Obst_map_data[y][x]*60;//*6
                // poi[y*imag_dis->widthStep + x * 3 + 1] = (char)Obst_dis_map[y][x] * 60;//*6
                // poi[y*imag_dis->widthStep + x * 3 + 2] = 0;//(char)Obst_map_data[y][x]*60;//*6
            }
        }
        if (1) // 显示障碍物
        {
            for (i = 0; i < AG_NUM; i++)
            {
                for (int j = 0; j < 10; j++)
                {
                    if ((pDataSummary->agent[i].obstacle[j].valid ==
                         true)) //&&(m_cObst[i].x/film_zoom_time<(img->width-4))&&(m_cObst[i].x/film_zoom_time>(0+4))&&(m_cObst[i].y/film_zoom_time<(img->height-4))&&(m_cObst[i].y/film_zoom_time>(0+4)))
                    {
                        cvCircle(imag_dis,
                                 cvPoint(int(pDataSummary->agent[i].obstacle[j].x / film_zoom_time),
                                         int(pDataSummary->agent[i].obstacle[j].y / film_zoom_time)),
                                 1, cvScalar(190, 190, 190, 0), 2, 8, 0);
                    }
                }
            }
            for (i = 0; i < Obst_t_l; i++)
            {
                cvCircle(imag_dis, cvPoint(int(Obst_t_x[i]), int(Obst_t_y[i])), 1, cvScalar(0, 0, 0, 0), 2, 8, 0);
            }
            for (i = 0; i < 5; i++)
            {
                cvCircle(imag_dis, cvPoint(int(max_dis_rx[i]), int(max_dis_ry[i])), 5, cvScalar(255, 0, 0, 0), 2, 8, 0);
            }
        }

        cvShowImage("重心", img);      // 显示图片
        cvShowImage("距离", imag_dis); // 显示图片

        cvReleaseImage(&img);
        cvReleaseImage(&imag_dis);
    }

    max_dis_x = max_dis_x * film_zoom_time; // 用于输出的变量
    max_dis_y = max_dis_y * film_zoom_time;

    // 第一个接球位置（共三个）
    posCatch[0].x = max_dis_x; // 1350;//
    posCatch[0].y = max_dis_y; // 600 ;//

    // 第二个接球位置(这里瞎填测试用)
    posCatch[1].x = 1200;
    posCatch[1].y = 400;

    // 第三个接球位置(这里瞎填测试用)
    posCatch[2].x = 1200;
    posCatch[2].y = 800;
}
