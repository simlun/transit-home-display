#include <Wire.h>

void initializeI2C(void) {
  Serial.println(F("\nInitializing I2C"));
  Wire.begin(0x07);
  Wire.onRequest(requestEvent);
}


void requestEvent() {
  Serial.println(F("\nGot I2C request"));
  Wire.write(0x50);
  Serial.println(F("Wrote I2C data"));
}
