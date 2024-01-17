#pragma once

#include "driver/tb6612fng_valve.hpp"
#include <array>
#include <cstdint>


namespace Config 
{

using namespace Driver;


constexpr TB6612FNGValve::PinDef phDownPumpDef = {.xIN1 = 4, .xIN2 = 5, .PWMx = 23}; // A

constexpr std::array<TB6612FNGValve::PinDef, 3> nutrientPumpDefs = 
    {
        TB6612FNGValve::PinDef{.xIN1 = 15, .xIN2 = 16, .PWMx = 17}, // A
        TB6612FNGValve::PinDef{.xIN1 = 18, .xIN2 = 19, .PWMx = 21}, // B
        TB6612FNGValve::PinDef{.xIN1 = 12, .xIN2 = 13, .PWMx = 14}
    };

constexpr uint8_t valveSwitchPin = 22;
constexpr uint8_t liquidLevelTopSensorPin = 32;
constexpr uint8_t liquidLevelBottomSensorPin = 34;
constexpr uint8_t phSensorPin = 35;
constexpr uint8_t ecSensorPin = 36;

}