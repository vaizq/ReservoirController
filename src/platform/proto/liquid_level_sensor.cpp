#include "liquid_level_sensor.hpp"


Proto::LiquidLevelSensor::LiquidLevelSensor(gpio_num_t pin)
    : mPin(pin)
{
    gpio_set_direction(mPin, GPIO_MODE_INPUT);
    gpio_set_pull_mode(mPin, GPIO_FLOATING);
}

bool Proto::LiquidLevelSensor::liquidIsPresent()
{
    return gpio_get_level(mPin) == 1;
}