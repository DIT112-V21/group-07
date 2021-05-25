#include "../arduino/wrappers/Utils.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>


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

struct MockSerialWrapper : public SerialWrapper {
  MOCK_METHOD(void, println, (String), (override));
  MOCK_METHOD(void , begin, (int ), (override));
  MOCK_METHOD(bool , available, (), (override));
  MOCK_METHOD(char , readStringUntil, (char ), (override));
};

struct MockArduinoRunTimeWrapper : public ArduinoRunTimeWrapper {
    MOCK_METHOD(void, setPinDirection, (int pin, PinDirection pinDirection), (override));
    MOCK_METHOD(void , setPin, (int ), (override));
    MOCK_METHOD(void , clearPin, (int), (override));
};

struct MockArduinoRunWrapper : public ArduinoRunWrapper {
    MOCK_METHOD(long, millis, (), (override));
    MOCK_METHOD(void, delay, (int), (override));

};

struct MockOdometerWrapper : public OdometerWrapper {
    MOCK_METHOD(int, getDirection, (), (override));

};


struct MockSR04Wrapper : public UltraSoundWrapper {
    MOCK_METHOD(int, getDistance, (), (override));
};

struct MockInfraredSensor : public InfraredSensorWrapper {
    MOCK_METHOD(int, getDistance, (), (override));
};

struct MockSmartcarWrapper : public SmartCarWrapper {
    MOCK_METHOD(float, getSpeed, (), (override));
    MOCK_METHOD(void, setSpeed, (float ), (override));
    MOCK_METHOD(void, setAngle, (int), (override));
    MOCK_METHOD(int, getDistance, (), (override));
    MOCK_METHOD(void, update, (), (override));
};

/*struct MockSmartCarControllerWrapper : public SmartCarControllerWrapper {

    MOCK_METHOD(float, getSpeed, (), (override));
    MOCK_METHOD(void, setSpeed, (float speed), (override));
    MOCK_METHOD(void, setAngle, (int angle), (override));
    MOCK_METHOD(int, getDistance, (), (override));
    MOCK_METHOD(void, update, (), (override));
    //MOCK_METHOD(void, SR04sensorData,(bool pubSensorData,MqttWrapper &mqttWrapper));
};
*/
using ::testing::_;
using ::testing::Return;



TEST(MQTTMessageInputTest, MQTTMessageInput_WhenCalled_WillConnectToMqtt) {
  MockMqttWrapper mqttWrapper;
  MockSerialWrapper serialWrapper;

  EXPECT_CALL(mqttWrapper, connect(_, _, _));

  MQTTMessageInput(mqttWrapper, serialWrapper);
}



TEST(MQTTMessageInputTest, MQTTMessageInput_WhenConnected_WillSubscribeToTopics) {
  MockMqttWrapper mqttWrapper;
  MockSerialWrapper serialWrapper;

  EXPECT_CALL(mqttWrapper, connect(_, _, _)).WillOnce(Return(true));
  EXPECT_CALL(mqttWrapper, subscribe("/smartcar/control/#", _));
  EXPECT_CALL(mqttWrapper, subscribe("/smartcar/connectionLost", _));

  MQTTMessageInput(mqttWrapper, serialWrapper);
}


TEST(MQTTMessageInputTest, MQTTMessageInput_WhenConnected_WillRegisterCallback) {
  MockMqttWrapper mqttWrapper;
  MockSerialWrapper serialWrapper;

  EXPECT_CALL(mqttWrapper, connect(_, _, _)).WillOnce(Return(true));
  EXPECT_CALL(mqttWrapper, onMessage(_));

  MQTTMessageInput(mqttWrapper, serialWrapper);
}


// When moving this method to the Utils.cpp file it gives this error..
// so we will keep it in the header file until we fix the error.
// Error :: clang: error: linker command failed with exit code 1 (use -v to see invocation)


TEST(handleSpeedInput_Test, handleSpeedInput_WhenSetSpeed_WillDivideTheCarSpeed) {
    MockMqttWrapper mqttWrapper;
    MockSerialWrapper serialWrapper;
    MockSmartcarWrapper car;

    const auto speed = -20.0f;

    EXPECT_CALL(car, setSpeed(speed/2));

    handleSpeedInput(0,speed,serialWrapper,mqttWrapper, car);
}

