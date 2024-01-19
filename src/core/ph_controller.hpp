#pragma once

#include "actuators.hpp"
#include "controller_config.hpp"
#include "common.hpp"
#include "dosing_pump.hpp"
#include "sensors.hpp"
#include <chrono>


namespace Core
{

template <typename SensorT, typename ValveT>
class PHController
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
        return Config{.targetMin = 5.8f, .targetMax = 6.2f, .dosingAmount = 1.0f, .dosingInterval = std::chrono::seconds(10)};
    }

    PHController(SensorT&& sensor, Doser&& phDownDoser, const Config& config = defaultConfig())
    : 
        mSensor{std::move(sensor)}, 
        mPhDownDoser{std::move(phDownDoser)}, 
        mConfig{config}
    {
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

        mPhDownDoser.getValve().open();
    }

    void closeValves()
    {
        if (isRunning())
            return;

        mPhDownDoser.getValve().close();
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
            mPhDownDoser.dose(mConfig.dosingAmount);
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
    bool mRunning;
};

}