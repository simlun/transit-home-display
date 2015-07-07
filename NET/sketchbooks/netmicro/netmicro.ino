/**
 * Arduino
 */

void setup(void) {
  initializeSerial();
  initializeWiFi();
  connectToWiFi();
  displayConnectionDetails();
  foo();
  disconnectFromWiFi();
}

void loop(void) {
 delay(10);
}


void initializeSerial(void) {
  Serial.begin(115200);
  Serial.println(F("Hello, CC3000!"));
}
