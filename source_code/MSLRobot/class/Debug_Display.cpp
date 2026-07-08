// Debug_Display.cpp

#include "stdafx.h"
#include "Debug_Display.h"
#include "math.h"

#ifndef Debug_Display_cpp
#define Debug_Display_cpp

int global_t = 0;

CDebug_Display::CDebug_Display()
{
    display_value_h = def_display_value_h;
    x = 0; // x坐标轴位置
    y = 0; // y坐标轴位置
    t = 0; // 时间坐标 贞坐标
    m_nHeight = def_display_height;
    m_nWidth = def_display_width;
    m_nWidth_m3 = m_nWidth * 3;    // 便于作点操作
    save_frame = new float[30000]; // 创建300秒的存储空间
    for (int i = 0; i < 30000; i++)
    {
        save_frame[i] = 0;
    }

    offsetX = 0;
    offsetY = 0;
    zoom = 1;
    time_point = 0;
    init_win_flag = 0;
    init_img_memory_flag = 0;

    cvInitFont(&out_txt, CV_FONT_VECTOR0, 0.5, 0.5, 0, 1, 8);
    displayline = 0; // 记录当前显示到第几行文字
    sprintf(strShow, " ");
    sprintf(save_frame_name, " ");

    clean_zero = 0;
}

CDebug_Display::~CDebug_Display()
{
    x = 0; // x坐标轴位置
    y = 0; // y坐标轴位置
    t = 0; // 时间坐标 贞坐标
    delete save_frame;

    if (init_img_memory_flag == 1)
    {
        init_img_memory_flag = 0;
        cvReleaseImage(&ImgOut);
    }
    cvDestroyAllWindows();
}

// 将图像的宽度高度指针 给CDebug_Display类
void CDebug_Display::ImageBuf(int Width, int Height, BYTE *Buf)
{
    m_nHeight = Height;
    m_nWidth = Width;
    m_nWidth_m3 = m_nWidth * 3; // 便于作点操作
    m_pImageBuf = Buf;
}

BOOL CDebug_Display::SetColor(int x, int y, RGBTRIPLE rgb)
{
    //	return TRUE;
    // 将x,y点赋值成rgb色
    if (x > m_nWidth - 1 || y > m_nHeight - 1 || x < 0 || y < 0)
        return FALSE;
    psetclr = m_pImageBuf + y * m_nWidth_m3 + x * 3;
    *psetclr = rgb.rgbtBlue;
    psetclr++;
    *psetclr = rgb.rgbtGreen;
    psetclr++;
    *psetclr = rgb.rgbtRed;
    // m_pImageBuf[y*1920 + x*3] = rgb.rgbtBlue;
    // m_pImageBuf[y*1920 + x*3 + 1] = rgb.rgbtGreen;
    // m_pImageBuf[y*1920 + x*3 + 2] = rgb.rgbtRed;
    return TRUE;
}

BOOL CDebug_Display::SetColor(int x, int y, RGBTRIPLE rgb, BYTE *Buf)
{
    //	return TRUE;
    // 将x,y点赋值成rgb色
    if (x > m_nWidth - 1 || y > m_nHeight - 1 || x < 0 || y < 0)
        return FALSE;
    psetclr = Buf + y * m_nWidth_m3 + x * 3;
    *psetclr = rgb.rgbtBlue;
    psetclr++;
    *psetclr = rgb.rgbtGreen;
    psetclr++;
    *psetclr = rgb.rgbtRed;
    // m_pImageBuf[y*1920 + x*3] = rgb.rgbtBlue;
    // m_pImageBuf[y*1920 + x*3 + 1] = rgb.rgbtGreen;
    // m_pImageBuf[y*1920 + x*3 + 2] = rgb.rgbtRed;
    return TRUE;
}
// DrawValuePoint  y:  x轴(既：t时间)高度     y_add:y偏置量   Value:实际数值		enable==1记录使能为1就记录
BOOL CDebug_Display::Draw_vp_(float y, int y_add, float Value, char enable, unsigned char r, unsigned char g,
                              unsigned char b, BYTE *Buf)
{ //	return 1;//
    return Draw_vp(y, y_add, display_value_h / Value, enable, r, g, b, Buf);
}

