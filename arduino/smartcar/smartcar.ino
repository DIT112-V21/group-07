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

//Runtime environment
ArduinoRuntime arduinoRuntime;
//Motors
BrushedMotor leftMotor(arduinoRuntime, smartcarlib::pins::v2::leftMotorPins);
BrushedMotor rightMotor(arduinoRuntime, smartcarlib::pins::v2::rightMotorPins);
//Control
DifferentialControl control(leftMotor, rightMotor);
//Infrared sensors (all medium - 12 to 78cm)
GP2Y0A21 frontIR(arduinoRuntime, FRONT_PIN);
GP2Y0A21 rightIR(arduinoRuntime, RIGHT_PIN);
GP2Y0A21 leftIR(arduinoRuntime, LEFT_PIN);
GP2Y0A21 backIR(arduinoRuntime, BACK_PIN);
//Ultrasonic sensor
SR04 frontUS(arduinoRuntime, TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
//Heading sensor
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

void loop()
{

    if (frontUS.getDistance() > 80 || frontUS.getDistance()  == 0) {
        handleInput();
        Serial.println(frontUS.getDistance());
        //Disabled to not overload the serial console, Enable if you want to see the distance printed.
        //Serial.println(frontIR.getDistance());
        //Serial.println(leftIR.getDistance());
        //Serial.println(rightIR.getDistance());
        //Serial.println(backIR.getDistance());
        delay(100);
    }else{
        car.setSpeed(0);
        delay(1000);
        while(front.getDistance() < 80){
            car.setSpeed(-10);
        }
    }
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

    }
  }
}