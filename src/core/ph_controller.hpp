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
public:
    using Doser = DosingPump<ValveT>;
    using Config = Controller::Config;

    static constexpr Config defaultConfig()
    {
        return Config(5.8f, 6.2f, 1.0f, std::chrono::minutes(1));
    }

    PhController(SensorT&& sensor, Doser&& phDownDoser, const Config& config = defaultConfig())
    : 
        mSensor{std::move(sensor)}, 
        mPhDownDoser{std::move(phDownDoser)}, 
        mConfig{config}
    {}

    void setConfig(const Config& config) 
    { 
        mConfig = config; 
    }

    const Config& getConfig() const
    {
        return mConfig;
    }

    Doser& getDoser()
    {
        return mPhDownDoser;
    }

    void update(Duration dt)
    {
        mFromLastDose += dt;
        const bool timeToDose = mFromLastDose > mConfig.dosingInterval;

        const float ph{mSensor.readPH()};

        if (timeToDose && ph > mConfig.targetMax)
        {
            mPhDownDoser.dose(mConfig.dosingAmount);
            mFromLastDose = Duration(0);
        }

        mPhDownDoser.update(dt);
    }

private:
    SensorT mSensor;
    Doser mPhDownDoser;
    Controller::Config mConfig;
    Duration mFromLastDose;
};

}