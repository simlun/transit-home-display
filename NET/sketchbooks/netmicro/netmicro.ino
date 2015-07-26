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
  //initializeWiFi();
  //connectToWiFi();
  //displayConnectionDetails();
  //foo();
  //disconnectFromWiFi();
}

void loop(void) {
 delay(100);
}
