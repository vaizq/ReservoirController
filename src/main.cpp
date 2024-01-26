#include "core/controller_manager.hpp"
#include "core/dt_timer.hpp"
#include "config.hpp"
#include "net/serialize.hpp"
#include "app.hpp"
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <optional>
#include <DFRobot_RGBLCD1602.h>
#include <ezButton.h>


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