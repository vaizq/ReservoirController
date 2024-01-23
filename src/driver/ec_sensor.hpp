#pragma once

#include <cstdint>
#include <deque>
#include "read_voltage.hpp"

namespace Driver 
{

class ECSensor
{
public:
    ECSensor(uint8_t pin);

    float readEC();

private:
    VoltageReader mVoltageReader;
};

}