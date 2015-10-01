#ifndef ADAFRUIT_ESP8266_H
#define ADAFRUIT_ESP8266_H

#include <Arduino.h>
#include <SoftwareSerial.h>

#include "WiFi.h"

class AdafruitHuzzahESP8266 : public WiFiDevice {
    private:
        SoftwareSerial * softser;
        bool initialize();
        bool hardReset();
    public:
        AdafruitHuzzahESP8266(SoftwareSerial *);
        bool wpa2Connect();
};

#endif
