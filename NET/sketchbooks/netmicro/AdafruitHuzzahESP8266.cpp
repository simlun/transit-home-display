#include "AdafruitHuzzahESP8266.h"

#include <EEPROM.h>

#define ARD_RESET_PIN 4

AdafruitHuzzahESP8266::AdafruitHuzzahESP8266(SoftwareSerial * softser) : softser(softser) {}

bool AdafruitHuzzahESP8266::initialize() {
    Serial.println(F("AdafruitHuzzahESP8266 initializing..."));

    // Soft serial connection to ESP8266
    softser->begin(9600);
    while(!softser);

    if (!hardReset()) {
        Serial.println(F("ERROR: Could not hard reset"));
        return false;
    }
    Serial.print('.');

    if (!sendVoidCommand("wifi.setmode(wifi.STATION);")) {
        return false;
    }
    Serial.print('.');

    if (!sendVoidCommand("wifi.sta.disconnect();")) {
        return false;
    }
    Serial.print('.');

    if (!sendVoidCommand("wifi.sta.autoconnect(0);")) {
        return false;
    }
    Serial.print('.');

    if (!sendCommandWithExpectedResponse("print(wifi.sta.status());", "0")) {
        return false;
    }
    Serial.print('.');

    Serial.println();

    Serial.println(F("AdafruitHuzzahESP8266 initialized"));
    return true;
}

bool AdafruitHuzzahESP8266::wpa2Connect() {
    // TODO Use ssid from EEPROM
    char * ssid = "Rector37C";

    char pass[32 + 1];
    memset(pass, NULL, 32 + 1);

    for (byte addr = 0; addr < 32; addr++) {
        pass[addr] = EEPROM.read(addr);
    }

    char * c1 = "wifi.sta.config(\"";
    char * c2 = "\", \"";
    char * c3 = "\")";
    byte total_length = strlen(c1) + strlen(ssid) + strlen(c2) + strlen(pass) + strlen(c3) + 1;

    char config[total_length];
    memset(config, NULL, sizeof(config));

    strcat(config, c1);
    strcat(config, ssid);
    strcat(config, c2);
    strcat(config, pass);
    strcat(config, c3);

    if (!sendVoidCommand(config)) {
        Serial.println(F("ERROR: Failed to configure WiFi credentials"));
        return false;
    }

    if (!sendVoidCommand("wifi.sta.connect()")) {
        Serial.println(F("ERROR: Failed to connect"));
        return false;
    }

    delay(1000);

    while (!sendCommandWithExpectedResponse("print(wifi.sta.status())", "5")) {
        // TODO Timeout
        delay(1000);
    }

    Serial.println(F("AdafruitHuzzahESP8266 connected!"));

    return true;
}

bool AdafruitHuzzahESP8266::sendVoidCommand(char * command) {
    softser->println(command);

    // Discard echo
    softser->readStringUntil('\n');

    // TODO Error handling here

    if (!softser->find("> ")) {
        Serial.print(F("Executed: "));
        Serial.println(command);
        Serial.println(F("ERROR: Expected the prompt"));
        return false;
    }

    if (softser->available() > 0) {
        Serial.println(F("ERROR: Did not expect any more data"));
        return false;
    }
    return true;
}

bool AdafruitHuzzahESP8266::sendCommandWithExpectedResponse(char * command, char * expectedResponse) {
    softser->println(command);
    softser->readStringUntil('\n'); // Discard echo

    String response = softser->readStringUntil('\r');
    bool match = response.equals(expectedResponse);

    if (!match) {
        Serial.println(F("ERROR: Expected:"));
        Serial.println(expectedResponse);
        Serial.println(F("but got:"));
        Serial.println(response);
        Serial.println(F("instead."));
    }

    if (!softser->find("> ")) {
        Serial.print(F("Executed: "));
        Serial.println(command);
        Serial.println(F("ERROR: Expected the prompt"));
        return false;
    }

    if (softser->available() > 0) {
        Serial.println(F("ERROR: Did not expect any more data"));
        return false;
    }

    return match;
}

bool AdafruitHuzzahESP8266::hardReset() {
    // Toggle reset pin
    digitalWrite(ARD_RESET_PIN, LOW);
    pinMode(ARD_RESET_PIN, OUTPUT); // Open drain; reset -> GND
    delay(10); // Hold a moment
    pinMode(ARD_RESET_PIN, INPUT); // Back to high-impedance pin state

    // Expected data read after hard reset:
    // ---------------------------------------------------------------
    // ..jibberish..
    // NodeMCU 0.9.5 build 20150318  powered by Lua 5.1.4
    // lua: cannot open init.lua
    // > 
    // ---------------------------------------------------------------

    // Discard first line of jibberish
    softser->readStringUntil('\n');

    // Print: "NodeMCU 0.9.5 build 20150318  powered by Lua 5.1.4"
    Serial.println(softser->readStringUntil('\n'));

    // Discard: "lua: cannot open init.lua"
    softser->readStringUntil('\n');

    // Then find prompt
    bool found;
    found = softser->find("> ");
    if (!found) {
        Serial.println(F("ERROR: Could not find prompt"));
        return false;
    }

    // Try echoing a message to make sure it's responsive
    if (!sendCommandWithExpectedResponse("print(\"hello\");", "hello")) {
        Serial.println(F("ERROR: Could not echo"));
        return false;
    }

    if (softser->available() > 0) {
        Serial.println(F("ERROR: Did not expect any more data"));
        return false;
    }

    return true;
}