TEST(handleAngleInput_Test, handleAngleInput_WhenSetAngle_WillSetTheCarAngle){
    MockMqttWrapper mqttWrapper;
    MockSerialWrapper serialWrapper;
    MockSmartcarWrapper car;

    const auto angle = 70;

    EXPECT_CALL(car, setAngle(angle));

    handleAngleInput (0, angle, serialWrapper, mqttWrapper, car);
}

TEST(handleInput_Test, handleInput_WhenConnectedAndSpeedIsPozitive_WillReturnSensorDistancePositive) {
    MockMqttWrapper mqttWrapper;
    MockSerialWrapper serialWrapper;
    MockSmartcarWrapper car;
    MockInfraredSensor infraredSensor;
    String input = "start";

    const auto speedPositive = 20.0f;

    EXPECT_CALL(serialWrapper, available()).WillOnce(Return(true));
    EXPECT_CALL(infraredSensor, getDistance());

    handleInput_SpeedTopicPositive(input, speedPositive, serialWrapper, mqttWrapper, car, infraredSensor);
}

TEST(handleInput_Test, handleInput_WhenConnectedAndSpeedIsNegative_WillReturnSensorDistanceNegative) {
    MockMqttWrapper mqttWrapper;
    MockSerialWrapper serialWrapper;
    MockSmartcarWrapper car;
    MockInfraredSensor infraredSensor;
    String input = "start";

    const auto speedNegative = -20.0f;

    EXPECT_CALL(serialWrapper, available()).WillOnce(Return(true));
    EXPECT_CALL(infraredSensor, getDistance());

    handleInput_SpeedTopicNegative(input, speedNegative, serialWrapper, mqttWrapper, car, infraredSensor);
}


TEST(handleInputStopCar_Test, handleInputStopCar_WhenConnectedAndTopicNotStartWithS_WillStopTheCar) {
    MockSerialWrapper serialWrapper;
    MockSmartcarWrapper car;

    String input = "dart";

    EXPECT_CALL(serialWrapper, available()).WillOnce(Return(true));
    EXPECT_CALL(car, setSpeed(0));

    handleInputStopCar(input, serialWrapper,car);
}

TEST(connectLocalHost_Test, connectLocalHost_WhenCalled_WillConnectToLocalHost) {
    MockMqttWrapper mqttWrapper;

    EXPECT_CALL(mqttWrapper, beginLocal());

    connectLocalHost(mqttWrapper);
}

TEST(connectExternalHost_Test, connectExternalHost_WhenCalled_WillConnectToExternalHost) {
    MockMqttWrapper mqttWrapper;

    EXPECT_CALL(mqttWrapper, beginExternal());

    connectExternalHost(mqttWrapper);
}

TEST(SR04Test, SR04sensorData_WhenConnected_WillPublishToUltrasoundTopic) {
    MockMqttWrapper mqttWrapper;

    EXPECT_CALL(mqttWrapper, connect(_, _, _)).WillOnce(Return(true));
    EXPECT_CALL(mqttWrapper, publish("/smartcar/ultrasound/front", "20"));

    SR04sensorData(true, mqttWrapper);
}

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

TEST(isClearTest, isClear_WhenCalled_WillUltraSoundSensorDistance) {
    MockSR04Wrapper ultraSoundWrapper;
    MockInfraredSensor infraredSensor;

    int sensor = 1; // 1 means "frontUS"

    EXPECT_CALL(ultraSoundWrapper, getDistance());

    isClear(sensor, ultraSoundWrapper, infraredSensor);
}

TEST(isClearTest, isClear_WhenCalled_WillInfraredSensorDistance) {
    MockSR04Wrapper ultraSoundWrapper;
    MockInfraredSensor infraredSensor;

    int sensor = 2; // 2 means "frontIR"

    EXPECT_CALL(infraredSensor, getDistance());

    isClear(sensor, ultraSoundWrapper, infraredSensor);
}

