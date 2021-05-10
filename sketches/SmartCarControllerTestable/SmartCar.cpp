#include "SmartCar.h"

namespace smartcar
{

SmartCar::SmartCar(HeadingCar& hCar, DistanceCar& dCar)
        : mHCar {hCar},
          mDCar {dCar}
{
}

void SmartCar::setSpeed(float speed)
{
    mHCar.setSpeed(speed);
}

void SmartCar::setAngle(int angle)
{
    mHCar.setAngle(angle);
}

void SmartCar::update()
{
    mHCar.update();
}

int SmartCar::getHeading()
{
    return mHCar.getHeading();
}

int SmartCar::getDistance()
{
    return mDCar.getDistance();
}

float SmartCar::getSpeed()
{
     return mDCar.getSpeed();
}


} // namespace smartcar