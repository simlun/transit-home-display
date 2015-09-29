#include "WiFi.h"
#include "constants.h"

WiFi::WiFi(StatusHandler * statusHandler, WiFiDevice * wifiDevice)
    : statusHandler(statusHandler),
      wifiDevice(wifiDevice) {}

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
