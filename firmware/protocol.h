#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

enum CommandType {
  CMD_ACK,
  CMD_PING,
  CMD_SET_COOK_TIME,
  CMD_SET_COOK_TEMP,
  CMD_START_COOK,
  CMD_STOP_COOK,
  CMD_RESET,
  CMD_RETURN_STATE,
  CMD_EVENT
};

enum StatusEvent {
  EVENT_POWERED_ON,
  EVENT_COOK_STARTED,
  EVENT_COOK_END,
  EVENT_LID_OPENED,
  EVENT_LID_CLOSED
};

enum LidState {
  LID_STATE_CLOSED,
  LID_STATE_OPPENNED
};

#pragma pack(push, 1)

struct Message {
  uint8_t length;
  CommandType type;
  uint8_t* payload;
  uint8_t crc;
};

struct Message_Ping {

};

struct Message_SetCookTime {
  uint16_t time;
};

struct Message_SetCookTemp {
  uint8_t temp;
};

struct Message_StartCook {

};

struct Message_TurnOff {
  
};

struct Message_TurnOn {
  
};

struct Message_Reset {
  
};

struct Message_ReturnState {
  uint16_t cookTime;
  uint16_t cookTimeLeft;
  uint8_t cookTemp;
  uint8_t currentTemp;
  uint8_t lidState;
};

#pragma pack(pop)

#endif
