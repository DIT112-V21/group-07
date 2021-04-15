#include <Smartcar.h>

//Pin definition and constants
const int FRONT_STOP_DISTANCE = 100;
const int BACK_STOP_DISTANCE = 50;
const int SIDE_REACT_DISTANCE = 50;
const int CLEAR_DISTANCE = 0;
const int FRONT_PIN = 0;
const int LEFT_PIN = 1;
const int RIGHT_PIN = 2;
const int BACK_PIN = 3;
const int TRIGGER_PIN           = 6; // D6
const int ECHO_PIN              = 7; // D7
const unsigned int MAX_DISTANCE = 400;
const auto PULSES_PER_METER = 600;

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
DirectionalOdometer leftOdometer{
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
    // car.overrideMotorSpeed(100, -100);
}

void loop()
{
        handleInput();
        emergencyBrake();
        reactToSides();
}

void reactToSides() {
    float currentHeading = car.getHeading();
    float rightValue = rightIR.getDistance();
    float leftValue = leftIR.getDistance();
    if (currentHeading > 0) {
        if (rightValue < SIDE_REACT_DISTANCE) {
            delay(300);
            float newValue = rightIR.getDistance();
            if (newValue < rightValue) {
                sideAvoidance(-30);
            }
        }
    } else if (currentHeading < 0) {
        if (leftValue < SIDE_REACT_DISTANCE) {
            delay(300);
            float newValue = leftIR.getDistance();
            if (newValue < leftValue) {
                sideAvoidance(30);
            }
        }
    } else {
        if(rightValue < SIDE_REACT_DISTANCE && leftValue < SIDE_REACT_DISTANCE){
            car.setSpeed(0);
            Serial.println("Obstacle detected in the right and left direction, car stopped for safety");
        }
    }
}

void sideAvoidance(int newAngle){
    if (newAngle < 0){
        while(rightIR.getDistance() < 60){
            car.setAngle(newAngle);
            emergencyBrake();
            car.update();
            //for distance x, then turn back
        }
    }else{
        while(leftIR.getDistance() < 60){
            car.setAngle(newAngle);
            emergencyBrake();
            car.update();
        }
    }
    car.setAngle(0 - newAngle);

}

void handleInput() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        //TODO: Look at how the mqtt com has been implemented and how it impacts this method
        if (input.startsWith("s")) {
            // front and back sensors and we look at the + or - for direction
            int inputSpeed = input.substring(1).toInt();
            if (inputSpeed > 0) {
                float frontValue = frontIR.getDistance();
                handleSpeedInput(frontValue);
            } else if (inputSpeed < 0) {
                float backValue = backIR.getDistance();
                handleSpeedInput(backValue);
            }
        } else if (input.startsWith("a")) {
            // look at the angle + or - :  + -> right and - -> left
            // int inputAngle = input.substring(1).toInt();
            if (inputAngle > 0) {
                float rightValue = rightIR.getDistance();
                handleAngleInput(rightValue)
            } else if (inputAngle < 0) {//get left sensor
                float leftValue = leftIR.getDistance();
                handlehandleAngleInput(leftValue);
            }
            car.update();
        }
    }
}

void handleSpeedInput(float distance){
    if(distance != 0){
        Serial.println("Obstacle detected in the direction you are trying to move");
    }else{
        car.setSpeed(inputSpeed);
    }
}

void handleAngleInput(float angle){
    if (angle != 0) {
        Serial.println("Obstacle detected in the direction you are trying to move");
    } else {
        car.setAngle(inputAngle);
    }
}

void emergencyBrake(){
    int leftDirection = leftOdometer.getDirection();
    int rightDirection = rightOdometer.getDirection();

    if(leftDirection == 1 && rightDirection == 1){
        int frontSensorDistance = frontUS.getDistance();
        reactToSensor(frontSensorDistance, FRONT_STOP_DISTANCE);
    }else{ //TODO for the future: Make sure the situation where leftDirection and rightDirection are not equal that it we always want the behaviour described in the else part (following)
        int backSensorDistance = backIR.getDistance();
        reactToSensor(backSensorDistance, BACK_STOP_DISTANCE);
    }
}

void reactToSensor(int sensorDistance, int STOP_DISTANCE){
    if (sensorDistance != 0){ // if the sensor has readings ..
        if ( sensorDistance <= STOP_DISTANCE ){ // check if the sensor measurement is equal or less than the stopping distance
            //TODO: Add call to Gimmy's method (slow down smoothly) here! Possibly call emergency break into Gimmy's also to cover obstacle popping while adjusting speed
            car.setSpeed(0);// stop the car.
            delay(2000);
        }
    }
}


























