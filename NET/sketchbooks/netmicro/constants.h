#ifndef CONSTANTS_H
#define CONSTANTS_H


/**
 * Register Names
 */

#define PING       0x01
#define STATUS     0x02

#define SSID       0x10
#define PASSPHRASE 0x11
#define CONNECT    0x12

#define HOST       0x20
#define PATH       0x21
#define DO         0x22
#define RESPONSE   0x23


/**
 * Labels
 */

#define NULL          0x00
#define PONG          0x50
#define STARTING      0x60
#define OFFLINE       0x61
#define WPA2          0x62
#define WPA           0x63
#define WEP           0x64
#define CONNECTING    0x65
#define ONLINE        0x66
#define DISCONNECTING 0x67
#define BUSY          0x68
#define GET           0x69
#define PUT           0x6A
#define POST          0x6B
#define DELETE        0x6C
#define EEPROM_DEBUG  0x70
#define FAILED        0xF0
#define DEAD          0xF1


/**
 * EEPROM Offsets
 */

#define EEPROM_OFFSET_SSID       0x00
#define EEPROM_OFFSET_PASSPHRASE 0x20


#endif
