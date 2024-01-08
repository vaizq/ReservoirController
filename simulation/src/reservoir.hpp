#pragma once

struct Liquid;


Liquid operator+(const Liquid& lhs, const Liquid& rhs);


struct Liquid
{
    float amount{};
    float ph = 7.0f;
    float ec{};
    void operator+=(const Liquid& other)
    {
        *this = *this + other;
    }
};





class Reservoir
{
public:
    Reservoir(Liquid content)
    : mContent(content)
    {}

    void add(const Liquid liquid)
    {
        mContent += liquid; 
    }

    void remove(float amount)
    {
        if (amount > mContent.amount)
            mContent.amount = 0.0f;
        mContent.amount -= amount;
    }
    
    const Liquid& content() const
    {
        return mContent;
    }
private:
    Liquid mContent;
};


