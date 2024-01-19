#pragma once

#include "controller.hpp"
#include "core/common.hpp"
#include <array>
#include <type_traits>
#include <Arduino.h>
#include <ezButton.h>
#include "pin_config.hpp"


using namespace std::chrono_literals;


constexpr std::array<float, 3> feedingSchedule = {1.0f, 2.0f, 3.0f}; // GHE 3 part

void printStatus(LiquidLevelController& controller)
{
    auto& status = controller.getStatus();
    Serial.print("Level is ");
    Serial.println(status.levelIsHigh ? "high" : "low");
    Serial.print("Valve is ");
    Serial.println(status.valveIsOpen ? "open" : "closed");
}

void printStatus(PHController& controller)
{
    auto& status = controller.getStatus();
    Serial.print("PH is ");
    Serial.println(status.ph);
    Serial.print("Pump is ");
    Serial.println(status.isDosing ? "on" : "off");
}

void printStatus(ECController& controller)
{
    auto& status = controller.getStatus();
    Serial.print("EC is ");
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
        auto* c = get<ECController>();
        c->setSchedule(feedingSchedule);
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
                if (auto* p = std::get_if<PHController>(c))
                {
                    controller.stop();
                    controller.openValves();
                }
            }
        }

        if (mButton.isReleased())
        {
            // Start controllers
            for (Controller& c : mControllers)
            {
                if (auto* p = std::get_if<PHController>(c))
                {
                    controller.closeValves();
                    controller.start();
                } 
            }
        }

        if (mFromStatusPrint > 1000ms)
        {
            mFromStatusPrint = 0ms;
            // Start controllers
            for (Controller& c : mControllers)
            {
                std::visit([](auto& controller)
                {
                    printStatus(controller);
                    Serial.println("-------");
                }, c);
                Serial.println("========");
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