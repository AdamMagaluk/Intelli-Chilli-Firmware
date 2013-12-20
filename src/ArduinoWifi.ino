/***************************************************
  
 ****************************************************/

#include "slowcooker.h"

#define TEMP_SENSOR_PIN 0
#define HEATER_IO 13
#define LID_SWITCH 8


void eventCallback(int event){

}

SlowCooker slowcooker(HEATER_IO,TEMP_SENSOR_PIN,LID_SWITCH,&eventCallback);


void setup()
{
  Serial.begin(115200);          //  setup serial
}

void loop()
{
  slowcooker.loop();
}