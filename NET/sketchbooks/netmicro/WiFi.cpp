#include "WiFi.h"
#include "constants.h"

WiFi::WiFi(StatusHandler * statusHandler, WiFiDevice * wifiDevice)
    : statusHandler(statusHandler),
      wifiDevice(wifiDevice) {}

void WiFi::initialize() {
    Serial.println(F("Initializing WiFi"));
    bool initializationSucceeded = wifiDevice->initialize();
    if (initializationSucceeded) {
        Serial.println(F("WiFi initialization succeeded"));
        statusHandler->setStatus(OFFLINE);
    } else {
        Serial.println(F("WiFi initialization failed"));
        statusHandler->setStatus(FAILED);
    }
}

void WiFi::wpa2Connect() {
    Serial.println(F("Connecting to WPA2 WiFi"));
    statusHandler->setStatus(CONNECTING);
    bool connectionSucceeded = wifiDevice->wpa2Connect();
    if (connectionSucceeded) {
        Serial.println(F("WiFi connection succeeded"));
        statusHandler->setStatus(ONLINE);
    } else {
        Serial.println(F("WiFi connection failed"));
        statusHandler->setStatus(FAILED);
    }
}

void WiFi::httpGet() {
    Serial.println(F("HTTP GET"));
    statusHandler->setStatus(BUSY);
    bool getSucceeded = wifiDevice->httpGet();
    if (getSucceeded) {
        Serial.println(F("HTTP GET succeeded"));
        statusHandler->setStatus(ONLINE);
    } else {
        Serial.println(F("HTTP GET failed"));
        statusHandler->setStatus(FAILED);
    }
}
