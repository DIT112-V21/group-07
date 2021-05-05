#pragma once

#include "Car.h"
#include "PinController.h"
#include "RestServer.h"

namespace smartcar
{
class SmartCarController
{
public:
    SmartCarController(Car& car,
                       RestServer& restServer,
                       PinController& pinController);

    void registerDriveEndpoint();
    void registerGyroscopeEndpoint();
    void registerErrorHandler();
    void beginServer();
    void update();

private:
    Car& mCar;
    RestServer& mRestServer;
    PinController& mPinController;
};
} // namespace smartcar
