Transit Home Display
====================

_A display in your hallway which shows you the time left until the
bus/train/whatever leaves your closest public transit station._


User Story
----------

Alice, a bit tired this morning, is late for work. How much time is left until
the next subway train leaves towards the office? A quick glance at the LCD in
the hallway will tell her right away!


Architecture
------------

Small modules, connected through a shared I2C data bus.

Modules:

- Network microcontroller (`NET`)
- Web server (`SERVER`)
- Display microcontroller (`DISP`)
- Main microcontroller (`MAIN`)

`MAIN` is the I2C master. It will basically read from `SERVER` via `NET` and
write the response to `DISP`. The `SERVER` is responsible for fetching and
parsing real public transit data.

Asynchronous. Since web requests can take relatively long time to complete, the system is made async.
