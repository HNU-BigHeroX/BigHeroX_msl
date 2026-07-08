#if !defined(MSLPROTOCOL_H)
#define MSLPROTOCOL_H

// 消息类型
#define MSL_INFO 1
#define MSL_CMD 2
#define MSL_OBST 3
#define MSL_TEAMMATE 4
#define MSL_KICK 5
#define MSL_INITPOS 6

// ctrl宏定义 11
#define CTRL_STOP 0
#define CTRL_ATTACK 1
#define CTRL_GOALKEEP 2
#define CTRL_DEFENCE 3
#define CTRL_PASS 4
#define CTRL_CATCH 5
#define CTRL_PASS_MOVE 6
#define CTRL_CATCH_MOVE 7
#define CTRL_CATCH_FOCUS 8

#define CTRL_MOVETO 11
#define CTRL_BLOCK 12
#define CTRL_ATTENTIONBALL 13
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

// Goalkick
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
    int x;
    int y;
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

#endif