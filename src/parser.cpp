#include "parser.h"

/**
  
  */
bool command_valid(const uint8_t& type){
  switch(type){
    case CMD_ACK:
    case CMD_PING:
    case CMD_SET_COOK_DELAY:
    case CMD_SET_COOK_TIME:
    case CMD_SET_COOK_TEMP:
    case CMD_START_COOk:
    case CMD_TURN_OFF:
    case CMD_TURN_ON:
    case CMD_RESET:
    case CMD_RETURN_STATE:
    case CMD_EVENT:
      return true;
    default:
      return false;
  };
}

bool parse_packet(uint8_t* payload, uint8_t len, Message* msg){
  if(len < 3)
    return false;

  if(!command_valid(payload[0]))
    return false;

  const uint8_t payloadLen = payload[1];

  if(len < payloadLen+3)
    return false;

  if(!check_crc(payload,payloadLen+3))
    return false;

  msg->type = (CommandType)payload[0];
  msg->length = payload[1];
  msg->payload = &payload[2];
  msg->crc = payload[payloadLen+2];
  
  return true;
}

bool check_crc(uint8_t* payload, uint8_t len){
  // @todo
  return true;
}