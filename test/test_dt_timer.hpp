#pragma once

#include "core/dt_timer.hpp"
#include "unity.h"
#include <delay.hpp>






void test_dt_timer()
{
    DtTimer dtTimer;


    constexpr int n = 10;
    constexpr Duration delay(std::chrono::milliseconds(50));
    Duration dtMin = Duration::max();
    Duration dtMax = Duration::min();

    for (int i = 0; i < n; i++)
    {
        Cultimatic::delay(delay);
        const auto dt = dtTimer.tick();
        if (i > 0)
        {
            dtMin = std::min(dtMin, dt);
            dtMax = std::max(dtMax, dt);
        }
    }

    constexpr auto acceptedError = Duration(std::chrono::milliseconds(1)).count();

    TEST_ASSERT(std::abs((delay - dtMin).count()) < acceptedError);
    TEST_ASSERT(std::abs((delay - dtMax).count()) < acceptedError);
}
