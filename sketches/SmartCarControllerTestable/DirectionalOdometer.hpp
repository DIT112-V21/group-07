#pragma once

#include "DirectionlessOdometer.hpp"


struct DirectionalOdometerPins
{

    DirectionalOdometerPins(uint8_t pulsePin, uint8_t forwardWhenLowPin)
        : pulse{ pulsePin }
        , direction{ forwardWhenLowPin }
{
}

    const uint8_t pulse;     // NOLINT: It's OK for these member variables to be non-private
    const uint8_t direction; // NOLINT
};

class DirectionalOdometer : public DirectionlessOdometer
{
public:

    DirectionalOdometer(Runtime& runtime,
                        uint8_t pulsePin,
                        uint8_t forwardWhenLowPin,
                        InterruptCallback callback,
                        unsigned long pulsesPerMeter);

    DirectionalOdometer(Runtime& runtime,
                        DirectionalOdometerPins pins,
                        InterruptCallback callback,
                        unsigned long pulsesPerMeter);


    void reset() override;
    void update() override;
    long getDistance() override;
    float getSpeed() override;
    bool providesDirection() const override;
    int8_t getDirection() const;

private:
    const uint8_t mDirectionPin;
    Runtime& mRuntime;
    const int kPinStateWhenForward;
    volatile unsigned long mNegativePulsesCounter{ 0 };
    volatile int mDirectionPinState;
};
