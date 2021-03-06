/*
  Arduino Yun Bridge example

 This example for the Arduino Yun shows how to use the
 Bridge library to access the digital and analog pins
 on the board through REST calls. It demonstrates how
 you can create your own API when using REST style
 calls through the browser.

 Possible commands created in this shetch:

 * /arduino/start
 * /arduino/stop
 * /arduino/ping
 * /arduino/reset
 * /arduino/state
 * /arduino/time/:val
 * /arduino/temp/:val

 This example code is part of the public domain

 http://arduino.cc/en/Tutorial/Bridge

 */


#include <avr/wdt.h>
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include "slowcooker.h"

#define TEMP_SENSOR_PIN 2
#define HEATER_IO 4
#define LID_SWITCH 5

struct HttpCommand {
  char* cmd;
  void (*func)(YunClient&);
};

void eventCallback(StatusEvent event);
void sendEvent(StatusEvent event);
void hardwareReset();

/*
 * /arduino/start
 * /arduino/stop
 * /arduino/ping
 * /arduino/reset
 * /arduino/state
 * /arduino/time/:val
 * /arduino/temp/:val
 */
void handle_state(YunClient& client);
void handle_ping(YunClient& client);
void handle_start(YunClient& client);
void handle_stop(YunClient& client);
void handle_reset(YunClient& client);
void handle_set_time(YunClient& client);
void handle_set_temp(YunClient& client);
void handle_test_event(YunClient& client);

#define HTTP_STATUS(code)   client.print(F("Status: "));client.println(code); \
                            client.println(F("Content-Type: application/json")); \
                            client.println();

static struct HttpCommand http_endpoints[] = {
  {"state", handle_state},
  {"ping", handle_ping},
  {"start", handle_start},
  {"stop", handle_stop},
  {"reset", handle_reset},
  {"time", handle_set_time},
  {"temp", handle_set_temp},
  {"testevent", handle_test_event},
  {0, 0}
};

SlowCooker slowcooker(HEATER_IO, TEMP_SENSOR_PIN, LID_SWITCH, &eventCallback);

// Listen on default port 5555, the webserver on the Yun
// will forward there all the HTTP requests for us.
YunServer server;
Process eventProcess;
bool resetFlag;

#define RESET_PIN 12

void setup() {
  digitalWrite(RESET_PIN, HIGH);
  delay(40);
  pinMode(RESET_PIN,OUTPUT);
  
  digitalWrite(RESET_PIN, HIGH);
  
  resetFlag = false;
  Serial.begin(115200);
  
  //setup slowcooker
  slowcooker.setup();

  Bridge.begin();

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();

//  sendEvent(EVENT_POWERED_ON);
  
}


bool it = true;
void loop() {
  // handle slow cooker functions.
  slowcooker.loop();

  // Get clients coming from server
  YunClient client = server.accept();

  // There is a new client?
  if (client) {
    // Process request
    process(client);
    // Close connection and free resources.
    client.stop();
  }
  
  if(resetFlag){
    hardwareReset();
  }
  
  delay(50); // Poll every 50ms
}

void process(YunClient& client) {
  it = !it;
  
  // read the command
  String command = client.readStringUntil('/');

  // handle /state with no trailing /
  if (command[command.length() - 2] == 0xD && command[command.length() - 1] == 0xA) {
    command = command.substring(0, command.length() - 2);
  }
       
  int i = 0;
  while (1) {
    if (http_endpoints[i].cmd == 0)
      break;

    if (command.equals(http_endpoints[i].cmd)) {
      http_endpoints[i].func(client);
      return;
    }

    i++;
  }

  HTTP_STATUS(404);
  client.println(F("{\"error\":\"Command not found.\"}"));
}

