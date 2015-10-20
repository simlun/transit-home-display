#include "Protocol.h"
#include "constants.h"

// TODO Rename to I2CProtocol.{h, cpp}

/**
 * Ping
 */

byte PingHandler::command() {
    return PING;
}

byte PingHandler::requestedByte() {
    return PONG;
}


/**
 * Status
 */

StatusHandler::StatusHandler() {
    status = STARTING;
}

byte StatusHandler::command() {
    return STATUS;
}

byte StatusHandler::requestedByte() {
    return status;
}

void StatusHandler::setStatus(byte s) {
    // Expected s input values:
    // OFFLINE, ONLINE,
    // CONNECTING, DISCONNECTING,
    // BUSY,
    // FAILED, DEAD
    status = s;
}


/**
 * Passphrase
 */

PassphraseHandler::PassphraseHandler(EventBus * eventBus) : ReceiveEventHandler(eventBus) {}

byte PassphraseHandler::command() {
    return PASSPHRASE;
}

byte PassphraseHandler::numberOfBytesRequested() {
    // offset + 8 bytes = 9
    return 9;
}

void PassphraseHandler::handleBytes(byte buffer[]) {
    // TODO 1st byte is an offset
    // TODO 8 rest bytes are part of the passphrase
    // TODO Store it in EEPROM
    for (byte i = 0; i < numberOfBytesRequested(); i++) {
        Serial.print(i, DEC);
        Serial.print(":");
        Serial.println(buffer[i], HEX);
    }
    Serial.println("-");
    // TODO
}


/**
 * Connect
 */

ConnectHandler::ConnectHandler(EventBus * eventBus) : ReceiveEventHandler(eventBus) {}

byte ConnectHandler::command() {
    return CONNECT;
}

byte ConnectHandler::numberOfBytesRequested() {
    return 1;
}

void ConnectHandler::handleByte(byte b) {
    if (b == WPA2) {
        eventBus->post(WPA2);
    }
}
