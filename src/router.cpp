#include "router.h"

PacketRouter::PacketRouter(SlowCooker& slowcooker) :
  m_slowcooker(slowcooker)
{

}

uint8_t PacketRouter::route(Message& msg,uint8_t* retBuffer){
  
  printMessage(msg);

  switch(msg.type){
    case CMD_PING:
      return handle_ping(msg,retBuffer);
    case CMD_SET_COOK_TIME:
      return handle_setCookTime(msg,retBuffer);
    default:
      return 0;
  };
}

uint8_t PacketRouter::handle_ping(Message& msg,uint8_t* retBuffer){
  retBuffer[0] = 'O';
  retBuffer[1] = 'K';
  return 2;
}

uint8_t PacketRouter::handle_setCookTime(Message& msg,uint8_t* retBuffer){
  if(msg.length-3 != 2)
    return 0;

  m_slowcooker.setCookTime((msg.payload[0] << 8) + msg.payload[1]);

  retBuffer[0] = 'O';
  retBuffer[1] = 'K';
  return 2;  
}

void PacketRouter::printMessage(const Message& msg){
  Serial.print("Message Type:0x");
  Serial.println(msg.type,HEX);

  Serial.print("    Data Len:");
  Serial.println(msg.length);

  for(int i=0;i<msg.length;i++){
    Serial.print("        Data ");
    Serial.print(i);
    Serial.print(":");
    Serial.println(msg.payload[i]);
  }

  Serial.print("         CRC:0x");
  Serial.println(msg.crc,HEX);
}