#include <Arduino.h>
#include "DriverManager.h"
DriverManager::DriverManager(const std::vector<DriverConfig>& configs) {
    std::lock_guard<std::mutex> lock(mutex);
    initializeDrivers(configs);
}

DriverManager::~DriverManager() {
    std::lock_guard<std::mutex> lock(mutex);
    drivers.clear();
    Serial.println("All drivers destroyed.");
}

void DriverManager::initializeDrivers(const std::vector<DriverConfig>& configs) {
    for (const auto& config : configs) {
        switch (config.type) {
            case DriverType::MOTOR_WITH_DIRECTION:
                if (config.pins.size() >= 2) {
                    drivers[config.name] = std::make_shared<MotorDriverWithDirection>(config.pins[0], config.pins[1]);
                }
                break;

            case DriverType::PIN_ON_OFF:
                if (config.pins.size() >= 1) {
                    drivers[config.name] = std::make_shared<OnOffDriver>(config.pins[0]);
                }
                break;

            default:
                Serial.println("Error: Unknown driver type in configuration.");
        }
    }
}

ResultCode DriverManager::setDriverValue(const std::string& name, void* value, ValueType valueType) {
    std::lock_guard<std::mutex> lock(mutex);

    auto it = drivers.find(name);
    if (it == drivers.end()) {
        return ResultCode::DRIVER_NOT_FOUND;
    }

    auto driver = it->second;
    if (driver->getValueType() != valueType) {
        return ResultCode::INVALID_VALUE_TYPE;
    }

    try {
        driver->setValue(value);
        return ResultCode::SUCCESS;
    } catch (...) {
        Serial.println("Error: Exception while setting driver value.");
        return ResultCode::UNKNOWN_ERROR;
    }
}

void DriverManager::renewDrivers(const std::vector<DriverConfig>& newConfigs) {
    std::lock_guard<std::mutex> lock(mutex);

    // Clear the existing map
    drivers.clear();

    // Reinitialize drivers with the new configuration
    initializeDrivers(newConfigs);

    Serial.println("Drivers map renewed.");
}

