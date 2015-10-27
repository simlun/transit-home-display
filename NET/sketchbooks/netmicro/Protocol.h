#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>
#include "I2C.h"
#include "EventBus.h"

class PingHandler : public RequestEventHandler {
    public:
        PingHandler(EventBus *);
        byte command();
        byte requestedByte();
};

class StatusHandler : public RequestEventHandler {
    private:
        byte status;
    public:
        StatusHandler(EventBus *);
        byte command();
        byte requestedByte();

        void setStatus(byte);
};

class SSIDHandler : public ReceiveEventHandler {
    public:
        SSIDHandler(EventBus *);
        byte command();
        byte numberOfBytesRequested();
        void handleBytes(byte[]);
};

class PassphraseHandler : public ReceiveEventHandler {
    public:
        PassphraseHandler(EventBus *);
        byte command();
        byte numberOfBytesRequested();
        void handleBytes(byte[]);
};

class ConnectHandler : public ReceiveEventHandler {
    public:
        ConnectHandler(EventBus *);
        byte command();
        byte numberOfBytesRequested();
        void handleByte(byte);
};

#endif
