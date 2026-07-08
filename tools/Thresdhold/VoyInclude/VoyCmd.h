// VoyCmd.h: interface for the CVoyCmd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOYCMD_H__8007E941_6DAD_4AA1_88D7_21A255E426F0__INCLUDED_)
#define AFX_VOYCMD_H__8007E941_6DAD_4AA1_88D7_21A255E426F0__INCLUDED_

#include "IPhy.h"
#include "IBehavior.h"
#include "PieSensor.h"
#include "Compasee.h"
#include "NetSend.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_BUF 1024 // 接受缓冲长度
#define MAX_SEND 20  // 发送缓冲长度

#define ULTRASONICAMOUNT 24 // 超声数量
#define INFRAREDCHAR 3      // 3字节24位
#define INFRAREDMOUNT 24

// 机器人运动状态标记
#define STOP 0       // 停止状态
#define FORWARD 1    // 笔直前行
#define BACKWARD 2   // 笔直后退
#define RIGHT 3      // 原地右转
#define LEFT 4       // 原地左转
#define RIGHTFRONT 5 // 右转前进
#define LEFTFRONT 6  // 左转前进
#define RIGHTBACK 7  // 右转后退
#define LEFTBACK 8   // 左转后退

class CCaptureVideo;
class IBehavior;

class CVoyCmd
{
    friend class CSampleGrabberCB;

  public:
    void SetServMotor(int inSpeed);
    void SetRMotorPos(int inPos, int inSpeed);
    void SetLMotorPos(int inPos, int inSpeed);
    void DemarcateForCompass();
    void Demarcate();                                           // 陀螺仪的标定
    void Kick();                                                // 踢球动作
    void AutoQueryCompass(UINT TimeGap);                        // 自动查询罗盘信息
    void QueryCompass();                                        // 单次查询罗盘信息
    void CircleByGyro(int inAngel, int inSpeed);                // 原地转向（基于陀螺）
    void SpeedByGyro(int inSpeed);                              // 直行（基于陀螺）
    void ShowSensor();                                          // 在传感器显示控件上显示信息
    void AutoQueryInfraRed(UINT TimeGap);                       // 自动查询红外传感器信息
    void AutoQueryUSonic(UINT TimeGap);                         // 自动查询超声传感器信息
    UINT GetState();                                            // 返回当前机器人状态
    void SetBehavior(IBehavior *pBeh);                          // 设定机器人行为对象
    void QueryUltrasonicSensor();                               // 单次查询超声传感器
    void QueryInfrared();                                       // 单次查询红外传感器
    void SetRMotorSpeed(int inrightspeed);                      // 设置右电机速度
    void SetLMotorSpeed(int inleftspeed);                       // 设置左电机速度
    void SetBothMotorsSpeed(int inleftspeed, int inrightspeed); // 同时设置两个电机速度
    void Brake(UCHAR breakmode);                                // 刹车
    HANDLE event;

    void Parse(void *buf, int length); // 解析接收到的指令

    CVoyCmd();
    virtual ~CVoyCmd();

    IPhy *m_pPhy;      // 通讯硬件对象指针
    IBehavior *m_pBeh; // 行为对象指针
    // CCaptureVideo * m_pCap;			//视频捕捉对象指针

    BOOL bToEndThreads; // 退出所有子线程标记，退出程序前置TRUE

    UINT QueryUSonicTime;   // 超声传感器查询时间间隔
    UINT QueryInfraRedTime; // 红外传感器查询时间间隔
    UINT QueryCompassTime;  // 罗盘信息查询时间间隔

    UCHAR *m_charUltrasonic; // 超声传感器原始信息数组
    BOOL *EnableUSonic;      // 超声传感器使能数组
    DOUBLE *ValUSonic;       // 超声传感器信息
    BOOL *ValInfrared;       // 红外数据数组
    BOOL *EnableInfrared;    // 红外使能数组

    FLOAT m_angle; // 罗盘角度

    CListBox *CmdShow_Send; // 显示发送的指令
    CListBox *CmdShow_Rec;  // 显示接收到的指令
    CPieSensor *pieSensor;  // 传感器信息图
    CCompasee *compass;     // 罗盘信息图

    CStatic *USDataShow;   // 超声数值文本显示对象
    CStatic *IRDataShow;   // 红外数值文本显示对象
    CStatic *CmpsDataShow; // 陀螺仪数值显示

    int m_iLspeed; // 左电机速度
    int m_iRspeed; // 右电机速度
    // FLOAT m_fSpReduc;				//电机减速比
    float m_fLSpdCoe;
    float m_fRSpdCoe;

    FLOAT m_XRoll; // X滚转角
    FLOAT m_YRoll; // Y滚转角

    CNetSend m_netsend; // 网络发送对象

    BYTE ValServMotor[8]; // 伺服电机转角

    UINT nState; // 机器人状态标记
  protected:
    void m_UpdateState();                              // 根据电机速度值更新状态标记
    WORD m_CalculateSpeed(int speed);                  // 将长整型速度换算成两字节
    virtual void m_ParseFrame(UCHAR *buf, int length); // 将接收缓存数据进行切分

    UCHAR *m_pRcvBuf;           // 数据接收缓冲
    UCHAR *m_pSendBuf;          // 数据发送缓冲
    unsigned int m_nSendlength; // 已发送字节计数
    unsigned int m_nRcvIndex;   // 已接收字节计数

    BOOL m_bFrameStart;          // 指令解析首字符标记
    UCHAR m_cLast;               // 检验头两个字节用到的辅助变量
    unsigned int m_nFrameLength; // 当前指令长度

  private:
    DOUBLE m_CalDistance(UCHAR inUSChar); // 由超声返回数值计算距离
    UCHAR m_CalSum(int length);           // 为指令缓冲计算校验和
    void m_GenerateSendBuffer(UCHAR addr, UCHAR status, UCHAR length, UCHAR ctrlcode, UCHAR *data);
    void m_Response(UCHAR *recbuf, int length); // 受到指令后的处理函数
    void m_ResponseError();                     // 收到不符合协议的指令的处理函数
    BOOL m_ValidFrame(UCHAR *buf, int length);  // 判断一个指令是否符合协议
    void m_ParseBuffer(const UCHAR buf);        // 对一片缓冲区数据进行指令切分
    void m_ResetSendBuf();                      // 清空发送缓冲区
    void m_ResetRcvBuf();                       // 清空接收缓冲区

    UCHAR *m_charInfrared; // 红外传感器接收信息数组
};

#endif // !defined(AFX_VOYCMD_H__8007E941_6DAD_4AA1_88D7_21A255E426F0__INCLUDED_)