BOOL CDebug_Display::Draw_vp_CV(float y, int y_add, float Value, char enable, unsigned char r, unsigned char g,
                                unsigned char b,
                                char *WIN_NAME) // y:  x轴(既：t时间)高度     y_add:y偏置量   Value:数值值域
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif

    BOOL temp = 0;
    if (t == 0)
    {
        cvNamedWindow(WIN_NAME, 1);
        CvSize ImgOutSize;                                   // 设置显示图像大小 长宽存在这个结构体中
        ImgOutSize.width = OMNI_VISION_CAM_WIDTH;            // 设置宽
        ImgOutSize.height = OMNI_VISION_CAM_HEIGHT;          // 设置长
        ImgOut = cvCreateImage(ImgOutSize, IPL_DEPTH_8U, 3); // 创建存储区
    }
    unsigned char *Buf = (unsigned char *)ImgOut->imageData;
    temp = Draw_vp(y, y_add, display_value_h / Value, enable, r, g, b, Buf);
    cvShowImage(WIN_NAME, ImgOut);
    cvWaitKey(1);
    return (temp);
}
// y:  x轴(既：t时间)高度     y_add:y偏置量   quotiety:数值缩放比例		enable==1记录使能为1就记录
BOOL CDebug_Display::Draw_vp(float y, int y_add, float quotiety, char enable)
{
    return Draw_vp(y, y_add, quotiety, enable, 0, 255, 0, m_pImageBuf);
}

// y:  x轴(既：t时间)高度     y_add:y偏置量   quotiety:数值缩放比例		enable==1记录使能为1就记录    R G B 视频存储区
BOOL CDebug_Display::Draw_vp(float y, int y_add, float quotiety, char enable, unsigned char r, unsigned char g,
                             unsigned char b, BYTE *Buf)
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif

    int i;
    RGBTRIPLE draw_color;
    draw_color.rgbtBlue = b;
    draw_color.rgbtGreen = g;
    draw_color.rgbtRed = r;
    RGBTRIPLE draw_color_;
    draw_color_.rgbtBlue = 255 - b;
    draw_color_.rgbtGreen = 255 - g;
    draw_color_.rgbtRed = 255 - r;

#ifdef DEBUG_DISPLAY_GLOBAL_TIME
    if (enable == 1) // 记录使能 为1 就记录
    {
        save_frame[global_t] = y;
        return TRUE;
    }
#endif

#ifndef DEBUG_DISPLAY_GLOBAL_TIME
    if (enable == 1) // 记录使能 为1 就记录
    {
        save_frame[t] = y;
    }
    if (enable == 1) // 记录使能 为1 就记录
    {
        t++;
        if (t >= 30000)
        {
            t = 0;
        }
        return TRUE;
    }
#endif

    if (t < m_nWidth)
    {
        for (i = 0; i < m_nWidth; i++)
        {
            SetColor(i, y_add, draw_color, Buf);                                        // 画坐标轴
            SetColor(i, (int)(save_frame[i] * quotiety) + 1 + y_add, draw_color_, Buf); //(int)(save_frame[i]+y_add)
            SetColor(i, (int)(save_frame[i] * quotiety) + y_add, draw_color, Buf);      //(int)(save_frame[i]+y_add)
            SetColor(i, (int)(save_frame[i] * quotiety) - 1 + y_add, draw_color_, Buf); //(int)(save_frame[i]+y_add)
        }
    }
    else
    {
        for (i = t - m_nWidth; i <= t; i++)
        {
            SetColor(m_nWidth - (t - i), y_add, draw_color, Buf); // 画坐标轴
            SetColor(m_nWidth - (t - i), (int)(save_frame[i] * quotiety) + 1 + y_add, draw_color_, Buf);
            SetColor(m_nWidth - (t - i), (int)(save_frame[i] * quotiety) + y_add, draw_color, Buf);
            SetColor(m_nWidth - (t - i), (int)(save_frame[i] * quotiety) - 1 + y_add, draw_color_, Buf);
        }
    }
    return TRUE;
}

