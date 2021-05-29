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

const int NO_OBSTACLE_VALUE = 0; //sensor value will be equal to this when no obstacle is detected.
const int FRONT_STOP_DISTANCE = 70; //value used for emergency brake (front)
const int BACK_STOP_DISTANCE = 50; //value used for emergency brake (back)
const int SIDE_REACT_DISTANCE = 35; //value used for emergency brake/side avoidance (sides)
const auto ONE_SECOND = 1000UL;
const auto HAlF_SECOND = 500UL;
const int FRONT_PIN = 0;
const int LEFT_PIN = 1;
const int RIGHT_PIN = 2;
const int BACK_PIN = 3;
const int TRIGGER_PIN = 6; // D6
const int ECHO_PIN = 7; // D7
const unsigned int MAX_DISTANCE = 400; //value used for slowDownSmoothly as the distance to react
const auto PULSES_PER_METER = 600;
const float MAX_SPEED = 1.845; //value used for the conversion of speed into percentage
const float STOPPING_SPEED = 0.3; //m/s. used to decide when to stop in slowDownSmoothly
const int PULL_OVER_DISTANCE = 250; //value used for connectivityLoss(), as how far the car pulls over
bool isParked = true;

//Sensor numbers used in isClear().
const int frontUS = 1;
const int frontIR = 2;
const int backIR = 3;
const int rightIR = 4;
const int leftIR = 5;

//Single image with the values of "r,g,b, and a" through MQTT.
std::vector<char> frameBuffer;

//Runtime environment
ArduinoRuntime arduinoRuntime;

//Motors
BrushedMotor leftMotor(arduinoRuntime, smartcarlib::pins::v2::leftMotorPins);
BrushedMotor rightMotor(arduinoRuntime, smartcarlib::pins::v2::rightMotorPins);

//Control
DifferentialControl control(leftMotor, rightMotor);

//Infrared sensors (all long range 25 - 120cm range)
GP2Y0A02 frontIRSensor(arduinoRuntime, FRONT_PIN);
GP2Y0A02 rightIRSensor(arduinoRuntime, RIGHT_PIN);
GP2Y0A02 leftIRSensor(arduinoRuntime, LEFT_PIN);
GP2Y0A02 backIRSensor(arduinoRuntime, BACK_PIN);

//Ultrasonic sensor
SR04 frontUSSensor(arduinoRuntime, TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

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
    startCamera(); // To initialize the camera as soon as the car starts rolling
    connectHost(true); //choose true to connect to localhost.

    MQTTMessageInput();
}

/**
 * Main loop
 */
void loop()
{

   if (mqtt.connected()) { // check if the mqtt is connected .. needed if you connect through MQTT
        mqtt.loop();  // Also needed to keep storing the mqtt operations
        cameraData(true); // True if camera is on, false otherwise.
        measureDistance(true, "/smartcar/odometer"); // publishes the traveled distance to the odometer topic
  }
    handleInput();
    emergencyBrake(true);
    reactToSides();
    noCPUoverload();
}

/**
 * Method called when receiving messages via MQTT from the server.
 * Message topics are checked and appropriate action taken.
 */
