#include <vector>
#ifdef __SMCE__
#include <OV767X.h>
#endif
#include <Smartcar.h>
#include <MQTT.h>
#include <WiFi.h>
#ifndef __SMCE__ // If the definition of SMCE then instantiate the WiFi client.
WiFiClient net;
#endif
MQTTClient mqtt;
const int FRONT_STOP_DISTANCE = 100;
const int BACK_STOP_DISTANCE = 50;
const int SIDE_REACT_DISTANCE = 35;
const int CLEAR_DISTANCE = 0;
const auto ONE_SECOND = 1000UL;
const auto HAlF_SECOND = 500UL;
const int FRONT_PIN = 0;
const int LEFT_PIN = 1;
const int RIGHT_PIN = 2;
const int BACK_PIN = 3;
const int TRIGGER_PIN = 6; // D6
const int ECHO_PIN = 7; // D7
const unsigned int MAX_DISTANCE = 400;
const auto PULSES_PER_METER = 600;
const float MAX_SPEED = 1.845;
const float STOPPING_SPEED = 0.2; //m/s. used to decide when to stop in slowDownSmoothly
const int SAFETY_RANGE_COEFF = 150; // multiply by car.getSpeed() to get a safety range

//Runtime environment
ArduinoRuntime arduinoRuntime;

//Motors
BrushedMotor leftMotor(arduinoRuntime, smartcarlib::pins::v2::leftMotorPins);
BrushedMotor rightMotor(arduinoRuntime, smartcarlib::pins::v2::rightMotorPins);

//Control
DifferentialControl control(leftMotor, rightMotor);

//Infrared sensors (all long range 25 - 120cm range)
GP2Y0A02 frontIR(arduinoRuntime, FRONT_PIN);
GP2Y0A02 rightIR(arduinoRuntime, RIGHT_PIN);
GP2Y0A02 leftIR(arduinoRuntime, LEFT_PIN);
GP2Y0A02 backIR(arduinoRuntime, BACK_PIN);

//Ultrasonic sensor
SR04 frontUS(arduinoRuntime, TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

//Header
GY50 gyroscope(arduinoRuntime, 37);

//Directional odometers

DirectionalOdometer leftOdometer{ // for MAC - use : () instead of {} -- windows use {}
        arduinoRuntime,
        smartcarlib::pins::v2::leftOdometerPins,
        []() { leftOdometer.update(); },
        PULSES_PER_METER};
DirectionalOdometer rightOdometer{
        arduinoRuntime,
        smartcarlib::pins::v2::rightOdometerPins,
        []() { rightOdometer.update(); },
        PULSES_PER_METER};

//Constructor of the SmartCar
SmartCar car(arduinoRuntime, control, gyroscope, leftOdometer, rightOdometer);

void setup()
{
    Serial.begin(9600);
  //Example: 
    // chose to connect to localhost or external

    connectHost(false); //choosing to connect to localhost.
  
    MQTTMessageInput();

}

/**
 * Main loop
 */
void loop()
{
   if (mqtt.connected() && isFrontClear()) { // check if the mqtt is connected .. needed if you connect through MQTT
        mqtt.loop();  // Also needed to keep soing the mqtt operations
     
        SR04sensorData(true, "/smartcar/ultrasound/front"); //publish sensor data every one second through MQTT
        measureDistance(true, "/smartcar/car/distance");
  }
  else {slowDownSmoothly();}
  
        handleInput();
        emergencyBrake();
        reactToSides();
}

/**
 * Handle user input/restrict movement into an obstacle
 */
void handleInput() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        //TODO: Look at how the mqtt com has been implemented and how it impacts this method
        if (input.startsWith("s")) {
            // front and back sensors and we look at the + or - for direction
            int inputSpeed = input.substring(1).toInt();
            if (inputSpeed > 0) {
                float frontValue = frontIR.getDistance();
                handleSpeedInput(frontValue, inputSpeed);
            } else if (inputSpeed < 0) {
                float backValue = backIR.getDistance();
                handleSpeedInput(backValue, inputSpeed);
            } else {
                car.setSpeed(0);
            }
        } else if (input.startsWith("a")) {
            // look at the angle + or - :  + -> right and - -> left
            int inputAngle = input.substring(1).toInt();
            if (inputAngle > 0) {
                float rightValue = rightIR.getDistance();
                handleAngleInput(rightValue, inputAngle);
            } else if (inputAngle < 0) {//get left sensor
                float leftValue = leftIR.getDistance();
                handleAngleInput(leftValue, inputAngle);
            } else {
                car.setAngle(0);
            }
            car.update();
        }
    }
}

/**
 * handleInput helper method for speed
 */
void handleSpeedInput(float distance, int inputSpeed){
    if(distance != 0){
        Serial.println("Obstacle detected in the direction you are trying to move");
    }else{
        car.setSpeed(inputSpeed);
    }
}

/**
 * handleInput helper method for angle
 */
void handleAngleInput(float angle, int inputAngle){
    if (angle != 0) {
        Serial.println("Obstacle detected in the direction you are trying to move");
    } else {
        car.setAngle(inputAngle);
    }
}
 /**
  * Brakes in case of emergency. Looks at the direction and reacts to the relevant sensors.
  * @return true if a reaction to sensor has been needed. False if no reaction.
  */
