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

// TODO: Rename this to simply "connect"? Because httpGet does not know about
// different connection methods. Also later when connecting via GPRS this isn't
// even WiFi. Let's simplify the API by defaulting to WPA2 when using WiFi. A
// WPA variant could easily be implemented instead if needed.
void WiFi::wpa2Connect() {
    Serial.println(F("Connecting to WPA2 WiFi"));

    // Re-initialize if status isn't OFFLINE
    byte tries = 0;
    for (byte s = statusHandler->getStatus(); s != OFFLINE; s = statusHandler->getStatus()) {
        if (tries > 5) {
            Serial.println(F("Too many attempts, stopped trying to initialize. Fail."));
            statusHandler->setStatus(FAILED);
            return;
        }
        Serial.println(F("Expected status to be OFFLINE but it was:"));
        Serial.println(s, HEX);
        Serial.println(F("Trying to re-initialize to fix this..."));
        delay(1000 * tries * 2); // Linear backoff
        initialize();
        tries++;
    }

    // Connect
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

    // Be quite blunt and re-connect if status isn't ONLINE by now
    if (statusHandler->getStatus() != ONLINE) {
        Serial.println(F("Status was not ONLINE, will attempt to connect..."));
        byte tries = 0;
        while (statusHandler->getStatus() != ONLINE) {
            if (tries > 5) {
                Serial.println(F("Too many attempts, stopped trying to connect."));
                return;
            }
            delay(1000 * tries * 2); // Linear backoff
            wpa2Connect();
            tries++;
        }
    }

    // If _still_ not ONLINE, there's no reason to even try to HTTP GET
    if (statusHandler->getStatus() != ONLINE) {
        Serial.println(F("Status needs to be ONLINE for HTTP GET to work. Fail."));
        statusHandler->setStatus(FAILED);
        return;
    }

    // Do HTTP GET
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