void MQTTMessageInput(){

    if (mqtt.connect("arduino", "public", "public")) {
        mqtt.subscribe("/smartcar/control/#", 1);
        mqtt.subscribe("/smartcar/connectionLost", 1);

        mqtt.onMessage([](String topic, String message) {

            //Check if connectionLost(Last will) topic is received
            if(topic == "/smartcar/connectionLost"){
                    connectivityLoss();
                }
            if (topic == "/smartcar/control/speed") {
                handleSpeedTopic(message.toInt());
            } else if (topic == "/smartcar/control/angle") {
                handleAngleTopic(message.toInt());
            } else {
                Serial.println(message);
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
    isParked = false;
    if (input > 0) {
        int frontValue = frontIRSensor.getDistance();
        handleSpeedInput(frontValue, input);
    } else if (input < 0) {
        int backValue = backIRSensor.getDistance();
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
    if (input > 0){
        int rightValue = rightIRSensor.getDistance();
        handleAngleInput(rightValue, input);
    } else if (input < 0) {
        int leftValue = leftIRSensor.getDistance();
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
        if (input.startsWith("s")) {
            // front and back sensors and we look at the + or - for direction
            int inputSpeed = input.substring(1).toInt();
            if (inputSpeed > 0) {
                int frontValue = frontIRSensor.getDistance();
                handleSpeedInput(frontValue, inputSpeed);
            } else if (inputSpeed < 0) {
                int backValue = backIRSensor.getDistance();
                handleSpeedInput(backValue, inputSpeed);
            } else {
                car.setSpeed(0);
            }
        } else if (input.startsWith("a")) {
            // look at the angle + or - :  + -> right and - -> left
            int inputAngle = input.substring(1).toInt();
            if (inputAngle > 0) {
                int rightValue = rightIRSensor.getDistance();
                handleAngleInput(rightValue, inputAngle);
            } else if (inputAngle < 0) {//get left sensor
                int leftValue = leftIRSensor.getDistance();
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
 * helper for MQTT and Serial inputs taking care of the angle
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
  * @param isSlowDown, send true if you want to use slowDownSmoothly()
  * @return true if a reaction to sensor has been needed. False if no reaction.
  */

bool emergencyBrake(bool isSlowDown){
    int leftDirection = leftOdometer.getDirection();
    int rightDirection = rightOdometer.getDirection();
    float currentSpeed = car.getSpeed();
    if(leftDirection == 1 && rightDirection == 1 && currentSpeed > 0){
        int frontSensorDistance = frontUSSensor.getDistance();
        if(isClear(frontIR)){
            if(reactToSensor(frontSensorDistance, FRONT_STOP_DISTANCE, isSlowDown)){
                mqtt.publish("/smartcar/cruisecontrol", "disable");
                return true;}
        }else{
            car.setSpeed(0);
            isParked = true;
            mqtt.publish("/smartcar/cruisecontrol", "disable");
        }
    }else if (leftDirection == -1 && rightDirection == -1 && currentSpeed > 0){
        int backSensorDistance = backIRSensor.getDistance();
        if(reactToSensor(backSensorDistance, BACK_STOP_DISTANCE, isSlowDown)){
        return true;}
    }
    return false;
}

/**
 * EmergencyBrake() helper method to react to sensor value
 * @return true if a reaction to sensor was engaged. False otherwise
 */
bool reactToSensor(int sensorDistance, int STOP_DISTANCE, bool isSlowDown){
    if (sensorDistance != 0){
        if(sensorDistance > STOP_DISTANCE && sensorDistance <= 250 && isSlowDown){
            slowDownSmoothly();
        }else if ( sensorDistance <= STOP_DISTANCE ){
            car.setSpeed(0);
            isParked = true;
            return true;
        }
    }
    return false;
}

/**
 * Instructions for the car to perform a "pulling over" maneuver, triggered when last will message is received.
 */
void connectivityLoss(){
    Serial.println("Connection to the app lost, pulling the vehicle over");
    //If obstacle is on the right side of the car
    if(rightIRSensor.getDistance() != NO_OBSTACLE_VALUE && !isParked){
        while(!isClear(rightIR)){
            handleSpeedTopic(30);
            if(emergencyBrake(false)){
                return;
            }
        }
    }
    if(car.getSpeed() > NO_OBSTACLE_VALUE){
        car.update();
        handleAngleTopic(35);
        float distance = car.getDistance();
        //Drive at an angle for a certain distance
        while(car.getDistance() < distance + PULL_OVER_DISTANCE) {
            handleSpeedTopic(30);
            if(emergencyBrake(false)){
                return;
            }
            reactToSides();
            car.update();
        }

        car.update();
        float distance2 = car.getDistance();
        //Return to original trajectory
        while(car.getDistance() < distance2 + PULL_OVER_DISTANCE) {
            handleAngleTopic(-35);
            if(emergencyBrake(false)){
                return;
            }
            reactToSides();
            car.update();
            //Stop when original trajectory is met
            if (car.getDistance() >= distance2 + PULL_OVER_DISTANCE) {
                handleSpeedTopic(0);
                handleAngleTopic(0);
                return;
            }
        }
        isParked = true;

    }
}

/**
 * @param sensor : receives the number of the sensor to be checked. Sensor numbers declared as constant class variables.
 * @Returns true if sensor does not detect an obstacle or if it is too close to be detected.
 * False if an obstacle is detected within the range of the sensor.
 */
bool isClear(int sensor){

    switch (sensor) {
        case 1:
            return (frontUSSensor.getDistance() == NO_OBSTACLE_VALUE);
        case 2:
            return (frontIRSensor.getDistance() == NO_OBSTACLE_VALUE);
        case 3:
            return (backIRSensor.getDistance() == NO_OBSTACLE_VALUE);
        case 4:
            return (rightIRSensor.getDistance() == NO_OBSTACLE_VALUE);
        case 5:
            return (leftIRSensor.getDistance() == NO_OBSTACLE_VALUE);
        default:
            return false;
    }
}

/**
 * Slows down the car smoothly by reducing speed by 30% until it reaches a safe speed to stop (stop performed by emergencyBrake()).
 */
void slowDownSmoothly()
{
    if (car.getSpeed() >= STOPPING_SPEED){
        car.setSpeed(convertSpeed(car.getSpeed()) * 0.7);
    }
}

/**
 * Method to look and compare two sensor values (100ms apart) to know if an obstacle is coming towards the car
 */
void reactToSides() {
    int rightValue = rightIRSensor.getDistance();
    int leftValue = leftIRSensor.getDistance();
    if (rightValue < SIDE_REACT_DISTANCE && !isClear(rightIR)) {
        delay(100);
        float newValue = rightIRSensor.getDistance();
        if (newValue < rightValue && !isClear(rightIR)) {
            sideAvoidance(-45);
        }
    }
    if (leftValue < SIDE_REACT_DISTANCE && !isClear(leftIR)) {
        delay(100);
        int newValue = leftIRSensor.getDistance();
        if (newValue < leftValue && !isClear(leftIR)) {
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
        int rightIRDistance = rightIRSensor.getDistance();
        while(rightIRDistance < SIDE_REACT_DISTANCE && !isClear(rightIR)) {
            car.setAngle(newAngle);
            rightIRDistance = rightIRSensor.getDistance();
            car.update();
            if(emergencyBrake(true)){
                return;
            }
        }
    }else{
        int leftIRDistance = leftIRSensor.getDistance();
        while(leftIRDistance < SIDE_REACT_DISTANCE && !isClear(leftIR)){
            car.setAngle(newAngle);
            leftIRDistance = leftIRSensor.getDistance();
            car.update();
            if(emergencyBrake(true)){
                return;
            }
        }
    }
    car.setAngle(0);
    car.update();
}

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
        const auto distance = String(frontUSSensor.getDistance());
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
 * Calculates current speed percentage.
 * @param : car.getSpeed()
 * @return : percentage over maxSpeed
 */
float convertSpeed(float currentSpeedMs) 
{
    return (currentSpeedMs/MAX_SPEED)*100;
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

/**
 * Initialize the video streaming
 */
void startCamera()
{
#ifdef __SMCE__
    Camera.begin(QVGA, RGB888, 15);
    frameBuffer.resize(Camera.width() * Camera.height() * Camera.bytesPerPixel()); //setup
#endif
}

/**
 * Method to publish Camera Data
 * @param pubCameraData
 */
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
