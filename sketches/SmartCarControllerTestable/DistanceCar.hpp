#pragma once

#include ".Runtime.hpp"
#include "Odometer.hpp"
#include "car/simple/SimpleCar.hpp"

namespace smartcar
{
namespace constants
{
namespace car
{
const unsigned long kDefaultPidFrequency = 80;
const float kDefaultProportional         = 5.0F;
const float kDefaultIntegral             = 0.0F;
const float kDefaultDerivative           = 10.0F;
const int kOdometersNotAttachedError     = -1000;
const auto kBreakSpeedScale              = 10;
} // namespace car
} // namespace constants
} // namespace smartcar

class DistanceCar : virtual public SimpleCar
{
public:

    DistanceCar(Runtime& runtime, Control& control, Odometer& odometer);

    DistanceCar(Runtime& runtime,
                Control& control,
                Odometer& odometerLeft,
                Odometer& odometerRight);

    long getDistance();
    void setSpeed(float speed) override;
    float getSpeed();

    void enableCruiseControl(float proportional = smartcar::constants::car::kDefaultProportional,
                             float integral     = smartcar::constants::car::kDefaultIntegral,
                             float derivative   = smartcar::constants::car::kDefaultDerivative,
                             unsigned long frequency
                             = smartcar::constants::car::kDefaultPidFrequency);

    virtual void update();
    void disableCruiseControl();
    void overrideMotorSpeed(int firstMotorSpeed, int secondMotorSpeed) override;

private:
    Odometer& mOdometerLeft;
    Odometer& mOdometerRight;
    Runtime& mRuntime;
    bool mCruiseControlEnabled{ false };
    float mProportional{ 0 };
    float mIntegral{ 0 };
    float mDerivative{ 0 };
    unsigned long mFrequency{ 0 };
    unsigned long mPreviousUpdate{ 0 };
    float mTargetSpeed{ 0 };
    float mPreviousControlledSpeed{ 0 };
    float mIntegratedError{ 0 };
    float mPreviousError{ 0 };

    bool areOdometersAttached();
    bool areOdometersDirectional();
    float controlMotorSpeed(const float& previousSpeed,
                            const float& targetSpeed,
                            const float& currentSpeed);

    void brake();
};
