#include "core/controller_manager.hpp"
#include "core/dt_timer.hpp"
#include "config.hpp"
#include "net/serialize.hpp"
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <optional>
#include <DFRobot_RGBLCD1602.h>
#include <ezButton.h>


constexpr char ssid[] = "Zhone_9C8E";
constexpr char passwd[] = "znid309149582";
constexpr char broker[] = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client{espClient};

DFRobot_RGBLCD1602 lcd{0x60, 16, 2};

ezButton button{buttonPin, INPUT_PULLUP};

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

void callback(char* topic, byte* msg, unsigned int length)
{
    Serial.print("Topic: ");
    Serial.println(topic);

    msg[length] = 0;
    Serial.print("Message: ");
    Serial.println((char*)msg);
}

void reconnect()
{
    while (!client.connected())
    {
        if (client.connect("ReservoirController"))
        {
            Serial.println("Connected");
            client.subscribe("ReservoirController/config");
            break;
        }
        else
        {
            Serial.println("Connection failed");
        }
    }
}

void connectWifi()
{
    Serial.println("Connecting to wifi");
    WiFi.begin(ssid, passwd); 

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("Wifi connected");
    Serial.println(WiFi.localIP());
}

void sendTelemetry()
{
    auto& controllers = controllerManager.getControllers();
    for (auto& c : controllers)
    {
        std::visit([](auto& controller) {
            const auto& status = serializeStatus(controller.getStatus());
            const auto& config = serializeConfig(controller.getConfig());
            client.publish("ReservoirController/Status", status.dump().c_str());
            client.publish("ReservoirController/Config", config.dump().c_str());
        }, c);
    }
}

void displayStatus()
{
    auto& controllers = controllerManager.getControllers();
    for (auto& c : controllers)
    {
        std::visit([](auto& c) {
            using T = std::decay_t<decltype(c)>;
            if constexpr (std::is_same_v<T, PHController>)
            {
                lcd.setCursor(0, 0);
                lcd.printf("PH %.1f", c.getStatus().ph);
            }
            else if constexpr (std::is_same_v<T, ECController>)
            {
                lcd.setCursor(0, 1);
                lcd.printf("EC %.1f", c.getStatus().ec);
            }
            else if constexpr (std::is_same_v<T, LiquidLevelController>)
            {

            }
        }, c);
    }
   
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    lcd.init();
    lcd.setBacklight(true);

    connectWifi();
    client.setServer(broker, 1883);
    client.setCallback(callback);
    reconnect();
}

Core::DtTimer<> timer;

using Clock = std::chrono::steady_clock;
Clock::time_point prevTelemetry;
Clock::time_point prevDisplay;
DoserManager::DoserID currentID{0};

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

    const auto now = Clock::now();
    if (now - prevTelemetry > std::chrono::seconds(1))
    {
        prevTelemetry = now;
        sendTelemetry();
    }
    if (now - prevDisplay > std::chrono::milliseconds(100))
    {
        prevDisplay = now;
        displayStatus();
    }

    // Simple manual control for dosers
    button.loop();
    if (button.isPressed())
    {
        doserManager.reset();
        doserManager.queueDose(currentID, 1000.0f);
    }
    else if (button.isReleased())
    {
        doserManager.reset();
        currentID = (currentID + 1) % DoserManager::DoserCount;
    }

    const auto dt = timer.tick();
    controllerManager.update(dt);
    doserManager.update(dt);
    delay(1);
}