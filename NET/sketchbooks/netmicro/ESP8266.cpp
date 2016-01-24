#include "ESP8266.h"

#include <EEPROM.h>

#include "constants.h"

#define ARD_RESET_PIN 4

#define ESP8266_DEFAULT_COMMAND_TIMEOUT 3000

ESP8266::ESP8266(SoftwareSerial * softser) : softser(softser) {}

bool ESP8266::initialize() {
    Serial.println(F("ESP8266 initializing..."));

    // High-impedance pin state
    pinMode(ARD_RESET_PIN, INPUT);

    // Soft serial connection to ESP8266
    softser->setTimeout(ESP8266_DEFAULT_COMMAND_TIMEOUT);
    softser->begin(9600);
    while(!softser);

    if (!hardReset()) {
        Serial.println(F("ERROR: Could not hard reset"));
        return false;
    }

    // Make sure it is disconnected
    if (!sendVoidCommand("AT+CWQAP")) {
        return false;
    }

    // Set wifi mode to STATION
    if (!setStationMode()) {
        return false;
    }

    Serial.println(F("ESP8266 initialized"));
    return true;
}

bool ESP8266::connect() {
    char ssid[32 + 1];
    memset(ssid, NULL, 32 + 1);
    for (byte addr = 0; addr < 32; addr++) {
        ssid[addr] = EEPROM.read(EEPROM_OFFSET_SSID + addr);
    }

    char pass[32 + 1];
    memset(pass, NULL, 32 + 1);
    for (byte addr = 0; addr < 32; addr++) {
        pass[addr] = EEPROM.read(EEPROM_OFFSET_PASSPHRASE + addr);
    }

    char * c1 = "AT+CWJAP=\"";
    char * c2 = "\",\"";
    char * c3 = "\"";
    byte total_length = strlen(c1) + strlen(ssid) + strlen(c2) + strlen(pass) + strlen(c3) + 1;

    char config[total_length];
    memset(config, NULL, sizeof(config));

    strcat(config, c1);
    strcat(config, ssid);
    strcat(config, c2);
    strcat(config, pass);
    strcat(config, c3);

    Serial.println(F("ESP8266 connecting to:"));
    Serial.println(config);

    if (!sendVoidCommand(config, 30000, 3)) {
        Serial.println(F("ERROR: Failed to join WiFi"));
        return false;
    }

    Serial.println(F("ESP8266 connected!"));
    return true;
}

bool ESP8266::httpGet() {

    // TODO

    return false;
}

bool ESP8266::hardReset() {
    // Toggle reset pin
    digitalWrite(ARD_RESET_PIN, LOW);
    pinMode(ARD_RESET_PIN, OUTPUT); // Open drain; reset -> GND
    delay(10); // Hold a moment
    pinMode(ARD_RESET_PIN, INPUT); // Back to high-impedance pin state

    // Expected data read after hard reset:
    // ---------------------------------------------------------------
    // ..jibberish..
    // [Vendor:www.ai-thinker.com Version:0.9.2.4]
    //
    // ready
    // ---------------------------------------------------------------

    // Discard first line of jibberish
    if (!softser->find("\n")) {
        Serial.println(F("ERROR: Could not discard initial jibberish!"));
        return false;
    }

    // Print welcome message
    Serial.println(softser->readStringUntil('\r'));

    // Then find `ready`
    if (!softser->find("ready\r\n")) {
        Serial.println(F("ERROR: Could not find `ready` message!"));
        return false;
    }
    if (softser->available() > 0) {
        Serial.println(F("ERROR: Did not expect any more data"));
        return false;
    }

    // Try sending a command
    if (!sendVoidCommand("AT")) {
        Serial.println(F("ERROR: Communication failure"));
        return false;
    }

    return true;
}

bool ESP8266::setStationMode() {
    if (!sendAndExpectResponseLine("AT+CWMODE?", "+CWMODE:1")) {
        Serial.println(F("Mode was not STATION"));
        if (!sendVoidCommand("AT+CWMODE=1")) {
            Serial.println(F("ERROR: Could not set mode to STATION"));
            return false;
        }
    }
    return true;
}

bool ESP8266::sendVoidCommand(char * command) {
    return sendVoidCommand(command, ESP8266_DEFAULT_COMMAND_TIMEOUT);
}

bool ESP8266::sendVoidCommand(char * command, unsigned long timeout) {
    softser->flush();
    softser->setTimeout(timeout);
    softser->print(command);
    softser->print("\r\n");

    String response = softser->readStringUntil('\n');
    response.trim();
    bool echo = response.equals(command);
    if (!echo) {
        Serial.println(F("ERROR: Expected the command:"));
        Serial.println(command);
        Serial.println(F("to be echoed but got:"));
        Serial.println(response);
        Serial.println(F("instead."));
        return false;
    }

    do {
        response = softser->readStringUntil('\n');
        response.trim();
        if (response.equals("FAIL") || response.equals("ERROR")) {
            Serial.println(F("Got FAIL/ERROR response instead of OK!"));
            return false;
        }
    } while (!response.equals("OK"));

    if (softser->available() > 0) {
        Serial.println(F("ERROR: Did not expect any more data"));
        while (softser->available() > 0) {
            Serial.print(softser->read());
        }
        Serial.println();
        return false;
    }
    return true;
}

bool ESP8266::sendVoidCommand(char * command, unsigned long timeout, unsigned int tries) {
    unsigned int retries = 0;
    while (!sendVoidCommand(command, timeout)) {
        if (retries == tries) {
            Serial.println(F("Stopped retrying."));
            return false;
        }
        Serial.println(F("Delaying..."));
        delay(1000 * (1 + retries)); // Linear backoff
        Serial.println(F("...retrying"));
        retries++;
    }
    return true;
}

bool ESP8266::sendAndExpectResponseLine(char * command, char * expectedResponse) {
    return sendAndExpectResponseLine(command, expectedResponse, true);
}

bool ESP8266::sendAndExpectResponseLine(char * command, char * expectedResponse, bool requireOk) {
    return sendAndExpectResponseLine(command, expectedResponse, requireOk, ESP8266_DEFAULT_COMMAND_TIMEOUT);
}

bool ESP8266::sendAndExpectResponseLine(char * command, char * expectedResponse, bool requireOk, unsigned long timeout) {
    softser->setTimeout(timeout);

    softser->flush();
    softser->print(command);
    softser->print("\r\n");

    String response = softser->readStringUntil('\n');
    response.trim();
    bool echo = response.equals(command);
    if (!echo) {
        Serial.println(F("ERROR: Expected the command:"));
        Serial.println(command);
        Serial.println(F("to be echoed but got:"));
        Serial.println(response);
        Serial.println(F("instead."));
        return false;
    }

    response = softser->readStringUntil('\n');
    response.trim();
    bool match = response.equals(expectedResponse);

    if (!match) {
        Serial.println(F("ERROR: Expected:"));
        Serial.println(expectedResponse);
        Serial.println(F("but got:"));
        Serial.println(response);
        Serial.println(F("instead."));
        return false;
    }

    if (requireOk && !softser->find("OK\r\n")) {
        Serial.print(F("Executed: "));
        Serial.println(command);
        Serial.println(F("ERROR: Expected response to end with OK"));
        return false;
    }

    // Discard any leftovers
    while (softser->available() > 0) {
        softser->read();
    }

    return true;
}
