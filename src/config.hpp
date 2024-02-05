#pragma once

#include "core/liquid_level_controller.hpp"
#include "core/ph_controller.hpp"
#include "core/ec_controller.hpp"
#include "core/doser_manager.hpp"
#include "drivers/solenoid_valve.hpp"
#include "drivers/AnalogSensor.h"
#include "drivers/DigitalSensor.h"
#include "drivers/tb6612fng_valve.hpp"
#include <array>
#include <cstdint>
#include <variant>


constexpr size_t totalDoserCount{4};
constexpr size_t nutrientDoserCount{3};

// Core device types
using Valve = Driver::TB6612FNGValve;
using Doser = Core::Doser<Valve>;
using DoserManager = Core::DoserManager<Valve, totalDoserCount>;
using LiquidLevelController = Core::LiquidLevelController<Driver::DigitalSensor, Driver::SolenoidValve>;
using PHController = Core::PHController<Driver::AnalogSensor, Valve, DoserManager::DoserCount>;
using ECController = Core::ECController<Driver::AnalogSensor, Valve, DoserManager::DoserCount, nutrientDoserCount>;


// Pin definitions
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


// Application constants
constexpr float flowRate = 1.0f;
constexpr int parallelDosersLimit = 1;

constexpr DoserManager::DoserID phDownDoser{0};
constexpr DoserManager::DoserID growDoser{1};
constexpr DoserManager::DoserID microDoser{2};
constexpr DoserManager::DoserID bloomDoser{3};

constexpr ECController::NutrientSchedule ghe3part = {
    std::make_pair<int, float>(growDoser, 1.0f), 
    std::make_pair<int, float>(microDoser, 2.0f), 
    std::make_pair<int, float>(bloomDoser, 3.0f)
};
