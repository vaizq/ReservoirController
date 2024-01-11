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
#include <delay.hpp>


constexpr float flowRate = 1.0f;
constexpr std::array<float, 3> floweringSchedule = {1.0f, 2.0f, 3.0f}; // GHE 3 part


// You can select drivers at compile time based on definitions for example

using Pump =
#ifdef ESP_PLATFORM
    Driver::TB6612FNGValve;
#endif

using DosingPump = 
#ifdef ESP_PLATFORM
    Core::DosingPump<Pump>;
#endif

using LiquidLevelController = 
#ifdef ESP_PLATFORM
    Core::LiquidLevelController<Driver::LiquidLevelSensor, Driver::Valve>;
#endif

using PhController = 
#ifdef ESP_PLATFORM
    Core::PhController<Driver::PhSensor, Pump>;
#endif

using EcController = 
#ifdef ESP_PLATFORM
    Core::ECController<Driver::ECSensor, Pump, 3>;
#endif


void configure(LiquidLevelController& llController)
{
}

void configure(PhController& phController)
{
}

void configure(EcController& ecController)
{
    ecController.setSchedule(floweringSchedule);
}


extern "C" void app_main(void)
{
    LiquidLevelController llController {
        Driver::LiquidLevelSensor{Config::liquidLevelTopSensorPin}, 
        Driver::Valve{Config::valveSwitchPin}
    };

    PhController phController {
        Driver::PhSensor{Config::phSensorPin}, 
        DosingPump{Pump{Config::phDownPumpDef}, flowRate}
    };

    EcController ecController {
        Driver::ECSensor{Config::ecSensorPin},
        {
            DosingPump{Pump{Config::nutrientPumpDefs[0]}, flowRate}, 
            DosingPump{Pump{Config::nutrientPumpDefs[1]}, flowRate},
            DosingPump{Pump{Config::nutrientPumpDefs[2]}, flowRate}
        }
    };

    configure(llController);
    configure(phController);
    configure(ecController);

    Core::DtTimer<Clock> dtTimer;

    while (true)
    {
        const auto dt = dtTimer.tick();

        llController.update(dt);
        phController.update(dt);
        ecController.update(dt);

        Cultimatic::delay(100ms);
    }
}