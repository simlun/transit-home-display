#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>
#include "I2C.h"
#include "EventBus.h"
#include "Storage.h"

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
    private:
        Storage * storage;
    public:
        SSIDHandler(EventBus *, Storage *);
        byte command();
        byte numberOfBytesRequested();
        void handleBytes(byte[]);
};

class PassphraseHandler : public ReceiveEventHandler {
    private:
        Storage * storage;
    public:
        PassphraseHandler(EventBus *, Storage *);
        byte command();
        byte numberOfBytesRequested();
        void handleBytes(byte[]);
};

class HostHandler : public ReceiveEventHandler {
    private:
        Storage * storage;
    public:
        HostHandler(EventBus *, Storage *);
        byte command();
        byte numberOfBytesRequested();
        void handleBytes(byte[]);
};

class PathHandler : public ReceiveEventHandler {
    private:
        Storage * storage;
    public:
        PathHandler(EventBus *, Storage *);
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
