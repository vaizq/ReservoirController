#pragma once

#include "core/actuators.hpp"
#include "core/sensors.hpp"

#include <optional>


namespace Device
{

template <PHSensor SensorT, DosingPump PumpT>
class PhController
{
public:
    struct Config
    {
        float phTarget = 5.8f;
        float doseAmount = 1.0f;
    };

    PhController(SensorT&& sensor, PumpT&& downPump, PumpT&& upPump)
    : 
    mSensor(std::forward<SensorT>(sensor)), 
    mDownPump(std::forward<PumpT>(downPump)), 
    mUpPump(std::forward<PumpT>(upPump))
    {}

    void setConfig(const Config& config) { mConfig = config; }

    void update()
    {
        const float ph = mSensor.readPH();
        if (ph < mConfig.phTarget)
        {
            mUpPump.dose(mConfig.doseAmount);
        }
        else if (ph > mConfig.phTarget)
        {
            mDownPump.dose(mConfig.doseAmount);
        }
    }

private:
    SensorT mSensor;
    PumpT mDownPump;
    PumpT mUpPump;
    Config mConfig;
};

}