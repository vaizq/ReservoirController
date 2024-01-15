#pragma once

#include "core/liquid_level_controller.hpp"
#include "core/ph_controller.hpp"
#include "core/ec_controller.hpp"
#include "driver/liquid_level_sensor.hpp"
#include "driver/valve.hpp"
#include "driver/dfrobot_ph_sensor_v2.hpp"
#include "driver/tb6612fng_valve.hpp"
#include "driver/ec_sensor.hpp"
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
    Core::LiquidLevelController<Driver::LiquidLevelSensor, Driver::Valve>;
#endif

using PhController = 
#ifdef CULTIMATICS_V_0_1
    Core::PhController<Driver::PhSensor, Pump>;
#endif

using EcController = 
#ifdef CULTIMATICS_V_0_1
    Core::ECController<Driver::ECSensor, Pump, 3>;
#endif


using Controller = std::variant<PhController, EcController, LiquidLevelController>;