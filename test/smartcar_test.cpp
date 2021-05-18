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
  MOCK_METHOD(void, millis, (), (override));
};

struct MockSR04Wrapper : public UltraSoundWrapper {
    MOCK_METHOD(int, getDistance, (), (override));
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

TEST(SR04Test, SR04sensorData_WhenCOnnected_WillPublishToTopics) {
    MockMqttWrapper mqttWrapper;
    MockSerialWrapper serialWrapper;
    MockSR04Wrapper sr04Wrapper;
    bool pubSensorData = true;
    String publishTopic = "/smartcar/ultrasound/front";

    String message = reinterpret_cast<const char *>(sr04Wrapper.getDistance());

    EXPECT_CALL(mqttWrapper, connect(_, _, _)).WillOnce(Return(true));
    EXPECT_CALL(mqttWrapper, publish(publishTopic, message));

    SR04sensorData(pubSensorData, publishTopic, sr04Wrapper, serialWrapper, mqttWrapper);
}