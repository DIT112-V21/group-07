#include "../arduino/wrappers/Utils.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

// MOCKS for Wrappers we have created to be able to use them in tested units.
// ------------------------ MOCKS --------------------------------//

struct MockArduinoRunWrapper : public ArduinoRunWrapper {
    MOCK_METHOD(long, millis, (), (override));
    MOCK_METHOD(void, delay, (int), (override));
};

struct MockSR04Wrapper : public UltraSoundWrapper {
    MOCK_METHOD(int, getDistance, (), (override));
};

struct MockInfraredSensor : public InfraredSensorWrapper {
    MOCK_METHOD(int, getDistance, (), (override));
};

struct MockSerialWrapper : public SerialWrapper {
    MOCK_METHOD(void, println, (String), (override));
    MOCK_METHOD(void , begin, (int ), (override));
    MOCK_METHOD(bool , available, (), (override));
    MOCK_METHOD(char , readStringUntil, (char ), (override));
};


struct MockMqttWrapper : public MqttWrapper {
  MOCK_METHOD(void, beginLocal, (), (override));
  MOCK_METHOD(void, beginExternal, (), (override));
  MOCK_METHOD(bool, connected, (), (override));
  MOCK_METHOD(bool, loop, (), (override));
  MOCK_METHOD(bool, connect, (String, String, String), (override));
  MOCK_METHOD(void, subscribe, (String, int), (override));
  MOCK_METHOD(void, publish, (String, String), (override));
  MOCK_METHOD(void, onMessage, (std::function<void(String, String)>), (override));
};


struct MockSmartcarWrapper : public SmartCarWrapper {
    MOCK_METHOD(float, getSpeed, (), (override));
    MOCK_METHOD(void, setSpeed, (float ), (override));
    MOCK_METHOD(void, setAngle, (int), (override));
    MOCK_METHOD(int, getDistance, (), (override));
    MOCK_METHOD(void, update, (), (override));
};
// ------------------------ end of MOCKS --------------------------------//


using ::testing::_;
using ::testing::Return;


// A test if we can connect to MQTT.
// We are using MQTTMessageInput() method which is expected to call mqtt.connect().
TEST(MQTTMessageInputTest, MQTTMessageInput_WhenCalled_WillConnectToMqtt) {
  MockMqttWrapper mqttWrapper;
  MockSerialWrapper serialWrapper;

  EXPECT_CALL(mqttWrapper, connect(_, _, _));

  MQTTMessageInput(mqttWrapper, serialWrapper);
}


// A test if we can subscribe to topic.
// We are using MQTTMessageInput() method which is expected to call mqtt.connect() & subscribe().
TEST(MQTTMessageInputTest, MQTTMessageInput_WhenConnected_WillSubscribeToTopics) {
  MockMqttWrapper mqttWrapper;
  MockSerialWrapper serialWrapper;

  EXPECT_CALL(mqttWrapper, connect(_, _, _)).WillOnce(Return(true));
  EXPECT_CALL(mqttWrapper, subscribe("/smartcar/control/#", _));
  EXPECT_CALL(mqttWrapper, subscribe("/smartcar/connectionLost", _));

  MQTTMessageInput(mqttWrapper, serialWrapper);
}

// A test if we can register callback.
// We are using MQTTMessageInput() method which is expected to call mqtt.connect() & onMessage().
TEST(MQTTMessageInputTest, MQTTMessageInput_WhenConnected_WillRegisterCallback) {
  MockMqttWrapper mqttWrapper;
  MockSerialWrapper serialWrapper;

  EXPECT_CALL(mqttWrapper, connect(_, _, _)).WillOnce(Return(true));
  EXPECT_CALL(mqttWrapper, onMessage(_));

  MQTTMessageInput(mqttWrapper, serialWrapper);
}

// A test if we can divide the car speed, when handleSpeedInput() is called,
// We are using handleSpeedInput() method which is expected to call car.setSpeed().
TEST(handleSpeedInput_Test, handleSpeedInput_WhenSetSpeed_WillDivideTheCarSpeed) {
    MockMqttWrapper mqttWrapper;
    MockSerialWrapper serialWrapper;
    MockSmartcarWrapper car;

    const auto speed = -20.0f;

    EXPECT_CALL(car, setSpeed(speed/2));

    handleSpeedInput(0,speed,serialWrapper,mqttWrapper, car);
}

