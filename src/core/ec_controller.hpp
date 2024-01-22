#pragma once

#include "controller_config.hpp"
#include "common.hpp"
#include "dosing_pump.hpp"
#include "sensors.hpp"
#include "doser_manager.hpp"

#include <array>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <Arduino.h>


namespace Core
{

template<typename SensorT, typename ValveT, size_t TotalDoserCount, size_t NutrientDoserCount>
class ECController
{
public:
    using Dosers = DoserManager<ValveT, TotalDoserCount>;
    using AmountPerLiter = float;
    using NutrientSchedule = std::array<std::pair<typename Dosers::DoserID, AmountPerLiter>, NutrientDoserCount>;
    using Config = Controller::Config;

    struct Status
    {
        float ec;
        NutrientSchedule schedule;
    };

    static constexpr Config defaultConfig()
    {
        return Config{.targetMin = 1.0f, .targetMax = 1.5f, .dosingAmount = 10.0f, .dosingInterval = std::chrono::seconds(60)};
    }

    ECController(SensorT&& sensor, Dosers& dosers, NutrientSchedule schedule, const Config config = defaultConfig())
    : 
        mSensor(std::forward<SensorT>(sensor)), 
        mDosers(dosers), 
        mSchedule{std::move(schedule)},
        mConfig(config)
    {
        mTotalAmount = totalAmount(mSchedule);
        start();
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

    void setSchedule(NutrientSchedule schedule)
    {
        mSchedule = std::move(schedule);
        mTotalAmount = totalAmount(mSchedule);
    }

    const NutrientSchedule& getSchedule()
    {
        return mSchedule;
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

    void setTargetEc(float ec)
    {
        if (ec >= 0.0f)
        {
            mConfig.targetMax = ec;
            mConfig.targetMin = ec;
        }
    }

    void update(Duration dt)
    {
        updateStatus(dt);
        if (mRunning)
            updateControl(dt);
    }

private:
    void updateControl(const Duration dt)
    {
        mFromPrevDosing += dt;
        const bool timeToDose = mFromPrevDosing > mConfig.dosingInterval;

        const float ec = mSensor.readEC();

        // Check if it's timeToDose, EC is too low and none of the pumps are dosing
        if (timeToDose && ec < mConfig.targetMin)
        {
            mFromPrevDosing = Duration{0};

            for (const auto& nutrientPump : mSchedule)
            {
                const float amount = nutrientPump.second / mTotalAmount * mConfig.dosingAmount;
                const auto id = nutrientPump.first;
                mDosers.queueDose(id, amount);
            }
        }
    }

    void updateStatus(const Duration dt)
    {
        mStatus = Status{.ec = mSensor.readEC(), .schedule = mSchedule};
    }

    float totalAmount(const NutrientSchedule& schedule)
    {
        return std::accumulate(schedule.begin(), schedule.end(), 0.0f, [](float total, const auto& nutrientPump) { return total + nutrientPump.first; });
    }

    SensorT mSensor;
    Dosers& mDosers;
    NutrientSchedule mSchedule;
    Controller::Config mConfig;
    Duration mFromPrevDosing;
    Status mStatus;
    bool mRunning;
    AmountPerLiter mTotalAmount;
};

}