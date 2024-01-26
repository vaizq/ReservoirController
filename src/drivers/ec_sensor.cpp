#include "ec_sensor.hpp"
#include "read_voltage.hpp"
#include <Arduino.h>
#include <numeric>


Driver::ECSensor::ECSensor(uint8_t pin)
: mVoltageReader{pin, 10} 
{}

float Driver::ECSensor::readEC()
{
    const float k = mCalibrationPoint.second / mCalibrationPoint.first;
    return k * mVoltageReader.voltage();
}

bool Driver::ECSensor::calibrate(float ec)
{
    const float voltage = mVoltageReader.voltage();
    mCalibrationPoint.first = voltage;
    mCalibrationPoint.second = ec;
    return true;
}
