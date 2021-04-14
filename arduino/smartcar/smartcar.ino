#include <vector>

#include <MQTT.h>
#include <WiFi.h>
#ifdef __SMCE__
#include <OV767X.h>
#endif

#include <Smartcar.h>

#ifndef __SMCE__
WiFiClient net;
#endif
MQTTClient mqtt;

ArduinoRuntime arduinoRuntime;
BrushedMotor leftMotor(arduinoRuntime, smartcarlib::pins::v2::leftMotorPins);
BrushedMotor rightMotor(arduinoRuntime, smartcarlib::pins::v2::rightMotorPins);
DifferentialControl control(leftMotor, rightMotor);

SimpleCar car(control);

const auto oneSecond = 1000UL;
const auto triggerPin = 6;
const auto echoPin = 7;
const auto maxDistance = 400;
SR04 front(arduinoRuntime, triggerPin, echoPin, maxDistance);

std::vector<char> frameBuffer;


void setup() {

  Serial.begin(9600);

  //Ex: 
  // chose to connect to localhost or external

    //connectHost(true,"aerostun.dev",1883); //choosing to connect to localhost. 
  
    //MQTTMessageInput();
}

void loop() {

  if (mqtt.connected()) { // check if the mqtt is connected .. needed if you connect through MQTT
     mqtt.loop();  // Also needed to keep soing the mqtt operations
     
     //SR04sensorData(true, "/smartcar/ultrasound/front"); //publish sensor data every one second through MQTT
    
  }

}

void SR04sensorData(boolean pubSensorData, String publishTopic){  // Method to publish SR04 sensor Data 
      
  if(pubSensorData){

    //ex:
  // SR04 front(arduinoRuntime, TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);  this should be created in the header.

  // SR04sensorData (true, "/smartcar/ultrasound/front" , front); // ex how to use in loop method 
      const auto currentTime = millis();
      static auto previousTransmission = 0UL;

      if (currentTime - previousTransmission >= oneSecond) {
        previousTransmission = currentTime;
        const auto distance = String(front.getDistance());
        mqtt.publish(publishTopic, distance);  
      }
    }
}


/*void CameraData (boolean pubCameraData, String cameraTopic){  // Method to publish camera data

  //ex:
  // const auto currentTime = millis();
  // publishCameraData (true, currentTime, "/smartcar/camera");
  
    if (pubCameraData){
      
#ifdef __SMCE__
      const auto currentTime = millis();
      static auto previousFrame = 0UL;
      if (currentTime - previousFrame >= 65) {
        previousFrame = currentTime;
        Camera.readFrame(frameBuffer.data());
          
    }

#endif
    }

}*/

void connectHost(boolean ifLocalhost, String AddIP, int Hport){ // in case of other host just set the IP and the port, local host is false by default. 

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

/*void connectHost(boolean ifLocalhost, String IP, int port){ // in case of other host just set the IP and the port, local host is false by default. 

  if (ifLocalhost){connectLocalHost();}
  else {connectOthertHost(IP, port);} 

}


void connectLocalHost(){

#ifdef __SMCE__
  mqtt.begin(WiFi);
#else
  mqtt.begin(net);
#endif
}

void connectOthertHost(String IP, int port){

#ifdef __SMCE__
  mqtt.begin(IP, port, WiFi);
#else
  mqtt.begin(net);
#endif
}*/

void MQTTMessageInput(){

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