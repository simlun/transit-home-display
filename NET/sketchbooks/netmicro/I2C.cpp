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
    command = readSingleByte();
    if (numBytesReadFromMaster > 1) {
        handleReceiveCommand();
        command = NULL;
    }
}

void I2C::handleReceiveCommand() {
    ReceiveEventHandler *handler = getReceiveEventHandler(command);
    if (handler != NULL) {
        byte numberOfBytesRequested = handler->numberOfBytesRequested();
        if (numberOfBytesRequested == 1) {
            handler->handleByte(readSingleByte());
        } else if (numberOfBytesRequested > 1) {
            // Prepare buffer
            byte buffer[numberOfBytesRequested];
            for (byte i = 0; i < numberOfBytesRequested; i++) {
                buffer[i] = NULL;
            }

            // Read into buffer
            for (byte i = 0; Wire.available() && i < numberOfBytesRequested; i++) {
                buffer[i] = Wire.read();
            }
            readAndThrowAwayRest();

            handler->handleBytes(buffer);
        }
    } else {
        readAndThrowAwayRest();
        Serial.print(F("Unknown command: 0x"));
        Serial.println(command, HEX);
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
 * RequestEventHandler
 */

RequestEventHandler::RequestEventHandler(EventBus * eventBus) : eventBus(eventBus) {}


/**
 * ReceiveEventHandler
 */

ReceiveEventHandler::ReceiveEventHandler(EventBus * eventBus) : eventBus(eventBus) {}

void ReceiveEventHandler::handleByte(byte b) {}

void ReceiveEventHandler::handleBytes(byte buffer[]) {}
