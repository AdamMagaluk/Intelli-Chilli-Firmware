#include <Arduino.h>

#include "slowcooker.h"

SlowCooker::SlowCooker(const int& heaterRelay, const int& tempSensor, const int& lidSwitch, void (*eventCallback)(int)) :
  m_heaterRelayPin(heaterRelay),
  m_tempSensorPin(tempSensor),
  m_lidSwitchPin(lidSwitch),
  m_eventCallback(eventCallback),
  m_cookTime(DEFAULT_COOK_TIME),
  m_cookTemp(DEFAULT_COOK_TEMP),
  m_state(STATE_NOT_COOKING),
  m_heaterActive(false)
{ 
  // start with heater off
  setHeaterState(LOW);
}

uint8_t SlowCooker::readTemp(){
  // analog read 
  uint8_t val = analogRead(m_tempSensorPin);
  return val;
}

// run once in main loop, handles reading pin vals
// setting heater on/off
void SlowCooker::loop(){
  m_lidState = digitalRead(m_lidSwitchPin);
  m_currentTemp = readTemp();

  if(m_state == STATE_NOT_COOKING){
    
  }
}

// resets cook time to 0:00
// resets heat to warm level.  
void SlowCooker::resetToDefault() {
  m_cookTime = DEFAULT_COOK_TIME;
  m_cookTemp = DEFAULT_COOK_TEMP;
}

// start cooking, at current temp and start counting down time.
bool SlowCooker::startCook() {
  m_state = STATE_COOKING;
}

// stops cooking, resets counter.
bool SlowCooker::stopCook() {
  m_state = STATE_NOT_COOKING;
  resetToDefault();
  setHeaterState(LOW);
}

bool SlowCooker::setCookTime(const uint16_t& time) {
  m_cookTime = time;
}

bool SlowCooker::setCookTemp(const uint8_t& temp) {
  m_cookTemp = temp;
}

void SlowCooker::setHeaterState(bool state) {
  m_heaterActive = state;
  // set heater pin
  digitalWrite(m_heaterRelayPin, state);
}

uint16_t SlowCooker::CookTime() const {
  return m_cookTime;
}

uint16_t SlowCooker::CookTimeLeft() const {
  // 
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