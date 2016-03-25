#include <Wire.h>

#define DISP_I2C_SLAVE_ADDRESS 0x17

byte I2C_PING = 0x01;
byte I2C_CLEAR = 0x02;
byte I2C_DATA = 0x03;
byte I2C_REFRESH = 0x04;

void transmitCmd(byte cmd) {
  Wire.beginTransmission(DISP_I2C_SLAVE_ADDRESS);
  Wire.write(cmd);
  Wire.endTransmission();  
}

void transmitData(byte data) {
  Wire.beginTransmission(DISP_I2C_SLAVE_ADDRESS);
  Wire.write(I2C_DATA);
  Wire.write(data);
  Wire.endTransmission();  
}

void transmitData(char * data) {
  int i = 0;
  char c = data[i];
  while (c != '\0') {
    transmitData(c);
    i++;
    c = data[i];
  }
}

void setup() {
  Wire.begin();

  transmitCmd(I2C_CLEAR);
  transmitData("Foo\nbar\nbaz hej svej lol roflmao!");
  transmitCmd(I2C_REFRESH);
}

void loop() {}
