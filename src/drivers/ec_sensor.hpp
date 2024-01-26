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
    bool calibrate(float ec);
private:
    VoltageReader mVoltageReader;
    std::pair<float, float> mCalibrationPoint{2.4f, 2.0f};
};

}