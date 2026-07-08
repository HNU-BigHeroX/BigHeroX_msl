#pragma once

#include "stdafx.h"
#include "ObstFusion.h"

#define caomao_R 20
#define MAP_SCALE 0.3 // 障碍物生成图比例尺

int caomao_h = 10;
int caomao_b_min_area = 30;
int mask_init_flag = 0;

int omask_l; // 2~80 直径障碍物掩模
int omask_x[80 * 80 * 4];
int omask_y[80 * 80 * 4];
float omask_w[80 * 80 * 4];

const int Obst_map_W = int(1800 * MAP_SCALE + 2 * caomao_R);
const int Obst_map_H = int(1200 * MAP_SCALE + 2 * caomao_R);

int Obst_point_x[1000];
int Obst_point_y[1000];
double Obst_point_w[1000];
int Obst_point_l;

long Obst_map[Obst_map_H][Obst_map_W];
int Obst_map_B[Obst_map_H][Obst_map_W];

const int CS_OMNI_VISION_CAM_WIDTH = Obst_map_W;
const int CS_OMNI_VISION_CAM_HEIGHT = Obst_map_H;

unsigned int bmca_area_grow_data_arex[CS_OMNI_VISION_CAM_HEIGHT *
                                      CS_OMNI_VISION_CAM_WIDTH]; //[640*480];//currently当前生长的一圈的纪录
unsigned int bmca_area_grow_data_arey[CS_OMNI_VISION_CAM_HEIGHT *
                                      CS_OMNI_VISION_CAM_WIDTH]; //[640*480];//currently当前生长的一圈的纪录

unsigned int bool_data_in[CS_OMNI_VISION_CAM_HEIGHT][CS_OMNI_VISION_CAM_WIDTH];
unsigned int bool_data_out[CS_OMNI_VISION_CAM_HEIGHT][CS_OMNI_VISION_CAM_WIDTH];

unsigned int connect_max_area_order[1000];
int Obst_connect_area_x[1000];
int Obst_connect_area_y[1000];
int Obst_connect_area_w[1000];   // 连通域 面积
long Obst_connect_area_Ww[1000]; // 连通域 重量

unsigned int Obst_connect_area_num = 0;
long max_connectivity_weight = 0;

IplImage *imgCS_norm;
IplImage *imgCS;
IplImage *imgt_norm;
IplImage *imgt;

CObstFusion::CObstFusion()
{
    opencv_debug = true;
    m_fusObst = new obstacle[FULL_OST_NUM];

    HINSTANCE hInst = AfxGetResourceHandle();
    hBitmap1 = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BMP_L2R));
    hBitmap2 = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BMP_L2R2));

    imgt_norm = hBitmapToIpl(hBitmap1);
    imgt_norm->origin = 1;
    imgt = hBitmapToIpl(hBitmap1); // new IplImage;
    imgt->origin = 1;

    imgCS_norm = hBitmapToIpl(hBitmap2);
    imgCS_norm->origin = 1;
    imgCS = hBitmapToIpl(hBitmap2); // new IplImage;
    imgCS->origin = 1;
}

CObstFusion::~CObstFusion()
{
    delete[] m_fusObst;
    cvDestroyAllWindows();
    Sleep(100);
}

// hbitmap convert to IplImage
IplImage *CObstFusion::hBitmapToIpl(HBITMAP hBmp)
{
    BITMAP bmp;
    GetObject(hBmp, sizeof(BITMAP), &bmp);

    // get channels which equal 1 2 3 or 4
    // bmBitsPixel :
    // Specifies the number of bits
    // required to indicate the color of a pixel.
    int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;

    // get depth color bitmap or grayscale
    int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;

    // create header image
    IplImage *img = cvCreateImage(cvSize(bmp.bmWidth, bmp.bmHeight), depth, nChannels);

    // allocat memory for the pBuffer
    BYTE *pBuffer = new BYTE[bmp.bmHeight * bmp.bmWidth * nChannels];

    // copies the bitmap bits of a specified device-dependent bitmap into a buffer
    GetBitmapBits(hBmp, bmp.bmHeight * bmp.bmWidth * nChannels, pBuffer);

    // copy data to the imagedata
    memcpy(img->imageData, pBuffer, bmp.bmHeight * bmp.bmWidth * nChannels);
    delete pBuffer;

    // create the image
    IplImage *dst = cvCreateImage(cvGetSize(img), img->depth, 3);
    // convert color
    cvCvtColor(img, dst, CV_BGRA2BGR);
    cvReleaseImage(&img);
    return dst;
}

// 剔除场边和小禁区的障碍物
bool CObstFusion::m_Obst_RemoveOutOfField(obstacle *inObstAr)
{
    for (int i = 0; i < FULL_OST_NUM; i++)
    {
        if (inObstAr[i].valid == true)
        {
            const int o_dis = 50;
            // 场外的障碍物
            if (inObstAr[i].x < 0 + o_dis || inObstAr[i].x > 1800 - o_dis || inObstAr[i].y < 0 + o_dis ||
                inObstAr[i].y > 1200 - o_dis)
            {
                inObstAr[i].valid = false;
            }

            // 小禁区障碍物
            if (inObstAr[i].x > 0 && inObstAr[i].x < 75 && inObstAr[i].y > 0 + 425 && inObstAr[i].y < 1200 - 425)
            {
                inObstAr[i].valid = false;
            }
        }
    }
    return true;
}

int CObstFusion::m_CalDist(int inx1, int iny1, int inx2, int iny2)
{
    double ret = (inx1 - inx2) * (inx1 - inx2) + (iny1 - iny2) * (iny1 - iny2);
    ret = (int)sqrt(ret);
    return (int)ret;
}

void CObstFusion::circle_maskGA(float R, int *Gcmask_x, int *Gcmask_y, float *Gcmask_w, int *Gcmask_lo)
{
    // int R=3;//10;//5;
    int Gcmask_l = 0;
    int x, y;
    float temp_r;
    for (x = -R; x <= R; x++)
        for (y = -R; y <= R; y++)
        {
            temp_r = sqrt(y * y + x * x);
            if (temp_r <= R)
            {
                Gcmask_x[Gcmask_l] = x;
                Gcmask_y[Gcmask_l] = y;
                Gcmask_w[Gcmask_l++] = R - temp_r;
            }
        }

    *Gcmask_lo = Gcmask_l;
}

