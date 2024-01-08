#include "unity.h"
#include "pid/pid.hpp"
#include <chrono>


constexpr std::chrono::milliseconds runtime(1000);
constexpr std::chrono::milliseconds dt(1);
constexpr float kp = 1.0f;
constexpr float kd = 1.0f;
constexpr float ki = 1.0f;


void test_pid()
{
    using Clock = std::chrono::steady_clock;

    Pid pid;
    pid.setParams(kp, kd, ki); 


    const auto start = Clock::now();
    auto now = start;

    while (now - start > runtime)
    {
        float output = pid.calculate(6.0f, 6.0f, dt);
    }
}