#include <EEPROM.h>

#include "Protocol.h"
#include "constants.h"

// TODO Rename to I2CProtocol.{h, cpp}

/**
 * Ping
 */

PingHandler::PingHandler(EventBus * eventBus) : RequestEventHandler(eventBus) {}

byte PingHandler::command() {
    return PING;
}

byte PingHandler::requestedByte() {
    return PONG;
}


/**
 * Status
 */

StatusHandler::StatusHandler(EventBus * eventBus) : RequestEventHandler(eventBus) {
    status = STARTING;
}

byte StatusHandler::command() {
    return STATUS;
}

byte StatusHandler::requestedByte() {
    eventBus->post(EEPROM_DEBUG);
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
 * SSID
 */

SSIDHandler::SSIDHandler(EventBus * eventBus) : ReceiveEventHandler(eventBus) {}

byte SSIDHandler::command() {
    return SSID;
}

byte SSIDHandler::numberOfBytesRequested() {
    // offset + 8 bytes = 9
    return 9;
}

void SSIDHandler::handleBytes(byte buffer[]) {
    byte address;
    byte offset = buffer[0];
    for (byte i = 1; i < numberOfBytesRequested(); i++) {
        address = EEPROM_OFFSET_SSID + offset * 8 + i - 1;
        if (EEPROM.read(address) != buffer[i]) {
            EEPROM.write(address, buffer[i]);
        }
    }
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
    byte address;
    byte offset = buffer[0];
    for (byte i = 1; i < numberOfBytesRequested(); i++) {
        address = EEPROM_OFFSET_PASSPHRASE + offset * 8 + i - 1;
        if (EEPROM.read(address) != buffer[i]) {
            EEPROM.write(address, buffer[i]);
        }
    }
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
