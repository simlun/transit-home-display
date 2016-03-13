Network Microcontroller Module
==============================

Bill Of Materials
-----------------

* Arduino
* ESP8266 WiFi Module
* N-channel MOSFET


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

(Current development status: 1-8 are implemented)

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

TODO: Write `DISCONNECT` to the `CONNECT` address to disconnect.

TODO: Write `GPRS` to the `CONNECT` address to connect using a GPRS module
instead of WiFi.


Registers and Labels
--------------------

```
Register   Register   R/W   EEPROM   Data
Name       Address          Address
-----------------------------------------------------------
PING       0x01       R              PONG
STATUS     0x02       R              STARTING,
                                     OFFLINE, ONLINE,
                                     CONNECTING, DISCONNECTING,
                                     BUSY,
                                     FAILED, DEAD

SSID       0x10       W     0x00     0x00 + [8 first ASCII characters]
SSID       0x10       W     0x08     0x01 + [8 ASCII characters]
SSID       0x10       W     0x10     0x02 + [8 ASCII characters]
SSID       0x10       W     0x18     0x03 + [8 last ASCII characters]

PASSPHRASE 0x11       W     0x20     0x00 + [8 first ASCII characters]
PASSPHRASE 0x11       W     0x28     0x01 + [8 ASCII characters]
PASSPHRASE 0x11       W     0x30     0x02 + [8 ASCII characters]
PASSPHRASE 0x11       W     0x38     0x03 + [8 last ASCII characters]

CONNECT    0x12       W              {WPA2, WPA, WEP}

HOST       0x20       W     0x40     0x00 + [first 8 ASCII characters]
HOST       0x20       W     0x48     0x01 + [8 ASCII characters]
HOST       0x20       W     0x50     0x02 + [8 ASCII characters]
HOST       0x20       W     0x58     0x03 + [8 ASCII characters]
HOST       0x20       W     0x60     0x04 + [8 ASCII characters]
HOST       0x20       W     0x68     0x05 + [8 ASCII characters]
HOST       0x20       W     0x70     0x06 + [8 ASCII characters]
HOST       0x20       W     0x78     0x07 + [last 8 ASCII characters]

PATH       0x21       W     0x80     0x00 + [first 8 ASCII characters]
PATH       0x21       W     0x88     0x01 + [8 ASCII characters]
PATH       0x21       W     0x90     0x02 + [8 ASCII characters]
PATH       0x21       W     0x98     0x03 + [8 ASCII characters]
PATH       0x21       W     0xA0     0x04 + [8 ASCII characters]
PATH       0x21       W     0xA8     0x05 + [8 ASCII characters]
PATH       0x21       W     0xB0     0x06 + [8 ASCII characters]
PATH       0x21       W     0xB8     0x07 + [last 8 ASCII characters]

DO         0x22       W              {GET}
RESPONSE   0x23       R              [256 bytes]
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
