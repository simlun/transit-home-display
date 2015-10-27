#ifndef I2C_H
#define I2C_H

#include <Arduino.h>

#include <StandardCplusplus.h>
#include <map>

#include "EventBus.h"

class RequestEventHandler {
    protected:
        EventBus * eventBus;
    public:
        RequestEventHandler(EventBus *);
        virtual byte command() = 0;
        virtual byte requestedByte() = 0;
};

class ReceiveEventHandler {
    protected:
        EventBus * eventBus;
    public:
        ReceiveEventHandler(EventBus *);
        virtual byte command() = 0;
        virtual byte numberOfBytesRequested() = 0;
        virtual void handleByte(byte);
        virtual void handleBytes(byte[]);
};

class I2C {
    public:
        I2C(byte);

        void initialize();

        void registerReceiveEventHandler(ReceiveEventHandler *);
        void registerRequestEventHandler(RequestEventHandler *);

        void onReceiveEvent(int);
        void onRequestEvent();
        void handleReceiveCommand();

        ReceiveEventHandler * getReceiveEventHandler(byte);
        RequestEventHandler * getRequestEventHandler(byte);

    private:
        I2C();
        byte address;

        std::map<byte, ReceiveEventHandler *> receiveEventHandlers;
        std::map<byte, RequestEventHandler *> requestEventHandlers;

        byte command;

        byte readSingleByte();
        void readAndThrowAwayRest();
};

#endif
