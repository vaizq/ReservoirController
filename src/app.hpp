#pragma once

#include "controller.hpp"
#include "core/common.hpp"
#include <array>
#include <type_traits>


constexpr std::array<float, 3> feedingSchedule = {1.0f, 2.0f, 3.0f}; // GHE 3 part


template <size_t N>
class App
{
public:
    App(std::array<Controller, N>&& controllers)
    : mControllers(std::move(controllers))
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

            // Send telemetry

            // Receive RPC's
        }
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
};