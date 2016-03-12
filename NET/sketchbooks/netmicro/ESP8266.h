#ifndef ADAFRUIT_ESP8266_H
#define ADAFRUIT_ESP8266_H

#include <Arduino.h>
#include <AltSoftSerial.h>

#include "WiFi.h"

class ESP8266 : public WiFiDevice {
    private:
        AltSoftSerial * softser;
        bool softReset();
        bool tcpConnect();
        bool sendHttpGetRequest();
        bool receiveHttpGetResponse();
        bool sendValueUpdate(char *, char *, char *);
        bool sendVoidCommand(char *);
        bool sendVoidCommand(char *, unsigned long);
        bool sendVoidCommand(char *, unsigned long, unsigned int);
        bool sendAndExpectResponseLine(char *, char *);
        bool sendAndExpectResponseLine(char *, char *, bool);
        bool sendAndExpectResponseLine(char *, char *, bool, bool);
        bool sendAndExpectResponseLine(char *, char *, bool, bool, unsigned long);
    public:
        ESP8266(AltSoftSerial *);
        bool initialize();
        bool connect();
        bool httpGet();
};

#define ESP8266_CHAR_BUFF_SIZE 64

#endif
