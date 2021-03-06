#ifndef ADAFRUIT_ESP8266_H
#define ADAFRUIT_ESP8266_H

#include <Arduino.h>
#include <AltSoftSerial.h>

#include "WiFi.h"

// Configuration
#define ESP8266_DEFAULT_COMMAND_TIMEOUT 10000
#define ESP8266_ON_OFF_PIN 10

class ESP8266 : public WiFiDevice {
    private:
        AltSoftSerial * softser;
        Storage * ssidStorage;
        Storage * passphraseStorage;
        Storage * hostStorage;
        Storage * pathStorage;
        char * responseBuffer;
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
        ESP8266(AltSoftSerial *, Storage *, Storage *, Storage *, Storage *, char *);
        bool initialize();
        bool connect();
        bool httpGet();
};

#endif
