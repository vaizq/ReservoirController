#pragma once

#include "driver/tb6612fng_valve.hpp"
#include <array>
#include <cstdint>


namespace Config 
{

using namespace Driver;

constexpr TB6612FNGValve::PinDef phDownPumpDef = {.xIN1 = GPIO_NUM_4, .xIN2 = GPIO_NUM_5, .PWMx = GPIO_NUM_23}; // A

constexpr std::array<TB6612FNGValve::PinDef, 3> nutrientPumpDefs = 
    {
        TB6612FNGValve::PinDef{.xIN1 = GPIO_NUM_15, .xIN2 = GPIO_NUM_16, .PWMx = GPIO_NUM_17}, // A
        TB6612FNGValve::PinDef{.xIN1 = GPIO_NUM_18, .xIN2 = GPIO_NUM_19, .PWMx = GPIO_NUM_21}, // B
        TB6612FNGValve::PinDef{.xIN1 = GPIO_NUM_12, .xIN2 = GPIO_NUM_13, .PWMx = GPIO_NUM_14}
    };

constexpr gpio_num_t valveSwitchPin = GPIO_NUM_22;
constexpr gpio_num_t liquidLevelTopSensorPin = GPIO_NUM_32;
constexpr gpio_num_t liquidLevelBottomSensorPin = GPIO_NUM_34;
constexpr gpio_num_t phSensorPin = GPIO_NUM_35;
constexpr gpio_num_t ecSensorPin = GPIO_NUM_36;

}