void eventCallback(StatusEvent event) {
  switch (event) {
    case EVENT_LID_OPENED:
      sendEvent(EVENT_LID_OPENED);
      break;
    case EVENT_LID_CLOSED:
      sendEvent(EVENT_LID_CLOSED);
      break;
    case EVENT_COOK_END:
      sendEvent(EVENT_COOK_END);
      break;
  };
}


void sendEvent(StatusEvent event) {
  if(eventProcess.running()){
    return;
  }
  
  char buf[4];
  sprintf(buf,"%d",event);
 
  eventProcess.begin(F("intellichillievent"));
  eventProcess.addParameter(buf);
  eventProcess.runAsynchronously();
}

void handle_state(YunClient& client) {
 
  Serial.println(F("Handle state"));
  
  HTTP_STATUS(200);
  
  client.print(F("{\"cookTime\":"));
  client.print(slowcooker.CookTime());
  client.print(F(",\"cookTimeLeft\":"));
  client.print(slowcooker.CookTimeLeft());
  client.print(F(",\"cookTemp\":\""));
  client.print(SlowCooker::cookTempToText(slowcooker.CookTemp()));
  client.print(F("\",\"currentTemp\":"));
  client.print(slowcooker.CurrentTemp());
  client.print(F(",\"lidState\":"));
  client.print(slowcooker.isLidOpen());
  client.print(F(",\"cooking\":"));
  client.print(slowcooker.isCooking());
  client.print(F(",\"address\":\""));
   
  for(int i=0;i<8;i++){
    if( slowcooker.m_sensorAddr[i] < 0x10)
      client.print("0");
    client.print(slowcooker.m_sensorAddr[i],HEX);
  }
  
  client.print(F("\",\"heaterOn\":"));
  client.print(slowcooker.isHeaterActive());
    
  if(!slowcooker.tempSensorFound()){
    client.print(F(",\"error\":\"No temperature sensor found.\""));
  }
  
  client.println(F("}"));
}

void handle_ping(YunClient& client) {
  HTTP_STATUS(200);
  client.println(F("{}"));
}

void handle_start(YunClient& client) {
  HTTP_STATUS(200);
  slowcooker.startCook();
  client.println(F("{}"));
}

void handle_stop(YunClient& client) {
  HTTP_STATUS(200);
  slowcooker.stopCook();
  client.println(F("{}"));
}

void handle_reset(YunClient& client) {
  HTTP_STATUS(200);
  client.println(F("{}"));
  
  resetFlag = true;
}

void handle_set_time(YunClient& client) {
  if(client.available() == 0){
    HTTP_STATUS(400);
    client.println(F("{\"error\":\"Must supply a time value.\"}"));
    return;
  }
  
  
  int time = client.parseInt();
  if (!slowcooker.setCookTime(time)) {
    HTTP_STATUS(400);
    client.println(F("{\"error\":\"Failed to set cook time.\"}"));
    return;
  }
  
  HTTP_STATUS(200);
  client.print(F("{\"time\":"));
  client.print(time);
  client.println(F("}"));
}

void handle_set_temp(YunClient& client) {
  if(client.available() == 0){
    HTTP_STATUS(400);
    client.println(F("{\"error\":\"Must supply a temp value.\"}"));
    return;
  }
  
  int temp = client.parseInt();
  if(!SlowCooker::validCookTemp(temp)){
    HTTP_STATUS(400);
    client.println(F("{\"error\":\"Non valid cook temp.\"}"));
    return;
  }
  
  slowcooker.setCookTemp((SlowCooker::TempSetting)temp);
  
  HTTP_STATUS(200);
  client.print(F("{\"temp\":\""));
  client.print(SlowCooker::cookTempToText(slowcooker.CookTemp()));
  client.println(F("\"}"));
}

void handle_test_event(YunClient& client) {
  sendEvent(EVENT_POWERED_ON);
  client.println(F("{}"));
}


void hardwareReset(){
  Process p;
  p.begin("reboot");
  p.run();
  delay(500);
  digitalWrite(RESET_PIN, LOW);
}

