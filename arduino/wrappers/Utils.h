#pragma once

#include <functional>

#if defined(ARDUINO)
#include <Arduino.h>
#else

#include <string>
using String = std::string;
#endif

const unsigned int kDefaultMaxDistance = 70;


enum class PinDirection
{
    kInput,
    kOutput
};


struct ArduinoRunTimeWrapper {
    virtual ~ArduinoRunTimeWrapper() = default;

    virtual void setPinDirection(int pin, PinDirection pinDirection) = 0;
    virtual void setPin(int pin)                                     = 0;
    virtual void clearPin(int pin)                                   = 0;
};


struct MqttWrapper {
  virtual ~MqttWrapper() = default;

  virtual bool connect(String hostname, String id, String password) = 0;
  virtual void subscribe(String topic, int qos) = 0;
  virtual void publish(String topic, String message) = 0;
  virtual void onMessage(std::function<void(String, String)> callback) = 0;
};



struct SmartCarWrapper {
    virtual ~SmartCarWrapper() = default;

    virtual float getSpeed()           = 0;
    virtual void setSpeed(float speed) = 0;
    virtual void setAngle(int angle)   = 0;
    virtual int getDistance()          = 0;
    virtual void update()              = 0;
};


struct UltraSoundWrapper {

    virtual ~UltraSoundWrapper() = default;

    virtual int getDistance()          = 0;

};

struct InfraredSensorWrapper {
    virtual ~InfraredSensorWrapper() = default;

    virtual int getDistance()          = 0;

};

struct SerialWrapper {
    virtual ~SerialWrapper() = default;

    //virtual float millis()                = 0;
    virtual void println(String message) = 0;
    virtual void begin(int n) = 0;
};

struct ArduinoRunWrapper {
    virtual ~ArduinoRunWrapper() = default;

    virtual long millis()                = 0;
    virtual void delay(int n)                = 0;
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

class SmartCarControllerWrapper : public SmartCarWrapper
{

public:
    SmartCarControllerWrapper(SmartCarWrapper& car, MqttWrapper& mqtt, ArduinoRunTimeWrapper& pinController);


    void setSpeed(float speed) override;
    float getSpeed() override;
    void setAngle(int angle) override;
    int getDistance()    override;
    void update() override;


private:
    SmartCarWrapper& mCar;
    MqttWrapper& mMqtt;
    ArduinoRunTimeWrapper& mPinController;
};

/**
 * Subscribing the car with the app so it can react to the different input from
 * the app. Used when connected to MQTT server.
 */


void MQTTMessageInput(MqttWrapper &mqtt, SerialWrapper &serial){
    if (mqtt.connect("arduino", "public", "public")) {
        mqtt.subscribe("/smartcar/control/#", 1);
        mqtt.subscribe("/smartcar/connectionLost", 1);

        mqtt.onMessage([&](String topic, String message) {
            // Check if connectionLost(Last will) topic is received
            if (topic == "/smartcar/connectionLost") {
                connectivityLoss();
            }
            if (topic == "/smartcar/control/speed") {
                handleSpeedTopic(stringToInt(message));
            } else if (topic == "/smartcar/control/angle") {
                handleAngleTopic(stringToInt(message));
            } else {
                serial.println(message);
            }
        });
    }
}


void SR04sensorData(bool pubSensorData,MqttWrapper &mqttWrapper){
    if (pubSensorData) {
        String message = "20";

        if (mqttWrapper.connect("arduino", "public", "public")) {
            mqttWrapper.publish("/smartcar/ultrasound/front", message);}
    }
}

/*void handleSpeedInput(float speed,SmartCarWrapper &car, MqttWrapper &mqtt){
    if (mqtt.connect("arduino", "public", "public")) {

        car.setSpeed(speed);
    }
}*/

void handleSpeedInput(int distance, int inputSpeed, SerialWrapper &serialWrapper, MqttWrapper &mqttWrapper, SmartCarWrapper &car) {
    if (distance != 0) {
        serialWrapper.println(
                "Obstacle detected in the direction you are trying to move");//void handleSpeedInput(float speed,SmartCarWrapper &car, MqttWrapper &mqtt);
    } else {
        if (inputSpeed < 0) {
            car.setSpeed(inputSpeed/2);
        }
    }
}

