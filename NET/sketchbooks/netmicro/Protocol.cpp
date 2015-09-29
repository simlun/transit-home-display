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
