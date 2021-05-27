/* This is the implementation of wrapper classes in header.
*/

#pragma once

// Libraries
#include <Arduino.h>
#include <MQTT.h>
#include <Smartcar.h>
#include <WiFi.h>

//Header
#include "Utils.h"


MQTTClient mqtt;
WiFiClient net;
ArduinoRuntime arduinoRuntime;

//Sensors
DistanceCar distanceCar;
SR04 ultraSound;
InfraredAnalogSensor infraredSensor;



// The following ArduinoWrapper that is calling the functions from above Arduino library.
struct ArduinoRunWrp : public ArduinoRunWrapper{

    long millis()override
    {
        return arduinoRuntime.millis();
    }

    void delay(int n)override
    {
        arduinoRuntime.delay(n);
    }

};

// The following UltraSoundWrapper that is calling the functions from above SmartCar library.
struct UltraSoundWrp : public UltraSoundWrapper{

    int getDistance()override{
        return ultraSound.getDistance();
    }

};

// The following InfraredWrapper that is calling the functions from above SmartCar library.
struct InfraredSensorWrp : public InfraredSensorWrapper{

    int getDistance()override{
        return infraredSensor.getDistance();
    }

};

// The following SerialWrapper that is calling the functions from above Arduino library.
struct SerialWrp: public SerialWrapper {
    void println(String message) override {
        Serial.println(message);
    }

    void begin(int n) override {
        Serial.begin(n);
    }

    bool available() override {
        Serial.available();
    }

    char readStringUntil(char singleQuotion) override {
        Serial.readStringUntil(singleQuotion);
    }
};

// The following MqttWrapper that is calling the functions from above MQTT library.
struct MqttWrp : public MqttWrapper {

  void beginLocal() override{

#ifdef __SMCE__
      mqtt.begin(WiFi);
#else
      mqttWrapper.beginLocal();
#endif
  }


  void beginExternal() override{

#ifdef __SMCE__
      mqtt.begin("test.mosquitto.org", 1883, WiFi);
#else
      mqtt.begin(net);
#endif
  }


  bool connected() override {
        return mqtt.connected();
    }

  bool loop() override {
        return mqtt.connected();
    }

  bool connect(String hostname, String id, String password) override {
    return mqtt.connect(hostname.c_str(), id.c_str(), password.c_str());
  }


  void subscribe(String topic, int qos) override {
      mqtt.subscribe(topic, qos);
  }

  void publish(String topic, String message) override {
      mqtt.subscribe(topic, message);
  }

  void onMessage(std::function<void(String, String)> callback) override {
    mqtt.onMessage(callback);
  }

};


// The following SmartCarWrapper that is calling the functions from above SmartCar library.
struct SmartCarWrp: public SmartCarWrapper{

    int getDistance() override{
        return distanceCar.getDistance();
    }

    void update() override{
        distanceCar.update()
    }

    void setAngle(int angle) override{
        distanceCar.setAngle(angle);
    }

    void setSpeed(float speed) override{
        distanceCar.setSpeed(speed);
    }

    float getSpeed() override{
        return distanceCar.getSpeed();
    }

};
