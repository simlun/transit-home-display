#include "AdafruitHuzzahESP8266.h"

#define ARD_RESET_PIN 4

AdafruitHuzzahESP8266::AdafruitHuzzahESP8266(SoftwareSerial * softser) : softser(softser) {}

bool AdafruitHuzzahESP8266::wpa2Connect() {
    initialize();
    return false;
}

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

    if (!sendVoidCommand(F("wifi.setmode(wifi.STATION);"))) {
        return false;
    }
    Serial.print('.');

    if (!sendVoidCommand(F("wifi.sta.disconnect();"))) {
        return false;
    }
    Serial.print('.');

    if (!sendVoidCommand(F("wifi.sta.autoconnect(0);"))) {
        return false;
    }
    Serial.print('.');

    if (!sendCommandWithExpectedResponse(F("print(wifi.sta.status());"), "0")) {
        return false;
    }
    Serial.print('.');

    Serial.println();

    Serial.println(F("AdafruitHuzzahESP8266 initialized"));
    return true;
}

bool AdafruitHuzzahESP8266::sendVoidCommand(Fstr * command) {
    softser->println(command);
    if (!softser->find(">")) {
        Serial.print(F("Executed: "));
        Serial.println(command);
        Serial.println(F("ERROR: Expected the prompt"));
        return false;
    }
    softser->readString(); // Discard prompt whitespace
    return true;
}

bool AdafruitHuzzahESP8266::sendCommandWithExpectedResponse(Fstr * command, String expectedResponse) {
    softser->println(command);
    softser->readStringUntil('\n'); // Discard echo
    String response = softser->readStringUntil('\r');
    if (!response.equals(expectedResponse)) {
        Serial.println(F("ERROR: Expected:"));
        Serial.println(expectedResponse);
        Serial.println(F("but got:"));
        Serial.println(response);
        Serial.println(F("instead."));
        return false;
    }
    if (!softser->find(">")) {
        Serial.print(F("Executed: "));
        Serial.println(command);
        Serial.println(F("ERROR: Expected the prompt"));
        return false;
    }
    softser->readString(); // Discard prompt whitespace
    return true;
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
    found = softser->find(">");
    if (!found) {
        Serial.println(F("ERROR: Could not find NodeMCU prompt"));
        return false;
    }

    // Try echoing a message to make sure it's responsive
    if (!sendCommandWithExpectedResponse(F("print(\"hello\");"), "hello")) {
        return false;
    }

    // If there's more data to read, do so and throw it away
    while (softser->available() > 0) {
        softser->read();
    }

    return true;
}
