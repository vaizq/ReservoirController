#include "core/dt_timer.hpp"
#include "core/liquid_level_controller.hpp"
#include "core/ph_controller.hpp"
#include "core/ec_controller.hpp"
#include "platform/proto/liquid_level_sensor.hpp"
#include "platform/proto/valve.hpp"
#include "platform/proto/dfrobot_ph_sensor_v2.hpp"
#include "platform/proto/tb6612fng_pump.hpp"
#include "platform/proto/ec_sensor.hpp"
#include "pin_config.hpp"
#include <delay.hpp>


// If platform is ESP32, define alias for liquid level controller

using Pump =
#ifdef ESP_PLATFORM
    Proto::TB6612FNGPump;
#endif

using DosingPump = 
#ifdef ESP_PLATFORM
    Core::DosingPump<Pump>;
#endif

using LiquidLevelController = 
#ifdef ESP_PLATFORM
    Core::LiquidLevelController<Proto::LiquidLevelSensor, Proto::Valve>;
#endif

using PhController = 
#ifdef ESP_PLATFORM
    Core::PhController<Proto::PhSensor, DosingPump>;
#endif

using EcController = 
#ifdef ESP_PLATFORM
    Core::ECController<Proto::ECSensor, DosingPump, 2>;
#endif


constexpr float flowRate = 1.0f;
constexpr unsigned int nutrientPumpCount = 2;
constexpr std::array<float, 2> floweringSchedule = {1.0f, 2.0f}; // Lucas formula


void configure(LiquidLevelController& llController)
{
}

void configure(PhController& phController)
{
    phController.setConfig(PhController::Config{.phTarget = 5.8, .doseAmount = 1.0f});
}

void configure(EcController& ecController)
{
    ecController.setConfig(EcController::Config{.targetEC = 1.8f, .doseAmount = 10.0f});
    ecController.setSchedule(floweringSchedule);
}


extern "C" void app_main(void)
{
    LiquidLevelController llController {
        Proto::LiquidLevelSensor{Config::liquidLevelTopSensorPin}, 
        Proto::Valve{Config::valveSwitchPin}
    };

    PhController phController {
        Proto::PhSensor{Config::phSensorPin}, 
        DosingPump{Pump{Config::phDownPumpDef}, flowRate}, 
        DosingPump{Pump{Config::phDownPumpDef}, flowRate}
    };

    EcController ecController {
        Proto::ECSensor{Config::ecSensorPin},
        {
            DosingPump{Pump{Config::nutrientPumpDefs[0]}, flowRate}, 
            DosingPump{Pump{Config::nutrientPumpDefs[1]}, flowRate}
        }
    };

    configure(llController);
    configure(phController);
    configure(ecController);

    DtTimer dtTimer;

    while (true)
    {
        const auto dt = dtTimer.tick();

        llController.update(dt);
        phController.update(dt);
        ecController.update(dt);

        Cultimatic::delay(100ms);
    }
}