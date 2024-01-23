#include "tb6612fng_valve.hpp"
#include <Arduino.h>

Driver::TB6612FNGValve::TB6612FNGValve(const PinDef& pins)
: mPins{pins} 
{

    pinMode(mPins.xIN1, OUTPUT);
    pinMode(mPins.xIN2, OUTPUT);
    pinMode(mPins.PWMx, OUTPUT);

    digitalWrite(mPins.xIN1, HIGH);
    digitalWrite(mPins.xIN2, LOW);

    close();
}

void Driver::TB6612FNGValve::open()
{
    digitalWrite(mPins.PWMx, HIGH);
    mIsRunning = true;
}

void Driver::TB6612FNGValve::close()
{
    digitalWrite(mPins.PWMx, LOW);
    mIsRunning = false;
}

bool Driver::TB6612FNGValve::isOpen() const 
{ 
    return mIsRunning; 
}
