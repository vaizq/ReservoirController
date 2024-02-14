#include "core/dt_timer.hpp"
#include "app.hpp"
#include <Arduino.h>
#include <EEPROM.h>

App* app;


void setup()
{
    Serial.begin(115200);
    EEPROM.begin(512);
    delay(1000);

    app = new App();
    app->setup();
}

Core::DtTimer<> timer;

void loop()
{
    const auto dt = timer.tick();
    app->update(dt);
    delay(1);
}