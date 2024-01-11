#pragma once

#include <driver/gpio.h>

namespace Driver 
{

class Valve 
{
public:
    Valve(gpio_num_t pin);
    ~Valve();

    void open();
    void close();
    bool isOpen() const;
private:
    gpio_num_t mPin;
    bool mIsOpen;
};

};