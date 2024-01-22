#include "core/dt_timer.hpp"
#include "core/liquid_level_controller.hpp"
#include "core/ph_controller.hpp"
#include "core/ec_controller.hpp"
#include "driver/liquid_level_sensor.hpp"
#include "driver/solenoid_valve.hpp"
#include "driver/dfrobot_ph_sensor_v2.hpp"
#include "driver/tb6612fng_valve.hpp"
#include "driver/ec_sensor.hpp"
#include "pin_config.hpp"
#include "controller.hpp"
#include "app.hpp"
#include "core/doser_manager.hpp"
#include <delay.hpp>
#include <Arduino.h>
#include <ezButton.h>
#include <Wire.h>
#include <DFRobot_RGBLCD1602.h>
#include <optional>

constexpr float flowRate = 1.0f;
constexpr ECController::NutrientSchedule feedingSchedule = {std::make_pair<int, float>(1, 1.0f), std::make_pair<int, float>(2, 2.0f), std::make_pair<int, float>(3, 3.0f)}; // GHE 3 part

std::array<DosingPump, 4> dosers{
    DosingPump{Pump{Config::nutrientPumpDefs[0]}, flowRate}, 
    DosingPump{Pump{Config::nutrientPumpDefs[1]}, flowRate},
    DosingPump{Pump{Config::nutrientPumpDefs[2]}, flowRate},
    DosingPump{Pump{Config::phDownPumpDef}, flowRate}
};


DoserManager<Pump, 4> doserManager{std::move(dosers), 1};


std::array<Controller, 3> controllers = {
    LiquidLevelController{ 
        Driver::LiquidLevelSensor{Config::liquidLevelTopSensorPin}, 
        Driver::SolenoidValve{Config::valveSwitchPin}
    },
    PHController{
        Driver::DFRobotV2PHSensor{Config::phSensorPin}, 
        doserManager,
        0
    },
    ECController{
        Driver::ECSensor{Config::ecSensorPin},
        doserManager,
        feedingSchedule
    }
};


App<3> app{std::move(controllers)};

void setup()
{
    Serial.begin(115200);
}

Core::DtTimer<> timer;

void loop()
{
    const auto dt = timer.tick();
    app.update(dt);
    doserManager.update(dt);
    delay(1);
}

