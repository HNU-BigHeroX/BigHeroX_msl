// UPRColor.h: interface for the UPRColor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UPRCOLOR_H__DEBCC6BB_A839_4DC6_ABF4_56443BAEC536__INCLUDED_)
#define AFX_UPRCOLOR_H__DEBCC6BB_A839_4DC6_ABF4_56443BAEC536__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "HSIParam.h"
#include "YUVParam.h" cgi
#include <WINGDI.H>
#include "Yuzhi.h"
#include <math.h>
#include "..\TxtArray.h"
#include "./../class/Pp.h"

#include "../UPFullField.h"

#define UPRColor_DISPLAY_SetColor  // 定义 画点函数起作用
#define UPRColor_DISPLAY_DrawFocus // 定义 画十字函数起作用
// #define  UPRColor_FUNCTION_DSPLAY//设置显示 场地范围信息
extern int black_OBJ_L;
extern float black_OBJ_A[1000];  // 记录空间1000个 但只使用 最近的10个做输出到教练机
extern float black_OBJ_R[1000];  // 记录空间1000个 但只使用 最近的10个做输出到教练机
extern float black_OBJ_W[1000];  // 记录空间1000个 但只使用 最近的10个做输出到教练机
extern float black_OBJ_fx[1000]; // 记录空间1000个 但只使用 最近的10个做输出到教练机
extern float black_OBJ_fy[1000]; // 记录空间1000个 但只使用 最近的10个做输出到教练机
extern int black_OBJ_sequence[1000];

class UPRColor
{
  public:
    void Init_Mask_sectorBLACK(void);
    void getblackobj2(float f_x, float f_y, float f_a);

    double jiajiao1(double jizhun, double jiao);
    int sijiao(Yuzhi y);
    int qijiaopanding(Yuzhi y, int a);
    void m_LoadTables();
    void DebugInfoS(CString inStr);
    void DebugInfoM(CString inStr);
    void shichangfa(int vol, double *jiao, double *chang);
    void vhecheng(double jiao1, double sudu1, double top1, double jiao2, double sudu2, double top2, double *jiao,
                  double *sudu);
    void bianxiantouying(Yuzhi n);
    void DelWL(int a);
    void guibiqiu(Yuzhi *y);
    void menbianjie(Yuzhi *y);
    void dingwei(Yuzhi *changdi);
    void jingqumen(Yuzhi *yu);

    int getzhangaiwu(int nei, int wai, int chang, int mubiao, int fanwei, int bian, int *zuo, int *you);
    void pengzhang(int n);

    // CListBox* pList;
    CUPFullField *pField;
    void changdiyidong(int x, int y, double *jiao, double *chang);
    double pipei(Yuzhi *changdi);
    void getchangxian();
    void xianshiweitu();
    void guibi(void);
    void xianshi(void);
    void yanmo(void);
    //
    char path_generate(int startx, int starty, int endx, int endy, int *xx, int *yy);
    void path_generate_init(void);
    //
    void Gethei(int s, int i);
    void lianxuyu(int val, int fangshi, Yuzhi *HSI11);
    void zhizuobiao(double *x, double *y, double jiao, double chang, double zhongxinx, double zhongxiny);

    void jizuobiao(double x, double y, double *jiao, double *chang, int zhongxinx, int zhongxiny);
    int quzhi(int x);
    void DrawFocus(int x, int y, RGBTRIPLE *prgb);
    BOOL SetColor(int x, int y, RGBTRIPLE rgb);
    void ImageBuf(int Width, int Height, BYTE *Buf);
    BOOL GetHSIFix(Yuzhi *zz);
    BOOL GetYUV(int x, int y, YUVParam *YUV);
    int Min(int x, int y, int z);
    int Max(int x, int y, int z);
    BOOL GetRGB(int x, int y, RGBTRIPLE *prgb);
    // BOOL banjing[320][240];
    UPRColor();
    virtual ~UPRColor();

    //	void bool_max_connectivity_analyze2(float *obj_weight_map);//
    void bool_max_connectivity_analyze2_1(float *obj_weight_map);
    void bool_max_connectivity_analyze_mask_int(void);
    //	void bool_max_connectivity_analyze_e1(float *obj_weight_map);
    void bool_max_connectivity_analyze_e2(float *obj_weight_map);

