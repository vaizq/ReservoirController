#pragma once

#include "actuators.hpp"
#include "controller_config.hpp"
#include "common.hpp"
#include "dosing_pump.hpp"
#include "sensors.hpp"
#include "doser_manager.hpp"
#include <chrono>


namespace Core
{

template <typename SensorT, typename ValveT, size_t N>
class PHController
{
public:
    using Doser = DosingPump<ValveT>;
    using Dosers = DoserManager<ValveT, N>;
    using Config = Controller::Config;

    struct Status
    {
        float ph;
        bool isDosing;
    };

    static constexpr Config defaultConfig()
    {
        return Config{.targetMin = 5.8f, .targetMax = 6.2f, .dosingAmount = 1.0f, .dosingInterval = std::chrono::seconds(10)};
    }

    PHController(SensorT&& sensor, DoserManager<ValveT, N>& doserManager, typename Dosers::DoserID doserID, const Config& config = defaultConfig())
    : 
        mSensor{std::move(sensor)}, 
        mDoserManager{doserManager}, 
        mPHDownDoser{doserID},
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

    void setTargetPh(float ph)
    {
        if (ph >= 0.0f && ph <= 14.0f)
        {
            mConfig.targetMin = ph;
            mConfig.targetMax = ph;
        }
    }

    float ph()
    {
        return mSensor.readPH();
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

        const float ph{mSensor.readPH()};

        if (timeToDose && ph > mConfig.targetMax)
        {
            mFromLastDose = Duration(0);
            mDoserManager.queueDose(mPHDownDoser, mConfig.dosingAmount);
        }
    }

    SensorT mSensor;
    Dosers& mDoserManager;
    const typename Dosers::DoserID mPHDownDoser;
    Controller::Config mConfig;
    Duration mFromLastDose;
    Status mStatus;
    bool mRunning;
};

}