#pragma once

#include <cstdint>
#include <deque>
#include "read_voltage.hpp"

namespace Driver 
{

class DFRobotV2PHSensor
{
public:
    DFRobotV2PHSensor(uint8_t pin);

    float readPH();

    bool calibrate();

private:
    VoltageReader mVoltageReader;
    float mVoltageAtPh7 = 1.50f;
    float mVoltageAtPh4 = 2.03244f;
};

}