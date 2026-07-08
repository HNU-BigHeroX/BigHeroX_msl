// VoyCmd.h: interface for the CVoyCmd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOYCMD_H__8007E941_6DAD_4AA1_88D7_21A255E426F0__INCLUDED_)
#define AFX_VOYCMD_H__8007E941_6DAD_4AA1_88D7_21A255E426F0__INCLUDED_

#include "SerialPort.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_BUF 1024 // 接受缓冲长度
#define MAX_SEND 20  // 发送缓冲长度

#define ULTRASONICAMOUNT 24 // 超声数量
#define INFRAREDCHAR 3      // 3字节24位
#define INFRAREDMOUNT 24

class CVoyCmd : public CSerialPort
{
  public:
    BYTE m_CalSum(BYTE *inBuf, int length);
    void SendMotorValAgain();
    void SetLiftPos(int inLiftPos);
    void SetSixMotorsSpeed(int *inSpeed);
    void Parse(BYTE inData);
    static UINT DaemonThread(LPVOID pParam);
    void InitSensor();
    void m_GenerateSigCmd(UINT inIndex, UCHAR inID, UCHAR inLen, UCHAR inMode, UCHAR inMethod, UCHAR *data);
    void SynGetIO(UINT TimeGap);
    void SynGetAD(UINT TimeGap);
    void SynGetMotorPos(UINT TimeGap);
    void SetWatchDog(UINT inTime);
    void m_ParseBuffer(const UCHAR buf); // 对一片缓冲区数据进行指令切分
    short m_WordFromChar(BYTE *inBuf);
    int m_IntFromChar(BYTE *inBuf);
    void SetFiveMotorsSpeed(short *inSpeed);
    void VectorMove(double inAngle, int inLV, int inPSpeed); // 全向矢量移动（矢量角，线速度，角速度）
    int CalAngle(int angle1, int angle2);                    // 计算两个角度之间的夹角
    void SetFourMotorsSpeed(int No1speed, int No2speed, int No3speed, int No4speed); // 同时设置四个电机速度
    void SetServMotor(int inSpeed);
    void SetRMotorPos(int inPos, int inSpeed);
    void SetLMotorPos(int inPos, int inSpeed);
    void DemarcateForCompass();
    void Demarcate(); // 陀螺仪的标定
    void Kick(int st);
    void Kick(int inPwm, int inTime);                           // 踢球动作
    void AutoQueryCompass(UINT TimeGap);                        // 自动查询罗盘信息
    void QueryCompass();                                        // 单次查询罗盘信息
    void CircleByGyro(int inAngel, int inSpeed);                // 原地转向（基于陀螺）
    void SpeedByGyro(int inSpeed);                              // 直行（基于陀螺）
    void ShowSensor();                                          // 在传感器显示控件上显示信息
    void AutoQueryInfraRed(UINT TimeGap);                       // 自动查询红外传感器信息
    void AutoQueryUSonic(UINT TimeGap);                         // 自动查询超声传感器信息
    UINT GetState();                                            // 返回当前机器人状态
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

    UINT nState; // 机器人状态标记

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

    CStatic *USDataShow;   // 超声数值文本显示对象
    CStatic *IRDataShow;   // 红外数值文本显示对象
    CStatic *CmpsDataShow; // 陀螺仪数值显示

    int m_iLspeed; // 左电机速度
    int m_iRspeed; // 右电机速度
    // FLOAT m_fSpReduc;				//电机减速比
    float m_fLSpdCoe;
    float m_fRSpdCoe;
    float m_fSpdCoe; // 全向速度比例

    FLOAT m_XRoll; // X滚转角
    FLOAT m_YRoll; // Y滚转角

    BYTE ValServMotor[8]; // 伺服电机转角

    unsigned char Arduino_MPU6050Data[40];

    // 新协议，电机位置和电流
    int nMotorPos[5];
    short wMotorCurrent[5];
    CEdit *pMotorDis;
    CEdit *pIODis;
    CEdit *pADDis;

    // IO
    BYTE m_chIO;
    BYTE m_markIO[4];
    bool m_valIO[4];

    // AD
    WORD m_valAD[5];

    // 要发送的电机值
    int m_nMotorToSend[6];

    // 监控
    int m_nDataCount;  // 累加，一秒清零，监控下位机是否有返回
    int m_nRecvCntLog; // 累计每秒帧率
    bool bNoReply;
    bool m_bDaemonThreadisWorking;

  protected:
    void m_GenerateCmd(UCHAR inID, UCHAR inLen, UCHAR inMode, UCHAR inMethod, UCHAR *data);
    void m_Split2Bytes(unsigned char *inTarg, UINT inSrc);
    void m_Split2Bytes(unsigned char *inTarg, short inSrc);
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
    void m_ResetSendBuf();                      // 清空发送缓冲区
    void m_ResetRcvBuf();                       // 清空接收缓冲区

    UCHAR *m_charInfrared; // 红外传感器接收信息数组
};

#endif // !defined(AFX_VOYCMD_H__8007E941_6DAD_4AA1_88D7_21A255E426F0__INCLUDED_)
