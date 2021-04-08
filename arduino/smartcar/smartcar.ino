#include <Smartcar.h>

//Pin definition and constants
const int FRONT_PIN = 0;
const int LEFT_PIN = 1;
const int RIGHT_PIN = 2;
const int BACK_PIN = 3;
const int TRIGGER_PIN           = 6; // D6
const int ECHO_PIN              = 7; // D7
const unsigned int MAX_DISTANCE = 1000;
const auto pulsesPerMeter = 600;
const float maxSpeedMs = 1.845;

//Runtime environment
ArduinoRuntime arduinoRuntime;
//Motors
BrushedMotor leftMotor(arduinoRuntime, smartcarlib::pins::v2::leftMotorPins);
BrushedMotor rightMotor(arduinoRuntime, smartcarlib::pins::v2::rightMotorPins);
//Control
DifferentialControl control(leftMotor, rightMotor);
//Infrared sensors (all medium - 12 to 78cm) backIR = 25 - 120cm range
GP2Y0A21 frontIR(arduinoRuntime, FRONT_PIN);
GP2Y0A21 rightIR(arduinoRuntime, RIGHT_PIN);
GP2Y0A21 leftIR(arduinoRuntime, LEFT_PIN);
GP2Y0A02 backIR(arduinoRuntime, BACK_PIN);
//Ultrasonic sensor
SR04 frontUS(arduinoRuntime, TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

//Header
GY50 gyroscope(arduinoRuntime, 37);

//Directional odometers
DirectionalOdometer leftOdometer(
        arduinoRuntime,
        smartcarlib::pins::v2::leftOdometerPins,
        []() { leftOdometer.update(); },
        pulsesPerMeter);
DirectionalOdometer rightOdometer(
        arduinoRuntime,
        smartcarlib::pins::v2::rightOdometerPins,
        []() { rightOdometer.update(); },
        pulsesPerMeter);

//Constructor of the SmartCar
SmartCar car(arduinoRuntime, control, gyroscope, leftOdometer, rightOdometer);

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    if(isClear()){
        handleInput();
    }else{
        stop();
        delay(100);
        adjustCar();

    }
    delay(50);
}

void handleInput() {
    if (Serial.available()) {

        String input = Serial.readStringUntil('\n');

        if (input.startsWith("m")) {

            int inputSpeed = input.substring(1).toInt();
            car.setSpeed(inputSpeed);

        } else if (input.startsWith("t")) {

            int inputAngle = input.substring(1).toInt();
            car.setAngle(inputAngle);

        }
    }
}

void adjustCar(){
    if (frontUS.getDistance() <= 80 && frontUS.getDistance() != 0){
        while(frontUS.getDistance() > 0 && frontUS.getDistance() <= 100){
            car.setSpeed(-10);
        }
    } else if (backIR.getDistance() <= 30 && backIR.getDistance() != 0){
        while(backIR.getDistance() != 0){
            car.setSpeed(10);
        }
    }
    car.setSpeed (0);
}

void stop(){
    car.setSpeed(0);
}

boolean isClear(){
    return ((frontUS.getDistance() > 80 || frontUS.getDistance()== 0) && (backIR.getDistance() > 30 || backIR.getDistance() == 0));
}