#pragma once

namespace smartcar
{
struct Car
{
    virtual ~Car() = default;

    virtual void setSpeed(float speed)                                          = 0;
    virtual void setAngle(int angle)                                            = 0;
    virtual void overrideMotorSpeed(int firstMotorSpeed, int secondMotorSpeed)  = 0;
};
} // namespace smartcar
