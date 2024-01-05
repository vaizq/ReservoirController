#include "liquid_level/liquid_level_controller.hpp"
#include "platform/proto/liquid_level_sensor.hpp"
#include "platform/proto/valve.hpp"
#include "cultimatic.hpp"

constexpr gpio_num_t LL_SENSOR_PIN = GPIO_NUM_32;
constexpr gpio_num_t VALVE_PIN = GPIO_NUM_22;


// If platform is ESP32, define alias for liquid level controller

using LLController = 
#ifdef ESP_PLATFORM
    LiquidLevelController<Proto::LiquidLevelSensor, Proto::Valve>;
#elif defined(ARDUINO)

#endif


extern "C" void app_main(void)
{
    LLController controller{Proto::LiquidLevelSensor{LL_SENSOR_PIN}, Proto::Valve{VALVE_PIN}};

    while (true)
    {
        controller.update();
        Cultimatic::delay(100ms);
    }
}