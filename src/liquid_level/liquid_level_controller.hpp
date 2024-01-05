#pragma once

#include "core/sensors.hpp"
#include "core/actuators.hpp"
#include <utility>


template <LiquidLevelSensor SensorT, Valve ValveT>
class LiquidLevelController
{
public:
    LiquidLevelController(SensorT&& sensor, ValveT&& valve)
    : mSensor{std::forward<SensorT>(sensor)}, mValve{std::forward<ValveT>(valve)}
    {
        mValve.close();
    }

    void update()
    {
        if (mSensor.liquidIsPresent())
        {
            mValve.close();
        }
        else
        {
            mValve.open();
        }
    }
private:
    SensorT mSensor;
    ValveT mValve;
};