#pragma once

#include "controller.hpp"
#include "core/common.hpp"
#include <array>
#include <type_traits>


template <size_t N>
class App
{
public:
    App(std::array<Controller, N>&& controllers)
    : mControllers(std::move(controllers))
    {}

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
        if (auto* phController = get<PhController>())
            phController->setTargetPh(ph);
    }

    void setTargetEc(float ec)
    {
        if (auto* ecController = get<EcController>())
            ecController->setTargetEc(ec);
    }

    template <typename T>
    T* get()
    {
        static_assert(std::is_same_v<T, PhController>() || std::is_same_v<T, EcController>() || std::is_same_v<T, LiquidLevelController>());
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