#include "ec_sensor.hpp"
#include "read_voltage.hpp"
#include <Arduino.h>


Driver::ECSensor::ECSensor(uint8_t pin)
: mPin{pin}
{
    pinMode(mPin, INPUT);
}

float Driver::ECSensor::readEC()
{
    return readVoltage(mPin) / 3.3f * 2.0f;
}