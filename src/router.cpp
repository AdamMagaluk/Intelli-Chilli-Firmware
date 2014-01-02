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
    case CMD_SET_COOK_TEMP:
      return handle_setCookTemp(msg,retBuffer);
    case CMD_START_COOK:
      return handle_startCook(msg,retBuffer);
    case CMD_STOP_COOK:
      return handle_stopCook(msg,retBuffer);
    case CMD_RESET:
      return handle_reset(msg,retBuffer);
    case CMD_RETURN_STATE:
      return handle_returnState(msg,retBuffer);
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

uint8_t PacketRouter::handle_setCookTemp(Message& msg,uint8_t* retBuffer){
  if(msg.length-3 != 1)
    return 0;

  m_slowcooker.setCookTemp(msg.payload[0]);

  retBuffer[0] = 'O';
  retBuffer[1] = 'K';
  return 2;  
}

uint8_t PacketRouter::handle_startCook(Message& msg,uint8_t* retBuffer){
  m_slowcooker.startCook();

  retBuffer[0] = 'O';
  retBuffer[1] = 'K';
  return 2;  
}

uint8_t PacketRouter::handle_stopCook(Message& msg,uint8_t* retBuffer){
  m_slowcooker.stopCook();

  retBuffer[0] = 'O';
  retBuffer[1] = 'K';
  return 2;  
}

uint8_t PacketRouter::handle_reset(Message& msg,uint8_t* retBuffer){
    
  // for now do it without a wdt.
  asm volatile ("  jmp 0");

  retBuffer[0] = 'O';
  retBuffer[1] = 'K';
  return 2;
}


/*
struct Message_ReturnState {
  uint16_t cookTime;
  uint16_t cookTimeLeft;
  uint8_t cookTemp;
  uint8_t currentTemp;
  uint8_t lidState;
};
*/
uint8_t PacketRouter::handle_returnState(Message& msg,uint8_t* retBuffer){
  uint16_t b = m_slowcooker.CookTime();
  retBuffer[0] = ((b >> (16)) & 0xff); // cook time msb
  retBuffer[1] = ((b >> (8)) & 0xff); // cook time lsb

  b = m_slowcooker.CookTimeLeft();
  retBuffer[2] = ((b >> (16)) & 0xff); // cook timeleft msb
  retBuffer[3] = ((b >> (8)) & 0xff); // cook timeleft lsb

  retBuffer[4] = m_slowcooker.CookTemp(); // cook temp
  retBuffer[5] = m_slowcooker.CurrentTemp(); // current temp

  retBuffer[6] = m_slowcooker.isLidOpen(); // lid state
  retBuffer[7] = m_slowcooker.isCooking(); // lid state
  retBuffer[8] = m_slowcooker.isHeaterActive(); // lid state

  return 9;
}

void PacketRouter::printMessage(const Message& msg){
  Serial.print("Message Type:0x");
  Serial.println(msg.type,HEX);

  Serial.print("    Data Len:");
  Serial.println(msg.length);

  for(int i=0;i<msg.length-3;i++){
    Serial.print("        Data ");
    Serial.print(i);
    Serial.print(":");
    Serial.println(msg.payload[i]);
  }

  Serial.print("         CRC:0x");
  Serial.println(msg.crc,HEX);
}