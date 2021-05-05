#include "SmartCarController.h"
#include "MockCar.hpp"
#include "MockPinController.hpp"
#include "MockRestServer.hpp"
#include "gtest/gtest.h"

using namespace testing;

namespace
{
const auto kSuccess   = 200;
const auto kLightsPin = 2;
const std::vector<char> kDummyName{'a', 'b', 'c', '\0'};
const std::vector<char> kSpeed{'s', 'p', 'e', 'e', 'd', '\0'};
const std::vector<char> kAngle{'a', 'n', 'g', 'l', 'e', '\0'};
} // namespace

namespace smartcar
{
struct SmartCarControllerTest : public Test
{
    MockCar mCar;
    MockRestServer mRestServer;
    MockPinController mPinController;
    SmartCarController mSmartCarController{mCar, mRestServer, mPinController};
};

struct DriveEndpointTest : public Test
{
    void SetUp() override
    {
        EXPECT_CALL(mRestServer, on(_, _)).WillOnce(SaveArg<1>(&mCallback));
        ON_CALL(mRestServer, argName(_)).WillByDefault(Return(kDummyName));

        mSmartCarController.registerDriveEndpoint();
    }

    MockCar mCar;
    MockRestServer mRestServer;
    MockPinController mPinController;
    SmartCarController mSmartCarController{mCar, mRestServer, mPinController};
    std::function<void()> mCallback;
};

TEST(SmartCarConstructorTest, constructor_WhenCalled_WillSetLightsPinDirection)
{
    MockCar car;
    MockRestServer restServer;
    MockPinController pinController;

    EXPECT_CALL(pinController,
                setPinDirection(kLightsPin, PinDirection::kOutput));
    SmartCarController mSmartCarController{car, restServer, pinController};
}

TEST_F(SmartCarControllerTest,
       registerDriveEndpoint_WhenCalled_WillRegisterEndpoint)
{
    EXPECT_CALL(mRestServer, on(StrEq("/drive"), _));

    mSmartCarController.registerDriveEndpoint();
}

TEST_F(SmartCarControllerTest,
       registerGyroscopeEndpoint_WhenCalled_WillRegisterEndpoint)
{
    // TODO: Write this test
}

TEST_F(SmartCarControllerTest, gyroscopeEndpoint_WhenCalled_WillSendHeading)
{
    std::function<void()> callback;
    EXPECT_CALL(mRestServer, on(_, _)).WillOnce(SaveArg<1>(&callback));
    mSmartCarController.registerGyroscopeEndpoint();

    const auto heading = 334;
    EXPECT_CALL(mCar, getHeading()).WillOnce(Return(heading));
    EXPECT_CALL(mRestServer,
                send(kSuccess, _, StrEq(std::to_string(heading).c_str())));

    callback();
}

TEST_F(SmartCarControllerTest,
       registerErrorHandler_WhenCalled_WillRegisterErrorHandler)
{
    // TODO: Write this test
}

TEST_F(SmartCarControllerTest, errorEndpoint_WhenCalled_WillSendError)
{
    // TODO: Write this test
}

TEST_F(SmartCarControllerTest, beginServer_WhenCalled_WillBeginServer)
{
    // TODO: Write this test
}

TEST_F(SmartCarControllerTest, update_WhenCalled_WillUpdateServerAndCar)
{
    // TODO: Write this test
}

TEST_F(DriveEndpointTest, callback_WhenCalled_WillSendSuccess)
{
    EXPECT_CALL(mRestServer, send(kSuccess, _, _));
    mCallback();
}

TEST_F(DriveEndpointTest, callback_WhenNoArguments_WillNotFetchArguments)
{
    EXPECT_CALL(mRestServer, args()).WillOnce(Return(0));
    EXPECT_CALL(mRestServer, argName(_)).Times(0);

    mCallback();
}

TEST_F(DriveEndpointTest, callback_WhenArguments_WillGetAllArguments)
{
    // TODO: Write this test
}

TEST_F(DriveEndpointTest, callback_WhenSpeedAsArgument_WillSetSpeed)
{
    const auto speed = 512;
    EXPECT_CALL(mRestServer, args()).WillOnce(Return(1));
    EXPECT_CALL(mRestServer, argName(_)).WillOnce(Return(kSpeed));
    EXPECT_CALL(mRestServer, argToInt(_)).WillOnce(Return(speed));
    EXPECT_CALL(mCar, setSpeed(static_cast<float>(speed)));

    mCallback();
}

TEST_F(DriveEndpointTest, callback_WhenAngleAsArgument_WillSetAngle)
{
    // TODO: Write this test
}

TEST_F(DriveEndpointTest,
       callback_WhenSpeedAndAngleAsArgument_WillSetSpeedAndAngle)
{
    const auto angle = -843;
    const auto speed = -512;
    EXPECT_CALL(mRestServer, args()).WillOnce(Return(2));
    EXPECT_CALL(mRestServer, argName(_))
        .WillOnce(Return(kAngle))
        .WillOnce(Return(kSpeed));
    EXPECT_CALL(mRestServer, argToInt(_))
        .WillOnce(Return(angle))
        .WillOnce(Return(speed));
    EXPECT_CALL(mCar, setAngle(angle));
    EXPECT_CALL(mCar, setSpeed(static_cast<float>(speed)));

    mCallback();
}
} // namespace smartcar
