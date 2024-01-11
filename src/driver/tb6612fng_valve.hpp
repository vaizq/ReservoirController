#ifndef TB6612FNG_MOTOR_HPP 
#define TB6612FNG_MOTOR_HPP 


#include <driver/gpio.h>


namespace Driver 
{

class TB6612FNGValve
{
public:
    struct PinDef 
    {
        gpio_num_t xIN1;
        gpio_num_t xIN2;
        gpio_num_t PWMx;
    };

    TB6612FNGValve(const PinDef& pins);

    void open();

    void close();
    
    bool isOpen() const;

private:
    const PinDef mPins;
    bool mIsRunning;
};

}

#endif