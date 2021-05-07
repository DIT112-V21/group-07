#pragma once

#include "Car.h"
#include "Runtime.hpp"
#include "Control.hpp"
#include "HeadingSensor.hpp"
#include "Odometer.hpp"
#include <car/heading/HeadingCar.hpp>
#include <car/distance/DistanceCar.hpp>
#include <car/simple/SimpleCar.hpp>


namespace smartcar
{
class SmartCar : public Car
{
public:
    SmartCar(Runtime& runtime,
             Control& control,
             HeadingSensor& headingSensor,
             Odometer& odometerLeft,
             Odometer& odometerRight);

    void setSpeed(float speed) override;
    void setAngle(int angle) override;
    void update() override;
    int getHeading() override;
    long getDistance() override;
    float getSpeed()   override;

//private:
    //HeadingCar& mHCar; we have the heading sensor.
    //DistanceCar& mDCar;
};
} // namespace smartcar