#pragma once

namespace smartcar
{
struct Car
{
    virtual ~Car() = default;

    virtual void setSpeed(float speed) = 0;
    virtual void setAngle(int angle)   = 0;
    virtual void update()              = 0;
    virtual int getHeading()           = 0;
};
} // namespace smartcar
