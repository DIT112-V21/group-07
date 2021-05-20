#pragma once
#include <Arduino.h>
#include <MQTT.h>
#include <Smartcar.h>
#include "Utils.h"


MQTTClient mqtt;
DistanceCar distanceCar;
SR04 ultraSound;
InfraredAnalogSensor infraredSensor;
ArduinoRuntime arduinoRuntime;


const unsigned int kDefaultMaxDistance = 70;
const auto ONE_SECOND = 1000UL;

SmartCarControllerWrapper::SmartCarControllerWrapper(SmartCarWrapper& car, MqttWrapper& mqttWrapper,
                                                     ArduinoRunTimeWrapper& pinController)
                                                     : mCar{car}
                                                     , mMqtt{mqtt}
                                                     , mPinController{pinController}
{
  mPinController.setPinDirection(kLightsPin, PinDirection::kOutput);
}

int SmartCarControllerWrapper::getDistance() {
    return distanceCar.getDistance();
}

void SmartCarControllerWrapper::update() {
    distanceCar.update()
}

void SmartCarControllerWrapper::setAngle(int angle) {
     distanceCar.setAngle(angle);
}

void SmartCarControllerWrapper::setSpeed(float speed) {
     distanceCar.setSpeed(speed);
}

float SmartCarControllerWrapper::getSpeed() {
    return distanceCar.getSpeed();
}





struct ArduinoRunTimeWrp : public ArduinoRunTimeWrapper{

    void setPinDirection(int pin, PinDirection pinDirection)
    {
        pinMode(pin, pinDirection == PinDirection::kInput ? INPUT : OUTPUT);
    }

    void setPin(int pin)
    {
        digitalWrite(pin, HIGH);
    }

    void clearPin(int pin)
    {
        digitalWrite(pin, LOW);
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

};


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

struct SmartCarWrp: public SmartCarWrapper{

    int getDistance() {
        return distanceCar.getDistance();
    }

    void update() {
        distanceCar.update()
    }

    void setAngle(int angle) {
        distanceCar.setAngle(angle);
    }

    void setSpeed(float speed) {
        distanceCar.setSpeed(speed);
    }

    float getSpeed() {
        return distanceCar.getSpeed();
    }

};


/*void handleSpeedInput(float speed,SmartCarWrapper &car, MqttWrapper &mqtt){
    if (mqtt.connect("arduino", "public", "public")) {

        car.setSpeed(speed);
    }
}*/
