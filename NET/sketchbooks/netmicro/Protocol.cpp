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

byte StatusHandler::getStatus() {
    return status;
}


/**
 * SSID
 */

SSIDHandler::SSIDHandler(EventBus * eventBus, Storage * storage)
    : ReceiveEventHandler(eventBus),
      storage(storage) {}

byte SSIDHandler::command() {
    return SSID;
}

byte SSIDHandler::numberOfBytesRequested() {
    // offset + 8 bytes = 9
    return 9;
}

void SSIDHandler::handleBytes(byte buffer[]) {
    storage->update(buffer);
}


/**
 * Passphrase
 */

PassphraseHandler::PassphraseHandler(EventBus * eventBus, Storage * storage)
    : ReceiveEventHandler(eventBus),
      storage(storage) {}

byte PassphraseHandler::command() {
    return PASSPHRASE;
}

byte PassphraseHandler::numberOfBytesRequested() {
    // offset + 8 bytes = 9
    return 9;
}

void PassphraseHandler::handleBytes(byte buffer[]) {
    storage->update(buffer);
}


/**
 * Host
 */

HostHandler::HostHandler(EventBus * eventBus, Storage * storage)
    : ReceiveEventHandler(eventBus),
      storage(storage) {}

byte HostHandler::command() {
    return HOST;
}

byte HostHandler::numberOfBytesRequested() {
    // offset + 8 bytes = 9
    return 9;
}

void HostHandler::handleBytes(byte buffer[]) {
    storage->update(buffer);
}


/**
 * Path
 */

PathHandler::PathHandler(EventBus * eventBus, Storage * storage)
    : ReceiveEventHandler(eventBus),
      storage(storage) {}

byte PathHandler::command() {
    return PATH;
}

byte PathHandler::numberOfBytesRequested() {
    // offset + 8 bytes = 9
    return 9;
}

void PathHandler::handleBytes(byte buffer[]) {
    storage->update(buffer);
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


/**
 * Do
 */

DoHandler::DoHandler(EventBus * eventBus) : ReceiveEventHandler(eventBus) {}

byte DoHandler::command() {
    return DO;
}

byte DoHandler::numberOfBytesRequested() {
    return 1;
}

void DoHandler::handleByte(byte b) {
    if (b == GET) {
        eventBus->post(GET);
    }
}


/**
 * Response
 */

ResponseHandler::ResponseHandler(EventBus * eventBus, char * responseBuffer) :
    RequestEventHandler(eventBus),
    responseBuffer(responseBuffer),
    responseBufferIndex(0) {}

byte ResponseHandler::command() {
    return RESPONSE;
}

byte ResponseHandler::requestedByte() {
    char c = responseBuffer[responseBufferIndex];
    if (c != '\0') {
        responseBufferIndex++;
    } else {
        responseBufferIndex = 0;
    }
    return (byte) c;
}
