// CPU_Battery.h: interface for the CCPU_Battery class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPU_BATTERY_H__43EDCF83_6B71_4416_B0FA_9135B94F3E82__INCLUDED_)
#define AFX_CPU_BATTERY_H__43EDCF83_6B71_4416_B0FA_9135B94F3E82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCPU_Battery
{
  public:
    void Stop();
    static UINT QueryInfoThread(LPVOID pParam);
    void AutoQuery(int inTime);
    CCPU_Battery();
    virtual ~CCPU_Battery();

    SYSTEM_POWER_STATUS lpPwrStatus; // µç³Ø×´Ì¬
    int nCpuUsage;
    float fCpuFrequency;

    int nSleepTime;

  protected:
    float GetCpuFrequency();
    int GetCpuUsage();
    void GetBattreyStatus();
    double CpuFrequency();
    bool bExit;
};

#endif // !defined(AFX_CPU_BATTERY_H__43EDCF83_6B71_4416_B0FA_9135B94F3E82__INCLUDED_)
