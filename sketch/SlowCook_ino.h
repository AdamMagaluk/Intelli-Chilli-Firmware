#include "slowcooker.h"

#define TEMP_SENSOR_PIN 2
#define HEATER_IO 13
#define LID_SWITCH 6

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

SlowCooker slowcooker(HEATER_IO,TEMP_SENSOR_PIN,LID_SWITCH,&eventCallback);

void setup()
{
  Serial.begin(115200);          //  setup serial
  slowcooker.setup();
  slowcooker.setCookTime(2); // two minutes
  slowcooker.startCook();
}

void loop()
{
  slowcooker.loop();
}
