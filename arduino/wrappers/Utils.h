/*
 * HOW: The implementation of unit testing!
 * The initial opinion of our testing started by being independent from "smartCar.ino" sketch.
 * That is why we need to refactor some of the methods in "smartCar.ino" into "utils.h" class (this class).
 * Due to leading information given by Dimitrios Platis and Ruthger, it is reasonable to implement these
 * refactored smartCar.ino 's methods in this header class.
 *
 * Third party libraries are not tested, however due to our functionalities that depends on the
 * third library functionalities we were forced to create "wrappers and abstracts" to able to use them
 * in our unit testing.
 *
 * Unit testing has been made by testing only the functionalities of the methods in smartCar.ino sketch.
*/

#pragma once

// This header is part of the function objects library and provides the standard hash function.
#include <functional>

// In case of Arduino, use Arduino syntax.
#if defined(ARDUINO)
#include <Arduino.h>
#else

// else use the syntax for C++
#include <string>
using String = std::string;
#endif

// Global variable declarations.
const auto ONE_SECOND = 1000UL;
const int NO_OBSTACLE_VALUE = 0;
const int FRONT_STOP_DISTANCE = 70;
const int BACK_STOP_DISTANCE = 50;
const float STOPPING_SPEED = 0.3;
const int SIDE_REACT_DISTANCE = 35;
const float MAX_SPEED = 1.845;
float initialSpeed = 1.845;

// Wrapper for ArduinoRunTime
struct ArduinoRunWrapper {
    virtual ~ArduinoRunWrapper() = default;

    virtual long millis()                = 0;
    virtual void delay(int n)            = 0;
};

// Wrapper for UltraSoundSensor
struct UltraSoundWrapper {

    virtual ~UltraSoundWrapper() = default;

    virtual int getDistance()          = 0;

};

//Wrapper for InfraredSensor
struct InfraredSensorWrapper {

    virtual ~InfraredSensorWrapper() = default;

    virtual int getDistance()          = 0;

};

// Wrapper for ArduinoSerial
struct SerialWrapper {

    virtual ~SerialWrapper() = default;

    virtual void println(String message) = 0;
    virtual void begin(int n) = 0;
    virtual bool available() = 0;
    virtual char readStringUntil(char singleQuotion) = 0;
};

//Wrapper for MQTT
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

//Wrapper for some of smartCar functionalities.
struct SmartCarWrapper {

    virtual ~SmartCarWrapper() = default;

    virtual float getSpeed()           = 0;
    virtual void setSpeed(float speed) = 0;
    virtual void setAngle(int angle)   = 0;
    virtual int getDistance()          = 0;
    virtual void update()              = 0;
};

float convertSpeed(float speed);

// In case of Arduino, use Arduino syntax,
inline int stringToInt(String input) {
#if defined(ARDUINO)
  return input.toInt();
#else // use the syntax for C++.
  return std::stoi(input);
#endif
}

// In this function is tested;
// connect() to "arduino", "public", "public",
// subscribe() to this "/smartcar/control/#" topic and subscribe() to this "/smartcar/connectionLost" topic.
// and onMessage() function is tested to callback().
// handleSpeedTopic() & handleAngleTopic() are commented out because they are tested in this method.
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

// In this function is tested;
// connect() to "arduino", "public", "public",
// publish() to this "/smartcar/ultrasound/front" topic.
void SR04sensorData(bool pubSensorData,MqttWrapper &mqttWrapper){
    if (pubSensorData) {
        String message = "20";

        if (mqttWrapper.connect("arduino", "public", "public")) {
            mqttWrapper.publish("/smartcar/ultrasound/front", message);}
    }
}


// In this function is tested;
// publish() to this "/smartcar/ultrasound/front" topic default value of the UltraSoundSensor(0) for every second.
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

/* Note: Because of the size of the initial method handleInput(), the method is broken down to smaller parts */
// ------------------------ broken down methods --------------------------------//

