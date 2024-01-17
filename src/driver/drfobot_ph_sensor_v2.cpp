#include "dfrobot_ph_sensor_v2.hpp"
#include "read_voltage.hpp"
#include <Arduino.h>


Driver::DFRobotV2PHSensor::DFRobotV2PHSensor(uint8_t pin)
: mPin{pin}
{
    pinMode(mPin, INPUT);
}


float Driver::DFRobotV2PHSensor::readPH()
{
    const float voltage = readVoltage(mPin);
    const float k = (7.0f - 4.0f) / (mVoltageAtPh7 - mVoltageAtPh4);
    const float b = 7.0f - k * mVoltageAtPh7;
    return k * voltage + b; // y = kx + b
}

bool Driver::DFRobotV2PHSensor::calibrate()
{
    const float voltage = readVoltage(mPin);
    if (std::abs(voltage - mVoltageAtPh7) < std::abs(voltage - mVoltageAtPh4))
    {
        mVoltageAtPh7 = voltage;
    }
    else
    {
        mVoltageAtPh4 = voltage;
    }

    return true;
}