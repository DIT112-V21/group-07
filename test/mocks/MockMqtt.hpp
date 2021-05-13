#pragma once

#include "Mqtt.h"
#include "gmock/gmock.h"
#include <WiFi.h>


namespace smartcar
{
    class MockMqtt : public Mqtt
    {
    public:
        MOCK_METHOD(void, begin, (), (override));
        MOCK_METHOD(void, beginExternal, (string IP, int port), (override));
        MOCK_METHOD(bool, connect, (string hostname, string username, string password), (override));
        MOCK_METHOD(bool, subscribe, ()string topic,int qos, (override));
        MOCK_METHOD(bool, publish, (string topic, string message), (override));
        MOCK_METHOD(bool, onMessage,(string topic, string message),(override));
        MOCK_METHOD(int, argToInt, (int index), (override));
        MOCK_METHOD(void, handleClient, (), (override));
    };
} // namespace smartcar