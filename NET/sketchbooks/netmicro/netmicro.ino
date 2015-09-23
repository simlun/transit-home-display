#include "MemoryFree.h"

#include "configuration.h"
#include "constants.h"

#include "I2C.h"
#include "Protocol.h"

#include <Wire.h>


/**
 * Global Instances
 */

I2C i2c(MY_I2C_ADDRESS);


/**
 * My Event Handlers
 */

PingHandler pingHandler;
StatusHandler statusHandler;
ConnectHandler connectHandler;

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

  registerProtocolHandlers();

  registerI2CEventHandlers();
  i2c.initialize();

  printFreeMemory();
}

void loop(void) {
  // TODO Implement event loop
  delay(0.1);
}
