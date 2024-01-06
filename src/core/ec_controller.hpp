#pragma once

#include "actuators.hpp"
#include "duration.hpp"
#include "dosing_pump.hpp"
#include "sensors.hpp"

#include <array>
#include <chrono>
#include <algorithm>
#include <ranges>
#include <numeric>

namespace Core
{

template<ECSensor SensorT, Doser DoserT, unsigned int PumpCount>
class ECController
{
public:
    using PumpArray = std::array<DoserT, PumpCount>;
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

    void setTargetEC(float ec) { mConfig.targetEC = ec; }

    void update(Duration dt)
    {
        const float ec = mSensor.readEC();
        // Check if EC is too low and none of the pumps are dosing
        if (ec < mConfig.targetEC && 
            std::find_if(mPumps.begin(), mPumps.end(), [](const auto& pump) { return pump.isDosing(); }) == std::end(mPumps))
        {
            for (int i = 0; i < PumpCount; i++)
            {
                const float amount = mPumpRatios[i] * mConfig.doseAmount; 
                mPumps[i].dose(amount);
            }
        }

        for (auto& pump : mPumps)
        {
            pump.update(dt);
        }
    }

private:
    SensorT mSensor;
    PumpArray mPumps;
    std::array<float, PumpCount> mPumpRatios{0.0f};
    Config mConfig;
};

}