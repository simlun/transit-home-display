#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>
#include "I2C.h"

class PingHandler : public RequestEventHandler {
    public:
        byte command();
        byte requestedByte();
};

class StatusHandler : public RequestEventHandler {
    public:
        byte command();
        byte requestedByte();
};

class ConnectHandler : public ReceiveEventHandler {
    public:
        byte command();
        byte numberOfBytesRequested();
        void handleByte(byte);
};

#endif