BOOL CDebug_Display::Draw_line(int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b,
                               BYTE *Buf)
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif

    //	if (x1>m_nWidth-1 || y1>m_nHeight-1 || x1<0 || y1<0 || x2>m_nWidth-1 || y2>m_nHeight-1 || x2<0 || y2<0)
    //		return FALSE;;
    RGBTRIPLE draw_color;
    draw_color.rgbtBlue = b;
    draw_color.rgbtGreen = g;
    draw_color.rgbtRed = r;
    float i, j;
    j = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    if (j <= 0)
        return FALSE;
    float x = x1;
    float y = y1;
    float xadd = (x2 - x1) / j;
    float yadd = (y2 - y1) / j;
    for (i = 0; i <= j; i++)
    {
        SetColor(x, y, draw_color, Buf); // 画线段
        x += xadd;
        y += yadd;
    }
    return TRUE;
}

BOOL CDebug_Display::Draw_line_(int x, int y, float angle, float radius, unsigned char r, unsigned char g,
                                unsigned char b, BYTE *Buf)
{
    RGBTRIPLE draw_color;
    draw_color.rgbtBlue = b;
    draw_color.rgbtGreen = g;
    draw_color.rgbtRed = r;

    //	float rad,x1,y1;
    //	for(rad=0;rad<=radius;rad++)//半径50以下是机体不算
    //	{	x1=(int)(cos(-angle*3.14/180.0)*rad)+x;
    //		y1=(int)(sin(-angle*3.14/180.0)*rad)+y;
    //		SetColor(x1,y1, draw_color , Buf);//画线段
    //	}
    Draw_line(x, y, (int)(cos(-angle * 3.14 / 180.0) * radius) + x, (int)(sin(-angle * 3.14 / 180.0) * radius) + y, r,
              g, b, Buf);
    return TRUE;
}

BOOL CDebug_Display::Draw_triangle(int x, int y, float angle, float radius, unsigned char r, unsigned char g,
                                   unsigned char b, BYTE *Buf)
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif

    Draw_line_(x + cos(-angle * 3.14 / 180.0) * radius, y + sin(-angle * 3.14 / 180.0) * radius, angle + 150,
               radius * 1.732, r, g, b, Buf);
    Draw_line_(x, y, angle, radius / 10, r, g, b, Buf);
    Draw_line_(x + cos(-(angle + 120) * 3.14 / 180.0) * radius, y + sin(-(angle + 120) * 3.14 / 180.0) * radius,
               angle + 120 + 150, radius * 1.732, r, g, b, Buf);
    Draw_line_(x, y, angle + 120, radius / 10, r, g, b, Buf);
    Draw_line_(x + cos(-(angle - 120) * 3.14 / 180.0) * radius, y + sin(-(angle - 120) * 3.14 / 180.0) * radius,
               angle - 120 + 150, radius * 1.732, r, g, b, Buf);
    Draw_line_(x, y, angle - 120, radius / 10, r, g, b, Buf);
    return TRUE;
}

BOOL CDebug_Display::Draw_circle(int x, int y, float angle_start, float angle_end, float radius, unsigned char r,
                                 unsigned char g, unsigned char b, BYTE *Buf)
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif

    float angle_start_t = angle_start;
    float angle_end_t = angle_end;
    if (angle_start_t > angle_end)
    {
        angle_start_t = angle_end;
        angle_end_t = angle_start;
    }

    RGBTRIPLE draw_color;
    draw_color.rgbtBlue = b;
    draw_color.rgbtGreen = g;
    draw_color.rgbtRed = r;
    float point_angle; // 当前绘制点角度
    int i;
    float point_angle_d; // 每个绘制点的角度差
    float point_x, point_y;
    point_angle_d = 360 / (2 * 3.14 * radius);
    for (point_angle = angle_start_t; point_angle < angle_end_t; point_angle += point_angle_d) // 2*3.14*radius
    {
        point_x = cos(point_angle * 3.14 / 180) * radius + x;
        point_y = -sin(point_angle * 3.14 / 180) * radius + y;
        SetColor(point_x, point_y, draw_color, Buf);
    }
    return TRUE;
}

