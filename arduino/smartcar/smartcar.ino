#include <Smartcar.h>

/*
Assignment - Basic obstacle avoidance
"Create simple Arduino sketch that performs basic obstacle avoidance with a Smartcar. Specifically, 
you need to make the Smartcar move forward until it detects an obstacle at a distance of your choosing."

Upon our understanding over the assignment, we concluded that smartcar move forward directly until it detects
an obstacle at a distance of our choosing which was 25cm.
*/

//Pin definition and constants
const int FRONT_PIN = 0;
const int LEFT_PIN = 1;
const int RIGHT_PIN = 2;
const int BACK_PIN = 3;
const int TRIGGER_PIN = 6; // D6
const int ECHO_PIN = 7; // D7
const unsigned int MAX_DISTANCE = 1000;
const auto pulsesPerMeter = 600;
const float maxSpeedMs = 1.845F;

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
DirectionalOdometer leftOdometer{
    arduinoRuntime,
    smartcarlib::pins::v2::leftOdometerPins,
    []() { leftOdometer.update(); },
    pulsesPerMeter};
DirectionalOdometer rightOdometer{
    arduinoRuntime,
    smartcarlib::pins::v2::rightOdometerPins,
    []() { rightOdometer.update(); },
    pulsesPerMeter};

//Constructor of the SmartCar
SmartCar car(arduinoRuntime, control, gyroscope, leftOdometer, rightOdometer);

void setup()
{
    Serial.begin(9600);
}

// Distance of our choice is 25cm from the wall.
void loop()
{
    car.setSpeed(70);
    while(!(frontUS.getDistance() == 0) && frontUS.getDistance() < 90 ){
        car.setSpeed(0);
        Serial.println(frontUS.getDistance());
    }
}