void CObstFusion::bool_max_connectivity_analyze2_n(void)
{
    register unsigned long int area_grow_data_arel = 0; // 纪录currently当前这一圈有几个像素

    RGBTRIPLE cPointG;
    cPointG.rgbtBlue = 0x00;
    cPointG.rgbtGreen = 0xFF;
    cPointG.rgbtRed = 0x00;
    RGBTRIPLE cPointB;
    cPointB.rgbtBlue = 0xFF;
    cPointB.rgbtGreen = 0x00;
    cPointB.rgbtRed = 0x00;

    // const char H_min_diff		=8;	//这是一个可调值关系到颜色的分辨粗略程度 如15以上黄色和橘黄就分不开了
    // const int grow_area_times =12000;//整个生成 生长区的次数//5000 要1min 40s
    // const int grow_min_weight	=40;//30如果单个生长区重量大于grow_min_weight 则把纪录A存入C[n]
    const int x320 = CS_OMNI_VISION_CAM_WIDTH; // 320;//320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240 = CS_OMNI_VISION_CAM_HEIGHT; // 240;//240;//处理图像数据的大小设置
    const int x320r1 = x320 - 1;                // 320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240r1 = y240 - 1;                // 240;//处理图像数据的大小设置

    register int i, j, x, y; //,k,l,seedx,seedy,,used_num
    // int x_grow,y_grow;
    // static unsigned char  area_grow_dataC[y240][x320];// 标号输出
    // register float  circumscription_weight;
    // register float  max_area_w;//最大的连通域的重量
    // register int    max_area_num;//最大的连通域的标号
    register int x_add, x_reduce, y_add, y_reduce, xg, yg;

    //	connect_area_l=0;
    register int connect_area_num =
        1; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点 2连通域的第一个编号 3 4 5....
    register int connect_max_area_num = 1;  // 最大连通域的编号	默认无效值1
    register int connect_area_w = 0;        // 记录连通域重量值
    register int connect_max_area_w = 0;    // 记录连通域最大重量值
    int connect_area_s_e_w[65536][3];       // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                            // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
    register int connect_area_layer_l = 0;  // 存储连通域 层的 点数 长度
    register int connect_area_layer_s = 0;  // 存储连通域 层的 首
    register int connect_area_layer_e = 0;  // 存储连通域 层的 尾
    register int connect_area_layer_pl = 0; // 存储连通域 层的 点数 长度
    register int connect_area_layer_ps = 0; // 存储连通域 上层的 首
    register int connect_area_layer_pe = 0; // 存储连通域 上层的 尾

    // used_num  =1;
    // max_area_w=0;
    // seedx=0;////////////////////
    // seedy=0;////////////////////

    ////////////////////////////////////
    // 黑边处理  防止出现边缘冒了//////
    for (x = 0; x < x320; x++)
    {
        bool_data_in[0][x] = 0;
        bool_data_in[y240 - 1][x] = 0;
    }
    for (y = 1; y < y240 - 1; y++)
    {
        bool_data_in[y][0] = 0;
        bool_data_in[y][x320 - 1] = 0;
    }
    // 黑边处理  防止出现边缘冒了//////

    /*测试代码
    for (y=0;y<480;y++)	for (x=0;x<640;x++)
    if (bool_data_in[y][x]==1)	OpenCv_DrawPoint(x,y,255,255,255,test);
    else						OpenCv_DrawPoint(x,y,0,0,0,test);
    cvShowImage("1",test);
    cvWaitKey(3000);
    */

    area_grow_data_arel = 0; // 所有连通域纪录 位置置零
    // 1自动设置这次生长的起点种子
    // for(i=0;i<circle_mask_long;i++)
    y = 0;
    while (y < CS_OMNI_VISION_CAM_HEIGHT)
    {
        x = 0;
        while (x < CS_OMNI_VISION_CAM_WIDTH)
        {
            if (bool_data_in[y][x] == 1) // 没生长过 且是有效点
            {                            // seedx=x;
                // seedy=y;
                // 2开始生长
                connect_area_num++; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点
                                    // 2连通域的第一个编号 3 4 5....
                connect_area_w = 0; // 记录连通域重量值	清零
                connect_area_s_e_w[connect_area_num][0] =
                    area_grow_data_arel; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                         // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
                // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
                // 1设置这次生长的起点种子
                bmca_area_grow_data_arex[area_grow_data_arel] = x;   // 当前次生长的一圈的纪录
                bmca_area_grow_data_arey[area_grow_data_arel++] = y; // 当前次生长的一圈的纪录
                connect_area_layer_l = 1;
                connect_area_layer_pe = connect_area_layer_ps + 1;
                //    		area_grow_dataAtpx[0]=x;//seedx;////手动设置种子
                //    		area_grow_dataAtpy[0]=y;//seedy;////手动设置种子
                //    		area_grow_dataAtpl=1;////手动设置种子
                connect_area_w++; // 连通域重量+1

                // 开始生长
                while (1) // for (i=0;i<1500;i++)//生成生长区域 的生长次数
                {
                    connect_area_layer_s = area_grow_data_arel;
                    j = connect_area_layer_ps;
                    while (j < connect_area_layer_pe)
                    {                                     // 周围不是A中的有效点且符合生长规则
                        xg = bmca_area_grow_data_arex[j]; // 当前次生长的一圈的纪录
                        yg = bmca_area_grow_data_arey[j]; // 当前次生长的一圈的纪录
                        x_add = xg + 1;
                        x_reduce = xg - 1;
                        y_add = yg + 1;
                        y_reduce = yg - 1;
                        if (bool_data_in[y_add][x_reduce] == 1)
                        {
                            bmca_area_grow_data_arey[area_grow_data_arel] = y_add;
                            bmca_area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                            bool_data_in[y_add][x_reduce] = connect_area_num;
                        }
                        if (bool_data_in[y_add][xg] == 1)
                        {
                            bmca_area_grow_data_arey[area_grow_data_arel] = y_add;
                            bmca_area_grow_data_arex[area_grow_data_arel++] = xg; // 当前次生长的一圈的纪录
                            bool_data_in[y_add][xg] = connect_area_num;
                        }
                        if (bool_data_in[y_add][x_add] == 1)
                        {
                            bmca_area_grow_data_arey[area_grow_data_arel] = y_add;
                            bmca_area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                            bool_data_in[y_add][x_add] = connect_area_num;
                        }
                        if (bool_data_in[yg][x_reduce] == 1)
                        {
                            bmca_area_grow_data_arey[area_grow_data_arel] = yg;
                            bmca_area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                            bool_data_in[yg][x_reduce] = connect_area_num;
                        }
                        //	||
                        //((area_grow_dataA[y+0][x+0]==1)&&(abs(dataHSV[y+0][x+0][0]-tempH)<5)&&(abs(dataHSV[y+0][x+0][2]-tempV)<30)&&(abs(dataHSV[y+0][x+0][1]-tempS)<40))
                        if (bool_data_in[yg][x_add] == 1)
                        {
                            bmca_area_grow_data_arey[area_grow_data_arel] = yg;
                            bmca_area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                            bool_data_in[yg][x_add] = connect_area_num;
                        }
                        if (bool_data_in[y_reduce][x_reduce] == 1)
                        {
                            bmca_area_grow_data_arey[area_grow_data_arel] = y_reduce;
                            bmca_area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                            bool_data_in[y_reduce][x_reduce] = connect_area_num;
                        }
                        if (bool_data_in[y_reduce][xg] == 1)
                        {
                            bmca_area_grow_data_arey[area_grow_data_arel] = y_reduce;
                            bmca_area_grow_data_arex[area_grow_data_arel++] = xg; // 当前次生长的一圈的纪录
                            bool_data_in[y_reduce][xg] = connect_area_num;
                        }
                        if (bool_data_in[y_reduce][x_add] == 1)
                        {
                            bmca_area_grow_data_arey[area_grow_data_arel] = y_reduce;
                            bmca_area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                            bool_data_in[y_reduce][x_add] =
                                connect_area_num; ////////////////////////////////////////////改区域生长 加参数
                        }
                        j++;
                    }
                    connect_area_layer_e = area_grow_data_arel;
                    connect_area_layer_l = connect_area_layer_e - connect_area_layer_s; // 连通域重量+当前一圈的点数
                    connect_area_layer_ps = connect_area_layer_s;                       // 存储连通域 上层的 首
                    connect_area_layer_pe = connect_area_layer_e;                       // 存储连通域 上层的 尾

                    connect_area_w += connect_area_layer_l;
                    if (connect_area_layer_l == 0)
                        break;

                    /*测试代码
                    for (test_y=0;test_y<480;test_y++)	for (test_x=0;test_x<640;test_x++)
                    if (bool_data_in[test_y][test_x]>1)	OpenCv_DrawPoint(test_x,test_y,255,255,255,test);
                    else								OpenCv_DrawPoint(test_x,test_y,0,0,0,test);
                    strShow.Format("%d %d",area_grow_data_arel,connect_area_num);
                    cvPutText(test, strShow , cvPoint(0, 40), &out_txt, CV_RGB(255,255,255));
                    cvShowImage("2",test);
                    cvWaitKey(5);

                    if (area_grow_data_arel>=640*480)
                    area_grow_data_arel=area_grow_data_arel;
                    */
                }
                // 生长完成
                // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
                connect_area_s_e_w[connect_area_num][1] =
                    area_grow_data_arel; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                         // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
                connect_area_s_e_w[connect_area_num][2] =
                    connect_area_w; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                    // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
                if (connect_max_area_w < connect_area_w)
                {
                    connect_max_area_w = connect_area_w;
                    connect_max_area_num = connect_area_num;
                }
            }

            x++;
        }
        y++;
    }
    // return;//
    //	for (i=0;i<area_grow_data_arel;i++)//测试代码
    //	{
    //		data[(bmca_area_grow_data_arey[i]*320+bmca_area_grow_data_arex[i])*3+0]=1;//测试代码//显示种子点
    //		data[(bmca_area_grow_data_arey[i]*320+bmca_area_grow_data_arex[i])*3+1]=254;//测试代码
    //		data[(bmca_area_grow_data_arey[i]*320+bmca_area_grow_data_arex[i])*3+2]=1;//测试代码
    //		m_VideoOCX.Show(m_Image);//测试代码				// display capture frame
    //		Sleep(200);
    //	}//测试代码

    ////////////////////////////////////////////////////////////////////////////////////
    //*obj_weight_map=connect_area_s_e_w[connect_max_area_num][2];//存储1000个连通域的 首 尾 标号
    //[首]connect_area_s_e[n][0] [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]

    // 带出黑白图象重心横坐标  obj_properties *All  All.x结果的重心的x坐标 All.y结果的重心的y坐标
    // 需要处理部分的掩膜数组  int *yan_mo_x,int *yan_mo_y
    // 需要处理部分的掩膜数组长度int yan_mo_long,
    for (i = 0; i <= connect_area_num; i++)
    {
        connect_max_area_order[i] = i;
        // connect_area_s_e_w[i][2]=i+6;//test code
    }
    unsigned int temp_order;
    for (i = 0; i < connect_area_num - 2; i++)
    {
        for (j = connect_area_num; j > 2 + i; j--)
        {
            if (connect_area_s_e_w[connect_max_area_order[j]][2] > connect_area_s_e_w[connect_max_area_order[j - 1]][2])
            {
                temp_order = connect_max_area_order[j - 1];
                connect_max_area_order[j - 1] = connect_max_area_order[j];
                connect_max_area_order[j] = temp_order;
            }
        }
    }

    register float OBJ_temp_b = 0;
    register float OBJ_temp_b_ = 0;
    register float OBJ_temp_c = 0;
    register float ball_op_x = 0;
    register float ball_op_y = 0;
    register float ball_op_weight = 0;
    register int wg;
    for (y = 0; y < CS_OMNI_VISION_CAM_HEIGHT; y++)
    {
        for (x = 0; x < CS_OMNI_VISION_CAM_WIDTH; x++)
        {
            bool_data_out[y][x] = 0;
        }
    }
    if (connect_area_num >= 2) // 最大连通域的编号	默认无效值1
    {
        for (j = 2; j <= connect_area_num; j++)
        {
            OBJ_temp_b = 0;
            OBJ_temp_b_ = 0;
            OBJ_temp_c = 0;
            for (i = connect_area_s_e_w[connect_max_area_order[j]][0];
                 i < connect_area_s_e_w[connect_max_area_order[j]][1]; i++)
            { //	SetColor(bmca_area_grow_data_arey[i],bmca_area_grow_data_arex[i],cPointB);
                xg = bmca_area_grow_data_arex[i];
                yg = bmca_area_grow_data_arey[i];
                wg = Obst_map[yg][xg];
                OBJ_temp_c += wg;
                OBJ_temp_b += xg * wg;
                OBJ_temp_b_ += yg * wg;

                //	data[(bmca_area_grow_data_arey[i]*320+bmca_area_grow_data_arex[i])*3+0]=254;//测试代码//显示种子点
                //	data[(bmca_area_grow_data_arey[i]*320+bmca_area_grow_data_arex[i])*3+1]=254;//测试代码
                //	data[(bmca_area_grow_data_arey[i]*320+bmca_area_grow_data_arex[i])*3+2]=254;//测试代码
                //	SetColor(y,x,cPointG);//测试代码
                bool_data_out[bmca_area_grow_data_arey[i]][bmca_area_grow_data_arex[i]] = j - 1;
            }
            if (OBJ_temp_c != 0) // 保险程序  当同步不佳时  有时Capture不到图象 就会出现所有象素都为零 这时做OBJ_temp_b
                                 // / OBJ_temp_c的除法就会产生分母为零的错误
            {
                Obst_connect_area_x[j - 1] = OBJ_temp_b / OBJ_temp_c;
                Obst_connect_area_y[j - 1] = OBJ_temp_b_ / OBJ_temp_c;
            }
            else
            {
                Obst_connect_area_x[j - 1] = 0;
                Obst_connect_area_y[j - 1] = 0;
            }
            Obst_connect_area_w[j - 1] = connect_area_s_e_w[connect_max_area_order[j]][2];
        }
        Obst_connect_area_num = connect_area_num - 1;
    }

    return;
    max_connectivity_weight = long(OBJ_temp_c);
    // m_VideoOCX.Show(m_Image);//测试代码				// display capture frame

    if (OBJ_temp_c != 0) // 保险程序  当同步不佳时  有时Capture不到图象 就会出现所有象素都为零 这时做OBJ_temp_b /
                         // OBJ_temp_c的除法就会产生分母为零的错误
    {
        ball_op_x = OBJ_temp_b / OBJ_temp_c;  //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+320
        ball_op_y = OBJ_temp_b_ / OBJ_temp_c; //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+240
    }
    ball_op_weight = OBJ_temp_c; // 准备把象素重量传给主逻辑函数
    //					HSI11->ball_x=ball_op_x;
    //					HSI11->ball_y=ball_op_y;
    //					HSI11->ball_zhong=ball_op_weight;

    // 完成二值化图像标重心程序
}

