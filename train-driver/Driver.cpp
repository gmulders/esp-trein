#include "Driver.h"
#include <Arduino.h>  // For pinMode, analogWrite


MotorDriverWithDirection::MotorDriverWithDirection(int fPin, int bPin)
    : forwardPin(fPin), backwardPin(bPin) {
    pinMode(forwardPin, OUTPUT);
    pinMode(backwardPin, OUTPUT);
    reset();
}

void MotorDriverWithDirection::setValue(void* value) {
    MotorControl* control = static_cast<MotorControl*>(value);

    if (control->direction) {
        analogWrite(forwardPin, control->speed);
        analogWrite(backwardPin, 0);
    } else {
        analogWrite(forwardPin, 0);
        analogWrite(backwardPin, control->speed);
    }
}

void MotorDriverWithDirection::reset() {
    analogWrite(forwardPin, 0);
    analogWrite(backwardPin, 0);
}

ValueType MotorDriverWithDirection::getValueType() const {
    return ValueType::MOTOR_CONTROL;
}

MotorDriverWithDirection::~MotorDriverWithDirection() {
    reset();
}



OnOffDriver::OnOffDriver(int pin) : pin(pin) {
    pinMode(pin, OUTPUT);
    reset();
}

void OnOffDriver::setValue(void* value) {
    bool state = *static_cast<bool*>(value);
    digitalWrite(pin, state ? HIGH : LOW);
}

void OnOffDriver::reset() {
    digitalWrite(pin, LOW);
}

ValueType OnOffDriver::getValueType() const {
    return ValueType::BOOL;
}

OnOffDriver::~OnOffDriver() {
    reset();
}


