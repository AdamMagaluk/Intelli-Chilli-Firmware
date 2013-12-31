/***************************************************
  
 ****************************************************/
#include <Adafruit_CC3000.h>
#include <SPI.h>
#include "utility/debug.h"
#include "utility/socket.h"

#include "protocol.h"
#include "parser.h"
#include "slowcooker.h"
#include "router.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10

#define TEMP_SENSOR_PIN 0
#define HEATER_IO 1
#define LID_SWITCH 6

#define WLAN_SSID       "Loft21"
#define WLAN_PASS       "silkylotus997"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2
#define LISTEN_PORT           3000    // What TCP port to listen on for connections.  The echo protocol uses port 7.
#define BUFFER_SIZE 25

bool displayConnectionDetails(void);
void eventCallback(StatusEvent event);

// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS,
                                         ADAFRUIT_CC3000_IRQ,
                                         ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIV2
                                        );

Adafruit_CC3000_Server echoServer(LISTEN_PORT);
SlowCooker slowcooker(HEATER_IO,TEMP_SENSOR_PIN,LID_SWITCH,&eventCallback);
PacketRouter router(slowcooker);

uint8_t buffer[BUFFER_SIZE];
uint8_t sendBuffer[BUFFER_SIZE];
Message msg;


void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("Hello, CC3000!\n")); 

  slowcooker.setup();

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

  slowcooker.loop();

  // Try to get a client which is connected.
  Adafruit_CC3000_ClientRef client = echoServer.available();
  if (client) {

    uint16_t dataAvailable = client.available();
     // Check if there is data available to read.
     if (dataAvailable > 0) {

       uint8_t bRead = client.read(buffer,BUFFER_SIZE,0);
       if(parse_packet(buffer,bRead,&msg)){

        uint8_t sendLen = router.route(msg,sendBuffer);
        if(sendLen > 0){
          client.write(sendBuffer,sendLen);
          delay(300);
        }

       }else{
        Serial.println("failed to parse packet");
       }

       client.close();
     }
  }
}

void eventCallback(StatusEvent event){
  switch(event){
    case EVENT_LID_OPENED:
      Serial.println("EVENT: LID Openned.");
      break;
    case EVENT_LID_CLOSED:
      Serial.println("EVENT: LID Closed.");
      break;
    case EVENT_COOK_END:
      Serial.println("EVENT: Cook finished turning off.");
      break;
  };
}

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
