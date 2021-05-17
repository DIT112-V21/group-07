#include "gtest/gtest.h"
#include "MockCar.hpp"

using namespace testing;

namespace smartcar {

    struct SmartCarTest : public Test {
        MockCar car;
    };

    TEST(SmartCarSpeedTest, constructor_WhenCalled_WillSetLightsPinDirection
    ) {
    MockCar car;

    car.setSpeed(31);
    EXPECT_EQ(31, car.getSpeed());
}
}