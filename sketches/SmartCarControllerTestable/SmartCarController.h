#pragma once

#include "Car.h"
#include "PinController.h"
#include "Mqtt.h"

namespace smartcar
{
class SmartCarController
{
public:
    SmartCarController(Car& car,
                       Mqtt& mqttClient,
                       PinController& pinController);

    void mqttMessageInput(string topic, string message);
    void handleSpeedTopic(int input);
    void handleAngleTopic(int input);
    void handleInput();
    void handleSpeedInput(int distance, int inputSpeed);
    void handleAngleInout(int distance, int inputAngle));
    bool emergencyBreak();
    bool reactToSensor(int sensorDistance, int STOP_DISTANCE);
    bool isClear(std::string sensor);
    void slowDownSmoothly();
    void reactToSides();
    void sideAvoidance(int newAngle);
    void SR04SensorData(bool pubSensorData, std:: string publishSensorTopic);
    void measureDistance(bool pubCarDistance, std:: string publishDistanceTopic);
    void cameraData(bool pubCameraData);
    void startCamera();
    float convertSpeed(float currentSpeedMs);
    void connectHost(bool ifLocalHost);
    void update();

private:
    Car& mCar;
    Mqtt& mMqttClient;
    PinController& mPinController;
};
} // namespace smartcar
