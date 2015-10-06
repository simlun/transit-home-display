#ifndef ADAFRUIT_ESP8266_H
#define ADAFRUIT_ESP8266_H

#include <Arduino.h>
#include <SoftwareSerial.h>

#include "WiFi.h"

typedef const __FlashStringHelper Fstr; // PROGMEM/flash-resident string

class AdafruitHuzzahESP8266 : public WiFiDevice {
    private:
        SoftwareSerial * softser;
        bool hardReset();
        bool sendVoidCommand(Fstr *);
        bool sendCommandWithExpectedResponse(Fstr *, String);
    public:
        AdafruitHuzzahESP8266(SoftwareSerial *);
        bool initialize();
        bool wpa2Connect();
};

#endif
