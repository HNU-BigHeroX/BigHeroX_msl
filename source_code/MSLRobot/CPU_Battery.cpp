// CPU_Battery.cpp: implementation of the CCPU_Battery class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CPU_Battery.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCPU_Battery::CCPU_Battery()
{

    nCpuUsage = 0;
    fCpuFrequency = 0.0f;
    memset(&lpPwrStatus, 0, sizeof(SYSTEM_POWER_STATUS));

    bExit = false;
    nSleepTime = 1000;
}

CCPU_Battery::~CCPU_Battery()
{
}

void CCPU_Battery::GetBattreyStatus()
{
    // Call Api!
    GetSystemPowerStatus(&lpPwrStatus);

    /***************************
    ACLineStatus
    交流电源状态
    0
    Offline
    1
    Online
    255
    Unknown status

    BatteryFlag
    电池充电状态。 可以包含一或多个以下指

    1
    高，电量大于66%

    2
    低，小于33%

    4
    极低，小于5%

    8
    充电中

    128
    没有电池

    255
    位置，无法读取状态。

    BatteryLifePercent
    电池还有百分之几能充满.0~100，若未知则为255
    Reserved1
    保留，必须为0

    BatteryLifeTime
    秒为单位的电池剩余电量, 若未知则为-1

    BatteryFullLifeTime
    秒为单位的电池充满电的电量，若未知则为-1
    *****************************/
}

LARGE_INTEGER liOldIdleTime = {0, 0};
LARGE_INTEGER liOldSystemTime = {0, 0};

#define SystemBasicInformation 0
#define SystemPerformanceInformation 2
#define SystemTimeInformation 3

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

typedef struct
{
    DWORD dwUnknown1;
    ULONG uKeMaximumIncrement;
    ULONG uPageSize;
    ULONG uMmNumberOfPhysicalPages;
    ULONG uMmLowestPhysicalPage;
    ULONG uMmHighestPhysicalPage;
    ULONG uAllocationGranularity;
    PVOID pLowestUserAddress;
    PVOID pMmHighestUserAddress;
    ULONG uKeActiveProcessors;
    BYTE bKeNumberProcessors;
    BYTE bUnknown2;
    WORD wUnknown3;
} SYSTEM_BASIC_INFORMATION;

// typedef struct
// {
// 	LARGE_INTEGER liIdleTime;
// 	DWORD dwSpare[76];
// } SYSTEM_PERFORMANCE_INFORMATION;
typedef struct SYSTEM_PERFORMANCE_INFORMATION
{
    LARGE_INTEGER liIdleTime;
    LARGE_INTEGER ReadTransferCount;
    LARGE_INTEGER WriteTransferCount;
    LARGE_INTEGER OtherTransferCount;
    ULONG ReadOperationCount;
    ULONG WriteOperationCount;
    ULONG OtherOperationCount;
    ULONG AvailablePages;
    ULONG TotalCommittedPages;
    ULONG TotalCommitLimit;
    ULONG PeakCommitment;
    ULONG PageFaults;   // total soft or hard Page Faults since boot (wraps at 32-bits)
    ULONG Reserved[74]; // unknown
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
    LARGE_INTEGER liKeBootTime;
    LARGE_INTEGER liKeSystemTime;
    LARGE_INTEGER liExpTimeZoneBias;
    ULONG uCurrentTimeZoneId;
    DWORD dwReserved;
} SYSTEM_TIME_INFORMATION;

typedef LONG(WINAPI *PROCNTQSI)(UINT, PVOID, ULONG, PULONG);

PROCNTQSI NtQuerySystemInformation;

