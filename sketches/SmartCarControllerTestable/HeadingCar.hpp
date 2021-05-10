#pragma once

#include "HeadingSensor.hpp"
#include "car/simple/SimpleCar.hpp"

class HeadingCar : virtual public SimpleCar
{
public:
    HeadingCar(Control& control, HeadingSensor& headingSensor);

    int getHeading();
    virtual void update();

private:
    HeadingSensor& mHeadingSensor;
};
