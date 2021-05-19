#pragma once

#include <Arduino.h>
#include <MQTT.h>
#include <Smartcar.h>

#include "Utils.h"

MQTTClient mqtt;
HeadingCar headingCar;
DistanceCar distanceCar;
SR04 ultraSound;
InfraredAnalogSensor infraredSensor;
ArduinoRuntime arduinoRuntime;

struct MqttWrp : public MqttWrapper {
  bool connect(String hostname, String id, String password) override {
    return mqtt.connect(hostname.c_str(), id.c_str(), password.c_str());
  }

  void subscribe(String topic, int qos) override { mqtt.subscribe(topic, qos); }
  void publish(String topic, String message) override { mqtt.subscribe(topic, message); }
  void onMessage(std::function<void(String, String)> callback) override {
    mqtt.onMessage(callback);
  }
};

struct SmartCarWrp : public SmartCarWrapper{

    float getSpeed() {
        distanceCar.getSpeed();
    }

    void setSpeed(float speed) {
        distanceCar.setSpeed(speed);
    }
    void setAngle(int angle) {
        distanceCar.setAngle(angle);
    }
    int getHeading() {
        headingCar.getHeading();
    }
    int getDistance() {
        distanceCar.getDistance();
    }
    void update() {
        distanceCar.update();
    }
};

struct UltraSoundWrp : public UltraSoundWrapper{

    int getDistance(){
        ultraSound.getDistance();
    }

};

struct InfraredSensorWrp : public InfraredSensorWrapper{

    int getDistance(){
        infraredSensor.getDistance();
    }

};

struct SerialWrp: public SerialWrapper {
  void println(String message) override {
      Serial.println(message);
  }
    void begin(int n) override {
        Serial.begin(n);
    }


/*#if defined(ARDUINO)
  float millis() override {

        return millis();
      }
#endif*/

};


struct ArduinoRunWrp: public ArduinoRunWrapper {
    long millis() override {

#if defined(ARDUINO)
        return millis();
#endif
      }
        //arduinoRuntime.millis();
    void delay(int n) override {
        arduinoRuntime.delay(n);
    }

};

