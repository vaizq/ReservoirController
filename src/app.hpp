#pragma once

#include "controller.hpp"
#include "core/common.hpp"
#include <array>
#include <type_traits>
#include <Arduino.h>
#include <ezButton.h>
#include "pin_config.hpp"
#include <chrono>
#include <Wire.h>
#include <DFRobot_RGBLCD1602.h>


using namespace std::chrono_literals;



void printStatus(DFRobot_RGBLCD1602& lcd, LiquidLevelController& controller)
{
}

void printStatus(DFRobot_RGBLCD1602& lcd, PHController& controller)
{
    float ph = controller.ph();
    lcd.setCursor(0, 0);
    lcd.printf("PH %.1f", ph);
}

void printStatus(DFRobot_RGBLCD1602& lcd, ECController& controller)
{
    auto status = controller.getStatus();
    lcd.setCursor(0, 1);
    lcd.printf("EC %.1f ppm", status.ec);
}


template <size_t N>
class App
{
    using Clock = std::chrono::steady_clock;
public:
    App(std::array<Controller, N>&& controllers, DoserManager& dosers)
    : 
        mControllers(std::move(controllers)), 
        mDosers{dosers},
        mButton{Config::buttonPin, INPUT_PULLUP},
        mLcd{0x60, 16, 2}
    {
    }

    void init()
    {
        mLcd.init();
        mLcd.setBacklight(true);
        mLcd.print("Hello world!");
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
            Serial.println("Run dosers");
            mDosers.queueDose(1000.0f);
        }

        if (mButton.isReleased())
        {
            Serial.println("Stop dosers");
            mDosers.reset();
        }

        if (mFromStatusPrint > 500ms)
        {
            mFromStatusPrint = 0ms;

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
    void setTargetPh(float ph)
    {
        if (auto* phController = get<PHController>())
            phController->setTargetPh(ph);
    }

    void setTargetEc(float ec)
    {
        if (auto* ecController = get<ECController>())
            ecController->setTargetEc(ec);
    }

    template <typename T>
    T* get()
    {
        static_assert(std::is_same_v<T, PHController> || std::is_same_v<T, ECController> || std::is_same_v<T, LiquidLevelController>);
        for (auto& c : mControllers)
        {
            if (auto* p = std::get_if<T>(&c))
            {
                return p;
            }
        }
        return nullptr;
    }

    std::array<Controller, N> mControllers;
    DoserManager& mDosers;
    ezButton mButton;
    Duration mFromStatusPrint;
    DFRobot_RGBLCD1602 mLcd;
};