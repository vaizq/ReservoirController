#ifndef TB6612FNG_MOTOR_HPP 
#define TB6612FNG_MOTOR_HPP 


#include <cstdint>


namespace Driver 
{

class TB6612FNGValve
{
public:
    struct PinDef 
    {
        uint8_t xIN1;
        uint8_t xIN2;
        uint8_t PWMx;
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