int CCPU_Battery::GetCpuUsage()
{
    SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
    SYSTEM_TIME_INFORMATION SysTimeInfo;
    SYSTEM_BASIC_INFORMATION SysBaseInfo;
    double dbIdleTime = 0;
    double dbSystemTime = 0;
    LONG status;

    NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle("ntdll"), "NtQuerySystemInformation");

    if (!NtQuerySystemInformation)
        return -1;

    // get number of processors in the system
    status = NtQuerySystemInformation(SystemBasicInformation, &SysBaseInfo, sizeof(SysBaseInfo), NULL);
    if (status != NO_ERROR)
        return -1;

    // get new system time
    status = NtQuerySystemInformation(SystemTimeInformation, &SysTimeInfo, sizeof(SysTimeInfo), 0);
    if (status != NO_ERROR)
        return -1;

    // get new CPU's idle time
    status = NtQuerySystemInformation(SystemPerformanceInformation, &SysPerfInfo, sizeof(SysPerfInfo), NULL);
    if (status != NO_ERROR)
        return -1;

    // if it's a first call - skip it
    if (liOldIdleTime.QuadPart != 0)
    {
        // CurrentValue = NewValue - OldValue
        dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(liOldIdleTime);
        dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(liOldSystemTime);

        // CurrentCpuIdle = IdleTime / SystemTime
        dbIdleTime = dbIdleTime / dbSystemTime;

        // CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
        dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors + 0.5;
    }

    // store new CPU's idle and system time
    liOldIdleTime = SysPerfInfo.liIdleTime;
    liOldSystemTime = SysTimeInfo.liKeSystemTime;

    if (dbIdleTime < 0)
    {
        dbIdleTime = 0;
    }

    nCpuUsage = (int)dbIdleTime;
    return nCpuUsage;

    /****************************************
    微软建议在获取SystemPerformanceInformation时用CryptGenRandom 代替NtQuerySystemInformation。
    另，获取CPU数目可用GetSystemInfo代替，
    GetSystemTimes获取系统时间不知道是否适用？
    *****************************************/
}

// RDTSC-Read Time-Stamp Counter
// 自开机以来CPU经历的时钟周期数
unsigned __int64 RDTSC()
{
    __asm _emit 0x0F;
    __asm _emit 0x31;
}

// CPU的频率
double CCPU_Battery::CpuFrequency()
{
    // On a multiprocessor machine, it should not matter which processor is called.
    // However, you can get different results on different processors due to bugs in
    // the BIOS or the HAL. To specify processor affinity for a thread, use the SetThreadAffinityMask function.
    HANDLE hThread = GetCurrentThread();
    SetThreadAffinityMask(hThread, 0x1);

    // 主板上高精度定时器的晶振频率
    // 这个定时器应该就是一片8253或者8254
    // 在intel ich7中集成了8254
    LARGE_INTEGER lFrequency;
    QueryPerformanceFrequency(&lFrequency);
    // printf("高精度定时器的晶振频率：%1.0fHz.\n",(double)lFrequency.QuadPart);

    // 这个定时器每经过一个时钟周期，其计数器会+1
    LARGE_INTEGER lPerformanceCount_Start;
    QueryPerformanceCounter(&lPerformanceCount_Start);

    // RDTSC指令:获取CPU经历的时钟周期数
    __int64 _i64StartCpuCounter = RDTSC();

    // 延时长一点,误差会小一点
    // int nTemp=100000;
    // while (--nTemp);
    Sleep(200);

    LARGE_INTEGER lPerformanceCount_End;
    QueryPerformanceCounter(&lPerformanceCount_End);

    __int64 _i64EndCpuCounter = RDTSC();

    // f=1/T => f=计数次数/(计数次数*T)
    // 这里的“计数次数*T”就是时间差
    double fTime = ((double)lPerformanceCount_End.QuadPart - (double)lPerformanceCount_Start.QuadPart) /
                   (double)lFrequency.QuadPart;

    return (_i64EndCpuCounter - _i64StartCpuCounter) / fTime;
}

float CCPU_Battery::GetCpuFrequency()
{
    fCpuFrequency = CpuFrequency() / 1000000.0;
    return fCpuFrequency;
}

void CCPU_Battery::AutoQuery(int inTime)
{
    if (inTime < 0)
    {
        return;
    }
    nSleepTime = inTime;
    bExit = false;
    AfxBeginThread(QueryInfoThread, this);
}

UINT CCPU_Battery::QueryInfoThread(LPVOID pParam)
{
    CCPU_Battery *pComInfo = (CCPU_Battery *)pParam;

    while (pComInfo->bExit == false)
    {
        pComInfo->GetBattreyStatus();
        pComInfo->GetCpuUsage();
        pComInfo->GetCpuFrequency();
        Sleep(pComInfo->nSleepTime);
    }
    return 0L;
}

void CCPU_Battery::Stop()
{
    bExit = true;
}
