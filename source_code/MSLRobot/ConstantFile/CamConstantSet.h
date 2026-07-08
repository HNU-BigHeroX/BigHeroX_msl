// CamConstantSet.h
// 注意：当运动跟踪目标切换时 原来的积分量速度记录 等还在保存 会引起初运动不正常
// #include "stdafx.h"

#ifndef CamConstantSet_h
#define CamConstantSet_h

// 这里设置一些 程序常量	21:39 2008-11-3
const int OMNI_VISION_CAM_WIDTH = 640;                         // 320;//全景分辨率x
const int OMNI_VISION_CAM_HEIGHT = 480;                        // 240;//全景分辨率y
const int MIRROR_CENTER_X = OMNI_VISION_CAM_WIDTH / 2;         // 镜子中心x
const int MIRROR_CENTER_Y = OMNI_VISION_CAM_HEIGHT / 2;        // 镜子中心y
const int MIRROR_RADIUS = OMNI_VISION_CAM_HEIGHT / 2;          // 镜子半径r
const int MIRROR_LIMIT_L = MIRROR_CENTER_X - MIRROR_RADIUS;    // 镜子左边界	自动计算出的常量
const int MIRROR_LIMIT_R = MIRROR_CENTER_X + MIRROR_RADIUS;    // 镜子左边界	自动计算出的常量
const int MIRROR_LIMIT_UP = MIRROR_CENTER_Y + MIRROR_RADIUS;   // 镜子上边界	自动计算出的常量
const int MIRROR_LIMIT_DOWN = MIRROR_CENTER_Y - MIRROR_RADIUS; // 镜子下边界	自动计算出的常量

const int circle_mask_radius_min = 0;   // 20//0 //设置全景掩膜 半径最小长度		//影响包括 HSV转换 二值化 连通域
const int circle_mask_radius_max = 240; // 176;//90  //设置全景掩膜 半径最大长度

#endif CamConstantSet_h