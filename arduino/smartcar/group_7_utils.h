#pragma once

#include <functional>

#if defined(ARDUINO)
#include <Arduino.h>
#else
#include <string>
using String = std::string;
#endif

struct MqttWrapper {
  virtual ~MqttWrapper() = default;

  virtual bool connect(String hostname, String id, String password) = 0;
  virtual void subscribe(String topic, int qos) = 0;
  virtual void onMessage(std::function<void(String, String)> callback) = 0;
};

struct SerialWrapper {
  virtual ~SerialWrapper() = default;

  virtual void println(String message) = 0;
};

#if defined(ARDUINO)
// TODO: Remove these forward declarations as you refactor code
void connectivityLoss();
void handleSpeedTopic(int);
void handleAngleTopic(int);
#else
inline void connectivityLoss() {}

inline void handleSpeedTopic(int) {}

inline void handleAngleTopic(int) {}
#endif

inline int stringToInt(String input) {
#if defined(ARDUINO)
  return input.toInt();
#else
  return std::stoi(input);
#endif
}

/**
 * Subscribing the car with the app so it can react to the different input from
 * the app. Used when connected to MQTT server.
 */
inline void MQTTMessageInput(MqttWrapper &mqtt, SerialWrapper &serial) {

  if (mqtt.connect("arduino", "public", "public")) {
    mqtt.subscribe("/smartcar/control/#", 1);
    mqtt.subscribe("/smartcar/connectionLost", 1);

    mqtt.onMessage([&](String topic, String message) {
      // Check if connectionLost(Last will) topic is received
      if (topic == "/smartcar/connectionLost") {
        connectivityLoss();
      }
      if (topic == "/smartcar/control/speed") {
        // car.setSpeed(message.toInt());
        // save speed and angle
        handleSpeedTopic(stringToInt(message));
      } else if (topic == "/smartcar/control/angle") {
        // car.setAngle(message.toInt());
        handleAngleTopic(stringToInt(message));
      } else {
        serial.println(message);
      }
    });
  }
}
