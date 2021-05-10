#pragma once
#include <stddef.h> // NOLINT(modernize-deprecated-headers)
#include <stdint.h> // NOLINT(modernize-deprecated-headers)

// Determine if we use an Espressif platform (i.e. an ESP32 or ESP8266 board).
// We need this since they are not fully compatible with the Arduino API
// and therefore we have to do some extra adaptations.
#if defined(ESP32) || defined(ESP8266)
#define ESP_BOARD
#endif

#include "InterruptCallback.hpp"

#ifdef ESP_BOARD
#if defined(ESP32)
#include "esp_attr.h"
#define STORED_IN_RAM IRAM_ATTR
#else // ESP8266
#include "c_types.h"
#define STORED_IN_RAM ICACHE_RAM_ATTR
#endif
#else // Other architectures
#define STORED_IN_RAM
#endif

class Runtime
{
public:
    virtual ~Runtime() = default;
    virtual void setPinDirection(uint8_t pin, uint8_t direction) = 0;
    virtual void setPinState(uint8_t pin, uint8_t state) = 0;
    virtual int getPinState(uint8_t pin) = 0;
    virtual int getAnalogPinState(uint8_t pin) = 0;
    virtual void setPWM(uint8_t pin, int dutyCycle) = 0;
    virtual void i2cInit() = 0;
    virtual void i2cBeginTransmission(uint8_t address) = 0;
    virtual size_t i2cWrite(uint8_t value) = 0;
    virtual uint8_t i2cEndTransmission() = 0;
    virtual uint8_t i2cRequestFrom(uint8_t address, uint8_t numberOfBytes) = 0;
    virtual int i2cAvailable() = 0;
    virtual int i2cRead() = 0;
    virtual int8_t pinToInterrupt(uint8_t pin) = 0;
    virtual unsigned long currentTimeMillis() = 0;
    virtual unsigned long currentTimeMicros() = 0;
    virtual void delayMillis(unsigned long milliseconds) = 0;
    virtual void delayMicros(unsigned int microseconds) = 0;
    virtual unsigned long getPulseDuration(uint8_t pin, uint8_t state, unsigned long timeout) = 0;
    virtual void setInterrupt(uint8_t pin, InterruptCallback callback, int mode) = 0;
    virtual uint8_t getLowState() const = 0;
    virtual uint8_t getHighState() const = 0;
    virtual uint8_t getOutputState() const = 0;
    virtual uint8_t getInputState() const = 0;
    virtual int getRisingEdgeMode() const = 0;
};
