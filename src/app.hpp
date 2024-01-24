#pragma once

#include "config.hpp"
#include "core/common.hpp"
#include <array>
#include <type_traits>
#include <Arduino.h>
#include <ezButton.h>
#include <chrono>
#include <Wire.h>
#include <DFRobot_RGBLCD1602.h>


void printStatus(DFRobot_RGBLCD1602& lcd, LiquidLevelController& controller)
{
}

void printStatus(DFRobot_RGBLCD1602& lcd, PHController& controller)
{
    auto& status = controller.getStatus();
    lcd.setCursor(0, 0);
    lcd.printf("PH %.1f", status.ph);
}

void printStatus(DFRobot_RGBLCD1602& lcd, ECController& controller)
{
    auto status = controller.getStatus();
    lcd.setCursor(0, 1);
    lcd.printf("EC %.1f ppm", status.ec);
}


class App
{
    using Clock = std::chrono::steady_clock;
    using ControllerArray = std::array<Controller, 3>;
public:
    App(ControllerArray&& controllers, DoserManager& dosers)
    : 
        mControllers(std::move(controllers)), 
        mDosers{dosers},
        mButton{buttonPin, INPUT_PULLUP},
        mLcd{0x60, 16, 2}
    {
    }

    void initLCD()
    {
        mLcd.init();
        mLcd.setBacklight(true);
    }

    void update(const Duration dt)
    {
        mDosers.update(dt);

        for (Controller& c: mControllers)
        {
            std::visit([dt](auto& controller) 
            {
                controller.update(dt);
            }, c);
        }

        mButton.loop();

        if (mButton.isPressed())
        {
            mDosers.queueDose(mID, 1000.0f);
        }

        if (mButton.isReleased())
        {
            mDosers.reset();
            mID = (mID + 1) % DoserManager::DoserCount;
        }

        if (mFromStatusPrint > std::chrono::milliseconds(200))
        {
            mFromStatusPrint = Duration{};

            mLcd.clear();
            mLcd.setCursor(0, 0);

            for (Controller& c : mControllers)
            {
                std::visit([this](auto& controller) 
                {
                    printStatus(mLcd, controller);
                }, c);
            }
        }
        mFromStatusPrint += dt;
    }

private:
    ControllerArray mControllers;
    DoserManager& mDosers;
    ezButton mButton;
    Duration mFromStatusPrint{0};
    DFRobot_RGBLCD1602 mLcd;
    DoserManager::DoserID mID{0};
};