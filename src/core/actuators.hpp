#pragma once

#include <concepts>
#include "duration.hpp"


template <typename T>
concept Valve = requires(T valve)
{
    { valve.open() };
    { valve.close() };
    { valve.isOpen() } -> std::convertible_to<bool>;
};


template <typename T>
concept Doser = requires(T doser, float amount, Duration dt)
{
    { doser.dose(amount) };
    { doser.isDosing() } -> std::convertible_to<bool>;
    { doser.update(dt) };
};