#pragma once

#include "Motor.hpp"
#include "Control.hpp"

class DifferentialControl : public Control
{
public:

    DifferentialControl(Motor& leftMotor, Motor& rightMotor);

    void setAngle(int angle) override;
    void setSpeed(int speed) override;
    void overrideMotorSpeed(int firstMotorSpeed, int secondMotorSpeed) override;

private:
    void setMotors();
    Motor& mLeftMotor;
    Motor& mRightMotor;
    int mAngle;
    int mSpeed;
};
