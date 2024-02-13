#pragma once

#include <chrono>


template <typename Clock = std::chrono::steady_clock, typename Scalar = double>
class RelativeClock 
{
public:
    using BaseClock = Clock;
    using duration = typename Clock::duration;
    using time_point = typename Clock::time_point;

    RelativeClock(Scalar scale = 1.0f)
    : mScale(scale)
    {
        mTp = mClock.now();
        mStart = mClock.now();
    }

    void setScale(Scalar scale)
    {
        mTp = now();
        mStart = mClock.now();
        mScale = scale;
    }

    Scalar getScale() const
    {
        return mScale;
    }

    time_point now() const noexcept
    {
        const auto elapsed = mClock.now() - mStart;
        return mTp + std::chrono::duration_cast<duration>(mScale * elapsed);
    }

private:
    Clock mClock;
    Scalar mScale;
    time_point mTp;
    typename Clock::time_point mStart;
};