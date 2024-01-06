#pragma once

#include "actuators.hpp"
#include "duration.hpp"
#include "dosing_pump.hpp"
#include "sensors.hpp"
#include <chrono>


namespace Core
{

template <PHSensor SensorT, Doser DoserT>
class PhController
{
public:
    struct Config
    {
        float phTarget = 5.8f;
        float doseAmount = 1.0f;
    };

    PhController(SensorT&& sensor, DoserT&& downPump, DoserT&& upPump)
    : 
        mSensor(std::forward<SensorT>(sensor)), 
        mDownPump(std::forward<DoserT>(downPump)), 
        mUpPump(std::forward<DoserT>(upPump))
    {}

    void setConfig(const Config& config) { mConfig = config; }

    void update(Duration dt)
    {
        const float ph = mSensor.readPH();
        if (ph < mConfig.phTarget && !mUpPump.isDosing())
        {
            mUpPump.dose(mConfig.doseAmount);
        }
        else if (ph > mConfig.phTarget && !mDownPump.isDosing())
        {
            mDownPump.dose(mConfig.doseAmount);
        }

        mUpPump.update(dt);
        mDownPump.update(dt);
    }

private:
    SensorT mSensor;
    DoserT mDownPump;
    DoserT mUpPump;
    Config mConfig;
};

}