#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>
#include "I2C.h"
#include "EventBus.h"

class PingHandler : public RequestEventHandler {
    public:
        byte command();
        byte requestedByte();
};

class StatusHandler : public RequestEventHandler {
    private:
        byte status;
    public:
        StatusHandler();
        byte command();
        byte requestedByte();

        void setStatus(byte);
};

class ConnectHandler : public ReceiveEventHandler {
    public:
        ConnectHandler(EventBus *);
        byte command();
        byte numberOfBytesRequested();
        void handleByte(byte);
};

#endif
