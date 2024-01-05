#pragma once
#include <concepts>


template <typename T>
concept LiquidLevelSensor = requires(T sensor)
{
    { sensor.liquidIsPresent() } -> std::convertible_to<bool>;
};


template <typename T>
concept PHSensor = requires(T sensor)
{
    { sensor.readPH() } -> std::convertible_to<float>;
};


template <typename T>
concept ECSensor = requires(T sensor)
{
    { sensor.readEC() } -> std::convertible_to<float>;
};