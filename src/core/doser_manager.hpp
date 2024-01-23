#pragma once

#include "dosing_pump.hpp"
#include "common.hpp"
#include <array>
#include <queue>
#include <set>
#include <algorithm>


namespace Core
{

template <typename ValveT, size_t N>
class DoserManager
{
    using Doser = Core::DosingPump<ValveT>;
    using DoserArray = std::array<Doser, N>;

    class Dose
    {
    public:
        Dose(Doser* doser, float amount)
        : mDoser{doser}, mAmount{amount}
        {}

        void exec() 
        {
            mDoser->dose(mAmount);
        }

        bool done() const
        {
            return !mDoser->isDosing();
        }

        bool operator < (const Dose& rhs) const
        {
            return mAmount < rhs.mAmount;
        }

    private:
        Doser* mDoser;
        float mAmount;
    };


public:
    using DoserID = size_t;

    DoserManager(DoserArray&& dosers, size_t simultaneusMax = N)
    : mDosers{std::move(dosers)}, mSM{simultaneusMax} 
    {}

    bool queueDose(DoserID id, float amount)
    {
        if (id >= mDosers.size() || amount < 0)
            return false;
        
        mPendingDoses.push(Dose{&mDosers[id], amount});
        return true;
    }

    bool queueDose(float amount)
    {
        constexpr float singleDose = 1.0f;
        for(int i = 0; i < static_cast<int>(amount / singleDose); ++i)
        {
            for (auto& doser : mDosers)
            {
                mPendingDoses.push(Dose{&doser, singleDose});
            }
        }

        return true;
    }

    void reset()
    {
        mActiveDoses.clear();

        while(!mPendingDoses.empty())
        {
            mPendingDoses.pop();
        }
    }

    void update(const Duration dt)
    {
        for (Doser& d : mDosers)
        {
            d.update(dt);
        }

        mActiveDoses.erase(
            std::remove_if(mActiveDoses.begin(), mActiveDoses.end(), [](const Dose& d) { return d.done(); }),
            mActiveDoses.end()
        );

        while (!mPendingDoses.empty() && mActiveDoses.size() < mSM)
        {
            auto dose = mPendingDoses.front();
            dose.exec();
            mActiveDoses.push_back(dose);
            mPendingDoses.pop();
        }
    }

private:
    DoserArray mDosers;
    size_t mSM;
    std::queue<Dose> mPendingDoses;
    std::vector<Dose> mActiveDoses;
};

}