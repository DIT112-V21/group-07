#include "MqttServer.h"
#include <WiFi.h>

namespace smartcar
{
MqttServer::MqttServer(MQTTClient& mqttClient, WifiClient& wifiClient)
    : mMqttClient{mqttClient}
    , mWifiClient{wifiClient}
{
}

void MqttServer::begin()
{
    mMqttClient.begin();
}

void MqttServer::beginExternal(string IP, int port)
{
    mMqttClient.begin(IP, port, WiFi);
}

bool MqttServer::connect(string hostname, string username, string password)
{
    return mMqttClient.connect(hostname, username, password);

}

bool MqttServer::subscribe(string topic, int qos)
{
    return mMqttClient.subscribe(topic, qos);
}

bool MqttServer::publish(string topic, string message)
{
    return mMqttClient.publish(topic, message);
}

void MqttServer::onMessage(string topic, string message)
{
    mMqttClient.onMessage(topic, message);
}

int MqttServer::argToInt(int index)
{
    return mMqttClient.arg(index).toInt();
}


void MqttServer::handleClient()
{
    mMqttClient.handleClient();
}
} // namespace magic_car
