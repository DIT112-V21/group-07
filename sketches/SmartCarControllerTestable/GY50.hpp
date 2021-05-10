#pragma once

#include <stdint.h> // NOLINT(modernize-deprecated-headers)

#include "Runtime.hpp"
#include "HeadingSensor.hpp"

namespace smartcar
{
namespace constants
{
namespace gy50
{
const uint8_t kDefaultSamplingInterval    = 100;
const int kError                          = -32767;
const int kDefaultCalibrationMeasurements = 100;
} // namespace gy50
} // namespace constants
} // namespace smartcar

class GY50 : public HeadingSensor
{
public:

    GY50(Runtime& runtime,
         int offset,
         unsigned long samplingInterval = smartcar::constants::gy50::kDefaultSamplingInterval);

    int getHeading() override;
    void update() override;

    int getOffset(int measurements = smartcar::constants::gy50::kDefaultCalibrationMeasurements);

private:
    const int kOffset;
    const unsigned long kSamplingInterval;
    Runtime& mRuntime;
    unsigned long mPreviousSample;
    bool mAttached;
    float mAngularDisplacement;

    void attach();
    int getAngularVelocity();
    int readL3G4200DRegister(uint8_t registerAddress);
    void writeL3G4200DRegister(uint8_t registerAddress, uint8_t value);
};
