// PathLayout.cpp

#include "stdafx.h"
#include "math.h"

#ifndef PathLayout_cpp
#define PathLayout_cpp

#include "PathLayout.h"

long int area_grow_dataA[240][320];
bool area_grow_datao[240][320];
unsigned int area_grow_dataAtcx[10000]; // currently当前生长的一圈的纪录
unsigned int area_grow_dataAtcy[10000]; // currently当前生长的一圈的纪录
unsigned int area_grow_dataAtcl = 0;    // 纪录currently当前这一圈有几个像素
unsigned int area_grow_dataAtpx[10000]; // pre-	上一次生长的一圈的纪录
unsigned int area_grow_dataAtpy[10000]; // pre-	上一次生长的一圈的纪录
unsigned int area_grow_dataAtpl = 0;    // 纪录pre-	上一次这一圈有几个像素
int pathx[10000];                       // 存储路径的 顺序坐标
int pathy[10000];
int pathl = 0;
int pathxII[1000]; // 用线段 拟和后的路径 端点坐标
int pathyII[1000];
int pathlII;

const int radial_cx = 160;                                                   // 射线中心x
const int radial_cy = 120;                                                   // 射线中心y
const int orm_radial_num = 144;                                              // 射线条数
const int orm_radial_max_radius = 120;                                       // 50;//120;//射线最大半径
const int orm_radial_min_radius = 14;                                        // 12;//19;//10;//射线最小半径
const float radial_spacing = (3.14 / 180) * (360 / (float)(orm_radial_num)); // 计算每条线的间距
int radial_lm[orm_radial_num][orm_radial_max_radius];                        //[radial_max_radius-radial_min_radius];
int radial_lm_view[sxtiaoshu][sxradius_max];                                 // test

//;//存储障碍 位置 地图
int obstacleRadialMap[sxtiaoshu][sxradius_max];    // 存储障碍 位置 地图
int obstacleRadial_A_Map[sxtiaoshu][sxradius_max]; // 存储有行走权值的 地图

// 场线匹配用 颜色设置值带入函数
void CPathLayout::path_layout_black_green_color_set(int blackSmax, int blackVmax, int greenHmax, int greenHmin,
                                                    int greenSmax, int greenSmin, int greenVmax, int greenVmin)
{
    FLblackSmax = blackSmax;
    FLblackVmax = blackVmax;
    FLgreenHmax = greenHmax;
    FLgreenHmin = greenHmin;
    FLgreenSmax = greenSmax;
    FLgreenSmin = greenSmin;
    FLgreenVmax = greenVmax;
    FLgreenVmin = greenVmin;
}

void CPathLayout::make_radial_mask(void)
{ /*
 int radial_cx=160;//射线中心x
 int radial_cy=120;//射线中心y
 int radial_num=120;//射线条数
 int radial_max_radius=100;//50;//120;//射线最大半径
 int radial_min_radius=10;//10;//射线最小半径
 float radial_spacing=(3.14/180)*(360/(float)(radial_num));//计算每条线的间距
 int	radial_lm[radial_num][radial_max_radius-radial_min_radius];
 int	radial_lm_view[120][120];//test
 */
    // 一圈射线的掩膜%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    int r, du; // 视野中心在 x122 y120

    for (du = 0; du < orm_radial_num; du = du + 1)
    {
        for (r = 0; r < orm_radial_max_radius; r++) // 半径50以下是机体不算
        {
            radial_lm[du][r] = 0;
        }
    }
    // for(du=0;du<radial_num;du=du+1)
    //	for(r=radial_min_radius;r<radial_max_radius;r++)//半径50以下是机体不算
    //	{
    // radial_lm[du][r]=320*((int)(cos(du*radial_spacing)*r)+radial_cy)+((int)(sin(du*radial_spacing)*r)+radial_cx);//z=(320*y)+x;
    //	}
    for (du = 0; du < orm_radial_num; du = du + 1) // 一条长一条短的射线		也可以1长 2小短 4小小短
    {
        for (r = orm_radial_min_radius; r < orm_radial_max_radius; r++) // 半径50以下是机体不算
        {
            radial_lm[du][r] = 320 * ((int)(cos(du * radial_spacing) * r) + radial_cy) +
                               ((int)(sin(du * radial_spacing) * r) + radial_cx); // z=(320*y)+x;
        }
    }
    /// 完成射线的掩膜%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // 测试代码
    for (du = 0; du < orm_radial_num; du++)
    {
        for (r = 0; r < orm_radial_max_radius; r++) // 半径50以下是机体不算
        {
            radial_lm_view[du][r] = radial_lm[du][r];
        }
    }

    // 初始化 记录
    direct_history = 0; // 存储上次规划结果方向的信息	//只在path_generateIIII中记录	//在black_p_detect2_d2_circle_2
                        // path_generateIIII中调用
    path_result_history =
        0; // 存储上次规划    失败 原因纪录 //只在path_generateIIII中记录   //在black_p_detect2_d2_circle_2  中调用
    black_circle_zoom =
        1; // 存储规划    失败 后对障碍进行 缩小 便于在新的周期中可以找到路径//只在在black_p_detect2_d2_circle_2中改变
           // //在black_p_detect2_d2_circle_2  中调用
    black_circle_zoom_timeloop =
        0; // 存储 需要进行 缩小的障碍的时间 以求这段时间可以冲出 不可行走区域	//只在path_generateIIII中记录
           // //在black_p_detect2_d2_circle_2  path_generateIIII中调用
}

float orm_radial_r[orm_radial_num][120];
const int orm_radial_inrstart = 1; //;10;//[10~80]查找表最小输入半径
const int orm_radial_inrend = 80;  //[10~80]查找表最大输入半径
// const float zoomtime=5.0;//5.0;//缩放大小 缩小时是大于1的数字
const float zoomtime = 8.0;                                               // 5.0;//缩放大小 缩小时是大于1的数字
const float orm_radial_inr[orm_radial_inrend - orm_radial_inrstart + 1] = //[10~80]查找表实际数组
    {4,   8,   12,  16,  20,  24,  28,  32,  36,  40,  45,  50,  55,  57,  60,  65,  70,  73,  77,  80,
     83,  86,  90,  93,  96,  100, 105, 108, 110, 114, 118, 122, 125, 128, 131, 134, 140, 144, 147, 150,
     155, 160, 164, 168, 172, 176, 182, 189, 196, 204, 210, 215, 221, 228, 235, 242, 248, 255, 262, 271,
     283, 290, 304, 317, 330, 342, 356, 370, 385, 400, 418, 433, 450, 468, 480, 500, 520, 540, 560, 580};