// A test if we can set the car angle, when handleAngleInput() is called,
// We are using handleAngleInput() method which is expected to call car.setAngle().
TEST(handleAngleInput_Test, handleAngleInput_WhenSetAngle_WillSetTheCarAngle){
    MockMqttWrapper mqttWrapper;
    MockSerialWrapper serialWrapper;
    MockSmartcarWrapper car;

    const auto angle = 70;

    EXPECT_CALL(car, setAngle(angle));

    handleAngleInput (0, angle, serialWrapper, mqttWrapper, car);
}

// A test if the input start with "s" we check first if there is an obstacle in the front when the car is moving forward,
// we handle the car speed.
// We are using handleInput_SpeedTopicPositive() method which is expected to call serialWrapper.available() & infraredSensor.getDistance().
TEST(handleInput_Test, handleInput_WhenInputStartWithSAndSpeedIsPositive_WillHandleSpeedInput) {
    MockMqttWrapper mqttWrapper;
    MockSerialWrapper serialWrapper;
    MockSmartcarWrapper car;
    MockInfraredSensor infraredSensor;
    String input = "start";

    const auto speedPositive = 20.0;

    EXPECT_CALL(serialWrapper, available()).WillOnce(Return(true));
    EXPECT_CALL(infraredSensor, getDistance());

    handleInput_SpeedTopicPositive(input, speedPositive, serialWrapper, mqttWrapper, car, infraredSensor);
}


// A test if the input start with "s" we check first if there is an obstacle in the back when the car is moving backward,
// we handle the car speed.
// We are using handleInput_SpeedTopicNegative() method which is expected to call serialWrapper.available() & infraredSensor.getDistance().
TEST(handleInput_Test, handleInput_WhenInputStartWithSAndSpeedIsNegative_WillHandleSpeedInput) {
    MockMqttWrapper mqttWrapper;
    MockSerialWrapper serialWrapper;
    MockSmartcarWrapper car;
    MockInfraredSensor infraredSensor;
    String input = "start";

    const auto speedNegative = -20.0;

    EXPECT_CALL(serialWrapper, available()).WillOnce(Return(true));
    EXPECT_CALL(infraredSensor, getDistance());

    handleInput_SpeedTopicNegative(input, speedNegative, serialWrapper, mqttWrapper, car, infraredSensor);
}

// A test if the input doesn't start with "s" we stop the car.
// We are using handleInputStopCar() method which is expected to call serialWrapper.available() & car.setSpeed().
TEST(handleInputStopCar_Test, handleInputStopCar_WhenConnectedAndTopicNotStartWithS_WillStopTheCar) {
    MockSerialWrapper serialWrapper;
    MockSmartcarWrapper car;

    String input = "dart";

    EXPECT_CALL(serialWrapper, available()).WillOnce(Return(true));
    EXPECT_CALL(car, setSpeed(0));

    handleInputStopCar(input, serialWrapper,car);
}

// A test to connect to localHost.
// We are using connectLocalHost() method which is expected to call mqttWrapper.beginLocal().
TEST(connectLocalHost_Test, connectLocalHost_WhenCalled_WillConnectToLocalHost) {
    MockMqttWrapper mqttWrapper;

    EXPECT_CALL(mqttWrapper, beginLocal());

    connectLocalHost(mqttWrapper);
}

// A test to connect to externalHost.
// We are using connectExternalHost() method which is expected to call mqttWrapper.beginExternal().
TEST(connectExternalHost_Test, connectExternalHost_WhenCalled_WillConnectToExternalHost) {
    MockMqttWrapper mqttWrapper;

    EXPECT_CALL(mqttWrapper, beginExternal());

    connectExternalHost(mqttWrapper);
}

// A test to connect to publish to this "/smartcar/ultrasound/front" topic.
TEST(SR04Test, SR04sensorData_WhenConnected_WillPublishToUltrasoundTopic) {
    MockMqttWrapper mqttWrapper;

    EXPECT_CALL(mqttWrapper, connect(_, _, _)).WillOnce(Return(true));
    EXPECT_CALL(mqttWrapper, publish("/smartcar/ultrasound/front", "20"));

    SR04sensorData(true, mqttWrapper);
}

