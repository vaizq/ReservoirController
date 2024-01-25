#include "core/controller_manager.hpp"
#include "core/dt_timer.hpp"
#include "config.hpp"
#include "net/serialize.hpp"
#include <Arduino.h>



DoserManager doserManager{
    {
        Doser{Valve{doserDefs[0]}, flowRate}, 
        Doser{Valve{doserDefs[1]}, flowRate},
        Doser{Valve{doserDefs[2]}, flowRate},
        Doser{Valve{doserDefs[3]}, flowRate}
    }, 
    parallelDosersLimit
};

ControllerManager controllerManager{
    PHController{
        Driver::DFRobotV2PHSensor{phSensorPin}, 
        doserManager,
        phDownDoser,
        PHController::Config{
            .target = 6.0f,
            .dosingAmount = 1.0f,
            .dosingInterval = std::chrono::seconds(60),
        } 
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
        Driver::SolenoidValve{valveSwitchPin},
        LiquidLevelController::Config{
            .refillInterval = std::chrono::seconds(60)
        }
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