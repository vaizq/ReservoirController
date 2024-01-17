#pragma once

#include <cstdint>

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
    float mVoltageAtPh7 = 1500.0f;
    float mVoltageAtPh4 = 2032.44f;
};

}