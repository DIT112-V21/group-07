#include "SmartCarController.h"
#include <iosfwd>
#include <cstdio>
#include <cstring>

using namespace std;

namespace
{
const auto kPlainText = "text/plain";
const auto kSuccess   = 200;
const auto kError     = 404;
const auto kLightsPin = 2;

bool areEqual(const std::vector<char>& v, const char* c)
{
    return strcmp(v.data(), c) == 0;
}
} // namespace

namespace smartcar
{
SmartCarController::SmartCarController(Car& car,
                                       Mqtt& mqttClient,
                                       PinController& pinController)
    : mCar{car}
    , mMqttClient{mqttClient}
    , mPinController{pinController}
{
    mPinController.setPinDirection(kLightsPin, PinDirection::kOutput);
}

    void SmartCarController::mqttMessageInput(string topic, string message) {

        if (mMqttClient.connect("arduino", "public", "public")) {
            mMqttClient.subscribe("/smartcar/control/#", 1);
            mMqttClient.onMessage(topic, message) {
                if (topic == "/smartcar/control/speed") {
                    //car.setSpeed(message.toInt());
                    handleSpeedTopic(message.);
                } else if (topic == "/smartcar/control/angle") {
                    //car.setAngle(message.toInt());
                    handleAngleTopic(message.toInt());
                } else {
                    Serial.println(topic + " " + message);
                }
            });
        }
    }

    void SmartCarController::handleSpeedTopic(int input){
        if (input > 0) {
            int frontValue = frontIR.getDistance();
            handleSpeedInput(frontValue, input);
        } else if (input < 0) {
            int backValue = backIR.getDistance();
            handleSpeedInput(backValue, input);
        } else {
            car.setSpeed(0);
        }
        car.update();
    }

    void SmartCarController::handleAngleTopic(int input){

}
    void SmartCarController::handleInput(){

}
    void SmartCarController::handleSpeedInput(int distance, int inputSpeed){

}
    void SmartCarController::handleAngleInout(int distance, int inputAngle)){

}
    bool SmartCarController::emergencyBreak(){

}
    bool SmartCarController::reactToSensor(int sensorDistance, int STOP_DISTANCE){

}
    bool SmartCarController::isClear(std::string sensor){

}
    void SmartCarController::slowDownSmoothly(){

}
    void SmartCarController::reactToSides(){

}
    void SmartCarController::sideAvoidance(int newAngle){

}
    void SmartCarController::SR04SensorData(bool pubSensorData, std:: string publishSensorTopic){

}
    void SmartCarController::measureDistance(bool pubCarDistance, std:: string publishDistanceTopic){

}
    void SmartCarController::cameraData(bool pubCameraData){

}
    void SmartCarController::startCamera(){

}
    float SmartCarController::convertSpeed(float currentSpeedMs){

}
    void SmartCarController::connectHost(bool ifLocalHost){

}
    void SmartCarController::update(){
        mMqttClient.handleClient();
        mCar.update();
}
} // namespace magic_car