void CPathLayout::make_field_line_rectify_tab(void)
{
    int i, j;
    // 长度对照表 用来校准 镜子的曲率
    // 这个对照表作用：
    // 输入点在射线掩膜上的位置 得到此点在 校正过的平直图象上的位置x和y
    // 暂时先用 1：1来算	先生成一个假的表
    for (i = 0; i < orm_radial_num; i++)
    {
        for (j = 0; j < 120; j++)
        {
            orm_radial_r[i][j] =
                j; // orm_radial_inrend是跟CFieldLocalisation不一样的地方 注意0这个输出距离特殊 规定这个点肯定无效
        }
    }
    for (i = 0; i < orm_radial_num; i++)
    {
        for (j = orm_radial_inrstart; j < orm_radial_inrend + 1; j++)
        {
            orm_radial_r[i][j] = orm_radial_inr[j - orm_radial_inrstart];
        }
    }
}

// 畸变校正查表程序  会线性补足查找表里<10的项    >80的点全部为=80缺失
inline float CPathLayout::field_line_rectify(int line_num, int radius)
{
    int i;
    if (radius < orm_radial_inrstart)
    {
        return ((float)(orm_radial_r[line_num][orm_radial_inrstart] * radius) / (float)(orm_radial_inrstart) /
                zoomtime); //
    }
    if (radius > orm_radial_inrend)
    {
        return ((float)(orm_radial_r[line_num][orm_radial_inrend]) / zoomtime);
    }
    return ((float)(orm_radial_r[line_num][radius]) / zoomtime);
}

// void black_p_detect(unsigned char *data);
// Bool_ProcessIII_W_VHS_v(unsigned char *data,int i)
// Bool_ProcessVI_VHS_P(unsigned char *data,int i)
int blackpoingmap[orm_radial_num];       // 存储条射线 第一个连续5个点的遮挡物的第一个点
int blackpoing_temp_map[orm_radial_num]; // 作连通分析时暂存用
void CPathLayout::black_p_detect2_d1(unsigned char *data,
                                     int *blackpoingmapout) // 测出每条射线第一个连续5个点的遮挡物的第一个点
{
    int i, j, srbpn; // 单条 线 上以纪录连续 黑点 个数
    // int bprs;//黑点开始点半径
    for (i = 0; i < orm_radial_num; i++)
    {
        srbpn = 0;
        blackpoingmapout[i] = orm_radial_max_radius - 1;
        for (j = orm_radial_min_radius; j < orm_radial_max_radius; j++)
        {
            if (Bool_ProcessIII_B_VHS_v(data, radial_lm[i][j] * 3) == 0)
            {
                srbpn++;
            }
            else
            {
                srbpn = 0;
            }
            if (srbpn >= 5) // 如果有连续3个黑点说明这里有障碍
            {
                blackpoingmapout[i] = j - 3;
                break;
            }
        }
    }
}

inline float radius_num_add(float a, float b)
{
    float c = a + b;
    if (c < -orm_radial_num / 2) //<-180
        return (c + orm_radial_num);
    if (c > orm_radial_num / 2) //>+180
        return (c - orm_radial_num);
    return (c);
}
inline float radius_num_add360(float a, float b)
{
    float c = a + b;
    if (c < 0) //<-180
        return (c + orm_radial_num);
    if (c > orm_radial_num) //>+180
        return (c - orm_radial_num);
    return (c);
}

const int black_circle_zoom_maxtimeloop_set = 3; // 设置 当全部路径堵死后 进行黑色圆 缩小 再次出现路径后持续缩小的时间
const float black_circle_zoom_time_one_loop_set = 0.1; // 设置 当全部路径堵死后 进行黑色圆 每周期缩小 百分比  原大小1
const float black_circle_min_zoom_time_set = 0.15;     // 设置 当全部路径堵死后 进行黑色圆 缩小 最小百分比  原大小1

