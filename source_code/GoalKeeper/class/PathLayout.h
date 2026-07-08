// PathLayout.h

#include "stdafx.h"

#ifndef PathLayout_h
#define PathLayout_h

#define sxcenter_x 160   // int  射线中心X
#define sxcenter_y 120   // int 射线中心Y
#define sxradius_min 0   // int 射线半径最小长度
#define sxradius_max 120 // int 射线半径最大长度
#define sxtiaoshu 144    // int 射线条数

extern long int area_grow_dataA[240][320];
extern bool area_grow_datao[240][320];
extern unsigned int area_grow_dataAtcx[10000]; // currently当前生长的一圈的纪录
extern unsigned int area_grow_dataAtcy[10000]; // currently当前生长的一圈的纪录
extern unsigned int area_grow_dataAtcl;        // 纪录currently当前这一圈有几个像素
extern unsigned int area_grow_dataAtpx[10000]; // pre-	上一次生长的一圈的纪录
extern unsigned int area_grow_dataAtpy[10000]; // pre-	上一次生长的一圈的纪录
extern unsigned int area_grow_dataAtpl;        // 纪录pre-	上一次这一圈有几个像素
extern int pathx[10000];                       // 存储路径的 顺序坐标
extern int pathy[10000];
extern int pathl;
extern int pathxII[1000]; // 用线段 拟和后的路径 端点坐标
extern int pathyII[1000];
extern int pathlII;
extern int obstacleRadialMap[sxtiaoshu][sxradius_max]; // 存储障碍 位置 地图

extern int radial_lm_view[sxtiaoshu][sxradius_max];

class CPathLayout
{
  protected:
    int FLblackSmax;
    int FLblackVmax;
    int FLgreenHmax;
    int FLgreenHmin;
    int FLgreenSmax;
    int FLgreenSmin;
    int FLgreenVmax;
    int FLgreenVmin;

  public:
    void path_layout_black_green_color_set(int blackSmax, int blackVmax, int greenHmax, int greenHmin, int greenSmax,
                                           int greenSmin, int greenVmax, int greenVmin);
    void make_radial_mask(void);

    void make_field_line_rectify_tab(void);
    inline float field_line_rectify(int line_num,
                                    int radius); // 畸变校正查表程序  会线性补足查找表里<10的项    >80的点全部为=80缺失
    //	void black_p_detect(unsigned char *data);//make_radial_point
    //	void black_p_detect2(unsigned char *data);//make_radial_point
    void black_p_detect2_d1(unsigned char *data,
                            int *blackpoingmapout); // 测出每条射线第一个连续5个点的遮挡物的第一个点
    //	void black_p_detect2_d2(int *blackpoingmapin);// 进行后续 有效性判断 扩边 生成规划用地图
    void black_p_detect2_d2_circle_2(int *blackpoingmapin, float min_radius); // 进行后续 有效性判断 扩边 生成规划用地图
    inline bool Bool_ProcessIII_B_VHS_v(unsigned char *data, int i);
    char path_generateIIII(int endAngle, int endRadius);
    //	char path_generate_V(int endAngle,int endRadius);

    //	下列中间状态变量 都在make_radial_mask中初始化
    int direct_history; // 存储上次规划结果方向的信息	//只在path_generateIIII中记录	//在black_p_detect2_d2_circle_2
                        // path_generateIIII中调用
    char path_result_history;       // 存储上次规划    失败 原因纪录 //只在path_generateIIII中记录
                                    // //在black_p_detect2_d2_circle_2  中调用
    float black_circle_zoom;        // 存储规划    失败 后对障碍进行 缩小
                                    // 便于在新的周期中可以找到路径//只在在black_p_detect2_d2_circle_2中改变
                                    // //在black_p_detect2_d2_circle_2  中调用
    int black_circle_zoom_timeloop; // 存储 需要进行 缩小的障碍的时间 以求这段时间可以冲出 不可行走区域
                                    // //只在path_generateIIII中记录	//在black_p_detect2_d2_circle_2
                                    // path_generateIIII中调用
};

#endif