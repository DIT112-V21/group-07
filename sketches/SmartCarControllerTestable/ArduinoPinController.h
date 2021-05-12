#pragma once

#include "PinController.h"
#include <MQTT.h>
#include <WiFi.h>

namespace magic_car
{
struct ArduinoPinController : public PinController
{
    void setPinDirection(int pin, PinDirection pinDirection) override;
    void setPin(int pin) override;
    void clearPin(int pin) override;
};
} // namespace magic_car
