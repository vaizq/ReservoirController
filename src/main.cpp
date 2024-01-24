#include "app.hpp"
#include "core/dt_timer.hpp"
#include "config.hpp"
#include <Arduino.h>


// Construct DoserManager and Controllers and pass those to App


constexpr DoserManager::DoserID phDownDoser{0};
constexpr DoserManager::DoserID growDoser{1};
constexpr DoserManager::DoserID microDoser{2};
constexpr DoserManager::DoserID bloomDoser{3};

constexpr float flowRate = 1.0f;

constexpr ECController::NutrientSchedule ghe3part = {
    std::make_pair<int, float>(growDoser, 1.0f), 
    std::make_pair<int, float>(microDoser, 2.0f), 
    std::make_pair<int, float>(bloomDoser, 3.0f)
};

std::array<Doser, 4> dosers{
    Doser{Valve{doserDefs[0]}, flowRate}, 
    Doser{Valve{doserDefs[1]}, flowRate},
    Doser{Valve{doserDefs[2]}, flowRate},
    Doser{Valve{doserDefs[3]}, flowRate}
};

DoserManager doserManager{std::move(dosers), 1};

std::array<Controller, 3> controllers = {
    LiquidLevelController{ 
        Driver::LiquidLevelSensor{liquidLevelTopSensorPin}, 
        Driver::SolenoidValve{valveSwitchPin}
    },
    PHController{
        Driver::DFRobotV2PHSensor{phSensorPin}, 
        doserManager,
        0
    },
    ECController{
        Driver::ECSensor{ecSensorPin},
        doserManager,
        ECController::Config{
            .target = 1.0f, 
            .dosingAmount = 10.0f, 
            .dosingInterval = std::chrono::seconds(60), 
            .schedule = ghe3part
        } 
    }
};

App app{std::move(controllers), doserManager};

void setup()
{
    Serial.begin(115200);
    app.initLCD();
}

Core::DtTimer<> timer;

void loop()
{
    const auto dt = timer.tick();
    app.update(dt);
    delay(1);
}