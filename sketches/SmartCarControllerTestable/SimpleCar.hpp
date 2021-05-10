#pragma once

#include "Control.hpp"
#include "Car.hpp"

class SimpleCar : public Car
{
public:

    SimpleCar(Control& control);


    void setSpeed(float speed) override;
    void setAngle(int angle) override;
    void overrideMotorSpeed(int firstMotorSpeed, int secondMotorSpeed) override;

private:
    Control& mControl;
};

