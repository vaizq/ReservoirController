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


constexpr float flowRate = 1.0f;


std::array<Controller, 3> controllers = {
    LiquidLevelController{ 
        Driver::LiquidLevelSensor{Config::liquidLevelTopSensorPin}, 
        Driver::SolenoidValve{Config::valveSwitchPin}
    },
    PHController{
        Driver::DFRobotV2PHSensor{Config::phSensorPin}, 
        DosingPump{Pump{Config::phDownPumpDef}, flowRate}
    },
    ECController{
        Driver::ECSensor{Config::ecSensorPin},
        {
            DosingPump{Pump{Config::nutrientPumpDefs[0]}, flowRate}, 
            DosingPump{Pump{Config::nutrientPumpDefs[1]}, flowRate},
            DosingPump{Pump{Config::nutrientPumpDefs[2]}, flowRate}
        }
    }
};

App<3> app{std::move(controllers)};


void setup()
{
    Serial.begin(115200);
    pinMode(33, INPUT_PULLUP);
}

Core::DtTimer<> timer;
void loop()
{
    app.update(timer.tick());
    Cultimatic::delay(1ms);
}