void CPathLayout::black_p_detect2_d2_circle_2(int *blackpoingmapin,
                                              float min_radius) // 测出每条射线第一个连续5个点的遮挡物的第一个点
{
    int i, j, n, r, tempr, fa = 0;
    for (i = 0; i < orm_radial_num; i++) // 开始扩边工作
    {
        for (j = orm_radial_min_radius; j < orm_radial_max_radius; j++)
        {
            obstacleRadialMap[i][j] = 1;
        }
    }
    // 注意对blackpoingmapin[i]作连通分析看左右都有黑色没有 如果才接着做后面的
    //
    // 黑色检查 B型
    for (i = 0; i < orm_radial_num; i++)
    {
        blackpoing_temp_map[i] = orm_radial_max_radius - 1;
        if (blackpoingmapin[i] != orm_radial_max_radius - 1)
        {
            fa = 0;
            for (r = -2; r <= 2; r++) // 这里也可以判断 最近的几条线里有 几个是有效的线
            {
                if (abs(blackpoingmapin[(int)radius_num_add360(i, r)] - blackpoingmapin[i]) <= 5)
                {
                    fa++;
                }
            }
            if (fa >= 4)
            {
                blackpoing_temp_map[i] = blackpoingmapin[i];
            }
            else
            {
                blackpoing_temp_map[i] = orm_radial_max_radius - 1;
            }
        }
    }
    for (i = 0; i < orm_radial_num; i++) // 开始扩边工作
    {
        blackpoingmapin[i] = blackpoing_temp_map[i];
    }
    // int r,tempr;
    // for (i=0;i<orm_radial_num;i++)//测试代码 设定需要位置的障碍
    //{	blackpoingmapin[i]=orm_radial_max_radius-1;//119;
    // }	blackpoingmapin[orm_radial_num-40 ]=17;//15;//144*0/100
    //	blackpoingmapin[orm_radial_num-10]=18;//15;//144*0/100
    //	blackpoingmapin[orm_radial_num-20]=19;//15;//144*0/100

    // 施密特 方向变化速度限定  之前是障碍的T时间内都是障碍
    // 施密特 在小的范围里是可以快速变化的 但到大的范围就出现施密特特性曲线
    // 设置出发点不是一个正圆形 是凹槽偏向上次方向的心型
    /*测试代码*/ // float  mradius;//=12;//24;//22;//16;//20;//15;//测试规避设置  机体半径 单位像素 测试时注释掉下面一行
    float mradius[orm_radial_num];
    if (black_circle_zoom_timeloop < black_circle_zoom_maxtimeloop_set)
    {
        black_circle_zoom_timeloop++;
    }
    if (path_result_history != 1)
    {
        black_circle_zoom_timeloop = 0;
    }
    if ((path_result_history == 1) &&
        (black_circle_zoom_timeloop >=
         black_circle_zoom_maxtimeloop_set)) // 存储上次规划    失败 原因纪录	//0默认无状态  1成功 2失败一开始点黑色
                                             // 3失败二目标点黑色 4找不到路径,超过maxGrownTime1500点还没到达目标点
                                             // 5生长完连通域还没生长到目标点 失败 6失败一开始点出了范围[320*240]
                                             // 7失败二目标点出了范围[320*240]
    {

        black_circle_zoom = 1; // 存储规划    失败 后对障碍进行 缩小
                               // 便于在新的周期中可以找到路径//只在在black_p_detect2_d2_circle_2中改变
                               // //在black_p_detect2_d2_circle_2  中调用
        for (i = 0; i < orm_radial_num / 2; i++) // 开始扩边工作
        {
            mradius[int(radius_num_add360(direct_history, i))] =
                min_radius + min_radius * 1.5 * ((float)(i) * 2 / (float)(orm_radial_num));
            mradius[int(radius_num_add360(direct_history, -i))] =
                min_radius + min_radius * 1.5 * ((float)(i) * 2 / (float)(orm_radial_num));
        }
    }
    else
    {
        if ((black_circle_zoom > black_circle_min_zoom_time_set) &&
            (path_result_history != 1)) // 设置 当全部路径堵死后 进行黑色圆 缩小 最小百分比  原大小1
        {
            black_circle_zoom =
                black_circle_zoom - 0.1; // 存储规划    失败 后对障碍进行 缩小
                                         // 便于在新的周期中可以找到路径//只在在black_p_detect2_d2_circle_2中改变
                                         // //在black_p_detect2_d2_circle_2  中调用
        }
        for (i = 0; i < orm_radial_num / 2; i++) // 开始扩边工作
        {
            mradius[int(radius_num_add360(direct_history, i))] =
                (min_radius + min_radius * 1.5 * ((float)(i) * 2 / (float)(orm_radial_num))) * black_circle_zoom;
            mradius[int(radius_num_add360(direct_history, -i))] =
                (min_radius + min_radius * 1.5 * ((float)(i) * 2 / (float)(orm_radial_num))) * black_circle_zoom;
        }
    }

    // float  pradius=0.7;//0.9;//0.8;//规避机体远障碍的距离 是机体半径的多少倍	圆周方向   法向
    // float  p_distance=0.19;//1.5;//规避机体远障碍的距离 是机体半径的多少倍   半径方向  径向
    float pdistance = 15 / zoomtime;     //[厘米/]4;//8;//0.8;//1.5;//规避机体远障碍的距离 是机体半径的多少倍   半径方向
                                         // 径向  此值越大规避越近是一个偏移量
    float nearest_obviate_angle = 0.35;  // 最近时机体可以以什么角度躲避
    float circle_obviate_angle = 200;    // 140;//障碍 扩展半径的包括角度
    for (i = 0; i < orm_radial_num; i++) // 开始扩边工作q
    {
        if (blackpoingmapin[i] <
            orm_radial_max_radius - 1) //(blackpoingmapin[i]<orm_radial_max_radius-1-20)//如果是到特殊半径
                                       // 即最大半径 说明此射线上无障碍可以不做扩边
        {                              // 55为机身宽度[厘米]
            int blackpoingmapintemp = blackpoingmapin[i];
            blackpoingmapin[i] = (int)(field_line_rectify(
                i, blackpoingmapin[i])); // 畸变校正查表程序  会线性补足查找表里<10的项    >80的点全部为=80缺失
            if (blackpoingmapin[i] < 2)  // 防止出现除出小与一 半径为零 的情况
                blackpoingmapin[i] = 2;
            if (blackpoingmapin[i] > orm_radial_max_radius - 1) // 防止出现掩膜溢出
                blackpoingmapin[i] = orm_radial_max_radius - 1;
            float point_angle_c, point_radius_c; // 极坐标 下
            float point_angle;                   // 当前绘制点角度
            float point_angle_d;                 // 每个绘制点的角度差
            float point_x, point_y;
            //		float radius=10;
            //		point_angle_d = 360/(2*3.14*radius);
            point_angle_d = 18;        // 36;///(1000);
            float Ppoint_angle_c = 0;  // 上每两点间连直线时纪录上一点位置
            float Ppoint_radius_c = 0; // 上每两点间连直线时纪录上一点位置
            bool Ppoint1 = 0;          // 标志第一个点初始化问题
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////画半周圆
            for (point_angle = 90 - circle_obviate_angle / 2; point_angle <= 90;
                 point_angle += point_angle_d) // 2*3.14*radius
            //		for(point_angle=90-circle_obviate_angle/2+point_angle_d*5;point_angle<=90-circle_obviate_angle/2+point_angle_d*6;point_angle+=point_angle_d)//2*3.14*radius
            {
                float point_angle_t = point_angle + radius_num_add360(i, 0) * 360 / orm_radial_num;
                point_x = cos(point_angle_t * 3.14 / 180) * mradius[i] +
                          (blackpoingmapin[i] + pdistance) * sin(i * 6.28 / orm_radial_num);
                point_y = -sin(point_angle_t * 3.14 / 180) * mradius[i] +
                          (blackpoingmapin[i] + pdistance) * cos(i * 6.28 / orm_radial_num);
                //	data[(int)(point_y)*320*3 + (int)(point_x)*3]=255;
                //	data[(int)(point_y)*320*3 + (int)(point_x)*3+1]=255;
                //	data[(int)(point_y)*320*3 + (int)(point_x)*3+2]=255;
                point_angle_c = atan2(point_x, point_y) * 180.0 * orm_radial_num /
                                (3.14 * 360); // atan2((point_x-radial_cx),(point_y-radial_cy));
                int point_angle_cd =
                    radius_num_add(point_angle_c, -i); //-orm_radial_num/2 ~ +orm_radial_num/2  //+180 ~ -180
                if (point_angle_cd >
                    orm_radial_num * nearest_obviate_angle) // 只要是到与 本机和障碍夹角 90度以上的点都-180度翻折
                    //	point_angle_c =-point_angle_c+i+i+orm_radial_num/2;
                    point_angle_c = radius_num_add360(-point_angle_c, (float)i + i + orm_radial_num / 2);
                if (point_angle_cd <
                    -orm_radial_num * nearest_obviate_angle) // 只要是到与 本机和障碍夹角 90度以上的点都-180度翻折
                    //	point_angle_c =-point_angle_c+i+i+orm_radial_num/2;
                    point_angle_c = radius_num_add360(-point_angle_c, (float)i + i + orm_radial_num / 2);
                point_angle_c = radius_num_add360(point_angle_c, 0);
                point_radius_c =
                    sqrt(point_x * point_x + point_y * point_y); // sqrt( (point_x-radial_cx)*(point_x-radial_cx)
                                                                 // +(point_y-radial_cy)*(point_y-radial_cy) );
                /*
                            if (point_radius_c <(float)orm_radial_min_radius)    //如果扩展障碍半径小于 射线最小半径
                                point_radius_c =(float)orm_radial_min_radius;
                            if (point_radius_c >(float)orm_radial_max_radius)    //如果扩展障碍半径大于 射线最大半径
                                point_radius_c =(float)orm_radial_max_radius;
                //    		obstacleRadialMap[(int)point_angle_c][(int)point_radius_c  ]=0;
                */
                ///*
                if (Ppoint1 == 0) // 标志第一个点初始化问题
                {
                    Ppoint_angle_c = point_angle_c;
                    Ppoint_radius_c = point_radius_c;
                    Ppoint1 = 1;
                }
                float linelong = sqrt((point_radius_c - Ppoint_radius_c) * (point_radius_c - Ppoint_radius_c) +
                                      radius_num_add(point_angle_c, -Ppoint_angle_c) *
                                          radius_num_add(point_angle_c, -Ppoint_angle_c));
                float fla, flr;
                fla = radius_num_add(Ppoint_angle_c, -point_angle_c) / linelong;
                flr = (Ppoint_radius_c - point_radius_c) / linelong;
                float point_angle_d = point_angle_c;
                float point_radius_d = point_radius_c;
                for (float jn = 0; jn <= linelong; jn++)
                {
                    point_angle_d = radius_num_add360(point_angle_c, fla * jn);
                    point_radius_d = point_radius_c + flr * jn;
                    if (point_radius_d < (float)orm_radial_min_radius) // 如果扩展障碍半径小于 射线最小半径
                        point_radius_d = (float)orm_radial_min_radius;
                    if (point_radius_d > (float)orm_radial_max_radius) // 如果扩展障碍半径大于 射线最大半径
                        point_radius_d = (float)orm_radial_max_radius;
                    obstacleRadialMap[(int)(point_angle_d)][(int)(point_radius_d)] = 0;
                    obstacleRadialMap[(int)(point_angle_d)][(int)(point_radius_d + 1)] = 0;
                    obstacleRadialMap[(int)(point_angle_d)][(int)(point_radius_d + 2)] = 0;
                }
                Ppoint_angle_c = point_angle_c;
                Ppoint_radius_c = point_radius_c;
                //*/
            }
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // 分成两个半周画圆的原因 ：当翻折时一定要在 当前障碍的180度方向有 一个点  否则在此处会出现 直线从
            // 本机的另一边经过  造成障碍设定错误
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////画另半周圆
            for (point_angle = 90; point_angle <= circle_obviate_angle / 2 + 90;
                 point_angle += point_angle_d) // 2*3.14*radius
            {
                float point_angle_t = point_angle + radius_num_add360(i, 0) * 360 / orm_radial_num;
                point_x = cos(point_angle_t * 3.14 / 180) * mradius[i] +
                          (blackpoingmapin[i] + pdistance) * sin(i * 6.28 / orm_radial_num);
                point_y = -sin(point_angle_t * 3.14 / 180) * mradius[i] +
                          (blackpoingmapin[i] + pdistance) * cos(i * 6.28 / orm_radial_num);
                point_angle_c = atan2(point_x, point_y) * 180.0 * orm_radial_num /
                                (3.14 * 360); // atan2((point_x-radial_cx),(point_y-radial_cy));
                int point_angle_cd = radius_num_add(point_angle_c, -i);
                if (point_angle_cd >
                    orm_radial_num * nearest_obviate_angle) // 只要是到与 本机和障碍夹角 90度以上的点都-180度翻折
                    point_angle_c = radius_num_add360(-point_angle_c, (float)i + i + orm_radial_num / 2);
                if (point_angle_cd <
                    -orm_radial_num * nearest_obviate_angle) // 只要是到与 本机和障碍夹角 90度以上的点都-180度翻折
                    point_angle_c = radius_num_add360(-point_angle_c, (float)i + i + orm_radial_num / 2);
                point_angle_c = radius_num_add360(point_angle_c, 0);

                point_radius_c =
                    sqrt(point_x * point_x + point_y * point_y); // sqrt( (point_x-radial_cx)*(point_x-radial_cx)
                                                                 // +(point_y-radial_cy)*(point_y-radial_cy) );
                /*
                            if (point_radius_c <(float)orm_radial_min_radius)    //如果扩展障碍半径小于 射线最小半径
                                point_radius_c =(float)orm_radial_min_radius;
                            if (point_radius_c >(float)orm_radial_max_radius)    //如果扩展障碍半径大于 射线最大半径
                                point_radius_c =(float)orm_radial_max_radius;
                            obstacleRadialMap[(int)point_angle_c][(int)point_radius_c  ]=0;
                */
                ///*
                //			if (Ppoint1==0)//标志第一个点初始化问题
                //			{	Ppoint_angle_c=point_angle_c;
                //				Ppoint_radius_c=point_radius_c;
                //				Ppoint1=1;
                //			}
                float linelong = sqrt((point_radius_c - Ppoint_radius_c) * (point_radius_c - Ppoint_radius_c) +
                                      radius_num_add(point_angle_c, -Ppoint_angle_c) *
                                          radius_num_add(point_angle_c, -Ppoint_angle_c));
                float fla, flr;
                fla = radius_num_add(Ppoint_angle_c, -point_angle_c) / linelong;
                flr = (Ppoint_radius_c - point_radius_c) / linelong;
                float point_angle_d = point_angle_c;
                float point_radius_d = point_radius_c;
                for (float jn = 0; jn <= linelong; jn++)
                {
                    point_angle_d = radius_num_add360(point_angle_c, fla * jn);
                    point_radius_d = point_radius_c + flr * jn;
                    if (point_radius_d < (float)orm_radial_min_radius) // 如果扩展障碍半径小于 射线最小半径
                        point_radius_d = (float)orm_radial_min_radius;
                    if (point_radius_d > (float)orm_radial_max_radius) // 如果扩展障碍半径大于 射线最大半径
                        point_radius_d = (float)orm_radial_max_radius;
                    obstacleRadialMap[(int)(point_angle_d)][(int)(point_radius_d)] = 0;
                    obstacleRadialMap[(int)(point_angle_d)][(int)(point_radius_d + 1)] = 0;
                    obstacleRadialMap[(int)(point_angle_d)][(int)(point_radius_d + 2)] = 0;
                }
                Ppoint_angle_c = point_angle_c;
                Ppoint_radius_c = point_radius_c;
                //*/
            }
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            /*
                    int bpr;//存储 bpr=blackpoingmapin[i]-r          注释:r为机体在视觉半径方向上
            blackpoingmapin[i]位置所占据的半径r j=blackpoingmapin[i];
            //		if (j>orm_radial_inrend) //防止出畸变校正表 上限		orm_radial_inrend
            =80;//[10~80]查找表最大输入半径
            //			j=orm_radial_inrend;
            //		if (j<orm_radial_inrstart)  //防止出畸变校正表 下限
            orm_radial_inrstart=10;//[10~80]查找表最小输入半径
            //			j=orm_radial_inrstart;
                    bpr=j;//默认情况
                    bpr = bpr - 10*pdistance;//mradius*pdistance/(orm_radial_r[i][bpr+1]-orm_radial_r[i][bpr]);
            //		if (bpr<orm_radial_min_radius)
            //			bpr=orm_radial_min_radius;
                    if (bpr<2)
                        bpr=2;
            //		if (j<orm_radial_inrstart)  //防止出畸变校正表 下限
            orm_radial_inrstart=10;//[10~80]查找表最小输入半径
            //			j=orm_radial_inrstart;
                    int expand_r =
            (160.0*pradius*(float)(120+1-bpr)/120.0)/(360.0/(float)(orm_radial_num));//mradius*pradius/(2*3.14*orm_radial_r[i][bpr]/orm_radial_num);//膨胀角度
                    int bprt,bpr_t;
            //		    bprt = (int)(bpr/cos((float)(r)*3.14*2/orm_radial_num));
            //		    if (bprt >(int)( mradius*pdistance/(orm_radial_r[i][bpr+1]-orm_radial_r[i][bpr]) ))
            //如果扩展障碍半径大于 mradius*pradius
            //		        bprt =(int)( mradius*pdistance/(orm_radial_r[i][bpr+1]-orm_radial_r[i][bpr]) );
            //		    if (bprt <orm_radial_min_radius)    //如果扩展障碍半径小于 射线最小半径
            //		        bprt =orm_radial_min_radius;
            //		    if (bprt >orm_radial_max_radius)    //如果扩展障碍半径大于 射线最大半径
            //		        bprt =orm_radial_max_radius;
                        bpr_t=bpr;
                    for(r=0;r<expand_r;r++)//近大远小 贴身*Rnear 最远时为Rfar  可以靠查表非线性
                    {

                        bprt = (int)(bpr/cos((float)(r)*3.14*2/orm_radial_num));
                        if ((bprt-bpr) >(int)( mradius*p_distance/(orm_radial_r[i][bpr+1]-orm_radial_r[i][bpr]) ))
            //如果扩展障碍半径大于 mradius*pradius break;//bprt=bpr  +(int)(
            mradius*pdistance/(orm_radial_r[i][bpr+1]-orm_radial_r[i][bpr]) ); if (bprt <orm_radial_min_radius)
            //如果扩展障碍半径小于 射线最小半径 bprt =orm_radial_min_radius; if (bprt >orm_radial_max_radius)
            //如果扩展障碍半径大于 射线最大半径 bprt =orm_radial_max_radius;
            //j=bprt;
                        for (j=bpr_t;j<=bprt;j++)
                        {
                            tempr=i-r;
                            if (tempr<0)
                                tempr+=orm_radial_num;
                            obstacleRadialMap[tempr][j  ]=0;
                            obstacleRadialMap[tempr][j+1]=0;
                            obstacleRadialMap[tempr][j+2]=0;
                            tempr=i+r;
                            if (tempr>=orm_radial_num)
                                tempr-=orm_radial_num;
                            obstacleRadialMap[tempr][j  ]=0;
                            obstacleRadialMap[tempr][j+1]=0;
                            obstacleRadialMap[tempr][j+2]=0;
                        }
                        bpr_t=bprt;
                    }
                */
            blackpoingmapin[i] = blackpoingmapintemp;
        }
    }
}

