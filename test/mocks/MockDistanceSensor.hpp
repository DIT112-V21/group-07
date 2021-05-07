#pragma once

#include "gmock/gmock.h"

#include "DistanceSensor.hpp"

class MockDistanceSensor : public DistanceSensor
{
public:
    MOCK_METHOD0(getDistance, int());
};
