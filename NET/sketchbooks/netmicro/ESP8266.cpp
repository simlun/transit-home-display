#include "ESP8266.h"

#include <EEPROM.h>
#include "MemoryFree.h"

#include "constants.h"

#define ESP8266_DEFAULT_COMMAND_TIMEOUT 10000

ESP8266::ESP8266(AltSoftSerial * softser) : softser(softser) {}

bool ESP8266::initialize() {
    Serial.println(F("ESP8266 initializing..."));

    // Soft serial connection to ESP8266
    // AT+UART_DEF=57600,8,1,0,0
    softser->begin(57600);
    while(!softser);
    softser->setTimeout(ESP8266_DEFAULT_COMMAND_TIMEOUT);

    if (!softReset()) {
        Serial.println(F("ERROR: Could not soft reset"));
        return false;
    }

    // Set wifi mode to STATION
    if (!sendValueUpdate("AT+CWMODE_CUR?", "+CWMODE_CUR:1", "AT+CWMODE_CUR=1")) {
        return false;
    }

    // Disable multiple connection mode
    if (!sendValueUpdate("AT+CIPMUX?", "+CIPMUX:0", "AT+CIPMUX=0")) {
        return false;
    }

    // Disable autoconnect
    if (!sendValueUpdate("AT+CWAUTOCONN?", "+CWAUTOCONN:0", "AT+CWAUTOCONN=0")) {
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

bool ESP8266::tcpConnect() {
    char host[64 + 1] = {0};
    for (byte addr = 0; addr < 64; addr++) {
        host[addr] = EEPROM.read(EEPROM_OFFSET_HOST + addr);
    }

    char * c1 = "AT+CIPSTART=\"TCP\",\"";
    char * c2 = "\",80";
    byte total_length = strlen(c1) + strlen(host) + strlen(c2) + 1;

    char cipstart[total_length];
    memset(cipstart, NULL, sizeof(cipstart));

    strcat(cipstart, c1);
    strcat(cipstart, host);
    strcat(cipstart, c2);

    printFreeMemory();
    Serial.println(F("ESP8266 Starting TCP connection:"));
    Serial.println(cipstart);

    if (!sendAndExpectResponseLine(cipstart, "CONNECT", true, 30000)) {
        Serial.println(F("ERROR: Failed to start TCP connection"));
        return false;
    }
    Serial.println(F("ESP8266: Connected to remote TCP server!"));
    return true;
}

bool ESP8266::httpGet() {
    printFreeMemory();
    if (!tcpConnect()) {
        return false;
    }
    printFreeMemory();

    // TODO Dynamic CIPSEND value
    char * cipsend = "AT+CIPSEND=37";
    softser->print(cipsend);
    softser->print("\r\n");

    // Discard part of echo
    for (byte i = 0; i < 11; i++) {
        while (!softser->available());
        softser->read();
    }
    /*String response = softser->readStringUntil('\n');
    response.trim();
    bool echo = response.equals(cipsend);
    if (!echo) {
        Serial.println(F("ERROR: Expected the command:"));
        Serial.println(cipsend);
        Serial.println(F("to be echoed but got:"));
        Serial.println(response);
        Serial.println(F("instead."));
        return false;
    }*/

    // Expect the CIPSEND prompt
    while (!softser->available());
    if (!softser->find("> ")) {
        Serial.println(F("ERROR: Could not find CIPSEND prompt"));
        return false;
    }

    // Prepare to receive data
    char c;
    char statusCodeBuff[3 + 1] = {0};
    bool badHttpStatusCode = true;
    char headerBuff[32 + 1] = {0};
    char contentLength[32 + 1] = {0};
    char cBuff[64 + 1] = {0};
    int bytesRead = 0;
    String incomingDataLengthString;
    long bytesLeft;

    if (softser->overflow()) {
        Serial.println(F("Buffer overflow!"));
    }
    softser->flush();

    // Send request
    softser->setTimeout(30000);
    // TODO Get real path from EEPROM
    //softser->print("GET / HTTP/1.0\r\n\r\n");
    softser->print("GET / HTTP/1.0\r\nHost: example.com\r\n\r\n");

    // Discard echo
    /*
    bytesLeft = 37; // TODO: Use correct number
    while (bytesLeft > 0) {
        while (softser->peek() == -1);
        softser->read();
        bytesLeft--;
    }
    */

    // Wait for AT data header
    if (!softser->find("+IPD,")) {
        Serial.println(F("ERROR: Could not find +IPD response"));
        return false;
    }
    incomingDataLengthString = softser->readStringUntil(':');
    bytesLeft = incomingDataLengthString.toInt();
    //Serial.println(F("Receiving data of length:"));
    //Serial.println(bytesLeft);

    // Read response
    byte step = 0;
    while (bytesLeft > 0) {
        while (softser->peek() == -1);

        c = softser->read();
        bytesRead++;
        bytesLeft--;

        if (step == 0 && bytesRead <= 4) {
            // First line, only continue if it begins with "HTTP"
            headerBuff[bytesRead - 1] = c;
            if (strcmp(headerBuff, "HTTP") == 0) {
                step++;
                bytesRead = 0;
            }
        } else if (step == 1) {
            // Find beginning of status code
            if (c == ' ') {
                step++;
                bytesRead = 0;
            }
        } else if (step == 2) {
            // Parse status code
            if (bytesRead <= 3) {
                statusCodeBuff[bytesRead - 1] = c;
            }
            if (bytesRead == 3) {
                if (statusCodeBuff[0] == '1' || statusCodeBuff[0] == '2' || statusCodeBuff[0] == '3') {
                    badHttpStatusCode = false;
                }
                step++;
                bytesRead = 0;
            }
        } else if (step == 3) {
            // Skip rest of first HTTP line
            if (c == '\n') {
                step++;
                bytesRead = 0;
            }
        } else if (step == 4) {
            // Find Content-Length header key
            if (bytesRead > 32) {
                // Skip rest of lines longer than 32 chars
                if (c == '\n') {
                    bytesRead = 0;
                }
                continue;
            }
            if (c == '\n') {
                bytesRead = 0;
                continue;
            }
            headerBuff[bytesRead - 1] = c;
            headerBuff[bytesRead] = '\0';
            if (strcmp(headerBuff, "Content-Length: ") == 0) {
                step++;
                bytesRead = 0;
            }
        } else if (step == 5 && bytesRead <= 32) {
            // Read Content-Length header value
            if (c == '\n') {
                step++;
                bytesRead = 0;
                continue;
            }
            contentLength[bytesRead - 1] = c;
            contentLength[bytesRead] = '\0';
        } else if (step == 6) {
            // Discard rest of headers
            if (bytesRead > 32) {
                // Skip rest of lines longer than 32 chars
                if (c == '\n') {
                    bytesRead = 0;
                }
                continue;
            }
            if (c == '\n') {
                if (headerBuff[0] == '\r') {
                    step++;
                    bytesRead = 0;
                    continue;
                } else {
                    bytesRead = 0;
                    continue;
                }
            }
            headerBuff[bytesRead - 1] = c;
        } else {
            // TODO This is the error step

            // Store the rest 64 bytes in the cBuff
            if (bytesRead < 64) {
                cBuff[bytesRead - 1] = c;
            }
        }
    }

    Serial.print(F("Step: "));
    Serial.println(step, DEC);
    Serial.print(F("HTTP Status Code: "));
    Serial.println(statusCodeBuff);
    Serial.print(F("Content-Length: "));
    Serial.println(contentLength);
    Serial.println(F("cBuff:"));
    Serial.println(cBuff);
    Serial.println(F("=============="));
    printFreeMemory();

    if (!softser->find("CLOSED\r\n")) {
        Serial.println(F("ERROR: expected the TCP connection to be CLOSED after a successful transmission"));
        return false;
    }

    if (softser->available() > 0) {
        Serial.println(F("ERROR: Did not expect any more data"));
        for (char c; softser->available() > 0; c = softser->read()) {
            Serial.print(c);
        }
        Serial.println();
        return false;
    }

    if (softser->overflow()) {
        Serial.println(F("Buffer overflow!"));
        return false;
    }

    if (badHttpStatusCode) {
        Serial.println(F("ESP8266 HTTP GET failed with bad HTTP status code!"));
        return false;
    }

    Serial.println(F("ESP8266 HTTP GET successful!"));
    return true;
}

bool ESP8266::softReset() {
    // Send test command
    if (!sendVoidCommand("AT")) {
        Serial.println(F("ERROR: Communication failure during initial test command"));
        return false;
    }

    sendAndExpectResponseLine("AT+RST", "ready", false);

    // Send test command
    if (!sendVoidCommand("AT")) {
        Serial.println(F("ERROR: Communication failure during test command"));
        return false;
    }

    return true;
}

bool ESP8266::sendValueUpdate(char * query, char * expectedValue, char * setCommand) {
    if (!sendValueQuery(query, expectedValue)) {
        if (!sendVoidCommand(setCommand)) {
            Serial.println(F("ERROR: Could not send value update"));
            return false;
        } else if (!sendValueQuery(query, expectedValue)) {
            Serial.println(F("ERROR: After successfully updating the value it didn't stick"));
            return false;
        }
    }
    return true;
}

bool ESP8266::sendValueQuery(char * query, char * expectedValue) {
    softser->flush();
    softser->setTimeout(ESP8266_DEFAULT_COMMAND_TIMEOUT);
    softser->print(query);
    softser->print("\r\n");

    String response = softser->readStringUntil('\n');
    response.trim();
    bool echo = response.equals(query);
    if (!echo) {
        Serial.println(F("ERROR: Expected the command:"));
        Serial.println(query);
        Serial.println(F("to be echoed but got:"));
        Serial.println(response);
        Serial.println(F("instead."));
        return false;
    }

    response = softser->readStringUntil('\n');
    response.trim();
    bool match = true;
    if (!response.equals(expectedValue)) {
        Serial.println(F("Got unexpected value:"));
        Serial.println(response);
        Serial.println(F("Instead of:"));
        Serial.println(expectedValue);
        match = false;
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
        for (char c; softser->available() > 0; c = softser->read()) {
            Serial.print(c);
        }
        Serial.println();
        return false;
    }
    return match;
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
        for (char c; softser->available() > 0; c = softser->read()) {
            Serial.print(c);
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
    String response;
    byte searchedLines = 0;
    bool match;

    softser->setTimeout(timeout);

    Serial.println(F("Sending command:"));
    Serial.println(command);

    softser->flush();
    softser->print(command);
    softser->print("\r\n");

    response = softser->readStringUntil('\n');
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

    while (true) {
        if (searchedLines > 100) {
            Serial.println(F("ERROR Could not find expected line in 100 response lines. Giving up."));
            softser->flush();
            return false;
        }
        response = softser->readStringUntil('\n');
        response.trim();
        match = response.equals(expectedResponse);

        if (match) {
            Serial.println(F("Got expected response:"));
            Serial.println(response);
            break;
        } else {
            Serial.println(F("Expected:"));
            Serial.println(expectedResponse);
            Serial.println(F("but got:"));
            Serial.println(response);
            Serial.println(F("instead."));

            if (softser->overflow()) {
                Serial.println(F("Buffer overflow!"));
            }

            searchedLines++;
        }
    }
    printFreeMemory();

    if (requireOk) {
        response = softser->readStringUntil('\n');
        response.trim();
        if (response.length() == 0) {
            // The ESP8266 throws out blank lines now and then, discard them.
            response = softser->readStringUntil('\n');
            response.trim();
        }
        bool ok = response.equals("OK");
        if (!ok) {
            Serial.print(F("Executed: "));
            Serial.println(command);
            Serial.println(F("Expected OK but got:"));
            Serial.println(response);
            Serial.println(F("ERROR: Expected response to end with OK"));
            return false;
        }
    }

    // Discard any leftovers
    while (softser->available() > 0) {
        softser->read();
    }
    softser->flush();

    return true;
}

bool ESP8266::searchForResponse(char * expectedResponse) {
    return false; // TODO REMOVE THIS METHOD
}
