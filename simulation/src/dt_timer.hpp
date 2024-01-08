#pragma once

#include <chrono>


template<typename Clock, typename Duration>
class DtTimer
{
public:
    DtTimer(Clock& clock)
    : mClock(clock)
    {}

    Duration tick()
    {
        if (!mInitialized)
        {
            mLastTick = mClock.now();
            mInitialized = true;
            return Duration(0);
        }

        const auto now = mClock.now();
        const auto dt = std::chrono::duration_cast<Duration>(now - mLastTick);
        mLastTick = now;
        return dt;
    }
private:
    Clock& mClock;
    Clock::time_point mLastTick;
    bool mInitialized{false};
};