#include "I2C.h"
#include "constants.h"

#include <Wire.h>

I2C::I2C(byte address) : address(address) {}

void I2C::initialize() {
  Wire.begin(address);
}

void I2C::registerReceiveEventHandler(ReceiveEventHandler * handler) {
  receiveEventHandlers[handler->command()] = handler;
}

void I2C::registerRequestEventHandler(RequestEventHandler * handler) {
  requestEventHandlers[handler->command()] = handler;
}

void I2C::onReceiveEvent(int numBytesReadFromMaster) {
  if (command == NULL) {
    command = readSingleByte();
  } else {
    ReceiveEventHandler *handler = getReceiveEventHandler(command);
    // TODO Handle null handler case
    byte numberOfBytesRequested = handler->numberOfBytesRequested();
    if (numberOfBytesRequested == 1) {
      handler->handleByte(readSingleByte());
    } else if (numberOfBytesRequested > 1) {
      // TODO Read into a buffer and pass to handler->handleBytes(...)
    }
    command = NULL;
  }
  readAndThrowAwayRest();
}

void I2C::onRequestEvent() {
  RequestEventHandler *handler = getRequestEventHandler(command);
  // TODO Handle null handler case
  Wire.write(handler->requestedByte());
  command = NULL;
}

ReceiveEventHandler * I2C::getReceiveEventHandler(byte command) {
    return receiveEventHandlers[command];
}

RequestEventHandler * I2C::getRequestEventHandler(byte command) {
    return requestEventHandlers[command];
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
