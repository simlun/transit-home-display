#include "I2C.h"
#include "constants.h"

#include <Wire.h>

/**
 * I2C
 */

I2C::I2C(byte address) : address(address) {}

void I2C::initialize() {
    Serial.println(F("Initializing I2C device"));
    Wire.begin(address);
}

void I2C::registerReceiveEventHandler(ReceiveEventHandler * handler) {
    Serial.println(F("Registering I2C receive event handler"));
    receiveEventHandlers[handler->command()] = handler;
}

void I2C::registerRequestEventHandler(RequestEventHandler * handler) {
    Serial.println(F("Registering I2C request event handler"));
    requestEventHandlers[handler->command()] = handler;
}

void I2C::onReceiveEvent(int numBytesReadFromMaster) {
    if (command == NULL) {
        command = readSingleByte();
    } else {
        handleReceiveCommand();
        command = NULL;
    }
    readAndThrowAwayRest();
}

void I2C::handleReceiveCommand() {
    ReceiveEventHandler *handler = getReceiveEventHandler(command);
    if (handler != NULL) {
        byte numberOfBytesRequested = handler->numberOfBytesRequested();
        if (numberOfBytesRequested == 1) {
            handler->handleByte(readSingleByte());
        } else if (numberOfBytesRequested > 1) {
            // TODO Read into a buffer and pass to handler->handleBytes(...)
        }
    }
}

void I2C::onRequestEvent() {
    RequestEventHandler *handler = getRequestEventHandler(command);
    if (handler != NULL) {
        Wire.write(handler->requestedByte());
    }
    command = NULL;
}

ReceiveEventHandler * I2C::getReceiveEventHandler(byte command) {
    if (receiveEventHandlers.count(command) == 1) {
        return receiveEventHandlers[command];
    } else {
        return NULL;
    }
}

RequestEventHandler * I2C::getRequestEventHandler(byte command) {
    if (requestEventHandlers.count(command) == 1) {
        return requestEventHandlers[command];
    } else {
        return NULL;
    }
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


/**
 * ReceiveEventHandler
 */

ReceiveEventHandler::ReceiveEventHandler(EventBus * eventBus) : eventBus(eventBus) {}
