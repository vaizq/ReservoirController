#pragma once

#include <cstdint>

namespace Driver 
{

class SolenoidValve 
{
public:
    SolenoidValve(uint8_t pin);
    ~SolenoidValve();

    void open();
    void close();
    bool isOpen() const;
private:
    uint8_t mPin;
    bool mIsOpen;
};

};