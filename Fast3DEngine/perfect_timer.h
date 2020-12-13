#pragma once

#include <Windows.h>

class PerfectTimer
{
public:
    PerfectTimer(DOUBLE r);

    void start();
    void stop();
    BOOL process(DWORD* frameRate = nullptr);

private:
    DWORD frames_;
    DWORD lastTimeMuS_;
    DOUBLE ratioMSInFrame_;
};
