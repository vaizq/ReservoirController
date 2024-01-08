#pragma once

#include <GLFW/glfw3.h>
#include <chrono>
#include "reservoir.hpp"
#include "pid/pid.hpp"

using Duration = std::chrono::duration<float>;
using Clock = std::chrono::steady_clock;

class App
{
public:
    App();
    ~App();
    bool loop(Duration dt);
private:
    void update(Duration dt);
    void render(Duration dt);
private:
    GLFWwindow* mWindow;
    Reservoir mReservoir{Liquid{.amount = 10.0f, .ph = 8.0f, .ec = 0.2f}};
    Pid mPid;
    float mTargetPh = 6.0f;
    float mTimeScale = 60.0f; // One minute means one hour
    Duration mDosingInterval = std::chrono::minutes(1);
};