#include <Smartcar.h>

const int TRIGGER_PIN           = 6; // D6
const int ECHO_PIN              = 7; // D7
const unsigned int MAX_DISTANCE = 1000;
const auto pulsesPerMeter = 600;
const float maxSpeedMs = 1.845;

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
  float safetyDistance = car.getSpeed() * 200;
  //Add all sensors!
  if (front.getDistance() > safetyDistance || front.getDistance()  == 0) {
    handleInput();
    Serial.println(front.getDistance());
    delay(100);
  } else {
    slowDownSmoothly();
  }
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

void slowDownSmoothly()
{
  while (car.getSpeed() > 0) { //while moving forward
    car.setSpeed(convertSpeed(car.getSpeed())/2);//cut it down by 50%
    Serial.println(car.getSpeed());
    if (convertSpeed(car.getSpeed()) <= 10) { //if/when "slow enough" (10%)
      car.setSpeed(0);//stop
    }
  }
}

float convertSpeed(float currentSpeedMs)
{
    float result = (currentSpeedMs/maxSpeedMs)*100;
    return result;
}
