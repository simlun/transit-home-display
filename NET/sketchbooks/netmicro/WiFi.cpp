#include "debug.h"
#include "WiFi.h"
#include "constants.h"

WiFi::WiFi(StatusHandler * statusHandler, WiFiDevice * wifiDevice)
    : statusHandler(statusHandler),
      wifiDevice(wifiDevice) {}

void WiFi::initialize() {
    DEBUG Serial.println(F("Initializing WiFi"));
    bool initializationSucceeded = wifiDevice->initialize();
    if (initializationSucceeded) {
        DEBUG Serial.println(F("WiFi initialization succeeded"));
        statusHandler->setStatus(OFFLINE);
    } else {
        DEBUG Serial.println(F("WiFi initialization failed"));
        statusHandler->setStatus(FAILED);
    }
}

void WiFi::connect() {
    DEBUG Serial.println(F("Connecting to WiFi"));

    // Re-initialize if status isn't OFFLINE
    byte tries = 0;
    for (byte s = statusHandler->getStatus(); s != OFFLINE; s = statusHandler->getStatus()) {
        if (tries > 5) {
            DEBUG Serial.println(F("Too many attempts, stopped trying to initialize. Fail."));
            statusHandler->setStatus(FAILED);
            return;
        }
        DEBUG Serial.println(F("Expected status to be OFFLINE but it was:"));
        DEBUG Serial.println(s, HEX);
        DEBUG Serial.println(F("Trying to re-initialize to fix this..."));
        delay(1000 * tries * 2); // Linear backoff
        initialize();
        tries++;
    }

    // Connect
    statusHandler->setStatus(CONNECTING);
    bool connectionSucceeded = wifiDevice->connect();
    if (connectionSucceeded) {
        DEBUG Serial.println(F("WiFi connection succeeded"));
        statusHandler->setStatus(ONLINE);
    } else {
        DEBUG Serial.println(F("WiFi connection failed"));
        statusHandler->setStatus(FAILED);
    }
}

void WiFi::httpGet() {
    DEBUG Serial.println(F("HTTP GET"));

    // Be quite blunt and re-connect if status isn't ONLINE by now
    if (statusHandler->getStatus() != ONLINE) {
        DEBUG Serial.println(F("Status was not ONLINE, will attempt to connect..."));
        byte tries = 0;
        while (statusHandler->getStatus() != ONLINE) {
            if (tries > 5) {
                DEBUG Serial.println(F("Too many attempts, stopped trying to connect."));
                return;
            }
            delay(1000 * tries * 2); // Linear backoff
            connect();
            tries++;
        }
    }

    // If _still_ not ONLINE, there's no reason to even try to HTTP GET
    if (statusHandler->getStatus() != ONLINE) {
        DEBUG Serial.println(F("Status needs to be ONLINE for HTTP GET to work. Fail."));
        statusHandler->setStatus(FAILED);
        return;
    }

    // Do HTTP GET
    statusHandler->setStatus(BUSY);
    bool getSucceeded = wifiDevice->httpGet();
    if (getSucceeded) {
        DEBUG Serial.println(F("HTTP GET succeeded"));
        statusHandler->setStatus(ONLINE);
    } else {
        DEBUG Serial.println(F("HTTP GET failed"));
        statusHandler->setStatus(FAILED);
    }
}
