// 所有结构体在此定义，会被所有role和tac 引用
#pragma once

/***********************************************************************
 消息类型
***********************************************************************/
#define MSL_INFO 1
#define MSL_CMD 2
#define MSL_OBST 3
#define MSL_TEAMMATE 4
#define MSL_KICK 5
#define MSL_INITPOS 6

/***********************************************************************
角色号
***********************************************************************/
// ctrl宏定义 11
#define CTRL_STOP 0
#define CTRL_ATTACK 1
#define CTRL_GOALKEEP 2
#define CTRL_DEFENCE 3
#define CTRL_PASS 4
#define CTRL_CATCH 5 // 接球状态
#define CTRL_PASS_MOVE 6
#define CTRL_CATCH_MOVE 7
#define CTRL_CATCH_FOCUS 8
#define CTRL_MANUAL 9 // 手动控制

#define CTRL_MOVETO 11 // 根据设置前往指定地点
#define CTRL_BLOCK 12
#define CTRL_FOCUSONBALL 13
#define CTRL_PRODEF 14
#define CTRL_SEARCHBALL 15
#define CTRL_SHIFTATK 16
#define CTRL_REMOTECTRL 17
#define CTRL_AROUNDBALL 18
#define CTRL_ATKCOVER 19
#define CTRL_BACKPOS 20
#define CTRL_LSATK 21
#define CTRL_LSATKCOVER 22

#define CTRL_ZONEDEF 30

///////////////2013年新增///////////////
#define CTRL_FOLLOW 31
#define CTRL_DEF_BALL 32
#define CTRL_DEF_GOAL 33
///////////////2013年新增///////////////

#define CTRL_TEST 40
#define CTRL_IDLE 41

// KickOff
#define CTRL_KICKOFF_PRIM_READY 50
#define CTRL_KICKOFF_SLAVE_READY 51
#define CTRL_KICKOFF_PRIM 52
#define CTRL_KICKOFF_SLAVE 53

// FreeKick
#define CTRL_FREEKICK_PRIM_READY 60
#define CTRL_FREEKICK_SLAVE_READY 61
#define CTRL_FREEKICK_PRIM 62
#define CTRL_FREEKICK_SLAVE 63

// GoalKick
#define CTRL_GOALKICK_PRIM_READY 70
#define CTRL_GOALKICK_SLAVE_READY 71
#define CTRL_GOALKICK_PRIM 72
#define CTRL_GOALKICK_SLAVE 73

// Throw in
#define CTRL_THROWIN_PRIM_READY 80
#define CTRL_THROWIN_SLAVE_READY 81
#define CTRL_THROWIN_PRIM 82
#define CTRL_THROWIN_SLAVE 83

// Corner Kick
#define CTRL_CORNERKICK_PRIM_READY 90
#define CTRL_CORNERKICK_SLAVE_READY 91
#define CTRL_CORNERKICK_PRIM 92
#define CTRL_CORNERKICK_SLAVE 93

// Anti_KickOff

// PENALTY
#define CTRL_PENALTY_READY 100
#define CTRL_PENALTY 101

// status
#define ROBST_NEW 110
#define ROBST_ERR 111
#define ROBST_PROBE 112

// 技术挑战赛
#define TC_FINDBALL 200
// 不在线
#define AG_ST_OFFLINE 255

/***********************************************************************
策略号
***********************************************************************/
#define MATCH_OFF 0
#define MATCH_STOP 1
#define MATCH_PLAYING 2
#define MATCH_PARK_IN 3
#define MATCH_PARK_OUT 4
#define MATCH_START 5

#define MATCH_DROPPEDBALL_READY 9
#define MATCH_DROPPEDBALL_START 10

#define MATCH_KICKOFF_READY 11
#define MATCH_KICKOFF_START 12
#define MATCH_FREEKICK_READY 13
#define MATCH_FREEKICK_START 14
#define MATCH_GOALKICK_READY 15
#define MATCH_GOALKICK_START 16
#define MATCH_THROWIN_READY 17
#define MATCH_THROWIN_START 18
#define MATCH_CORNERKICK_READY 19
#define MATCH_CORNERKICK_START 20
#define MATCH_PENALTY_READY 21
#define MATCH_PENALTY_START 22

