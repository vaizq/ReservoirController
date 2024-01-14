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
constexpr std::array<float, 3> feedingSchedule = {1.0f, 2.0f, 3.0f}; // GHE 3 part


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

    Core::DtTimer<std::chrono::steady_clock> dtTimer;

    while (true)
    {
        const auto dt = dtTimer.tick();

        llController.update(dt);
        phController.update(dt);
        ecController.update(dt);

        Cultimatic::delay(100ms);
    }
}