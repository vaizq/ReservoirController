#include "liquid_level_sensor.hpp"
#include <Arduino.h>


Driver::LiquidLevelSensor::LiquidLevelSensor(uint8_t pin)
: mPin{pin}
{
    pinMode(mPin, INPUT);
}

bool Driver::LiquidLevelSensor::liquidIsPresent()
{
    return digitalRead(mPin) == HIGH; 
}