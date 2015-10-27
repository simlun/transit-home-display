#ifndef EVENTBUSHANDLERS_H
#define EVENTBUSHANDLERS_H

#include <Arduino.h>
#include "EventBus.h"
#include "constants.h"
#include "WiFi.h"

class EEPROMDebugHandler : public EventHandler {
    public:
        EEPROMDebugHandler(EventBus *);
        Event event();
        void handle();
};

class WPA2ConnectHandler : public EventHandler {
    private:
        WiFi * wifi;
    public:
        WPA2ConnectHandler(EventBus *, WiFi *);
        Event event();
        void handle();
};

#endif
