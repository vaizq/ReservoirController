#pragma once

#include "core/dt_timer.hpp"
#include "unity.h"
#include <delay.hpp>






void test_dt_timer()
{
    DtTimer dtTimer;


    constexpr int n = 100;
    constexpr Duration delay(std::chrono::milliseconds(10));
    Duration total(0);
    Duration dtMin = Duration::max();
    Duration dtMax = Duration::min();

    const auto dt0 = dtTimer.tick(); 
    for (int i = 0; i < n; i++)
    {
        Cultimatic::delay(delay);
        const auto dt = dtTimer.tick();
        total += dt;
        dtMin = std::min(dtMin, dt);
        dtMax = std::max(dtMax, dt);
    }
    const auto avg = total / n;

    constexpr auto acceptedError = Duration(std::chrono::milliseconds(1)).count();

    TEST_ASSERT(std::abs((delay - dtMin).count()) < acceptedError);
    TEST_ASSERT(std::abs((delay - dtMax).count()) < acceptedError);
    TEST_ASSERT(std::abs((delay - avg).count()) < acceptedError);
}
