#pragma once

#include "config.hpp"
#include "DFRobot_RGBLCD1602.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <nlohmann/json_fwd.hpp>
#include <ezButton.h>


constexpr char ssid[] = "Zhone_9C8E";
constexpr char passwd[] = "znid309149582";
constexpr char broker[] = "test.mosquitto.org";


class App
{
public:
    App();
    void setup();
    void update(const Duration dt);
private:
    void connectWifi();
    void mqttClientConnect();
    void updateDisplay(const Duration dt);
    void manageControllers();
    void manageButton();
    void mqttCallback(char* topic, byte* msg, unsigned int length);
    void handleRPC(const nlohmann::json& rpc);
    
    DoserManager mDosers;
    PHController mPHController;
    ECController mECController;
    LiquidLevelController mLLController;
    DFRobot_RGBLCD1602 mLcd;
    ezButton mButton;
    WiFiClient mWifiClient;
    PubSubClient mMqttClient;
};