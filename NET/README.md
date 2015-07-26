Network Microcontroller Module
==============================

Bill Of Materials
-----------------

* Arduino
* Adafruit CC3000 WiFi Module


I2C Protocol
------------

It uses a standard register-based protocol:

_"The details of the registers may differ from slave to slave, but all follow a
standard pattern: write the first data byte to set the internal
register-pointer, and then write the second data byte to transfer the data to
the register pointed-to by the register-pointer (the first data byte)._

_When a master wants to read from the slave, it first writes the
register-pointer to the slave, then starts a read transaction and reads out the
data pointed-to by the register-pointer. Many devices allow multi-byte reads
and writes, but it is in this way that they differ from device-to-device."_

Source:
http://www.embedded.com/design/connectivity/4025661/Inter-IC-I2C-bus-design-and-test-for-embedded-systems-Part-1

Usage
-----

1. Read from the `PING` registry and get a `PONG` response to see that the
   device is fundamentally working.
2. Wait for the device to be in the `OFFLINE` state by reading the `STATUS`
   registry.
3. Write to the `SSID` and `PASSPHRASE` registrys to give WiFi connection
   credentials to the device.
4. Write `WPA2` to the `CONNECT` registry to initiate a WiFi connection.
   (TODO: support `WEP` and `WPA`).
5. `STATUS` should proceed to `CONNECTING` and then `ONLINE`.
6. Write `HOST` and `PATH`, i.e `http://<HOST>:80<PATH>`.
7. Write `GET` to the `DO` registry to initiate a HTTP GET. (TODO: support
   `PUT`, `POST` and `DELETE`).
8. `STATUS` should proceed to `BUSY` and then `ONLINE`.
9. Read up to 256 bytes from `RESPONSE` to get the HTTP response.

TODO: The `SSID`, `PASSPHRASE`, `HOST` and `PATH` will be stored in EEPROM
(where memory is kept after reset and power loss).


Registers and Labels
--------------------

```
Register   Register   R/W   Data
Name       Address
-----------------------------------------------------------
PING       0x01       R     PONG
STATUS     0x02       R     STARTING,
                            OFFLINE, ONLINE,
                            CONNECTING, DISCONNECTING,
                            BUSY,
                            FAILED, DEAD

SSID       0x03       W     [32 ASCII characters]
PASSPHRASE 0x04       W     [32 ASCII characters]
CONNECT    0x05       W     {WPA2, DISCONNECT}

HOST       0x06       W     [32 ASCII characters]
PATH       0x07       W     [32 ASCII characters]
DO         0x08       W     {GET}
RESPONSE   0x09       R     [256 bytes]
```

```
Label         Data
------------------
NULL          0x00
PONG          0x50
STARTING      0x60
OFFLINE       0x61
WPA2          0x62
WPA           0x63
WEP           0x64
CONNECTING    0x65
ONLINE        0x66
DISCONNECTING 0x67
BUSY          0x68
GET           0x69
PUT           0x6A
POST          0x6B
DELETE        0x6C
FAILED        0xF0
DEAD          0xF1
```
