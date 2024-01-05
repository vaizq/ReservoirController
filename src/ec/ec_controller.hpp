#pragma once

#include "core/actuators.hpp"
#include "core/sensors.hpp"

#include <array>
#include <chrono>
#include <algorithm>
#include <ranges>
#include <numeric>




template<ECSensor SensorT, DosingPump PumpT, unsigned int PumpCount>
class ECController
{
public:
    using PumpArray = std::array<DosingPump, PumpCount>;
    using AmountPerLiter = float;
    using Schedule = std::array<AmountPerLiter, PumpCount>;

    struct Config
    {
        float targetEC;
        float doseAmount = 10.0f;
    };

    ECController(SensorT&& sensor, PumpArray&& pumps)
    : mSensor(std::forward<SensorT>(sensor)), mPumps(std::move(pumps))
    {}

    void setSchedule(const Schedule& schedule)
    {
        const AmountPerLiter amountTotal = std::accumulate(
            schedule.begin(), schedule.end(), 
            0.0f, [&schedule](float total, AmountPerLiter amount) { return total + amount; });

        for (int i = 0; i < PumpCount; i++)
        {
            mPumpRatios[i] = schedule[i] / amountTotal;
        }
    }

    void setConfig(const Config& config) { mConfig = config; }

    void setTargetEC(float ec) { mTargetEC = ec; }

    void loop()
    {
        const float ec = mSensor.readEC();
        // Check if EC is too low and none of the pumps are dosing
        if (ec < mTargetEC && 
            std::find_if(mPumps.begin(), mPumps.end(), [](const PumpT& pump) { return pump.isDosing(); }) == std::end(mPumps));
        {
            for (int i = 0; i < PumpCount; i++)
            {
                const float amount = mPumpRatios[i] * mConfig.doseAmount; 
                mPumps[i].dose(amount);
            }
        }
    }

private:
    SensorT mSensor;
    PumpArray mPumps;
    std::array<float, PumpCount> mPumpRatios{0.0f};
    Config mConfig;
};






