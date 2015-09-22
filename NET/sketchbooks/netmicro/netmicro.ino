#include "MemoryFree.h"
#include "configuration.h"
#include "constants.h"
#include "I2C.h"
#include <Wire.h>


/**
 * Global Instances
 */

I2C i2c(MY_I2C_ADDRESS);


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

  registerI2CEventHandlers();
  i2c.initialize();

  printFreeMemory();
}

void loop(void) {
  //if (connectState == WPA2) {
    //wifiStatus = CONNECTING;
    //connectToWiFi();
    //displayConnectionDetails();
    //wifiStatus = ONLINE;
    //connectState = NULL;
  //}
  
  //foo();

  //disconnectFromWiFi();
  //wifiStatus = OFFLINE;
  
  
  delay(0.1);
}
