#include <Arduino.h>
#include <OneWire.h>
#include <Time.h>
#include <DallasTemperature.h>

#include "slowcooker.h"

SlowCooker::SlowCooker(const int& heaterRelay, const int& tempSensor, const int& lidSwitch, void (*eventCallback)(StatusEvent)) :
  m_heaterRelayPin(heaterRelay),
  m_tempSensorPin(tempSensor),
  m_lidSwitchPin(lidSwitch),
  m_eventCallback(eventCallback),
  m_cookTime(DEFAULT_COOK_TIME),
  m_cookTemp(DEFAULT_COOK_TEMP),
  m_state(STATE_NOT_COOKING),
  m_heaterActive(false),
  m_oneWire(m_tempSensorPin),
  m_sensor(&m_oneWire)
{ 
  // start with heater off
  
}

void SlowCooker::setup(){
  pinMode(m_heaterRelayPin, OUTPUT);
  pinMode(m_lidSwitchPin, INPUT);

  setHeaterState(LOW);
  initTempSensor();
}

// run once in main loop, handles reading pin vals
// setting heater on/off
void SlowCooker::loop(){
  m_lastLidState = m_lidState;
  m_lidState = digitalRead(m_lidSwitchPin);
  m_currentTemp = readTemp(); // float in C

  if(m_state == STATE_NOT_COOKING)
    return;

  // turn heater on/off based on temp.
  if(m_currentTemp >= m_cookTemp){
    if(m_heaterActive)
      Serial.println(F("Turning Heater Off"));
    setHeaterState(false);
  }else if(m_currentTemp <= m_cookTemp-1){
    if(!m_heaterActive)
      Serial.println(F("Turning Heater On"));
    setHeaterState(true);
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
  if (!m_sensor.getAddress(m_sensorAddr, 0)) Serial.println(F("Unable to find address for Device 0"));
    
  m_sensor.setResolution(m_sensorAddr, 9);
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
  m_cookTemp = DEFAULT_COOK_TEMP;
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

bool SlowCooker::setCookTemp(const uint8_t& temp) {
  m_cookTemp = temp;
}

uint16_t SlowCooker::CookTime() const {
  return m_cookTime;
}

uint16_t SlowCooker::CookTimeLeft() const {
  if(m_cookTime == 0)
    return 0;

  return ((m_cookTime * 60) - ((millis() - m_timeCookStarted)/1000))/60;
}

uint8_t SlowCooker::CookTemp() const {
  return m_cookTemp;
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