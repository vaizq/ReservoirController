#pragma once

#include <chrono>
#include "duration.hpp"


class DtTimer
{
    using Clock = std::chrono::steady_clock;
public:
    Duration tick()
    {
        if (!mInitialized)
        {
            mLastTick = Clock::now();
            mInitialized = true;
            return Duration(0.0f); 
        }

        const auto now = Clock::now();
        const auto dt = std::chrono::duration_cast<Duration>(now - mLastTick);
        mLastTick = now;
        return dt;
    }
private:
    Clock::time_point mLastTick;
    bool mInitialized{false};
};