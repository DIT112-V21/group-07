#include "SmartCar.h"
#include <MQTT.h>
#include <WiFi.h>

namespace smartcar
{
    SmartCar::SmartCar(Runtime& runtime,
                       Control& control,
                       HeadingSensor& headingSensor,
                       Odometer& odometerLeft,
                       Odometer& odometerRight)

            : mControl{control},
            mRuntime{runtime},
            mHeadingSensor{headingSensor},
            mOdometerLeft{odometerLeft},
            mOdometerRight{odometerRight}
            //SimpleCar::SimpleCar(control),
            //, DistanceCar::DistanceCar(runtime, control, odometerLeft, odometerRight)
            //, HeadingCar::HeadingCar(control, headingSensor)
    {
    }

    void SmartCar::setSpeed(float speed)
    {
        mControl.setSpeed(speed);
    }

    void SmartCar::setAngle(int angle)
    {
        mControl.setAngle(angle);
    }

    void SmartCar::update()
    {
        mHeadingSensor.update();
    }

    int SmartCar::getHeading()
    {
        return mHeadingSensor.getHeading();
    }

    int SmartCar::getDistance()
    {
        return (mOdometerLeft.getDistance() + mOdometerRight.getDistance()) / 2;
    }

    float SmartCar::getSpeed()
    {
        return (mOdometerLeft.getSpeed() + mOdometerRight.getSpeed()) / 2;
    }
} // namespace smartcar