#define MATCH_COUNTER_KICKOFF_READY 31
#define MATCH_COUNTER_KICKOFF_START 32
#define MATCH_COUNTER_FREEKICK_READY 33
#define MATCH_COUNTER_FREEKICK_START 34
#define MATCH_COUNTER_GOALKICK_READY 35
#define MATCH_COUNTER_GOALKICK_START 36
#define MATCH_COUNTER_THROWIN_READY 37
#define MATCH_COUNTER_THROWIN_START 38
#define MATCH_COUNTER_CORNERKICK_READY 39
#define MATCH_COUNTER_CORNERKICK_START 40
#define MATCH_COUNTER_PENALTY_READY 41
#define MATCH_COUNTER_PENALTY_START 42

#define MATCH_TECHCHALLENGE 90

#define MATCH_TEST_PASSING 100
#define MATCH_TEST_4PASS 101
#define MATCH_TEST_ALLDEF 102
#define MATCH_TEST_MOVEAROUND 103
#define MATCH_TEST_CATCHMOVE 104

/*
#define sFetchBall,

#define sTestMultiDef,
#define sTestPressDef,
#define sTestPassCatch,
#define sTechChallenge,
#define sTestATKCover,
#define sPennaltyStart,

/////////2013新增/////////
#define sTestMode,
#define sLeaveMode,
#define sPassing,*/

/***********************************************************************
其他参数
***********************************************************************/
// 障碍物信息
#define EACH_AG_OBST_NUM 10
#define AG_NUM 5
#define FULL_OST_NUM AG_NUM *EACH_AG_OBST_NUM

#define MOVETO_DIST_DEADZONE 25  // moveto行为的调节死区
#define MOVETO_ANGLE_DEADZONE 5  // moveto行为的调节死区
#define MOVETO_DEFAULT_SPEED 100 // moveto的默认速度

#define DIST_TAC_START_PASS 100     // 开球传球队员离球距离
#define DIST_TAC_START_CATCH 250    // 开球接球队员离球距离
#define DIST_TAC_BALL_FROM_PASS 100 // 球与传球队员的距离大于这个值时，认为球已经传出去

#define DIST_TAC_COUNTER 350     // 对方开球时，我方队员距离球的最小距离
#define DIST_TAC_DROPPEDBALL 140 // 争球时，我方队员距离球的最小距离

#define DIST_TAC_PENALTY 200 // 接近罚球点激活罚球动作的距离

#define DIST_TAC_DEFGOAL 200 // 防守射门的距离
#define DIST_TAC_DEFBALL 200 // 防守传球的距离

#define TIMEOUT_PASSING 3 // 传球状态保持时间

#define TIMEOUT_NOONESEENBALL 30 // 球消失标志的计数上限

// 障碍物
struct obstacle
{
    UINT ID; // 障碍物的目击者ID
    int x;   // 障碍物的x坐标值
    int y;   // 障碍物的y坐标值
    int x1;
    int x2;
    int y1;
    int y2;
    int size;        // 障碍物的体积大小（估算，范围0~255）
    int DistToOwner; // 障碍物到目击者的距离（废弃）
    bool valid;      // 该障碍物是否可信(废弃)
    bool sval;
};

struct enemy
{
    int clst_id; // 距离障碍物最近的己方队员
    int x;
    int y;
    int size;
};

struct teammate
{
    UINT ID;   // 队友ID
    int state; // 队友的角色和状态
    int x;     // 队友的x坐标
    int y;     // 队友的y坐标
    int angle; // 队友的朝向角
    int V_angle;
    int V;
    bool valid;
};

struct AgentData
{
    obstacle m_obst[10];      // 障碍物
    unsigned char other[100]; // 扩展数据
};

// 球 记录单个球信息
typedef struct uBall
{
    UINT ID;
    int x;
    int y;
    bool updated;
} uBall;