void CObstFusion::bool_max_connectivity_analyze2_n_(void)
{
    register unsigned long int area_grow_data_arel = 0; // 纪录currently当前这一圈有几个像素

    RGBTRIPLE cPointG;
    cPointG.rgbtBlue = 0x00;
    cPointG.rgbtGreen = 0xFF;
    cPointG.rgbtRed = 0x00;
    RGBTRIPLE cPointB;
    cPointB.rgbtBlue = 0xFF;
    cPointB.rgbtGreen = 0x00;
    cPointB.rgbtRed = 0x00;

    // const char H_min_diff		=8;	//这是一个可调值关系到颜色的分辨粗略程度 如15以上黄色和橘黄就分不开了
    // const int grow_area_times =12000;//整个生成 生长区的次数//5000 要1min 40s
    // const int grow_min_weight	=40;//30如果单个生长区重量大于grow_min_weight 则把纪录A存入C[n]
    const int x320 = CS_OMNI_VISION_CAM_WIDTH; // 320;//320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240 = CS_OMNI_VISION_CAM_HEIGHT; // 240;//240;//处理图像数据的大小设置
    const int x320r1 = x320 - 1;                // 320;//处理图像数据的大小设置  x值必须和原数据相同否则出现 行错误
    const int y240r1 = y240 - 1;                // 240;//处理图像数据的大小设置

    register int i, j, x, y; //,k,l,seedx,seedy,,used_num
    // int x_grow,y_grow;
    // static unsigned char  area_grow_dataC[y240][x320];// 标号输出
    // register float  circumscription_weight;
    // register float  max_area_w;//最大的连通域的重量
    // register int    max_area_num;//最大的连通域的标号
    register int x_add, x_reduce, y_add, y_reduce, xg, yg;

    //	connect_area_l=0;
    register int connect_area_num =
        1; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点 2连通域的第一个编号 3 4 5....
    register int connect_max_area_num = 1;  // 最大连通域的编号	默认无效值1
    register int connect_area_w = 0;        // 记录连通域重量值
    register int connect_max_area_w = 0;    // 记录连通域最大重量值
    int connect_area_s_e_w[65536][3];       // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                            // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
    register int connect_area_layer_l = 0;  // 存储连通域 层的 点数 长度
    register int connect_area_layer_s = 0;  // 存储连通域 层的 首
    register int connect_area_layer_e = 0;  // 存储连通域 层的 尾
    register int connect_area_layer_pl = 0; // 存储连通域 层的 点数 长度
    register int connect_area_layer_ps = 0; // 存储连通域 上层的 首
    register int connect_area_layer_pe = 0; // 存储连通域 上层的 尾
    register long connect_area_weightT = 0;

    register long connect_area_weight[1000];

    // used_num  =1;
    // max_area_w=0;
    // seedx=0;////////////////////
    // seedy=0;////////////////////

    ////////////////////////////////////
    // 黑边处理  防止出现边缘冒了//////
    for (x = 0; x < x320; x++)
    {
        bool_data_in[0][x] = 0;
        bool_data_in[y240 - 1][x] = 0;
    }
    for (y = 1; y < y240 - 1; y++)
    {
        bool_data_in[y][0] = 0;
        bool_data_in[y][x320 - 1] = 0;
    }
    // 黑边处理  防止出现边缘冒了//////

    /*测试代码
    for (y=0;y<480;y++)	for (x=0;x<640;x++)
    if (bool_data_in[y][x]==1)	OpenCv_DrawPoint(x,y,255,255,255,test);
    else						OpenCv_DrawPoint(x,y,0,0,0,test);
    cvShowImage("1",test);
    cvWaitKey(3000);
    */

    area_grow_data_arel = 0; // 所有连通域纪录 位置置零
    // 1自动设置这次生长的起点种子
    // for(i=0;i<circle_mask_long;i++)
    y = 0;
    while (y < CS_OMNI_VISION_CAM_HEIGHT)
    {
        x = 0;
        while (x < CS_OMNI_VISION_CAM_WIDTH)
        {
            if (bool_data_in[y][x] == 1) // 没生长过 且是有效点
            {                            // seedx=x;
                // seedy=y;
                // 2开始生长
                connect_area_weightT = Obst_map[y][x];
                connect_area_num++; // 记录用到的连通域号	注意:这里的编号 0无效点 1未处理过的有效点
                                    // 2连通域的第一个编号 3 4 5....
                connect_area_w = 0; // 记录连通域重量值	清零
                connect_area_s_e_w[connect_area_num][0] =
                    area_grow_data_arel; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                         // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
                // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
                // 1设置这次生长的起点种子
                bmca_area_grow_data_arex[area_grow_data_arel] = x;   // 当前次生长的一圈的纪录
                bmca_area_grow_data_arey[area_grow_data_arel++] = y; // 当前次生长的一圈的纪录
                connect_area_layer_l = 1;
                connect_area_layer_pe = connect_area_layer_ps + 1;
                //    		area_grow_dataAtpx[0]=x;//seedx;////手动设置种子
                //    		area_grow_dataAtpy[0]=y;//seedy;////手动设置种子
                //    		area_grow_dataAtpl=1;////手动设置种子
                connect_area_w++; // 连通域重量+1

                // 开始生长
                while (1) // for (i=0;i<1500;i++)//生成生长区域 的生长次数
                {
                    connect_area_layer_s = area_grow_data_arel;
                    j = connect_area_layer_ps;
                    while (j < connect_area_layer_pe)
                    {                                     // 周围不是A中的有效点且符合生长规则
                        xg = bmca_area_grow_data_arex[j]; // 当前次生长的一圈的纪录
                        yg = bmca_area_grow_data_arey[j]; // 当前次生长的一圈的纪录
                        x_add = xg + 1;
                        x_reduce = xg - 1;
                        y_add = yg + 1;
                        y_reduce = yg - 1;
                        if (bool_data_in[y_add][x_reduce] == 1)
                        {
                            bmca_area_grow_data_arey[area_grow_data_arel] = y_add;
                            bmca_area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                            bool_data_in[y_add][x_reduce] = connect_area_num;
                            connect_area_weightT += Obst_map[y_add][x_reduce];
                        }
                        if (bool_data_in[y_add][xg] == 1)
                        {
                            bmca_area_grow_data_arey[area_grow_data_arel] = y_add;
                            bmca_area_grow_data_arex[area_grow_data_arel++] = xg; // 当前次生长的一圈的纪录
                            bool_data_in[y_add][xg] = connect_area_num;
                            connect_area_weightT += Obst_map[y_add][xg];
                        }
                        if (bool_data_in[y_add][x_add] == 1)
                        {
                            bmca_area_grow_data_arey[area_grow_data_arel] = y_add;
                            bmca_area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                            bool_data_in[y_add][x_add] = connect_area_num;
                            connect_area_weightT += Obst_map[y_add][x_add];
                        }
                        if (bool_data_in[yg][x_reduce] == 1)
                        {
                            bmca_area_grow_data_arey[area_grow_data_arel] = yg;
                            bmca_area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                            bool_data_in[yg][x_reduce] = connect_area_num;
                            connect_area_weightT += Obst_map[yg][x_reduce];
                        }
                        //	||
                        //((area_grow_dataA[y+0][x+0]==1)&&(abs(dataHSV[y+0][x+0][0]-tempH)<5)&&(abs(dataHSV[y+0][x+0][2]-tempV)<30)&&(abs(dataHSV[y+0][x+0][1]-tempS)<40))
                        if (bool_data_in[yg][x_add] == 1)
                        {
                            bmca_area_grow_data_arey[area_grow_data_arel] = yg;
                            bmca_area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                            bool_data_in[yg][x_add] = connect_area_num;
                            connect_area_weightT += Obst_map[yg][x_add];
                        }
                        if (bool_data_in[y_reduce][x_reduce] == 1)
                        {
                            bmca_area_grow_data_arey[area_grow_data_arel] = y_reduce;
                            bmca_area_grow_data_arex[area_grow_data_arel++] = x_reduce; // 当前次生长的一圈的纪录
                            bool_data_in[y_reduce][x_reduce] = connect_area_num;
                            connect_area_weightT += Obst_map[y_reduce][x_reduce];
                        }
                        if (bool_data_in[y_reduce][xg] == 1)
                        {
                            bmca_area_grow_data_arey[area_grow_data_arel] = y_reduce;
                            bmca_area_grow_data_arex[area_grow_data_arel++] = xg; // 当前次生长的一圈的纪录
                            bool_data_in[y_reduce][xg] = connect_area_num;
                            connect_area_weightT += Obst_map[y_reduce][xg];
                        }
                        if (bool_data_in[y_reduce][x_add] == 1)
                        {
                            bmca_area_grow_data_arey[area_grow_data_arel] = y_reduce;
                            bmca_area_grow_data_arex[area_grow_data_arel++] = x_add; // 当前次生长的一圈的纪录
                            bool_data_in[y_reduce][x_add] =
                                connect_area_num; ////////////////////////////////////////////改区域生长 加参数
                            connect_area_weightT += Obst_map[y_reduce][x_add];
                        }
                        j++;
                    }
                    connect_area_layer_e = area_grow_data_arel;
                    connect_area_layer_l = connect_area_layer_e - connect_area_layer_s; // 连通域重量+当前一圈的点数
                    connect_area_layer_ps = connect_area_layer_s;                       // 存储连通域 上层的 首
                    connect_area_layer_pe = connect_area_layer_e;                       // 存储连通域 上层的 尾

                    connect_area_w += connect_area_layer_l;
                    if (connect_area_layer_l == 0)
                        break;

                    /*测试代码
                    for (test_y=0;test_y<480;test_y++)	for (test_x=0;test_x<640;test_x++)
                    if (bool_data_in[test_y][test_x]>1)	OpenCv_DrawPoint(test_x,test_y,255,255,255,test);
                    else								OpenCv_DrawPoint(test_x,test_y,0,0,0,test);
                    strShow.Format("%d %d",area_grow_data_arel,connect_area_num);
                    cvPutText(test, strShow , cvPoint(0, 40), &out_txt, CV_RGB(255,255,255));
                    cvShowImage("2",test);
                    cvWaitKey(5);

                    if (area_grow_data_arel>=640*480)
                    area_grow_data_arel=area_grow_data_arel;
                    */
                }
                // 生长完成
                // ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●
                connect_area_s_e_w[connect_area_num][1] =
                    area_grow_data_arel; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                         // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
                connect_area_s_e_w[connect_area_num][2] =
                    connect_area_w; // 存储1000个连通域的 首 尾 标号 [首]connect_area_s_e[n][0]
                                    // [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]
                connect_area_weight[connect_area_num] = connect_area_weightT;
                if (connect_max_area_w < connect_area_w)
                {
                    connect_max_area_w = connect_area_w;
                    connect_max_area_num = connect_area_num;
                }
            }

            x++;
        }
        y++;
    }
    // return;//
    //	for (i=0;i<area_grow_data_arel;i++)//测试代码
    //	{
    //		data[(bmca_area_grow_data_arey[i]*320+bmca_area_grow_data_arex[i])*3+0]=1;//测试代码//显示种子点
    //		data[(bmca_area_grow_data_arey[i]*320+bmca_area_grow_data_arex[i])*3+1]=254;//测试代码
    //		data[(bmca_area_grow_data_arey[i]*320+bmca_area_grow_data_arex[i])*3+2]=1;//测试代码
    //		m_VideoOCX.Show(m_Image);//测试代码				// display capture frame
    //		Sleep(200);
    //	}//测试代码

    ////////////////////////////////////////////////////////////////////////////////////
    //*obj_weight_map=connect_area_s_e_w[connect_max_area_num][2];//存储1000个连通域的 首 尾 标号
    //[首]connect_area_s_e[n][0] [尾]connect_area_s_e[n][1] [重量]connect_area_s_e[n][2]

    // 带出黑白图象重心横坐标  obj_properties *All  All.x结果的重心的x坐标 All.y结果的重心的y坐标
    // 需要处理部分的掩膜数组  int *yan_mo_x,int *yan_mo_y
    // 需要处理部分的掩膜数组长度int yan_mo_long,
    for (i = 0; i <= connect_area_num; i++)
    {
        connect_max_area_order[i] = i;
        // connect_area_s_e_w[i][2]=i+6;//test code
    }
    unsigned int temp_order;
    for (i = 0; i < connect_area_num - 2; i++)
    {
        for (j = connect_area_num; j > 2 + i; j--)
        {
            //			if
            //(connect_area_s_e_w[connect_max_area_order[j]][2]>connect_area_s_e_w[connect_max_area_order[j-1]][2])
            if (connect_area_weight[connect_max_area_order[j]] > connect_area_weight[connect_max_area_order[j - 1]])
            {
                temp_order = connect_max_area_order[j - 1];
                connect_max_area_order[j - 1] = connect_max_area_order[j];
                connect_max_area_order[j] = temp_order;
            }
        }
    }

    register float OBJ_temp_b = 0;
    register float OBJ_temp_b_ = 0;
    register float OBJ_temp_c = 0;
    register float ball_op_x = 0;
    register float ball_op_y = 0;
    register float ball_op_weight = 0;
    register int wg;
    for (y = 0; y < CS_OMNI_VISION_CAM_HEIGHT; y++)
    {
        for (x = 0; x < CS_OMNI_VISION_CAM_WIDTH; x++)
        {
            bool_data_out[y][x] = 0;
        }
    }
    if (connect_area_num >= 2) // 最大连通域的编号	默认无效值1
    {
        for (j = 2; j <= connect_area_num; j++)
        {
            OBJ_temp_b = 0;
            OBJ_temp_b_ = 0;
            OBJ_temp_c = 0;
            for (i = connect_area_s_e_w[connect_max_area_order[j]][0];
                 i < connect_area_s_e_w[connect_max_area_order[j]][1]; i++)
            { //	SetColor(bmca_area_grow_data_arey[i],bmca_area_grow_data_arex[i],cPointB);
                xg = bmca_area_grow_data_arex[i];
                yg = bmca_area_grow_data_arey[i];
                wg = Obst_map[yg][xg];
                OBJ_temp_c += wg;
                OBJ_temp_b += xg * wg;
                OBJ_temp_b_ += yg * wg;

                //	data[(bmca_area_grow_data_arey[i]*320+bmca_area_grow_data_arex[i])*3+0]=254;//测试代码//显示种子点
                //	data[(bmca_area_grow_data_arey[i]*320+bmca_area_grow_data_arex[i])*3+1]=254;//测试代码
                //	data[(bmca_area_grow_data_arey[i]*320+bmca_area_grow_data_arex[i])*3+2]=254;//测试代码
                //	SetColor(y,x,cPointG);//测试代码
                bool_data_out[bmca_area_grow_data_arey[i]][bmca_area_grow_data_arex[i]] = j - 1;
            }
            if (OBJ_temp_c != 0) // 保险程序  当同步不佳时  有时Capture不到图象 就会出现所有象素都为零 这时做OBJ_temp_b
                                 // / OBJ_temp_c的除法就会产生分母为零的错误
            {
                Obst_connect_area_x[j - 1] = OBJ_temp_b / OBJ_temp_c;
                Obst_connect_area_y[j - 1] = OBJ_temp_b_ / OBJ_temp_c;
            }
            else
            {
                Obst_connect_area_x[j - 1] = 0;
                Obst_connect_area_y[j - 1] = 0;
            }
            Obst_connect_area_w[j - 1] = connect_area_s_e_w[connect_max_area_order[j]][2];
            Obst_connect_area_Ww[j - 1] = connect_area_weight[connect_max_area_order[j]];
        }
        Obst_connect_area_num = connect_area_num - 1;
    }

    return;
    max_connectivity_weight = long(OBJ_temp_c);
    // m_VideoOCX.Show(m_Image);//测试代码				// display capture frame

    if (OBJ_temp_c != 0) // 保险程序  当同步不佳时  有时Capture不到图象 就会出现所有象素都为零 这时做OBJ_temp_b /
                         // OBJ_temp_c的除法就会产生分母为零的错误
    {
        ball_op_x = OBJ_temp_b / OBJ_temp_c;  //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+320
        ball_op_y = OBJ_temp_b_ / OBJ_temp_c; //(int)(OBJ_temp_b / OBJ_temp_c);  //除以1+2+3+...+240
    }
    ball_op_weight = OBJ_temp_c; // 准备把象素重量传给主逻辑函数
    //					HSI11->ball_x=ball_op_x;
    //					HSI11->ball_y=ball_op_y;
    //					HSI11->ball_zhong=ball_op_weight;

    // 完成二值化图像标重心程序
}

