#pragma once

#include "doser.hpp"
#include "common.hpp"
#include <array>
#include <queue>
#include <set>
#include <algorithm>


namespace Core
{

    // DoserManager makes sure that at most parallelLimit dosers run simultaneously.
    // Dosers are accessed by indices in range [0, (DoserCount-1)].
    template <typename ValveT, size_t N>
    class DoserManager
    {
    public:
        static constexpr size_t DoserCount = N;
        using DoserID = size_t;

        class DoserHandle
        {
        public:
            DoserHandle(DoserManager<ValveT, N>& dm, DoserID id)
            :   mDM{dm},
                mID{id}
            {}

            void queueDose(float amount)
            {
                mDM.queueDose(mID, amount);
            }

        private:
            DoserManager<ValveT, N>& mDM;
            DoserID mID;
        };

    private:
        using DoserArray = std::array<Doser<ValveT>, DoserCount>;

        class Dose
        {
        public:
            Dose(Doser<ValveT>* doser, float amount)
            : mDoser{doser}, mAmount{amount}
            {}

            void exec()
            {
                mDoser->dose(mAmount);
            }

            [[nodiscard]] bool isDone() const
            {
                return !mDoser->isDosing();
            }

            bool operator < (const Dose& rhs) const
            {
                return mAmount < rhs.mAmount;
            }

        private:
            Doser<ValveT>* mDoser;
            float mAmount;
        };

    public:

        explicit DoserManager(DoserArray&& dosers, size_t parallelLimit = DoserCount)
        :   mDosers{std::move(dosers)},
            mParallelLimit{parallelLimit}
        {}

        DoserManager(const DoserManager&) = delete;

        DoserHandle getHandle(DoserID doserID)
        {
            return DoserHandle(*this, doserID);
        }

        bool queueDose(DoserID id, float amount)
        {
            if (id >= mDosers.size() || amount < 0)
                return false;

            mPendingDoses.push(Dose{&mDosers[id], amount});
            return true;
        }

        void reset()
        {
            for (auto& d : mDosers)
            {
                d.reset();
            }

            mActiveDoses.clear();

            while(!mPendingDoses.empty())
            {
                mPendingDoses.pop();
            }
        }

        void update(const Duration dt)
        {
            for (auto& doser : mDosers)
            {
                doser.update(dt);
            }

            mActiveDoses.erase(
                std::remove_if(mActiveDoses.begin(), mActiveDoses.end(), [](const Dose& dose) { return dose.isDone(); }),
                mActiveDoses.end()
            );

            while (!mPendingDoses.empty() && mActiveDoses.size() < mParallelLimit)
            {
                auto dose = mPendingDoses.front();
                dose.exec();
                mActiveDoses.push_back(dose);
                mPendingDoses.pop();
            }
        }

    private:
        DoserArray mDosers;
        size_t mParallelLimit;
        std::queue<Dose> mPendingDoses;
        std::vector<Dose> mActiveDoses;
    };

}