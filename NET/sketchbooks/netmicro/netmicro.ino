#include "MemoryFree.h"

#include "configuration.h"
#include "constants.h"


/**
 * Global Data
 */

byte registerPointer = NULL;
byte wifiStatus = STARTING;
byte connectState = NULL;

/**
 * Arduino
 */

void setup(void) {
  initializeSerial();
  initializeI2CSlave(MY_I2C_ADDRESS);

  initializeWiFi();
  wifiStatus = OFFLINE;
  
  printFreeMemory();
}

void loop(void) {
  if (connectState == WPA2) {
    wifiStatus = CONNECTING;
    connectToWiFi();
    displayConnectionDetails();
    wifiStatus = ONLINE;
    connectState = NULL;
  }
  
  //foo();

  //disconnectFromWiFi();
  //wifiStatus = OFFLINE;
  
  
  delay(0.1);
}
