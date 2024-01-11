#pragma once

#include "../reservoir.hpp"


namespace Driver
{

class EcSensor
{
public:
    EcSensor(const Reservoir& reservoir)
    : mReservoir{reservoir}
    {}
    float readEC()
    {
        return mReservoir.content().ec;
    }

private:
    const Reservoir& mReservoir;
};

}