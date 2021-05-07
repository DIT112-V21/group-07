#pragma once

#include <SmartCar.h>
#include "Car.h"
#include "gmock/gmock.h"

namespace smartcar
{
class MockCar : public SmartCar
{
public:
    MOCK_METHOD(void, setSpeed, (float speed), (override));
    MOCK_METHOD(void, setAngle, (int angle), (override));
    MOCK_METHOD(void, update, (), (override));
    MOCK_METHOD(int, getHeading, (), (override));
    MOCK_METHOD(int, getDistance, (), (override));
};
} // namespace smartcar