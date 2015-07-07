/**
 * Arduino
 */

void setup(void) {
  initializeSerial();
  initializeI2C();
  //initializeWiFi();
  //connectToWiFi();
  //displayConnectionDetails();
  //foo();
  //disconnectFromWiFi();
}

void loop(void) {
 delay(10);
}

void initializeSerial(void) {
  Serial.begin(115200);
  Serial.println(F("Hello, CC3000!"));
}
