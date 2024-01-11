#pragma once

#include "app_base.hpp"
#include "reservoir.hpp"
#include "relative_clock.hpp"
#include "dt_timer.hpp"
#include "do_timer.hpp"
#include "core/ph_controller.hpp"
#include "core/ec_controller.hpp"
#include "core/liquid_level_controller.hpp"
#include "core/dosing_pump.hpp"
#include "./driver/liquid_level_sensor.hpp"
#include "./driver/valve.hpp"
#include "./driver/ph_sensor.hpp"

#include <chrono>



struct Config
{
    float targetPH = 6.0f;
    float targetEC = 1.0f;
    float targetLevel = 200.0f;
    float waterFlowRate_L = 1.0f;
    float phFlowRate_mL = 1.0f;
};


constexpr std::array<float, 3> nutrientSchedule = {1.0f, 2.0f, 3.0f}; // GHE three part


class Simulation : public AppBase
{
    using Duration = std::chrono::duration<float>;
    using Clock = RelativeClock<std::chrono::steady_clock, float>;
    using Doser = Core::DosingPump<Driver::Valve>;
public:
    static Simulation& instance();
    Simulation(const Simulation&) = delete;
    Simulation& operator=(const Simulation&) = delete;
    Simulation(Simulation&&) = delete;
    Simulation& operator=(Simulation&&) = delete;

private:
    Simulation();
    bool update() override;
    void updateControllers(const Duration dt);
    void updateGui(const Duration dt);

private:
    GLFWwindow* mWindow;
    Config mConfig{};
    Clock mClock;
    Reservoir mReservoir{250.0f};
    Core::LiquidLevelController<Driver::LiquidLevelSensor, Driver::Valve> mLLController;
    Core::PhController<Driver::PhSensor, Driver::Valve> mPhController;
};


static constexpr Core::Controller::Config phControllerConfig();