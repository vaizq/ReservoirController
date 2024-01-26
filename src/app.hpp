#pragma once

#include "config.hpp"
#include "DFRobot_RGBLCD1602.h"
#include <WiFi.h>
#include <PubSubClient.h>


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
    void updateDisplay(const Duration dt);
    void manageControllers();
    void mqttCallback(char* topic, byte* msg, unsigned int length);
    void connectWifi();
    void mqttClientConnect();

    DoserManager mDosers;
    PHController mPHController;
    ECController mECController;
    LiquidLevelController mLLController;
    DFRobot_RGBLCD1602 mLcd;
    WiFiClient mWifiClient;
    PubSubClient mMqttClient;

};