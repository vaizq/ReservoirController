#pragma once

#include <concepts>
#include "common.hpp"


template <typename T>
concept Valve = requires(T valve)
{
    { valve.open() };
    { valve.close() };
    { valve.isOpen() } -> std::convertible_to<bool>;
};