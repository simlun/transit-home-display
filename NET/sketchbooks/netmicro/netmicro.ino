#include "MemoryFree.h"

#include "configuration.h"
#include "constants.h"


/**
 * Global Data
 */

byte registerPointer = NULL;
byte wifiStatus = STARTING;

  
/**
 * Arduino
 */

void setup(void) {
  initializeSerial();
  initializeI2CSlave(MY_I2C_ADDRESS);

  initializeWiFi();
  wifiStatus = OFFLINE;
  
  connectToWiFi();
  wifiStatus = ONLINE;
  displayConnectionDetails();
  
  //foo();
  //disconnectFromWiFi();
  //wifiStatus = OFFLINE;
  
  printFreeMemory();
}

void loop(void) {
  delay(1000);
}
