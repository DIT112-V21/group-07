#pragma once

#include "Car.h"
#include "PinController.h"
#include "MQTT.h"

namespace smartcar
{
class SmartCarController
{
public:
    SmartCarController(Car& car,
                       MQTT& restServer,
                       PinController& pinController);

    void registerDriveEndpoint();
    void registerGyroscopeEndpoint();
    void registerErrorHandler();
    void beginServer();
    void update();

private:
    Car& mCar;
    MQTT& mRestServer;
    PinController& mPinController;
};
} // namespace smartcar
