// CDebug_Display.h
// 注意：当运动跟踪目标切换时 原来的积分量速度记录 等还在保存 会引起初运动不正常

#ifndef Debug_Display_h
#define Debug_Display_h

// #include "ObjPropertiesStruct.h"
#include ".././ConstantFile/CamConstantSet.h"
#include <highgui.h>
#include "cv.h"
#include "cxcore.h"

// #define DEBUG_DISPLAY_ENABLE
#define DEBUG_DISPLAY_GLOBAL_TIME

#define def_display_value_h 100 // 默认显示数值区域标准高度
// 并不是显示区域之在上述 +-100 条框里  只是这个值与Draw_vp(,,,)函数里的 int Value值域参数 按照y=Value=1000 out=100  out
// = y*100/value  out = y*def_display_value_h/value
#define def_display_height OMNI_VISION_CAM_HEIGHT // 默认图像存储区域高度
#define def_display_width OMNI_VISION_CAM_WIDTH   // 默认图像存储区域宽度
#define def_display_widthM3 def_display_width * 3

class CDebug_Display
{
  protected:
    IplImage *ImgOut;
    BYTE *m_pImageBuf; // 存储当前图像指针
    UCHAR *psetclr;    // 存储图像内像素指针
    int m_nHeight;
    int m_nWidth;
    int m_nWidth_m3;       // 便于作点操作
    float display_value_h; // 默认显示数值区域标准高度out = y*def_display_value_h/value
    int x;                 // x坐标轴位置
    int y;                 // y坐标轴位置
    int t;                 // 时间坐标 贞坐标

    char save_frame_name[100]; // 本个实例的 数据显示名称
    char strShow[100];
    CvFont out_txt;
    int displayline;
    int init_win_flag;
    int init_img_memory_flag;
    INT offsetX;
    INT offsetY;
    double zoom;
    int time_point;

  public:
    float *save_frame;

    int scaleX(double X);
    int scaleY(double Y);
    BOOL Draw_point(int x, int y, CvScalar color);            // opencv Draw_point on	ImgOut
    BOOL Draw_point_scale(int x, int y, int r, int g, int b); // opencv Draw_point on
                                                              // ImgOut可以通过键盘统一缩放平移版本
    BOOL DrawCircle_scale(int x, int y, int radius, int r, int g,
                          int b); // opencv Draw Circle on	ImgOut可以通过键盘统一缩放平移版本
    BOOL InitOpenWindow(int width, int height, char *WIN_NAME); // opencv 创建叫WIN_NAME的窗口
    BOOL InitImgMemory(int width, int height);                  // opencv 创建ImgOut 存储空间
    void read_key_set_display_onwin();                          // 读键盘按键 设置显示缩放 平移等
    // 在叫WIN_NAME的窗口画点 系列数据
    BOOL Draw_point_string_onwin(float *x, float *y, int pointNUM, unsigned char r, unsigned char g, unsigned char b,
                                 char *WIN_NAME);
    BOOL Draw_point_string_onwin(CDebug_Display *x, CDebug_Display *y, int pointNUM, unsigned char r, unsigned char g,
                                 unsigned char b, char *WIN_NAME);
    // 在ImgOut上叠加点画点 系列数据
    BOOL Draw_point_string(float *x, float *y, int pointNUM, unsigned char r, unsigned char g, unsigned char b);
    BOOL Draw_point_string(CDebug_Display *x, CDebug_Display *y, int pointNUM, unsigned char r, unsigned char g,
                           unsigned char b);
    // 在ImgOut上叠加画数字 系列数据
    BOOL Draw_vp_string(float *x, char *Name, unsigned char r, unsigned char g, unsigned char b);
    BOOL Draw_vp_string(CDebug_Display *x, unsigned char r, unsigned char g, unsigned char b);
    // 在ImgOut上叠加极坐标 系列数据 坐标显示 y正（上方）0度 右正（） 左负
    BOOL Draw_line_string_polar_coordinates(float *radius, float *a, int ox, int oy, int pointNUM, unsigned char r,
                                            unsigned char g, unsigned char b);
    BOOL Draw_line_string_polar_coordinates(CDebug_Display *radius, CDebug_Display *a, int ox, int oy, int pointNUM,
                                            unsigned char r, unsigned char g, unsigned char b);
    // 显示 距离标定图像
    BOOL Draw_scaling_onwin(unsigned char *buf, unsigned char r, unsigned char g, unsigned char b, char *WIN_NAME);
    BOOL Draw_vp_(CHAR *Disname, float y, int y_add, float Value, char enable, unsigned char r, unsigned char g,
                  unsigned char b, BYTE *Buf);

    BOOL Clean_Zero_Save_Frame(CDebug_Display *t);
    int clean_zero;
    int Debug_Display_Circle_End(void); // 表明这个纪录周期结束  global_t++;

    CDebug_Display();
    ~CDebug_Display();
    // virtual 虚函数字头
    void ImageBuf(int Width, int Height, BYTE *Buf); // 将图像的宽度高度指针 给CDebug_Display类
    BOOL SetColor(int x, int y, RGBTRIPLE rgb);
    BOOL SetColor(int x, int y, RGBTRIPLE rgb, BYTE *Buf);

    BOOL Draw_vp(float y, int y_add, float quotiety,
                 char enable); // DrawValuePoint  //y:  x轴(既：t时间)高度     y_add:y偏置量   quotiety:数值缩放比例
    BOOL Draw_vp(float y, int y_add, float quotiety, char enable, unsigned char r, unsigned char g, unsigned char b,
                 BYTE *Buf); // y:  x轴(既：t时间)高度     y_add:y偏置量   quotiety:数值缩放比例
    BOOL Draw_vp_(float y, int y_add, float Value, char enable, unsigned char r, unsigned char g, unsigned char b,
                  BYTE *Buf); // y:  x轴(既：t时间)高度     y_add:y偏置量   Value:数值值域
    BOOL Draw_vp_CV(float y, int y_add, float Value, char enable, unsigned char r, unsigned char g, unsigned char b,
                    char *WIN_NAME); // y:  x轴(既：t时间)高度     y_add:y偏置量   Value:数值值域
    // BOOL Draw_circle	(int x,int y,int r);
    // BOOL Draw_box		(int x1,int y1,int x2,int y2);
    // BOOL Draw_triangle	(int x1,int y1,int x2,int y2,int x3,int y3);
    // BOOL Draw_triangle	(int x ,int y ,float angle);
    BOOL Draw_triangle(int x, int y, float angle, float radius, unsigned char r, unsigned char g, unsigned char b,
                       BYTE *Buf);
    BOOL Draw_circle(int x, int y, float angle_start, float angle_end, float radius, unsigned char r, unsigned char g,
                     unsigned char b, BYTE *Buf);
    BOOL Draw_line(int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, BYTE *Buf);
    BOOL Draw_line_(int x, int y, float angle, float radius, unsigned char r, unsigned char g, unsigned char b,
                    BYTE *Buf);
};

#endif Debug_Display_h
