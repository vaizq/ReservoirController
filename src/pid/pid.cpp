#include "pid.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>


Pid::Pid(Duration dtMin)
: mDtMin{dtMin}
{
}

Pid::Error Pid::setParams(float kp, float kd, float ki, float integralLimit, float outputLimit)
{
    Error err = Error::Success;

    auto setFactor = [&err](float& factor, float param) {
        if (std::isfinite(param))
            factor = param;
        else
            err = Error::InvalidArguments;
    };

    auto setLimit = [&err](float& limit, float param) {
        if (std::isfinite(param) && param >= 0.0f)
            limit = param;
        else
            err = Error::InvalidArguments;
    };

    setFactor(mKp, kp);
    setFactor(mKd, kd);
    setFactor(mKi, ki);
    setLimit(mIntegralLimit, integralLimit);
    setLimit(mOutputLimit, outputLimit);

    return err;
}

float Pid::calculate(float sp, float val, Duration dt)
{
    using std::chrono::duration;
    using std::chrono::duration_cast;

    dt = std::max(dt, mDtMin);
    const float error = sp - val;

    // Calculate P and D
    const float p = mKp * error;
    const float d = mKd * (error - mErrorPrevious) / dt.count();
    mErrorPrevious = error;

    // Calculate integral
    const float i = mIntegral + error * dt.count();
    mIntegral = std::abs(i) < mIntegralLimit ? i : mIntegralLimit;

    const float output = p + d + mKi * mIntegral;

    return std::abs(output) < mOutputLimit ? output : mOutputLimit;
}

void Pid::resetIntegral()
{
    mIntegral = 0.0f;
}