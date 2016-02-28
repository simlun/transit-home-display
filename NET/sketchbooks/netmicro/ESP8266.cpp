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

    // Disable echoes
    if (!sendVoidCommand("ATE0")) {
        Serial.println(F("ERROR: Failed to disable echoes"));
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

    Serial.print(F("ESP8266 connecting to: "));
    Serial.println(config);

    if (!sendVoidCommand(config, 30000, 3)) {
        Serial.println(F("ERROR: Failed to join WiFi"));
        return false;
    }

    Serial.println(F("ESP8266 connected!"));
    return true;
}

bool ESP8266::tcpConnect() {
    Serial.println(F("[tcpConnect()"));
    printFreeMemory();
    bool result = true;

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

    if (!sendAndExpectResponseLine(cipstart, "CONNECT", true, false, 30000)) {
        Serial.println(F("ERROR: Failed to start TCP connection"));
        result = false;
    } else {
        Serial.println(F("ESP8266: Connected to remote TCP server!"));
    }

    printFreeMemory();
    Serial.println(F("tcpConnect]"));
    Serial.print(F("return "));
    Serial.println(result ? F("true") : F("false"));
    Serial.println();
    return result;
}

bool ESP8266::sendHttpGetRequest() {
    Serial.println(F("[sendHttpGetRequest()"));
    printFreeMemory();

    // TODO Dynamic CIPSEND value
    char * cipsend = "AT+CIPSEND=37";
    softser->print(cipsend);
    softser->print("\r\n");

    // Expect the CIPSEND prompt
    while (softser->available() == 0);
    if (!softser->find("> ")) {
        Serial.println(F("ERROR: Could not find CIPSEND prompt"));
        Serial.println(F("sendHttpGetRequest]"));
        Serial.println();
        return false;
    }

    // Send request
    if (softser->overflow()) {
        Serial.println(F("Buffer overflow!"));
    }
    softser->flush();

    softser->setTimeout(30000);
    // TODO Get real path from EEPROM
    softser->print("GET / HTTP/1.0\r\nHost: example.com\r\n\r\n");

    // One second delay after printing the request was recommended by the ESP8266 docs:
    //delay(1000);

    // Recv 37 bytes
    //
    // SEND OK
    //

    while (softser->available() == 0);
    if (!softser->find("Recv ")) {
        Serial.println(F("ERROR: Could not find beginning of CIPSEND response"));
        Serial.println(F("sendHttpGetRequest]"));
        Serial.println();
        return false;
    }
    //String recvBytes = softser->readStringUntil(' ');
    //long recvBytes_long = recvBytes.toInt();
    // TODO: Verify recvBytes_long
    while (softser->available() == 0);
    if (!softser->find("37")) {
        Serial.println(F("ERROR: Could not find length of CIPSEND response"));
        Serial.println(F("sendHttpGetRequest]"));
        Serial.println();
        return false;
    }

    while (softser->available() == 0);
    if (!softser->find(" bytes\r\n\r\n")) {
        Serial.println(F("ERROR: Could not find end of CIPSEND response"));
        Serial.println(F("sendHttpGetRequest]"));
        Serial.println();
        return false;
    }
    while (softser->available() == 0);
    if (!softser->find("SEND OK\r\n\r\n")) {
        Serial.println(F("ERROR: Could not find SEND OK"));
        Serial.println(F("sendHttpGetRequest]"));
        Serial.println();
        return false;
    }

    printFreeMemory();
    Serial.println(F("sendHttpGetRequest]"));
    Serial.println();
    return true;
}