// A test to connect to publish to this "/smartcar/ultrasound/front" topic for every second.
// We are using connectExternalHost() method which is expected to call arduinoRunWrapper.millis() will return second,
// ultraSoundWrapper.getDistance() & mqttWrapper.publish().
TEST(SR04Test, SR04sensorData_WhenConnected_WillPublishToUltrasoundTopicEverySecond) {

    MockMqttWrapper mqttWrapper;
    MockSerialWrapper serialWrapper;
    MockArduinoRunWrapper arduinoRunWrapper;
    MockSR04Wrapper ultraSoundWrapper;

    String message = std::to_string(ultraSoundWrapper.getDistance());

    EXPECT_CALL(arduinoRunWrapper, millis()).WillOnce(Return(1000));
    EXPECT_CALL(ultraSoundWrapper, getDistance());
    EXPECT_CALL(mqttWrapper, publish("/smartcar/ultrasound/front", message));

    SR04sensorData(true,mqttWrapper,serialWrapper, arduinoRunWrapper,ultraSoundWrapper );
}

// A test if the distance which is given is not null, we return false.
TEST(isClearTest, isClear_WhenDistanceIsNotNull_WillReturnFalse) {

    int sensor = 1; // 1 means "frontUS"
    int distance = 100 ;

    EXPECT_EQ(false, isClear(sensor, distance));
}

// A test if the distance which is given is null, we return true.
TEST(isClearTest, isClear_WhenDistanceIsNotNull_WillReturnTrue) {


    int sensor = 1; // 1 means "frontUS"
    int distance = 0 ;

    EXPECT_EQ(true, isClear(sensor, distance));
}

// A test if we get expected speed 1.0 from the conversion of the initial speed 1.845.
TEST(convertSpeedTest, convertSpeed_WhenCalled_WillConvertTheSpeed) {

    float expectedSpeed = 100;

    EXPECT_EQ(expectedSpeed, convertSpeed(initialSpeed));
}

// A test if we get (to slow down the speed) to 0.7 from the initial speed 1.845
TEST(slowDownSmoothlyTest, slowDownSmoothly_WhenCalled_WillSetTheSpeedToSlowDownSmoothly) {

    float expectedSpeed = 70;

    EXPECT_EQ(expectedSpeed, slowDownSmoothly(STOPPING_SPEED, initialSpeed));
}

// A test if we get (to slow down the speed) to 0.7 from the initial speed 1.845. If there is an obstacle in range,
// we are using the reactToSensor() method which is expected to call car.setSpeed().
TEST(reactToSensorTest, reactToSensor_WhenObstacle_WillSlowDownTheCar) {
    MockSmartcarWrapper car;
    MockInfraredSensor infraredSensor;

    int sensorDistance = 80;
    float expectedSpeed = 70;

    EXPECT_CALL(car, setSpeed(expectedSpeed));

    reactToSensor(sensorDistance, FRONT_STOP_DISTANCE, car, STOPPING_SPEED, initialSpeed);
}

// A test if we get set to car speed to 0 from the initial speed 1.845.
// we are using the reactToSensor() method which is expected to call car.setSpeed().
TEST(reactToSensorTest, reactToSensor_WhenObstacle_WillStopTheCar) {
    MockSmartcarWrapper car;

    int sensorDistance = 60;

    EXPECT_CALL(car, setSpeed(0));

    reactToSensor(sensorDistance, FRONT_STOP_DISTANCE,car, STOPPING_SPEED, initialSpeed);
}

// A test to see if front sensor dont detect any obstacle return false.
TEST(reactToSensorTest, reactToSensor_When_NO_obstacle_WillReturnFalse) {
    MockSmartcarWrapper car;

    int sensorDistance = 0;

    bool isObstacle = reactToSensor(sensorDistance, FRONT_STOP_DISTANCE,car, STOPPING_SPEED, initialSpeed);

    EXPECT_EQ(false, isObstacle);

}

