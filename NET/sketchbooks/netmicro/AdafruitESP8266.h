#ifndef ADAFRUIT_ESP8266_H
#define ADAFRUIT_ESP8266_H

#include <Arduino.h>
#include "WiFi.h"

class AdafruitESP8266 : public WiFiDevice {
    public:
        bool wpa2Connect();
};

#endif