inline bool CPathLayout::Bool_ProcessIII_B_VHS_v(unsigned char *data, int i)
{
    //[i](V);
    //[i+1](S);
    //[i+2](H);
    if ((data[i + 1] < FLblackSmax) && (data[i] < FLblackVmax)) // luminosity是正的
        return (0);
    else
        return (1);
}

float juli(float x, float y)
{
    float a = sqrt(x * x + y * y);
    return (a);
}

// 给出一点种子seed.x seed.y 得到由此点开始生长的连通域
// 得到从开始点开始 第一个路径与障碍的碰撞点
// int starty  int startx    开始点  中止点
// int endy    int endx      终止点  终止点
// 分辨率	  320*240
// BW_data	  320*240       二值化 输入地图 1可以走的点 0不能走的点
// path_data_out		320*240
// const direct_cost=71;//160;//路径规划中 转向的花费  注意只计算当前第一次转向的	71是现在时间
// 花费最大的值否则起点就不变了
const direct_cost = 50; // 路径规划中 转向的花费  注意只计算当前第一次转向的	53最小花费 再小就和现在一样了

char CPathLayout::path_generateIIII(int endAngle, int endRadius)
{
    endRadius = (int)(field_line_rectify(
        endAngle, endRadius));             // 畸变校正查表程序  会线性补足查找表里<10的项    >80的点全部为=80缺失
    if (endRadius < orm_radial_min_radius) // 防止出现除出小与一 半径为零 的情况
        endRadius = orm_radial_min_radius;
    if (endRadius > sxradius_max) // 防止出现除出小与一 半径为零 的情况
        endRadius = sxradius_max;

    //	int starty=0;
    //	int startx=0;
    //	int endy;
    //	int endx;
    // char *BW_data;
    // char **area_grow_datao;
    // char *path_data_out;

    // obstacleRadialMap
    // int sxcenter_x;//射线中心X
    // int sxcenter_y;//射线中心Y
    // int sxradius_min;//射线半径最小长度
    // int sxradius_max;//射线半径最大长度
    // int sxtiaoshu   ;//射线条数

    // r*2*PI/sxtiaoshu
    float henbuju = 2 * 3.1415926 / sxtiaoshu * 10; // 切向一个格步距

    // 约等于(r*2*PI/sxtiaoshu) +1
    // float   xiebuju;//斜切向一个格步距

    // 注意后面变量是用转化问题 x==半径 y==角度号[射线号]	因为存储相似性
    // 最大生长层数不能超过4680次
    const maxGrownTime = 1500; // 最大生长层
    char path_result =
        0; // 0默认无状态  1成功 2失败一开始点黑色 3失败二目标点黑色 4找不到路径,超过maxGrownTime1500点还没到达目标点
           // 5生长完连通域还没生长到目标点 失败 6失败一开始点出了范围[320*240] 7失败二目标点出了范围[320*240]
    const int x320 = 320; // 320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240 = 240; // 240;//处理图像数据的大小设置

    int pathoutlong;
    int circumscription_weight;
    int i, j, x, y;
    int x_add, x_reduce, y_add, y_reduce;

    // if ((starty<0)||(y240<starty)||(startx<0)||(x320<startx))
    //{	path_result=6;//6失败一开始点出了范围[320*240]
    //	return(path_result);//如果 没成功就返回
    // }
    if ((endAngle < 0) || (sxtiaoshu < endAngle) || (endRadius < 0) || (sxradius_max < endRadius))
    {
        path_result = 7;                   // 7失败二目标点出了范围[320*240]
        path_result_history = path_result; // 存储上次规划    失败 原因纪录
        return (path_result);              // 如果 没成功就返回
    }

    i = 0;
    for (y = 0; y < sxtiaoshu; y++)
    {
        for (x = 0; x < sxradius_max; x++)
        {
            area_grow_dataA[y][x] =
                65536; // 清零		注意因为65536是没有生长过的点所以 从10生长到65536最小以14的步长 4681次
                       // 所以当超过4681次还没找到目标就会硬性出错 			最大生长层数不能超过4680次
            area_grow_datao[y][x] = 0;
        }
    }
    // 黑边处理  防止出现边缘冒了//////y 既角度不做防冒处理 因为后续有卷绕处理(过180度问题)
    // for (x=0;x<x320;x++)
    //{	obstacleRadialMap[0][x]		=0;
    //	obstacleRadialMap[y240-1][x]	=0;
    // }
    for (y = 1; y < sxtiaoshu; y++)
    {
        obstacleRadialMap[y][0] = 0;                // 65536;
        obstacleRadialMap[y][sxradius_max - 1] = 0; // 65536;
    }
    // 最外面一圈 黑边处理  防止出现边缘冒了//////
    for (j = 0, i = endRadius; i < sxtiaoshu; i++)
    {
        if (obstacleRadialMap[endAngle][i] == 1)
        {
            j++;
        }
    }
    if (j > 0)
    {

        for (j = 0, i = 0; i < sxtiaoshu; i++) // 检查最内圈是否有 可行走的点如果有 j>0
        {
            j += obstacleRadialMap[i][orm_radial_min_radius];
        }
        if (j > 0) // 检查最内圈是否有 可行走的点如果有 j>0就继续路径规划
        {
            // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●

            area_grow_dataAtcl = 0; // 纪录当前次这一圈有几个像素
            area_grow_dataAtpl = 0; // 纪录当前次这一圈有几个像素
            // 1设置这次生长的起点种子//int starty  int startx    开始点  中止点
            // 从极坐标0,0点开始  相当于 从每个角度同样的0半径开始
            /*for (i=0;i<sxtiaoshu;i++)
            {		area_grow_dataAtpx[i]=orm_radial_min_radius;////手动设置种子
                    area_grow_dataAtpy[i]=i;////手动设置种子
                    area_grow_dataAtpl++;////手动设置种子
                    area_grow_dataA[i][orm_radial_min_radius]=10;////手动设置种子
                    circumscription_weight=circumscription_weight+1;//连通域重量+1
                }
            }
            */
            if (direct_history > sxtiaoshu) // 保险程序
                direct_history = sxtiaoshu;
            if (direct_history < 0) // 保险程序
                direct_history = 0;
            for (i = 0; i < sxtiaoshu / 2; i++)
            {
                int tempr;
                tempr = direct_history - i;
                if (tempr < 0)
                    tempr += sxtiaoshu;

                if (obstacleRadialMap[tempr][orm_radial_min_radius] == 1)
                {
                    area_grow_dataA[tempr][orm_radial_min_radius] =
                        10 + (int)(direct_cost * (float)(i) / (float)(sxtiaoshu / 2)); ////手动设置种子
                    area_grow_dataAtpx[area_grow_dataAtpl] = orm_radial_min_radius;    ////手动设置种子
                    area_grow_dataAtpy[area_grow_dataAtpl] = tempr;                    ////手动设置种子
                    area_grow_dataAtpl++;                                              ////手动设置种子
                    circumscription_weight = circumscription_weight + 1;               // 连通域重量+1
                }
                tempr = direct_history + i;
                if (tempr >= sxtiaoshu)
                    tempr -= sxtiaoshu;
                if (obstacleRadialMap[tempr][orm_radial_min_radius] == 1)
                {
                    area_grow_dataA[tempr][orm_radial_min_radius] =
                        10 + (int)(direct_cost * (float)(i) / (float)(sxtiaoshu / 2)); ////手动设置种子
                    area_grow_dataAtpx[area_grow_dataAtpl] = orm_radial_min_radius;    ////手动设置种子
                    area_grow_dataAtpy[area_grow_dataAtpl] = tempr;                    ////手动设置种子
                    area_grow_dataAtpl++;                                              ////手动设置种子
                    circumscription_weight = circumscription_weight + 1;               // 连通域重量+1
                }
            }
            // 2开始生长
            for (i = 2; i < 1500; i++) // 生成生长区域 的生长次数
            {

                area_grow_dataAtcl = 0; // 纪录当前次这一圈有几个像素 注意：有重复的
                for (j = 0; j < area_grow_dataAtpl; j++)
                {                              // 周围不是A中的有效点且符合生长规则
                    x = area_grow_dataAtpx[j]; // 当前次生长的一圈的纪录
                    y = area_grow_dataAtpy[j]; // 当前次生长的一圈的纪录
                    x_add = x + 1;
                    x_reduce = x - 1;

                    y_add = y + 1;
                    if (y_add >= sxtiaoshu)
                        y_add = y_add - sxtiaoshu;
                    y_reduce = y - 1;
                    if (y_reduce < 0)
                        y_reduce = y_reduce + sxtiaoshu;

                    /**/ if ((area_grow_dataA[y_add][x_reduce] >
                              (area_grow_dataA[y][x] + juli((henbuju * x_reduce), 10))) &&
                             (obstacleRadialMap[y_add][x_reduce] == 1))
                    {
                        area_grow_dataAtcy[area_grow_dataAtcl] = y_add;    // 当前次生长的一圈的纪录
                        area_grow_dataAtcx[area_grow_dataAtcl] = x_reduce; // 当前次生长的一圈的纪录
                        area_grow_dataAtcl++;
                        area_grow_dataA[y_add][x_reduce] =
                            area_grow_dataA[y][x] +
                            juli((henbuju * x_reduce), 10); // henbuju*x+9;//将A``合并入A 不会有重复点了
                    }                                       /**/

                    // 切向
                    if ((area_grow_dataA[y_add][x + 0] > (area_grow_dataA[y][x] + henbuju * x)) &&
                        (obstacleRadialMap[y_add][x + 0] == 1))
                    {
                        area_grow_dataAtcy[area_grow_dataAtcl] = y_add; // 当前次生长的一圈的纪录
                        area_grow_dataAtcx[area_grow_dataAtcl] = x + 0; // 当前次生长的一圈的纪录
                        area_grow_dataAtcl++;
                        area_grow_dataA[y_add][x + 0] =
                            area_grow_dataA[y][x] + henbuju * x; // 将A``合并入A 不会有重复点了
                    }

                    /**/ if ((area_grow_dataA[y_add][x_add] > (area_grow_dataA[y][x] + juli((henbuju * x_add), 10))) &&
                             (obstacleRadialMap[y_add][x_add] == 1))
                    {
                        area_grow_dataAtcy[area_grow_dataAtcl] = y_add; // 当前次生长的一圈的纪录
                        area_grow_dataAtcx[area_grow_dataAtcl] = x_add; // 当前次生长的一圈的纪录
                        area_grow_dataAtcl++;
                        area_grow_dataA[y_add][x_add] =
                            area_grow_dataA[y][x] +
                            juli((henbuju * x_add), 10); // henbuju*x+9;//将A``合并入A 不会有重复点了
                    }                                    /**/

                    if ((area_grow_dataA[y + 0][x_reduce] > (area_grow_dataA[y][x] + 10)) &&
                        (obstacleRadialMap[y + 0][x_reduce] == 1))
                    {
                        area_grow_dataAtcy[area_grow_dataAtcl] = y + 0;    // 当前次生长的一圈的纪录
                        area_grow_dataAtcx[area_grow_dataAtcl] = x_reduce; // 当前次生长的一圈的纪录
                        area_grow_dataAtcl++;
                        area_grow_dataA[y + 0][x_reduce] = area_grow_dataA[y][x] + 10; // 将A``合并入A 不会有重复点了
                    }

                    //	||
                    //((area_grow_dataA[y+0][x+0]==1)&&(abs(dataHSV[y+0][x+0][0]-tempH)<5)&&(abs(dataHSV[y+0][x+0][2]-tempV)<30)&&(abs(dataHSV[y+0][x+0][1]-tempS)<40))
                    if ((area_grow_dataA[y + 0][x_add] > (area_grow_dataA[y][x] + 10)) &&
                        (obstacleRadialMap[y + 0][x_add] == 1))
                    {
                        area_grow_dataAtcy[area_grow_dataAtcl] = y + 0; // 当前次生长的一圈的纪录
                        area_grow_dataAtcx[area_grow_dataAtcl] = x_add; // 当前次生长的一圈的纪录
                        area_grow_dataAtcl++;
                        area_grow_dataA[y + 0][x_add] = area_grow_dataA[y][x] + 10; // 将A``合并入A 不会有重复点了
                    }

                    /**/ if ((area_grow_dataA[y_reduce][x_reduce] >
                              (area_grow_dataA[y][x] + juli((henbuju * x_reduce), 10))) &&
                             (obstacleRadialMap[y_reduce][x_reduce] == 1))
                    {
                        area_grow_dataAtcy[area_grow_dataAtcl] = y_reduce; // 当前次生长的一圈的纪录
                        area_grow_dataAtcx[area_grow_dataAtcl] = x_reduce; // 当前次生长的一圈的纪录
                        area_grow_dataAtcl++;
                        area_grow_dataA[y_reduce][x_reduce] =
                            area_grow_dataA[y][x] +
                            juli((henbuju * x_reduce), 10); // henbuju*x+9;//将A``合并入A 不会有重复点了
                    }                                       /**/
                                                            // 切向

                    if ((area_grow_dataA[y_reduce][x + 0] > (area_grow_dataA[y][x] + henbuju * x)) &&
                        (obstacleRadialMap[y_reduce][x + 0] == 1))
                    {
                        area_grow_dataAtcy[area_grow_dataAtcl] = y_reduce; // 当前次生长的一圈的纪录
                        area_grow_dataAtcx[area_grow_dataAtcl] = x + 0;    // 当前次生长的一圈的纪录
                        area_grow_dataAtcl++;
                        area_grow_dataA[y_reduce][x + 0] =
                            area_grow_dataA[y][x] + henbuju * x; // 将A``合并入A 不会有重复点了
                    }

                    /**/ if ((area_grow_dataA[y_reduce][x_add] >
                              (area_grow_dataA[y][x] + juli((henbuju * x_add), 10))) &&
                             (obstacleRadialMap[y_reduce][x_add] == 1))
                    {
                        area_grow_dataAtcy[area_grow_dataAtcl] = y_reduce; // 当前次生长的一圈的纪录
                        area_grow_dataAtcx[area_grow_dataAtcl] = x_add;    // 当前次生长的一圈的纪录
                        area_grow_dataAtcl++;
                        area_grow_dataA[y_reduce][x_add] =
                            area_grow_dataA[y][x] +
                            juli((henbuju * x_add), 10); // henbuju*x+9;//将A``合并入A 不会有重复点了
                    }                                    /**/
                }

                //	if (area_grow_dataA[endAngle][endRadius]!=65536)//说明生长过了
                // 20080925改：生长终点变到一条从原终点向外的射线 	{	path_result=1;//成功 		break;
                //	}
                if (area_grow_dataAtcl == 0)
                {
                    path_result = 5; // 失败 生长完连通域还没生长到目标点
                    break;
                }
                // 注意后面注释掉的部分 不做去掉重复的点的步骤了
                for (j = 0; j < area_grow_dataAtcl; j++)
                {
                    area_grow_dataAtpx[j] = area_grow_dataAtcx[j];
                    area_grow_dataAtpy[j] = area_grow_dataAtcy[j];
                    if ((area_grow_dataAtpy[j] == endAngle) && (area_grow_dataAtpx[j] >= endRadius))
                    {
                        path_result = 1; // 成功  20080925加：生长终点变到一条从原终点向外的射线
                        // endAngle=endAngle;
                        endRadius = area_grow_dataAtpx[j];
                        goto complete_seed_grow;
                    }
                }
                area_grow_dataAtpl = area_grow_dataAtcl;      // A``倒入At先去掉重复的点重新计算Atcl
                circumscription_weight += area_grow_dataAtcl; // 连通域重量+当前一圈的点数
            }
        complete_seed_grow:
            // 生长完成
            if (i == maxGrownTime)
            {
                path_result = 4; // 失败 找完1500层点还没生长到目标点
            }

            // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
        }
        else
        {
            path_result = 2; // 失败一开始点黑色
        }
    }
    else
    {
        path_result = 2; // 3失败二目标点黑色
    }

    if (path_result != 1)
    {
        //	pathxII[0] = 160;
        //	pathyII[0] = 120;
        path_result_history = path_result; // 存储上次规划    失败 原因纪录
        return (path_result);              // 如果 没成功就返回
    }
    // int cx,cy;
    // i现在存的是 长了多少层

    // 测试 代码  显示省长层数  和最大值
    // int f,g,m;
    // m=0;
    // for (f=0;f<240;f++)
    // for (g=0;g<320;g++)
    //{	if (m<area_grow_dataA[f][g])
    //		m=area_grow_dataA[f][g];
    // }
    //	CString str;
    //	str.Format("i %d  max %d",i,m);
    //	AfxMessageBox(str);
    int xp, yp;

    // 1设置这次生长的起点种子int endy,int endx
    x = endRadius; ////手动设置种子
    y = endAngle;  ////手动设置种子
    pathl = i;
    // 生长不开时的问题??????????????????????????????????????

    int maxSlope = 0;
    int maxSlopey = 0;
    int maxSlopex = 0;
    // 2开始反向生长 几乎为单线生长
    for (i = i; i > 1; i--) // 生成生长区域 的生长次数  计数最低为1 开始点层数1
    {
        area_grow_datao[y][x] = 1; //=i-1
        pathx[i] = x;              // 存储路径的 顺序坐标
        pathy[i] = y;
        // 因为一个当前点可能对应多个父点 具体向哪个父点回溯前进 就要看那个父点方向的数值减小率最大即最速下降方向
        // 在方形的像素中是  横竖 比斜角 变化速率大 1.414:1
        // 起始比较斜率应该算三个点的 综合的斜率但是简单起见就只认为 横竖 比斜角大了
        // 这段程序查找规律是先找 横竖正方向的点  再找四个斜角方向的点
        // 最先符合规则的有效点为下次前进目标父点
        maxSlope = 0;
        // maxSlopey =0;
        // maxSlopex =0;

        if (x_add < sxradius_max - 1)
            x_add = x + 1;
        else
            x_add = x;
        if (x_add > 0)
            x_reduce = x - 1;
        else
            x_reduce = x;

        y_add = y + 1;
        if (y_add >= sxtiaoshu)
            y_add = y_add - sxtiaoshu;
        y_reduce = y - 1;
        if (y_reduce < 0)
            y_reduce = y_reduce + sxtiaoshu;

        if (maxSlope < (area_grow_dataA[y][x] - area_grow_dataA[y_add][x + 0]))
        {
            maxSlope = area_grow_dataA[y][x] - area_grow_dataA[y_add][x + 0];
            maxSlopey = y_add; // 当前次生长的一圈的纪录
            maxSlopex = x + 0; // 当前次生长的一圈的纪录
        }
        if (maxSlope < (area_grow_dataA[y][x] - area_grow_dataA[y + 0][x_reduce]))
        {
            maxSlope = area_grow_dataA[y][x] - area_grow_dataA[y + 0][x_reduce];
            maxSlopey = y + 0;    // 当前次生长的一圈的纪录
            maxSlopex = x_reduce; // 当前次生长的一圈的纪录
        }
        if (maxSlope < (area_grow_dataA[y][x] - area_grow_dataA[y + 0][x_add]))
        {
            maxSlope = area_grow_dataA[y][x] - area_grow_dataA[y + 0][x_add];
            maxSlopey = y + 0; // 当前次生长的一圈的纪录
            maxSlopex = x_add; // 当前次生长的一圈的纪录
        }
        if (maxSlope < (area_grow_dataA[y][x] - area_grow_dataA[y_reduce][x + 0]))
        {
            maxSlope = area_grow_dataA[y][x] - area_grow_dataA[y_reduce][x + 0];
            maxSlopey = y_reduce; // 当前次生长的一圈的纪录
            maxSlopex = x + 0;    // 当前次生长的一圈的纪录
        }
        //	||
        //((area_grow_dataA[y+0][x+0]==1)&&(abs(dataHSV[y+0][x+0][0]-tempH)<5)&&(abs(dataHSV[y+0][x+0][2]-tempV)<30)&&(abs(dataHSV[y+0][x+0][1]-tempS)<40))
        if (maxSlope < (area_grow_dataA[y][x] - area_grow_dataA[y_add][x_add]))
        {
            maxSlope = area_grow_dataA[y][x] - area_grow_dataA[y_add][x_add];
            maxSlopey = y_add; // 当前次生长的一圈的纪录
            maxSlopex = x_add; // 当前次生长的一圈的纪录
        }
        if (maxSlope < (area_grow_dataA[y][x] - area_grow_dataA[y_add][x_reduce]))
        {
            maxSlope = area_grow_dataA[y][x] - area_grow_dataA[y_add][x_reduce];
            maxSlopey = y_add;    // 当前次生长的一圈的纪录
            maxSlopex = x_reduce; // 当前次生长的一圈的纪录
        }
        if (maxSlope < (area_grow_dataA[y][x] - area_grow_dataA[y_reduce][x_reduce]))
        {
            maxSlope = area_grow_dataA[y][x] - area_grow_dataA[y_reduce][x_reduce];
            maxSlopey = y_reduce; // 当前次生长的一圈的纪录
            maxSlopex = x_reduce; // 当前次生长的一圈的纪录
        }
        if (maxSlope < (area_grow_dataA[y][x] - area_grow_dataA[y_reduce][x_add]))
        {
            maxSlope = area_grow_dataA[y][x] - area_grow_dataA[y_reduce][x_add];
            maxSlopey = y_reduce; // 当前次生长的一圈的纪录
            maxSlopex = x_add;    // 当前次生长的一圈的纪录
        }

        y = maxSlopey;
        x = maxSlopex;

        if ((xp == x) && (yp == y))
        {
            xp = x;
            yp = y;
        }
        xp = x;
        yp = y;
    }

    // area_grow_datao[y][x]=1;//因为最后一个点 for 已经退出了所以这里补上
    area_grow_datao[0][0] = 1; //=i-1
    pathx[1] = 0;              // 存储路径的 顺序坐标
    pathy[1] = 0;

    /*int pathx_[1500];
    int pathy_[1500];
    for (i=0;i<1500;i++)
    {	pathx_[i] =pathx[i];
        pathy_[i] =pathy[i];
    }

        pathx_[i] =pathx[i];
        pathy_[i] =pathy[i];

    int ax,ay,k;//线段起始点
    int bx,by;//线段终点
    int pathxII[1000];
    int pathyII[1000];
    //					(ay-by)/(ax-bx)=(ay-y)/(ax-x)
    //					(ay-by)*(ax-x)/(ax-bx)=(ay-y)
    //					(ax-bx)/(ay-by)=(ax-x)/(ay-y)
    //					(ax-bx)*(ay-y)/(ay-by)=(ax-x)
    const int nihebuchang=5;//用直线 拟合 路径点//步长3  j++  <=> j=j+1 说明以起点开始 后每个点都逐个进行连线段
    测试是否有障碍   可以为j=j+2 j=j+3等 加大每次增长长度

    for (y=0;y<sxtiaoshu;y++)
    {	for (x=0;x<sxradius_max;x++)
        {	obstacleRadialMap[y][x]=area_grow_datao[y][x];
    }	}
    */
    direct_history = pathy[2];
    path_result_history = path_result; // 存储上次规划    失败 原因纪录
    return (path_result);              // 成功返回
}

#endif