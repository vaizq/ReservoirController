#pragma once


#include <concepts>
#include "common.hpp"


namespace Core
{

template <typename T>
concept AnalogSensor = requires(T sensor, Duration dt, float value)
{
    { sensor.read() } -> std::convertible_to<float>;
    { sensor.update(dt) };
    { sensor.calibrate(value) } -> std::convertible_to<bool>;
};

template <typename T>
concept DigitalSensor = requires(T sensor)
{
    { sensor.read() } -> std::convertible_to<bool>;
};

}