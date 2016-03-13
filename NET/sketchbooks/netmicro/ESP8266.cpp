#include "ESP8266.h"

#include "MemoryFree.h"

#include "constants.h"

ESP8266::ESP8266(AltSoftSerial * softser,
                 Storage * ssidStorage,
                 Storage * passphraseStorage,
                 Storage * hostStorage,
                 Storage * pathStorage) :
    softser(softser),
    ssidStorage(ssidStorage),
    passphraseStorage(passphraseStorage),
    hostStorage(hostStorage),
    pathStorage(pathStorage) {}

bool ESP8266::initialize() {
    Serial.println(F("ESP8266 initializing..."));

    // Device is OFF by default
    pinMode(ESP8266_ON_OFF_PIN, OUTPUT);
    digitalWrite(ESP8266_ON_OFF_PIN, LOW);

    // Soft serial connection to ESP8266
    // AT+UART_DEF=57600,8,1,0,0
    softser->begin(57600);
    softser->setTimeout(ESP8266_DEFAULT_COMMAND_TIMEOUT);

    Serial.println(F("ESP8266 initialized"));
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

bool ESP8266::connect() {
    Serial.print(F("ESP8266 resetting before connecting..."));

    // Hard reset
    digitalWrite(ESP8266_ON_OFF_PIN, LOW);
    delay(100);
    softser->flush();
    digitalWrite(ESP8266_ON_OFF_PIN, HIGH);
    while (softser->available() == 0);
    if (!softser->find("ready\r\n")) {
        Serial.println(F("ERROR: Could not hard reset"));
        return false;
    }

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

    // Join WiFi
    softser->print("AT+CWJAP=\"");

    Serial.print(F("SSID: "));
    ssidStorage->printTo(softser);
    Serial.println();

    softser->print("\",\"");

    Serial.print(F("PASSPHRASE: "));
    passphraseStorage->printTo(softser);
    Serial.println();

    softser->print("\"");

    if (!sendVoidCommand("", 30000, 3)) {
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

    softser->print("AT+CIPSTART=\"TCP\",\"");

    Serial.print(F("HOST: "));
    hostStorage->printTo(softser);
    Serial.println();

    softser->print("\",80");

    printFreeMemory();
    Serial.println(F("ESP8266 Starting TCP connection"));

    if (!sendAndExpectResponseLine("", "CONNECT", true, false, 30000)) {
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

    long pathLength = pathStorage->length();
    long hostLength = hostStorage->length();

    // See below to understand where 4, 17 and 4 comes from
    String cipsendLength = String(4 + pathLength + 17 + hostLength + 4);

    char * cipsend = "AT+CIPSEND=";
    softser->print(cipsend);
    softser->print(cipsendLength.c_str());
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

    softser->print("GET "); // 4

    Serial.print(F("PATH: "));
    pathStorage->printTo(softser);
    Serial.println();

    softser->print(" HTTP/1.0\r\nHost: "); // 17

    Serial.print(F("HOST: "));
    hostStorage->printTo(softser);
    Serial.println();

    softser->print("\r\n\r\n"); // 4

    // Recv 37 bytes
    //
    // SEND OK
    //

    Serial.println(F("Sent CIPSEND, parsing response..."));

    while (softser->available() == 0);
    if (!softser->find("Recv ")) {
        Serial.println(F("ERROR: Could not find beginning of CIPSEND response"));
        Serial.println(F("sendHttpGetRequest]"));
        Serial.println();
        return false;
    }
    Serial.println(F("Got 'Recv '"));
    String recvBytes = softser->readStringUntil(' ');
    long recvBytes_long = recvBytes.toInt();
    if (recvBytes_long != cipsendLength.toInt()) {
        Serial.println(F("ERROR: Could not send CIPSEND message"));
        Serial.print(F("Expected ESP8266 to have received: "));
        Serial.print(cipsendLength.toInt());
        Serial.println();
        Serial.print(F("But it received: "));
        Serial.print(recvBytes_long);
        Serial.println();
        return false;
    }

    Serial.println(F("Got number of bytes"));
    while (softser->available() == 0);
    if (!softser->find("bytes")) {
        Serial.println(F("ERROR: Could not find end of CIPSEND response"));
        Serial.println(F("sendHttpGetRequest]"));
        Serial.println();
        return false;
    }
    Serial.println(F("Got 'bytes'"));

    while (softser->available() == 0);
    if (!softser->find("SEND OK")) {
        Serial.println(F("ERROR: Could not find SEND OK"));
        Serial.println(F("sendHttpGetRequest]"));
        Serial.println();
        return false;
    }

    softser->flush();

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
    char cBuff[ESP8266_CHAR_BUFF_SIZE + 1] = {0};
    int bytesRead = 0;
    String incomingDataLengthString;
    long bytesLeft;
    char ipdOrClose[6] = {0};
    long discarded = 0;
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
            } else if (step == 7) {
                // Store what fits in the cBuff
                cBuff[bytesRead - 1] = c;

                if (bytesRead == ESP8266_CHAR_BUFF_SIZE) {
                    step++;
                }
            } else {
                //Serial.print(c);
                discarded++;
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
    Serial.println(F("=============="));
    Serial.println(cBuff);
    Serial.println(F("=============="));
    Serial.print(F("Discarded: "));
    Serial.println(discarded, DEC);
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
                if (response.equals("ERROR")) {
                    Serial.println(F("Unexpected 'ERROR' response"));
                    return false;
                } else if (response.equals("DNS Fail")) {
                    Serial.println(F("Unexpected 'DNS Fail' response"));
                    return false;
                } else {
                    Serial.print(F("Discarding: "));
                    Serial.println(response);
                }
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
