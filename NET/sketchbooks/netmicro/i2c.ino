#include <Wire.h>

void initializeI2CSlave(int myAddress) {
  Serial.println(F("\nInitializing I2C slave"));
  Wire.onReceive(onReceiveEvent);
  Wire.onRequest(onRequestEvent);
  Wire.begin(myAddress);
}

void onReceiveEvent(int numBytesReadFromMaster) {
  byte firstByte = readSingleByte();
  if (registerPointer == NULL) {
    registerPointer = firstByte;
  }
  readAndThrowAwayRest();
}

void onRequestEvent() {
  if (registerPointer == PING) {
    Wire.write(PONG);
  } else {
    Wire.write(NULL);
  }
  registerPointer = NULL;
}

byte readSingleByte() {
  if (Wire.available()) {
    return Wire.read();
  } else {
    return 0;
  }
}

void readAndThrowAwayRest() {
  while (Wire.available()) {
    Wire.read();
  }
}
