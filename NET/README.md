Network Microcontroller Module
==============================

Bill Of Materials
-----------------

* Arduino
* Adafruit CC3000 WiFi Module


I2C Protocol
------------

It uses a standard register-based protocol:

_"The details of the registers may differ from slave to slave, but all follow a standard pattern: write the first data byte to set the internal register-pointer, and then write the second data byte to transfer the data to the register pointed-to by the register-pointer (the first data byte)._

_When a master wants to read from the slave, it first writes the register-pointer to the slave, then starts a read transaction and reads out the data pointed-to by the register-pointer. Many devices allow multi-byte reads and writes, but it is in this way that they differ from device-to-device."_

Source: http://www.embedded.com/design/connectivity/4025661/Inter-IC-I2C-bus-design-and-test-for-embedded-systems-Part-1

```
Register   Register   R/W   Data
Name       Address
-----------------------------------------------------------
PING       0x01       R     PONG
STATUS     0x02       R     STARTING, OFFLINE, ONLINE,
                            LOADING, LOADED, FAILED, DEAD
URL        0x03       W
DATA       0x04       R
```

```
Label     Data
--------------
PONG      0x50
```
