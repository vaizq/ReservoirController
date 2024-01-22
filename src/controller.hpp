#pragma once

#include "core/liquid_level_controller.hpp"
#include "core/ph_controller.hpp"
#include "core/ec_controller.hpp"
#include "driver/liquid_level_sensor.hpp"
#include "driver/solenoid_valve.hpp"
#include "driver/dfrobot_ph_sensor_v2.hpp"
#include "driver/tb6612fng_valve.hpp"
#include "driver/ec_sensor.hpp"
#include "pin_config.hpp"
#include <variant>


// Select drivers at compile time based on definition
#define CULTIMATICS_V_0_1

using Pump =
#ifdef CULTIMATICS_V_0_1 
    Driver::TB6612FNGValve;
#elif
    Driver::Valve;
#endif

using DosingPump = 
#ifdef CULTIMATICS_V_0_1
    Core::DosingPump<Pump>;
#endif

using LiquidLevelController = 
#ifdef CULTIMATICS_V_0_1
    Core::LiquidLevelController<Driver::LiquidLevelSensor, Driver::SolenoidValve>;
#endif

using PHController = 
#ifdef CULTIMATICS_V_0_1
    Core::PHController<Driver::DFRobotV2PHSensor, Pump, Config::PumpCount>;
#endif

using ECController = 
#ifdef CULTIMATICS_V_0_1
    Core::ECController<Driver::ECSensor, Pump, 3>;
#endif


using Controller = std::variant<PHController, ECController, LiquidLevelController>;