#include <iostream>

#include "parser.h"
#include "protocol.h"

uint8_t data[] = {0x1,0x2,0x3,0x4,0x5};

void show_message(const Message& msg){
  printf("Message Type:0x%x\n",msg.type);
  printf("    Data Len:%d\n",msg.length);
  for(int i=0;i<msg.length;i++){
    printf("        Data %d:0x%x\n",i,msg.payload[i]);
  }
  printf("         CRC:0x%x\n",msg.crc);
}
/*
uint16_t delayTime;
  uint16_t delayTimeLeft;
  uint16_t cookTime;
  uint16_t cookTimeLeft;
  uint8_t cookTemp;
  uint8_t currentTemp;
  uint8_t lidState;
  */
void handle_StateRequest(const Message& msg){

  Message_ReturnState state;
  state.delayTime = 1;
  state.delayTimeLeft = 2;
  state.cookTime = 3;
  state.cookTimeLeft = 4;
  state.cookTemp = 5;
  state.currentTemp = 6;
  state.lidState = 0;



  uint8_t data[ (sizeof(Message_ReturnState)) + 3];

//  Message_ReturnState* pT;
//  pT = (Message_ReturnState*)buf;
//  Message_ReturnState msg2 = *pT;

  char buf[sizeof(Message_ReturnState)];
  sprintf(buf,"%s",(char*)&state);

  for(int i=0;i<sizeof(buf);i++){
    printf("%d - 0x%x\n",i,buf[i]);
  }

  //printf("Side %d\n",sizeof(Message_SetCookTemp));
  //printf("Side %d\n",sizeof(Message_ReturnState));
  //printf("Side %d\n",sizeof(Message));

}


Message msg;
int main(){
  uint8_t* buf = data;
  
  if(parse_packet(data,sizeof(data),&msg)){
    std::cout << "parse success\n";
    show_message(msg);
    handle_StateRequest(msg);
  }else{
    std::cout << "Failed to parse\n";
  }

}