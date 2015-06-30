Network Microcontroller Module
==============================

Bill Of Materials
-----------------

* Arduino
* Adafruit CC3000 WiFi Module


I2C Protocol
------------

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
