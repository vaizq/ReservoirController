#include "app.hpp"
#include <chrono>
#include <nlohmann/json.hpp>
#include "net/serialize.hpp"


using namespace std::chrono_literals;


App::App()
:
    mPHSensor{phSensorPin, {{1.5f, 7.0f}, {2.03244f, 4.0f}}, 69},
    mECSensor{ecSensorPin, {{0.0f, 0.0f}, {2.4f, 2.0f}}, 420},
    mLiquidLevelFullSensor{liquidLevelFullSensorPin},
    mLiquidLevelLowSensor{liquidLevelLowSensorPin},
    mValve{valveSwitchPin},
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
                mPHSensor,
                mDoserManager.getHandle(phDownDoser),
                PHController::Config{
            .target = 6.0f,
            .dosingAmount = 1.0f,
            .dosingInterval = std::chrono::seconds(60),
        } 
    },
    mECController {
                mECSensor,
                mDoserManager,
                ECController::Config{
            .target = 1.0f, 
            .dosingAmount = 10.0f, 
            .dosingInterval = std::chrono::seconds(60), 
            .schedule = ghe3part
        } 
    },
    mLLController {
        mLiquidLevelFullSensor,
        mValve,
        LiquidLevelController::Config{
            .refillInterval = std::chrono::seconds(60)
        }
    },
    mLcd{0x60, 16, 2},
    mButton{buttonPin, INPUT_PULLUP},
    mMqttClient{mWifiClient}
{
    mPHController.stop();
    mECController.stop();
    mLLController.stop();

    buildApi();
}

void App::setup()
{
    mLcd.init();
    mLcd.setBacklight(true);

    connectWifi();
    connectMqttClient();
}

void App::update(const Duration dt)
{
    static Duration fromLastSensorUpdate{0};
    static constexpr Duration sensorUpdateInterval{std::chrono::milliseconds {500}};
    if ((fromLastSensorUpdate+= dt) > sensorUpdateInterval)
    {
        fromLastSensorUpdate = Duration{0};
        mPHSensor.update(dt);
        mECSensor.update(dt);
    }
    mDoserManager.update(dt);
    mPHController.update(dt);
    mECController.update(dt);
    mLLController.update(dt);
    mMqttClient.loop();
    mButton.loop();

    manageButton();
    updateDisplay(dt);

    static Duration fromLastTelemetry{0};
    static constexpr Duration telemetryInterval{std::chrono::seconds{1}};
    if ((fromLastTelemetry += dt) > telemetryInterval)
    {
        fromLastTelemetry = Duration{0};
        sendTelemetry();
    }
}

void App::updateDisplay(const Duration dt)
{
    static auto mFromPrevDisplay = Duration{0};
    mFromPrevDisplay += dt;
    if (mFromPrevDisplay > 200ms)
    {
        mFromPrevDisplay = Duration{0};
        mLcd.setCursor(0, 0);
        mLcd.printf("PH %.2f", mPHSensor.read());

        mLcd.setCursor(0, 1);
        mLcd.printf("EC %.2f", mECSensor.read());
    }
}

void App::sendTelemetry()
{
    nlohmann::json telemetry;

    telemetry["ph"] = mPHSensor.read();
    telemetry["ec"] = mECSensor.read();
    telemetry["liquidLevel"] = [this](){
        if (mLiquidLevelFullSensor.read())
        {
            return "full";
        }
        else if (mLiquidLevelLowSensor.read())
        {
            return "low";
        }
        else
        {
            return "empty";
        }
    }();

    mMqttClient.publish("ReservoirController/telemetry", telemetry.dump().c_str());
}

// User can manually control dosers with a button
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
        const auto response = handleRPC(j);
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
    return mRpcApi.handle(rpc);
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

void App::connectMqttClient()
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

void App::buildApi()
{
    using Request = JsonRpc::Api::Request;
    using Response = JsonRpc::Api::Response;
    using Parameter = JsonRpc::Parameter;

    static auto hasParam = [] (const Request& req, const std::string& name) {
        return req.contains("params") && req["params"].contains(name);
    };

    mRpcApi.bind("openValve",
                 [this](const Request& req) {
                     mValve.open();
                     return JsonRpc::Api::createSuccessResponse(req["id"]);
                 }
    );

    mRpcApi.bind("closeValve",
                 [this](const Request& req) {
                     mValve.close();
                     return JsonRpc::Api::createSuccessResponse(req["id"]);
                 }
    );

    mRpcApi.bind("dose",
                 [this](const Request& req) {
                     if (req.contains("params") &&
                     req["params"].contains("doserID") &&
                     req["params"].contains("amount"))
                     {
                         DoserManager::DoserID id = req["params"]["doserID"];
                         float amount = req["params"]["amount"];
                         if (id >= DoserManager::DoserCount) {
                             return JsonRpc::Api::createErrorResponse(req["id"], JsonRpc::ErrorCode::InvalidParams, "Invalid doserID");
                         }
                         mDoserManager.queueDose(id, amount);
                         return JsonRpc::Api::createSuccessResponse(req["id"]);
                     }
                     else {
                         return JsonRpc::Api::createErrorResponse(req["id"], JsonRpc::ErrorCode::InvalidParams, "parameters doserID or amount missing");
                     }
                 }
    );

    mRpcApi.bind("resetDosers",
                 [this](const Request& req) {
                     mDoserManager.reset();
                     return JsonRpc::Api::createSuccessResponse(req["id"]);
                 }
    );

    mRpcApi.bind("dosersCount",
                 [](const Request& req) {
                     const auto count = DoserManager::DoserCount;
                     return JsonRpc::Api::createSuccessResponse(req["id"])["result"] = count;
                 }
    );

    mRpcApi.bind("calibratePHSensor",
                       [this](const Request& req) {
                            if (hasParam(req, "phValue")) {
                                const bool success = mPHSensor.calibrate(req["params"]["phValue"]);
                                if (success) {
                                    return JsonRpc::Api::createSuccessResponse(req["id"]);
                                }
                                else {
                                    return JsonRpc::Api::createErrorResponse(req["id"], JsonRpc::ErrorCode::ServerError, "calibration failed");
                                }
                            }
                            else {
                                return JsonRpc::Api::createErrorResponse(req["id"], JsonRpc::ErrorCode::InvalidParams, "phValue missing");
                            }
                       },
                       {Parameter{Parameter::Type::Real, "phValue"}}
    );

    mRpcApi.bind("calibrateECSensor",
                 [this](const Request& req) {
                     if (hasParam(req, "ecValue")) {
                         const bool success = mECSensor.calibrate(req["params"]["ecValue"]);
                         if (success) {
                             return JsonRpc::Api::createSuccessResponse(req["id"]);
                         }
                         else {
                             return JsonRpc::Api::createErrorResponse(req["id"], JsonRpc::ErrorCode::ServerError, "calibration failed");
                         }
                     }
                     else {
                         return JsonRpc::Api::createErrorResponse(req["id"], JsonRpc::ErrorCode::InvalidParams, "ecValue missing");
                     }
                 },
                 {Parameter{Parameter::Type::Real, "ecValue"}}
    );
}

















