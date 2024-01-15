#pragma once

#include "controller.hpp"
#include "core/common.hpp"
#include <array>


template <size_t N>
class App
{
public:
    App(std::array<Controller, N>&& controllers)
    : mControllers(std::move(controllers))
    {}

    void loop(const Duration dt)
    {
        for (Controller& c: mControllers)
        {
            std::visit([dt](auto& controller) 
            {
                controller.update(dt);
            }, c);
        }

        // Handle received rpc's
    }
private:
    std::array<Controller, N> mControllers;
};