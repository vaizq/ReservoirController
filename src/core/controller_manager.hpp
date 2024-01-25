#pragma once


#include "common.hpp"
#include <array>
#include <chrono>
#include <cstdint>
#include <variant>
#include <type_traits>


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


    template <typename ControllerT>
    [[nodiscard]] const ControllerT& get() const
    {
        static_assert(std::disjunction_v<std::is_same<ControllerT, Types>...>);
        for (const auto& c : mControllers)
        {
            if (auto*p = std::get_if<ControllerT>(&c))
            {
                return *p;
            }
        }
    }


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