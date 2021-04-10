#include <Smartcar.h>

//Pin definition and constants
int stopDistance = 70;
const int TRIGGER_PIN           = 6; // D6
const int ECHO_PIN              = 7; // D7
const unsigned int MAX_DISTANCE = 400;
const auto pulsesPerMeter = 600;

//Runtime environment
ArduinoRuntime arduinoRuntime;

//Motors
BrushedMotor leftMotor(arduinoRuntime, smartcarlib::pins::v2::leftMotorPins);
BrushedMotor rightMotor(arduinoRuntime, smartcarlib::pins::v2::rightMotorPins);

//Control
DifferentialControl control(leftMotor, rightMotor);


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
    car.setSpeed(20); // set the car speed to 20
}

void loop()
{
    
   
   int sensorDistance = frontUS.getDistance(); 
   if (sensorDistance !=0 ){ // if the sensor has readings .. 
        if ( sensorDistance <= stopDistance ){ // check if the sensor measurment is equal or less than the stopping distance (70 cm).
          car.setSpeed(0); // stop the car.
        }
   }
}
