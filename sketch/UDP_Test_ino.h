/***************************************************
  
 ****************************************************/
#include <Adafruit_CC3000_UDP_Server.h>

#include <Adafruit_CC3000.h>
#include <SPI.h>
#include "utility/debug.h"
#include "utility/socket.h"
#include <string.h>

#include "protocol.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10

// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS,
                                         ADAFRUIT_CC3000_IRQ,
                                         ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIV2
                                        );

#define WLAN_SSID       "Loft21"
#define WLAN_PASS       "silkylotus997"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define LISTEN_PORT           3000    // What TCP port to listen on for connections.  The echo protocol uses port 7.

#define BUFFER_SIZE 64
uint8_t buffer[BUFFER_SIZE];
char buf[BUFFER_SIZE];
Message message;

Adafruit_CC3000_UDP_Server echoServer = Adafruit_CC3000_UDP_Server(LISTEN_PORT);


/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("IP Addr: ")); cc3000.printIPdotsRev(ipAddress); Serial.println();
    Serial.print(F("Netmask: ")); cc3000.printIPdotsRev(netmask); Serial.println();
    Serial.print(F("Gateway: ")); cc3000.printIPdotsRev(gateway); Serial.println();
    Serial.print(F("DHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv); Serial.println();
    Serial.print(F("DNSserv: ")); cc3000.printIPdotsRev(dnsserv); Serial.println();
    
    return true;
  }
}

void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("Hello, CC3000!\n")); 

  Serial.print("Free RAM: "); Serial.println(getFreeRam(), DEC);
  
  /* Initialise the module */
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
  
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
   
  Serial.println(F("Connected!"));
  
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }  

  /* Display the IP address DNS, Gateway, etc. */  
  while (! displayConnectionDetails()) {
    delay(1000);
  }

  // Start listening for connections
  echoServer.begin();
  
  Serial.println(F("Listening for connections..."));
}

void loop(void)
{
  // Try to get a client which is connected.
  
  if (echoServer.available()) {
    sockaddr_in remote;
    int bytesRead = echoServer.read(buf, 512, (sockaddr *)&remote);
    
    for(int i=0;i<bytesRead;i++)
      Serial.print(buf[i]);
    Serial.println("");

    echoServer.write(buf, bytesRead, (sockaddr *)&remote);
  }
}
