#pragma once

namespace smartcar
{
namespace constants
{
namespace control
{
const int kMinControlAngle  = -90;
const int kIdleControlAngle = 0;
const int kMaxControlAngle  = 90;
const int kMinControlSpeed  = -100;
const int kIdleControlSpeed = 0;
const int kMaxControlSpeed  = 100;
} // namespace control
} // namespace constants
} // namespace smartcar

class Control
{
public:
    virtual ~Control() = default;
    virtual void setAngle(int angle) = 0;
    virtual void setSpeed(int speed) = 0;
    virtual void overrideMotorSpeed(int firstMotorSpeed, int secondMotorSpeed) = 0;
};
