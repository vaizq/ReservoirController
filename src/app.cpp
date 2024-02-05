#include "app.hpp"
#include <chrono>
#include <nlohmann/json.hpp>
#include "net/serialize.hpp"


using namespace std::chrono_literals;


App::App()
:
    mDoserManager {
        {
            Doser{Valve{doserDefs[0]}, flowRate}, 
            Doser{Valve{doserDefs[1]}, flowRate},
            Doser{Valve{doserDefs[2]}, flowRate},
            Doser{Valve{doserDefs[3]}, flowRate}
        }, 
        parallelDosersLimit
    },
    mPHController {
                Driver::AnalogSensor{phSensorPin, {{1.5f, 7.0f}, {2.03244f, 4.0f}}},
                mDoserManager,
                phDownDoser,
                PHController::Config{
            .target = 6.0f,
            .dosingAmount = 1.0f,
            .dosingInterval = std::chrono::seconds(60),
        } 
    },
    mECController {
                Driver::AnalogSensor{ecSensorPin, {{0.0f, 0.0f}, {2.4f, 2.0f}}},
                mDoserManager,
                ECController::Config{
            .target = 1.0f, 
            .dosingAmount = 10.0f, 
            .dosingInterval = std::chrono::seconds(60), 
            .schedule = ghe3part
        } 
    },
    mLLController {
        Driver::DigitalSensor{liquidLevelTopSensorPin},
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

    buildRpcInterface();
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
    mDoserManager.update(dt);
    mPHController.update(dt);
    mECController.update(dt);
    mLLController.update(dt);
    mMqttClient.loop();
    mButton.loop();

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
        mDoserManager.reset();
        mDoserManager.queueDose(id, 69696969.0f);
    }
    else if (mButton.isReleased())
    {
        mDoserManager.reset();
        id = (id + 1) % DoserManager::DoserCount;
    }
}

void App::mqttCallback(char* topic, byte* msg, unsigned int length)
{
    using namespace nlohmann;

    msg[length] = 0;
    json j = json::parse(msg, nullptr, false);
    if (!j.is_discarded())
    {
        Serial.print("Call handleRPC for ");
        Serial.println(j.dump().c_str());

        const auto response = handleRPC(j);

        if (response)
        {
            Serial.println(response->dump().c_str());
            mMqttClient.publish("ReservoirController/rpc/response", response->dump().c_str());
        }
        else
        {
            Serial.println("No response");
        }
    }
    else
    {
        Serial.println("Invalid json");
    }

}

std::optional<nlohmann::json> App::handleRPC(const nlohmann::json &rpc)
{
    return mRpcInterface.handle(rpc);
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
            mMqttClient.subscribe("ReservoirController/rpc/request");
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
    Serial.println("Done");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("Wifi connected");
    Serial.println(WiFi.localIP());
}

void App::buildRpcInterface()
{
    using Params = JsonRpcInterface::Params;
    mRpcInterface.bind("getPH", [this](const std::optional<Params>& params) {
        return mPHController.getStatus().ph;
    });

    mRpcInterface.bind("getEC", [this](const std::optional<Params>& params) {
        return mECController.getStatus().ec;
    });

    mRpcInterface.bind("setTargetPH", [this](const std::optional<Params>& params) {
        if (params)
        {
            const float value = (*params)["value"];
            auto config = mPHController.getConfig();
            config.target = value;
            mPHController.setConfig(config);
        }
        return std::nullopt;
    });

    mRpcInterface.bind("setTargetEC", [this](const std::optional<Params>& params) {
        if (params)
        {
            const float value = (*params)["value"];
            auto config = mECController.getConfig();
            config.target = value;
            mECController.setConfig(config);
        }
        return std::nullopt;
    });
}

















