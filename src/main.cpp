#include "core/controller_manager.hpp"
#include "core/dt_timer.hpp"
#include "app.hpp"
#include <Arduino.h>

App app;

void setup()
{
    Serial.begin(115200);
    delay(1000);

    app.setup();
}

Core::DtTimer<> timer;

void loop()
{
    const auto dt = timer.tick();
    app.update(dt);

    delay(1);
}