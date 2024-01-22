#pragma once

#include <cstdint>
#include <queue>

namespace Driver 
{

class DFRobotV2PHSensor
{
public:
    DFRobotV2PHSensor(uint8_t pin);

    float readPH();

    bool calibrate();

private:
    uint8_t mPin;
    float mVoltageAtPh7 = 1.50f;
    float mVoltageAtPh4 = 2.03244f;
};

}