#include "SmartCar.h"

namespace smartcar
{

    SmartCar::SmartCar(Runtime& runtime,
                       Control& control,
                       HeadingSensor& headingSensor,
                       Odometer& odometerleft,
                       Odometer& odometerRight)
            : SimpleCar::SimpleCar(control)
            , DistanceCar::DistanceCar(runtime, control, odometerleft, odometerRight)
            , HeadingCar::HeadingCar(control, headingSensor)
{
}

void SmartCar::setSpeed(float speed)
{
    SimpleCar.setSpeed(speed);
}

void SmartCar::setAngle(int angle)
{
    SimpleCar.setAngle(angle);
}

void SmartCar::update()
{
    HeadingCar.update();
}

int SmartCar::getHeading()
{
    return HeadingCar.getHeading();
}

long SmartCar::getDistance()
{
    return DistanceCar.getDistance();
}

float SmartCar::getSpeed()
{
     return DistanceCar.getSpeed();
}


} // namespace smartcar