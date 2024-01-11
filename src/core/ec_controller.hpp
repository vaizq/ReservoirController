#pragma once

#include "actuators.hpp"
#include "controller.hpp"
#include "common.hpp"
#include "dosing_pump.hpp"
#include "sensors.hpp"

#include <array>
#include <chrono>
#include <algorithm>
#include <ranges>
#include <numeric>


namespace Core
{

template<ECSensor SensorT, Valve ValveT, unsigned int PumpCount>
class ECController
{
    using Doser = DosingPump<ValveT>;

public:
    using PumpArray = std::array<Doser, PumpCount>;
    using AmountPerLiter = float;
    using Schedule = std::array<AmountPerLiter, PumpCount>;


    static constexpr Controller::Config defaultConfig()
    {
        return Controller::Config(std::make_pair<float, float>(1.0f, 1.5f), 1.0f, std::chrono::seconds(60));
    }


    ECController(SensorT&& sensor, PumpArray&& pumps, const Controller::Config config = defaultConfig())
    : mSensor(std::forward<SensorT>(sensor)), mPumps(std::move(pumps)), mConfig(config)
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

    void setConfig(const Controller::Config& config) 
    { 
        mConfig = config; 
    }

    void update(Duration dt)
    {
        const float ec = mSensor.readEC();
        const auto now = Clock::now();
        const bool doseTime = (now - mPrevDose) > mConfig.dosingInterval;

        // Check if EC is too low and none of the pumps are dosing
        if (doseTime && ec < mConfig.targetRange.first && 
            std::find_if(mPumps.begin(), mPumps.end(), [](const auto& pump) { return pump.isDosing(); }) == std::end(mPumps))
        {
            for (int i = 0; i < PumpCount; i++)
            {
                const float amount = mPumpRatios[i] * mConfig.dosingAmount; 
                mPumps[i].dose(amount);
            }

            mPrevDose = now;
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
    Controller::Config mConfig;
    Clock::time_point mPrevDose;
};

}