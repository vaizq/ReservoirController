#pragma once

#include <chrono>


template <typename Clock = std::chrono::steady_clock, typename Scalar = double>
class RelativeClock 
{
public:
    using duration = std::chrono::duration<Scalar, typename Clock::period>;
    using time_point = std::chrono::time_point<RelativeClock>;

    RelativeClock(Scalar scale = 1.0f)
    : mScale(scale)
    {
        mTp = time_point(mClock.now().time_since_epoch());
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

    time_point now() noexcept
    {
        const auto elapsed = mClock.now() - mStart;
        return mTp + mScale * elapsed;
    }

private:
    Clock mClock;
    Scalar mScale;
    time_point mTp;
    Clock::time_point mStart;
};