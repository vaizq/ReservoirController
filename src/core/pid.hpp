#pragma once

#include <chrono>
#include <numeric>


class Pid
{
    using Duration = std::chrono::duration<float>;
    static constexpr float maxFloat = std::numeric_limits<float>::max();
public:
    enum class Error 
    {
        Success,
        Failure,
        InvalidArguments
    };
    Pid(Duration dtMin = Duration::min());
    Error setParams(float kp, float kd, float ki, float integralLimit = maxFloat, float outputLimit = maxFloat);
    float calculate(float sp, float val, Duration dt = Duration(1.0f));
    void resetIntegral();
private:
    float mSp;
    float mKp;
    float mKd;
    float mKi;
    float mErrorPrevious;
    float mIntegral;
    float mOutputPrevious;
    float mIntegralLimit;
    float mOutputLimit;
    Duration mDtMin;
};