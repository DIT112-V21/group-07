#include <vector>
#ifdef __SMCE__
#include <OV767X.h>
#endif
#include <WiFi.h>
#include <Smartcar.h>
#include <MQTT.h>

#ifndef __SMCE__ // If the definition of SMCE then instantiate the WiFi client.
WiFiClient net;
#endif
MQTTClient mqtt;

const int NO_OBSTACLE_VALUE = 0;
const int FRONT_STOP_DISTANCE = 70;
const int BACK_STOP_DISTANCE = 50;
const int SIDE_REACT_DISTANCE = 35;
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
const float STOPPING_SPEED = 0.3; //m/s. used to decide when to stop in slowDownSmoothly

std::vector<char> frameBuffer;

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
/**
 * Setup the car:
 * - Activate the serial
 * - Connect the mqtt server (true for local, false for external. Check the Dashboard.java to make sure the app connect to the same server as the car
 * - Takes input from the app
 *
 * For testing, it is recommended using a local host when trying the app and using the serial when testing the car's behaviours.
 * To use the serial, comment out the connectHost() and MQTTMessageInput() methods in the setup().
 */

void setup()
{
    Serial.begin(9600);
  //Example:
    // chose to connect to localhost or external
    startCamera();
    connectHost(false); //choosing to connect to localhost.
    MQTTMessageInput();
}

/**
 * Main loop
 */
void loop()
{
   if (mqtt.connected()) { // check if the mqtt is connected .. needed if you connect through MQTT
        mqtt.loop();  // Also needed to keep doing the mqtt operations
        cameraData(true);
        SR04sensorData(true, "/smartcar/ultrasound/front"); //publish sensor data every one second through MQTT
        measureDistance(true, "/smartcar/car/distance");
   }else{
       handleInput();
   }
    emergencyBrake();
    reactToSides();
    noCPUoverload();
}

/**
 * Subscribing the car with the app so it can react to the different input from the app.
 * Used when connected to MQTT server.
 */
void MQTTMessageInput(){
    if (mqtt.connect("arduino", "public", "public")) {
        mqtt.subscribe("/smartcar/control/#", 1);
        mqtt.onMessage([](String topic, String message) {
            if (topic == "/smartcar/control/speed") {
                //car.setSpeed(message.toInt());
                handleSpeedTopic(message.toInt());
            } else if (topic == "/smartcar/control/angle") {
                //car.setAngle(message.toInt());
                handleAngleTopic(message.toInt());
            } else {
                Serial.println(topic + " " + message);
            }
        });
    }
}
/**
 * Helper method for MQTTMessageinput() that handles the input from the app received via MQTT.
 * @param input: takes the speed as an input
 */
void handleSpeedTopic(int input){
    // front and back sensors and we look at the + or - for direction
    //int inputSpeed = input.substring(1).toInt();
    if (input > 0) {
        int frontValue = frontIR.getDistance();
        handleSpeedInput(frontValue, input);
    } else if (input < 0) {
        int backValue = backIR.getDistance();
        handleSpeedInput(backValue, input);
    } else {
        car.setSpeed(0);
    }
    car.update();
}
/**
 * Helper method for MQTTMessageinput() that handles the input from the app received via MQTT.
 * @param input : takes the speed as an input
 */
void handleAngleTopic(int input){
    // look at the angle + or - :  + -> right and - -> left
    //int inputAngle = input.substring(1).toInt();
    if (input > 0) {
        int rightValue = rightIR.getDistance();
        handleAngleInput(rightValue, input);
    } else if (input < 0) {//get left sensor
        int leftValue = leftIR.getDistance();
        handleAngleInput(leftValue, input);
    } else {
        car.setAngle(0);
    }
    car.update();
}
/**
 * Method to control the car directly from the serial in the emulator.
 */
void handleInput() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        //TODO: Look at how the mqtt com has been implemented and how it impacts this method
        if (input.startsWith("s")) {
            // front and back sensors and we look at the + or - for direction
            int inputSpeed = input.substring(1).toInt();
            if (inputSpeed > 0) {
                int frontValue = frontIR.getDistance();
                handleSpeedInput(frontValue, inputSpeed);
            } else if (inputSpeed < 0) {
                int backValue = backIR.getDistance();
                handleSpeedInput(backValue, inputSpeed);
            } else {
                car.setSpeed(0);
            }
        } else if (input.startsWith("a")) {
            // look at the angle + or - :  + -> right and - -> left
            int inputAngle = input.substring(1).toInt();
            if (inputAngle > 0) {
                int rightValue = rightIR.getDistance();
                handleAngleInput(rightValue, inputAngle);
            } else if (inputAngle < 0) {//get left sensor
                int leftValue = leftIR.getDistance();
                handleAngleInput(leftValue, inputAngle);
            } else {
                car.setAngle(0);
            }
            car.update();
        }
    }
}

/**
 * helper for MQTT and Serial inputs taking care of the speed
 */
void handleSpeedInput(int distance, int inputSpeed){
    if(distance != 0){
        Serial.println("Obstacle detected in the direction you are trying to move");
    }else{
        if(inputSpeed < 0) {
            car.setSpeed(inputSpeed/2);
        }else{
            car.setSpeed(inputSpeed);
        }
    }
}

/**
 * helper for MQTT and Serial inputs taking care of the speed
 */