// A test to see if there is an obstacle which appears suddenly and there is no need for emergency break we slow down the car.
// The method emergencyBreak() will return false.
// We are using the emergencyBreak() method which is expected to call car.setSpeed().
TEST(emergencyBreakTest, emergencyBreak_WhenFrontSensorIsClearAnd_SuddenlyIsObstacleInRange_WillReturnFalseAndSlowDownTheCar) {
    MockSmartcarWrapper car;
    MockSR04Wrapper ultrasoundSensor;
    MockInfraredSensor infraredSensor;

    int leftDirection = 1;
    int rightDirection = 1;
    int frontSensorDistance = 0;
    int newSensorValue = 80;
    float expectedSpeed = 70;
    int sensor = 1;

    EXPECT_CALL(car, setSpeed(expectedSpeed));

    bool isEmergencyBreak = emergencyBrake(leftDirection, rightDirection,frontSensorDistance,
                                         sensor, newSensorValue, FRONT_STOP_DISTANCE,
                                         car,STOPPING_SPEED,  initialSpeed);

    EXPECT_EQ(false, isEmergencyBreak);
}

// A test to see if there is an obstacle that appears suddenly in the front of the car and we need an emergency break,
// the method emergencyBreak() will return true.
// We are using the emergencyBreak() method which is expected to call car.setSpeed().
TEST(emergencyBreakTest, emergencyBreak_WhenFrontSensorNotClearAnd_IsObstacle_WillReturnTrueAndStopTheCar) {
    MockSmartcarWrapper car;

    int leftDirection = 1;
    int rightDirection = 1;
    int frontSensorDistance = 40;
    int newSensorValue = 0;
    int sensor = 1;

    EXPECT_CALL(car, setSpeed(0));

    bool isEmergencyBreak = emergencyBrake(leftDirection, rightDirection,frontSensorDistance,
                                           sensor, newSensorValue,FRONT_STOP_DISTANCE,
                                           car,STOPPING_SPEED,  initialSpeed);

    EXPECT_EQ(true, isEmergencyBreak);
}

// A test to see if there is an obstacle which appears suddenly in the back of the car and we need an emergency break,
// the method emergencyBreak() will return true.
// We are using the emergencyBreak() method which is expected to call car.setSpeed().
TEST(emergencyBreakTest, emergencyBreak_WhenBackSensorNotClearAnd_IsObstacle_WillReturnTrueAndStopTheCar) {
    MockSmartcarWrapper car;
    MockSR04Wrapper ultrasoundSensor;
    MockInfraredSensor infraredSensor;

    int leftDirection = -1;
    int rightDirection = -1;
    int backSensorDistance = 0;
    int newSensorValue = 40;
    int sensor = 1;

    EXPECT_CALL(car, setSpeed(0));

    bool isEmergencyBreak = emergencyBrake(leftDirection, rightDirection,backSensorDistance,
                                           sensor, newSensorValue, BACK_STOP_DISTANCE,
                                           car,STOPPING_SPEED,  initialSpeed);

    EXPECT_EQ(true, isEmergencyBreak);
}

// A test to see if there is an obstacle which appears suddenly on the right side of the car, we need to react by turning -45 degrees left.
// We are using the reactToSides() method which is expected to call car.setAngle().
TEST(reactToSidesTest, reactToSides_WhenRightSensorNotClearAnd_IsObstacle_WillSetAngleToLeft) {

    MockSmartcarWrapper car;

    int leftValue = 50;
    int rightValue = 30;
    int distance = 29;
    int newDistance = 25;

    int sensor = 1;

    EXPECT_CALL(car, setAngle(-45));

    reactToSides(sensor, distance, newDistance, leftValue, rightValue,car);


}

// A test to see if there is an obstacle which appears suddenly on the left side of the car, we need to react by turning 45 degrees right.
// We are using the reactToSides() method which is expected to call car.setAngle().
TEST(reactToSidesTest, reactToSides_WhenLeftSensorNotClearAnd_IsObstacle_WillSetAngleToRight) {

    MockSmartcarWrapper car;

    int leftValue = 30;
    int rightValue = 50;
    int distance = 29;
    int newDistance = 25;
    int sensor = 1;

    EXPECT_CALL(car, setAngle(45));

    reactToSides(sensor, distance, newDistance,leftValue, rightValue,car);


}