bool CObstFusion::segmentsIntr(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2, int &x, int &y)
{
    // 三角形abc 面积的2倍
    double area_abc = (ax1 - bx1) * (ay2 - by1) - (ay1 - by1) * (ax2 - bx1);
    // 三角形abd 面积的2倍
    double area_abd = (ax1 - bx2) * (ay2 - by2) - (ay1 - by2) * (ax2 - bx2);

    // 面积符号相同则两点在线段同侧,不相交 (对点在线段上的情况,本例当作不相交处理);
    if (area_abc * area_abd >= 0)
    {
        return false;
    }

    // 三角形cda 面积的2倍
    double area_cda = (bx1 - ax1) * (by2 - ay1) - (by1 - ay1) * (bx2 - ax1);
    // 三角形cdb 面积的2倍
    double area_cdb = area_cda + area_abc - area_abd;
    if (area_cda * area_cdb >= 0)
    {
        return false;
    }

    // 计算交点坐标
    double t = area_cda / (area_abd - area_abc);
    double dx = t * (ax2 - ax1);
    double dy = t * (ay2 - ay1);
    x = int(ax1 + dx);
    y = int(ay1 + dy);
    return true;
}

// 障碍物数据融合
void CObstFusion::Fusion(stDataSummary *inDataSummary)
{
    // 队员传来的障碍物数据读取自(inDataSummary->agent[i].obstacle[j])
    // 融合后的障碍物数据请写入(inDataSummary->t_enemy[i]) 数量inDataSummary->NumOfemy

    int i, j;

    memset(inDataSummary->pre_obstacle, 0, sizeof(obstacle) * FULL_OST_NUM);
    for (i = 0; i < AG_NUM; i++)
        memcpy(inDataSummary->pre_obstacle + i * 10, inDataSummary->agent[i].obstacle,
               EACH_AG_OBST_NUM * sizeof(obstacle));

    // 连通域用点数组大小清零
    Obst_point_l = 0;
    // 敌方数组大小清零
    inDataSummary->NumOfemy = 0;
    memset(inDataSummary->t_enemy, 0, sizeof(enemy) * 20);
    int nNumOfObst = FULL_OST_NUM;

    // 重新载入场地图
    memcpy(imgt->imageData, imgt_norm->imageData, imgt_norm->nChannels * imgt_norm->width * imgt_norm->height);
    memcpy(imgCS->imageData, imgCS_norm->imageData, imgCS_norm->nChannels * imgCS_norm->width * imgCS_norm->height);

    // 清空连通域
    memset(Obst_connect_area_w, 0, sizeof(int) * 1000);
    // 交线判定清零
    for (i = 0; i < 60; i++)
        inDataSummary->pre_obstacle[i].sval = false;

    // 将故障球员返回的障碍物去除（因为他可能在场边休息）
    for (i = 0; i < FULL_OST_NUM; i++)
    {
        if (inDataSummary->pre_obstacle[i].valid == true &&
            inDataSummary->agent[inDataSummary->pre_obstacle[i].ID - 1].status == ROBST_ERR)
        {
            inDataSummary->pre_obstacle[i].valid = false;
        }
    }

    // 剔除场边和小禁区障碍物
    m_Obst_RemoveOutOfField(inDataSummary->pre_obstacle);

    // 计算每个障碍物到它的提供者的距离
    for (i = 0; i < FULL_OST_NUM; i++)
    {
        if (inDataSummary->pre_obstacle[i].valid == true)
            inDataSummary->pre_obstacle[i].DistToOwner =
                m_CalDist(inDataSummary->pre_obstacle[i].x, inDataSummary->pre_obstacle[i].y,
                          inDataSummary->agent[inDataSummary->pre_obstacle[i].ID - 1].x,
                          inDataSummary->agent[inDataSummary->pre_obstacle[i].ID - 1].y);
    }

    // 将距离观察者很远的障碍物剔除
    for (i = 0; i < FULL_OST_NUM; i++)
    {
        if (inDataSummary->pre_obstacle[i].valid == true && inDataSummary->pre_obstacle[i].DistToOwner > 1000)
            inDataSummary->pre_obstacle[i].valid = false;
    }

    // 去除定位不准和可能为本方队员的障碍物
    int DistToNearAg;
    for (i = 0; i < FULL_OST_NUM; i++)
    {
        if (inDataSummary->pre_obstacle[i].valid == true)
        {
            for (j = 0; j < AG_NUM; j++)
            {
                if (inDataSummary->agent[j].online == true && inDataSummary->agent[j].status != ROBST_ERR &&
                    (inDataSummary->pre_obstacle[i].ID - 1) != (UINT)j)
                {
                    // 计算障碍物与j号机器的距离
                    DistToNearAg = m_CalDist(inDataSummary->pre_obstacle[i].x, inDataSummary->pre_obstacle[i].y,
                                             inDataSummary->agent[j].x, inDataSummary->agent[j].y);

                    // 如果障碍物与观察者距离小于200，且与非观察者距离小于30，则认为是己方队员，剔除。
                    // 剩余的为障碍物。
                    if (DistToNearAg < 80 && inDataSummary->pre_obstacle[i].DistToOwner < 200)
                    {
                        inDataSummary->pre_obstacle[i].valid = false;
                        break;
                    }

                    // 如果障碍物与观察者距离大于200，小于500，且与非观察者距离小于200,
                    // 剔除,与某队员较近的障碍物只以它的数据为准，不参与交线融合。
                    if (DistToNearAg < 200 && inDataSummary->pre_obstacle[i].DistToOwner >= 200 &&
                        inDataSummary->pre_obstacle[i].DistToOwner < 500)
                    {
                        inDataSummary->pre_obstacle[i].valid = false;
                        break;
                    }

                    // 如果障碍物与观察者距离大于500，且与非观察者距离小于300,
                    // 剔除,与某队员较近的障碍物只以它的数据为准，不参与交线融合。
                    if (DistToNearAg < 300 && inDataSummary->pre_obstacle[i].DistToOwner >= 500)
                    {
                        inDataSummary->pre_obstacle[i].valid = false;
                        break;
                    }
                }
            }
        }
    }

    // 将障碍物信息复制出来用于判断交线以及进行融合
    memset(m_fusObst, 0, sizeof(obstacle) * FULL_OST_NUM);
    memcpy(m_fusObst, inDataSummary->pre_obstacle, (FULL_OST_NUM) * sizeof(obstacle));

    for (i = 0; i < FULL_OST_NUM; i++)
    {
        // 如果障碍物距离观察者小于200，则直接参与交线
        if (m_fusObst[i].valid == true && m_fusObst[i].DistToOwner < 200)
        {
            m_fusObst[i].valid = false;
            Obst_point_x[Obst_point_l] = int(m_fusObst[i].x * MAP_SCALE + caomao_R);
            Obst_point_y[Obst_point_l] = int(m_fusObst[i].y * MAP_SCALE + caomao_R);
            Obst_point_w[Obst_point_l] = 2.5;
            Obst_point_l++;
        }
    }

    // 画障碍物线段，并记录线段位置
    int ox, oy, mx, my, dx, dy, distance;
    double rad;
    for (i = 0; i < 50; i++)
    {
        if (m_fusObst[i].valid == true)
        {
            mx = inDataSummary->agent[m_fusObst[i].ID - 1].x;
            my = inDataSummary->agent[m_fusObst[i].ID - 1].y;
            ox = m_fusObst[i].x;
            oy = m_fusObst[i].y;
            dx = ox - mx;
            dy = oy - my;
            distance = m_fusObst[i].DistToOwner;

            rad = atan2(dy, dx);
            m_fusObst[i].x1 = int(mx + cos(rad) * (distance + 50));
            m_fusObst[i].y1 = int(my + sin(rad) * (distance + 50));
            m_fusObst[i].x2 = int(mx + cos(rad) * (distance - 50));
            m_fusObst[i].y2 = int(my + sin(rad) * (distance - 50));
            cvLine(imgt, cvPoint(int(m_fusObst[i].x1 * MAP_SCALE), int(m_fusObst[i].y1 * MAP_SCALE)),
                   cvPoint(int(m_fusObst[i].x2 * MAP_SCALE), int(m_fusObst[i].y2 * MAP_SCALE)),
                   cvScalar((m_fusObst[i].ID - 1) * 50, 0, 50 * (6 - m_fusObst[i].ID - 1), 0), 2, 8, 0);
            int nub;    // 之前障碍物线段
            int nx, ny; // 交点坐标
            //	bool disable;
            for (int j = i / 10 - 1; j >= 0; j--)
                for (int w = 0; w < 10; w++)
                {
                    nub = j * 10 + w;
                    if (segmentsIntr(m_fusObst[i].x1, m_fusObst[i].y1, m_fusObst[i].x2, m_fusObst[i].y2,
                                     m_fusObst[nub].x1, m_fusObst[nub].y1, m_fusObst[nub].x2, m_fusObst[nub].y2, nx,
                                     ny) == true)
                    {
                        cvCircle(imgt, cvPoint(int(nx * MAP_SCALE), int(ny * MAP_SCALE)), 4, cvScalar(255, 0, 0, 0), -1,
                                 8, 0);

                        Obst_point_x[Obst_point_l] = int(nx * MAP_SCALE + caomao_R);
                        Obst_point_y[Obst_point_l] = int(ny * MAP_SCALE + caomao_R);
                        Obst_point_w[Obst_point_l] = 2;
                        Obst_point_l++;

                        m_fusObst[i].sval = true;
                    }
                }
        }
    }

    // 只有一个机体看到的障碍物可以直接使用
    for (i = 0; i < 50; i++)
        if (m_fusObst[i].valid == true && m_fusObst[i].sval == false)
        {
            bool disable = true;
            for (int k = 0; k < AG_NUM; k++)
            {
                if (m_CalDist(m_fusObst[i].x, m_fusObst[i].y, inDataSummary->agent[k].x, inDataSummary->agent[k].y) <
                    50)
                {
                    disable = false;
                    break;
                }
            }
            if (disable == true)
            {
                Obst_point_x[Obst_point_l] = int(m_fusObst[i].x * MAP_SCALE + caomao_R);
                Obst_point_y[Obst_point_l] = int(m_fusObst[i].y * MAP_SCALE + caomao_R);
                Obst_point_w[Obst_point_l] = 1;
                Obst_point_l++;
            }
        }

    if (mask_init_flag == 0)
    {
        circle_maskGA(caomao_R, &omask_x[0], &omask_y[0], &omask_w[0], &omask_l); // 2~80 直径障碍物掩模
        mask_init_flag = 1;
    }

    int x = 0;
    int y = 0;
    for (x = 0; x < Obst_map_W; x++)
        for (y = 0; y < Obst_map_H; y++)
        {
            Obst_map[y][x] = 0;
            Obst_map_B[y][x] = 0;
        }

    int temp_x, temp_y;
    double temp_w;
    for (int k = 0; k < Obst_point_l; k++)
    {
        temp_x = Obst_point_x[k];
        temp_y = Obst_point_y[k];
        temp_w = Obst_point_w[k];
        if ((temp_x > caomao_R) && (temp_x < Obst_map_W - caomao_R) && (temp_y > caomao_R) &&
            (temp_y < Obst_map_H - caomao_R)) // 需要溢出保护 加边
            for (i = 0; i < omask_l; i++)
            {
                //	if((omask_y[i]==0)&&(omask_x[i]==0))
                //		temp_x=temp_x;
                Obst_map[omask_y[i] + temp_y][omask_x[i] + temp_x] += int(omask_w[i] * temp_w);
            }
    }

    for (x = 0; x < Obst_map_W; x++)
        for (y = 0; y < Obst_map_H; y++)
        {
            if (Obst_map[y][x] > caomao_h) // 79)//
            {
                bool_data_in[y][x] = 1;
                //	Obst_map_B[y][x]=255;
            }
        }
    //	bool_max_connectivity_analyze2_n();
    bool_max_connectivity_analyze2_n_();

    unsigned char *imgCS_data = (unsigned char *)imgCS->imageData;

    for (x = 0; x < Obst_map_W; x++)
        for (y = 0; y < Obst_map_H; y++)
            if (bool_data_out[y][x] != 0)
            {
                //	imgCS_data[(y*imgCS->width+x)*3+0]=Obst_map_B[y][x];
                //	imgCS_data[(y*imgCS->width+x)*3+1]=Obst_map_B[y][x];
                //	imgCS_data[(y*imgCS->width+x)*3+2]=Obst_map_B[y][x];
                imgCS_data[(y * imgCS->width + x) * 3 + 0] = bool_data_out[y][x] * 111;
                imgCS_data[(y * imgCS->width + x) * 3 + 1] = bool_data_out[y][x] * 222;
                imgCS_data[(y * imgCS->width + x) * 3 + 2] = bool_data_out[y][x] * 555;
                //	imgCS_data[(y*imgCS->width+x)*3+0]=Obst_map[y][x];
                //	imgCS_data[(y*imgCS->width+x)*3+1]=Obst_map[y][x];
                //	imgCS_data[(y*imgCS->width+x)*3+2]=Obst_map[y][x];
            }

    for (i = 1; i <= int(Obst_connect_area_num); i++)
    {
        if (Obst_connect_area_w[i] > caomao_b_min_area) //	if (Obst_connect_area_Ww[i]>caomao_b_min_area)
        {

            // 将融合后的坐标存入数组
            inDataSummary->t_enemy[inDataSummary->NumOfemy].x = int((Obst_connect_area_x[i] - caomao_R) / MAP_SCALE);
            inDataSummary->t_enemy[inDataSummary->NumOfemy].y = int((Obst_connect_area_y[i] - caomao_R) / MAP_SCALE);
            inDataSummary->t_enemy[inDataSummary->NumOfemy].size = 10;
            inDataSummary->NumOfemy++;
        }
    }

    if (opencv_debug == true)
    {
        // 绘制原始障碍物（黑色圆圈）
        for (i = 0; i < nNumOfObst; i++)
        {
            if (inDataSummary->pre_obstacle[i].valid == true)
            {
                cvCircle(imgCS,
                         cvPoint(int(inDataSummary->pre_obstacle[i].x * MAP_SCALE + caomao_R),
                                 int(inDataSummary->pre_obstacle[i].y * MAP_SCALE + caomao_R)),
                         20, cvScalar(0, 0, 0, 0), 1, 8, 0);
            }
        }

        // 绘制机器
        for (i = 0; i < AG_NUM; i++)
            if (inDataSummary->agent[i].online == true && inDataSummary->agent[i].status != ROBST_ERR)
            {
                cvCircle(imgCS,
                         cvPoint(int(inDataSummary->agent[i].x * MAP_SCALE + caomao_R),
                                 int(inDataSummary->agent[i].y * MAP_SCALE + caomao_R)),
                         int(200 * MAP_SCALE), cvScalar(0, 0, 255, 0), 1, 8, 0);
                cvCircle(imgCS,
                         cvPoint(int(inDataSummary->agent[i].x * MAP_SCALE + caomao_R),
                                 int(inDataSummary->agent[i].y * MAP_SCALE + caomao_R)),
                         int(30 * MAP_SCALE), cvScalar(0, 0, 255, 0), -1, 8, 0);
            }

        // 绘制生成后的障碍物
        int tmpx, tmpy;
        for (i = 0; i < inDataSummary->NumOfemy; i++)
        {
            tmpx = int(inDataSummary->t_enemy[i].x * MAP_SCALE + caomao_R);
            tmpy = int(inDataSummary->t_enemy[i].y * MAP_SCALE + caomao_R);
            cvCircle(imgCS, cvPoint(tmpx, tmpy), 4, cvScalar(255, 0, 0, 0), -1, 8, 0);
        }

        // 显示图片
        /*cvShowImage("敌我识别", imgCS);
        cvShowImage("线段测试", imgt);*/
    }
}
