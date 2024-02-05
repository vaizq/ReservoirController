//
// Created by vaige on 5.2.2024.
//

#include "DigitalSensor.h"
#include <Arduino.h>

Driver::DigitalSensor::DigitalSensor(uint8_t pin, int highState)
: mPin{pin}, mHighState{highState}
{}

bool Driver::DigitalSensor::read() const
{
    return digitalRead(mPin) == mHighState;
}