#pragma once

#include "Mqtt.h"
#include <MQTT.h>


namespace smartcar
{
class MqttServer : public Mqtt
{
public:
    MqttServer( MQTTClient& mqttClient, WifiClient& wifiClient);

    virtual void begin()override; // localhost
    virtual void beginExternal(string IP, int port) override; // externalHost
    virtual bool connect(string hostname, string username, string password) override; //returns boolean
    virtual bool subscribe(string topic,int qos) override;
    virtual bool publish(string topic, string message) override;
    virtual bool onMessage(string topic, string message) override; //returns boolean!!
    virtual int argToInt(int index) override;
    virtual void handleClient() override;

private:
    MQTTClient& mMqttClient;
    WifiClient& mWifiClient;
};
} // namespace magic_car