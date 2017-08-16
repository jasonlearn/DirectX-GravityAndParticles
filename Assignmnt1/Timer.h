//Tutorial: https://www.youtube.com/watch?v=NKoryWqddqI
#pragma once
#ifndef CTIMER_H
#define CTIMER_H
#include <Windows.h>

class Timer
{
public:
    Timer();
    void frameStart();
    void frameEnd();
    float getFPS();
    float getDeltaTime();
    

private:
    INT64 countsPerSec;
    INT64 prevTime;
    INT64 curTime;
    float secPerCount;
    float fps;
    float deltaTime;
    bool  timerStopped;
};
#endif