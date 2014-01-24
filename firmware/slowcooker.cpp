#include <Arduino.h>
#include <OneWire.h>
#include <Time.h>
#include <DallasTemperature.h>

#include "slowcooker.h"

#define MAX_HEAT 120


char* SlowCooker::cookTempToText(const TempSetting& temp){
   switch(temp){
     case SlowCooker::TEMP_WARM:
       return "warm";
     case SlowCooker::TEMP_LOW:
       return "low";
     case SlowCooker::TEMP_MED:
       return "medium";
     case SlowCooker::TEMP_HIGH:
       return "high";
     default:
       return "undefined";
   }
}

bool SlowCooker::validCookTemp(int temp){
   switch(temp){
     case SlowCooker::TEMP_WARM:
     case SlowCooker::TEMP_LOW:
     case SlowCooker::TEMP_MED:
     case SlowCooker::TEMP_HIGH:
       return true;
     default:
       return false;
   }
}

SlowCooker::SlowCooker(const int& heaterRelay, const int& tempSensor, const int& lidSwitch, void (*eventCallback)(StatusEvent)) :
  m_heaterRelayPin(heaterRelay),
  m_tempSensorPin(tempSensor),
  m_lidSwitchPin(lidSwitch),
  m_eventCallback(eventCallback),
  m_cookTime(DEFAULT_COOK_TIME),
  m_state(STATE_NOT_COOKING),
  m_heaterActive(false),
  m_oneWire(m_tempSensorPin),
  m_sensor(&m_oneWire),
  m_isWarming(false),
  m_tempSetting(TEMP_WARM)
{ 
  // start with heater off
  
}

void SlowCooker::setup(){
  pinMode(m_heaterRelayPin, OUTPUT);
  pinMode(m_lidSwitchPin, INPUT);

  setHeaterState(LOW);
  initTempSensor();
}

// calculates if heater should be active.  
bool SlowCooker::calcHeaterState(){
    
  // if warming heating element up, ramp to max temp
  if(m_isWarming){
    // turn heater on/off based on temp.
    if(m_currentTemp <= MAX_HEAT){
      return true;
    }else {
      Serial.println("Reached warming state.");
      m_isWarming = false;
      return false;
    }
  }
  
  long delta = millis()-m_heaterOnTime;  
  if(m_heaterActive){
    // calc how long the each interval should run. m_tempSetting is a percentage of cook_interval
    long tOn = ((m_tempSetting / 10.0) * COOK_INTERVAL);
    Serial.print("Heater On: ");
    Serial.print(delta);
    Serial.print(" >= ");
    Serial.println(tOn);
    
    if(delta >= tOn)
      return false;
    else
      return true;
  }else{
    // calc how long the each interval should run. 10-m_tempSetting is a percentage of cook_interval
    long tOn = ((10-m_tempSetting / 10.0) * COOK_INTERVAL);
    
    Serial.print("Heater Off: ");
    Serial.print(delta);
    Serial.print(" >= ");
    Serial.println(tOn);

    
    if(delta >= tOn){
      // off for desired time, now turn back on.
      return true;
    }else{
      // keep off
      return false;
    }
  }
  
}

// run once in main loop, handles reading pin vals
// setting heater on/off
void SlowCooker::loop(){
  m_lastLidState = m_lidState;
  m_lidState = digitalRead(m_lidSwitchPin);
  m_currentTemp = readTemp(); // float in C

  if(m_state == STATE_NOT_COOKING)
    return;
  
  if(calcHeaterState()){
    if(!m_heaterActive){
      Serial.println(F("Turning Heater On"));
      m_heaterOnTime = millis();
    }
    setHeaterState(true);
  }else{
    if(m_heaterActive){
      Serial.println(F("Turning Heater Off"));
      m_heaterOnTime = millis();
    }
    setHeaterState(false);
  }
  
  
  // check if cook is finished based on start time.
  // if cook time is zero then it will run forever!!!
  if(m_cookTime != 0){
    long sLeft = (m_cookTime * 60) - ((millis() - m_timeCookStarted)/1000);
    if(sLeft <= 0){
      stopCook();
      m_eventCallback(EVENT_COOK_END);
    }
  }

  // send event if lid has been opened or closed.
  if(m_lastLidState && !m_lidState){
    m_eventCallback(EVENT_LID_CLOSED);
  }else if(!m_lastLidState && m_lidState){
    m_eventCallback(EVENT_LID_OPENED);
  }

}

void SlowCooker::initTempSensor(){
  m_sensor.begin();

  // locate devices on the bus
  m_sensor.getDeviceCount();

  // search for devices on the bus and assign based on an index
  if (!m_sensor.getAddress(m_sensorAddr, 0)){
    m_tempFound = false;
    Serial.println(F("Unable to find address for Device 0"));
  }else{
    m_tempFound = true;
  }

  m_sensor.setResolution(m_sensorAddr, 9);
}

bool SlowCooker::tempSensorFound() const{
  return m_tempFound;
}

float SlowCooker::readTemp(){
  m_sensor.requestTemperatures();
  float tempC = m_sensor.getTempC(m_sensorAddr);
  return tempC;
}

// resets cook time to 0:00
// resets heat to warm level.  
void SlowCooker::resetToDefault() {
  m_cookTime = DEFAULT_COOK_TIME;
  m_tempSetting = TEMP_WARM;
}

void SlowCooker::setHeaterState(bool state) {
  m_heaterActive = state;
  // set heater pin
  digitalWrite(m_heaterRelayPin, state);
}

bool SlowCooker::isCooking(){
  return (m_state == STATE_COOKING);
}

// start cooking, at current temp and start counting down time.
bool SlowCooker::startCook() {
  m_state = STATE_COOKING;
  m_timeCookStarted = millis();
  m_isWarming = true;
}

// stops cooking, resets counter.
bool SlowCooker::stopCook() {
  m_state = STATE_NOT_COOKING;
  resetToDefault();
  setHeaterState(LOW);
}

bool SlowCooker::setCookTime(const uint16_t& minutes) {
  if(minutes > 1440)
    return false;
  
  m_cookTime = minutes;
  m_timeCookStarted = millis();

  return true;
}

bool SlowCooker::setCookTemp(const TempSetting& temp) {
  m_tempSetting = temp;
  return true;
}

uint16_t SlowCooker::CookTime() const {
  return m_cookTime;
}

uint16_t SlowCooker::CookTimeLeft() const {
  if(m_cookTime == 0)
    return 0;

  return ((m_cookTime * 60) - ((millis() - m_timeCookStarted)/1000))/60;
}

SlowCooker::TempSetting SlowCooker::CookTemp() const {
  return m_tempSetting;
}

uint8_t SlowCooker::CurrentTemp() const {
  return m_currentTemp;
}

bool SlowCooker::isLidOpen() const {
  return m_lidState;
}

bool SlowCooker::isHeaterActive() const {
  return m_heaterActive;
}
