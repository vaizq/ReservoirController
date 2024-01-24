#pragma once


#include "common.hpp"
#include <array>
#include <chrono>
#include <cstdint>
#include <variant>


namespace Core
{

template <typename... Types>
// requires Types to be controllers
class ControllerManager 
{
public:
    using Controller = std::variant<Types...>;

    ControllerManager(Types... args)
    :   mControllers{std::forward<Types>(args)...} 
    {}

    void update(const Duration dt)
    {
        for (auto& c : mControllers)
        {
            std::visit([dt](auto& controller) 
            {
                controller.update(dt);
            }, c);
        }
    }

private:
    std::array<Controller, std::variant_size_v<Controller>> mControllers;
};

}