#pragma once

#include <cstdint>

namespace Driver 
{

class ECSensor
{
public:
    ECSensor(uint8_t pin);
    float readEC();
private:
    uint8_t mPin;

};

}