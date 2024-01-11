#pragma once

#include <driver/gpio.h>


namespace Driver 
{

class LiquidLevelSensor
{
public:
    LiquidLevelSensor(gpio_num_t pin);
    bool liquidIsPresent();
private:
    gpio_num_t mPin;
};

}