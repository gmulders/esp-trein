#ifndef DRIVER_H
#define DRIVER_H

#include <cstdint>  // For uint8_t, uint32_t
#include <vector>

// Enum for value types
enum class ValueType {
    BOOL,
    UINT8,
    UINT32_ARRAY,
    MOTOR_CONTROL
};

// Abstract base class for all drivers
class Driver {
public:
    virtual ~Driver() {}
    virtual void setValue(void* value) = 0;  // Set a value dynamically
    virtual void reset() = 0;               // Reset the driver
    virtual ValueType getValueType() const = 0;  // Return expected value type
};

// Struct for motor control parameters
struct MotorControl {
    uint8_t speed;  // Speed value (0-255)
    bool direction; // Direction: true = forward, false = backward
};

// MotorDriverWithDirection class
class MotorDriverWithDirection : public Driver {
private:
    int forwardPin;
    int backwardPin;

public:
    MotorDriverWithDirection(int fPin, int bPin);
    void setValue(void* value) override;
    void reset() override;
    ValueType getValueType() const override;
    ~MotorDriverWithDirection();
};

class OnOffDriver : public Driver {
private:
    int pin;

public:
    OnOffDriver(int pin);
    void setValue(void* value) override;
    void reset() override;
    ValueType getValueType() const override;
    ~OnOffDriver();
};


#endif  // DRIVER_H
