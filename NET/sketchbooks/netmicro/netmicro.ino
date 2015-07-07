#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"

/**
 * Configure Pins
 **/

// MUST be an interrupt pin!
#define ADAFRUIT_CC3000_IRQ   3

// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10

// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11


/**
 * Global CC3000 Instance
 **/

Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ,
                                         ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIVIDER);


/**
 * Configure WiFi Credentials
 *
 * Set WLAN_SSID, WLAN_PASS and WLAN_SECURITY as #define's in this #included file.
 */

#include "wlan_credentials.h"


/**
 * General Configuration
 */

#define DHCP_TIMEOUT_MS 20000
#define IDLE_TIMEOUT_MS 3000
#define WEBSITE "www.adafruit.com"
#define WEBPAGE "/testwifi/index.html"


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


/**
 * CC3000 WiFi
 */

void initializeWiFi(void) {
 Serial.println(F("\nInitializing WiFi module..."));
  if (!cc3000.begin()) {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
}

void connectToWiFi(void) {
  Serial.print(F("\nAttempting to connect to "));
  Serial.println(WLAN_SSID);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  } else {
    Serial.println(F("Connected"));
  }
   
  Serial.print(F("Requesting DHCP"));
  int waited_ms = 0;
  while (!cc3000.checkDHCP()) {
    if (waited_ms >= DHCP_TIMEOUT_MS) {
      Serial.println(F("\nTimeout!"));
      while(1);
    }
    delay(1000);
    waited_ms += 1000;
    Serial.print(F("."));
  }
  Serial.println(F("\nGot IP"));
}

void disconnectFromWiFi(void) {
  Serial.println(F("\n\nDisconnecting"));
  cc3000.disconnect();
}

void displayConnectionDetails(void) {
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv)) {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    while(1);
  } else {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
  }
}


/**
 * Domain
 */

void foo(void) {
  uint32_t ip;
  
  // Try looking up the website's IP address
  ip = 0;
  Serial.print(WEBSITE); Serial.print(F(" -> "));
  while (ip == 0) {
    if (! cc3000.getHostByName(WEBSITE, &ip)) {
      Serial.println(F("Couldn't resolve!"));
    }
    delay(500);
  }
  cc3000.printIPdotsRev(ip);

  /* Try connecting to the website.
     Note: HTTP/1.1 protocol is used to keep the server from closing the connection before all data is read.
  */
  Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 80);
  if (www.connected()) {
    www.fastrprint(F("GET "));
    www.fastrprint(WEBPAGE);
    www.fastrprint(F(" HTTP/1.1\r\n"));
    www.fastrprint(F("Host: ")); www.fastrprint(WEBSITE); www.fastrprint(F("\r\n"));
    www.fastrprint(F("\r\n"));
    www.println();
  } else {
    Serial.println(F("Connection failed"));    
    return;
  }

  Serial.println(F("-------------------------------------"));
  
  /* Read data until either the connection is closed, or the idle timeout is reached. */ 
  unsigned long lastRead = millis();
  while (www.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    while (www.available()) {
      char c = www.read();
      Serial.print(c);
      lastRead = millis();
    }
  }
  www.close();
  Serial.println(F("-------------------------------------"));
}
