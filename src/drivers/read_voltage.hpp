#pragma once

#include <cstdint>
#include <Arduino.h>


float readVoltage(uint8_t pin);


class VoltageReader
{
public:
    VoltageReader(uint8_t pin, unsigned samplesCount, uint8_t mode = INPUT)
    : mPin{pin}, mSamplesCount{samplesCount}
    {
        pinMode(mPin, mode);
        mPrev = readVoltage(pin);
    }

    float voltage()
    {
        // Running average of previous sampleCount samples
        const float v = (readVoltage(mPin) + (mSamplesCount - 1) * mPrev) / mSamplesCount;
        mPrev = v;
        return v;
    }

    float operator()()
    {
        return voltage();
    }

private:
    uint8_t mPin;
    unsigned mSamplesCount;
    float mPrev;
};