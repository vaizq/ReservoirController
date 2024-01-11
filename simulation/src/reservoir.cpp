#include "reservoir.hpp"

Liquid operator+(const Liquid& lhs, const Liquid& rhs)
{
    Liquid l;
    l.amount = lhs.amount + rhs.amount;
    if (l.amount > 0.0f)
    {
        l.ph = (lhs.ph * lhs.amount + rhs.ph * rhs.amount) / l.amount; // Weighted average
        l.ec = (lhs.ec * lhs.amount + rhs.ec * rhs.amount) / l.amount; // Weighted average
    }
    return l;
}

