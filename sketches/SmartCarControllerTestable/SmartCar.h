#pragma once

#include "Car.h"
#include <car/heading/HeadingCar.hpp>
#include <car/distance/DistanceCar.hpp> // We might use this including later in other classes.


namespace smartcar
{
class SmartCar : public Car
{
public:
    SmartCar(HeadingCar& mHCar, DistanceCar& mDCar);

    void setSpeed(float speed) override;
    void setAngle(int angle) override;
    void update() override;
    int getHeading() override;
    int getDistance() override;
    float getSpeed()   override;

private:
    HeadingCar& mHCar;
    DistanceCar& mDCar;

};
} // namespace smartcar