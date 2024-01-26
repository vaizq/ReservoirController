#include "app.hpp"
#include <chrono>


using namespace std::chrono_literals;


App::App()
: 
    mDosers {
        {
            Doser{Valve{doserDefs[0]}, flowRate}, 
            Doser{Valve{doserDefs[1]}, flowRate},
            Doser{Valve{doserDefs[2]}, flowRate},
            Doser{Valve{doserDefs[3]}, flowRate}
        }, 
        parallelDosersLimit
    },
    mPHController {
        Driver::DFRobotV2PHSensor{phSensorPin}, 
        mDosers,
        phDownDoser,
        PHController::Config{
            .target = 6.0f,
            .dosingAmount = 1.0f,
            .dosingInterval = std::chrono::seconds(60),
        } 
    },
    mECController {
        Driver::ECSensor{ecSensorPin},
        mDosers,
        ECController::Config{
            .target = 1.0f, 
            .dosingAmount = 10.0f, 
            .dosingInterval = std::chrono::seconds(60), 
            .schedule = ghe3part
        } 
    },
    mLLController { 
        Driver::LiquidLevelSensor{liquidLevelTopSensorPin}, 
        Driver::SolenoidValve{valveSwitchPin},
        LiquidLevelController::Config{
            .refillInterval = std::chrono::seconds(60)
        }
    },
    mLcd{0x60, 16, 2},
    mMqttClient{mWifiClient}
{
    // Initially only run LiquidLevelController until reservoir if full
    mPHController.stop();
    mECController.stop();
    mLLController.start();
}

void App::setup()
{
    mLcd.init();
    mLcd.setBacklight(true);

    connectWifi();
    mqttClientConnect();
}

void App::update(const Duration dt)
{
    mDosers.update(dt);
    mPHController.update(dt);
    mECController.update(dt);
    mLLController.update(dt);
    mMqttClient.loop();

    manageControllers();
    updateDisplay(dt);
}

void App::updateDisplay(const Duration dt)
{
    static auto mFromPrevDisplay = Duration{0};
    mFromPrevDisplay += dt;
    if (mFromPrevDisplay > 200ms)
    {
        mFromPrevDisplay = Duration{0};
        mLcd.setCursor(0, 0);
        mLcd.printf("PH %.2f", mPHController.getStatus().ph);

        mLcd.setCursor(0, 1);
        mLcd.printf("EC %.2f", mECController.getStatus().ec);

    }
}

void App::manageControllers()
{
    // Only run ph and ec controllers when LLController is not running
    if (mLLController.getStatus().valveIsOpen)
    {
        if (mPHController.isRunning())
        {
            mPHController.stop();
        }
        if (mECController.isRunning())
        {
            mECController.stop();
        }
    }

    if (!mLLController.getStatus().valveIsOpen)
    {
        if (!mPHController.isRunning())
        {
            mPHController.start();
        }
        if (!mECController.isRunning())
        {
            mECController.start();
        }
    }
}

void App::mqttCallback(char* topic, byte* msg, unsigned int length)
{
}

void App::mqttClientConnect()
{
    mMqttClient.setServer(broker, 1883);
    mMqttClient.setCallback(std::bind(&App::mqttCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    while (!mMqttClient.connected())
    {
        if (mMqttClient.connect("ReservoirController"))
        {
            Serial.println("Connected");
            mMqttClient.subscribe("ReservoirController/rpc");
            break;
        }
        else
        {
            Serial.println("Connection failed");
        }
    }
}

void App::connectWifi()
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