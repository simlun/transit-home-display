#include "Protocol.h"
#include "constants.h"


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

byte StatusHandler::command() {
    return STATUS;
}

byte StatusHandler::requestedByte() {
    // TODO Not hard coded:
    // STARTING,
    // OFFLINE, ONLINE,
    // CONNECTING, DISCONNECTING,
    // BUSY,
    // FAILED, DEAD
    return STARTING;
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
    // TODO set some state of this object to the incoming byte b.
    // b will be WPA2 or DISCONNECT.
    if (b == WPA2) {
        eventBus->post(WPA2);
    }
}
