// ObjPropertiesStruct.h

#include "stdafx.h"

#ifndef ObjPropertiesStruct_h
#define ObjPropertiesStruct_h

typedef struct
{
    // 对本机直角坐标系
    float x;              // 图象重心x
    float velocity_x;     // 速度x
    float acceleration_x; // 加速度x
    float y;              // 图象重心y
    float velocity_y;     // 速度y
    float acceleration_y; // 加速度y
    // 对本机极坐标系
    float angle;
    float radius; // 半径
    // 物体面积
    float weight; // 图象重量

    // 对本机坐标系运动
    //	float l_vx;//本机坐标x 速度
    //	float l_vy;//本机坐标y 速度
    //	float l_v; //本机      速度
    //	float f_va;//本机      速度方向
    //	float l_w; //本机      角速度

    // 物体物理质量
    float mass;

    // 对地坐标系
    float f_x;    // field 场地坐标x
    float f_y;    // field 场地坐标y
    float f_a;    // field 场地坐标a
    float f_vx;   // field 场地坐标x 方向上的速度
    float f_vy;   // field 场地坐标y 方向上的速度
    float f_w;    // field 场地坐标旋转量
} obj_properties; // Coord
// extern obj_properties ;

/*
class CObj_properties
{

    public:

        typedef struct
        {
            float	weight;     //图象重量
            int	x;          //图象重心x
            int	y;          //图象重心y
            int     angle;
            int	ban_jing;	//半径
            int	radius;		//半径
        }obj_properties;  //Coord



};  //Coord
*/

#endif
