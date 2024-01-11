#pragma once

#include "../reservoir.hpp"

namespace Driver
{

class PhSensor
{
public:
    PhSensor(Reservoir& reservoir)
    : mReservoir{reservoir}
    {}

    float readPH()
    {
        return mReservoir.content().ph;
    }
private:
    Reservoir& mReservoir;
};

}