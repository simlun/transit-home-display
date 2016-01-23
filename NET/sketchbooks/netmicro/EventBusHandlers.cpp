#include "EventBusHandlers.h"
#include <EEPROM.h>
#include "constants.h"

/**
 * EEPROMDebugHandler
 */

EEPROMDebugHandler::EEPROMDebugHandler(EventBus * eventBus)
    : EventHandler(eventBus) {}

Event EEPROMDebugHandler::event() {
    return EEPROM_DEBUG;
}

void EEPROMDebugHandler::handle() {
    Serial.println(F("Dumping EEPROM contents:"));
    byte b;
    Serial.print("0x000: ");
    for (int i = 0; i < 1024; i++) {
        if (i != 0 && i % 8 == 0) {
            Serial.println();
            Serial.print("0x");
            if (i <= 0xF) {
                Serial.print("00");
            } else if (i <= 0xFF) {
                Serial.print("0");
            }
            Serial.print(i, HEX);
            Serial.print(": ");
        }
        b = EEPROM.read(i);
        if (b <= 0xF) {
            Serial.print("0");
        }
        Serial.print(b, HEX);
        Serial.print(" ");
    }
    Serial.println();
}


/**
 * WPA2ConnectHandler
 */

// TODO: Make this a ConnectHandler instead of a WPA2ConnectHandler. Mode will
// be decided in some other way.

WPA2ConnectHandler::WPA2ConnectHandler(EventBus * eventBus,
                                       WiFi * wifi)
    : EventHandler(eventBus),
      wifi(wifi) {}

Event WPA2ConnectHandler::event() {
    return WPA2;
}

void WPA2ConnectHandler::handle() {
    wifi->connect();
}


/**
 * HTTPGetHandler
 */

HTTPGetHandler::HTTPGetHandler(EventBus * eventBus,
                               WiFi * wifi)
    : EventHandler(eventBus),
      wifi(wifi) {}

Event HTTPGetHandler::event() {
    return GET;
}

void HTTPGetHandler::handle() {
    wifi->httpGet();
}
