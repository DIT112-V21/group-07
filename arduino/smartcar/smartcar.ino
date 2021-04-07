#include <Smartcar.h>

ArduinoRuntime arduinoRuntime;
BrushedMotor leftMotor(arduinoRuntime, smartcarlib::pins::v2::leftMotorPins);
BrushedMotor rightMotor(arduinoRuntime, smartcarlib::pins::v2::rightMotorPins);
DifferentialControl control(leftMotor, rightMotor);


GY50 gyroscope(arduinoRuntime, 37);

const auto pulsesPerMeter = 600;

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

SmartCar car(arduinoRuntime, control, gyroscope, leftOdometer, rightOdometer);

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    handleInput();
}

void handleInput()
{
  if (Serial.available()){

    String input = Serial.readStringUntil('\n');

    if(input.startsWith("s")){

      int inputSpeed = input.substring(1).toInt();
      car.setSpeed(inputSpeed);

    }else if(input.startsWith("a")){

      int inputAngle = input.substring(1).toInt();
      car.setAngle(inputAngle);

    }else if(input.
  }
}