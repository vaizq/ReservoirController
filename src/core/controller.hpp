#pragma once

#include "common.hpp"
#include <utility>

// Trivial control algorithm for ph and ec controller

namespace Core
{

namespace Controller
{
    struct Config
    {
        std::pair<float, float> targetRange;
        float dosingAmount;
        Duration dosingInterval;
    };
}

}