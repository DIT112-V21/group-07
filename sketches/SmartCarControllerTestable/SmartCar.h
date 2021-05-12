#pragma once

#include "Car.h"
#include <Smartcar.hpp>
#include <WiFi.h>
#include <MQTT.h>

namespace smartcar
{
class SmartCar : public Car
{
public:
    SmartCar(Runtime& mRuntime,
             Control& mControl,
             HeadingSensor& mHeadingSensor,
             Odometer& mOdometerLeft,
             Odometer& mOdometerRight);

    void setSpeed(float speed) override;
    void setAngle(int angle) override;
    void update() override;
    int getHeading() override;
    int getDistance() override;
    float getSpeed() override;

private:
    Runtime& mRuntime;
    Control& mControl;
    HeadingSensor& mHeadingSensor;
    Odometer& mOdometerLeft;
    Odometer& mOdometerRight;
};
} // namespace smartcarr