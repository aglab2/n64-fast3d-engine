#include "perfect_timer.h"

static DWORD GetMicroseconds()
{
    static bool acquiredFreq = false;
    static DWORD freq = 0;
    if (!acquiredFreq)
    {
        acquiredFreq = true;
        LARGE_INTEGER value;
        QueryPerformanceFrequency(&value);
        freq = value.QuadPart;
    }

    LARGE_INTEGER value;
    QueryPerformanceCounter(&value);
    return value.QuadPart * 1000000 / freq;
}

PerfectTimer::PerfectTimer(DOUBLE r)
    : frames_(0)
    , lastTimeMuS_(GetMicroseconds())
    , ratioMSInFrame_(r)
{ }

void PerfectTimer::start()
{
    TIMECAPS Caps;
    timeGetDevCaps(&Caps, sizeof(Caps));
    timeBeginPeriod(Caps.wPeriodMin);

    frames_ = 0;
    lastTimeMuS_ = GetMicroseconds();
}

void PerfectTimer::stop()
{
    TIMECAPS Caps;
    timeGetDevCaps(&Caps, sizeof(Caps));
    timeEndPeriod(Caps.wPeriodMin);
}

#define SLEEP_PRECISION_MUS 3000
#define MUS_TO_MS 1000
BOOL PerfectTimer::process(DWORD* frameRate)
{
    auto currentTimeMuS = GetMicroseconds();
    frames_++;

    auto calculatedTimeMuS = lastTimeMuS_ + (DWORD) ((1000 * frames_) * ratioMSInFrame_);
    while (calculatedTimeMuS > currentTimeMuS)
    {
        auto leftToSleepMuS = calculatedTimeMuS - currentTimeMuS;
        if (leftToSleepMuS > SLEEP_PRECISION_MUS + MUS_TO_MS)
        {
            auto leftToSleepS = (leftToSleepMuS - SLEEP_PRECISION_MUS) / MUS_TO_MS;
            Sleep(leftToSleepS);
        }

        currentTimeMuS = GetMicroseconds();
    }

    if (currentTimeMuS - lastTimeMuS_ >= 1000000)
    {
        if (frameRate)
            *frameRate = frames_;
        
        frames_ = 0;
        lastTimeMuS_ = currentTimeMuS;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
