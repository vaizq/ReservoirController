#pragma once

#include "../reservoir.hpp"
#include <chrono>
#include <thread>
#include <functional>


namespace Driver
{

class Valve
{
public:
    using Duration = std::chrono::duration<float>;
    using LiquidSupply = std::function<Liquid(float amount)>;

    Valve(Reservoir& reservoir, float flowRate, LiquidSupply supply = finnishTapWater)
    : mReservoir(reservoir), mFlowRate(flowRate), mLiquidSupply(supply)
    {
    }

    void open()
    {
        mIsOpen = true;
    }

    void close()
    {
        mIsOpen = false;
    }

    bool isOpen() const
    {
        return mIsOpen;
    }

    void update(Duration dt)
    {
        if (mIsOpen)
        {
            const float amount = mFlowRate * dt.count();
            mReservoir.add(mLiquidSupply(amount));
        }
    }
private:
    Reservoir& mReservoir;
    float mFlowRate;
    bool mIsOpen{false};
    LiquidSupply mLiquidSupply;
};




}