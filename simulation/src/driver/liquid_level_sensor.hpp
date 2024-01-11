#pragma once


#include "../reservoir.hpp"


namespace Driver
{

class LiquidLevelSensor
{
public:
    LiquidLevelSensor(const Reservoir& reservoir, float targetLevel)
    : mReservoir(reservoir), mTargetLevel(targetLevel)
    {}

    bool liquidIsPresent()
    {
        return mReservoir.content().amount >= mTargetLevel;
    }

private:
    const Reservoir& mReservoir;
    float mTargetLevel;
};

}