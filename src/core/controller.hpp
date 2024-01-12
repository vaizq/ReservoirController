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
        float targetMin;
        float targetMax;
        float dosingAmount;
        Duration dosingInterval;
    };
}

}