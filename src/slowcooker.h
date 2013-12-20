#ifndef SLOWCOOKER_H
#define SLOWCOOKER_H

#include <stdint.h>

#define DEFAULT_COOK_TEMP 60
#define DEFAULT_COOK_TIME 0

class SlowCooker {
public:

  enum State {
    STATE_NOT_COOKING, // on but not cooking
    STATE_COOKING // on and cooking
  };

  SlowCooker(const int& heaterRelay, const int& tempSensor, const int& lidSwitch, void (*eventCallback)(int));

  // level warm/low/med/high -> temp setting
  // cook time -> time in minutes to stay on

  // resets cook time to 0:00
  // resets heat to warm level.  
  void resetToDefault();

  // start cooking, at current temp and start counting down time.
  bool startCook();

  // stops cooking, resets counter.
  bool stopCook();

  uint16_t CookTime() const;
  uint16_t CookTimeLeft() const;
  bool setCookTime(const uint16_t& time);

  uint8_t CookTemp() const;
  bool setCookTemp(const uint8_t& temp);

  uint8_t CurrentTemp() const;
  
  bool isLidOpen() const;

  bool isHeaterActive() const;

  // run once in main loop, handles reading pin vals
  // setting heater on/off
  void loop();

protected:

  const int m_heaterRelayPin;
  const int m_tempSensorPin;
  const int m_lidSwitchPin;

  uint16_t m_cookTime;
  uint16_t m_cookTimeLeft;
    
  uint8_t m_cookTemp;
  uint8_t m_currentTemp;

  bool m_lidState;
  bool m_heaterActive;

  State m_state;

  void (*m_eventCallback)(int);

  void setHeaterState(bool state);

  uint8_t readTemp();

};

#endif