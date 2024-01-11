#include "tb6612fng_valve.hpp"


Driver::TB6612FNGValve::TB6612FNGValve(const PinDef& pins)
: mPins(pins) 
{
    gpio_set_direction(mPins.xIN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(mPins.xIN2, GPIO_MODE_OUTPUT);
    gpio_set_direction(mPins.PWMx, GPIO_MODE_OUTPUT);

    gpio_set_pull_mode(mPins.xIN1, GPIO_PULLDOWN_ONLY);
    gpio_set_pull_mode(mPins.xIN2, GPIO_PULLDOWN_ONLY);
    gpio_set_pull_mode(mPins.PWMx, GPIO_PULLDOWN_ONLY);

    gpio_set_level(mPins.xIN1, 1);
    gpio_set_level(mPins.xIN2, 0);

    close();
}

void Driver::TB6612FNGValve::open()
{
    gpio_set_level(mPins.PWMx, 1);
    mIsRunning = true;
}

void Driver::TB6612FNGValve::close()
{
    gpio_set_level(mPins.PWMx, 0);
    mIsRunning = false;
}

bool Driver::TB6612FNGValve::isOpen() const 
{ 
    return mIsRunning; 
}
