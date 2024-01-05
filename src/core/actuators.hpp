#pragma once

#include <concepts>


template <typename T>
concept Valve = requires(T valve)
{
    { valve.open() };
    { valve.close() };
    { valve.isOpen() } -> std::convertible_to<bool>;

};


template <typename T>
concept Pump = requires(T pump)
{
    { pump.start() };
    { pump.stop() };
    { pump.isRunning() } -> std::convertible_to<bool>;
};


template <typename T>
concept DosingPump = requires(T pump, float amount)
{
    { pump.dose(amount) };
    { pump.stop() };
    { pump.isDosing() } -> std::convertible_to<bool>;
    { pump.flowRate() } -> std::convertible_to<float>;
    { pump.loop() };
};