#pragma once

#include "Car.h"
#include <car/heading/HeadingCar.hpp>

namespace smartcar
{
class SmartCar : public Car
{
public:
    SmartCar(HeadingCar& car);

    void setSpeed(float speed) override;
    void setAngle(int angle) override;
    void update() override;
    int getHeading() override;

private:
    HeadingCar& mCar;
};
} // namespace smartcar