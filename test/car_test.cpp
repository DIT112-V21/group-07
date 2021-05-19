#include "../arduino/wrappers/Utils.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

struct MockMqttWrapper : public MqttWrapper {
  MOCK_METHOD(bool, connect, (String, String, String), (override));
  MOCK_METHOD(void, subscribe, (String, int), (override));
  MOCK_METHOD(void, publish, (String, String), (override));
  MOCK_METHOD(void, onMessage, (std::function<void(String, String)>), (override));
};

struct MockSerialWrapper : public SerialWrapper {
  MOCK_METHOD(void, println, (String), (override));
  MOCK_METHOD(void , begin, (int n), (override));
};

struct MockArduinoRunWrapper : public ArduinoRunWrapper {
    MOCK_METHOD(long, millis, (), (override));
    MOCK_METHOD(void, delay, (int n), (override));

};

struct MockSR04Wrapper : public UltraSoundWrapper {
    MOCK_METHOD(int, getDistance, (), (override));
};

struct MockSmartcarWrapper : public SmartCarWrapper {
    MOCK_METHOD(float, getSpeed, (), (override));
    MOCK_METHOD(void, setSpeed, (float speed), (override));
    MOCK_METHOD(void, setAngle, (int angle), (override));
    MOCK_METHOD(int, getDistance, (), (override));
    MOCK_METHOD(int, getHeading, (), (override));
    MOCK_METHOD(void, update, (), (override));
};

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