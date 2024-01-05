#include "liquid_level/liquid_level_controller.hpp"
#include "ph/ph_controller.hpp"
#include "platform/proto/liquid_level_sensor.hpp"
#include "platform/proto/valve.hpp"
#include "platform/proto/dosing_pump.hpp"
#include "platform/proto/dfrobot_ph_sensor_v2.hpp"
#include "platform/proto/tb6612fng_pump.hpp"
#include "pin_config.hpp"
#include <delay.hpp>


// If platform is ESP32, define alias for liquid level controller

using LiquidLevelController = 
#ifdef ESP_PLATFORM
    Device::LiquidLevelController<Proto::LiquidLevelSensor, Proto::Valve>;
#elif defined(ARDUINO)

#endif

using PhController = 
#ifdef ESP_PLATFORM
    Device::PhController<Proto::PhSensor, Proto::DosingPump<Proto::TB6612FNGPump>>;
#elif defined(ARDUINO)

#endif



extern "C" void app_main(void)
{
    LiquidLevelController controller{Proto::LiquidLevelSensor{Config::liquidLevelTopSensorPin}, Proto::Valve{Config::valveSwitchPin}};

    while (true)
    {
        controller.update();
        Cultimatic::delay(100ms);
    }
}