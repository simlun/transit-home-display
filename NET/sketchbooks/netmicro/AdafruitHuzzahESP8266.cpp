#include "AdafruitHuzzahESP8266.h"

#include <SoftwareSerial.h>

#define ARD_RX_ESP_TX 2
#define ARD_TX_ESP_RX 3
SoftwareSerial softser(ARD_RX_ESP_TX, ARD_TX_ESP_RX); // Arduino RX = ESP TX, Arduino TX = ESP RX

bool AdafruitHuzzahESP8266::wpa2Connect() {
    // Soft serial connection to ESP8266
    softser.begin(9600);

    return false;
}