//TODO for the future: Make sure the situation where leftDirection and rightDirection are not equal that it we always want the behaviour described in the else part (following)
bool emergencyBrake(){
    int leftDirection = leftOdometer.getDirection();
    int rightDirection = rightOdometer.getDirection();
    if(leftDirection == 1 && rightDirection == 1){
        int frontSensorDistance = frontUS.getDistance();
        if(reactToSensor(frontSensorDistance, FRONT_STOP_DISTANCE)){
        return true;}
    }else if (leftDirection == -1 && rightDirection == -1){
        int backSensorDistance = backIR.getDistance();
        if(reactToSensor(backSensorDistance, BACK_STOP_DISTANCE)){
        return true;}
    }else{
        return false;
    }
}
/**
 * EmergencyBrake() helper method to react to sensor value
 * @return true if a reaction to sensor was engaged. False otherwise
 */
bool reactToSensor(int sensorDistance, int STOP_DISTANCE){
    if (sensorDistance != 0){ // if the sensor has readings ..
        if ( sensorDistance <= STOP_DISTANCE ){ // check if the sensor measurement is equal or less than the stopping distance
            car.setSpeed(0);// stop the car.
            return true;
        }
    }
    return false;
}
/**
 * Method to look and compare two sensor values (100ms apart) to know if an obstacle is coming towards the car
 */
void reactToSides() {
    float currentHeading = car.getHeading();
    float rightValue = rightIR.getDistance();
    float leftValue = leftIR.getDistance();
    if (rightValue < SIDE_REACT_DISTANCE && rightValue > 0) {
        delay(100);
        float newValue = rightIR.getDistance();
        if (newValue < rightValue && newValue > 0) {
            sideAvoidance(-45);
        }
    }
    if (leftValue < SIDE_REACT_DISTANCE && leftValue > 0) {
        delay(100);
        float newValue = leftIR.getDistance();
        if (newValue < leftValue && newValue > 0) {
            sideAvoidance(45);
        }
    }
}
/**
 * reactToSides() helper method that set the angle of the car at the opposite direction
 * if an obstacle is detected coming towards the car.
 */
void sideAvoidance(int newAngle){
    if (newAngle < 0){
        while(rightIR.getDistance() < 35 && rightIR.getDistance() > 0) {
            car.setAngle(newAngle);
            bool emergencyBeak = emergencyBrake();
            if(emergencyBrake){
                return;
            }
        }
    }else{
        while(leftIR.getDistance() < 35 && leftIR.getDistance() > 0){
            car.setAngle(newAngle);
            bool emergencyBeak = emergencyBrake();
            if(emergencyBrake()){
                return;
            }
        }
    }
    car.setAngle(0);
} //TODO : Need to be improved (can be simplified)

// Method to publish SR04 sensor Data
//example:
  // SR04 front(arduinoRuntime, TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);  this should be created in the header.
  // SR04sensorData (true, "/smartcar/ultrasound/front" , front); // ex how to use in loop method 
void SR04sensorData(boolean pubSensorData, String publishTopic){ 
      
  if(pubSensorData){
      const auto currentTime = millis();
      static auto previousTransmission = 0UL;

      if (currentTime - previousTransmission >= ONE_SECOND) {
        previousTransmission = currentTime;
        const auto distance = String(frontUS.getDistance());
        mqtt.publish(publishTopic, distance);  
      }

    }
}

// Method to publish odometer sensor Data
//ex:
// measureDistance (true, "/smartcar/car/distance"); // ex how to use in loop method 
void measureDistance(boolean pubCarDistance, String publishDistanceTopic){
      
  if(pubCarDistance){
      const auto currentTime = millis();
      static auto previousTransmission = 0UL;

      if (currentTime - previousTransmission >= HAlF_SECOND) {
        previousTransmission = currentTime;
        const auto distance = String(car.getDistance());
        mqtt.publish(publishDistanceTopic, distance);  
      }
    }
}


//Returns true if frontUS is clear (depending on car speed) or car is moving backward
boolean isFrontClear()
{
  float safetyDistance = car.getSpeed() * SAFETY_RANGE_COEFF;
  float frontUSDistance = frontUS.getDistance();
  return (frontUSDistance > safetyDistance || frontUSDistance == 0 
          || leftOdometer.getDirection() == -1);
}

//Needs to be used together with isFrontClear. Slows down the car until full stop.
void slowDownSmoothly()
{
  while (car.getSpeed() >= STOPPING_SPEED){//check constant for details
    car.setSpeed(convertSpeed(car.getSpeed())/2);//cut speed down by 50%
  }
  car.setSpeed(0);
}

//parameter: car.getSpeed(). returns: percentage over maxSpeed
float convertSpeed(float currentSpeedMs) 
{
    return (currentSpeedMs/MAX_SPEED)*100;   // check max speed. 
}

// in case of other host just set the IP and the port, local host is false by default.
void connectHost(boolean ifLocalhost){  
if (ifLocalhost){
    #ifdef __SMCE__
      mqtt.begin(WiFi);
    #else
      mqtt.begin(net);
    #endif
}else{
    #ifdef __SMCE__
      mqtt.begin("test.mosquitto.org", 1883, WiFi);
    #else
      mqtt.begin(net);
    #endif
     }
}

// Subscribing emulator to topics to interact with the car.
void MQTTMessageInput(){ 
  if (mqtt.connect("arduino", "public", "public")) {
    mqtt.subscribe("/smartcar/control/#", 1);
    mqtt.onMessage([](String topic, String message) {
      if (topic == "/smartcar/control/speed") {
        car.setSpeed(message.toInt());
      } else if (topic == "/smartcar/control/angle") {
        car.setAngle(message.toInt());
      } else {
        Serial.println(topic + " " + message);
      }
    });
  }
}

// Avoid over-using the CPU if we are running in the emulator
void noCPUoverload (){ 
#ifdef __SMCE__
  delay(35);
#endif
}
