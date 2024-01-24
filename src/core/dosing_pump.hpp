#pragma once

#include "common.hpp"
#include <chrono>


namespace Core
{

template <typename ValveT>
class Doser 
{
public:
    Doser(ValveT&& valve, float flowRate)
    : mValve{std::move(valve)}, mFlowRate{flowRate}
    {}

    void dose(float amount)
    {
        mAmountLeft += amount;
        if (!mValve.isOpen()) { mValve.open(); }
    }

    void reset() 
    { 
        mValve.close(); 
        mAmountLeft = 0.0f;
    }

    bool isDosing() const 
    { 
        return mAmountLeft > 0.0f; 
    }

    void update(Duration dt)
    {
        if (mAmountLeft > 0.0f)
        {
            mAmountLeft -= mFlowRate * dt.count();
        }
        else if (mValve.isOpen()) 
        { 
            mAmountLeft = 0.0f;
            mValve.close(); 
        }
    }

private:
    ValveT mValve;
    const float mFlowRate;
    float mAmountLeft{0};
};

}