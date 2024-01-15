#pragma once

#include "actuators.hpp"
#include "controller_config.hpp"
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
    struct Status
    {
        float ph;
        bool isDosing;
    };

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

    const Status& getStatus() const
    {
        return mStatus;
    }

    Doser& getDoser()
    {
        return mPhDownDoser;
    }

    void setTargetPh(float ph)
    {
        if (ph >= 0.0f && ph <= 14.0f)
        {
            mConfig.targetMin = ph;
            mConfig.targetMax = ph;
        }
    }

    void update(const Duration dt)
    {
        updateStatus(dt);
        updateControl(dt);
    }

private:
    void updateControl(const Duration dt)
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

    void updateStatus(const Duration)
    {
        mStatus = Status{.ph = mSensor.readPH(), .isDosing = mPhDownDoser.isDosing()};
    }

    SensorT mSensor;
    Doser mPhDownDoser;
    Controller::Config mConfig;
    Duration mFromLastDose;
    Status mStatus;
};

}