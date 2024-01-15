#include "core/dt_timer.hpp"
#include "core/liquid_level_controller.hpp"
#include "core/ph_controller.hpp"
#include "core/ec_controller.hpp"
#include "driver/liquid_level_sensor.hpp"
#include "driver/valve.hpp"
#include "driver/dfrobot_ph_sensor_v2.hpp"
#include "driver/tb6612fng_valve.hpp"
#include "driver/ec_sensor.hpp"
#include "pin_config.hpp"
#include "controller.hpp"
#include "app.hpp"
#include <delay.hpp>


constexpr float flowRate = 1.0f;
constexpr std::array<float, 3> feedingSchedule = {1.0f, 2.0f, 3.0f}; // GHE 3 part


static const char* TAG = "main";


void configure(LiquidLevelController& llController)
{
}

void configure(PhController& phController)
{
}

void configure(EcController& ecController)
{
    ecController.setSchedule(feedingSchedule);
}


extern "C" void app_main(void)
{
    std::array<Controller, 3> controllers = {
        LiquidLevelController{ 
            Driver::LiquidLevelSensor{Config::liquidLevelTopSensorPin}, 
            Driver::Valve{Config::valveSwitchPin}
        },
        PhController{
            Driver::PhSensor{Config::phSensorPin}, 
            DosingPump{Pump{Config::phDownPumpDef}, flowRate}
        },
        EcController{
            Driver::ECSensor{Config::ecSensorPin},
            {
                DosingPump{Pump{Config::nutrientPumpDefs[0]}, flowRate}, 
                DosingPump{Pump{Config::nutrientPumpDefs[1]}, flowRate},
                DosingPump{Pump{Config::nutrientPumpDefs[2]}, flowRate}
            }
        }
    };

    for (auto& controller : controllers)
    {
        std::visit([](auto& c) { configure(c); }, controller);
    }

    App<3> app{std::move(controllers)};

    Core::DtTimer<std::chrono::steady_clock> timer;

    while (true)
    {
        app.update(timer.tick());
        Cultimatic::delay(1ms);
    }
}