BOOL CDebug_Display::Draw_point(int x, int y, CvScalar color)
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif
    if (x > ImgOut->width - 1 || y > ImgOut->height - 1 || x < 0 || y < 0)
        return FALSE;
    unsigned char *Buf = (unsigned char *)ImgOut->imageData;
    Buf += y * ImgOut->widthStep + x * 3;
    *Buf = unsigned char(color.val[0]);
    Buf++;
    *Buf = unsigned char(color.val[1]);
    Buf++;
    *Buf = unsigned char(color.val[2]);
    return (1);
}
// 可以通过键盘统一缩放平移
BOOL CDebug_Display::Draw_point_scale(int x, int y, int r, int g, int b)
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif
    InitImgMemory(640, 480);
    return (Draw_point(scaleX(x), scaleY(y), CV_RGB(r, g, b)));
}
// 可以通过键盘统一缩放平移
BOOL CDebug_Display::DrawCircle_scale(int x, int y, int radius, int r, int g, int b)
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif
    InitImgMemory(640, 480);
    cvDrawCircle(ImgOut, cvPoint(scaleX(x), scaleY(y)), radius, CV_RGB(r, g, b), 1, 8, 0);
    return (1);
}

BOOL CDebug_Display::InitImgMemory(int width, int height)
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif
    if (init_img_memory_flag == 0)
    {
        init_img_memory_flag = 1;
        CvSize ImgOutSize;                                   // 设置显示图像大小 长宽存在这个结构体中
        ImgOutSize.width = width;                            // 设置宽
        ImgOutSize.height = height;                          // 设置长
        ImgOut = cvCreateImage(ImgOutSize, IPL_DEPTH_8U, 3); // 创建存储区
        ImgOut->origin = 1;
    }
    return (1);
}

BOOL CDebug_Display::InitOpenWindow(int width, int height, char *WIN_NAME)
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif
    InitImgMemory(width, height);
    if (init_win_flag == 0)
    {
        init_win_flag = 1;
        cvNamedWindow(WIN_NAME, 1);
    }
    return (1);
}
void CDebug_Display::read_key_set_display_onwin()
{
    char getkey = cvWaitKey(10);
    if (getkey == 'w')
        offsetY += 10;
    if (getkey == 's')
        offsetY -= 10;
    if (getkey == 'a')
        offsetX -= 10;
    if (getkey == 'd')
        offsetX += 10;
    if (getkey == 'z')
        zoom--;
    if (getkey == 'x')
        zoom++;
    if (getkey == 'c')
        time_point++;
    if (getkey == 'v')
        time_point--;
    if (time_point < 0)
        time_point = 0;

    if (getkey == 'r')
    {
        global_t = 0;
        time_point = 0;
    }
}
int CDebug_Display::scaleX(double X)
{
    return (int(X * pow(1.5, zoom) + offsetX));
}
int CDebug_Display::scaleY(double Y)
{
    return (int(Y * pow(1.5, zoom) + offsetY));
}

// 只向ImgOut画点 不显示
BOOL CDebug_Display::Draw_vp_string(float *x, char *Name, unsigned char r, unsigned char g, unsigned char b)
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif
    InitImgMemory(640, 480);
    sprintf(strShow, "%s:%6.1f", Name, x[time_point]);
    displayline += 1;
    cvPutText(ImgOut, strShow, cvPoint(20, displayline * 20 + 10), &out_txt, CV_RGB(r, g, b));
    return (1);
}

// 只向ImgOut画点 不显示
BOOL CDebug_Display::Draw_vp_string(CDebug_Display *x, unsigned char r, unsigned char g, unsigned char b)
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif
    InitImgMemory(640, 480);
    Clean_Zero_Save_Frame(x);
    sprintf(strShow, "%s:%6.1f", x->save_frame_name, x->save_frame[time_point]);
    displayline += 1;
    cvPutText(ImgOut, strShow, cvPoint(20, displayline * 20 + 10), &out_txt, CV_RGB(r, g, b));
    return (1);
}

