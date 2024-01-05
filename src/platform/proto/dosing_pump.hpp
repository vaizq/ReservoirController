#pragma once

#include "core/actuators.hpp"
#include <chrono>

namespace Proto
{

template <Pump PumpT>
class DosingPump 
{
    using Clock = std::chrono::system_clock;
public:
    DosingPump(PumpT&& pump, float flowRate)
    : mPump(std::forward<PumpT>(pump)), mFlowRate(flowRate)
    {}
    void dose(float amount)
    {
        mAmountLeft += amount;
        if (!mPump.isRunning()) { mPump.start(); }
        mLastUpdate = Clock::now();
    }
    void stop() { mPump.stop(); }
    bool isDosing() const { return mAmountLeft > 0.0f; }
    void loop()
    {
        const auto now = Clock::now();
        const auto dt = std::chrono::duration_cast<std::chrono::duration<float>>(now - mLastUpdate);
        mAmountLeft -= mFlowRate * dt.count();

        if (mAmountLeft <= 0.0f) { mPump.stop(); }

        mLastUpdate = now;
    }
private:
    PumpT mPump;
    const float mFlowRate;
    float mAmountLeft{};
    Clock::time_point mLastUpdate;
};

}