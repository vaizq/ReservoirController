#pragma once

#include "actuators.hpp"
#include "common.hpp"
#include "dosing_pump.hpp"
#include "doser_manager.hpp"
#include <chrono>


namespace Core
{

template <typename SensorT, typename ValveT, size_t N>
class PHController
{
public:
    using Dosers = DoserManager<ValveT, N>;

    struct Config
    {
        float target = 6.2f;
        float dosingAmount = 1.0f;
        Duration dosingInterval = std::chrono::seconds(60);
    };

    struct Status
    {
        float ph;
    };

    static constexpr Config defaultConfig()
    {
        return Config{.target = 6.2f, .dosingAmount = 1.0f, .dosingInterval = std::chrono::seconds(50)};
    }

    PHController(SensorT&& sensor, Dosers& doserManager, typename Dosers::DoserID doserID, const Config& config = defaultConfig())
    : 
        mSensor{std::move(sensor)}, 
        mDosers{doserManager}, 
        mDownDoser{doserID},
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

        mStatus.ph = mSensor.readPH();

        if (timeToDose && mStatus.ph > mConfig.target)
        {
            mFromLastDose = Duration(0);
            mDosers.queueDose(mDownDoser, mConfig.dosingAmount);
        }
    }

    SensorT mSensor;
    Dosers& mDosers;
    const typename Dosers::DoserID mDownDoser;
    Config mConfig;
    Status mStatus;
    Duration mFromLastDose{0};
    bool mRunning;
};

}