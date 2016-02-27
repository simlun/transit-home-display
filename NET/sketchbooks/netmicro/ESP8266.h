#ifndef ADAFRUIT_ESP8266_H
#define ADAFRUIT_ESP8266_H

#include <Arduino.h>
#include <SoftwareSerial.h>

#include "WiFi.h"

class ESP8266 : public WiFiDevice {
    private:
        SoftwareSerial * softser;
        bool hardReset();
        bool sendValueUpdate(char *, char *, char *);
        bool sendVoidCommand(char *);
        bool sendVoidCommand(char *, unsigned long);
        bool sendVoidCommand(char *, unsigned long, unsigned int);
        bool sendAndExpectResponseLine(char *, char *);
        bool sendAndExpectResponseLine(char *, char *, bool);
        bool sendAndExpectResponseLine(char *, char *, bool, unsigned long);
        bool tcpConnect();
    public:
        ESP8266(SoftwareSerial *);
        bool initialize();
        bool connect();
        bool httpGet();
};

#endif
