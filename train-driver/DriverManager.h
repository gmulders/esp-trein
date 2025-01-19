#ifndef DRIVERMANAGER_H
#define DRIVERMANAGER_H

#include <map>
#include <memory>
#include <vector>
#include <string>
#include <mutex>
#include "Driver.h"

// Enum for driver types
enum class DriverType {
    MOTOR_WITH_DIRECTION,
    PIN_ON_OFF
//    FAST_LED_STRIP
};

// Struct to hold driver configuration
struct DriverConfig {
    DriverType type;
    std::string name;
    std::vector<int> pins;    // Pins required for the driver
    int extraParam = 0;       // Extra parameter, e.g., LED count
};

// Enum for result codes
enum class ResultCode {
    SUCCESS,
    DRIVER_NOT_FOUND,
    INVALID_VALUE_TYPE,
    UNKNOWN_ERROR
};

// DriverManager class
class DriverManager {
private:
    std::map<std::string, std::shared_ptr<Driver>> drivers;
    mutable std::mutex mutex;

    // Helper function to initialize drivers
    void initializeDrivers(const std::vector<DriverConfig>& configs);

public:
    DriverManager(const std::vector<DriverConfig>& configs);
    ~DriverManager();

    ResultCode setDriverValue(const std::string& name, void* value, ValueType valueType);
    void renewDrivers(const std::vector<DriverConfig>& newConfigs);
};

#endif  // DRIVERMANAGER_H