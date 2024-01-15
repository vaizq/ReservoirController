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

template<ECSensor SensorT, Valve ValveT, size_t PumpCount>
class ECController
{
public:
    using Doser = DosingPump<ValveT>;
    using DoserArray = std::array<Doser, PumpCount>;
    using AmountPerLiter = float;
    using Schedule = std::array<AmountPerLiter, PumpCount>;
    using Config = Controller::Config;

    struct Status
    {
        float ec;
        Schedule schedule;
    };

    static constexpr Config defaultConfig()
    {
        return Config(1.0f, 1.5f, 10.0f, std::chrono::minutes(1));
    }

    ECController(SensorT&& sensor, DoserArray&& dosers, const Config config = defaultConfig())
    : mSensor(std::forward<SensorT>(sensor)), mDosers(std::move(dosers)), mConfig(config)
    {}

    void setSchedule(const Schedule& schedule)
    {
        const AmountPerLiter amountTotal = std::accumulate(
            schedule.begin(), schedule.end(), 
            0.0f, [&schedule](float total, AmountPerLiter amount) { return total + amount; });

        for (int i = 0; i < PumpCount; i++)
        {
            mDoserSchedule[i] = schedule[i] / amountTotal;
        }
    }

    const Schedule& getSchedule()
    {
        return mDoserSchedule;
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
            }

    DoserArray& getDosers()
    {
        return mDosers;
    }

    void update(Duration dt)
    {
        updateStatus(dt);
        updateControl(dt);
    }

private:
    void updateControl(const Duration dt)
    {
        mFromPrevDosing += dt;
        const bool timeToDose = mFromPrevDosing > mConfig.dosingInterval;

        const float ec = mSensor.readEC();

        // Check if it's timeToDose, EC is too low and none of the pumps are dosing
        if (timeToDose && ec < mConfig.targetMin && 
            std::find_if(mDosers.begin(), mDosers.end(), [](const auto& pump) { return pump.isDosing(); }) == std::end(mDosers))
        {
            for (int i = 0; i < PumpCount; i++)
            {
                const float amount = mDoserSchedule[i] * mConfig.dosingAmount; 
                mDosers[i].dose(amount);
            }

            mFromPrevDosing = Duration{0};
        }

        for (auto& pump : mDosers)
        {
            pump.update(dt);
        }
    }

    void updateStatus(const Duration dt)
    {
        mStatus = Status{.ec = mSensor.readEC(), .schedule = mDoserSchedule};
    }

    SensorT mSensor;
    DoserArray mDosers;
    Schedule mDoserSchedule{0.0f};
    Controller::Config mConfig;
    Duration mFromPrevDosing;
    Status mStatus;
};

}