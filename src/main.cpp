#include "core/controller_manager.hpp"
#include "core/dt_timer.hpp"
#include "config.hpp"
#include <Arduino.h>


// Construct dosers, DoserManager and Controllers. Then pass those to application

/*

void printStatus(DFRobot_RGBLCD1602& lcd, LiquidLevelController& controller)
{
}

void printStatus(DFRobot_RGBLCD1602& lcd, PHController& controller)
{
    auto& status = controller.getStatus();
    lcd.setCursor(0, 0);
    lcd.printf("PH %.1f", status.ph);
}

void printStatus(DFRobot_RGBLCD1602& lcd, ECController& controller)
{
    auto status = controller.getStatus();
    lcd.setCursor(0, 1);
    lcd.printf("EC %.1f ppm", status.ec);
}

*/

constexpr DoserManager::DoserID phDownDoser{0};
constexpr DoserManager::DoserID growDoser{1};
constexpr DoserManager::DoserID microDoser{2};
constexpr DoserManager::DoserID bloomDoser{3};

constexpr float flowRate = 1.0f;
constexpr int parallelLimit = 1;

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

DoserManager doserManager{
    {
        Doser{Valve{doserDefs[0]}, flowRate}, 
        Doser{Valve{doserDefs[1]}, flowRate},
        Doser{Valve{doserDefs[2]}, flowRate},
        Doser{Valve{doserDefs[3]}, flowRate}
    }, 
    parallelLimit
};

ControllerManager controllerManager{
    PHController{
        Driver::DFRobotV2PHSensor{phSensorPin}, 
        doserManager,
        phDownDoser 
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
    },
    LiquidLevelController{ 
        Driver::LiquidLevelSensor{liquidLevelTopSensorPin}, 
        Driver::SolenoidValve{valveSwitchPin}
    }
};

void setup()
{
    Serial.begin(115200);
}

Core::DtTimer<> timer;

void loop()
{
    const auto dt = timer.tick();
    controllerManager.update(dt);
    doserManager.update(dt);
    delay(1);
}