// 只向ImgOut画点 不显示
BOOL CDebug_Display::Draw_point_string(CDebug_Display *x, CDebug_Display *y, int pointNUM, unsigned char r,
                                       unsigned char g, unsigned char b)
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif
    InitImgMemory(640, 480);
    Clean_Zero_Save_Frame(x);
    Clean_Zero_Save_Frame(y);
    int i;
    for (i = time_point; i < pointNUM; i++)
    {
        Draw_point(scaleX(x->save_frame[i]), scaleY(y->save_frame[i]), CV_RGB(r, g, b));
    }
    Draw_point(scaleX(x->save_frame[time_point]), scaleY(y->save_frame[time_point]), CV_RGB(255 - r, 255 - g, 255 - b));
    Draw_point(scaleX(x->save_frame[time_point]) + 1, scaleY(y->save_frame[time_point]), CV_RGB(r, g, b));
    Draw_point(scaleX(x->save_frame[time_point]) - 1, scaleY(y->save_frame[time_point]), CV_RGB(r, g, b));
    Draw_point(scaleX(x->save_frame[time_point]), scaleY(y->save_frame[time_point]) + 1, CV_RGB(r, g, b));
    Draw_point(scaleX(x->save_frame[time_point]), scaleY(y->save_frame[time_point]) - 1, CV_RGB(r, g, b));

    // 在图像中显示文本字符串
    sprintf(strShow, "%s,%s:(%4.1f,%4.1f)zoom%3.1f,ox%3d,oy%3d", x->save_frame_name, y->save_frame_name,
            x->save_frame[time_point], y->save_frame[time_point], pow(1.5, zoom), offsetX, offsetY);
    displayline += 1;
    cvPutText(ImgOut, strShow, cvPoint(20, displayline * 20 + 10), &out_txt, CV_RGB(r, g, b));
    return (1);
}
// 只向ImgOut画点 不显示
BOOL CDebug_Display::Draw_point_string(float *x, float *y, int pointNUM, unsigned char r, unsigned char g,
                                       unsigned char b)
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif
    InitImgMemory(640, 480);
    int i;
    for (i = time_point; i < pointNUM; i++)
    {
        Draw_point(scaleX(x[i]), scaleY(y[i]), CV_RGB(r, g, b));
    }
    Draw_point(scaleX(x[time_point]), scaleY(y[time_point]), CV_RGB(255 - r, 255 - g, 255 - b));
    Draw_point(scaleX(x[time_point]) + 1, scaleY(y[time_point]), CV_RGB(r, g, b));
    Draw_point(scaleX(x[time_point]) - 1, scaleY(y[time_point]), CV_RGB(r, g, b));
    Draw_point(scaleX(x[time_point]), scaleY(y[time_point]) + 1, CV_RGB(r, g, b));
    Draw_point(scaleX(x[time_point]), scaleY(y[time_point]) - 1, CV_RGB(r, g, b));

    sprintf(strShow, "(%4.1f,%4.1f)", x[time_point], y[time_point]);
    displayline += 1;
    cvPutText(ImgOut, strShow, cvPoint(20, displayline * 20 + 10), &out_txt, CV_RGB(r, g, b));
    return (1);
}
BOOL CDebug_Display::Draw_point_string_onwin(CDebug_Display *x, CDebug_Display *y, int pointNUM, unsigned char r,
                                             unsigned char g, unsigned char b, char *WIN_NAME)
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif
    InitOpenWindow(640, 480, WIN_NAME);
    Clean_Zero_Save_Frame(x);
    Clean_Zero_Save_Frame(y);

    int i;
    for (i = time_point; i < pointNUM; i++)
    {
        Draw_point(scaleX(x->save_frame[i]), scaleY(y->save_frame[i]), CV_RGB(r, g, b));
    }
    Draw_point(scaleX(x->save_frame[time_point]), scaleY(y->save_frame[time_point]), CV_RGB(255 - r, 255 - g, 255 - b));
    Draw_point(scaleX(x->save_frame[time_point]) + 1, scaleY(y->save_frame[time_point]), CV_RGB(r, g, b));
    Draw_point(scaleX(x->save_frame[time_point]) - 1, scaleY(y->save_frame[time_point]), CV_RGB(r, g, b));
    Draw_point(scaleX(x->save_frame[time_point]), scaleY(y->save_frame[time_point]) + 1, CV_RGB(r, g, b));
    Draw_point(scaleX(x->save_frame[time_point]), scaleY(y->save_frame[time_point]) - 1, CV_RGB(r, g, b));

    // 在图像中显示文本字符串
    displayline = 0;
    sprintf(strShow, "%s,%s:(%4.1f,%4.1f)zoom%3.1f,ox%3d,oy%3d,rec_time%d,view_time%d", x->save_frame_name,
            y->save_frame_name, x->save_frame[time_point], y->save_frame[time_point], pow(1.5, zoom), offsetX, offsetY,
            global_t, time_point);
    cvPutText(ImgOut, strShow, cvPoint(20, displayline * 20 + 10), &out_txt, CV_RGB(r, g, b));
    cvShowImage(WIN_NAME, ImgOut);
    read_key_set_display_onwin();
    cvZero(ImgOut);
    return TRUE;
}
// 向ImgOut画点 读键盘操作 显示 清零ImgOut
BOOL CDebug_Display::Draw_point_string_onwin(float *x, float *y, int pointNUM, unsigned char r, unsigned char g,
                                             unsigned char b, char *WIN_NAME)
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif
    InitOpenWindow(640, 480, WIN_NAME);

    int i;
    for (i = time_point; i < pointNUM; i++)
    {
        Draw_point(scaleX(x[i]), scaleY(y[i]), CV_RGB(r, g, b));
    }
    Draw_point(scaleX(x[time_point]), scaleY(y[time_point]), CV_RGB(255 - r, 255 - g, 255 - b));
    Draw_point(scaleX(x[time_point]) + 1, scaleY(y[time_point]), CV_RGB(r, g, b));
    Draw_point(scaleX(x[time_point]) - 1, scaleY(y[time_point]), CV_RGB(r, g, b));
    Draw_point(scaleX(x[time_point]), scaleY(y[time_point]) + 1, CV_RGB(r, g, b));
    Draw_point(scaleX(x[time_point]), scaleY(y[time_point]) - 1, CV_RGB(r, g, b));

    // 在图像中显示文本字符串
    displayline = 0;
    sprintf(strShow, "(%4.1f,%4.1f)zoom%3.1f,ox%3d,oy%3d,rec_time%d,view_time%d", x[time_point], y[time_point],
            pow(1.5, zoom), offsetX, offsetY, global_t, time_point);
    cvPutText(ImgOut, strShow, cvPoint(20, displayline * 20 + 10), &out_txt, CV_RGB(r, g, b));
    cvShowImage(WIN_NAME, ImgOut);
    read_key_set_display_onwin();
    cvZero(ImgOut);
    return TRUE;
}

