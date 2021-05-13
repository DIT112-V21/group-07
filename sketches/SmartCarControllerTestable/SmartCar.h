#pragma once

#include "Car.h"
#include <car/heading/HeadingCar.hpp>
#include <car/distance/DistanceCar.hpp>

namespace smartcar
{
class SmartCar : public Car
{
public:
    SmartCar(HeadingCar& headingCar, DistanceCar& distanceCar);

    void setSpeed(float speed) override;
    void setAngle(int angle) override;
    void update() override;
    int getHeading() override;
    int getDistance() override;
    float getSpeed()  override;

private:
    HeadingCar& mHeadingCar;
    DistanceCar& mDistanceCar;
};
} // namespace smartcar