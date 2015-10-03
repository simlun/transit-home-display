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

    bool resetSuccess = hardReset();
    if (!resetSuccess) {
        Serial.println(F("ERROR: Could not hard reset"));
        return false;
    }

    // TODO Continue proper initialization...
    softser->println("foo;");
    Serial.println(softser->readString());

    // TODO Method that sends a command and expects it to finish with some
    // response then the > within a timeout.

    Serial.println(F("AdafruitHuzzahESP8266 initialized"));
    return true;
}

bool AdafruitHuzzahESP8266::hardReset() {
    Serial.println(F("AdafruitHuzzahESP8266 hard reset"));

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

    // Discard first three lines
    Serial.println(softser->readStringUntil('\n'));
    Serial.println(softser->readStringUntil('\n'));
    Serial.println(softser->readStringUntil('\n'));

    // Then find prompt
    bool found;
    found = softser->find(">");
    if (!found) {
        Serial.println(F("ERROR: Could not find NodeMCU prompt"));
        return false;
    }

    // Try echoing a message to make sure it's responsive
    softser->println(F("print(\"hello\");"));
    found = softser->find("hello");
    if (!found) {
        Serial.println(F("ERROR: Did not get response from device"));
        return false;
    }

    found = softser->find(">");
    if (!found) {
        Serial.println(F("ERROR: Expected the prompt"));
        return false;
    }

    // If there's more data to read, do so and throw it away
    while (softser->available() > 0) {
        softser->read();
    }

    return true;
}
