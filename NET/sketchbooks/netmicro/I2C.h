#ifndef I2C_H
#define I2C_H
     
#include <Arduino.h>

class I2C {
  public:
    I2C(byte);

    void initialize();
    void onReceiveEvent(int);
    void onRequestEvent();

  private:
    I2C();
    byte address;
    byte registerPointer;

    byte connectState;
    byte wifiStatus;

    byte readSingleByte();
    void readAndThrowAwayRest();
};

#endif
