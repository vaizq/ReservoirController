#pragma once

#include "actuators.hpp"
#include "common.hpp"
#include "doser_manager.hpp"
#include "actuators.hpp"
#include "sensors.hpp"
#include <chrono>


namespace Core
{

template <AnalogSensor SensorT, Valve ValveT, size_t N>
class PHController
{
public:
    using DM = DoserManager<ValveT, N>;

    struct Config
    {
        float target;
        float dosingAmount;
        Duration dosingInterval;
    };

    struct Status
    {
        float ph;
    };

    PHController(SensorT&& sensor, DM::DoserHandle doser, const Config& config)
    : 
        mSensor{std::move(sensor)},
        mDoser{doser},
        mConfig{config}
    {
        start();
    }

    void start()
    {
        mRunning = true;
    }

    void stop()
    {
        mRunning = false;
    }

    bool isRunning() const
    {
        return mRunning;
    }

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

    SensorT& getSensor()
    {
        return mSensor;
    }

    void update(const Duration dt)
    {
        if (mRunning)
            updateControl(dt);
    }

private:
    void updateControl(const Duration dt)
    {
        mFromLastDose += dt;
        const bool timeToDose = mFromLastDose > mConfig.dosingInterval;

        mStatus.ph = mSensor.read();

        if (timeToDose && mStatus.ph > mConfig.target)
        {
            mFromLastDose = Duration(0);
            mDoser.queueDose(mConfig.dosingAmount);
        }
    }

    SensorT mSensor;
    DM::DoserHandle mDoser;
    Config mConfig;
    Status mStatus;
    Duration mFromLastDose{0};
    bool mRunning{};
};

}