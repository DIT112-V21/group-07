#include "SmartCarController.h"
#include <iosfwd>
#include <cstdio>
#include <cstring>
using namespace std;
namespace smartcar
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


} // namespace magic_car