// 向ImgOut画buf 的点 读键盘操作 显示 缩放后的图
BOOL CDebug_Display::Draw_scaling_onwin(unsigned char *buf, unsigned char r, unsigned char g, unsigned char b,
                                        char *WIN_NAME)
{
    return (0);
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif
    InitOpenWindow(640, 480, WIN_NAME);

    int x, y;
    for (y = 0; y < 480; y++)
        for (x = 0; x < 640; x++)
        {
            Draw_point(scaleX(x), scaleY(y),
                       CV_RGB(buf[(x + y * 640) * 3 + 2], buf[(x + y * 640) * 3 + 1], buf[(x + y * 640) * 3 + 0]));
        }

    // 在图像中显示文本字符串
    displayline = 0;
    sprintf(strShow, "(%4.1f,%4.1f)zoom%3.1f,ox%3d,oy%3d ", 1, 1, pow(1.5, zoom), offsetX, offsetY);
    cvPutText(ImgOut, strShow, cvPoint(20, displayline * 20 + 10), &out_txt, CV_RGB(r, g, b));
    cvShowImage(WIN_NAME, ImgOut);
    read_key_set_display_onwin();
    cvZero(ImgOut);
    return TRUE;
}

// 向ImgOut画点 读键盘操作 显示 清零ImgOut
BOOL CDebug_Display::Draw_line_string_polar_coordinates(float *radius, float *a, int ox, int oy, int pointNUM,
                                                        unsigned char r, unsigned char g, unsigned char b)
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif

    InitImgMemory(640, 480);
    cvDrawLine(ImgOut,
               cvPoint(scaleX(sin(a[time_point] * 3.14 / 180.0) * radius[time_point]) - offsetX + ox,
                       scaleY(cos(a[time_point] * 3.14 / 180.0) * radius[time_point]) - offsetY + oy),
               cvPoint(ox, oy), CV_RGB(r, g, b), 1, 8, 0);

    // 在图像中显示文本字符串
    sprintf(strShow, "(r%4.1f,a%4.1f)", radius[time_point], a[time_point]);
    displayline += 1;
    cvPutText(ImgOut, strShow, cvPoint(20, displayline * 20 + 10), &out_txt, CV_RGB(r, g, b));
    return TRUE;
}

