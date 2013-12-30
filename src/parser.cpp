#include "parser.h"

/**
  
  */
bool command_valid(const uint8_t& type){
  switch(type){
    case CMD_ACK:
    case CMD_PING:
    case CMD_SET_COOK_TIME:
    case CMD_SET_COOK_TEMP:
    case CMD_START_COOK:
    case CMD_STOP_COOK:
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

  const uint8_t packetLen = payload[0];

  if(len < packetLen)
    return false;

  if(!command_valid(payload[1]))
    return false;

  msg->length = payload[0];
  msg->type = (CommandType)payload[1];
  msg->payload = &payload[2];
  msg->crc = payload[packetLen-1];
  
  return true;
}

bool check_crc(uint8_t* payload, uint8_t len){
  // @todo
  return true;
}