bool ESP8266::httpGet() {
    Serial.println(F("[httpGet()"));
    printFreeMemory();
    char c;
    char statusCodeBuff[3 + 1] = {0};
    bool badHttpStatusCode = true;
    char headerBuff[32 + 1] = {0};
    char contentLength[32 + 1] = {0};
    char cBuff[64 + 1] = {0};
    int bytesRead = 0;
    String incomingDataLengthString;
    long bytesLeft;
    char ipdOrClose[6] = {0};
    printFreeMemory();

    if (!tcpConnect()) {
        return false;
    }
    printFreeMemory();

    if (!sendHttpGetRequest()) {
        return false;
    }
    printFreeMemory();

    byte step = 0;
    while (true) {

        while (softser->available() < 5);
        // Strip leading line breaks (will be a line break here for every +IPD after the first one)
        while (softser->peek() == '\r' || softser->peek() == '\n') {
            softser->read();
        }

        // Expect either "+IPD," or "CLOSE"(D\r\n) as the next read 5 characters
        while (softser->available() < 5);
        for (byte i = 0; i < 5; i++) {
            ipdOrClose[i] = softser->read();
        }

        if (strcmp(ipdOrClose, "CLOSE") == 0) {
            // Discard the rest of the "CLOSED\r\n" message:
            for (byte i = 0; i < 3; i++) {
                while (softser->available() == 0);
                softser->read();
            }
            break;
        } else if (strcmp(ipdOrClose, "+IPD,") == 0) {
            // Simply continue to parsing the rest of the +IPD message.
        } else {
            Serial.println(F("ERROR: expected either a +IPD, or a CLOSE. Got this instead:"));
            Serial.println(ipdOrClose);
            Serial.println(F("httpGet]"));
            Serial.println(F("return false"));
            Serial.println();
            return false;
        }

        incomingDataLengthString = softser->readStringUntil(':');
        bytesRead = 0;
        bytesLeft = incomingDataLengthString.toInt();
        //Serial.println(F("Receiving data of length:"));
        //Serial.println(bytesLeft);

        // Read response
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

                // Store what fits in the cBuff
                //if (bytesRead < 64) {
                //    cBuff[bytesRead - 1] = c;
                //} else {
                    // Discard what doesn't fit in the cBuff
                    Serial.print(c);
                //}
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

    if (softser->available() > 0) {
        Serial.println(F("ERROR: Did not expect any more data"));
        for (char c = softser->read(); softser->available() > 0; c = softser->read()) {
            Serial.print("0x");
            Serial.print(c, HEX);
            Serial.print(", ");
        }
        Serial.println();
        Serial.println(F("httpGet]"));
        Serial.println(F("return false"));
        Serial.println();
        return false;
    }

    if (softser->overflow()) {
        Serial.println(F("Buffer overflow!"));
        Serial.println(F("httpGet]"));
        Serial.println(F("return false"));
        Serial.println();
        return false;
    }

    if (badHttpStatusCode) {
        Serial.println(F("ESP8266 HTTP GET failed with bad HTTP status code!"));
        Serial.println(F("httpGet]"));
        Serial.println(F("return false"));
        Serial.println();
        return false;
    }

    Serial.println(F("ESP8266 HTTP GET successful!"));
    printFreeMemory();
    Serial.println(F("httpGet]"));
    Serial.println(F("return true"));
    Serial.println();
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
    if (!sendAndExpectResponseLine(query, expectedValue, true, true)) {
        if (!sendVoidCommand(setCommand)) {
            Serial.println(F("ERROR: Could not send value update"));
            return false;
        } else if (!sendAndExpectResponseLine(query, expectedValue, true, true)) {
            Serial.println(F("ERROR: After successfully updating the value it didn't stick"));
            return false;
        }
    }
    return true;
}

bool ESP8266::sendVoidCommand(char * command) {
    return sendVoidCommand(command, ESP8266_DEFAULT_COMMAND_TIMEOUT);
}

bool ESP8266::sendVoidCommand(char * command, unsigned long timeout) {
    Serial.print(F("[sendVoidCommand("));
    Serial.print(command);
    Serial.print(F(", "));
    Serial.print(timeout, DEC);
    Serial.println(F(")"));
    printFreeMemory();

    softser->flush();
    softser->setTimeout(timeout);
    softser->print(command);
    softser->print("\r\n");

    bool result = false;
    String response;
    byte searchedLines = 0;
    while (true) {
        if (searchedLines > 100) {
            Serial.println(F("ERROR Could not find expected line in 100 response lines. Giving up."));
            softser->flush();
            result = false;
            break;
        }

        response = softser->readStringUntil('\n');
        response.trim();

        if (response.equals("OK")) {
            Serial.println(F("OK"));
            result = true;
            break;
        } else if (response.equals(command)) {
            Serial.print(F("Echo: "));
            Serial.println(command);
        } else if (response.equals("FAIL") || response.equals("ERROR")) {
            Serial.println(F("Got FAIL/ERROR response instead of OK!"));
            result = false;
            break;
        } else if (response.length() == 0) {
            Serial.println(F("Discarding blank line"));
        } else {
            Serial.print(F("Discarding: "));
            Serial.println(response);
        }
        if (softser->overflow()) {
            Serial.println(F("Buffer overflow!"));
        }
        searchedLines++;
    }

    if (softser->available() > 0) {
        result = false;
        Serial.println(F("ERROR: Did not expect any more data"));
        for (char c = softser->read(); softser->available() > 0; c = softser->read()) {
            Serial.print(c);
        }
        Serial.println();
    }
    printFreeMemory();
    Serial.println(F("sendVoidCommand]"));
    Serial.print(F("return "));
    Serial.println(result ? F("true") : F("false"));
    Serial.println();
    return result;
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
    return sendAndExpectResponseLine(command, expectedResponse, requireOk, false, ESP8266_DEFAULT_COMMAND_TIMEOUT);
}

bool ESP8266::sendAndExpectResponseLine(char * command, char * expectedResponse, bool requireOk, bool strict) {
    return sendAndExpectResponseLine(command, expectedResponse, requireOk, strict, ESP8266_DEFAULT_COMMAND_TIMEOUT);
}

bool ESP8266::sendAndExpectResponseLine(char * command, char * expectedResponse, bool requireOk, bool strict, unsigned long timeout) {
    Serial.print(F("[sendAndExpectResponseLine("));
    Serial.print(command);
    Serial.print(F(", "));
    Serial.print(expectedResponse);
    Serial.print(F(", "));
    Serial.print(requireOk ? F("true") : F("false"));
    Serial.print(F(", "));
    Serial.print(strict ? F("true") : F("false"));
    Serial.print(F(", "));
    Serial.print(timeout, DEC);
    Serial.println(F(")"));
    printFreeMemory();

    softser->setTimeout(timeout);

    softser->flush();
    softser->print(command);
    softser->print("\r\n");

    bool result = false;
    String response;
    byte searchedLines = 0;
    while (true) {
        if (searchedLines > 100) {
            Serial.println(F("ERROR Could not find expected line in 100 response lines. Giving up."));
            softser->flush();
            return false;
        }
        response = softser->readStringUntil('\n');
        response.trim();

        if (response.equals(command)) {
            Serial.print(F("Echo: "));
            Serial.println(command);
        } else if (response.equals(expectedResponse)) {
            Serial.print(F("Got expected response: "));
            Serial.println(response);
            result = true;
            break;
        } else if (response.length() == 0) {
            Serial.println(F("Discarding blank line"));
        } else {
            if (strict) {
                Serial.print(F("FAIL: Got unexpected response: "));
                Serial.println(response);
                result = false;
                break;
            } else {
                Serial.print(F("Discarding: "));
                Serial.println(response);
            }
        }
        if (softser->overflow()) {
            Serial.println(F("Buffer overflow!"));
        }
        searchedLines++;
    }

    if (requireOk) {
        response = softser->readStringUntil('\n');
        response.trim();
        if (response.length() == 0) {
            // Discard empty lines before the expected OK
            response = softser->readStringUntil('\n');
            response.trim();
        }
        if (!response.equals("OK")) {
            Serial.println(F("Expected OK but got:"));
            Serial.println(response);
            Serial.println(F("ERROR: Expected response to end with OK"));
            result = false;
        }
    }

    // Fail if we get more data than expected:
    if (softser->available() > 0) {
        result = false;
        Serial.println(F("Unexpected response:"));
        while (softser->available() > 0) {
            Serial.print((char)softser->read());
        }
        Serial.println();
    }
    softser->flush();

    printFreeMemory();
    Serial.println(F("sendAndExpectResponseLine]"));
    Serial.print(F("return "));
    Serial.println(result ? F("true") : F("false"));
    Serial.println();
    return result;
}
