#include "debug.h"

#include <AltSoftSerial.h>
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
#include "ESP8266.h"


/**
 * Global Shared Instances
 */

EventBus eventBus;
I2C i2c(MY_I2C_ADDRESS);

char responseBuffer[RESPONSE_BUFFER_SIZE] = {0};


/**
 * I2C Protocol Handlers
 */

PingHandler pingHandler(&eventBus);
StatusHandler statusHandler(&eventBus);

Storage ssidStorage(EEPROM_OFFSET_SSID, EEPROM_SIZE_SSID);
SSIDHandler ssidHandler(&eventBus, &ssidStorage);

Storage passphraseStorage(EEPROM_OFFSET_PASSPHRASE, EEPROM_SIZE_PASSPHRASE);
PassphraseHandler passphraseHandler(&eventBus, &passphraseStorage);

ConnectHandler connectHandler(&eventBus);

Storage hostStorage(EEPROM_OFFSET_HOST, EEPROM_SIZE_HOST);
HostHandler hostHandler(&eventBus, &hostStorage);

Storage pathStorage(EEPROM_OFFSET_PATH, EEPROM_SIZE_PATH);
PathHandler pathHandler(&eventBus, &pathStorage);

DoHandler doHandler(&eventBus);

ResponseHandler responseHandler(&eventBus, responseBuffer);


/**
 * Event Bus Handlers
 */

#if DEBUG_BLOCK
EEPROMDebugHandler eepromDebugHandler(&eventBus);
#endif

AltSoftSerial softser; // Hard-coded pins: TX: 9, RX: 8
ESP8266 wifiDevice(&softser, &ssidStorage, &passphraseStorage, &hostStorage, &pathStorage, responseBuffer);
WiFi wifi(&statusHandler, &wifiDevice);
WPA2ConnectHandler wpa2ConnectHandler(&eventBus, &wifi);

HTTPGetHandler httpGetHandler(&eventBus, &wifi);


/**
 * Register Event Handlers
 */

void registerEventBusHandlers() {
    #if DEBUG_BLOCK
    eventBus.registerHandler(&eepromDebugHandler);
    #endif
    eventBus.registerHandler(&wpa2ConnectHandler);
    eventBus.registerHandler(&httpGetHandler);
}

void registerProtocolHandlers() {
    i2c.registerRequestEventHandler(&pingHandler);
    i2c.registerRequestEventHandler(&statusHandler);
    i2c.registerReceiveEventHandler(&ssidHandler);
    i2c.registerReceiveEventHandler(&passphraseHandler);
    i2c.registerReceiveEventHandler(&connectHandler);
    i2c.registerReceiveEventHandler(&hostHandler);
    i2c.registerReceiveEventHandler(&pathHandler);
    i2c.registerReceiveEventHandler(&doHandler);
    i2c.registerRequestEventHandler(&responseHandler);
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
