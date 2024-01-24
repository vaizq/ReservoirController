#pragma once

#include "core/liquid_level_controller.hpp"
#include "core/ph_controller.hpp"
#include "core/ec_controller.hpp"
#include "core/doser_manager.hpp"
#include "drivers/liquid_level_sensor.hpp"
#include "drivers/solenoid_valve.hpp"
#include "drivers/dfrobot_ph_sensor_v2.hpp"
#include "drivers/tb6612fng_valve.hpp"
#include "drivers/ec_sensor.hpp"
#include <array>
#include <cstdint>
#include <variant>


using Valve = Driver::TB6612FNGValve;

using Doser = Core::Doser<Valve>;

using DoserManager = Core::DoserManager<Valve, 4>;

using LiquidLevelController = Core::LiquidLevelController<Driver::LiquidLevelSensor, Driver::SolenoidValve>;

using PHController = Core::PHController<Driver::DFRobotV2PHSensor, Valve, DoserManager::DoserCount>;

using ECController = Core::ECController<Driver::ECSensor, Valve, DoserManager::DoserCount, 3>;

using Controller = std::variant<PHController, ECController, LiquidLevelController>;


constexpr std::array<Driver::TB6612FNGValve::PinDef, 4> doserDefs = 
    {
        Driver::TB6612FNGValve::PinDef{.xIN1 = 4, .xIN2 = 5, .PWMx = 23},
        Driver::TB6612FNGValve::PinDef{.xIN1 = 19, .xIN2 = 18, .PWMx = 26},
        Driver::TB6612FNGValve::PinDef{.xIN1 = 13, .xIN2 = 12, .PWMx = 14},
        Driver::TB6612FNGValve::PinDef{.xIN1 = 16, .xIN2 = 15, .PWMx = 17}
    };

constexpr uint8_t valveSwitchPin = 27;
constexpr uint8_t liquidLevelTopSensorPin = 32;
constexpr uint8_t liquidLevelBottomSensorPin = 35;
constexpr uint8_t phSensorPin = 34;
constexpr uint8_t ecSensorPin = 36;
constexpr uint8_t buttonPin = 33;