// 机器人数据结构体
typedef struct uAgent
{
    int ID;
    int x;
    int y;
    double angle;

    int V;
    int V_Angle;

    // 电脑信息
    BYTE nComputer_ACPlug;
    BYTE nComputer_BatteryFlag;
    BYTE nComputer_BatteryPercent;
    int nComputer_BatteryLife;
    BYTE nComputer_CPULoadPercent;
    float fComputer_CPUFrequency;
    float fComputer_SoftwareVer;
    BYTE nRobot_Power;
    BYTE nRobot_Charging;
    BYTE nMulticast_FPS;
    BYTE nSerialPort_FPS;
    BYTE nVision_FPS;
    BYTE nAttitude_FPS;

    // move target
    int dest_x;
    int dest_y;

    // pass target
    int passto_x;
    int passto_y;

    // status
    UINT status;    // 状态
    bool foundball; // 该ag是否看到了球
    int dist_ball;  // 该ag与球的距离
    BYTE bKick;

    // 看到的球信息
    uBall ball;

    // 看到的障碍物
    struct obstacle obstacle[EACH_AG_OBST_NUM];

    // log
    int logDefAngle;

    bool online;    // 该ag是否在线
    bool allocated; // 决策临时变量，表示ag是否已经分配角色
} uAgent;

// 角色分配
typedef struct uRole
{
    UINT ctrl;

    int allocated_ID; // 强制ID号

    int nDest_x;
    int nDest_y;
    int nDest_angle;

    int nPassTo_x;
    int nPassTo_y;

    int def_dist;
    int def_angle;

    int re_ballx;
    int re_bally;
} uRole;

// 对球筛选过程中使用的临时结构体
typedef struct suspBall
{
    int x;
    int y;
    int num;      // 附近嫌疑球个数
    int distance; // 与中心点的距离
} suspBall;

// 全局按键指令
typedef struct stKeyPress
{
    bool bShiftPressed;
    bool bCtrlPressed;
    bool bKeyAPressed;
    bool bKeySPressed;
} stKeyPress;

// 接收数据汇总
typedef struct stDataSummary
{
    uAgent *agent;           // 机器人队员数据
    obstacle *pre_obstacle;  // 预处理后的障碍物数组
    enemy *t_enemy;          // 疑似对手队员
    int NumOfemy;            // 疑似对手队员数量
    int nNoOneSeenBallCount; // 看不见球的计数
    bool bNoOneSeeBall;      // 是否侦测到球
    // 入场位置
    POINT posPlayerIn; // 入场点

    // 触球点
    POINT posTouchBall;     // 己方触球点
    bool bHoldBall;         // 己方队员是否持续触球
    bool bCtrlBallBackHalf; // 己方队员带球从对方半场回到己方
    POINT posOppoTouchBall; // 对方触球点（其实最后一次触球就很有参考意义）
    bool bOppoTouchedBall;  // 对方是否碰过球

    // 传接球
    POINT posPass;              // 开球接球跑位目标
    POINT posCatch;             // 开球传球跑位目标
    int nPassID;                // 传球队员ID
    int nCatchID;               // 接球队员ID
    UINT nPassKickTime;         // 传球队员起脚时间
    UINT nPassKickTimeCountSec; // 传球后计时
    bool bPassKick;             // 传球队员是否出脚传球
    bool bPassing;              // 当前是否处于传球阶段

    int posPasserKickTo_x;
    int posPasserKickTo_y;
    int nCatchBeforKick_x;
    int nCatchBeforKick_y;

    int nLastTacReady; // 记录上一次Ready的策略，主要用于应付按了Stop又马上按Start的错误操作
    bool bAction;      // 启动自主决策的信号

    POINT posPassToEnemy[2];  // 球传向的对方队员（疑似对方接球队员）
    int nNumOfPassToEnemy;    // 球传向的对方队员的数目
    POINT posPassToTeammate;  // 球传向的己方对方（最有可能上去截获的）
    int nNumOfPassToTeammate; // 球传向的对方队员的数目

    UINT nTimeLastStartSec;  // 最近一次start的时间
    UINT nStartTimeCountSec; // 离上一次start的时间计数

    int arObstIndexBackDef[3]; // 后防障碍物的index
    int nNumOfObstBackDef;     // 后防障碍物的数量

    double kDistance; // 各种固定距离的缩放比例（场地尺寸相关）

    stKeyPress key;    // 键盘事件
    uBall cGlobalBall; // 球坐标

    int nObstHoldBallInNoSeen_x; // 看不见球时，疑似带球目标
    int nObstHoldBallInNoSeen_y; // 看不见球时，疑似带球目标

} stDataSummary;

// 生成的策略结果
typedef struct stTactics
{
    uRole *role;
} stTactics;

// #define CUR_TAC_PLAYING_OBJ m_tac_Playing_AreaDef
#define CUR_TAC_PLAYING_OBJ m_tac_Playing
