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
//    sk=net.createConnection(net.TCP, 0)
//    sk:on("receive", function(sck, c) print(c) end )
//    sk:connect(80,"192.168.0.66")
//    sk:on("connection", function(sck,c)
//        -- Wait for connection before sending.
//        sk:send("GET / HTTP/1.1\r\nHost: 192.168.0.66\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n")
//        end)
    Serial.println(F("HTTP Get"));
}
