#pragma once

#include <chrono>


template <typename Clock = std::chrono::steady_clock>
class DoTimer
{
public:
    DoTimer(Clock& clock, Clock::duration interval)
    : mClock(clock), mInterval(interval)
    {} 

    bool isTime()
    {
        if (!mStarted)
        {
            mLastTime = mClock.now();
            mStarted = true;
            return false;
        }

        const auto now = mClock.now();
        if (now - mLastTime >= mInterval)
        {
            mLastTime = now;
            return true;
        }

        return false;
    }
private:
    Clock& mClock;
    Clock::duration mInterval;
    Clock::time_point mLastTime;
    bool mStarted{false};
};