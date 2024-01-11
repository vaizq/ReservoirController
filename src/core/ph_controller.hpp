#pragma once

#include "actuators.hpp"
#include "controller.hpp"
#include "common.hpp"
#include "dosing_pump.hpp"
#include "sensors.hpp"
#include <chrono>


namespace Core
{

template <PHSensor SensorT, Valve ValveT>
class PhController
{
    using Doser = DosingPump<ValveT>;
public:

    static constexpr Controller::Config defaultConfig()
    {
        return Controller::Config(std::make_pair<float, float>(5.8f, 6.2f), 1.0f, std::chrono::minutes(1));
    }

    PhController(SensorT&& sensor, Doser&& phDownPump, const Controller::Config& config = defaultConfig())
    : 
        mSensor{std::move(sensor)}, 
        mPhDownPump{std::move(phDownPump)}, 
        mConfig{config}
    {}

    void setConfig(const Controller::Config& config) 
    { 
        mConfig = config; 
    }

    ValveT& getValve()
    {
        return mPhDownPump.getValve();
    }

    void update(Duration dt)
    {
        mFromLastDose += dt;
        const bool timeToDose = mFromLastDose > mConfig.dosingInterval;

        const float ph{mSensor.readPH()};

        if (timeToDose && ph > mConfig.targetRange.second)
        {
            mPhDownPump.dose(mConfig.dosingAmount);
            mFromLastDose = Duration(0);
        }

        mPhDownPump.update(dt);
    }

private:
    SensorT mSensor;
    Doser mPhDownPump;
    Controller::Config mConfig;
    Duration mFromLastDose;
};

}