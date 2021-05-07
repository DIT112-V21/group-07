#include "SmartCarController.h"

#include <cstdio>
#include <cstring>

namespace
{
const auto kPlainText = "text/plain";
const auto kSuccess   = 200;
const auto kError     = 404;
const auto kLightsPin = 2;

bool areEqual(const std::vector<char>& v, const char* c)
{
    return strcmp(v.data(), c) == 0;
}
} // namespace

namespace smartcar
{
SmartCarController::SmartCarController(Car& car,
                                       MQTT& restServer,
                                       PinController& pinController)
    : mCar{car}
    , mRestServer{restServer}
    , mPinController{pinController}
{
    mPinController.setPinDirection(kLightsPin, PinDirection::kOutput);
}

void SmartCarController::registerDriveEndpoint()
{
    mRestServer.on("/drive", [this]() {
        const auto arguments = mRestServer.args();

        for (auto i = 0; i < arguments; i++)
        {
            const auto command = mRestServer.argName(i);
            if (areEqual(command, "speed"))
            {
                const auto userSpeed = mRestServer.argToInt(i);
                mCar.setSpeed(static_cast<float>(userSpeed));
                if (userSpeed == 0)
                {
                    mPinController.setPin(kLightsPin);
                }
                else
                {
                    mPinController.clearPin(kLightsPin);
                }
            }
            else if (areEqual(command, "angle"))
            {
                mCar.setAngle(mRestServer.argToInt(i));
            }
        }
        mRestServer.send(kSuccess, kPlainText, "ok");
    });
}

void SmartCarController::registerGyroscopeEndpoint()
{
    mRestServer.on("/gyro", [this]() {
        char heading[4];
        sprintf(heading, "%d", mCar.getHeading());
        mRestServer.send(kSuccess, kPlainText, heading);
    });
}

void SmartCarController::registerErrorHandler()
{
    mRestServer.onNotFound(
        [this]() { mRestServer.send(kError, kPlainText, "Unknown command"); });
}

void SmartCarController::beginServer()
{
    mRestServer.begin();
}

void SmartCarController::update()
{
    mRestServer.handleClient();
    mCar.update();
}
} // namespace smartcar
