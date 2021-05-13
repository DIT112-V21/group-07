#pragma once

#include <functional>
#include <vector>
#include <iostream>

using namespace std;

namespace smartcar

{
struct Mqtt
{
    virtual ~Mqtt() = default;

    virtual void begin()                                                            = 0;
    virtual void beginExternal(string IP, int port)                                 = 0;
    virtual bool connect(string hostname, string username, string password)         = 0; //returns boolean
    virtual bool subscribe(string topic,int qos)                                    = 0;
    virtual bool publish(string topic, string message)                              = 0;
    virtual bool onMessage(string topic, string message)                            = 0; //returns boolean!!
    virtual int argToInt(int index)                                                 = 0;
    virtual void handleClient()                                                     = 0;
};
}  // namespace smartcar