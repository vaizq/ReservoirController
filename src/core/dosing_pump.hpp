#pragma once

#include "core/actuators.hpp"
#include <chrono>


namespace Core
{

template <Valve PumpT>
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
        if (!mPump.isOpen()) { mPump.open(); }
    }
    void stop() { mPump.close(); }
    bool isDosing() const { return mAmountLeft > 0.0f; }
    void update(Duration dt)
    {
        if (mAmountLeft > 0.0f)
        {
            mAmountLeft -= mFlowRate * dt.count();
        }
        else if (mPump.isOpen()) 
        { 
            mAmountLeft = 0.0f;
            mPump.close(); 
        }
    }
private:
    PumpT mPump;
    const float mFlowRate;
    float mAmountLeft{};
};

}