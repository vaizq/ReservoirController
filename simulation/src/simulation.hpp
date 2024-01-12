#pragma once

#include "app_base.hpp"
#include "reservoir.hpp"
#include "relative_clock.hpp"
#include "core/ph_controller.hpp"
#include "core/ec_controller.hpp"
#include "core/liquid_level_controller.hpp"
#include "core/dosing_pump.hpp"
#include "./driver/liquid_level_sensor.hpp"
#include "./driver/valve.hpp"
#include "./driver/ph_sensor.hpp"
#include "./driver/ec_sensor.hpp"

#include <chrono>


using Doser = Core::DosingPump<Driver::Valve>;


struct Config
{
    float targetPH = 6.0f;
    float targetEC = 1.0f;
    float targetLevel = 200.0f;
    float waterFlowRate = 1.0f;
    float doserFlowRate = 0.001f;
};


static constexpr Core::Controller::Config phControllerConfig{5.8f, 6.2f, 0.01f, std::chrono::minutes(10)};
static constexpr Core::Controller::Config ecControllerConfig{1.0f, 1.2f, 0.1f, std::chrono::minutes(10)};
constexpr size_t nutrientPumpCount{3};
constexpr std::array<float, nutrientPumpCount> feedingSchedule = {1.0f, 2.0f, 3.0f}; // GHE three part


static Doser makeDoser(Reservoir& reservoir, float flowRate, Driver::Valve::LiquidSupply supply);
static std::array<Doser, nutrientPumpCount> makeNutrientDosers(Reservoir& reservoir, float flowRate, Driver::Valve::LiquidSupply supply);


class Simulation : public AppBase
{
    using Duration = std::chrono::duration<float>;
    using Clock = RelativeClock<std::chrono::steady_clock, float>;
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
    Core::ECController<Driver::EcSensor, Driver::Valve, 3> mEcController;
};


