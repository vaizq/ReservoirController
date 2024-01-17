#include "solenoid_valve.hpp"
#include <Arduino.h>


Driver::SolenoidValve::SolenoidValve(uint8_t pin)
: mPin{pin}
{

    pinMode(mPin, OUTPUT);

    close();
}

Driver::SolenoidValve::~SolenoidValve()
{
    close();
}

void Driver::SolenoidValve::open()
{
    digitalWrite(mPin, HIGH);
    mIsOpen = true;
}

void Driver::SolenoidValve::close()
{
    digitalWrite(mPin, LOW);
    mIsOpen = false;
}

bool Driver::SolenoidValve::isOpen() const
{
    return mIsOpen;
}