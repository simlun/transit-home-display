#include "I2C.h"
#include "constants.h"

#include <Wire.h>

I2C::I2C(byte address) : address(address) {}

void I2C::initialize() { 
  Wire.begin(address);
}

void I2C::onReceiveEvent(int numBytesReadFromMaster) {
  byte firstByte = readSingleByte();
  if (registerPointer == NULL) {
    registerPointer = firstByte;
  } else if (registerPointer == CONNECT) {
    connectState = firstByte;
    registerPointer = NULL;
  } else if (registerPointer == HOST) {
    // TODO https://www.arduino.cc/en/Reference/EEPROMPut
    // Read up to 32 bytes and store at EEPROM address 0x40
    //_readBytesAndStoreAtEEPROMAddress(32, 0x40);
    registerPointer = NULL;
  }
  readAndThrowAwayRest();
}

void I2C::onRequestEvent() {
  if (registerPointer == PING) {
    Wire.write(PONG);
  } else if (registerPointer == STATUS) {
    Wire.write(wifiStatus);
  } else {
    Wire.write(NULL);
  }
  registerPointer = NULL; 
}

byte I2C::readSingleByte() {
  if (Wire.available()) {
    return Wire.read();
  } else {
    return 0;
  }
}

void I2C::readAndThrowAwayRest() {
  while (Wire.available()) {
    Wire.read();
  }
}
