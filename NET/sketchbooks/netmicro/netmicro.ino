#include <StandardCplusplus.h>
#include "MemoryFree.h"

#include "configuration.h"
#include "constants.h"

#include "I2C.h"
#include "Protocol.h"

#include "EventBus.h"
#include "EventBusHandlers.h"

#include "WiFi.h"
#include "AdafruitESP8266.h"

#include <Wire.h>


/**
 * Global Shared Instances
 */

EventBus eventBus;
I2C i2c(MY_I2C_ADDRESS);

PingHandler pingHandler;
StatusHandler statusHandler;
ConnectHandler connectHandler(&eventBus);

AdafruitESP8266 wifiDevice;
WiFi wifi(&statusHandler, &wifiDevice);

WPA2ConnectHandler wpa2ConnectHandler(&eventBus, &wifi);


/**
 * Register Event Handlers
 */

void registerEventBusHandlers() {
    eventBus.registerHandler(&wpa2ConnectHandler);
}

void registerProtocolHandlers() {
    i2c.registerRequestEventHandler(&pingHandler);
    i2c.registerRequestEventHandler(&statusHandler);
    i2c.registerReceiveEventHandler(&connectHandler);
}


/**
 * Arduino I2C
 */

void _onReceiveEvent(int numBytesReadFromMaster) {
    i2c.onReceiveEvent(numBytesReadFromMaster);
}

void _onRequestEvent() {
    i2c.onRequestEvent();
}

void registerI2CEventHandlers() {
    // TODO Move this to I2C class somehow?
    Wire.onReceive(_onReceiveEvent);
    Wire.onRequest(_onRequestEvent);
}


/**
 * Arduino
 */

void initializeSerial(void) {
    Serial.begin(115200);
    Serial.println(F("\nInitialized serial communication"));
}

void setup(void) {
    initializeSerial();

    registerEventBusHandlers();

    registerProtocolHandlers();

    registerI2CEventHandlers();
    i2c.initialize();

    printFreeMemory();

    statusHandler.setStatus(OFFLINE);
}

void loop(void) {
    eventBus.process();
    delay(0.1);
}
