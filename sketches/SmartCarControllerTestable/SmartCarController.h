#pragma once

#include "Car.h"
#include "PinController.h"
#include <Smartcar.h>
#include <MQTT.h>
#include <WiFi.h>
#include <Arduino.h>


namespace smartcar
{
class SmartCarController
{
public:
    SmartCarController(Car& car,
                       PinController& pinController);

    void registerDriveEndpoint();
    void registerGyroscopeEndpoint();
    void registerErrorHandler();
    void beginServer();
    void update();

private:
    Car& mCar;
    WiFiClient net;
    PinController& mPinController;
};
} // namespace smartcar
