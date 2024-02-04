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
                Driver::DFRobotV2PHSensor{phSensorPin},
                mDoserManager,
                phDownDoser,
                PHController::Config{
            .target = 6.0f,
            .dosingAmount = 1.0f,
            .dosingInterval = std::chrono::seconds(60),
        } 
    },
    mECController {
                Driver::ECSensor{ecSensorPin},
                mDoserManager,
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
    json rpc = json::parse(msg, nullptr, false);
    if (!rpc.is_discarded())
    {
        const auto response = handleRPC(rpc);
        if (response)
        {
            mMqttClient.publish("ReservoirController/rpc/response", response->dump().c_str());
        }
    }
    else
    {
        Serial.println("Invalid json");
    }

}

std::optional<nlohmann::json> App::handleRPC(const nlohmann::json &rpc)
{
    const auto& method = rpc["method"];

    if (method == "calibrate")
    {
        if (rpc["sensor"] == "PHSensor")
        {
            if (mPHController.getSensor().calibrate())
            {
                Serial.println("PH calibration succeeded");
            }
            else
            {
                Serial.println("PH calibraion failed");
            }
        }
        else if (rpc["sensor"] == "ECSensor")
        {
            const float ec = rpc["ec"];
            Serial.printf("EC: %.2f\n", ec);
            if (mECController.getSensor().calibrate(ec))
            {
                Serial.println("EC calibration succeeded");
            }
            else
            {
                Serial.println("EC calibration failed");
            }
        }
    }
    else if (method == "startController")
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
    else if (method == "stopController")
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
    else if (method == "getStatus")
    {
        nlohmann::json response;

        response["id"] = rpc["id"];

        if (rpc["device"] == "PHController")
        {
            response["status"] = serializeStatus(mPHController.getStatus());
        }
        else if (rpc["device"] == "ECController")
        {
            response["status"] = serializeStatus(mECController.getStatus());
        }
        else if (rpc["device"] == "LiquidLevelController")
        {
            response["status"] = serializeStatus(mLLController.getStatus());
        }
        else
        {
            return {};
        }

        return response;
    }
    else if (method == "getConfig")
    {
        nlohmann::json response;

        response["id"] = rpc["id"];

        if (rpc["device"] == "PHController")
        {
            response["config"] = serializeConfig(mPHController.getConfig());
        }
        else if (rpc["device"] == "ECController")
        {
            response["config"] = serializeConfig(mECController.getConfig());
        }
        else if (rpc["device"] == "LiquidLevelController")
        {
            response["config"] = serializeConfig(mLLController.getConfig());
        }
        else
        {
            return {};
        }

        return response;
    }
    else if (method == "setConfig")
    {
        if (rpc["device"] == "PHController")
        {
            PHController::Config cfg = mPHController.getConfig();

            cfg.target = rpc.value("target", cfg.target);
            cfg.dosingAmount = rpc.value("dosingAmout", cfg.dosingAmount);
            if (rpc.contains("dosingInterval"))
            {
                cfg.dosingInterval = Duration(rpc["dosingInterval"].get<float>());
            }

            mPHController.setConfig(cfg);
        }
        else if (rpc["device"] == "ECController")
        {
            ECController::Config cfg = mECController.getConfig();

            cfg.target = rpc.value("target", cfg.target);
            cfg.dosingAmount = rpc.value("dosingAmout", cfg.dosingAmount);
            if (rpc.contains("dosingInterval"))
            {
                cfg.dosingInterval = Duration(rpc["dosingInterval"].get<float>());
            }
            if (rpc.contains("nutrientSchedule") && rpc["nutrientSchedule"].is_array() && rpc["nutrientSchedule"].size() == mECController.doserCount)
            {
                int i = 0;
                for (const auto& pair : rpc["nutrientSchedule"]) {
                    if (pair.is_array() && pair.size() == 2) {
                        const int pumpID = pair[0].get<unsigned int>();
                        const float nutrientAmount = pair[1].get<float>();
                        cfg.schedule[i].first = pumpID;
                        cfg.schedule[i].second = nutrientAmount;
                    } else {
                        Serial.println("Invalid pair structure in schedule");
                    }
                    ++i;
                }
            }
            else
            {
                Serial.println("Nutrient schedule is not valid");
            }

            mECController.setConfig(cfg);
        }
        else if (rpc["device"] == "LiquidLevelController")
        {
            LiquidLevelController::Config cfg = mLLController.getConfig();

            if (rpc.contains("refillInterval"))
            {
                cfg.refillInterval = Duration{rpc["refillInterval"].get<float>()};
            }

            mLLController.setConfig(cfg);
        }
        else
        {
            return {};
        }
    }

    return {};
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