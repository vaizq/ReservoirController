#pragma once

#include "actuators.hpp"
#include "duration.hpp"
#include "sensors.hpp"
#include <utility>


namespace Core
{

template <LiquidLevelSensor SensorT, Valve ValveT>
class LiquidLevelController
{
public:
    LiquidLevelController(SensorT&& sensor, ValveT&& valve)
    : mSensor{std::forward<SensorT>(sensor)}, mValve{std::forward<ValveT>(valve)}
    {
        mValve.close();
    }

    void update(Duration dt)
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

}