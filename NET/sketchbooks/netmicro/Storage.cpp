#include "Storage.h"
#include <EEPROM.h>

Storage::Storage(int address, int size_in_bytes)
    : address(address),
      size_in_bytes(size_in_bytes) {}

void Storage::update(byte data[]) {
    byte a;
    byte offset = data[0];
    for (byte i = 1; i < 9; i++) {
        a = address + offset * 8 + i - 1;
        if (a > address + size_in_bytes - 1) {
            // Stop writing if it's passed its storage size
            return;
        }
        if (EEPROM.read(a) != data[i]) {
            EEPROM.write(a, data[i]);
        }
    }
}