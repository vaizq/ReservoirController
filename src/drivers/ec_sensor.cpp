#include "ec_sensor.hpp"
#include "read_voltage.hpp"
#include <Arduino.h>
#include <numeric>


Driver::ECSensor::ECSensor(uint8_t pin)
: mVoltageReader{pin, 10} 
{}

float Driver::ECSensor::readEC()
{
    return mVoltageReader.voltage() / 3.3f * 2.0f;
}