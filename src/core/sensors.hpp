#pragma once


#include <concepts>


namespace Core
{

template <typename T>
concept AnalogSensor = requires(T sensor)
{
    { sensor.read() } -> std::convertible_to<float>;
};

template <typename T>
concept DigitalSensor = requires(T sensor)
{
    { sensor.read() } -> std::convertible_to<bool>;
};

}