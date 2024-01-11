#pragma once

#include "actuators.hpp"
#include "common.hpp"
#include "sensors.hpp"
#include <utility>


namespace Core
{

template <LiquidLevelSensor SensorT, Valve ValveT>
class LiquidLevelController
{
public:
    struct Config
    {
        Duration refillInterval = std::chrono::minutes(10);
    };

    struct Status
    {
        Duration timeTillRefill;
    };

    LiquidLevelController(SensorT&& sensor, ValveT&& valve, const Config& config = Config{})
    : mSensor{std::move(sensor)}, mValve{std::move(valve)}, mConfig(std::move(config))
    {
        mValve.close();
    }


    void setConfig(const Config& config)
    {
        mConfig = config;
    }

    ValveT& getValve()
    {
        return mValve;
    }

    const Config& getConfig()
    {
        return mConfig;
    }

    const Status& getStatus()
    {
        return mStatus;
    }

    void update(const Duration dt)
    {
        updateControl(dt);
        updateStatus(dt);
    }

private:
    void updateControl(const Duration dt)
    {
        mFromPrevRefill += dt;
        const bool timeToRefill = mFromPrevRefill > mConfig.refillInterval;

        if (mSensor.liquidIsPresent() && mValve.isOpen())
        {
            mValve.close();
        }
        else if (timeToRefill && !mSensor.liquidIsPresent() && !mValve.isOpen())
        {
            mValve.open();
            mFromPrevRefill = Duration{0};
        }
    }

    void updateStatus(const Duration dt)
    {
        if (mFromPrevRefill > mConfig.refillInterval)
        {
            mStatus.timeTillRefill = Duration{0};
        }
        else
        {
            mStatus.timeTillRefill = mConfig.refillInterval - mFromPrevRefill;
        }
    }

    SensorT mSensor;
    ValveT mValve;
    Config mConfig;
    Duration mFromPrevRefill;
    Status mStatus;
};

}