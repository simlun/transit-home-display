#include "Protocol.h"
#include "constants.h"

byte PingHandler::command() {
    return PING;
}

byte PingHandler::requestedByte() {
    return PONG;
}


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


byte ConnectHandler::command() {
    return CONNECT;
}

byte ConnectHandler::numberOfBytesRequested() {
    return 1;
}

void ConnectHandler::handleByte(byte b) {
    // TODO set some state of this object to the incoming byte b.
    // b will be WPA2 or DISCONNECT.
}