void handleAngleInput(int distance, int inputAngle){
    if (distance != 0) {
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
    float currentSpeed = car.getSpeed();
    if(leftDirection == 1 && rightDirection == 1 && currentSpeed > 0){
        int frontSensorDistance = frontUS.getDistance();
        if(isClear("frontIR")){
            if(reactToSensor(frontSensorDistance, FRONT_STOP_DISTANCE)){
                return true;}
        }else{
            car.setSpeed(0);
        }
    }else if (leftDirection == -1 && rightDirection == -1 && currentSpeed > 0){
        int backSensorDistance = backIR.getDistance();
        if(reactToSensor(backSensorDistance, BACK_STOP_DISTANCE)){
        return true;}
    }
    return false;
}

/**
 * EmergencyBrake() helper method to react to sensor value
 * @return true if a reaction to sensor was engaged. False otherwise
 */
bool reactToSensor(int sensorDistance, int STOP_DISTANCE){
    if (sensorDistance != 0){ // if the sensor has readings ..
        if(sensorDistance > STOP_DISTANCE && sensorDistance <= 250){
            slowDownSmoothly();
        }else if ( sensorDistance <= STOP_DISTANCE ){ // check if the sensor measurement is equal or less than the stopping distance
            car.setSpeed(0);// stop the car.
            return true;
        }
    }
    return false;
}

/**
 * @param sensor : takes the name of the sensor to be checked  as a String.
 * @Returns true if sensor does not detect an obstacle or if it is too close to be detected. False if an obstacle is detected within the range of the sensor.
 */
bool isClear(String sensor)
{
    if (sensor == "frontUS"){
        return (frontUS.getDistance() == NO_OBSTACLE_VALUE);
    } else if(sensor == "frontIR") {
        return (frontIR.getDistance() == NO_OBSTACLE_VALUE);
    }else if(sensor == "backIR"){
        return (backIR.getDistance() == NO_OBSTACLE_VALUE);
    }else if(sensor == "rightIR"){
        return (rightIR.getDistance() == NO_OBSTACLE_VALUE);
    }else if(sensor == "leftIR"){
        return (leftIR.getDistance() == NO_OBSTACLE_VALUE);
    }else{
        return false;
    }
}

/**
 * Slows down the car smoothly by dividing the speed by 3 until it reaches a safe speed to stop.
 */
void slowDownSmoothly()
{
    if (car.getSpeed() >= STOPPING_SPEED){//check constant for details
        car.setSpeed(convertSpeed(car.getSpeed()) * 0.7);
    }
}

/**
 * Method to look and compare two sensor values (100ms apart) to know if an obstacle is coming towards the car
 */
void reactToSides() {
    int rightValue = rightIR.getDistance();
    int leftValue = leftIR.getDistance();
    if (rightValue < SIDE_REACT_DISTANCE && !isClear("rightIR")) {
        delay(100);
        float newValue = rightIR.getDistance();
        if (newValue < rightValue && !isClear("rightIR")) {
            sideAvoidance(-45);
        }
    }
    if (leftValue < SIDE_REACT_DISTANCE && !isClear("leftIR")) {
        delay(100);
        int newValue = leftIR.getDistance();
        if (newValue < leftValue && !isClear("leftIR")) {
            sideAvoidance(45);
        }
    }
}

/**
 * reactToSides() helper method that set the angle of the car at the opposite direction
 * if an obstacle is detected coming towards the car.
 */
void sideAvoidance(int newAngle){
    //Serial.println(newAngle);
    if (newAngle < 0){
        int rightIRDistance = rightIR.getDistance();
        while(rightIRDistance < SIDE_REACT_DISTANCE && !isClear("rightIR")) {
            car.setAngle(newAngle);
            rightIRDistance = rightIR.getDistance();
            car.update();
            if(emergencyBrake()){
                return;
            }
        }
    }else{
        int leftIRDistance = leftIR.getDistance();
        while(leftIRDistance < SIDE_REACT_DISTANCE && !isClear("leftIR")){
            car.setAngle(newAngle);
            leftIRDistance = leftIR.getDistance();
            car.update();
            if(emergencyBrake()){
                return;
            }
        }
    }
    car.setAngle(0);
    car.update();
} //TODO : Need to be improved (can be simplified)

/**
 * Method to publish SR04 sensor Data
 * example:
 * SR04 front(arduinoRuntime, TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);  this should be created in the header.
 * SR04sensorData (true, "/smartcar/ultrasound/front" , front); // ex how to use in loop method
 */
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

/**
 * Method to publish odometer sensor Data
 * ex:
 * measureDistance (true, "/smartcar/car/distance"); // ex how to use in loop method
 */
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

/**
 * @param : car.getSpeed()
 * @return : percentage over maxSpeed
 */
float convertSpeed(float currentSpeedMs) 
{
    return (currentSpeedMs/MAX_SPEED)*100;   // check max speed. 
}

/**
 * in case of other host just set the IP and the port, local host is false by default.
 * @param ifLocalhost : boolean checking if the the MQTT server is local or not
 */
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

void startCamera()
{
#ifdef __SMCE__
    Camera.begin(QVGA, RGB888, 15);
    frameBuffer.resize(Camera.width() * Camera.height() * Camera.bytesPerPixel()); //setup
#endif
}

// Method to publish Camera Data
//example:
// CameraData (true, "/smartcar/camera/front" , front); // ex how to use in loop method
void cameraData(boolean pubCameraData)
{
    if (pubCameraData)
    {
        const auto currentTime = millis();
#ifdef __SMCE__
        static auto previousFrame = 0UL;
        if (currentTime - previousFrame >= 65)
        {
            previousFrame = currentTime;
            Camera.readFrame(frameBuffer.data());
            mqtt.publish("/smartcar/camera", frameBuffer.data(), frameBuffer.size(), false, 0);
        }
#endif
    }
}


/**
 * Avoid over-using the CPU if we are running in the emulator
 */
void noCPUoverload (){ 
#ifdef __SMCE__
  delay(35);
#endif
}
