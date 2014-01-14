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


#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include "protocol.h"
#include "parser.h"
#include "slowcooker.h"

#define TEMP_SENSOR_PIN 2
#define HEATER_IO 4
#define LID_SWITCH 7

struct HttpCommand {
  char* cmd;
  void (*func)(YunClient&);
};

void eventCallback(StatusEvent event);
void sendEvent(StatusEvent event);

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
  {0, 0}
};

SlowCooker slowcooker(HEATER_IO, TEMP_SENSOR_PIN, LID_SWITCH, &eventCallback);

// Listen on default port 5555, the webserver on the Yun
// will forward there all the HTTP requests for us.
YunServer server;

void setup() {

  Serial.begin(115200);
  
  delay(10000);
  
  Serial.println(F("Initializing..."));
  //setup slowcooker
  slowcooker.setup();

  Bridge.begin();

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();

  sendEvent(EVENT_POWERED_ON);
  Serial.println(F("Starting main loop."));
}

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

  delay(50); // Poll every 50ms
}

void process(YunClient& client) {
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
      Serial.println("EVENT: LID Openned.");
      sendEvent(EVENT_LID_OPENED);
      break;
    case EVENT_LID_CLOSED:
      sendEvent(EVENT_LID_CLOSED);
      Serial.println("EVENT: LID Closed.");
      break;
    case EVENT_COOK_END:
      sendEvent(EVENT_COOK_END);
      Serial.println("EVENT: Cook finished turning off.");
      break;
  };
}


void sendEvent(StatusEvent event) {

}

void handle_state(YunClient& client) {
  Serial.println(F("Handle state"));
  
  HTTP_STATUS(200);
  
  client.print(F("{\"cookTime\":"));
  client.print(slowcooker.CookTime());
  client.print(F(",\"cookTimeLeft\":"));
  client.print(slowcooker.CookTimeLeft());
  client.print(F(",\"cookTemp\":"));
  client.print(slowcooker.CookTemp());
  client.print(F(",\"currentTemp\":"));
  client.print(slowcooker.CurrentTemp());
  client.print(F(",\"lidState\":"));
  client.print(slowcooker.isLidOpen());
  client.print(F(",\"cooking\":"));
  client.print(slowcooker.isCooking());
  client.print(F(",\"heaterOn\":"));
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
  if (!slowcooker.setCookTemp(temp)) {
    HTTP_STATUS(400);
    client.println(F("{\"error\":\"Failed to set cook temp.\"}"));
    return;
  }
  
  HTTP_STATUS(200);
  client.print(F("{\"temp\":"));
  client.print(temp);
  client.println(F("}"));
}

