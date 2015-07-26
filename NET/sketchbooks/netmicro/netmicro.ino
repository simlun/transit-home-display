/**
 * Configuration
 */

#define MY_I2C_ADDRESS 0x07

/**
 * Constants
 */

// Register names:
#define NULL   0x00
#define PING   0x01
#define STATUS 0x02
#define URL    0x03
#define DATA   0x04

// Labels:
#define PONG 0x50


/**
 * Global Data
 */

byte registerPointer = NULL;


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
 delay(1000);
 Serial.println(F("\nState:"));
 Serial.print(F("registerPointer = 0x"));
 Serial.println(registerPointer, HEX);
}
