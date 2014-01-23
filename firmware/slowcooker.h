#include <OneWire.h>
#include <DallasTemperature.h>

#ifndef SLOWCOOKER_H
#define SLOWCOOKER_H

#include <stdint.h>
#include "protocol.h"

#define DEFAULT_COOK_TEMP 27
#define DEFAULT_COOK_TIME 0

#define COOK_INTERVAL 600000

class SlowCooker {
public:

  enum State {
    STATE_NOT_COOKING, // on but not cooking
    STATE_COOKING // on and cooking
  };
  
  enum TempSetting {
    TEMP_WARM = 1,
    TEMP_LOW = 3,
    TEMP_MED = 5,
    TEMP_HIGH = 8
  };

  SlowCooker(const int& heaterRelay, const int& tempSensor, const int& lidSwitch, void (*eventCallback)(StatusEvent));

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

  TempSetting CookTemp() const;
  bool setCookTemp(const TempSetting& temp);
  
  static char* cookTempToText(const TempSetting& temp);
  
  static bool validCookTemp(int temp);

  uint8_t CurrentTemp() const;
  
  bool isLidOpen() const;

  bool isHeaterActive() const;

  bool isCooking();
  
  bool tempSensorFound() const;

  // run once in main loop, handles reading pin vals
  // setting heater on/off
  void loop();

  void setup();

protected:

  bool m_tempFound;

  const int m_heaterRelayPin;
  const int m_tempSensorPin;
  const int m_lidSwitchPin;

  unsigned long m_timeCookStarted;
  uint16_t m_cookTime;
    
  float m_currentTemp;
  
  TempSetting m_tempSetting;

  bool m_lidState;
  bool m_lastLidState;

  bool m_heaterActive;

  State m_state;

  // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
  OneWire m_oneWire;
  
  // Pass our oneWire reference to Dallas Temperature. 
  DallasTemperature m_sensor;

  DeviceAddress m_sensorAddr;
  
  // time the heater was last turned on.
  unsigned long m_heaterOnTime;
  bool m_isWarming;
  
  // calculates if heater should be active.  
  bool calcHeaterState();

  void (*m_eventCallback)(StatusEvent);

  void setHeaterState(bool state);

  void initTempSensor();

  float readTemp();

};

#endif
