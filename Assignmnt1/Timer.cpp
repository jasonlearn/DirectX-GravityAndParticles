#include "Timer.h"

//default ctor
Timer::Timer(): fps(0) {
    QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
    secPerCount = 1.0f / countsPerSec; //store seconds per count

    //Initial previous time
    prevTime = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&prevTime);
}

//updates the counter and calculates the time elapsed since last update.
void Timer::frameStart() {
    //get current time
    curTime = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&curTime);
    //calculate delta time
    deltaTime = (curTime - prevTime) * secPerCount;
}

//set prev to current after frame end
void Timer::frameEnd() {
    
    prevTime = curTime;
}

/*
* gets the change in time from the last frame
* return:
*   The change in time
*/
float Timer::getDeltaTime() {
    return deltaTime;
}

/*
* increments the number of frames and after 1 second returns the current FPS
* Return:
*   the current FPS count
*/
float Timer::getFPS() {
    static int frameCount;
    static float elapsedTime;

    //increment framecount
    frameCount++;
    //increment elapsed time
    elapsedTime += deltaTime;

    if (elapsedTime >= 1.0f) {
        //update fps
        fps = (float)frameCount;

        //reset elapsed time and frame count
        elapsedTime = 0;
        frameCount = 0;
    }
    return fps;
}