TEST(convertSpeedTest, convertSpeed_WhenCalled_WillConvertTheSpeed) {

    float expectedSpeed = 100;

    EXPECT_EQ(expectedSpeed, convertSpeed(initialSpeed));
}

TEST(slowDownSmoothlyTest, slowDownSmoothly_WhenCalled_WillSetTheSpeedToSlowDownSmoothly) {


    float expectedSpeed = 70;


    EXPECT_EQ(expectedSpeed, slowDownSmoothly(STOPPING_SPEED, initialSpeed));

}

TEST(reactToSensorTest, reactToSensor_WhenObstacle_WillSlowDownTheCar) {
    MockSmartcarWrapper car;
    MockInfraredSensor infraredSensor;

    int sensorDistance = 80;
    float expectedSpeed = 70;

    EXPECT_CALL(car, setSpeed(expectedSpeed));

    reactToSensor(sensorDistance, FRONT_STOP_DISTANCE, car, STOPPING_SPEED, initialSpeed);
}

TEST(reactToSensorTest, reactToSensor_WhenObstacle_WillStopTheCar) {
    MockSmartcarWrapper car;

    int sensorDistance = 60;

    EXPECT_CALL(car, setSpeed(0));

    reactToSensor(sensorDistance, FRONT_STOP_DISTANCE,car, STOPPING_SPEED, initialSpeed);
}

TEST(reactToSensorTest, reactToSensor_WhenNOobstacle_WillReturnFalse) {
    MockSmartcarWrapper car;

    int sensorDistance = 0;

    bool obstacle = reactToSensor(sensorDistance, FRONT_STOP_DISTANCE,car, STOPPING_SPEED, initialSpeed);

    EXPECT_EQ(false, obstacle);

}

TEST(emergencyBreakTest, emergencyBreak_WhenFrontSensorIsClearAnd_NOobstacle_WillReturnFalse) {
    MockSmartcarWrapper car;
    MockSR04Wrapper ultrasoundSensor;
    MockInfraredSensor infraredSensor;

    int leftDirection = 1;
    int rightDirection = 1;
    int frontSensorDistance = 80;
    int backSensorDistance = 0;
    int sensor = 1;


    EXPECT_CALL(car, setSpeed(70));

    bool emergencyBreak = emergencyBrake(leftDirection, rightDirection,frontSensorDistance, backSensorDistance, sensor,
            ultrasoundSensor, infraredSensor,FRONT_STOP_DISTANCE, car,STOPPING_SPEED,  initialSpeed);


    EXPECT_EQ(false, emergencyBreak);

}

TEST(emergencyBreakTest, emergencyBreak_WhenFrontSensorNotClearAnd_IsObstacle_WillReturnTrue) {
    MockSmartcarWrapper car;
    MockSR04Wrapper ultrasoundSensor;
    MockInfraredSensor infraredSensor;

    int leftDirection = 1;
    int rightDirection = 1;
    int frontSensorDistance = 60;
    int backSensorDistance = 0;
    int sensor = 3;


    EXPECT_CALL(car, setSpeed(0));

    bool isEmergencyBreak = emergencyBrake(leftDirection, rightDirection,frontSensorDistance, backSensorDistance, sensor,
                                         ultrasoundSensor, infraredSensor,FRONT_STOP_DISTANCE, car,STOPPING_SPEED,  initialSpeed);


    EXPECT_EQ(true, isEmergencyBreak);

}

TEST(emergencyBreakTest, emergencyBreak_WhenBackSensorNotClearAnd_IsObstacle_WillReturnTrue) {
    MockSmartcarWrapper car;
    MockSR04Wrapper ultrasoundSensor;
    MockInfraredSensor infraredSensor;

    int leftDirection = -1;
    int rightDirection = -1;
    int frontSensorDistance = 0;
    int backSensorDistance = 40;
    int sensor = 0;


    EXPECT_CALL(car, setSpeed(0));

    bool isEmergencyBreak = emergencyBrake(leftDirection, rightDirection,frontSensorDistance, backSensorDistance, sensor,
                                           ultrasoundSensor, infraredSensor,BACK_STOP_DISTANCE, car,STOPPING_SPEED,  initialSpeed);


    EXPECT_EQ(true, isEmergencyBreak);

}