    void bool_max_connectivity_analyze2_1ball(float *obj_weight_map, Yuzhi *HSI11);

    void bool_max_connectivity_analyze2_1green(float *obj_weight_map);
    void bool_max_connectivity_analyze2_1_InGreen_ball(float *obj_weight_map, Yuzhi *HSI11);

    void Init_Mask_sector(void);
    void Init_Mask_sectorA(void);
    void Init_Mask_Checked_Box(void);
    void Init_mask_checked_box_lookup_table(void);
    void bool_max_connectivity_analyze2_2_checked_box_lookup(float *obj_weight_map, float *outx, float *outy,
                                                             float *min_r);
    void bool_max_connectivity_analyze2_2_checked_box_lookupA(float *obj_weight_map, float *outx, float *outy,
                                                              float *min_r, float *max_r);

    float film_w;                              // 设置make_in_film_radius_table()场地范围参数 右边线坐标
    float film_h;                              // 设置make_in_film_radius_table()场地范围参数 上边线坐标
    float film_w_;                             // 设置make_in_film_radius_table()场地范围参数 左边线坐标
    float film_h_;                             // 设置make_in_film_radius_table()场地范围参数 下边线坐标
    float max_radius;                          // 设置所有扇区的最大半径
    float min_radius;                          // 标示最小半径 即之内所有点不做计算
    void make_in_film_radius_table_init(void); // make_in_film_radius_table()参数初始化函数
    void make_in_film_radius_table(float f_x, float f_y, float f_a);

    void getchangxian2();
    void getblackobj(float f_x, float f_y, float f_a); // 得到 聚类后 的几个黑色目标坐标
    void getzhangaiwu2(int nei, int wai, int chang, int bian);

    char display_process_image;
    BOOL GetHSIFix2(Yuzhi *zz);
    BOOL GetHSIFix3(Yuzhi *zz);
    BOOL GetHSIFix3_no_G_W(Yuzhi *zz);

    float luopanjiao;
    bool luopanqingchu;

    char pipei2Display;
    double pipei2(Yuzhi *changdi);
    double pipei2_5(Yuzhi *changdi);

    void getchangxian_3(Yuzhi *zz);

    // 用于调试信息输出
    CListBox *pList;
    CEdit *pEdit;
    double ycx;
    double ycy;
    double ycj;

  private:
    int xbshang;
    int ybshang;
    int pipeikaishi;
    Yuzhi *pizhi;
    int jiajiao(int jizhun, int jiao);
    int max1;
    int min1;
    int B2;
    int G2;
    int R2;
    CString m_str;
    int baoliux;
    int baoliuy;
    // 路径
    int startpipei;
    int qishix;
    int qishiy;
    int qishijiao;
    int zuixiaox2;
    int zuixiaoy2;
    unsigned char **BW_data_;
    int **area_grow_dataA;
    bool **area_grow_datao;
    unsigned int *area_grow_dataAtcx; // currently当前生长的一圈的纪录
    unsigned int *area_grow_dataAtcy; // currently当前生长的一圈的纪录
    unsigned int area_grow_dataAtcl;  // 纪录currently当前这一圈有几个像素
    unsigned int *area_grow_dataAtpx; // pre-	上一次生长的一圈的纪录
    unsigned int *area_grow_dataAtpy; // pre-	上一次生长的一圈的纪录
    unsigned int area_grow_dataAtpl;  // 纪录pre-	上一次这一圈有几个像素
    int *pathx;                       // 存储路径的 顺序坐标
    int *pathy;
    int pathl;
    //	int pathxII[1000];//用线段 拟和后的路径 端点坐标
    //	int pathyII[1000];
    int pathlII;
    // 路ing
    Yuzhi ball_1;
    CTxtArray m_txtar;

    //	byte qiu[320][240];
    //	byte dui[320][240];
    //	byte ji[320][240];

    int xxx;
    int m_nCY;
    int m_nCX;
    int m_nPosition;
    int m_nWidth;
    int m_nHeight;
    BYTE *m_pImageBuf;
    int H, S, I;
    int pipeijishu;
    UCHAR *psetclr;
};

#endif // !defined(AFX_UPRCOLOR_H__DEBCC6BB_A839_4DC6_ABF4_56443BAEC536__INCLUDED_)
