#pragma once

#include <stdint.h> // NOLINT(modernize-deprecated-headers)

#include "Runtime.hpp"
#include "Odometer.hpp"

class DirectionlessOdometer : public Odometer
{
public:
    DirectionlessOdometer(Runtime& runtime,
                          uint8_t pulsePin,
                          InterruptCallback callback,
                          unsigned long pulsesPerMeter);

    ~DirectionlessOdometer() override = default;

    long getDistance() override;
    float getSpeed() override;
    bool isAttached() const override;
    bool providesDirection() const override;
    virtual void reset();
    virtual void update();

protected:
    const float
        mPulsesPerMeterRatio; // NOLINT: Refactor this so protected variables are not necessary
    volatile unsigned long mPulsesCounter{ 0 }; // NOLINT
    volatile unsigned long mPreviousPulse{ 0 }; // NOLINT
    volatile unsigned long mDt{ 0 };            // NOLINT

private:
    const float mMillimetersPerPulse;
    Runtime& mRuntime;
    const bool kSensorAttached;
};
