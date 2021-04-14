#include <Smartcar.h>

//Pin definition and constants
const int FRONT_STOP_DISTANCE = 100;
const int BACK_STOP_DISTANCE = 50;
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

    //car.setSpeed(20); // set the car speed to 20
}

void loop()
{
        handleInput();
        emergencyBrake();
}



/*bool isClear() {
    int backValue = backIR.getDistance();
    int frontValue = frontIR.getDistance();
    int rightValue = rightIR.getDistance();
    int leftValue = leftIR.getDistance();
    Serial.println(backValue);
    return (backValue == 0 && frontValue == 0 && rightValue == 0 && leftValue == 0);
}*/

void reverseFunction(bool direction){
    int frontSensorDistance = frontUS.getDistance();
    int backSensorDistance = backIR.getDistance();

    if(direction){
        while(frontSensorDistance < FRONT_STOP_DISTANCE && frontSensorDistance > CLEAR_DISTANCE){
            car.setSpeed(-10);
            frontSensorDistance = frontUS.getDistance();
            if (frontSensorDistance >= FRONT_STOP_DISTANCE && frontSensorDistance > CLEAR_DISTANCE){
                car.setSpeed(0);
            }
        }
    }else{
        while(backSensorDistance < BACK_STOP_DISTANCE && backSensorDistance > CLEAR_DISTANCE){
            car.setSpeed(10);
            backSensorDistance = backIR.getDistance();
            if (backSensorDistance >= BACK_STOP_DISTANCE && backSensorDistance > CLEAR_DISTANCE){
                car.setSpeed(0);
            }
        }
    }

}

/* 1) Look at direction and angle so we know which sensors we focus on
 * 2) look at the selected sensors
 * 3) act accordingly
 * */
void handleInput() {
    if (Serial.available()) {

        String input = Serial.readStringUntil('\n');

        if (input.startsWith("s")) {
            // front and back sensors and we look at the + or -
            int inputSpeed = input.substring(1).toInt();
            if (inputSpeed > 0){
                float frontValue = frontIR.getDistance();
                if(frontValue != 0){
                    Serial.println("Obstacle detected in the direction you are trying to move");
                }else{
                    car.setSpeed(inputSpeed);
                }
            } else if (inputSpeed < 0){
                float backValue = backIR.getDistance();
                if(backValue != 0){
                    Serial.println("Obstacle detected in the direction you are trying to move");
                }else{
                    car.setSpeed(inputSpeed);
                }
            } else {
                car.setSpeed(inputSpeed);
            }
        } else if (input.startsWith("a")) {
             // look at the angle + or - + -> right and - left
            int inputAngle = input.substring(1).toInt();
            if (inputAngle > 0){
                float rightValue = rightIR.getDistance();
                if(rightValue != 0){
                    Serial.println("Obstacle detected in the direction you are trying to move");
                }else{
                    car.setAngle(inputAngle);
                }
            }else if(inputAngle < 0){
                //get left sensor
                float leftValue = leftIR.getDistance();
                if(leftValue != 0){
                    Serial.println("Obstacle detected in the direction you are trying to move");
                }else{
                    car.setAngle(inputAngle);
                }
            }else{
                car.setAngle(inputAngle);
            }
            car.update();
        }
    }
}

void emergencyBrake(){
    int leftDirection = leftOdometer.getDirection();
    int rightDirection = rightOdometer.getDirection();
    bool direction = false;

    if(leftDirection == 1 && rightDirection == 1){
        int frontSensorDistance = frontUS.getDistance();
        direction = true;
        if (frontSensorDistance !=0 ){ // if the sensor has readings ..
            if ( frontSensorDistance <= FRONT_STOP_DISTANCE ){ // check if the sensor measurement is equal or less than the stopping distance
                car.setSpeed(0);// stop the car.
                delay(2000);
            }
        }
    }else{
        int backSensorDistance = backIR.getDistance();
        if (backSensorDistance !=0 ){ // if the sensor has readings ..
            if ( backSensorDistance <= BACK_STOP_DISTANCE ){ // check if the sensor measurement is equal or less than the stopping distance
                car.setSpeed(0); // stop the car.
                delay(2000);
            }
        }
    }
    reverseFunction(direction);
}


























