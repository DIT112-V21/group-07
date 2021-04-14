  
#include <Smartcar.h>
#include <MQTT.h>
#include <WiFi.h>

#ifndef __SMCE__
WiFiClient net;
#endif
MQTTClient mqtt;

//Pin definition and constants
const auto oneSecond = 1000UL;
const int FRONT_PIN = 0;
const int LEFT_PIN = 1;
const int RIGHT_PIN = 2;
const int BACK_PIN = 3;
const int TRIGGER_PIN = 6; // D6
const int ECHO_PIN = 7; // D7
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
DirectionalOdometer leftOdometer{  // for MAC - use : () instead of {}
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
      //Ex: 
  // chose to connect to localhost or external

    connectHost(false); //choosing to connect to localhost.
  
    MQTTMessageInput();
}

void loop()
{
      if (mqtt.connected()) { // check if the mqtt is connected .. needed if you connect through MQTT
        mqtt.loop();  // Also needed to keep soing the mqtt operations
     
        SR04sensorData(true, "/smartcar/ultrasound/front"); //publish sensor data every one second through MQTT
      }
  
}


void connectHost(boolean ifLocalhost){ // in case of other host just set the IP and the port, local host is false by default. 

if (ifLocalhost){
    #ifdef __SMCE__
      mqtt.begin(WiFi);
    #else
      mqtt.begin(net);
    #endif
}else{
    #ifdef __SMCE__
      mqtt.begin("test.mosquitto.org", 1883, WiFi);
    #else
      mqtt.begin(net);
    #endif
     }
}

void SR04sensorData(boolean pubSensorData, String publishTopic){ // Method to publish SR04 sensor Data
      
  if(pubSensorData){

          //ex:
  // SR04 front(arduinoRuntime, TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);  this should be created in the header.

  // SR04sensorData (true, "/smartcar/ultrasound/front" , front); // ex how to use in loop method 
      const auto currentTime = millis();
      static auto previousTransmission = 0UL;

      if (currentTime - previousTransmission >= oneSecond) {
        previousTransmission = currentTime;
        const auto distance = String(frontUS.getDistance());
        mqtt.publish(publishTopic, distance);  
      }

    }
}

void MQTTMessageInput(){ // Subscribing emulator to topics to interact with the car.

  if (mqtt.connect("arduino", "public", "public")) {
    mqtt.subscribe("/smartcar/control/#", 1);
    mqtt.onMessage([](String topic, String message) {
      if (topic == "/smartcar/control/speed") {
        car.setSpeed(message.toInt());
      } else if (topic == "/smartcar/control/angle") {
        car.setAngle(message.toInt());
      } else {
        Serial.println(topic + " " + message);
      }
    });
  }
}

void noCPUoverload (){ // Avoid over-using the CPU if we are running in the emulator
#ifdef __SMCE__
  delay(35);
#endif
}