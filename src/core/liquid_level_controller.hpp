#pragma once

#include "actuators.hpp"
#include "common.hpp"
#include "sensors.hpp"
#include <utility>


namespace Core
{

template <DigitalSensor SensorT, Valve ValveT>
class LiquidLevelController
{
public:
    struct Config
    {
        Duration refillInterval;
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

    LiquidLevelController(const SensorT& sensor, ValveT& valve, const Config& config)
    : 
        mSensor{sensor},
        mValve{valve},
        mConfig(std::move(config))
    {
        mValve.close();
        stop();
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
        mStatus.levelIsHigh = mSensor.read();
        mStatus.valveIsOpen = mValve.isOpen();
    }

    const SensorT& mSensor;
    ValveT& mValve;
    Config mConfig;
    Duration mFromPrevRefill{0};
    Status mStatus;
    bool mRunning{};
};

}