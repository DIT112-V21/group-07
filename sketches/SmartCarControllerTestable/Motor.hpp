#pragma once

namespace smartcar
{
namespace constants
{
namespace motor
{
const int kMinMotorSpeed = -100;
const int kIdleMotorSpeed = 0;
const int kMaxMotorSpeed = 100;
} // namespace motor
} // namespace constants
} // namespace smartcar

class Motor
{
public:
    virtual ~Motor() = default;

    virtual void setSpeed(int speed) = 0;
};
