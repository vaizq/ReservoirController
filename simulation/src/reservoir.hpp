#pragma once

#include <cmath>
#include <mutex>

struct Liquid;


Liquid operator+(const Liquid& lhs, const Liquid& rhs);


struct Liquid
{
    float amount;
    float ph;
    float ec;
    void operator+=(const Liquid& other)
    {
        *this = *this + other;
    }
};


constexpr Liquid finnishTapWater(float amount)
{
    return Liquid{.amount = amount, .ph = 8.0f, .ec = 0.2f};
}

constexpr Liquid phDownSolution(float amount)
{
    return Liquid{.amount = amount, .ph = 1.0f, .ec = 0.2f};
}

constexpr Liquid phUpSolution(float amount)
{
    return Liquid{.amount = amount, .ph = 13.0f, .ec = 0.2f};
}

constexpr Liquid nutrientSolution(float amount)
{
    return Liquid{.amount = amount, .ph = 5.0f, .ec = 100.0f};
}

constexpr float mL2L(float amount_mL)
{
    return amount_mL / 1000.0f;
}

constexpr float L2mL(float amount_L)
{
    return 1000.0f * amount_L;
}


class Reservoir
{
public:
    Reservoir(float capacity, Liquid content = Liquid{})
    : mCapacity{capacity}, mContent{content}
    {}

    void add(const Liquid liquid)
    {
        mContent += liquid; 
        mContent.amount = std::min(mCapacity, mContent.amount);
    }

    void remove(float amount)
    {
        mContent.amount -= amount;
        if (mContent.amount < 0.0f)
        {
            mContent.amount = 0.0f;
        }
    }

    float capacity() const
    {
        return mCapacity;
    }
   
    const Liquid& content() const
    {
        return mContent;
    }

private:
    float mCapacity;
    Liquid mContent;
};


