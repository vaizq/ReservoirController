#pragma once

#include <GLFW/glfw3.h>
#include <chrono>
#include "reservoir.hpp"
#include "pid/pid.hpp"
#include "scaled_clock.hpp"
#include "dt_timer.hpp"

using Duration = std::chrono::duration<float>;


class App
{
public:
    App();
    ~App();
    bool loop();
private:
    void update(Duration dt);
    void render();
private:
    GLFWwindow* mWindow;
    Reservoir mReservoir{Liquid{.amount = 10.0f, .ph = 8.0f, .ec = 0.2f}};
    Pid mPid;
    float mTargetPh = 6.0f;
    Duration mDosingInterval = std::chrono::minutes(1);
    ScaledClock<float, std::chrono::steady_clock> mClock;
    DtTimer<decltype(mClock), Duration> mTimer;
};