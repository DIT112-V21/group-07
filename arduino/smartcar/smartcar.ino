#include <Smartcar.h>

const int TRIGGER_PIN           = 6; // D6
const int ECHO_PIN              = 7; // D7
const unsigned int MAX_DISTANCE = 1000;
const auto pulsesPerMeter = 600;
const float maxSpeedMs = 1.845;
const float STOPPING_SPEED = 0.2; //m/s. used to decide when to stop in slowDownSmoothly
const int SAFETY_RANGE_COEFF = 150; // multiply by car.getSpeed() to get a safety range

ArduinoRuntime arduinoRuntime;
BrushedMotor leftMotor(arduinoRuntime, smartcarlib::pins::v2::leftMotorPins);
BrushedMotor rightMotor(arduinoRuntime, smartcarlib::pins::v2::rightMotorPins);
DifferentialControl control(leftMotor, rightMotor);

SR04 front(arduinoRuntime, TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

GY50 gyroscope(arduinoRuntime, 37);

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

SmartCar car(arduinoRuntime, control, gyroscope, leftOdometer, rightOdometer);

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  car.update();
  if (isFrontClear()){
    handleInput();
  } else {
    Serial.print("Going to slow down at distance:");//TODO: REMOVE PRINT LINES
    Serial.println(front.getDistance()); 
    Serial.print("Going to slow down at speed:");
    Serial.println(car.getSpeed());
    slowDownSmoothly();
  }
  Serial.println(car.getSpeed());
}

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

boolean isFrontClear()
{
  float safetyDistance = car.getSpeed() * SAFETY_RANGE_COEFF;
  float frontUSDistance = front.getDistance();
  return (frontUSDistance > safetyDistance || frontUSDistance == 0 
          || leftOdometer.getDirection() == -1);//true if frontUS clear or moving backward
}


void slowDownSmoothly()
{
  while (car.getSpeed() >= STOPPING_SPEED){//check constant for details
    car.setSpeed(convertSpeed(car.getSpeed())/2);//cut speed down by 50%
  }
  car.setSpeed(0);
}

float convertSpeed(float currentSpeedMs) //parameter: car.getSpeed(). returns: percentage over maxSpeed
{
    return (currentSpeedMs/maxSpeedMs)*100;
}
