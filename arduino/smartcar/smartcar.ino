#include <Smartcar.h>

//Pin definition and constants
const int FRONT_PIN = 0;
const int LEFT_PIN = 1;
const int RIGHT_PIN = 2;
const int BACK_PIN = 3;
const int TRIGGER_PIN           = 6; // D6
const int ECHO_PIN              = 7; // D7
const unsigned int MAX_DISTANCE = 400;
const auto pulsesPerMeter = 600;
const float maxSpeedMs = 1.845;
const float STOPPING_SPEED = 0.2; //m/s. used to decide when to stop in slowDownSmoothly
const int SAFETY_RANGE_COEFF = 150; // multiply by car.getSpeed() to get a safety range

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
   if (isFrontClear()){
    handleInput();
  } else {
    slowDownSmoothly();
  }
}

//Serial input - will be adapted to work with the app
void handleInput()
{
  if (Serial.available()) {

    String input = Serial.readStringUntil('\n');

    if (input.startsWith("s")) {

      int inputSpeed = input.substring(1).toInt();
      car.setSpeed(inputSpeed);

    } else if (input.startsWith("a")) {

      int inputAngle = input.substring(1).toInt();
      car.setAngle(inputAngle);

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
    return (currentSpeedMs/maxSpeedMs)*100;
}
