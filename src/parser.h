#ifndef PARSER_H
#define PARSER_H

#include "protocol.h"

bool parse_packet(uint8_t* payload,Message* msg);

#endif