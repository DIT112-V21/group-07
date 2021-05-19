#include "Utils.h"


struct SmartController : public Controller{

    void MQTTMessageInput(MqttWrapper &mqtt, SerialWrapper &serial){
        if (mqtt.connect("arduino", "public", "public")) {
            mqtt.subscribe("/smartcar/control/#", 1);
            mqtt.subscribe("/smartcar/connectionLost", 1);

            mqtt.onMessage([&](String topic, String message) {
                // Check if connectionLost(Last will) topic is received
                if (topic == "/smartcar/connectionLost") {
                    connectivityLoss();
                }
                if (topic == "/smartcar/control/speed") {
                    // car.setSpeed(message.toInt());
                    // save speed and angle
                    handleSpeedTopic(stringToInt(message));
                } else if (topic == "/smartcar/control/angle") {
                    // car.setAngle(message.toInt());
                    handleAngleTopic(stringToInt(message));
                } else {
                    serial.println(message);
                }
            });
        }
    }
};
