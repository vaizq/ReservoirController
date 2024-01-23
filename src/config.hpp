#pragma once

#include "driver/tb6612fng_valve.hpp"
#include <array>
#include <cstdint>


namespace Config 
{

using namespace Driver;

constexpr std::array<TB6612FNGValve::PinDef, 4> doserDefs = 
    {
        TB6612FNGValve::PinDef{.xIN1 = 4, .xIN2 = 5, .PWMx = 23}, // 1A
        TB6612FNGValve::PinDef{.xIN1 = 19, .xIN2 = 18, .PWMx = 26}, // 2B
        TB6612FNGValve::PinDef{.xIN1 = 13, .xIN2 = 12, .PWMx = 14}, // 1B
        TB6612FNGValve::PinDef{.xIN1 = 16, .xIN2 = 15, .PWMx = 17} // 2A
    };

constexpr uint8_t valveSwitchPin = 27;
constexpr uint8_t liquidLevelTopSensorPin = 32;
constexpr uint8_t liquidLevelBottomSensorPin = 35;
constexpr uint8_t phSensorPin = 34;
constexpr uint8_t ecSensorPin = 36;
constexpr uint8_t buttonPin = 33;
constexpr size_t PumpCount = 4;

}