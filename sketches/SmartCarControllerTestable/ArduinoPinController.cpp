#include "ArduinoPinController.h"
#include <Arduino.h>

namespace smartcar
{
void ArduinoPinController::setPinDirection(int pin, PinDirection pinDirection)
{
    pinMode(pin, pinDirection == PinDirection::kInput ? INPUT : OUTPUT);
}

void ArduinoPinController::setPin(int pin)
{
    digitalWrite(pin, HIGH);
}

void ArduinoPinController::clearPin(int pin)
{
    digitalWrite(pin, LOW);
}
} // namespace smartcar
