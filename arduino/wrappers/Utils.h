#pragma once

#include <functional>

#if defined(ARDUINO)
#include <Arduino.h>
#else

#include <string>
using String = std::string;
#endif


const auto ONE_SECOND = 1000UL;
const int NO_OBSTACLE_VALUE = 0;
const int FRONT_STOP_DISTANCE = 70;
const int BACK_STOP_DISTANCE = 50;
const float STOPPING_SPEED = 0.3;
const int SIDE_REACT_DISTANCE = 35;
const float MAX_SPEED = 1.845;
float initialSpeed = 1.845;


struct ArduinoRunWrapper {
    virtual ~ArduinoRunWrapper() = default;

    virtual long millis()                = 0;
    virtual void delay(int n)            = 0;
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

    virtual void println(String message) = 0;
    virtual void begin(int n) = 0;
    virtual bool available() = 0;
    virtual char readStringUntil(char singleQuotion) = 0;
};


struct MqttWrapper {

  virtual ~MqttWrapper() = default;

  virtual void beginLocal() = 0;
  virtual void beginExternal() = 0;
  virtual bool connected() = 0;
  virtual bool loop() = 0;
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


float convertSpeed(float speed);

inline int stringToInt(String input) {
#if defined(ARDUINO)
  return input.toInt();
#else
  return std::stoi(input);
#endif
}


// Speed and Angle is not tested because they are tested for connectionLost and Control topics
void MQTTMessageInput(MqttWrapper &mqtt, SerialWrapper &serial){
    if (mqtt.connect("arduino", "public", "public")) {
        mqtt.subscribe("/smartcar/control/#", 1);
        mqtt.subscribe("/smartcar/connectionLost", 1);

        mqtt.onMessage([&](String topic, String message) {

            if (topic == "/smartcar/connectionLost") {
                //connectivityLoss();
            }
            if (topic == "/smartcar/control/speed") {
                //handleSpeedTopic(stringToInt(message));
            } else if (topic == "/smartcar/control/angle") {
                //handleAngleTopic(stringToInt(message));
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


void SR04sensorData(bool pubSensorData,MqttWrapper &mqttWrapper, SerialWrapper &serialWrapper
                    ,ArduinoRunWrapper &arduinoRunWrapper
                    ,UltraSoundWrapper &ultraSoundWrapper ){
    if(pubSensorData){

        const auto currentTime = arduinoRunWrapper.millis();
        static auto previousTransmission = 0UL;

        if (currentTime - previousTransmission >= ONE_SECOND) {
            previousTransmission = currentTime;
            int distance = ultraSoundWrapper.getDistance();
            mqttWrapper.publish("/smartcar/ultrasound/front", std::to_string(distance));
        }
    }
}



void handleSpeedInput(int distance, int inputSpeed, SerialWrapper &serialWrapper, MqttWrapper &mqttWrapper, SmartCarWrapper &car) {
    if (distance != 0) {
        serialWrapper.println(
               "Obstacle detected in the direction you are trying to move");
    } else {
        if (inputSpeed < 0) {
            car.setSpeed(inputSpeed/2);
        }
    }
}



void handleAngleInput(int distance, int inputAngle, SerialWrapper &serialWrapper, MqttWrapper &mqttWrapper, SmartCarWrapper &car){
    if (distance != 0) {
        serialWrapper.println("Obstacle detected in the direction you are trying to move");
    } else {
        car.setAngle(inputAngle);
    }
}



void handleInput_SpeedTopicPositive(String input, int speed,
                              SerialWrapper &serialWrapper, MqttWrapper &mqttWrapper,
                              SmartCarWrapper &car, InfraredSensorWrapper &InfraredSensor){
    if (serialWrapper.available()) {
        String givenNameS = "s";
        if (input.starts_with(givenNameS)) {
            if (speed > 0) {
                int frontValue = InfraredSensor.getDistance();
                handleSpeedInput(frontValue, speed, serialWrapper, mqttWrapper, car);
            }
        }
    }
}



void handleInput_SpeedTopicNegative(String input, int speed,
    SerialWrapper &serialWrapper, MqttWrapper &mqttWrapper,
            SmartCarWrapper &car, InfraredSensorWrapper &InfraredSensor){
        if (serialWrapper.available()) {
            String givenNameS = "s";
            if (input.starts_with(givenNameS)) {
                if (speed < 0) {
                    int backValue = InfraredSensor.getDistance();
                    handleSpeedInput(backValue, speed, serialWrapper, mqttWrapper, car);
                }
            }
        }
}



void handleInputStopCar(String input, SerialWrapper &serialWrapper, SmartCarWrapper &car){
    if (serialWrapper.available()) {
        String givenNameS = "s";
        if (!(input.starts_with(givenNameS))) {
            car.setSpeed(0);
        }
    }
}



void connectLocalHost(MqttWrapper &mqttWrapper){
    mqttWrapper.beginLocal();
}


void connectExternalHost(MqttWrapper &mqttWrapper){
    mqttWrapper.beginExternal();
}



bool isClear(int sensor, int distance){

    switch (sensor) {
        case 1:
            return (distance == NO_OBSTACLE_VALUE);
        /*case 2:
            return (sensor== NO_OBSTACLE_VALUE);

        case 3:
            return (sensor.getDistance() == NO_OBSTACLE_VALUE);
        case 4:
            return (sensor.getDistance() == NO_OBSTACLE_VALUE);
        case 5:
            return (sensor.getDistance() == NO_OBSTACLE_VALUE);*/
        default:
            return false;
    }
}



float slowDownSmoothly(float STOPPING_SPEED, float initialSpeed)
{
    if (initialSpeed >= STOPPING_SPEED){ // 0.3
        float setSpeed = convertSpeed(initialSpeed * 0.7);
        return setSpeed;
    }
    return 0;
}



float convertSpeed(float speed) {
    return (speed / MAX_SPEED) * 100;  //1.845
}



bool reactToSensor(int sensorDistance, int STOP_DISTANCE,
                   SmartCarWrapper &car, float STOPPING_SPEED, float initialSpeed){ //1.845
    if (sensorDistance != 0){
        if(sensorDistance > STOP_DISTANCE && sensorDistance <= 250){
            float setSpeed = slowDownSmoothly(STOPPING_SPEED, initialSpeed);
            car.setSpeed(setSpeed);
            return false;
        }else if ( sensorDistance <= STOP_DISTANCE ){
            car.setSpeed(0);
            return true;
        }
    }
    return false;
}



bool emergencyBrake(int leftDirection, int rightDirection,
                    int frontSensorDistance, int sensor, int newSensorValue,
                    int STOP_DISTANCE, SmartCarWrapper &car,
                    float STOPPING_SPEED, float initialSpeed){

    if(leftDirection == 1 && rightDirection == 1 && initialSpeed > 0){

        if(isClear(sensor, frontSensorDistance)){
            if(reactToSensor(newSensorValue, STOP_DISTANCE,
                        car, STOPPING_SPEED, initialSpeed)){
                return false;
            }
        }else{
            car.setSpeed(0);
            return true;
        }
    }else if (leftDirection == -1 && rightDirection == -1 && initialSpeed > 0){

        if(reactToSensor(newSensorValue, STOP_DISTANCE,
                                       car, STOPPING_SPEED, initialSpeed)){
            return true;
        }
    }
    return false;
}



void reactToSides(int sensor, int distance,int newDistance, int leftValue, int rightValue,
                  SmartCarWrapper &car) {


    if (rightValue < SIDE_REACT_DISTANCE && !isClear(sensor, distance)) { //rightIR

        if (newDistance < rightValue && !isClear(sensor,distance)) { //rightIR
           car.setAngle(-45);
        }
    }
    if (leftValue < SIDE_REACT_DISTANCE && !isClear(sensor, distance)) { //leftIR
        if (newDistance < leftValue && !isClear(sensor, distance)) { // leftIR
            car.setAngle(45);
        }
    }
}
