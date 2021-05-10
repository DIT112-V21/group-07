#pragma once

#include <stdint.h> // NOLINT(modernize-deprecated-headers)

#include "Runtime.hpp"
#include "Motor.hpp"

struct BrushedMotorPins
{
BrushedMotorPins(uint8_t forwardPin, uint8_t backwardPin, uint8_t enablePin)
        : forward{ forwardPin }
        , backward{ backwardPin }
        , enable{ enablePin }
    {
    }

    const uint8_t forward;  // NOLINT: It's OK for these to be public
    const uint8_t backward; // NOLINT
    const uint8_t enable;   // NOLINT
};

class BrushedMotor : public Motor
{
public:

    BrushedMotor(Runtime& runtime, uint8_t forwardPin, uint8_t backwardPin, uint8_t enablePin);

    BrushedMotor(Runtime& runtime, BrushedMotorPins pins);

    void setSpeed(int speed) override;

private:
    void attach();
    const uint8_t kForwardPin;
    const uint8_t kBackwardPin;
    const uint8_t kEnablePin;
    Runtime& mRuntime;
    const uint8_t kOutput;
    const uint8_t kLow;
    const uint8_t kHigh;
    bool mAttached{ false };
};
