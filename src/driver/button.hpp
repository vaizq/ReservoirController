#pragma once

#include <Arduino.h>
#include <FunctionalInterrupt.h>
#include <functional>
#include <chrono>


namespace Driver
{

class Button
{
    using Clock = std::chrono::steady_clock; 
public:
    enum class State
    {
        Pressed,
        Released
    };

    using Callback = std::function<void(State)>;

    Button(uint8_t pin)
    : mPin{pin} 
    {}

    void begin(Callback cb)
    {
        mCb = std::move(cb);
        pinMode(mPin, INPUT_PULLDOWN);
        attachInterrupt(digitalPinToInterrupt(mPin), 
        [this]() {
            ++mSem;
        }, 
        CHANGE);
    }

    State state()
    {
        return mState;
    }

    void loop()
    {
        using namespace std::chrono_literals;

        auto now = Clock::now();
        if (mSem && (now - mLastSwitch) > 50ms)
        {
            mSem = 0;
            mLastSwitch = now;
            mState = (mState == State::Released) ? State::Pressed : State::Released;
            mCb(mState);
        }
    }

private:
    Callback mCb;
    uint8_t mPin;
    int mSem{0};
    Clock::time_point mLastSwitch;
    State mState{State::Released};
};

}