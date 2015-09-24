#ifndef EVENTBUSHANDLERS_H
#define EVENTBUSHANDLERS_H

#include <Arduino.h>
#include "EventBus.h"
#include "constants.h"

class WPA2ConnectHandler : public EventHandler {
    public:
        WPA2ConnectHandler(EventBus *);
        Event event();
        void handle();
};

#endif
