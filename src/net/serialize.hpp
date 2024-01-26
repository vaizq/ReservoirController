#pragma once

#include "config.hpp"
#include <nlohmann/json_fwd.hpp>


nlohmann::json serializeStatus(const PHController::Status& status);
nlohmann::json serializeStatus(const ECController::Status& status);
nlohmann::json serializeStatus(const LiquidLevelController::Status& status);
nlohmann::json serializeConfig(const PHController::Config& config);
nlohmann::json serializeConfig(const ECController::Config& config);
nlohmann::json serializeConfig(const LiquidLevelController::Config& config);