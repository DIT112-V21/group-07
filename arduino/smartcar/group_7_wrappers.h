#pragma once

#include <Arduino.h>
#include <MQTT.h>
#include <Smartcar.h>

#include "group_7_utils.h"

MQTTClient mqtt;

struct Group7MqttWrapper : public MqttWrapper {
  bool connect(String hostname, String id, String password) override {
    return mqtt.connect(hostname.c_str(), id.c_str(), password.c_str());
  }

  void subscribe(String topic, int qos) override { mqtt.subscribe(topic, qos); }

  void onMessage(std::function<void(String, String)> callback) override {
    mqtt.onMessage(callback);
  }
};

struct Group7SerialWrapper : public SerialWrapper {
  void println(String message) override { Serial.println(message); }
};
