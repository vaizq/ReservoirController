#ifndef TB6612FNG_MOTOR_HPP 
#define TB6612FNG_MOTOR_HPP 

#include <driver/gpio.h>



namespace Proto
{

class TB6612FNGPump
{
public:
    struct PinDef 
    {
        gpio_num_t xIN1;
        gpio_num_t xIN2;
        gpio_num_t PWMx;
    };

    TB6612FNGPump(const PinDef& pins)
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

    void open()
    {
        gpio_set_level(mPins.PWMx, 1);
        mIsRunning = true;
    }

    void close()
    {
        gpio_set_level(mPins.PWMx, 0);
        mIsRunning = false;
    }

    bool isOpen() const { return mIsRunning; }

private:
    const PinDef mPins;
    bool mIsRunning;
};

}

#endif