#include "SmartCar.h"

namespace smartcar
{
SmartCar::SmartCar(HeadingCar& headingCar, DistanceCar& distanceCar)
    : mHeadingCar{headingCar}
    , mDistanceCar{distanceCar}
{
}

void SmartCar::setSpeed(float speed)
{
    mDistanceCar.setSpeed(speed);
}

void SmartCar::getSpeed(float speed)
{
    mDistanceCar.setSpeed(speed);
}

void SmartCar::setAngle(int angle)
{
    mDistanceCar.setAngle(angle);
}

void SmartCar::update()
{
    mDistanceCar.update();
}

int SmartCar::getHeading()
{
    return mHeadingCar.getHeading();
}

int SmartCar::getDistance()
{
    return mDistanceCar.getDistance();
}
} // namespace smartcar