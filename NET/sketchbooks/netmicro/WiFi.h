#ifndef WIFI_H
#define WIFI_H

#include <Arduino.h>
#include "Protocol.h"

class WiFiDevice {
    public:
        virtual bool initialize() = 0;
        virtual bool wpa2Connect() = 0;
        virtual bool httpGet() = 0;
};

class WiFi {
    StatusHandler * statusHandler;
    WiFiDevice * wifiDevice;
    public:
        WiFi(StatusHandler *, WiFiDevice *);
        void initialize();
        void wpa2Connect();
        void httpGet();
};

#endif
