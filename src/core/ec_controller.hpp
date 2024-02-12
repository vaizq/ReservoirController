#pragma once

#include "actuators.hpp"
#include "sensors.hpp"
#include "common.hpp"
#include "doser_manager.hpp"
#include <array>
#include <algorithm>
#include <chrono>
#include <numeric>


namespace Core
{

    template<AnalogSensor SensorT, Valve ValveT, size_t TotalDoserCount, size_t NutrientDoserCount>
    class ECController
    {
    public:
        using DM = DoserManager<ValveT, TotalDoserCount>;
        using AmountPerLiter = float;
        using NutrientSchedule = std::array<std::pair<typename DM::DoserID, AmountPerLiter>, NutrientDoserCount>;

        struct Config
        {
            float target;
            float dosingAmount;
            Duration dosingInterval;
            NutrientSchedule schedule;
        };

        struct Status
        {
            float ec;
        };

        ECController(const SensorT& sensor, DM& dm, const Config& config)
        :
            mSensor{sensor},
            mDM{dm},
            mConfig{config}
        {
            mTotalAmount = totalAmount(mConfig.schedule);
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

        void setConfig(const Config& config)
        {
            mConfig = config;
            mTotalAmount = totalAmount(mConfig.schedule);
        }

        const Config& getConfig() const
        {
            return mConfig;
        }

        const Status& getStatus() const
        {
            return mStatus;
        }

        SensorT& getSensor()
        {
            return mSensor;
        }

        void update(Duration dt)
        {
            if (mRunning)
                updateControl(dt);
        }

    private:
        void updateControl(const Duration dt)
        {
            mFromPrevDosing += dt;
            const bool timeToDose = mFromPrevDosing > mConfig.dosingInterval;

            mStatus.ec = mSensor.read();

            if (timeToDose && mStatus.ec < mConfig.target)
            {
                mFromPrevDosing = Duration{0};

                for (const auto& nutrientPump : mConfig.schedule)
                {
                    const auto id = nutrientPump.first;
                    const float amount = nutrientPump.second / mTotalAmount * mConfig.dosingAmount;
                    mDM.queueDose(id, amount);
                }
            }
        }

        float totalAmount(const NutrientSchedule& schedule)
        {
            return std::accumulate(schedule.begin(), schedule.end(), 0.0f, [](float total, const auto& nutrientPump) { return total + nutrientPump.second; });
        }

        const SensorT& mSensor;
        DM& mDM;
        Config mConfig;
        Status mStatus;
        Duration mFromPrevDosing{0};
        AmountPerLiter mTotalAmount;
        bool mRunning{};
    };

}