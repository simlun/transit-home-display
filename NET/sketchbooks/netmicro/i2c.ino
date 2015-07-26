#include <Wire.h>

void initializeI2CSlave(int myAddress) {
  Serial.println(F("\nInitializing I2C slave"));
  Wire.onReceive(_onReceiveEvent);
  Wire.onRequest(_onRequestEvent);
  Wire.begin(myAddress);
}

void _onReceiveEvent(int numBytesReadFromMaster) {
  byte firstByte = _readSingleByte();
  if (registerPointer == NULL) {
    registerPointer = firstByte;
  }
  _readAndThrowAwayRest();
}

void _onRequestEvent() {
  if (registerPointer == PING) {
    Wire.write(PONG);
  } else if (registerPointer == STATUS) {
    Wire.write(wifiStatus);
  } else {
    Wire.write(NULL);
  }
  registerPointer = NULL;
}

byte _readSingleByte() {
  if (Wire.available()) {
    return Wire.read();
  } else {
    return 0;
  }
}

void _readAndThrowAwayRest() {
  while (Wire.available()) {
    Wire.read();
  }
}
