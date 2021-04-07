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
//Infrared sensors (all medium - 12 to 78cm) backIR = 25 - 120cm range
GP2Y0A21 frontIR(arduinoRuntime, FRONT_PIN);
GP2Y0A21 rightIR(arduinoRuntime, RIGHT_PIN);
GP2Y0A21 leftIR(arduinoRuntime, LEFT_PIN);
GP2Y0A02 backIR(arduinoRuntime, BACK_PIN);
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
//Main loop method.
// TODO *Alex & Chris* check reaction to obstacles on the sides.
// TODO *Alex & Chris* change while condition (local var or a bool method (if value is between x, return true) would probably be more efficient).
void loop()
{
    char sensor = checkSensors();
    switch(sensor){
        case 'f':
            { car.setSpeed(0);
            delay(1000);
            while(frontUS.getDistance() > 0 && frontUS.getDistance() < 80){
                car.setSpeed(-10);
            }car.setSpeed(0);};
            break;
        case 'b':
            {car.setSpeed(0);
            delay(1000);
            while(backIR.getDistance() > 0 && backIR.getDistance() < 30 ){
                car.setSpeed(10);
            }car.setSpeed(0);};
            break;
        case 'l':
            {car.setSpeed(10);
            delay(1000);
            while(leftIR.getDistance() > 0 && leftIR.getDistance() < 30 ){
                car.setAngle(90);
            }car.setAngle(0);};
            break;
        case 'r':
            { car.setSpeed(10);
            delay(1000);
            while(rightIR.getDistance() > 0 && rightIR.getDistance() < 30) {
                car.setAngle(-90);
            }car.setAngle(0);};
            break;
        case 'x':
            { handleInput();
            Serial.println(frontUS.getDistance());
            //Disabled to not overload the serial console, Enable if you want to see the distance printed.
            //Serial.println(frontIR.getDistance());
            //Serial.println(leftIR.getDistance());
            //Serial.println(rightIR.getDistance());
            //Serial.println(backIR.getDistance());
            delay(100);
            }};

    }

void handleInput() {
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
//Method to check which sensor is being affected.
// TODO *Alex & Chris* char value to be changed to the pin number of each sensor.
// TODO *Alex & Chris* comparative values (80, 30) to be changed to a calculated value based on the speed (Longer range sensors are needed for this).
char checkSensors()
{
    char sensor;
    if(frontUS.getDistance() > 0 && frontUS.getDistance() < 80){
        sensor = 'f';
    }else if (backIR.getDistance() > 0 && backIR.getDistance() < 30){
        sensor = 'b';
    }else if (leftIR.getDistance() > 0 && leftIR.getDistance() < 30){
        sensor = 'l';
    }else if (rightIR.getDistance() > 0 && rightIR.getDistance() < 30){
        sensor = 'r';
    }else{
        sensor = 'x';
    }
    return sensor;
}



