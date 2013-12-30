#ifndef ROUTER_H
#define ROUTER_H

#include "protocol.h"
#include "slowcooker.h"

class PacketRouter {
public:

  PacketRouter(SlowCooker& slowcooker);

  uint8_t route(Message& msg,uint8_t* retBuffer);

protected:
  SlowCooker& m_slowcooker;

  void printMessage(const Message& msg);


  uint8_t handle_ping(Message& msg,uint8_t* retBuffer);
  uint8_t handle_setCookTime(Message& msg,uint8_t* retBuffer);
  uint8_t handle_setCookTemp(Message& msg,uint8_t* retBuffer);
  uint8_t handle_startCook(Message& msg,uint8_t* retBuffer);
  uint8_t handle_stopCook(Message& msg,uint8_t* retBuffer);
  uint8_t handle_reset(Message& msg,uint8_t* retBuffer);
  uint8_t handle_returnState(Message& msg,uint8_t* retBuffer);

};

#endif