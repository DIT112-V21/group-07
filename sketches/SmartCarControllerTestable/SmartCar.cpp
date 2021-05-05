#include "SmartCar.h"

namespace smartcar
{
    SmartCar::SmartCar(HeadingCar& car)
    : mCar{car}
{
}

void SmartCar::setSpeed(float speed)
{
    mCar.setSpeed(speed);
}

void SmartCar::setAngle(int angle)
{
    mCar.setAngle(angle);
}

void SmartCar::update()
{
    mCar.update();
}

int SmartCar::getHeading()
{
    return mCar.getHeading();
}
} // namespace smartcar