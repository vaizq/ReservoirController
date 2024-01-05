#include "valve.hpp"


Proto::Valve::Valve(gpio_num_t pin)
    : mPin(pin)
{
    gpio_set_direction(mPin, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(mPin, GPIO_PULLDOWN_ONLY);
    close();
}

Proto::Valve::~Valve()
{
    close();
}

void Proto::Valve::open()
{
    gpio_set_level(mPin, 1);
    mIsOpen = true;
}

void Proto::Valve::close()
{
    gpio_set_level(mPin, 0);
    mIsOpen = false;
}

bool Proto::Valve::isOpen() const
{
    return mIsOpen;
}