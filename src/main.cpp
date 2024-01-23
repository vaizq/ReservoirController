#include "app.hpp"
#include "controller.hpp"
#include "core/dt_timer.hpp"
#include "config.hpp"
#include <Arduino.h>


constexpr DoserManager::DoserID phDown{0};
constexpr DoserManager::DoserID grow{1};
constexpr DoserManager::DoserID micro{2};
constexpr DoserManager::DoserID bloom{3};

constexpr float flowRate = 1.0f;

constexpr ECController::NutrientSchedule ghe3part = {
    std::make_pair<int, float>(grow, 1.0f), 
    std::make_pair<int, float>(micro, 2.0f), 
    std::make_pair<int, float>(bloom, 3.0f)
};

std::array<DosingPump, 4> dosers{
    DosingPump{Pump{Config::doserDefs[0]}, flowRate}, 
    DosingPump{Pump{Config::doserDefs[1]}, flowRate},
    DosingPump{Pump{Config::doserDefs[2]}, flowRate},
    DosingPump{Pump{Config::doserDefs[3]}, flowRate}
};

DoserManager doserManager{std::move(dosers), 1};

std::array<Controller, 3> controllers = {
    LiquidLevelController{ 
        Driver::LiquidLevelSensor{Config::liquidLevelTopSensorPin}, 
        Driver::SolenoidValve{Config::valveSwitchPin}
    },
    PHController{
        Driver::DFRobotV2PHSensor{Config::phSensorPin}, 
        doserManager,
        0
    },
    ECController{
        Driver::ECSensor{Config::ecSensorPin},
        doserManager,
        ECController::Config{
            .target = 1.0f, 
            .dosingAmount = 10.0f, 
            .dosingInterval = std::chrono::seconds(60), 
            .schedule = ghe3part
        } 
    }
};

App<3> app{std::move(controllers), doserManager};

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

