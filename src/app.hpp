#pragma once

#include "controller.hpp"
#include "core/common.hpp"
#include <array>
#include <type_traits>
#include <Arduino.h>
#include <ezButton.h>
#include "pin_config.hpp"
#include <chrono>


using namespace std::chrono_literals;



void printStatus(LiquidLevelController& controller)
{
}

void printStatus(PHController& controller)
{
    float ph = controller.ph();
    Serial.print("PH: ");
    Serial.println(ph);
}

void printStatus(ECController& controller)
{
    auto status = controller.getStatus();
    Serial.print("EC: ");
    Serial.println(status.ec);
}


template <size_t N>
class App
{
    using Clock = std::chrono::steady_clock;
public:
    App(std::array<Controller, N>&& controllers)
    : mControllers(std::move(controllers)), mButton{Config::buttonPin, INPUT_PULLUP}
    {
    }

    void update(const Duration dt)
    {
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
            for (Controller& c : mControllers)
            {
                if (auto* p = std::get_if<PHController>(&c))
                {
                    p->stop();
                }
            }
        }

        if (mButton.isReleased())
        {
            // Start controllers
            for (Controller& c : mControllers)
            {
                if (auto* p = std::get_if<PHController>(&c))
                {
                    p->start();
                } 
            }
        }

        if (mFromStatusPrint > 1000ms)
        {
            mFromStatusPrint = 0ms;

            for (Controller& c : mControllers)
            {
                std::visit([](auto& controller) 
                {
                    printStatus(controller);
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
    ezButton mButton;
    Duration mFromStatusPrint;
};