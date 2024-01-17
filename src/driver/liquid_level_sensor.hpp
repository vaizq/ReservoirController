#pragma once

#include <cstdint>


namespace Driver 
{

class LiquidLevelSensor
{
public:
    LiquidLevelSensor(uint8_t pin);
    bool liquidIsPresent();
private:
    uint8_t mPin;
};

}