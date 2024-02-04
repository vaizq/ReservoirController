#include "serialize.hpp"
#include <nlohmann/json.hpp>


nlohmann::json serializeStatus(const PHController::Status& status)
{
    nlohmann::json doc;
    doc["device"] = "PHController";
    doc["ph"] = status.ph;
    return doc;
}

nlohmann::json serializeStatus(const ECController::Status& status)
{
    nlohmann::json doc;
    doc["device"] = "ECController";
    doc["ec"] = status.ec;
    return doc;
}

nlohmann::json serializeStatus(const LiquidLevelController::Status& status)
{
    nlohmann::json doc;
    doc["device"] = "LiquidLevelController";
    doc["levelIsHigh"] = status.levelIsHigh;
    doc["valveIsOpen"] = status.valveIsOpen;
    return doc;
}

nlohmann::json serializeConfig(const PHController::Config& config)
{
    nlohmann::json doc;
    doc["device"] = "PHController";
    doc["target"] = config.target;
    doc["dosignAmount"] = config.dosingAmount;
    doc["dosingInterval"] = config.dosingInterval.count();
    return doc;
}

nlohmann::json serializeConfig(const ECController::Config& config)
{
    nlohmann::json doc;
    doc["device"] = "ECController";
    doc["target"] = config.target;
    doc["dosignAmount"] = config.dosingAmount;
    doc["dosingInterval"] = config.dosingInterval.count();
    doc["nutrientSchedule"] = nlohmann::json::array();
    for (const auto& pair : config.schedule)
    {
        nlohmann::json jPair;
        jPair[0] = pair.first;
        jPair[1] = pair.second;
        doc["nutrientSchedule"].push_back(jPair);
    }
    return doc;
}

nlohmann::json serializeConfig(const LiquidLevelController::Config& config)
{
    nlohmann::json doc;
    doc["device"] = "PHController";
    doc["refillInterval"] = config.refillInterval.count();
    return doc;
}