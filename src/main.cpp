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
#include <delay.hpp>
#include <Arduino.h>
#include <ezButton.h>
#include <Wire.h>
#include <optional>


constexpr float flowRate = 1.0f;
constexpr ECController::NutrientSchedule feedingSchedule = {std::make_pair<int, float>(1, 1.0f), std::make_pair<int, float>(2, 2.0f), std::make_pair<int, float>(3, 3.0f)}; // GHE 3 part


std::array<DosingPump, 4> dosers{
    DosingPump{Pump{Config::doserDefs[0]}, flowRate}, 
    DosingPump{Pump{Config::doserDefs[1]}, flowRate},
    DosingPump{Pump{Config::doserDefs[2]}, flowRate},
    DosingPump{Pump{Config::doserDefs[3]}, flowRate}
};


DoserManager doserManager{std::move(dosers), 1};


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


App<3> app{std::move(controllers), doserManager};

void setup()
{
    Serial.begin(115200);
    app.init();
}

Core::DtTimer<> timer;

void loop()
{
    const auto dt = timer.tick();
    app.update(dt);
    delay(1);
}

