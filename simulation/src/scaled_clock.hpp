#pragma once

#include <chrono>


template <typename Scalar = double, typename Clock = std::chrono::steady_clock>
class ScaledClock
{
public:
    using duration = std::chrono::duration<Scalar, typename Clock::period>;
    using time_point = std::chrono::time_point<ScaledClock>;

    ScaledClock(Scalar scale = 1.0f)
    : mScale(scale)
    {}

    void setScale(Scalar scale)
    {
        mScale = scale;
    }

    time_point now() noexcept
    {
        return time_point(mClock.now().time_since_epoch() * mScale);
    }

private:
    Clock mClock;
    Scalar mScale;
};