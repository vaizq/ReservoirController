#include "read_voltage.hpp"
#include <Arduino.h>



float readVoltage(uint8_t pin)
{
#ifdef ESP_PLATFORM
    return analogRead(pin) * 3.3f / 4096.0f;
#elif defined(ARDUINO)
    return analogRead(pin) * 5.0f / 1024.0f;
#endif
}