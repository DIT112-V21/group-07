#include "SimpleCar.hpp"
#include "Utilities.hpp"

using namespace smartcar::constants::control;
using namespace smartcar::utils;

SimpleCar::SimpleCar(Control& control)
    : mControl(control)
{
}

void SimpleCar::setSpeed(float speed)
{
    mControl.setSpeed(getConstrain(static_cast<int>(speed), kMinControlSpeed, kMaxControlSpeed));
}

void SimpleCar::setAngle(int angle)
{
    mControl.setAngle(getConstrain(angle, kMinControlAngle, kMaxControlAngle));
}

void SimpleCar::overrideMotorSpeed(int firstMotorSpeed, int secondMotorSpeed)
{
    mControl.overrideMotorSpeed(getConstrain(firstMotorSpeed, kMinControlSpeed, kMaxControlSpeed),
                                getConstrain(secondMotorSpeed, kMinControlSpeed, kMaxControlSpeed));
}