// 向ImgOut画点 读键盘操作 显示 清零ImgOut
BOOL CDebug_Display::Draw_line_string_polar_coordinates(CDebug_Display *radius, CDebug_Display *a, int ox, int oy,
                                                        int pointNUM, unsigned char r, unsigned char g, unsigned char b)
{
#ifndef DEBUG_DISPLAY_ENABLE
    return (0);
#endif

    InitImgMemory(640, 480);
    Clean_Zero_Save_Frame(radius);
    Clean_Zero_Save_Frame(a);
    cvDrawLine(
        ImgOut,
        cvPoint(scaleX(sin(a->save_frame[time_point] * 3.14 / 180.0) * radius->save_frame[time_point]) - offsetX + ox,
                scaleY(cos(a->save_frame[time_point] * 3.14 / 180.0) * radius->save_frame[time_point]) - offsetY + oy),
        cvPoint(ox, oy), CV_RGB(r, g, b), 1, 8, 0);

    // 在图像中显示文本字符串
    sprintf(strShow, "%s,%s(r%4.1f,a%4.1f)", radius->save_frame_name, a->save_frame_name,
            radius->save_frame[time_point], a->save_frame[time_point]);
    displayline += 1;
    cvPutText(ImgOut, strShow, cvPoint(20, displayline * 20 + 10), &out_txt, CV_RGB(r, g, b));
    return TRUE;
}

// DrawValuePoint  y:  x轴(既：t时间)高度     y_add:y偏置量   Value:实际数值		enable==1记录使能为1就记录
BOOL CDebug_Display::Draw_vp_(CHAR *Disname, float y, int y_add, float Value, char enable, unsigned char r,
                              unsigned char g, unsigned char b, BYTE *Buf)
{ //	return 1;//
    strcpy(save_frame_name, Disname);
    return Draw_vp(y, y_add, display_value_h / Value, enable, r, g, b, Buf);
}

BOOL CDebug_Display::Clean_Zero_Save_Frame(CDebug_Display *t)
{
    if (global_t > 0)
    {
        t->clean_zero = 1;
    }
    else if (global_t == 0 && t->clean_zero == 1)
    {
        t->clean_zero = 0;
        for (int i = 0; i < 30000; i++)
            t->save_frame[i] = 0;
    }
    return (1);
}
// 表明这个纪录周期结束  global_t++;
int CDebug_Display::Debug_Display_Circle_End(void)
{
    global_t++;
    if (global_t >= 30000)
        global_t = 0;
    time_point = global_t;
    return (global_t);
}
#endif Debug_Display_cpp