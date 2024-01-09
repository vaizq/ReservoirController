#pragma once

#include <GLFW/glfw3.h>
#include <chrono>
#include "reservoir.hpp"
#include "pid/pid.hpp"
#include "relative_clock.hpp"
#include "dt_timer.hpp"
#include "do_timer.hpp"


using Duration = std::chrono::duration<float>;
using Clock = RelativeClock<std::chrono::steady_clock, float>;


class App
{
public:
    static App& instance();
    App(const App&) = delete;
    App& operator=(const App&) = delete;
    App(App&&) = delete;
    App& operator=(App&&) = delete;
    ~App();

    bool loop();
private:
    App();
    void update(const Duration dt);
    void render();
private:
    GLFWwindow* mWindow;
    Reservoir mReservoir{Liquid{.amount = 10.0f, .ph = 8.0f, .ec = 0.2f}};
    Pid mPid;
    float mTargetPh = 6.0f;
    Clock mClock;
    std::chrono::minutes mDosingInterval {1};
    DtTimer<decltype(mClock), Duration> mTimer;
};