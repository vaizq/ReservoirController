#pragma once

#include "common.hpp"
#include "sensors.hpp"
#include <utility>


namespace Core
{

template <typename SensorT, typename ValveT>
class LiquidLevelController
{
public:
    struct Config
    {
        Duration refillInterval = std::chrono::seconds(1);
    };

    struct Status
    {
        bool levelIsHigh;
        bool valveIsOpen;
    };

    static constexpr Config defaultConfig()
    {
        return Config{};
    }

    LiquidLevelController(SensorT&& sensor, ValveT&& valve, const Config& config = defaultConfig())
    : 
        mSensor{std::move(sensor)}, 
        mValve{std::move(valve)}, 
        mConfig(std::move(config))
    {
        mValve.close();
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
        mFromPrevRefill += dt;
        const bool timeToRefill = mFromPrevRefill > mConfig.refillInterval;

        updateStatus();

        if (mStatus.levelIsHigh && mStatus.valveIsOpen)
        {
            mValve.close();
        }
        else if (timeToRefill && !mStatus.levelIsHigh && !mStatus.valveIsOpen)
        {
            mFromPrevRefill = Duration{0};
            mValve.open();
        }
    }

    void updateStatus()
    {
        mStatus.levelIsHigh = mSensor.liquidIsPresent();
        mStatus.valveIsOpen = mValve.isOpen();
    }

    SensorT mSensor;
    ValveT mValve;
    Config mConfig;
    Duration mFromPrevRefill{0};
    Status mStatus;
    bool mRunning;
};

}