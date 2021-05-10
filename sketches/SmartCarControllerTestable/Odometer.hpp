#pragma once

#include <stdint.h> // NOLINT(modernize-deprecated-headers)

#include "InterruptCallback.hpp"

namespace smartcar
{
namespace constants
{
namespace odometer
{
const int8_t kForward                      = 1;
const int8_t kBackward                     = -1;
const int8_t kIdle                         = 0;
const unsigned long kDefaultPulsesPerMeter = 100; // 1:1 cm to pulses ratio
const unsigned long kMinimumPulseGap       = 700;
} // namespace odometer
} // namespace constants
} // namespace smartcar

class Odometer
{
public:
    virtual ~Odometer() = default;

    virtual long getDistance() = 0;
    virtual float getSpeed() = 0;
    virtual bool isAttached() const = 0;
    virtual bool providesDirection() const = 0;
};
