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
        Duration refillInterval = std::chrono::milliseconds(1);
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
        closeValves();
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

    void openValves()
    {
        if (isRunning())
            return;

        mValve.open();
    }

    void closeValves()
    {
        if (isRunning())
            return;

        mValve.close();
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

    ValveT& getValve()
    {
        return mValve;
    }

    void update(const Duration dt)
    {
        updateStatus(dt);
        if (mRunning)
            updateControl(dt);
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

    void updateStatus(const Duration)
    {
        mStatus = Status {.levelIsHigh = mSensor.liquidIsPresent(), .valveIsOpen = mValve.isOpen()};
    }


    SensorT mSensor;
    ValveT mValve;
    Config mConfig;
    Duration mFromPrevRefill;
    Status mStatus;
    bool mRunning;
};

}