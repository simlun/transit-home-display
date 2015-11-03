#include <SoftwareSerial.h>
#include <Wire.h>
#include <EEPROM.h>

#include <StandardCplusplus.h>

#include "MemoryFree.h"

#include "configuration.h"
#include "constants.h"

#include "I2C.h"
#include "Protocol.h"

#include "EventBus.h"
#include "EventBusHandlers.h"

#include "WiFi.h"
#include "AdafruitHuzzahESP8266.h"


/**
 * Global Shared Instances
 */

EventBus eventBus;
I2C i2c(MY_I2C_ADDRESS);

PingHandler pingHandler(&eventBus);
StatusHandler statusHandler(&eventBus);

Storage ssidStorage(EEPROM_OFFSET_SSID, 32);
SSIDHandler ssidHandler(&eventBus, &ssidStorage);

Storage passphraseStorage(EEPROM_OFFSET_PASSPHRASE, 32);
PassphraseHandler passphraseHandler(&eventBus, &passphraseStorage);

ConnectHandler connectHandler(&eventBus);
//HostHandler hostHandler(&eventBus);
//PathHandler pathHandler(&eventBus);

#define ARD_RX_ESP_TX 2
#define ARD_TX_ESP_RX 3
SoftwareSerial softser(ARD_RX_ESP_TX, ARD_TX_ESP_RX);

AdafruitHuzzahESP8266 wifiDevice(&softser);
WiFi wifi(&statusHandler, &wifiDevice);

EEPROMDebugHandler eepromDebugHandler(&eventBus);
WPA2ConnectHandler wpa2ConnectHandler(&eventBus, &wifi);


/**
 * Register Event Handlers
 */

void registerEventBusHandlers() {
    eventBus.registerHandler(&eepromDebugHandler);
    eventBus.registerHandler(&wpa2ConnectHandler);
}

void registerProtocolHandlers() {
    i2c.registerRequestEventHandler(&pingHandler);
    i2c.registerRequestEventHandler(&statusHandler);
    i2c.registerReceiveEventHandler(&ssidHandler);
    i2c.registerReceiveEventHandler(&passphraseHandler);
    i2c.registerReceiveEventHandler(&connectHandler);
    //i2c.registerReceiveEventHandler(&hostHandler);
    //i2c.registerReceiveEventHandler(&pathHandler);
}


/**
 * Arduino I2C
 */

void _onReceiveEvent(int numBytesReadFromMaster) {
    i2c.onReceiveEvent(numBytesReadFromMaster);
}

void _onRequestEvent() {
    i2c.onRequestEvent();
}

void registerI2CEventHandlers() {
    // TODO Move this to I2C class somehow?
    Wire.onReceive(_onReceiveEvent);
    Wire.onRequest(_onRequestEvent);
}


/**
 * Arduino
 */

void initializeSerial(void) {
    Serial.begin(115200);
    while(!Serial);
    Serial.println(F("\nInitialized serial communication"));
}

void setup(void) {
    initializeSerial();

    registerEventBusHandlers();
    registerProtocolHandlers();
    registerI2CEventHandlers();

    i2c.initialize();
    wifi.initialize();

    printFreeMemory();
}

void loop(void) {
    eventBus.process();
    delay(1);
}
