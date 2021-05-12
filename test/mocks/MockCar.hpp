#pragma once

#include "Car.h"
#include "gmock/gmock.h"
#include <WiFi.h>


namespace smartcar
{
class MockCar : public Car
{
public:
    MOCK_METHOD(void, setSpeed, (float speed), (override));
    MOCK_METHOD(void, setAngle, (int angle), (override));
    MOCK_METHOD(void, update, (), (override));
    MOCK_METHOD(int, getHeading, (), (override));
    MOCK_METHOD(int, getDistance, (), (override));
    MOCK_METHOD(float, getSpeed,(),(override));







};
} // namespace smartcar