// In this function is tested;
// setSpeed() divided the car speed by 2.
void handleSpeedInput(int distance, int inputSpeed, SerialWrapper &serialWrapper, MqttWrapper &mqttWrapper, SmartCarWrapper &car) {
    if (distance != 0) {
        serialWrapper.println("Obstacle detected in the direction you are trying to move");
    } else {
        if (inputSpeed < 0) {
            car.setSpeed(inputSpeed/2);
        }
    }
}

// In this function is tested;
// setAngle() will set the cars angle according to the input.
void handleAngleInput(int distance, int inputAngle, SerialWrapper &serialWrapper, MqttWrapper &mqttWrapper, SmartCarWrapper &car){
    if (distance != 0) {
        serialWrapper.println("Obstacle detected in the direction you are trying to move");
    } else {
        car.setAngle(inputAngle);
    }
}
// ------------------------ end of broken down methods -------------------------------- //



/* Note: the method is broken down to smaller parts */
// ------------------------ broken down methods --------------------------------//
// In this function is tested;
// handleSpeedInput() in case of input start with "s" we check first if there is an obstacle in the front when the car is moving forward,
// we handle the car speed.
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

// In this function is tested;
// handleSpeedInput() in case of input start with "s" we check first if there is an obstacle in the back when the car is moving backward,
// we handle the car speed.
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

// In this function is tested;
// handleSpeedInput() in case of input doesn't start with "s"
// we stop the car.
void handleInputStopCar(String input, SerialWrapper &serialWrapper, SmartCarWrapper &car){
    if (serialWrapper.available()) {
        String givenNameS = "s";
        if (!(input.starts_with(givenNameS))) {
            car.setSpeed(0);
        }
    }
}

// ------------------------ end of broken down methods -------------------------------- //




/* Note: the method is broken down to smaller parts */
// ------------------------ broken down methods --------------------------------//
// In this function is tested;
// to connect to the local host.
void connectLocalHost(MqttWrapper &mqttWrapper){
    mqttWrapper.beginLocal();
}

// to connect to the external host "test.mosquitto.org", 1883, WiFi.
void connectExternalHost(MqttWrapper &mqttWrapper){
    mqttWrapper.beginExternal();
}
// ------------------------ end of broken down methods -------------------------------- //



// The functionality is tested. distance=sensor
// The commented cases are not tested because they have the same functionality as the first case.
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

// In this function is tested;
// We test to convert speed to slow down smoothly.
float slowDownSmoothly(float stoppingSpeed, float initial_Speed)
{
    if (initial_Speed >= stoppingSpeed){ // 0.3
        float setSpeed = convertSpeed(initial_Speed * 0.7);
        return setSpeed;
    }
    return 0;
}


// the helper method for above slowDownSmoothly method.
float convertSpeed(float speed) {
    return (speed / MAX_SPEED) * 100;  //1.845
}

// In this function is tested;
// We tested to reacting to sensor depending on sensor measurements.
bool reactToSensor(int sensorDistance, int STOP_DISTANCE,
                   SmartCarWrapper &car, float stoppingSpeed, float initial_Speed){ //1.845
    if (sensorDistance != 0){
        if(sensorDistance > STOP_DISTANCE && sensorDistance <= 250){
            float setSpeed = slowDownSmoothly(stoppingSpeed, initial_Speed);
            car.setSpeed(setSpeed);
            return false;
        }else if ( sensorDistance <= STOP_DISTANCE ){
            car.setSpeed(0);
            return true;
        }
    }
    return false;
}

// In this function is tested;
// We tested if we need to have emergency break in case of there is an obstacle.
bool emergencyBrake(int leftDirection, int rightDirection,
                    int frontSensorDistance, int sensor, int newSensorValue,
                    int STOP_DISTANCE, SmartCarWrapper &car,
                    float stoppingSpeed, float initial_Speed){

    if(leftDirection == 1 && rightDirection == 1 && initial_Speed > 0){

        if(isClear(sensor, frontSensorDistance)){
            if(reactToSensor(newSensorValue, STOP_DISTANCE,
                        car, stoppingSpeed, initial_Speed)){
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

// In this function is tested;
// We tested the car reaction if there is an obstacle on either sides.
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
