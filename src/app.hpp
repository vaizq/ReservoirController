#pragma once

#include "config.hpp"
#include "DFRobot_RGBLCD1602.h"
#include "net/JsonRpc.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <nlohmann/json_fwd.hpp>
#include <ezButton.h>
#include <optional>


constexpr char ssid[] = "Zhone_9C8E";
constexpr char passwd[] = "znid309149582";
constexpr char broker[] = "test.mosquitto.org";


class App
{
public:
    App();
    void setup();
    void update(Duration dt);
private:
    void connectWifi();
    void connectMqttClient();
    void updateDisplay(Duration dt);
    void sendTelemetry();
    void manageButton();
    void mqttCallback(char* topic, byte* msg, unsigned int length);
    void buildRpcInterface();
    std::optional<nlohmann::json> handleRPC(const nlohmann::json& rpc);

    // Sensors
    Driver::AnalogSensor mPHSensor;
    Driver::AnalogSensor mECSensor;
    Driver::DigitalSensor mLLSensor;
    // Controllers
    DoserManager mDoserManager;
    PHController mPHController;
    ECController mECController;
    LiquidLevelController mLLController;
    // Physical UI
    DFRobot_RGBLCD1602 mLcd;
    ezButton mButton;
    // Networking
    WiFiClient mWifiClient;
    PubSubClient mMqttClient;
    JsonRpc::Api mRpcApi;
};