#ifndef ADAFRUIT_ESP8266_H
#define ADAFRUIT_ESP8266_H

#include <Arduino.h>
#include <AltSoftSerial.h>

#include "WiFi.h"

class AdafruitHuzzahESP8266 : public WiFiDevice {
    private:
        AltSoftSerial * softser;
        bool hardReset();
        bool sendVoidCommand(char *);
        bool sendCommandWithExpectedResponse(char *, char *);
    public:
        AdafruitHuzzahESP8266(AltSoftSerial *);
        bool initialize();
        bool connect();
        bool httpGet();
};

#endif
