#include "../arduino/wrappers/Utils.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

const auto kSuccess   = 200;
const auto kLightsPin = 2;

struct MockMqttWrapper : public MqttWrapper {
  MOCK_METHOD(bool, connect, (String, String, String), (override));
  MOCK_METHOD(void, subscribe, (String, int), (override));
  MOCK_METHOD(void, publish, (String, String), (override));
  MOCK_METHOD(void, onMessage, (std::function<void(String, String)>), (override));
};



struct MockSerialWrapper : public SerialWrapper {
  MOCK_METHOD(void, println, (String), (override));
  MOCK_METHOD(void , begin, (int ), (override));
};

struct MockArduinoRunTimeWrapper : public ArduinoRunTimeWrapper {
    MOCK_METHOD(void, setPinDirection, (int pin, PinDirection pinDirection), (override));
    MOCK_METHOD(void , setPin, (int ), (override));
    MOCK_METHOD(void , clearPin, (int), (override));
};

/*struct MockArduinoRunWrapper : public ArduinoRunWrapper {
    MOCK_METHOD(long, millis, (), (override));
    MOCK_METHOD(void, delay, (int n), (override));

};*/



/*struct MockSR04Wrapper : public UltraSoundWrapper {
    MOCK_METHOD(int, getDistance, (), (override));
};*/



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
    MOCK_METHOD(void, SR04sensorData,(bool pubSensorData,MqttWrapper &mqttWrapper));
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



TEST(SR04Test, SR04sensorData_WhenConnected_WillPublishToTopics) {
    MockMqttWrapper mqttWrapper;

    EXPECT_CALL(mqttWrapper, connect(_, _, _)).WillOnce(Return(true));
    EXPECT_CALL(mqttWrapper, publish("/smartcar/ultrasound/front", "20"));

    SR04sensorData(true, mqttWrapper);
}



// When moving this method to the Utils.cpp file it gives this error..
// so we will keep it in the header file until we fix the error.
// Error :: clang: error: linker command failed with exit code 1 (use -v to see invocation)


TEST(carSetSpeedTest, handleSpeedInput_WhenConnected_WillSetTheCarSpeed) {
    MockMqttWrapper mqttWrapper;
    MockSerialWrapper serialWrapper;
    MockSmartcarWrapper car;
    const auto speed = 20.0f;

    EXPECT_CALL(mqttWrapper, connect(_, _, _)).WillOnce(Return(true));
    EXPECT_CALL(car, setSpeed(speed));


    handleSpeedInput(speed, car, mqttWrapper);
}
