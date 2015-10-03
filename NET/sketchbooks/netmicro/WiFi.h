#ifndef WIFI_H
#define WIFI_H

#include <Arduino.h>
#include "Protocol.h"

class WiFiDevice {
    public:
        virtual bool wpa2Connect() = 0;
};

class WiFi {
    StatusHandler * statusHandler;
    WiFiDevice * wifiDevice;
    public:
        WiFi(StatusHandler *, WiFiDevice *);
        void wpa2Connect();
};

#endif