#include "app.hpp"
#include <chrono>
#include <nlohmann/json.hpp>


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
    mButton{buttonPin, INPUT_PULLUP},
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
    manageButton();
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

// Only run ph and ec controllers when LLController is not running
void App::manageControllers()
{
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

// User can manually control dosers with an button
void App::manageButton()
{
    static DoserManager::DoserID id{0};

    if (mButton.isPressed())
    {
        mDosers.reset();
        mDosers.queueDose(id, 69696969.0f);
    }
    else if (mButton.isReleased())
    {
        mDosers.reset();
        id = (id + 1) % DoserManager::DoserCount;
    }
}

void App::mqttCallback(char* topic, byte* msg, unsigned int length)
{
    using namespace nlohmann;

    msg[length] = 0;
    try
    {
        Serial.println("Parse json");
        json rpc = json::parse(msg);
        Serial.println("Json parsed");
        handleRPC(rpc);
    }
    catch(const std::exception& e)
    {
        Serial.printf("Handling of rpc failed: %s\n", e.what());
    }
}

void App::handleRPC(const nlohmann::json &rpc)
{
    if (rpc["method"] == "calibrate")
    {
        if (rpc["sensor"] == "PHSensor")
        {
            if (!mPHController.getSensor().calibrate())
            {
                throw std::runtime_error{"PH calibration failed"};
            }
        }
        else if (rpc["sensor"] == "ECSensor")
        {
            if (!mECController.getSensor().calibrate(rpc["ec"]));
            {
                throw std::runtime_error{"EC calibration failed"};
            }
        }
    }
    else if (rpc["method"] == "startController")
    {
        if (rpc["controller"] == "PHController")
        {
            mPHController.start();
        }
        else if (rpc["controller"] == "ECController")
        {
            mECController.start();
        }
        else if (rpc["controller"] == "LiquidLevelController")
        {
            mLLController.start();
        }
    }
    else if (rpc["method"] == "stopController")
    {
        if (rpc["controller"] == "PHController")
        {
            mPHController.stop();
        }
        else if (rpc["controller"] == "ECController")
        {
            mECController.stop();
        }
        else if (rpc["controller"] == "LiquidLevelController")
        {
            mLLController.stop();
        }
    }
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